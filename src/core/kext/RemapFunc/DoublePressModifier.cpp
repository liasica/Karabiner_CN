#include "diagnostic_macros.hpp"

BEGIN_IOKIT_INCLUDE;
#include <IOKit/IOLib.h>
END_IOKIT_INCLUDE;

#include "Config.hpp"
#include "DoublePressModifier.hpp"
#include "IOLogWrapper.hpp"

namespace org_pqrs_Karabiner {
namespace RemapFunc {
void DoublePressModifier::add(AddDataType datatype, AddValue newval) {
  switch (datatype) {
  case BRIDGE_DATATYPE_KEYCODE:
  case BRIDGE_DATATYPE_CONSUMERKEYCODE:
  case BRIDGE_DATATYPE_POINTINGBUTTON: {
    switch (index_) {
    case 0:
      keytokey_.add(datatype, newval);
      keytokey_fire_.add(KeyCode::VK_PSEUDO_KEY);
      break;

    default:
      if (!isUseSeparator_ && index_ >= 2) {
        index_is_double_pressed_ = true;
      }

      if (!index_is_double_pressed_) {
        keytokey_.add(datatype, newval);
      } else {
        keytokey_fire_.add(datatype, newval);
      }
      break;
    }
    ++index_;

    break;
  }

  case BRIDGE_DATATYPE_MODIFIERFLAG:
  case BRIDGE_DATATYPE_MODIFIERFLAGS_END: {
    switch (index_) {
    case 0:
      IOLOG_ERROR("Invalid DoublePressModifier::add\n");
      break;

    case 1:
      keytokey_.add(datatype, newval);
      keytokey_fire_.add(datatype, newval);
      break;

    default:
      if (!index_is_double_pressed_) {
        keytokey_.add(datatype, newval);
      } else {
        keytokey_fire_.add(datatype, newval);
      }
      break;
    }
    break;
  }

  case BRIDGE_DATATYPE_OPTION: {
    Option option(newval);
    if (Option::USE_SEPARATOR == option) {
      isUseSeparator_ = true;
    } else if (Option::SEPARATOR == option) {
      if (index_ >= 2) {
        index_is_double_pressed_ = true;
      }
    } else {
      if (!index_is_double_pressed_) {
        keytokey_.add(datatype, newval);
      } else {
        keytokey_fire_.add(datatype, newval);
      }
    }

    break;
  }

  default:
    IOLOG_ERROR("DoublePressModifier::add invalid datatype:%u\n", static_cast<unsigned int>(datatype));
    break;
  }
}

bool DoublePressModifier::remap(RemapParams& remapParams) {
  bool iskeydown;
  if (!remapParams.paramsBase.iskeydown(iskeydown)) {
    return false;
  }

  bool result = keytokey_.remap(remapParams);
  if (!result) {
    pressCount_ = 0;
    return false;
  }

  // ----------------------------------------
  if (ic_.getmillisec() > static_cast<uint32_t>(Config::get_essential_config(BRIDGE_ESSENTIAL_CONFIG_INDEX_parameter_doublepressmodifier_threshold))) {
    pressCount_ = 0;
  }
  ic_.begin();

  if (iskeydown) {
    ++pressCount_;
  } else {
    if (pressCount_ >= 2) {
      pressCount_ = 0;

      // clear temporary flags.
      FlagStatus::globalFlagStatus().set();
      keytokey_fire_.call_remap_with_VK_PSEUDO_KEY(EventType::DOWN, remapParams.physicalEventType);
      keytokey_fire_.call_remap_with_VK_PSEUDO_KEY(EventType::UP, remapParams.physicalEventType);
    }
  }

  return true;
}
}
}
