#import <IOKit/hidsystem/IOHIDLib.h>
#import <IOKit/hidsystem/ev_keymap.h>
#import "IOHIDPostEventWrapper.h"

@interface IOHIDPostEventWrapper () {
  mach_port_t eventDriver_;
  IOOptionBits eventFlags_;
}
@end

@implementation IOHIDPostEventWrapper

- (id)init {
  self = [super init];

  if (self) {
    eventFlags_ = 0;

    // ------------------------------------------------------------
    // Getting eventDriver_

    mach_port_t masterPort = 0;
    mach_port_t service = 0;
    mach_port_t iter = 0;
    kern_return_t kr;

    // Getting master device port
    kr = IOMasterPort(bootstrap_port, &masterPort);
    if (KERN_SUCCESS != kr) goto finish;

    kr = IOServiceGetMatchingServices(masterPort, IOServiceMatching(kIOHIDSystemClass), &iter);
    if (KERN_SUCCESS != kr) goto finish;

    service = IOIteratorNext(iter);
    if (!service) goto finish;

    kr = IOServiceOpen(service, mach_task_self(), kIOHIDParamConnectType, &eventDriver_);
    if (KERN_SUCCESS != kr) goto finish;

  finish:
    if (service) {
      IOObjectRelease(service);
    }
    if (iter) {
      IOObjectRelease(iter);
    }
  }

  return self;
}

- (void)postModifierKey:(IOOptionBits)mask keydown:(BOOL)keydown {
  NXEventData event;
  bzero(&event, sizeof(event));

  if (keydown) {
    eventFlags_ |= mask;
  } else {
    eventFlags_ &= ~mask;
  }

  IOGPoint loc = {0, 0};
  kern_return_t kr = IOHIDPostEvent(eventDriver_, NX_FLAGSCHANGED, loc, &event, kNXEventDataVersion, eventFlags_, TRUE);
  if (KERN_SUCCESS != kr) {
    NSLog(@"[ERROR] IOHIDPostEvent returned 0x%x", kr);
  }
}

- (void)postAuxKey:(uint8_t)auxKeyCode {
  if (!eventDriver_) return;

  uint32_t keydownup[] = {NX_KEYDOWN, NX_KEYUP};

  for (size_t i = 0; i < sizeof(keydownup) / sizeof(keydownup[0]); ++i) {
    NXEventData event;
    bzero(&event, sizeof(event));
    event.compound.subType = NX_SUBTYPE_AUX_CONTROL_BUTTONS;
    event.compound.misc.L[0] = (auxKeyCode << 16 | keydownup[i] << 8);

    IOGPoint loc = {0, 0};
    kern_return_t kr = IOHIDPostEvent(eventDriver_, NX_SYSDEFINED, loc, &event, kNXEventDataVersion, eventFlags_, FALSE);
    if (KERN_SUCCESS != kr) {
      NSLog(@"[ERROR] IOHIDPostEvent returned 0x%x", kr);
    }
  }
}

- (void)postPowerKey {
  if (!eventDriver_) return;

  NXEventData event;
  bzero(&event, sizeof(event));
  event.compound.subType = NX_SUBTYPE_POWER_KEY;

  IOGPoint loc = {0, 0};
  kern_return_t kr = IOHIDPostEvent(eventDriver_, NX_SYSDEFINED, loc, &event, kNXEventDataVersion, 0, FALSE);
  if (KERN_SUCCESS != kr) {
    NSLog(@"[ERROR] IOHIDPostEvent returned 0x%x", kr);
  }
}

- (void)postKey:(uint8_t)keyCode {
  if (keyCode == NX_POWER_KEY) {
    [self postPowerKey];
  } else {
    [self postAuxKey:keyCode];
  }
}

@end
