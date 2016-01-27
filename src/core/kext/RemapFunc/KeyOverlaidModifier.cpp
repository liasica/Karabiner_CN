#include "diagnostic_macros.hpp"

BEGIN_IOKIT_INCLUDE;
#include <IOKit/IOLib.h>
END_IOKIT_INCLUDE;

#include "Config.hpp"
#include "IOLogWrapper.hpp"
#include "KeyOverlaidModifier.hpp"

namespace org_pqrs_Karabiner {
namespace RemapFunc {
void KeyOverlaidModifier::add(AddDataType datatype, AddValue newval) {
  switch (datatype) {
  case BRIDGE_DATATYPE_KEYCODE:
  case BRIDGE_DATATYPE_CONSUMERKEYCODE:
  case BRIDGE_DATATYPE_POINTINGBUTTON: {
    switch (index_) {
    case 0:
      fromEvent_ = FromEvent(datatype, newval);
      dppkeytokey_.setFromEvent(fromEvent_);
      break;

    default:
      if (!isUseSeparator_ && index_ >= 2 && indexType_ == INDEX_IS_HOLDING) {
        indexType_ = INDEX_IS_NORMAL;
      }

      addToDependingPressingPeriodKeyToKey(datatype, newval);
      break;
    }
    ++index_;

    break;
  }

  case BRIDGE_DATATYPE_MODIFIERFLAG:
  case BRIDGE_DATATYPE_MODIFIERFLAGS_END: {
    switch (index_) {
    case 0:
      IOLOG_ERROR("Invalid KeyOverlaidModifier::add\n");
      break;

    case 1: {
      dppkeytokey_.addFromModifierFlags(datatype, newval);
      break;
    }

    default:
      addToDependingPressingPeriodKeyToKey(datatype, newval);
      break;
    }
    break;
  }

  case BRIDGE_DATATYPE_OPTION: {
    Option option(newval);
    if (Option::KEYOVERLAIDMODIFIER_REPEAT == option) {
      dppkeytokey_.setPeriodMS(DependingPressingPeriodKeyToKey::PeriodMS::Mode::KEY_OVERLAID_MODIFIER_WITH_REPEAT);
    } else if (Option::KEYOVERLAIDMODIFIER_REPEAT_TOKEYS == option) {
      indexType_ = INDEX_IS_REPEAT_TOKEYS;
      dppkeytokey_.clearToKeys(DependingPressingPeriodKeyToKey::KeyToKeyType::LONG_LONG_PERIOD);
    } else if (Option::USE_SEPARATOR == option) {
      isUseSeparator_ = true;
    } else if (Option::SEPARATOR == option) {
      if (index_ >= 2 && indexType_ == INDEX_IS_HOLDING) {
        indexType_ = INDEX_IS_NORMAL;
      }
    } else if (Option::NOREPEAT == option) {
      if (indexType_ == INDEX_IS_REPEAT_TOKEYS) {
        dppkeytokey_.add(DependingPressingPeriodKeyToKey::KeyToKeyType::LONG_LONG_PERIOD, datatype, newval);
      } else {
        dppkeytokey_.add(DependingPressingPeriodKeyToKey::KeyToKeyType::LONG_PERIOD, datatype, newval);
      }
    } else if (Option::KEYTOKEY_BEFORE_KEYDOWN == option) {
      indexType_ = INDEX_IS_KEYTOKEY_BEFORE_KEYDOWN;
      dppkeytokey_.addBeforeAfterKeys(datatype, newval);
    } else if (Option::KEYTOKEY_AFTER_KEYUP == option) {
      indexType_ = INDEX_IS_KEYTOKEY_AFTER_KEYUP;
      dppkeytokey_.addBeforeAfterKeys(datatype, newval);
    } else {
      IOLOG_ERROR("KeyOverlaidModifier::add unknown option:%u\n", static_cast<unsigned int>(newval));
    }
    break;
  }

  case BRIDGE_DATATYPE_DELAYUNTILREPEAT:
  case BRIDGE_DATATYPE_KEYREPEAT: {
    if (indexType_ == INDEX_IS_REPEAT_TOKEYS) {
      dppkeytokey_.add(DependingPressingPeriodKeyToKey::KeyToKeyType::LONG_LONG_PERIOD, datatype, newval);
    } else {
      dppkeytokey_.add(DependingPressingPeriodKeyToKey::KeyToKeyType::LONG_PERIOD, datatype, newval);
    }
    break;
  }

  default:
    IOLOG_ERROR("KeyOverlaidModifier::add invalid datatype:%u\n", static_cast<unsigned int>(datatype));
    break;
  }
}

void KeyOverlaidModifier::addToDependingPressingPeriodKeyToKey(AddDataType datatype, AddValue newval) {
  switch (indexType_) {
  case INDEX_IS_HOLDING:
    dppkeytokey_.add(DependingPressingPeriodKeyToKey::KeyToKeyType::LONG_PERIOD, datatype, newval);
    break;
  case INDEX_IS_NORMAL:
    dppkeytokey_.add(DependingPressingPeriodKeyToKey::KeyToKeyType::SHORT_PERIOD, datatype, newval);
    dppkeytokey_.add(DependingPressingPeriodKeyToKey::KeyToKeyType::LONG_LONG_PERIOD, datatype, newval);
    dppkeytokey_.add(DependingPressingPeriodKeyToKey::KeyToKeyType::PRESSING_TARGET_KEY_ONLY, datatype, newval);
    break;
  case INDEX_IS_REPEAT_TOKEYS:
    dppkeytokey_.add(DependingPressingPeriodKeyToKey::KeyToKeyType::LONG_LONG_PERIOD, datatype, newval);
    break;
  case INDEX_IS_KEYTOKEY_BEFORE_KEYDOWN:
  case INDEX_IS_KEYTOKEY_AFTER_KEYUP:
    dppkeytokey_.addBeforeAfterKeys(datatype, newval);
    break;
  }
}
}
}
