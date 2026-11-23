/* Copyright (c) 2006-2007 Christopher J. W. Lloyd
   Copyright (c) 2024-2026 Zoe Knox

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#import <Foundation/NSObject.h>
#import <Foundation/NSAutoreleasePool.h>
#import <Foundation/NSException.h>
#import <Foundation/NSHashTable.h>
#import <Foundation/NSObjCRuntime.h>
#import <Foundation/NSInvocation.h>
#import <Foundation/NSString.h>
#import <Foundation/NSAutoreleasePool-private.h>
#import <Foundation/NSMethodSignature.h>
#import <Foundation/NSProxy.h>
#import <Foundation/NSRaise.h>
#import <objc/runtime.h>
#import <objc/message.h>
#include <stdio.h>
#import "forwarding.h"

#include <CoreFoundation/CFRuntime.h>

/* zoe 2/10/26 - we now use NSObject from Apple's runtime. The remaining functions
 * here are additions or changes to the base class */

#if __LITTLE_ENDIAN__
#define CF_INFO_BITS 0
#define CF_RC_BITS 3
#else
#define CF_INFO_BITS 3
#define CF_RC_BITS 0
#endif

typedef void *malloc_zone_t;

// From Apple docs:
// Returns a Boolean value that indicates whether the receiver is an instance of given class
// or an instance of any class that inherits from that class.

BOOL NSObjectIsKindOfClass(id object,Class kindOf) {
   Class class=object_getClass(object);

	while (object_getClass(object_getClass(class)) != class) {

		if(kindOf == class) {
			return YES;
		}
		
		class = class_getSuperclass(class);
	}
	
   return NO;
}


@interface NSInvocation(private)
+(NSInvocation *)invocationWithMethodSignature:(NSMethodSignature *)signature arguments:(void *)arguments;
@end

/* From libobjc2's objc-arc.h (OBJC_NONPORTABLE API) */
extern size_t object_getRetainCount_np(id obj);
extern id objc_retain_fast_np(id obj);
extern void objc_release_fast_np(id obj);
extern id objc_autorelease(id obj);

/* The root class implementation. Under the gnustep-2.0 ABI only a main
 * @implementation emits the class metadata (._OBJC_CLASS_NSObject and
 * friends); categories on NSObject merely reference it, so without this
 * every Foundation object file has undefined class symbols and the
 * framework cannot link. libobjc2 does not provide NSObject itself. */
@implementation NSObject

+(void)load {
}

+(instancetype)new {
    return [[self alloc] init];
}

-(instancetype)init {
    return self;
}

+(id)copyWithZone:(NSZone *)zone {
    [self doesNotRecognizeSelector:_cmd];
    return nil;
}

+(id)mutableCopyWithZone:(NSZone *)zone {
    [self doesNotRecognizeSelector:_cmd];
    return nil;
}

-(id)copy {
    return [self copyWithZone:NULL];
}

-(id)mutableCopy {
    return [self mutableCopyWithZone:NULL];
}

+(Class)class {
    return self;
}

-(Class)class {
    return object_getClass(self);
}

+(Class)superclass {
    return class_getSuperclass(self);
}

-(Class)superclass {
    return class_getSuperclass(object_getClass(self));
}

-(instancetype)self {
    return self;
}

+(NSUInteger)hash {
    return (NSUInteger)(uintptr_t)self >> 4;
}

-(NSUInteger)hash {
    return (NSUInteger)(uintptr_t)self >> 4;
}

+(BOOL)isEqual:(id)other {
    return self==other;
}

-(BOOL)isEqual:(id)other {
    return self==other;
}

-(BOOL)isProxy {
    return NO;
}

-(BOOL)isKindOfClass:(Class)aClass {
    Class class=object_getClass(self);

    while(class) {
        if(class == aClass)
            return YES;
        class = class_getSuperclass(class);
    }
    return NO;
}

-(BOOL)isMemberOfClass:(Class)aClass {
    return object_getClass(self) == aClass;
}

+(BOOL)isSubclassOfClass:(Class)aClass {
    Class class=self;

    while(class) {
        if(class == aClass)
            return YES;
        class = class_getSuperclass(class);
    }
    return NO;
}

+(BOOL)instancesRespondToSelector:(SEL)aSelector {
    return class_getInstanceMethod(self,aSelector) != NULL;
}

+(BOOL)conformsToProtocol:(Protocol *)protocol {
    Class class=self;

    while(class) {
        if(class_conformsToProtocol(class,protocol))
            return YES;
        class = class_getSuperclass(class);
    }
    return NO;
}

-(BOOL)conformsToProtocol:(Protocol *)protocol {
    return [[self class] conformsToProtocol:protocol];
}

-(BOOL)respondsToSelector:(SEL)aSelector {
    return class_getInstanceMethod(object_getClass(self),aSelector) != NULL;
}

+(NSMethodSignature *)instanceMethodSignatureForSelector:(SEL)aSelector {
    Method method=class_getInstanceMethod(self,aSelector);

    if(method == NULL)
        return nil;
    return [NSMethodSignature signatureWithObjCTypes:method_getTypeEncoding(method)];
}

+(IMP)instanceMethodForSelector:(SEL)aSelector {
    IMP imp=class_getMethodImplementation(self,aSelector);

    if(imp == NULL)
        [self doesNotRecognizeSelector:aSelector];
    return imp;
}

-(IMP)methodForSelector:(SEL)aSelector {
    IMP imp=class_getMethodImplementation(object_getClass(self),aSelector);

    if(imp == NULL)
        [self doesNotRecognizeSelector:aSelector];
    return imp;
}

-(void)doesNotRecognizeSelector:(SEL)aSelector {
    [NSException raise:NSInvalidArgumentException
        format:@"%s does not recognize selector %s",
        object_getClassName(self),sel_getName(aSelector)];
}

-(id)forwardingTargetForSelector:(SEL)aSelector {
    return nil;
}

-(instancetype)retain {
    /* Call the runtime refcount primitive directly: objc_retain() would
     * dispatch back to -retain for non-ARC classes and recurse forever. */
    objc_retain_fast_np(self);
    return self;
}

-(oneway void)release {
    /* libobjc2 sends -dealloc when the reference count reaches zero */
    objc_release_fast_np(self);
}

-(NSUInteger)retainCount {
    return object_getRetainCount_np(self);
}

-(instancetype)autorelease {
    objc_autorelease(self);
    return self;
}

-(void)dealloc {
    NSDeallocateObject(self);
}

-(void)finalize {
}

-(struct _NSZone *)zone {
    return NSDefaultMallocZone();
}

-(NSString *)description {
    return [NSString stringWithFormat:@"<%@: %p>",[self className],self];
}

+(NSString *)description {
    return NSStringFromClass(self);
}

@end

@implementation NSObject (Foundation)
+(NSObject *)allocWithZone: (NSZone *)zone {
   return NSAllocateObject(self, 0, zone);
}

+(id)alloc {
    return NSAllocateObject(self, 0, NULL);
}

+ (NSMethodSignature *) methodSignatureForSelector:(SEL) sel
{
	Method m;
	m = class_getInstanceMethod(self, sel);
	if (m) {
		/* A real Objective-C method */
                char *p = method_getTypeEncoding(m);
		return [NSMethodSignature 
		    signatureWithObjCTypes:method_getTypeEncoding(m)];
	}

	[NSException raise:NSInvalidArgumentException 
		format:@"Class %s: no such selector: %s", 
		class_getName(self), sel_getName(sel)];
	return nil;
}

-(NSMethodSignature *) methodSignatureForSelector:(SEL) sel
{
	Class		   cls;
	Method		   m;
	cls = object_getClass(self);
	m = class_getInstanceMethod(cls, sel);
	if (m) {
		/* A real Objective-C method */
		return [NSMethodSignature 
		    signatureWithObjCTypes:method_getTypeEncoding(m)];
	}

	[NSException raise:NSInvalidArgumentException 
		format:@"Class %s: no such selector: %s", 
		class_getName(cls), sel_getName(sel)];
	return nil;
}

-(id)alloc {
    return NSAllocateObject(self, 0, NULL);
}

+(NSInteger)version {
   return class_getVersion(self);
}


+(void)setVersion:(NSInteger)version {
   class_setVersion(self,version);
}
extern id NSObjCForward(id self, SEL _cmd, ...);
extern id NSObjCForward_stret(id self, SEL _cmd, ...);
extern void objc_setForwardHandler(id fwd, id fwd_stret);

+(void)initialize {
    objc_setForwardHandler(NSObjCForward, NSObjCForward_stret);
}


+ (void)poseAsClass:(Class)aClass
{
    NSAutoreleasePool * pool = [NSAutoreleasePool new];
    NSUnimplementedMethod();
    [pool release];
}

-(uint32_t *)cfinfo {
   return (uint32_t *)(self->cfinfo);
}

-(Class)classForCoder {
   return object_getClass(self);
}

-(Class)classForArchiver {
   return [self classForCoder];
}

-(Class)classForKeyedArchiver {
	return [self classForCoder];
}

-replacementObjectForCoder:(NSCoder *)coder {
   return self;
}


-awakeAfterUsingCoder:(NSCoder *)coder {
   return self;
}

-(NSUInteger)_frameLengthForSelector:(SEL)selector {
   NSMethodSignature *signature=[self methodSignatureForSelector:selector];

   return [signature frameLength];
}

-(void)forwardInvocation:(NSInvocation *)invocation {
    id target = [invocation target];
    SEL sel = [invocation selector];
    printf("target %p %s sel %p %s\n", target, object_getClassName(target), sel, sel_getName(sel));
    [invocation invoke];
}

-(id)forwardSelector:(SEL)selector arguments:(void *)arguments {
   NSMethodSignature *signature=[self methodSignatureForSelector:selector];

   if(signature==nil){
    [self doesNotRecognizeSelector:selector];
    return nil;
   }
   else {
    NSInvocation *invocation=[NSInvocation invocationWithMethodSignature:signature arguments:arguments];
    char          result[[signature methodReturnLength]];
    //id            result;

    [self forwardInvocation:invocation];
    [invocation getReturnValue:&result];

   /* __builtin_return(result); */ //Can we use __builtin_return like this? It still doesn't seem to work on float/doubles ?
    return result;
   }
}

+(NSString *)className {
   return NSStringFromClass(self);
}

-(NSString *)className {
   return NSStringFromClass(object_getClass(self));
}

@end


#import <Foundation/NSCFTypeID.h>

@implementation NSObject (CFTypeID)

- (unsigned) _cfTypeID
{
   return kNSCFTypeObject;
}

@end
