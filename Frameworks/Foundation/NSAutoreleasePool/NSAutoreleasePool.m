/* Copyright (c) 2006-2007 Christopher J. W. Lloyd

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#import <Foundation/NSZone.h>
#import <Foundation/NSAutoreleasePool.h>
#import <Foundation/NSException.h>
#import <Foundation/NSRaise.h>
#import <Foundation/NSString.h>
#import <Foundation/NSThread-Private.h>
#import <Foundation/NSRaiseException.h>

@implementation NSAutoreleasePool

-init {
    self = [super init];
    _objects = [NSMutableArray new];
    _previousPool = NSThreadCurrentPool();
    NSThreadSetCurrentPool(self);
    return self;
}

-(void)dealloc {
    NSThreadSetCurrentPool(_previousPool);
    for(id object in _objects)
        [object release];
    [_objects release];
   [super dealloc];
}

id NSAutorelease(id object){
    return objc_autorelease(object);
}

-(void)drain {
    [self release];
}

-retain {
   [NSException raise:NSInvalidArgumentException format:@"-[NSAutoreleasePool retain] not allowed"];
   return nil;
}

-autorelease {
   [NSException raise:NSInvalidArgumentException format:@"-[NSAutoreleasePool autorelease] not allowed"];
   return nil;
}

// indicate this class is ARC compatible
-(void)_ARCCompatibleAutoreleasePool {
}

+(id)new {
    return [[self alloc] init];
}

+(void)addObject:(id)anObject {
    NSAutoreleasePool *pool = NSThreadCurrentPool();

    if(pool == nil) {
        fprintf(stderr, "*** autorelease called with no pool in place - object leaked\n");
        return;
    }
    [pool addObject:anObject];
}

-(void)addObject:(id)anObject {
    [_objects addObject:anObject];
}

@end
