#include "diagnostic_macros.hpp"

BEGIN_IOKIT_INCLUDE;
#include <IOKit/IOLib.h>
END_IOKIT_INCLUDE;

#include "EventOutputQueue.hpp"
#include "FlagStatus.hpp"
#include "UserClient_kext.hpp"
#include "VK_CONFIG.hpp"

namespace org_pqrs_Karabiner {
VirtualKey::VK_CONFIG::Vector_Item VirtualKey::VK_CONFIG::items_;

void VirtualKey::VK_CONFIG::initialize(void) {}

void VirtualKey::VK_CONFIG::terminate(void) {
  items_.clear();
}

void VirtualKey::VK_CONFIG::add_item(RemapClass* remapclass,
                                     unsigned int keycode_toggle,
                                     unsigned int keycode_force_on,
                                     unsigned int keycode_force_off,
                                     unsigned int keycode_sync_keydownup) {
  items_.push_back(Item(remapclass, keycode_toggle, keycode_force_on, keycode_force_off, keycode_sync_keydownup));
}

void VirtualKey::VK_CONFIG::clear_items(void) {
  items_.clear();
}

bool VirtualKey::VK_CONFIG::handle(const Params_KeyboardEventCallBack& params, AutogenId autogenId, PhysicalEventType physicalEventType) {
  RemapClass* remapclass = nullptr;
  bool value_old = false;

  for (size_t i = 0; i < items_.size(); ++i) {
    remapclass = items_[i].remapclass;
    KeyCode keycode_toggle(items_[i].keycode_toggle);
    KeyCode keycode_force_on(items_[i].keycode_force_on);
    KeyCode keycode_force_off(items_[i].keycode_force_off);
    KeyCode keycode_sync_keydownup(items_[i].keycode_sync_keydownup);

    if (!remapclass) return false;

    value_old = remapclass->enabled();

    if (params.key == keycode_toggle) {
      if (params.repeat) goto finish;

      if (params.ex_iskeydown) {
        remapclass->toggleEnabled();
        goto refresh;
      }
      goto finish;

    } else if (params.key == keycode_force_on) {
      if (params.repeat) goto finish;

      if (params.ex_iskeydown) {
        remapclass->setEnabled(true);
        goto refresh;
      }
      goto finish;

    } else if (params.key == keycode_force_off) {
      if (params.repeat) goto finish;

      if (params.ex_iskeydown) {
        remapclass->setEnabled(false);
        goto refresh;
      }
      goto finish;

    } else if (params.key == keycode_sync_keydownup) {
      if (params.repeat) goto finish;

      if (params.ex_iskeydown) {
        remapclass->setEnabled(true);
      } else {
        remapclass->setEnabled(false);
      }
      EventOutputQueue::FireModifiers::fire(autogenId, physicalEventType);
      goto refresh;
    }
  }

  return false;

refresh:
  if (remapclass) {
    bool value_new = remapclass->enabled();

    // * send_notification_to_userspace takes a time.
    // * VK_CONFIG_FORCE_OFF_* might not change remapclass state.
    // Therefore, we call send_notification_to_userspace only if needed.

    if (value_old != value_new) {
      RemapClassManager::refresh();

      // Tell remapclass status is changed to userspace.
      org_pqrs_driver_Karabiner_UserClient_kext::send_notification_to_userspace(BRIDGE_USERCLIENT_NOTIFICATION_TYPE_CONFIG_ENABLED_UPDATED, remapclass->get_configindex());
    }
  }

finish:
  return true;
}

bool VirtualKey::VK_CONFIG::is_VK_CONFIG_SYNC_KEYDOWNUP(KeyCode keycode) {
  for (size_t i = 0; i < items_.size(); ++i) {
    KeyCode keycode_sync_keydownup(items_[i].keycode_sync_keydownup);
    if (keycode == keycode_sync_keydownup) return true;
  }

  return false;
}
}
