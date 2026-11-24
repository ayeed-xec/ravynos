/* Copyright (c) 2006-2007 Christopher J. W. Lloyd

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */
#import <Foundation/NSURLProtocol.h>
#import <Foundation/NSRaise.h>
#import <Foundation/NSArray.h>
#import <Foundation/NSMapTable.h>
#import <Foundation/NSMutableDictionary.h>
#import <Foundation/NSDictionary.h>
#import <Foundation/NSData.h>
#import <Foundation/NSURL.h>
#import <Foundation/NSURLRequest.h>
#import <Foundation/NSURLResponse.h>
#import <Foundation/NSCachedURLResponse.h>
#import <Foundation/NSError.h>
#import <Foundation/NSURLError.h>
#import "NSURLProtocol_http.h"

@implementation NSURLProtocol

static NSMutableArray *_registeredClasses=nil;

+(void)initialize {
   if(self==[NSURLProtocol class]){
    _registeredClasses=[NSMutableArray new];
    [_registeredClasses addObject:[NSURLProtocol_http class]];
   }
}

+(NSArray *)_registeredClasses {
   return _registeredClasses;
}

+(BOOL)registerClass:(Class)cls {
   [_registeredClasses addObject:cls];
   return YES;
}

+(Class)_URLProtocolClassForRequest:(NSURLRequest *)request {
   NSArray  *classes=[NSURLProtocol _registeredClasses];
   NSInteger count=[classes count];
   
   while(--count>=0){
    Class check=[classes objectAtIndex:count];
    
    if([check canInitWithRequest:request])
     return check;
   }
   return nil;
}

+(void)unregisterClass:(Class)cls {
   [_registeredClasses removeObjectIdenticalTo:cls];
}

+propertyForKey:(NSString *)key inRequest:(NSURLRequest *)request {
   if(request==nil || key==nil)
    return nil;

   return [(id)request propertyForKey:key];
}

+(void)removePropertyForKey:(NSString *)key inRequest:(NSMutableURLRequest *)request {
   [(id)request removePropertyForKey:key];
}

+(void)setProperty:value forKey:(NSString *)key inRequest:(NSMutableURLRequest *)request {
   [(id)request setProperty:value forKey:key];
}

+(BOOL)canInitWithRequest:(NSURLRequest *)request {
   return NO;
}

+(NSURLRequest *)canonicalRequestForRequest:(NSURLRequest *)request {
   return request;
}

+(BOOL)requestIsCacheEquivalent:(NSURLRequest *)request toRequest:(NSURLRequest *)other {
   return request==other || [request URL]==[other URL];
}

-initWithRequest:(NSURLRequest *)request cachedResponse:(NSCachedURLResponse *)response client:(id <NSURLProtocolClient>)client {
   _request=[request retain];
   _cachedResponse=[response retain];
   // Unknown whether the client should be retained
   // It will create a retain loop with NSURLConnection if it is, so if you retain it
   // Clear the loop at some point or itll leak
   _client=client;
   return self;
}

-(void)dealloc {
   [_request release];
   [_cachedResponse release];
   [super dealloc];
}

-(NSURLRequest *)request {
   return _request;
}

-(NSCachedURLResponse *)cachedResponse {
   return _cachedResponse;
}

-(id <NSURLProtocolClient>)client {
   return _client;
}

-(void)startLoading {
   NSUnimplementedMethod();
}

-(void)stopLoading {
   NSUnimplementedMethod();
}

@end
