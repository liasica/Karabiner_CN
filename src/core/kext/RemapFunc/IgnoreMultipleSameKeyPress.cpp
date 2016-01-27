#include "diagnostic_macros.hpp"

BEGIN_IOKIT_INCLUDE;
#include <IOKit/IOLib.h>
END_IOKIT_INCLUDE;

#include "IOLogWrapper.hpp"
#include "IgnoreMultipleSameKeyPress.hpp"

namespace org_pqrs_Karabiner {
namespace RemapFunc {
void IgnoreMultipleSameKeyPress::add(AddDataType datatype, AddValue newval) {
  switch (datatype) {
  case BRIDGE_DATATYPE_KEYCODE:
  case BRIDGE_DATATYPE_CONSUMERKEYCODE:
  case BRIDGE_DATATYPE_POINTINGBUTTON:
    fromEvent_ = FromEvent(datatype, newval);
    break;

  case BRIDGE_DATATYPE_MODIFIERFLAG:
  case BRIDGE_DATATYPE_MODIFIERFLAGS_END:
    fromModifierFlags_.push_back(ModifierFlag(datatype, newval));
    break;

  default:
    IOLOG_ERROR("IgnoreMultipleSameKeyPress::add invalid datatype:%u\n", static_cast<unsigned int>(datatype));
    break;
  }
}

bool IgnoreMultipleSameKeyPress::remap(RemapParams& remapParams) {
  if (remapParams.isremapped) goto nottargetevent;
  if (!fromEvent_.changePressingState(remapParams.paramsBase,
                                      FlagStatus::globalFlagStatus(),
                                      fromModifierFlags_)) {
    goto nottargetevent;
  }

  if (needToIgnore_) {
    // disable event.
    remapParams.isremapped = true;
    return true;
  }

  if (!fromEvent_.isPressing()) {
    needToIgnore_ = true;
  }
  return true;

nottargetevent:
  needToIgnore_ = false;
  return false;
}
}
}
