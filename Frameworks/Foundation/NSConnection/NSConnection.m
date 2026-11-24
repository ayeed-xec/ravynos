/* Copyright (c) 2006-2007 Christopher J. W. Lloyd

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */
#import <Foundation/NSConnection.h>
#import <Foundation/NSPortNameServer.h>
#import <Foundation/NSPort.h>
#import <Foundation/NSString.h>
#import <Foundation/NSArray.h>
#import <Foundation/NSDictionary.h>
#import <Foundation/NSRaise.h>
#import <Foundation/NSMessagePort.h>
#import <Foundation/NSThread.h>
#import <Foundation/NSDistantObject.h>

NSString * const NSConnectionReplyMode=@"NSConnectionReplyMode";

@implementation NSConnection

static NSMutableArray *_NSConnectionAllConnections=nil;

+(NSMutableArray *)_allConnections {
    if(_NSConnectionAllConnections==nil)
        _NSConnectionAllConnections=[[NSMutableArray alloc] init];

    return _NSConnectionAllConnections;
}

+(NSArray *)allConnections {
    return [NSArray arrayWithArray:[self _allConnections]];
}

+(NSConnection *)defaultConnection {
    NSArray *all=[self _allConnections];

    if([all count]>0)
        return [all objectAtIndex:0];

    NSPort *port=[[[NSMessagePort alloc] init] autorelease];

    return [self connectionWithReceivePort:port sendPort:port];
}

-initWithReceivePort:(NSPort *)receivePort sendPort:(NSPort *)sendPort {
   [super init];

   _receivePort=[receivePort retain];
   _sendPort=[sendPort retain];
   _isValid=YES;
   _localObjects=[[NSMutableArray alloc] init];
   _remoteObjects=[[NSMutableArray alloc] init];

   [[NSConnection _allConnections] addObject:self];

   return self;
}

-(void)dealloc {
   [[NSConnection _allConnections] removeObject:self];

   [_receivePort release];
   [_sendPort release];
   [_localObjects release];
   [_remoteObjects release];
   [_runLoops release];
   [_requestModes release];
   [_registeredName release];
   [super dealloc];
}

+(NSConnection *)connectionWithReceivePort:(NSPort *)receivePort sendPort:(NSPort *)sendPort {
   return [[[self alloc] initWithReceivePort:receivePort sendPort:sendPort] autorelease];
}

+(NSConnection *)connectionWithRegisteredName:(NSString *)name host:(NSString *)hostName usingNameServer:(NSPortNameServer *)nameServer {
   NSPort *port=[nameServer portForName:name host:hostName];

   if(port==nil)
      return nil;

   return [self connectionWithReceivePort:nil sendPort:port];
}

+(NSConnection *)connectionWithRegisteredName:(NSString *)name host:(NSString *)hostName {
   return [self connectionWithRegisteredName:name host:hostName usingNameServer:[NSPortNameServer systemDefaultPortNameServer]];
}

+(NSDistantObject *)rootProxyForConnectionWithRegisteredName:(NSString *)name host:(NSString *)hostName usingNameServer:(NSPortNameServer *)nameServer {
   return [[self connectionWithRegisteredName:name host:hostName usingNameServer:nameServer] rootProxy];
}

+(NSDistantObject *)rootProxyForConnectionWithRegisteredName:(NSString *)name host:(NSString *)hostName {
   return [[self connectionWithRegisteredName:name host:hostName] rootProxy];
}

+currentConversation {
   return [[[[self allConnections] firstObject] retain] autorelease];
}

-delegate {
   return _delegate;
}

-(BOOL)isValid {
   return _isValid;
}

-(BOOL)independentConversationQueueing {
   return _independentConversationQueueing;
}

-(BOOL)multipleThreadsEnabled {
   return _multipleThreadsEnabled;
}

-(NSTimeInterval)replyTimeout {
   return _replyTimeout;
}

-(NSTimeInterval)requestTimeout {
   return _requestTimeout;
}

-(NSPort *)sendPort {
   return _sendPort;
}

-(NSPort *)receivePort {
   return _receivePort;
}

-(NSArray *)requestModes {
   return _requestModes;
}

-rootObject {
   return _rootObject;
}

-(NSDistantObject *)rootProxy {
   if(!_isValid || _rootObject==nil)
      return nil;

   return [NSDistantObject proxyWithTarget:_rootObject connection:self];
}

-(NSArray *)localObjects {
   return [NSArray arrayWithArray:_localObjects];
}

-(NSArray *)remoteObjects {
   return [NSArray arrayWithArray:_remoteObjects];
}

-(void)setDelegate:delegate {
   _delegate=delegate;
}

-(void)invalidate {
   _isValid=NO;

   [[self class] _allConnections];

   [_registeredName release];
   _registeredName=nil;
}

-(void)setIndependentConversationQueueing:(BOOL)flag {
   _independentConversationQueueing=flag;
}

-(void)enableMultipleThreads {
   _multipleThreadsEnabled=YES;
}

-(void)setReplyTimeout:(NSTimeInterval)seconds {
   _replyTimeout=seconds;
}

-(void)setRequestTimeout:(NSTimeInterval)seconds {
   _requestTimeout=seconds;
}

-(void)addRequestMode:(NSString *)mode {
   if(_requestModes==nil)
      _requestModes=[[NSMutableArray alloc] init];

   [_requestModes addObject:mode];
}

-(void)removeRequestMode:(NSString *)mode {
   [_requestModes removeObject:mode];
}

-(void)setRootObject:rootObject {
   [_rootObject release];
   _rootObject=[rootObject retain];
}

-(void)runInNewThread {
   [NSThread detachNewThreadSelector:@selector(run) toTarget:self withObject:nil];
}

-(void)addRunLoop:(NSRunLoop *)runLoop {
   if(_runLoops==nil)
      _runLoops=[[NSMutableArray alloc] init];

   [_runLoops addObject:runLoop];
}

-(void)removeRunLoop:(NSRunLoop *)runLoop {
   [_runLoops removeObject:runLoop];
}

-(BOOL)registerName:(NSString *)name withNameServer:(NSPortNameServer *)nameServer {
   if(name==nil)
      return NO;

   BOOL registered=[nameServer registerPort:_receivePort name:name];

   if(registered) {
      [_registeredName release];
      _registeredName=[name retain];
   }

   return registered;
}

-(BOOL)registerName:(NSString *)name {
   return [self registerName:name withNameServer:[NSPortNameServer systemDefaultPortNameServer]];
}

-(NSDictionary *)statistics {
   return _statistics;
}

@end
