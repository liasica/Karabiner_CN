// -*- Mode: objc; Coding: utf-8; indent-tabs-mode: nil; -*-

#import <Cocoa/Cocoa.h>
#import <Carbon/Carbon.h>

@interface InputSource : NSObject

@property(copy, readonly) NSString* languagecode;
@property(copy, readonly) NSString* inputSourceID;
@property(copy, readonly) NSString* inputModeID;

- (id)initWithTISInputSourceRef:(TISInputSourceRef)ref;
- (Boolean)selected;
- (void)select;

@end
