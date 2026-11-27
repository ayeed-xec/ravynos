/* Copyright (c) 2006-2007 Christopher J. W. Lloyd <cjwl@objc.net>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */
#include <sys/param.h>
#import <Foundation/NSString.h>
#import <Foundation/NSRaise.h>
#import <Foundation/NSStringHashing.h>
#import <Foundation/NSRaiseException.h>
#import <Foundation/NSStringUTF8.h>

// clang support:
// Tiny strings are only used on 64-bit platforms.  They store 8 7-bit
// ASCII characters in the high 56 bits, followed by a 4-bit length and a
// 3-bit tag (which is always 4).

// clang with GNU runtime stores NSConstantString as either ASCII or UTF-16
// we use the GNUstep 2.0 runtime (libobjc2) which uses the following class
//  struct
//  {
//    Class isa;
//    uint32_t flags;
//    uint32_t length; // Number of codepoints
//    uint32_t size; // Number of bytes
//    uint32_t hash;
//    const char *data;
//  };


@implementation NSConstantString(Impl)

/* clang emits constant-string objects in exactly this layout (see the
 * struct comment above): 8 bytes of runtime header, then the fields.
 * Accessing the fields through the class's ivar offsets would read the
 * header-based layout instead, which is 8 bytes further back. */
typedef struct {
    Class isa;
    uint32_t flags;
    uint32_t length;
    uint32_t size;
    uint32_t hash;
    const char *data;
} NSConstantStringObject;

static inline NSConstantStringObject *NSConstantStringObjectFromObject(id object){
    return object;
}

static inline uint32_t constantStringLength(NSConstantStringObject *object){
    return object->length & 0x0fffffff;
}

static inline const char *constantStringBytes(NSConstantStringObject *object){
    return object->data;
}


-copy {
   return self;
}

-copyWithZone:(NSZone *)zone {
   return self;
}

-retain {
   return self;
}

-(void)release {
}

-autorelease {
   return self;
}

-(NSUInteger)retainCount {
   return UINT_MAX;
}

-(void)dealloc {
   return;
   [super dealloc];
}

-(NSUInteger)length {
   /* Clang's gnustep-2.0 constant strings store the length and are not
    * NUL-terminated, so use the field instead of scanning the bytes. */
   return constantStringLength(NSConstantStringObjectFromObject(self));
}

-(NSUInteger)lengthOfBytesUsingEncoding:(NSStringEncoding)encoding {
    switch (encoding) {
        case NSUTF8StringEncoding:
        case NSASCIIStringEncoding:
	    return constantStringLength(NSConstantStringObjectFromObject(self));
        default:
            [NSException raise:NSInvalidArgumentException format:@"unsupported string encoding %i",encoding];
            return 0;
    }
}

-(unichar)characterAtIndex:(NSUInteger)location {
   NSConstantStringObject *object = NSConstantStringObjectFromObject(self);
   NSUInteger length = constantStringLength(object);
   const char *bytes = constantStringBytes(object);
   unichar result;

   if(location>=length)
    NSRaiseException(NSRangeException,self,_cmd,@"index %d beyond length %d",location,length);

   if(((unsigned char)bytes[0] & 0xC0) == 0xC0) {
    NSUInteger i = 0;

    for(NSUInteger u = 0; u < location; u++) {
     do {
      i++;
     } while((bytes[i] & 0xC0) == 0x80);
    }

    NSUInteger width = 1;

    while((bytes[i + width] & 0xC0) == 0x80)
     width++;

    switch(width) {
        case 2:
         result = ((bytes[i] & 0x1F) << 6) | (bytes[i + 1] & 0x3F);
         break;
        case 3:
         result = ((bytes[i] & 0x0F) << 12) | ((bytes[i + 1] & 0x3F) << 6) | (bytes[i + 2] & 0x3F);
         break;
        default:
         result = '?';
         break;
    }
   }
   else {
    result = (unsigned char)bytes[location];
   }

   return result;
}

-(void)getCharacters:(unichar *)buffer {
   NSUInteger i,length=[self length];

   for(i=0;i<length;i++)
    buffer[i]=[self characterAtIndex:i];
}

-(void)getCharacters:(unichar *)buffer range:(NSRange)range {
   NSUInteger length=[self length];

   if(NSMaxRange(range)>length){
    NSRaiseException(NSRangeException,self,_cmd,@"range %@ beyond length %d",NSStringFromRange(range),length);
   }

   NSInteger i,loc=range.location,len=range.length;

   for(i=0;i<len;i++)
    buffer[i]=[self characterAtIndex:loc+i];
}

-(NSUInteger)hash {
   return NSStringHashASCII(constantStringBytes(NSConstantStringObjectFromObject(self)),
     MIN([self length],NSHashStringLength));
}

@end
