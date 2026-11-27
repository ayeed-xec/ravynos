/* Copyright (c) 2007 Christopher J. W. Lloyd

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

// Original - Christopher Lloyd <cjwl@objc.net>
#import <Foundation/NSDistributedNotificationCenter.h>
#import <Foundation/NSRaise.h>
#import <Foundation/NSRaiseException.h>
#import <Foundation/NSNotification.h>
#import <Foundation/NSString.h>
#import <Foundation/NSArray.h>
#import <Foundation/NSMutableArray.h>
#import <Foundation/NSDictionary.h>
#import <Foundation/NSMutableDictionary.h>
#import <Foundation/NSNumber.h>
#import <Foundation/NSData.h>
#import <Foundation/NSError.h>
#import <Foundation/NSPropertyList.h>
#import <Foundation/NSFileManager.h>
#import <Foundation/NSPathUtilities.h>
#import <Foundation/NSProcessInfo.h>
#import <Foundation/NSThread.h>
#import <Foundation/NSDate.h>

typedef enum {
   SuspensionBehaviorDrop=1,
   SuspensionBehaviorCoalesce=2,
   SuspensionBehaviorHold=3,
   SuspensionBehaviorDeliverImmediately=4
} SuspensionBehavior;

enum {
   OptionDeliverImmediately=1<<12,
   OptionPostToAllSessions=1<<17
};

enum {
   NSNotificationSuspensionBehaviorDrop=1,
   NSNotificationSuspensionBehaviorCoalesce=2,
   NSNotificationSuspensionBehaviorHold=3,
   NSNotificationSuspensionBehaviorDeliverImmediately=4
};

extern id objc_msgSend(id self, SEL op, ...);

static NSString *const DNCRootDirectoryName=@"ravynos-distributed-notifications";

@interface NSDistributedNotificationCenter () {
    NSString *_identifier;
    NSMutableArray *_entries;
    NSMutableArray *_queued;
    BOOL _suspended;
}
@end

@implementation NSDistributedNotificationCenter

static NSString *DNCRootDirectory() {
   NSString *root=[[NSTemporaryDirectory() stringByAppendingPathComponent:DNCRootDirectoryName] copy];

   [[NSFileManager defaultManager] createDirectoryAtPath:root withIntermediateDirectories:YES attributes:NULL error:NULL];

   return [root autorelease];
}

static NSString *DNCSubscriptionDirectory() {
   NSString *directory=[DNCRootDirectory() stringByAppendingPathComponent:@"subscriptions"];

   [[NSFileManager defaultManager] createDirectoryAtPath:directory withIntermediateDirectories:YES attributes:NULL error:NULL];

   return directory;
}

static NSString *DNCInboxDirectory(NSString *identifier) {
   NSString *directory=[[DNCRootDirectory() stringByAppendingPathComponent:@"inboxes"] stringByAppendingPathComponent:identifier];

   [[NSFileManager defaultManager] createDirectoryAtPath:directory withIntermediateDirectories:YES attributes:NULL error:NULL];

   return directory;
}

+(NSDistributedNotificationCenter *)notificationCenterForType:(NSString *)type {
   if(type==nil || ![type isEqualToString:NSLocalNotificationCenterType])
      NSRaiseException(NSGenericException,nil,0,@"unsupported notification center type");

   static NSDistributedNotificationCenter *localCenter=nil;

   if(localCenter==nil) {
      localCenter=[[NSDistributedNotificationCenter alloc] init];

      [NSThread detachNewThreadSelector:@selector(pollNotifications:) toTarget:localCenter withObject:nil];
   }

   return localCenter;
}

-initWithEntries {
   if((self=[super init])!=nil) {
      _identifier=[[NSProcessInfo processInfo] globallyUniqueString];
      _entries=[[NSMutableArray alloc] init];
      _queued=[[NSMutableArray alloc] init];
      _suspended=NO;
   }

   return self;
}

-(void)dealloc {
   [_identifier release];
   [_entries release];
   [_queued release];
   [super dealloc];
}

-(void)writeSubscriptions {
   NSMutableArray *plist=[NSMutableArray array];

   @synchronized(_entries) {
      for(NSUInteger count=[_entries count],index=0;index<count;index++) {
         NSDictionary *entry=[_entries objectAtIndex:index];
         NSMutableDictionary *copy=[NSMutableDictionary dictionary];

         if([entry objectForKey:@"name"]!=nil)
            [copy setObject:[entry objectForKey:@"name"] forKey:@"name"];

         if([entry objectForKey:@"object"]!=nil)
            [copy setObject:[entry objectForKey:@"object"] forKey:@"object"];

         [copy setObject:[entry objectForKey:@"behavior"] forKey:@"behavior"];

         [plist addObject:copy];
      }
   }

   NSData *data=[NSPropertyListSerialization dataFromPropertyList:plist format:NSPropertyListBinaryFormat_v1_0 errorDescription:NULL];

   [data writeToFile:[DNCSubscriptionDirectory() stringByAppendingPathComponent:_identifier] atomically:YES];
}

-(void)addObserver:observer selector:(SEL)selector name:(NSString *)name object:(NSString *)object suspensionBehavior:(NSNotificationSuspensionBehavior)behavior {
   if(observer==nil || selector==NULL)
      NSRaiseException(NSGenericException,nil,0,@"addObserver: needs observer and selector");

   NSMutableDictionary *entry=[NSMutableDictionary dictionary];

   [entry setObject:observer forKey:@"observer"];
   [entry setObject:NSStringFromSelector(selector) forKey:@"selector"];

   if(name!=nil)
      [entry setObject:name forKey:@"name"];

   if(object!=nil)
      [entry setObject:object forKey:@"object"];

   [entry setObject:[NSNumber numberWithInt:behavior] forKey:@"behavior"];

   @synchronized(_entries) {
      [_entries addObject:entry];
   }

   [self writeSubscriptions];
}

-(void)addObserver:observer selector:(SEL)selector name:(NSString *)name object:(NSString *)object {
   [self addObserver:observer selector:selector name:name object:object suspensionBehavior:NSNotificationSuspensionBehaviorCoalesce];
}

-(void)removeObserver:observer name:(NSString *)name object:(NSString *)object {
   @synchronized(_entries) {
      NSMutableArray *doomed=[NSMutableArray array];

      for(NSUInteger count=[_entries count],index=0;index<count;index++) {
         NSDictionary *entry=[_entries objectAtIndex:index];

         if([entry objectForKey:@"observer"]!=observer)
            continue;

         if(name!=nil && [name isEqualToString:[entry objectForKey:@"name"]]==NO)
            continue;

         if(object!=nil && [object isEqualToString:[entry objectForKey:@"object"]]==NO)
            continue;

         [doomed addObject:entry];
      }

      [_entries removeObjectsInArray:doomed];
   }

   [self writeSubscriptions];
}

-(void)removeObserver:observer {
   [self removeObserver:observer name:nil object:nil];
}

-(BOOL)suspended {
   return _suspended;
}

-(void)setSuspended:(BOOL)flag {
   _suspended=flag;

   if(!flag && [_queued count]>0) {
      NSArray *pending=nil;

      @synchronized(_queued) {
         pending=[NSArray arrayWithArray:_queued];

         [_queued removeAllObjects];
      }

      for(NSUInteger count=[pending count],index=0;index<count;index++)
         [self deliverEvent:[pending objectAtIndex:index]];
   }
}

-(void)deliverEvent:(NSDictionary *)event {
   NSString *name=[event objectForKey:@"name"];
   NSString *object=[event objectForKey:@"object"];
   NSDictionary *userInfo=[event objectForKey:@"userInfo"];
   BOOL immediate=[[event objectForKey:@"immediate"] boolValue];
   NSNotification *notification=[NSNotification notificationWithName:name object:object userInfo:userInfo];
   NSMutableArray *snapshot=[NSMutableArray array];

   @synchronized(_entries) {
      [snapshot addObjectsFromArray:_entries];
   }

   for(NSUInteger count=[snapshot count],index=0;index<count;index++) {
      NSDictionary *entry=[snapshot objectAtIndex:index];

      if([entry objectForKey:@"observer"]==nil)
         continue;

      NSString *entryName=[entry objectForKey:@"name"];

      if(entryName!=nil && [entryName isEqualToString:name]==NO)
         continue;

      NSString *entryObject=[entry objectForKey:@"object"];

      if(entryObject!=nil && [entryObject isEqualToString:object]==NO)
         continue;

      int behavior=[[entry objectForKey:@"behavior"] intValue];

      if(_suspended && !immediate && behavior!=SuspensionBehaviorDeliverImmediately) {
         if(behavior==SuspensionBehaviorDrop)
            continue;

         if(behavior==SuspensionBehaviorCoalesce) {
            NSMutableArray *doomed=[NSMutableArray array];

            @synchronized(_queued) {
               for(NSUInteger queuedCount=[_queued count],queuedIndex=0;queuedIndex<queuedCount;queuedIndex++) {
                  NSDictionary *queued=[_queued objectAtIndex:queuedIndex];

                  if([name isEqualToString:[queued objectForKey:@"name"]] &&
                     ((object==nil && [queued objectForKey:@"object"]==nil) ||
                      (object!=nil && [object isEqualToString:[queued objectForKey:@"object"]]))) {
                     [doomed addObject:queued];
                  }
               }

               [_queued removeObjectsInArray:doomed];
               [_queued addObject:event];
            }
         } else {
            @synchronized(_queued) {
               [_queued addObject:event];
            }
         }

         continue;
      }

      SEL selector=NSSelectorFromString([entry objectForKey:@"selector"]);

      ((void (*)(id,SEL,id))objc_msgSend)([entry objectForKey:@"observer"],selector,notification);
   }
}

-(void)pollNotifications:ignored {
   NSString *inbox=DNCInboxDirectory(_identifier);

   while(YES) {
      [NSThread sleepForTimeInterval:0.1];

      NSArray *names=[[NSFileManager defaultManager] contentsOfDirectoryAtPath:inbox error:NULL];

      if([names count]==0)
         continue;

      names=[names sortedArrayUsingSelector:@selector(compare:)];

      for(NSUInteger count=[names count],index=0;index<count;index++) {
         NSString *path=[inbox stringByAppendingPathComponent:[names objectAtIndex:index]];
         NSData *data=[NSData dataWithContentsOfFile:path];

         if(data==nil)
            continue;

         NSDictionary *event=[NSPropertyListSerialization propertyListFromData:data mutabilityOption:NSPropertyListImmutable format:NULL errorDescription:NULL];

         [self deliverEvent:event];
         [[NSFileManager defaultManager] removeItemAtPath:path error:NULL];
      }
   }
}

-(void)postNotificationName:(NSString *)name object:(NSString *)object userInfo:(NSDictionary *)userInfo deliverImmediately:(BOOL)deliverImmediately options:(NSUInteger)options {
   if(name==nil || [name length]==0)
      NSRaiseException(NSGenericException,nil,0,@"postNotificationName: nil name");

   if(userInfo!=nil && [NSPropertyListSerialization dataFromPropertyList:userInfo format:NSPropertyListBinaryFormat_v1_0 errorDescription:NULL]==nil)
      NSRaiseException(NSInvalidArgumentException,nil,0,@"userInfo is not property list serializable");

   BOOL immediate=deliverImmediately || (options&OptionDeliverImmediately)!=0;
   NSMutableDictionary *event=[NSMutableDictionary dictionary];

   [event setObject:name forKey:@"name"];

   if(object!=nil)
      [event setObject:object forKey:@"object"];

   if(userInfo!=nil)
      [event setObject:userInfo forKey:@"userInfo"];

   [event setObject:[NSNumber numberWithBool:immediate] forKey:@"immediate"];

   NSData *data=[NSPropertyListSerialization dataFromPropertyList:event format:NSPropertyListBinaryFormat_v1_0 errorDescription:NULL];
   NSString *inboxDirectory=[DNCRootDirectory() stringByAppendingPathComponent:@"inboxes"];
   NSArray *identifiers=[[NSFileManager defaultManager] contentsOfDirectoryAtPath:inboxDirectory error:NULL];
   NSString *fileName=[NSString stringWithFormat:@"%@-%@",[[NSDate date] timeIntervalSinceReferenceDate],[[NSProcessInfo processInfo] globallyUniqueString]];

   for(NSUInteger count=[identifiers count],index=0;index<count;index++) {
      NSString *identifier=[identifiers objectAtIndex:index];

      if([identifier hasSuffix:@".plist"]==NO)
         continue;

      [data writeToFile:[[inboxDirectory stringByAppendingPathComponent:[identifier stringByDeletingPathExtension]] stringByAppendingPathComponent:fileName] atomically:YES];
   }
}

-(void)postNotificationName:(NSString *)name object:(NSString *)object userInfo:(NSDictionary *)userInfo {
   [self postNotificationName:name object:object userInfo:userInfo deliverImmediately:NO options:0];
}

-(void)postNotificationName:(NSString *)name object:(NSString *)object {
   [self postNotificationName:name object:object userInfo:nil];
}

@end
