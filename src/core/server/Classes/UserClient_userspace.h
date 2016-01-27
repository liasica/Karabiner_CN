// -*- Mode: objc; Coding: utf-8; indent-tabs-mode: nil; -*-

#import <Cocoa/Cocoa.h>
#import <IOKit/IOKitLib.h>
#include "bridge.h"

@interface UserClient_userspace : NSObject

- (id)init:(io_async_ref64_t *)asyncref;

- (BOOL)refresh_connection;
- (void)disconnect_from_kext;
- (BOOL)synchronized_communication:(struct BridgeUserClientStruct *)bridgestruct;

@end
