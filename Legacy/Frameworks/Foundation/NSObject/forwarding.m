#import <runtime/objc.h>
#import <Foundation/NSObject.h>
#import <Foundation/NSInvocation.h>
#import <Foundation/NSRaiseException.h>
#import <strings.h>
#include <Foundation/NSMethodSignature.h>
#include <Foundation/NSException.h>
#include <stdio.h>

#define NSABISizeofRegisterReturn 8
#if defined(__x86_64__)
#define NSABIasm_jmp_objc_msgSend __asm__("jmp _objc_msgSend")
#define NSABIasm_jmp_objc_msgSend_stret __asm__("jmp _objc_msgSend_stret")
#else
/* aarch64: b _objc_msgSend; the arm64 branch/ret ABI makes the tail call valid */
#define NSABIasm_jmp_objc_msgSend __asm__("b _objc_msgSend")
#define NSABIasm_jmp_objc_msgSend_stret __asm__("b _objc_msgSend")
#endif
/* SysV x86-64 caller-saved regs the tail-jump may have disturbed.
 * aarch64: x0-x17 are caller-saved; a bare compiler barrier suffices. */
#if defined(__x86_64__)
#define NSABIForwardClobberAsm asm volatile("" ::: "memory", "rdx", "rcx", "r8", "r9")
#elif defined(__aarch64__) || defined(__arm64__)
#define NSABIForwardClobberAsm asm volatile("" ::: "memory")
#else
#define NSABIForwardClobberAsm asm volatile("" ::: "memory")
#endif

static void OBJCRaiseException(const char *name,const char *format,...) {
    va_list arguments;
    
    va_start(arguments,format);
    
    fprintf(stderr,"ObjC:%s:",name);
    vfprintf(stderr,format,arguments);
    fprintf(stderr,"\n");
    fflush(stderr);
    va_end(arguments);
}


#if !FOUNDATION_DISALLOW_FORWARDING
@interface NSObject(fastforwarding)
-forwardingTargetForSelector:(SEL)selector;
@end

@interface NSInvocation(_private)
+(NSInvocation *)invocationWithMethodSignature:(NSMethodSignature *)signature arguments:(void *)arguments;
@end

id NSObjCGetFastForwardTarget(id object,SEL selector){
   id check=nil;

   if([object respondsToSelector:@selector(forwardingTargetForSelector:)])
    if((check=[object forwardingTargetForSelector:selector])==object)
     check=nil;

   return check;
}
#endif

void NSObjCForwardInvocation(void *returnValue,id object,SEL selector,va_list arguments){
   NSMethodSignature *signature=[object methodSignatureForSelector:selector];
   if(signature==nil)
    [object doesNotRecognizeSelector:selector];
   else {
#if FOUNDATION_DISALLOW_FORWARDING
    Class class = object_getClass(object);
    OBJCRaiseException("ForwardingDisallowed", "%c[%s %s(%d)]", class_isMetaClass(class) ? '+' : '-', class_getName(class) , sel_getName(selector), selector);
#endif

    /* FIXME: Convert the va_list to the packed arg frame needed by NSInvocation.
     * This is ugly af. We should change the handful of call sites to use a
     * packed arg frame and void * passing. There's no need for va_list.
     */
    int frameLength = [signature frameLength];
    uint8_t *packedFrame = __builtin_alloca(frameLength);
    memset(packedFrame, 0, frameLength);
    *(id *)packedFrame = object;
    *(SEL *)(packedFrame + sizeof(id)) = selector;

    int argc = [signature numberOfArguments];
    for (int i = 2; i < argc; ++i) {
        int size = 0, align = 0;
        const char *type = [signature getArgumentTypeAtIndex:i];
        NSGetSizeAndAlignment(type, &size, &align);
        int offset = [signature _argumentOffsetAtIndex:i];

        if (type[0] == 'f' || type[0] == 'd') { // float/double in FP registers
            double val = va_arg(arguments, double);
            memcpy(packedFrame + offset, &val, size);
        } else if (size <= sizeof(uintptr_t)) { // anything else is a pointer
            uintptr_t val = va_arg(arguments, uintptr_t);
            memcpy(packedFrame + offset, &val, size);
        } else {
            NSRaiseException(NSInvalidArgumentException, object, selector,
                @"Unhandled argument type %s size %d in NSObjCForwardInvocation", type, size);
        }
    }

    NSInvocation *invocation=[NSInvocation invocationWithMethodSignature:signature arguments:packedFrame];
    [invocation setTarget:object];

    [object forwardInvocation:invocation];
    [invocation getReturnValue:returnValue];
   }
}

static inline id __attribute__((always_inline)) __fastForward(id obj, SEL _cmd) {
    id check = NSObjCGetFastForwardTarget(obj, _cmd);
    if (!check)
        return nil;
    obj = check;
    NSABIasm_jmp_objc_msgSend;
}

void NSObjCForward(id object,SEL selector,...){
   __fastForward(object, selector);

   // Erect a barrier here by telling clang we clobbered the regs
   NSABIForwardClobberAsm;

   va_list arguments;
   va_start(arguments,selector);
   uint8_t returnValue[NSABISizeofRegisterReturn] __attribute__((aligned(16)));
   NSObjCForwardInvocation(returnValue,object,selector,arguments);
   va_end(arguments);
}

void NSObjCForward_stret(void *returnValue,id object,SEL selector,...){
   __fastForward(object, selector);

   // Erect a barrier here by telling clang we clobbered the regs
   NSABIForwardClobberAsm;

   va_list arguments;
   va_start(arguments,selector);
   NSObjCForwardInvocation(returnValue,object,selector,arguments);
   va_end(arguments);
}


