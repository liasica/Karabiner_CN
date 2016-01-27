#include <IOKit/IOKitLib.h>
#import "AppDelegate.h"
#import "KarabinerKeys.h"
#import "MigrationUtilities.h"
#import "PreferencesKeys.h"
#import "Relauncher.h"
#import "SessionObserver.h"

enum { MAX_FINGERS = 4 };
static int current_status_[MAX_FINGERS];
static FingerStatus* lastFingerStatus_ = nil;
static BOOL has_last_device = NO;
static int last_device = 0;
static time_t last_timestamp_ = 0;
static NSTimer* global_timer_[MAX_FINGERS];
static NSTimer* reset_timer_;

@interface AppDelegate () {
  SessionObserver* sessionObserver_;
}
@end

@implementation AppDelegate

- (id)init {
  self = [super init];

  if (self) {
    lastFingerStatus_ = [FingerStatus new];

    for (int i = 0; i < MAX_FINGERS; ++i) {
      current_status_[i] = 0;
      global_timer_[i] = nil;
    }

    reset_timer_ = [NSTimer scheduledTimerWithTimeInterval:1.0
                                                    target:self
                                                  selector:@selector(resetTimerFireMethod:)
                                                  userInfo:nil
                                                   repeats:YES];
  }

  return self;
}

// ------------------------------------------------------------
typedef struct {
  float x;
  float y;
} mtPoint;

typedef struct {
  mtPoint position;
  mtPoint velocity;
} mtReadout;

typedef struct {
  int frame;
  double timestamp;
  int identifier;
  int state;
  int unknown1;
  int unknown2;
  mtReadout normalized;
  float size;
  int unknown3;
  float angle;
  float majorAxis;
  float minorAxis;
  mtReadout unknown4;
  int unknown5[2];
  float unknown6;
} Finger;

typedef void* MTDeviceRef;
typedef int (*MTContactCallbackFunction)(int, Finger*, int, double, int);

CFMutableArrayRef MTDeviceCreateList(void);
void MTRegisterContactFrameCallback(MTDeviceRef, MTContactCallbackFunction);
void MTUnregisterContactFrameCallback(MTDeviceRef, MTContactCallbackFunction);
void MTDeviceStart(MTDeviceRef, int);
void MTDeviceStop(MTDeviceRef, int);

AppDelegate* global_self_ = nil;
IgnoredAreaView* global_ignoredAreaView_ = nil;
KarabinerClient* global_client_ = nil;

- (void)setValueFromTimer:(NSTimer*)timer {
  NSDictionary* dict = [timer userInfo];
  [[global_client_ proxy] setValue:[dict[@"value"] intValue] forName:dict[@"name"]];
}

static void setPreference(int fingers, int newvalue) {
  @synchronized(global_client_) {
    NSString* name = [PreferencesController getSettingName:fingers];
    if ([name length] > 0) {
      @try {
        id client = [global_client_ proxy];

        if (global_timer_[fingers - 1]) {
          [global_timer_[fingers - 1] invalidate];
          global_timer_[fingers - 1] = nil;
        }

        NSInteger delay = 0;
        if (newvalue == 0) {
          delay = [[NSUserDefaults standardUserDefaults] integerForKey:kDelayBeforeTurnOff];
        } else {
          delay = [[NSUserDefaults standardUserDefaults] integerForKey:kDelayBeforeTurnOn];
        }

        if (delay == 0) {
          [client setValue:newvalue forName:name];
        } else {
          global_timer_[fingers - 1] = [NSTimer scheduledTimerWithTimeInterval:(1.0 * delay / 1000.0)
                                                                        target:global_self_
                                                                      selector:@selector(setValueFromTimer:)
                                                                      userInfo:@{
                                                                        @"name" : name,
                                                                        @"value" : @(newvalue),
                                                                      }
                                                                       repeats:NO];
        }
      }
      @catch (NSException* exception) {
        NSLog(@"%@", exception);
      }
    }
  }
}

- (void)resetPreferences {
  for (int i = 0; i < MAX_FINGERS; ++i) {
    setPreference(i + 1, 0);
  }
}

- (void)resetTimerFireMethod:(NSTimer*)timer {
  dispatch_async(dispatch_get_main_queue(), ^{
    // ------------------------------------------------------------
    // If multi touch devices are touched at launch,
    // the first multi touch callback is called with invalid `device` arg.
    //
    // It causes an issue that all following events are ignored by has_last_device flag.
    //
    // To avoid this issue, if recent multi touch callback is too old, we reset has_last_device.
    // The callback must be called by each 100ms when a device is touched.
    // So, if the callback is not called in 2 seconds, we can reset has_last_device.

    time_t now;
    time(&now);

    if (now - last_timestamp_ > 2) {
      if (has_last_device) {
        NSLog(@"resetting the internal state by timer");

        has_last_device = NO;
        [self resetPreferences];
      }
    }
  });
}

// ------------------------------------------------------------
// Multitouch callback
static int callback(int device, Finger* data, int fingers, double timestamp, int frame) {
  if (!data) {
    fingers = 0;
  }

  __block Finger* dataCopy = NULL;
  if (fingers > 0) {
    size_t size = sizeof(Finger) * fingers;
    dataCopy = (Finger*)(malloc(size));
    memcpy(dataCopy, data, size);
  }

  dispatch_async(dispatch_get_main_queue(), ^{
    // ------------------------------------------------------------
    // If there are multiple devices (For example, Trackpad and Magic Mouse),
    // we handle only one device at the same time.
    if (has_last_device) {
      // ignore other devices.
      if (device != last_device) {
        goto finish;
      }
    }

    if (fingers == 0) {
      has_last_device = NO;
    } else {
      has_last_device = YES;
      last_device = device;
    }

    // ------------------------------------------------------------
    time(&last_timestamp_);

    [global_ignoredAreaView_ clearFingers];

    {
      int valid_fingers = 0;
      FingerStatus* fingerStatus = [FingerStatus new];

      for (int i = 0; i < fingers; ++i) {
        // state values:
        //   4: touched
        //   1-3,5-7: near
        if (dataCopy[i].state != 4) {
          continue;
        }

        int identifier = dataCopy[i].identifier;
        NSPoint point = NSMakePoint(dataCopy[i].normalized.position.x, dataCopy[i].normalized.position.y);

        BOOL ignored = NO;
        if ([IgnoredAreaView isIgnoredArea:point]) {
          ignored = YES;

          // Finding FingerStatus by identifier.
          if ([lastFingerStatus_ isActive:identifier]) {
            // If a finger is already active, we should not ignore this finger.
            // (This finger has been moved into ignored area from active area.)
            ignored = NO;
          }
        }

        [fingerStatus add:identifier active:(!ignored)];

        if (!ignored) {
          ++valid_fingers;
        }

        [global_ignoredAreaView_ addFinger:point ignored:ignored];
      }

      lastFingerStatus_ = fingerStatus;

      // ----------------------------------------
      // deactivating settings first.
      for (int i = 0; i < MAX_FINGERS; ++i) {
        if (current_status_[i] && valid_fingers != i + 1) {
          current_status_[i] = 0;
          setPreference(i + 1, 0);
        }
      }

      // activating setting.
      //
      // Note: Set current_status_ only if the targeted setting is enabled.
      // If not, unintentional deactivation is called in above.
      //
      // - one finger: disabled
      // - two fingers: enabled
      //
      // In this case,
      // we must not call "setPreference" if only one finger is touched/released on multi-touch device.
      // If we don't check [PreferencesController isSettingEnabled],
      // setPreference is called in above when we release one finger from device.
      //
      if (valid_fingers > 0 && current_status_[valid_fingers - 1] == 0 &&
          [PreferencesController isSettingEnabled:valid_fingers]) {
        current_status_[valid_fingers - 1] = 1;
        setPreference(valid_fingers, 1);
      }
    }

  finish:
    if (dataCopy) {
      free(dataCopy);
      dataCopy = NULL;
    }
  });

  return 0;
}

- (void)setcallback:(BOOL)isset {
  @synchronized(self) {
    // ------------------------------------------------------------
    // unset callback (even if isset is YES.)
    if (mtdevices_) {
      for (NSUInteger i = 0; i < [mtdevices_ count]; ++i) {
        MTDeviceRef device = (__bridge MTDeviceRef)(mtdevices_[i]);
        if (!device) continue;

        MTDeviceStop(device, 0);
        MTUnregisterContactFrameCallback(device, callback);
      }
      mtdevices_ = nil;
    }

    // ------------------------------------------------------------
    // set callback if needed
    if (isset) {
      mtdevices_ = (NSArray*)CFBridgingRelease(MTDeviceCreateList());
      if (mtdevices_) {
        for (NSUInteger i = 0; i < [mtdevices_ count]; ++i) {
          MTDeviceRef device = (__bridge MTDeviceRef)(mtdevices_[i]);
          if (!device) continue;

          MTRegisterContactFrameCallback(device, callback);
          MTDeviceStart(device, 0);
        }
      }
    }

    [self resetPreferences];
  }
}

// ------------------------------------------------------------
// IONotification
- (void)release_iterator:(io_iterator_t)iterator {
  for (;;) {
    io_object_t obj = IOIteratorNext(iterator);
    if (!obj) break;

    IOObjectRelease(obj);
  }
}

static void observer_IONotification(void* refcon, io_iterator_t iterator) {
  dispatch_async(dispatch_get_main_queue(), ^{
    // Relaunch when devices are plugged/unplugged.
    NSLog(@"observer_IONotification");
    [NSTask launchedTaskWithLaunchPath:[[NSBundle mainBundle] executablePath] arguments:@[]];
    [NSApp terminate:nil];
  });
}

- (void)unregisterIONotification {
  NSLog(@"unregisterIONotification");

  @synchronized(self) {
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

- (void)registerIONotification {
  NSLog(@"registerIONotification");

  @synchronized(self) {
    if (notifyport_) {
      [self unregisterIONotification];
    }

    notifyport_ = IONotificationPortCreate(kIOMasterPortDefault);
    if (!notifyport_) {
      NSLog(@"[ERROR] IONotificationPortCreate");
      return;
    }

    {
      // ------------------------------------------------------------
      NSMutableDictionary* match = (__bridge NSMutableDictionary*)(IOServiceMatching("AppleMultitouchDevice"));

      // ----------------------------------------------------------------------
      io_iterator_t it;
      kern_return_t kr;

      // for kIOTerminatedNotification
      kr = IOServiceAddMatchingNotification(notifyport_,
                                            kIOTerminatedNotification,
                                            (__bridge CFMutableDictionaryRef)(match),
                                            &observer_IONotification,
                                            (__bridge void*)(self),
                                            &it);
      if (kr != kIOReturnSuccess) {
        NSLog(@"[ERROR] IOServiceAddMatchingNotification");
        return;
      }
      [self release_iterator:it];
    }

    {
      // ------------------------------------------------------------
      NSMutableDictionary* match = (__bridge NSMutableDictionary*)(IOServiceMatching("AppleMultitouchDevice"));

      // ----------------------------------------------------------------------
      io_iterator_t it;
      kern_return_t kr;

      // for kIOMatchedNotification
      kr = IOServiceAddMatchingNotification(notifyport_,
                                            kIOMatchedNotification,
                                            (__bridge CFMutableDictionaryRef)(match),
                                            &observer_IONotification,
                                            (__bridge void*)(self),
                                            &it);
      if (kr != kIOReturnSuccess) {
        NSLog(@"[ERROR] IOServiceAddMatchingNotification");
        return;
      }
      [self release_iterator:it];
    }

    // ----------------------------------------------------------------------
    loopsource_ = IONotificationPortGetRunLoopSource(notifyport_);
    if (!loopsource_) {
      NSLog(@"[ERROR] IONotificationPortGetRunLoopSource");
      return;
    }
    CFRunLoopAddSource(CFRunLoopGetCurrent(), loopsource_, kCFRunLoopDefaultMode);
  }
}

// ------------------------------------------------------------
- (void)observer_NSWorkspaceDidWakeNotification:(NSNotification*)notification {
  dispatch_async(dispatch_get_main_queue(), ^{
    NSLog(@"observer_NSWorkspaceDidWakeNotification");

    // sleep until devices are settled.
    [NSThread sleepForTimeInterval:1.0];

    if ([[NSUserDefaults standardUserDefaults] boolForKey:@"relaunchAfterWakeUpFromSleep"]) {
      double wait = [[[NSUserDefaults standardUserDefaults] stringForKey:@"relaunchWait"] doubleValue];
      if (wait > 0) {
        [NSThread sleepForTimeInterval:wait];
      }

      [NSTask launchedTaskWithLaunchPath:[[NSBundle mainBundle] executablePath] arguments:@[]];
      [NSApp terminate:self];
    }

    has_last_device = NO;

    [self setcallback:YES];
  });
}

- (void)registerWakeNotification {
  [[[NSWorkspace sharedWorkspace] notificationCenter] addObserver:self
                                                         selector:@selector(observer_NSWorkspaceDidWakeNotification:)
                                                             name:NSWorkspaceDidWakeNotification
                                                           object:nil];
}

- (void)unregisterWakeNotification {
  [[[NSWorkspace sharedWorkspace] notificationCenter] removeObserver:self
                                                                name:NSWorkspaceDidWakeNotification
                                                              object:nil];
}

// ----------------------------------------
- (void)distributedObserver_kKarabinerServerDidLaunchNotification:(NSNotification*)notification {
  dispatch_async(dispatch_get_main_queue(), ^{
    [NSTask launchedTaskWithLaunchPath:[[NSBundle mainBundle] executablePath] arguments:@[]];
    [NSApp terminate:self];
  });
}

// ------------------------------------------------------------
- (void)applicationDidFinishLaunching:(NSNotification*)aNotification {
  if ([MigrationUtilities migrate:@[ @"org.pqrs.KeyRemap4MacBook.multitouchextension" ]
           oldApplicationSupports:@[]
                         oldPaths:@[ @"/Applications/KeyRemap4MacBook.app/Contents/Applications/KeyRemap4MacBook_multitouchextension.app" ]]) {
    [Relauncher relaunch];
  }

  // ----------------------------------------
  [preferences_ load];

  if (![[NSUserDefaults standardUserDefaults] boolForKey:@"hideIconInDock"]) {
    ProcessSerialNumber psn = {0, kCurrentProcess};
    TransformProcessType(&psn, kProcessTransformToForegroundApplication);
  }

  global_self_ = self;
  global_ignoredAreaView_ = ignoredAreaView_;
  global_client_ = client_;

  sessionObserver_ = [[SessionObserver alloc] init:1
      active:^{
        [self registerIONotification];
        [self registerWakeNotification];

        // sleep until devices are settled.
        [NSThread sleepForTimeInterval:1.0];

        [self setcallback:YES];
      }
      inactive:^{
        [self unregisterIONotification];
        [self unregisterWakeNotification];
        [self setcallback:NO];
      }];

  [[NSDistributedNotificationCenter defaultCenter] addObserver:self
                                                      selector:@selector(distributedObserver_kKarabinerServerDidLaunchNotification:)
                                                          name:kKarabinerServerDidLaunchNotification
                                                        object:nil
                                            suspensionBehavior:NSNotificationSuspensionBehaviorDeliverImmediately];

  [self setcallback:YES];

  [Relauncher resetRelaunchedCount];
}

- (void)applicationWillTerminate:(NSNotification*)aNotification {
  [self setcallback:NO];
}

- (BOOL)applicationShouldHandleReopen:(NSApplication*)theApplication hasVisibleWindows:(BOOL)flag {
  [preferences_ show];
  return YES;
}

@end
