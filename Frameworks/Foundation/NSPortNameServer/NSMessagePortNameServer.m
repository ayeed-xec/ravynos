/* Copyright (c) 2006-2007 Christopher J. W. Lloyd

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

// Original - Christopher Lloyd <cjwl@objc.net>
#import <Foundation/NSMessagePortNameServer.h>
#import <Foundation/NSRaiseException.h>
#import <Foundation/NSString.h>
#import <Foundation/NSMutableDictionary.h>

@implementation NSMessagePortNameServer

static NSMutableDictionary *_NSMessagePortNameServerRegistry=nil;

+sharedInstance {
   static NSMessagePortNameServer *sharedInstance=nil;

   if(sharedInstance==nil)
      sharedInstance=[NSAllocateObject(self,0,NULL) init];

   return sharedInstance;
}

-(BOOL)registerPort:(NSPort *)port name:(NSString *)name {
   if(name==nil || [name length]==0)
      NSRaiseException(NSGenericException,nil,0,@"registerPort: needs a name");

   @synchronized([NSMessagePortNameServer class]) {
      if(_NSMessagePortNameServerRegistry==nil)
         _NSMessagePortNameServerRegistry=[[NSMutableDictionary alloc] init];

      [_NSMessagePortNameServerRegistry setObject:port forKey:name];
   }

   return YES;
}

-(BOOL)removePortForName:(NSString *)name {
   if(name==nil || [name length]==0)
      NSRaiseException(NSGenericException,nil,0,@"removePortForName: needs a name");

   BOOL removed=NO;

   @synchronized([NSMessagePortNameServer class]) {
      if(_NSMessagePortNameServerRegistry!=nil && [_NSMessagePortNameServerRegistry objectForKey:name]!=nil) {
         [_NSMessagePortNameServerRegistry removeObjectForKey:name];
         removed=YES;
      }
   }

   return removed;
}

-(NSPort *)portForName:(NSString *)name host:(NSString *)host {
   if(name==nil)
      NSRaiseException(NSGenericException,nil,0,@"portForName: nil name");

   NSPort *port=nil;

   @synchronized([NSMessagePortNameServer class]) {
      port=[_NSMessagePortNameServerRegistry objectForKey:name];
   }

   return port;
}

-(NSPort *)portForName:(NSString *)name {
   return [self portForName:name host:nil];
}

@end
