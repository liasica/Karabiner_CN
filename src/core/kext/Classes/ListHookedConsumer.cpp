#include "diagnostic_macros.hpp"

#define protected public // A hack for access private member of IOHIKeyboard
#define private public
BEGIN_IOKIT_INCLUDE;
#include <IOKit/hidsystem/IOHIKeyboard.h>
END_IOKIT_INCLUDE;
#undef protected
#undef private

#include "CommonData.hpp"
#include "Config.hpp"
#include "EventInputQueue.hpp"
#include "FlagStatus.hpp"
#include "GlobalLock.hpp"
#include "IOLogWrapper.hpp"
#include "ListHookedConsumer.hpp"
#include "Params.hpp"

namespace org_pqrs_Karabiner {
namespace {
ListHookedConsumer listHookedConsumer;
}

ListHookedConsumer&
ListHookedConsumer::instance(void) {
  return listHookedConsumer;
}

ListHookedConsumer::Item::Item(IOHIDevice* p) : ListHookedDevice::Item(p),
                                                orig_keyboardSpecialEventAction_(nullptr),
                                                orig_keyboardSpecialEventTarget_(nullptr) {}

ListHookedConsumer::Item::~Item(void) {
  IOLOG_DEBUG("ListHookedConsumer::Item::~Item()\n");
  restoreEventAction();
}

// ----------------------------------------------------------------------
bool ListHookedConsumer::Item::refresh(void) {
  if (!device_) goto restore;

  {
    const char* name = device_->getName();
    if (!name) goto restore;

    if (!ListHookedDevice::Item::isConsumer(name)) goto restore;
  }

  // ------------------------------------------------------------
  if (!Config::get_initialized()) {
    goto restore;
  }
  if (!Config::get_essential_config(BRIDGE_ESSENTIAL_CONFIG_INDEX_notsave_automatically_enable_keyboard_device)) {
    goto restore;
  }
  if (Config::get_essential_config(BRIDGE_ESSENTIAL_CONFIG_INDEX_general_dont_remap_thirdvendor_keyboard) &&
      deviceType_ != DeviceType::APPLE_INTERNAL &&
      deviceType_ != DeviceType::APPLE_EXTERNAL) {
    goto restore;
  }
  if (Config::get_essential_config(BRIDGE_ESSENTIAL_CONFIG_INDEX_general_dont_remap_internal) &&
      deviceType_ == DeviceType::APPLE_INTERNAL) {
    goto restore;
  }
  if (Config::get_essential_config(BRIDGE_ESSENTIAL_CONFIG_INDEX_general_dont_remap_external) &&
      deviceType_ != DeviceType::APPLE_INTERNAL) {
    goto restore;
  }
  if (Config::get_essential_config(BRIDGE_ESSENTIAL_CONFIG_INDEX_general_dont_remap_apple_keyboard) &&
      getDeviceIdentifier().isApple()) {
    goto restore;
  }
  if (Config::get_essential_config(BRIDGE_ESSENTIAL_CONFIG_INDEX_general_dont_remap_non_apple_keyboard) &&
      !getDeviceIdentifier().isApple()) {
    goto restore;
  }

  // Logitech USB Headset
  if (getDeviceIdentifier().isEqualVendorProduct(DeviceVendor::LOGITECH, DeviceProduct::LOGITECH_USB_HEADSET)) {
    goto restore;
  }
  // Kensington Virtual Device (0x0, 0x0)
  if (getDeviceIdentifier().isEqualVendorProduct(DeviceVendor::PSEUDO, DeviceProduct::PSEUDO)) {
    // Note: USB Overdrive also use 0x0,0x0.
    // We allow to use USB Overdrive.
    if (deviceType_ != DeviceType::USB_OVERDRIVE) {
      goto restore;
    }
  }

  // ------------------------------------------------------------
  return replaceEventAction();

restore:
  return restoreEventAction();
}

bool ListHookedConsumer::Item::replaceEventAction(void) {
  if (!device_) return false;

  IOHIKeyboard* kbd = OSDynamicCast(IOHIKeyboard, device_);
  if (!kbd) return false;

  bool result = false;

  // ------------------------------------------------------------
  {
    // We need to replace _keyboardSpecialEventAction until it points Karabiner's callback function.
    // (A reason is described at ListHookedKeyboard::replaceEventAction.)
    KeyboardSpecialEventCallback callback = reinterpret_cast<KeyboardSpecialEventCallback>(kbd->_keyboardSpecialEventAction);
    if (!callback) {
      IOLOG_DEBUG("HookedConsumer::replaceEventAction device_:%p callback is null.\n", device_);
      inProgress_ = true;

    } else if (callback != EventInputQueue::push_KeyboardSpecialEventCallback) {
      inProgress_ = false;

      IOLOG_DEBUG("HookedConsumer::replaceEventAction device_:%p (%p -> %p)\n",
                  device_, callback, EventInputQueue::push_KeyboardSpecialEventCallback);

      orig_keyboardSpecialEventAction_ = callback;
      orig_keyboardSpecialEventTarget_ = kbd->_keyboardSpecialEventTarget;

      kbd->_keyboardSpecialEventAction = reinterpret_cast<KeyboardSpecialEventAction>(EventInputQueue::push_KeyboardSpecialEventCallback);

      result = true;
    }
  }

  return result;
}

bool ListHookedConsumer::Item::restoreEventAction(void) {
  if (!device_) return false;

  IOHIKeyboard* kbd = OSDynamicCast(IOHIKeyboard, device_);
  if (!kbd) return false;

  bool result = false;

  {
    KeyboardSpecialEventCallback callback = reinterpret_cast<KeyboardSpecialEventCallback>(kbd->_keyboardSpecialEventAction);
    if (callback == EventInputQueue::push_KeyboardSpecialEventCallback) {
      IOLOG_DEBUG("HookedConsumer::restoreEventAction device_:%p (%p -> %p)\n",
                  device_, callback, orig_keyboardSpecialEventAction_);

      kbd->_keyboardSpecialEventAction = reinterpret_cast<KeyboardSpecialEventAction>(orig_keyboardSpecialEventAction_);

      result = true;
    }
  }

  orig_keyboardSpecialEventAction_ = nullptr;
  orig_keyboardSpecialEventTarget_ = nullptr;

  return result;
}

// ======================================================================
void ListHookedConsumer::Item::apply(const Params_KeyboardSpecialEventCallback& params) {
  if (params.key >= ConsumerKeyCode::VK__BEGIN__) {
    // Invalid keycode
    IOLOG_ERROR("%s invalid key:%d\n", __PRETTY_FUNCTION__, params.key.get());
    return;
  }

  // ------------------------------------------------------------
  KeyboardSpecialEventCallback callback = orig_keyboardSpecialEventAction_;
  if (!callback) return;

  OSObject* target = orig_keyboardSpecialEventTarget_;
  if (!target) return;

  OSObject* sender = OSDynamicCast(OSObject, device_);
  if (!sender) return;

  const AbsoluteTime& ts = CommonData::getcurrent_ts();
  OSObject* refcon = nullptr;

  Params_KeyboardSpecialEventCallback::log(false, params.eventType, params.flags, params.key,
                                           params.flavor, params.guid, params.repeat);
  {
    // We need to unlock the global lock while we are calling the callback function.
    // For more information, See ListHookedKeyboard::Item::apply(const Params_KeyboardEventCallBack& params)
    GlobalLock::ScopedUnlock lk;
    callback(target, params.eventType.get(), params.flags.get(), params.key.get(),
             params.flavor, params.guid, params.repeat, ts, sender, refcon);
  }
}

void ListHookedConsumer::Item::disableNumLock(void) {
  IOHIKeyboard* kbd = OSDynamicCast(IOHIKeyboard, device_);
  if (!kbd) return;

  GlobalLock::ScopedUnlock lk;
  if (kbd->numLock()) {
    kbd->setNumLock(false);
  }
}

bool ListHookedConsumer::apply(const Params_KeyboardSpecialEventCallback& params) {
  ListHookedConsumer::Item* p = static_cast<ListHookedConsumer::Item*>(get_replaced());
  if (p) {
    p->apply(params);
    return true;
  }
  return false;
}

void ListHookedConsumer::disableNumLock(void) {
  ListHookedConsumer::Item* p = static_cast<ListHookedConsumer::Item*>(get_replaced());
  if (p) {
    p->disableNumLock();
  }
}
}
