#include "CommonData.hpp"
#include "FlagStatus.hpp"
#include "IOLogWrapper.hpp"
#include "KeyCodeModifierFlagPairs.hpp"
#include "ModifierName.hpp"

namespace org_pqrs_Karabiner {
FlagStatus globalFlagStatus_;

FlagStatus&
FlagStatus::globalFlagStatus(void) {
  return globalFlagStatus_;
}

void FlagStatus::Item::initialize(ModifierFlag f) {
  flag_ = f;
  count_ = 0;
  temporary_count_ = 0;
  lock_count_ = 0;
  negative_lock_count_ = 0;
  sticky_count_ = 0;
}

void FlagStatus::Item::set(void) {
  temporary_count_ = 0;
}

void FlagStatus::Item::set(KeyCode key, Flags flags) {
  temporary_count_ = 0;

  // ------------------------------------------------------------
  if (key != flag_.getKeyCode()) return;

  // ------------------------------------------------------------
  if (flag_ == ModifierFlag::CAPSLOCK) {
    if (flags.isOn(flag_)) {
      lock_count_ = 1;
    } else {
      lock_count_ = 0;
    }

  } else {
    if (flags.isOn(flag_)) {
      increase();
    } else {
      decrease();
    }
  }
}

void FlagStatus::Item::reset(void) {
  count_ = 0;
  temporary_count_ = 0;
  lazy_count_ = 0;
  lazy_enabled_ = false;

  /*
      preserve lock_count, negative_lock_count_ and sticky_count_.

      FlagStatus::reset is called when PressingPhysicalKeys is empty,
      so we need remember the status of CapsLock.
    */
}

void FlagStatus::Item::increase(void) {
  if (flag_ == ModifierFlag::CAPSLOCK) {
    lock_count_ = !lock_count_;
  } else {
    ++count_;
  }
}

void FlagStatus::Item::decrease(void) {
  if (flag_ == ModifierFlag::CAPSLOCK) {
    // do nothing (toggle at Item::increase).
  } else {
    --count_;
  }
}

// ----------------------------------------------------------------------
void FlagStatus::initialize(void) {
  item_.clear();

  auto& pairs = KeyCodeModifierFlagPairs::getPairs();
  for (size_t i = 0; i < pairs.size(); ++i) {
    item_.push_back(Item());
    item_.back().initialize(pairs[i].getModifierFlag());
  }
}

FlagStatus::FlagStatus(void) {
  initialize();
}

FlagStatus::FlagStatus(Flags flags) {
  initialize();

  for (size_t i = 0; i < item_.size(); ++i) {
    if (flags.isOn(item_[i].flag_)) {
      item_[i].increase();
    }
  }
}

void FlagStatus::set(void) {
  for (size_t i = 0; i < item_.size(); ++i) {
    item_[i].set();
  }
  updateStatusMessage();
}

void FlagStatus::set(KeyCode key, Flags flags) {
  for (size_t i = 0; i < item_.size(); ++i) {
    item_[i].set(key, flags);
  }
  updateStatusMessage();
}

void FlagStatus::reset(void) {
  for (size_t i = 0; i < item_.size(); ++i) {
    item_[i].reset();
  }
  updateStatusMessage();
}

bool FlagStatus::isOn(ModifierFlag modifierFlag) const {
  for (size_t i = 0; i < item_.size(); ++i) {
    if (item_[i].flag_ == modifierFlag) {
      return item_[i].sum(true) > 0;
    }
  }
  return false;
}

bool FlagStatus::isOn(const Vector_ModifierFlag& modifierFlags) const {
  bool strict = false;

  for (size_t i = 0; i < modifierFlags.size(); ++i) {
    if (modifierFlags[i] == ModifierFlag::ZERO) continue;

    if (modifierFlags[i] == ModifierFlag::NONE) {
      strict = true;
    } else {
      if (!isOn(modifierFlags[i])) return false;
    }
  }

  // If modifierFlags contains ModifierFlag::NONE,
  // return false when unspecified modifierflag is pressed.
  if (strict) {
    for (size_t i = 0; i < item_.size(); ++i) {
      if (item_[i].sum(true) > 0 &&
          !modifierFlags.is_include(item_[i].flag_)) {
        return false;
      }
    }
  }

  return true;
}

bool FlagStatus::isLocked(const Vector_ModifierFlag& modifierFlags) const {
  for (size_t i = 0; i < item_.size(); ++i) {
    if (item_[i].flag_ == ModifierFlag::ZERO) continue;
    if (item_[i].flag_ == ModifierFlag::NONE) continue;

    if (modifierFlags.is_include(item_[i].flag_) &&
        item_[i].lock_count_ <= 0) {
      return false;
    }
  }
  return true;
}

bool FlagStatus::isStuck(const Vector_ModifierFlag& modifierFlags) const {
  for (size_t i = 0; i < item_.size(); ++i) {
    if (item_[i].flag_ == ModifierFlag::ZERO) continue;
    if (item_[i].flag_ == ModifierFlag::NONE) continue;

    if (modifierFlags.is_include(item_[i].flag_) &&
        item_[i].sticky_count_ <= 0) {
      return false;
    }
  }
  return true;
}

Flags FlagStatus::makeFlags(void) const {
  Flags flags;
  for (size_t i = 0; i < item_.size(); ++i) {
    if (item_[i].sum(false) > 0) {
      flags.add(item_[i].flag_);
    }
  }
  return flags;
}

ModifierFlag
FlagStatus::getFlag(size_t index) const {
  for (size_t i = 0; i < item_.size(); ++i) {
    if (i == index) {
      return item_[i].flag_;
    }
  }
  return ModifierFlag::ZERO;
}

// ------------------------------------------------------------
#define DEFINE_METHODS(METHOD)                                                                   \
  void FlagStatus::METHOD(ModifierFlag modifierFlag) {                                           \
    for (size_t i = 0; i < item_.size(); ++i) {                                                  \
      if (modifierFlag == item_[i].flag_) {                                                      \
        item_[i].METHOD();                                                                       \
        if (item_[i].flag_ == ModifierFlag::CAPSLOCK) {                                          \
          updateStatusMessage();                                                                 \
        }                                                                                        \
      }                                                                                          \
    }                                                                                            \
  }                                                                                              \
  void FlagStatus::METHOD(ModifierFlag modifierFlag, const Vector_ModifierFlag& modifierFlags) { \
    for (size_t i = 0; i < item_.size(); ++i) {                                                  \
      if (modifierFlag == item_[i].flag_ || modifierFlags.is_include(item_[i].flag_)) {          \
        item_[i].METHOD();                                                                       \
        if (item_[i].flag_ == ModifierFlag::CAPSLOCK) {                                          \
          updateStatusMessage();                                                                 \
        }                                                                                        \
      }                                                                                          \
    }                                                                                            \
  }                                                                                              \
  void FlagStatus::METHOD(const Vector_ModifierFlag& modifierFlags) {                            \
    for (size_t i = 0; i < item_.size(); ++i) {                                                  \
      if (modifierFlags.is_include(item_[i].flag_)) {                                            \
        item_[i].METHOD();                                                                       \
        if (item_[i].flag_ == ModifierFlag::CAPSLOCK) {                                          \
          updateStatusMessage();                                                                 \
        }                                                                                        \
      }                                                                                          \
    }                                                                                            \
  }

DEFINE_METHODS(increase)
DEFINE_METHODS(decrease)
DEFINE_METHODS(temporary_increase)
DEFINE_METHODS(temporary_decrease)
DEFINE_METHODS(lazy_increase)
DEFINE_METHODS(lazy_decrease)
#undef DEFINE_METHODS

#define DEFINE_METHODS(METHOD)                                                                   \
  void FlagStatus::METHOD(ModifierFlag modifierFlag) {                                           \
    for (size_t i = 0; i < item_.size(); ++i) {                                                  \
      if (modifierFlag == item_[i].flag_) {                                                      \
        item_[i].METHOD();                                                                       \
        updateStatusMessage();                                                                   \
      }                                                                                          \
    }                                                                                            \
  }                                                                                              \
  void FlagStatus::METHOD(ModifierFlag modifierFlag, const Vector_ModifierFlag& modifierFlags) { \
    for (size_t i = 0; i < item_.size(); ++i) {                                                  \
      if (modifierFlag == item_[i].flag_ || modifierFlags.is_include(item_[i].flag_)) {          \
        item_[i].METHOD();                                                                       \
        updateStatusMessage();                                                                   \
      }                                                                                          \
    }                                                                                            \
  }                                                                                              \
  void FlagStatus::METHOD(const Vector_ModifierFlag& modifierFlags) {                            \
    for (size_t i = 0; i < item_.size(); ++i) {                                                  \
      if (modifierFlags.is_include(item_[i].flag_)) {                                            \
        item_[i].METHOD();                                                                       \
        updateStatusMessage();                                                                   \
      }                                                                                          \
    }                                                                                            \
  }

DEFINE_METHODS(lock_increase)
DEFINE_METHODS(lock_decrease)
DEFINE_METHODS(lock_toggle)
DEFINE_METHODS(negative_lock_increase)
DEFINE_METHODS(negative_lock_decrease)
DEFINE_METHODS(negative_lock_toggle)
DEFINE_METHODS(sticky_increase)
DEFINE_METHODS(sticky_decrease)
DEFINE_METHODS(sticky_toggle)
#undef DEFINE_METHODS

#define STICKY_ACTIVE_MODIFIERS_METHOD(METHOD)  \
  {                                             \
    for (size_t i = 0; i < item_.size(); ++i) { \
      if (item_[i].sum(true) > 0) {             \
        item_[i].METHOD();                      \
      }                                         \
    }                                           \
    updateStatusMessage();                      \
  }

void FlagStatus::sticky_active_modifiers_toggle(void) { STICKY_ACTIVE_MODIFIERS_METHOD(sticky_toggle); }
void FlagStatus::sticky_active_modifiers_increase(void) { STICKY_ACTIVE_MODIFIERS_METHOD(sticky_increase); }
void FlagStatus::sticky_active_modifiers_decrease(void) { STICKY_ACTIVE_MODIFIERS_METHOD(sticky_decrease); }
#undef STICKY_ACTIVE_MODIFIERS_METHOD

void FlagStatus::sticky_clear(void) {
  for (size_t i = 0; i < item_.size(); ++i) {
    item_[i].sticky_decrease();
  }
  updateStatusMessage();
}

void FlagStatus::lock_clear(void) {
  for (size_t i = 0; i < item_.size(); ++i) {
    if (item_[i].lock_count_) {
      item_[i].lock_decrease();
    }
  }
  updateStatusMessage();
}

void FlagStatus::negative_lock_clear(void) {
  for (size_t i = 0; i < item_.size(); ++i) {
    if (item_[i].negative_lock_count_) {
      item_[i].negative_lock_decrease();
    }
  }
  updateStatusMessage();
}

void FlagStatus::lazy_enable(void) {
  for (size_t i = 0; i < item_.size(); ++i) {
    item_[i].lazy_enable();
  }
}

void FlagStatus::lazy_disable_if_off(void) {
  for (size_t i = 0; i < item_.size(); ++i) {
    item_[i].lazy_disable_if_off();
  }
}

void FlagStatus::subtract(const FlagStatus& other, Vector_ModifierFlag& modifierFlags) const {
  modifierFlags.clear();

  if (item_.size() == other.item_.size()) {
    for (size_t i = 0; i < item_.size(); ++i) {
      int sum1 = item_[i].sum(true);
      int sum2 = other.item_[i].sum(true);
      if (sum1 > sum2) {
        for (int j = 0; j < sum1 - sum2; ++j) {
          modifierFlags.push_back(item_[i].flag_);
        }
      }
    }
  }
}

void FlagStatus::log(void) const {
  for (size_t i = 0; i < item_.size(); ++i) {
    int sum = item_[i].sum(false);
    if (sum != 0) {
      const char* name = ModifierName::getName(item_[i].flag_);
      if (name) {
        IOLOG_DEVEL("FlagStatus %s: %d\n", name, sum);
      }
    }
  }
}

void FlagStatus::updateStatusMessage(unsigned int statusMessageIndex) {
  CommonData::clear_statusmessage(statusMessageIndex);

  for (size_t i = 0; i < item_.size(); ++i) {
    const char* name = ModifierName::getName(item_[i].flag_);
    if (name) {
      switch (statusMessageIndex) {
      case BRIDGE_USERCLIENT_STATUS_MESSAGE_MODIFIER_LOCK:
        // Skip caps lock.
        if (item_[i].flag_ != ModifierFlag::CAPSLOCK) {
          if (item_[i].negative_lock_count_ > 0) {
            CommonData::append_statusmessage(statusMessageIndex, "-");
            CommonData::append_statusmessage(statusMessageIndex, name);
            CommonData::append_statusmessage(statusMessageIndex, " ");
          } else if (item_[i].lock_count_ > 0) {
            CommonData::append_statusmessage(statusMessageIndex, name);
            CommonData::append_statusmessage(statusMessageIndex, " ");
          }
        }
        break;

      case BRIDGE_USERCLIENT_STATUS_MESSAGE_MODIFIER_CAPS_LOCK:
        if (item_[i].flag_ == ModifierFlag::CAPSLOCK) {
          if (item_[i].lock_count_ > 0) {
            CommonData::append_statusmessage(statusMessageIndex, name);
            CommonData::append_statusmessage(statusMessageIndex, " ");
          }
        }
        break;

      case BRIDGE_USERCLIENT_STATUS_MESSAGE_MODIFIER_STICKY:
        if (item_[i].sticky_count_ > 0) {
          CommonData::append_statusmessage(statusMessageIndex, name);
          CommonData::append_statusmessage(statusMessageIndex, " ");
        }
        break;
      }
    }
  }
}

void FlagStatus::updateStatusMessage(void) {
  int indexes[] = {
      BRIDGE_USERCLIENT_STATUS_MESSAGE_MODIFIER_LOCK,
      BRIDGE_USERCLIENT_STATUS_MESSAGE_MODIFIER_CAPS_LOCK,
      BRIDGE_USERCLIENT_STATUS_MESSAGE_MODIFIER_STICKY,
  };
  for (size_t i = 0; i < sizeof(indexes) / sizeof(indexes[0]); ++i) {
    int idx = indexes[i];

    static char previousMessage[BRIDGE_USERCLIENT_STATUS_MESSAGE_MAXLEN];
    strlcat(previousMessage, CommonData::get_statusmessage(idx), sizeof(previousMessage));

    updateStatusMessage(idx);

    if (strcmp(CommonData::get_statusmessage(idx), previousMessage) != 0) {
      CommonData::send_notification_statusmessage(idx);
    }
  }
}
}
