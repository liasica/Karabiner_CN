#include "diagnostic_macros.hpp"

BEGIN_IOKIT_INCLUDE;
#include <IOKit/IOLib.h>
END_IOKIT_INCLUDE;

#include "Config.hpp"
#include "EventOutputQueue.hpp"
#include "EventWatcher.hpp"
#include "IOLogWrapper.hpp"
#include "PointingRelativeToScroll.hpp"

namespace org_pqrs_Karabiner {
namespace RemapFunc {
List PointingRelativeToScroll::queue_;
Vector_ModifierFlag PointingRelativeToScroll::currentFromModifierFlags_;
Vector_ModifierFlag PointingRelativeToScroll::currentToModifierFlags_;
AutogenId PointingRelativeToScroll::currentAutogenId_(0);
PhysicalEventType PointingRelativeToScroll::lastPhysicalEventType_ = PhysicalEventType::DOWN;
TimerWrapper PointingRelativeToScroll::timer_;

void PointingRelativeToScroll::static_initialize(IOWorkLoop& workloop) {
  timer_.initialize(&workloop, nullptr, PointingRelativeToScroll::timer_callback);
}

void PointingRelativeToScroll::static_terminate(void) {
  timer_.terminate();

  queue_.clear();
}

void PointingRelativeToScroll::cancelScroll(void) {
  timer_.cancelTimeout();

  queue_.clear();
}

void PointingRelativeToScroll::add(AddDataType datatype, AddValue newval) {
  switch (datatype) {
  case BRIDGE_DATATYPE_MODIFIERFLAG:
  case BRIDGE_DATATYPE_MODIFIERFLAGS_END: {
    switch (index_type_) {
    case INDEX_TYPE_DEFAULT:
      fromModifierFlags_.push_back(ModifierFlag(datatype, newval));
      keytokey_.add(datatype, newval);
      break;
    case INDEX_TYPE_TOFLAGS:
      toModifierFlags_.push_back(ModifierFlag(datatype, newval));
      break;
    case INDEX_TYPE_TOKEYS:
      keytokey_.add(datatype, newval);
      break;
    }
    break;
  }

  case BRIDGE_DATATYPE_KEYCODE:
  case BRIDGE_DATATYPE_CONSUMERKEYCODE:
  case BRIDGE_DATATYPE_POINTINGBUTTON: {
    switch (index_type_) {
    case INDEX_TYPE_DEFAULT:
      fromEvent_ = FromEvent(datatype, newval);
      toEvent_ = ToEvent(datatype, newval);
      break;
    case INDEX_TYPE_TOKEYS:
      keytokey_.add(datatype, newval);
      break;
    default:
      IOLOG_ERROR("PointingRelativeToScroll::add invalid BRIDGE_DATATYPE_POINTINGBUTTON\n");
      break;
    }
    break;
  }

  case BRIDGE_DATATYPE_OPTION: {
    Option option(newval);
    if (Option::POINTINGRELATIVETOSCROLL_TOFLAGS == option) {
      index_type_ = INDEX_TYPE_TOFLAGS;
    }
    if (Option::POINTINGRELATIVETOSCROLL_TOKEYS == option) {
      index_type_ = INDEX_TYPE_TOKEYS;
      isToKeysDefined_ = true;
    }
    break;
  }

  default:
    IOLOG_ERROR("PointingRelativeToScroll::add invalid datatype:%u\n", static_cast<unsigned int>(datatype));
    break;
  }
}

bool PointingRelativeToScroll::remap(RemapParams& remapParams) {
  // ------------------------------------------------------------
  // PointingRelativeToScroll grabs all pointing movement events.
  // Therefore, if user write inappropriate <autogen> (empty flags and empty buttons),
  // user cannot control pointing device at all.
  //
  // For example:
  //   <autogen>__PointingRelativeToScroll__ PointingButton::LEFT | PointingButton::RIGHT</autogen>
  //
  // (Buttons(LEFT | RIGHT) will be ignored. So, this autogen is interpreted as
  // <autogen>__PointingRelativeToScroll__</autogen>.)
  //
  // Skip on error in order to avoid this situation.
  if (fromEvent_.getType() == FromEvent::Type::NONE &&
      fromModifierFlags_.empty()) {
    IOLOG_WARN("Ignore __PointingRelativeToScroll__ with no option. "
               "Please use \"__PointingRelativeToScroll__ PointingButton::NONE\".\n");
    return false;
  }

  // ------------------------------------------------------------
  if (remapParams.isremapped) return false;

  bool useFromEvent = true;
  if (fromEvent_.getType() == FromEvent::Type::NONE) {
    useFromEvent = false;
  }
  if (fromEvent_.getType() == FromEvent::Type::POINTING_BUTTON &&
      fromEvent_.getPointingButton() == PointingButton::NONE) {
    useFromEvent = false;
  }

  if (!useFromEvent) {
    if (!FlagStatus::globalFlagStatus().isOn(fromModifierFlags_)) return false;
    goto doremap;

  } else {
    // FromEvent == KeyCode or ConsumerKeyCode or PointingButton.

    bool pressingStateChanged = fromEvent_.changePressingState(remapParams.paramsBase,
                                                               FlagStatus::globalFlagStatus(),
                                                               fromModifierFlags_);
    if (pressingStateChanged) {
      bool iskeydown;
      if (remapParams.paramsBase.iskeydown(iskeydown)) {
        if (iskeydown) {
          FlagStatus::globalFlagStatus().decrease(fromEvent_.getModifierFlag());
          ButtonStatus::decrease(fromEvent_.getPointingButton());

          absolute_distance_ = 0;
          begin_ic_.begin();
          chained_ic_.begin();
          chained_delta1_ = 0;
          chained_delta2_ = 0;

        } else {
          FlagStatus::globalFlagStatus().increase(fromEvent_.getModifierFlag());
          ButtonStatus::increase(fromEvent_.getPointingButton());

          cancelScroll();

          const uint32_t DISTANCE_THRESHOLD = 5;
          const uint32_t TIME_THRESHOLD = 300;
          if (absolute_distance_ <= DISTANCE_THRESHOLD && begin_ic_.getmillisec() < TIME_THRESHOLD) {
            // Fire by a click event.
            if (isToKeysDefined_) {
              keytokey_.call_remap_with_VK_PSEUDO_KEY(EventType::DOWN, remapParams.physicalEventType);
              keytokey_.call_remap_with_VK_PSEUDO_KEY(EventType::UP, remapParams.physicalEventType);

            } else {
              toEvent_.fire_downup(autogenId_, remapParams.physicalEventType);
            }
          }
        }
      }

      // ignore this event.
      goto returntrue;

    } else {
      if (!fromEvent_.isPressing()) {
        return false;
      }
    }
  }

doremap:
  // change only cursor move events.
  {
    auto params = remapParams.paramsBase.get_Params_RelativePointerEventCallback();
    if (!params) return false;
    if (params->ex_button != PointingButton::NONE) return false;
  }

  // We need to call EventWatcher::on here.
  // See the comments in EventInputQueue::fire_timer_callback.
  EventWatcher::on();
  toscroll(remapParams);

returntrue:
  remapParams.isremapped = true;
  lastPhysicalEventType_ = remapParams.physicalEventType;
  return true;
}

void PointingRelativeToScroll::toscroll(RemapParams& remapParams) {
  auto params = remapParams.paramsBase.get_Params_RelativePointerEventCallback();
  if (!params) return;

  // ----------------------------------------
  const uint32_t CANCEL_THRESHOLD = 100;
  if (chained_ic_.getmillisec() > CANCEL_THRESHOLD) {
    chained_delta1_ = 0;
    chained_delta2_ = 0;
    cancelScroll();
  }

  int delta1 = -params->dy;
  int delta2 = -params->dx;

  chained_ic_.begin();

  // ----------------------------------------
  if (Config::get_essential_config(BRIDGE_ESSENTIAL_CONFIG_INDEX_option_pointing_disable_vertical_scroll)) delta1 = 0;
  if (Config::get_essential_config(BRIDGE_ESSENTIAL_CONFIG_INDEX_option_pointing_disable_horizontal_scroll)) delta2 = 0;

  // ----------------------------------------
  // ignore minuscule move
  const unsigned int abs1 = abs(delta1);
  const unsigned int abs2 = abs(delta2);

  if (abs1 > abs2 * 2) {
    delta2 = 0;
  }
  if (abs2 > abs1 * 2) {
    delta1 = 0;
  }

  // ----------------------------------------
  // Fixation processing

  if (Config::get_essential_config(BRIDGE_ESSENTIAL_CONFIG_INDEX_option_pointing_enable_scrollwheel_fixation)) {
    // When 300ms passes from the last event, we reset a value.
    const uint32_t FIXATION_MILLISEC = 300;
    if (fixation_ic_.getmillisec() > FIXATION_MILLISEC) {
      fixation_begin_ic_.begin();
      fixation_delta1_ = 0;
      fixation_delta2_ = 0;
    }
    fixation_ic_.begin();

    if (fixation_delta1_ > fixation_delta2_ * 2) {
      delta2 = 0;
    }
    if (fixation_delta2_ > fixation_delta1_ * 2) {
      delta1 = 0;
    }

    // Only first 1000ms performs the addition of fixation_delta1, fixation_delta2.
    const uint32_t FIXATION_EARLY_MILLISEC = 1000;
    if (fixation_begin_ic_.getmillisec() < FIXATION_EARLY_MILLISEC) {
      if (delta1 == 0) fixation_delta2_ += abs2;
      if (delta2 == 0) fixation_delta1_ += abs1;
    }
  }

  // ------------------------------------------------------------
  // when sign is different
  if (0 > delta1 * chained_delta1_ ||
      0 > delta2 * chained_delta2_) {
    queue_.clear();
    chained_delta1_ = delta1;
    chained_delta2_ = delta2;

  } else if (abs(delta1) > abs(chained_delta1_) ||
             abs(delta2) > abs(chained_delta2_)) {
    // greater delta.
    chained_delta1_ = delta1;
    chained_delta2_ = delta2;
  }

  absolute_distance_ += abs(chained_delta1_) + abs(chained_delta2_);
  queue_.push_back(new Item(chained_delta1_ * EventOutputQueue::FireScrollWheel::DELTA_SCALE, chained_delta2_ * EventOutputQueue::FireScrollWheel::DELTA_SCALE));

  currentFromModifierFlags_ = fromModifierFlags_;
  currentToModifierFlags_ = toModifierFlags_;
  currentAutogenId_ = autogenId_;
  timer_.setTimeoutMS(SCROLL_INTERVAL_MS, false);
}

void PointingRelativeToScroll::timer_callback(OSObject* owner, IOTimerEventSource* sender) {
  // ----------------------------------------
  int delta1 = 0;
  int delta2 = 0;
  {
    Item* p = static_cast<Item*>(queue_.safe_front());
    if (!p) return;

    delta1 = p->delta1;
    delta2 = p->delta2;

    queue_.pop_front();
  }

  // ----------------------------------------
  FlagStatus::globalFlagStatus().temporary_decrease(currentFromModifierFlags_);
  FlagStatus::globalFlagStatus().temporary_increase(currentToModifierFlags_);
  {
    int d1 = delta1;
    int d2 = delta2;
    if (Config::get_essential_config(BRIDGE_ESSENTIAL_CONFIG_INDEX_option_pointing_reverse_vertical_scrolling)) {
      d1 = -d1;
    }
    if (Config::get_essential_config(BRIDGE_ESSENTIAL_CONFIG_INDEX_option_pointing_reverse_horizontal_scrolling)) {
      d2 = -d2;
    }
    EventOutputQueue::FireScrollWheel::fire(d1, d2, currentAutogenId_, lastPhysicalEventType_);
  }
  // We need to restore temporary flags.
  // Because normal cursor move event don't restore temporary_count_.
  // (See EventInputQueue::push_RelativePointerEventCallback.)
  FlagStatus::globalFlagStatus().temporary_decrease(currentToModifierFlags_);
  FlagStatus::globalFlagStatus().temporary_increase(currentFromModifierFlags_);

  // ----------------------------------------
  if (!Config::get_essential_config(BRIDGE_ESSENTIAL_CONFIG_INDEX_option_pointing_disable_momentum_scroll)) {
    if (delta1 != 0 || delta2 != 0) {
      queue_.push_back(new Item(delta1 / 2, delta2 / 2));
    }
  }

  timer_.setTimeoutMS(SCROLL_INTERVAL_MS, false);
}
}
}
