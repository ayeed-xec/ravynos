/* Copyright (c) 2006-2007 Christopher J. W. Lloyd

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#import "NSArray_placeholder.h"
#import "NSArray_concrete.h"
#import <Foundation/NSRaise.h>

@implementation NSArray_placeholder


+allocWithZone:(NSZone *)zone {
   if(self==objc_lookUpClass("NSArray"))
    return NSAllocateObject([NSArray_placeholder class],0,NULL);

   return NSAllocateObject(self,0,zone);
}

- initWithObjects:(id *)objects count:(NSUInteger)count
{
    /* The placeholder was allocated unsized, so build a real concrete
     * array: the placeholder's own +alloc reference is consumed here and
     * +arrayWithObjects:count: autoreleases the returned object. */
    NSArray_concrete *concrete = NSArray_concreteNew([self zone], objects, count);

    [self release];
    return concrete;
}


-init
{
    return [self initWithObjects:NULL count:0];
}


@end
