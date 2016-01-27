// -*- Mode: objc; Coding: utf-8; indent-tabs-mode: nil; -*-

#import <Cocoa/Cocoa.h>

@class ClientForKernelspace;
@class XMLCompiler;

@interface PreferencesManager : NSObject {
  IBOutlet ClientForKernelspace* clientForKernelspace_;
  IBOutlet XMLCompiler* xmlCompiler_;
}

// --------------------------------------------------
- (void)load;

- (int)value:(NSString*)name;
- (int)defaultValue:(NSString*)name;
- (void)setValue:(int)newval forName:(NSString*)name;
- (void)setValue:(int)newval forName:(NSString*)name tellToKext:(BOOL)tellToKext;
- (void)clearNotSave;

- (NSArray*)essential_config;
- (NSDictionary*)changed;

// --------------------------------------------------
- (NSInteger)configlist_selectedIndex;
- (NSString*)configlist_selectedName;
- (NSString*)configlist_selectedIdentifier;
- (NSArray*)configlist_getConfigList;
- (NSUInteger)configlist_count;
- (NSDictionary*)configlist_dictionary:(NSInteger)rowIndex;
- (NSString*)configlist_name:(NSInteger)rowIndex;
- (NSString*)configlist_identifier:(NSInteger)rowIndex;
- (void)configlist_select:(NSInteger)newindex;
- (void)configlist_setName:(NSInteger)rowIndex name:(NSString*)name;
- (void)configlist_append;
- (void)configlist_delete:(NSInteger)rowIndex;
- (void)configlist_clear_all_values:(NSInteger)rowIndex;
- (void)configlist_sortByAppendIndex;
- (void)configlist_sortByName;

- (BOOL)isCheckForUpdates;

- (IBAction)sendConfigListChangedNotification:(id)sender;

@end
