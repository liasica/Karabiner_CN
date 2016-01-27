#import "Relauncher.h"
#import "UserClient_userspace.h"
#include "bridge.h"

@interface UserClient_userspace () {
  io_service_t service_;
  io_connect_t connect_;
  IONotificationPortRef notifyport_;
  CFRunLoopSourceRef loopsource_;
  io_async_ref64_t* asyncref_;
}
@end

@implementation UserClient_userspace

- (void)closeUserClient {
  // ----------------------------------------
  // call BRIDGE_USERCLIENT_CLOSE
  if (service_ != IO_OBJECT_NULL && connect_ != IO_OBJECT_NULL) {
    // BRIDGE_USERCLIENT_CLOSE may be failed. (when kext is unloaded, etc.)
    // So we don't output a log message when it is failed.
    IOConnectCallScalarMethod(connect_, BRIDGE_USERCLIENT_CLOSE, NULL, 0, NULL, NULL);
  }

  // ----------------------------------------
  // release previous values.
  if (connect_ != IO_OBJECT_NULL) {
    IOServiceClose(connect_);
    connect_ = IO_OBJECT_NULL;
  }
  if (service_ != IO_OBJECT_NULL) {
    IOObjectRelease(service_);
    service_ = IO_OBJECT_NULL;
  }
}

- (void)openUserClient {
  io_iterator_t iterator;

  kern_return_t kernResult = IOServiceGetMatchingServices(kIOMasterPortDefault, IOServiceMatching("org_pqrs_driver_Karabiner"), &iterator);
  if (kernResult != KERN_SUCCESS) {
    NSLog(@"[ERROR] IOServiceGetMatchingServices returned 0x%08x\n\n", kernResult);
    return;
  }

  for (;;) {
    // service will not be found until registerService() is called in kext.
    // If service does not exist, wait for a while and retry.
    io_service_t s = IOIteratorNext(iterator);
    if (s == IO_OBJECT_NULL) {
      NSLog(@"[INFO] IOService is not found.");
      break;
    }

    [self closeUserClient];

    // ----------------------------------------
    // setup service_
    service_ = s;
    kernResult = IOObjectRetain(service_);
    if (kernResult != KERN_SUCCESS) {
      NSLog(@"[ERROR] IOObjectRetain returned 0x%08x\n", kernResult);
      continue;
    }

    // ----------------------------------------
    // setup connect_
    kernResult = IOServiceOpen(service_, mach_task_self(), 0, &connect_);
    if (kernResult != KERN_SUCCESS) {
      NSLog(@"[ERROR] IOServiceOpen returned 0x%08x\n", kernResult);
      continue;
    }

    // ----------------------------------------
    // open
    {
      uint64_t bridge_version =
#include "../../../../src/bridge/output/include.bridge_version.h"
          ;
      uint64_t open_result = 0;
      uint32_t count = 1;

      kernResult = IOConnectCallScalarMethod(connect_, BRIDGE_USERCLIENT_OPEN,
                                             &bridge_version,
                                             1,
                                             &open_result,
                                             &count);
      if (kernResult != KERN_SUCCESS) {
        NSLog(@"[ERROR] BRIDGE_USERCLIENT_OPEN returned 0x%08x\n", kernResult);
        continue;

      } else {
        if (open_result == BRIDGE_USERCLIENT_OPEN_RETURN_ERROR_BRIDGE_VERSION_MISMATCH) {
          NSLog(@"[ERROR] BRIDGE_USERCLIENT_OPEN_RETURN_ERROR_BRIDGE_VERSION_MISMATCH\n");
          if ([Relauncher isEqualPreviousProcessVersionAndCurrentProcessVersion]) {
            @throw [NSException exceptionWithName:@"UserClient_userspaceException"
                                           reason:@"Kernel extension and app version are mismatched.\nPlease restart your system in order to reload kernel extension."
                                         userInfo:nil];
          } else {
            NSLog(@"Karabiner might have been upgraded.");
            [Relauncher relaunch];
          }
          continue;
        }
      }
    }

    // ----------------------------------------
    // set notification
    if (notifyport_) {
      kernResult = IOConnectCallAsyncScalarMethod(connect_,
                                                  BRIDGE_USERCLIENT_NOTIFICATION_FROM_KEXT,
                                                  IONotificationPortGetMachPort(notifyport_),
                                                  *asyncref_,
                                                  kOSAsyncRef64Count,
                                                  NULL,  // input
                                                  0,     // inputCnt
                                                  NULL,  // output
                                                  NULL); // outputCnt
      if (kernResult != KERN_SUCCESS) {
        NSLog(@"[ERROR] BRIDGE_USERCLIENT_NOTIFICATION_FROM_KEXT returned 0x%08x\n", kernResult);
        continue;
      }

      // succeed
      goto finish;
    }
  }

  // failed to open connection.
  [self closeUserClient];

finish:
  IOObjectRelease(iterator);
}

- (id)init:(io_async_ref64_t*)asyncref;
{
  self = [super init];

  if (self) {
    service_ = IO_OBJECT_NULL;
    connect_ = IO_OBJECT_NULL;
    asyncref_ = asyncref;
  }

  return self;
}

// ======================================================================
- (void)connect_to_kext {
  @synchronized(self) {
    [self disconnect_from_kext];

    service_ = IO_OBJECT_NULL;
    connect_ = IO_OBJECT_NULL;

    // ----------------------------------------
    // setup IONotification
    notifyport_ = IONotificationPortCreate(kIOMasterPortDefault);
    if (!notifyport_) {
      NSLog(@"[ERROR] IONotificationPortCreate failed\n");

    } else {
      loopsource_ = IONotificationPortGetRunLoopSource(notifyport_);
      if (!loopsource_) {
        NSLog(@"[ERROR] IONotificationPortGetRunLoopSource failed\n");

      } else {
        CFRunLoopAddSource(CFRunLoopGetCurrent(), loopsource_, kCFRunLoopDefaultMode);
        [self openUserClient];
      }
    }
  }
}

- (void)disconnect_from_kext {
  @synchronized(self) {
    [self closeUserClient];

    if (notifyport_) {
      if (loopsource_) {
        CFRunLoopSourceInvalidate(loopsource_);
        loopsource_ = nil;
      }
      IONotificationPortDestroy(notifyport_);
      notifyport_ = nil;
    }
  }
}

- (BOOL)refresh_connection {
  @synchronized(self) {
    [self disconnect_from_kext];
    [self connect_to_kext];

    return connect_ != IO_OBJECT_NULL;
  }
}

- (BOOL)synchronized_communication:(struct BridgeUserClientStruct*)bridgestruct {
  @synchronized(self) {
    if (connect_ == IO_OBJECT_NULL) {
      NSLog(@"[INFO] BRIDGE_USERCLIENT_SYNCHRONIZED_COMMUNICATION connection is null");
      return NO;
    }
    if (!bridgestruct) return NO;

    uint64_t output = 0;
    uint32_t outputCnt = 1;
    kern_return_t kernResult = IOConnectCallMethod(connect_,
                                                   BRIDGE_USERCLIENT_SYNCHRONIZED_COMMUNICATION,
                                                   NULL, 0,                             // scalar input
                                                   bridgestruct, sizeof(*bridgestruct), // struct input
                                                   &output, &outputCnt,                 // scalar output
                                                   NULL, NULL);                         // struct output
    if (kernResult != KERN_SUCCESS) {
      NSLog(@"[ERROR] BRIDGE_USERCLIENT_SYNCHRONIZED_COMMUNICATION returned 0x%08x\n", kernResult);
      return NO;
    }
    if (output != BRIDGE_USERCLIENT_SYNCHRONIZED_COMMUNICATION_RETURN_SUCCESS) {
      NSLog(@"[ERROR] BRIDGE_USERCLIENT_SYNCHRONIZED_COMMUNICATION output is not SUCCESS (%lld)\n", output);
      return NO;
    }

    // succeed
    return YES;
  }
}

@end
