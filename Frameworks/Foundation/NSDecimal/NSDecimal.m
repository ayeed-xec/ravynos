/* Copyright (c) 2007 Christopher J. W. Lloyd

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#import <Foundation/NSObjCRuntime.h>
#import <Foundation/NSDecimal.h>
#import <Foundation/NSString.h>

#import <stdio.h>
#import <string.h>

void NSDecimalCopy(NSDecimal *destination,const NSDecimal *source) {
    *destination=*source;
}

NSComparisonResult NSDecimalCompare(const NSDecimal *left,const NSDecimal *right) {
    if(left->isNaN && right->isNaN)
        return NSOrderedSame;

    if(left->isNaN)
        return NSOrderedDescending;

    if(right->isNaN)
        return NSOrderedAscending;

    if(left->negative!=right->negative)
        return left->negative ? NSOrderedAscending : NSOrderedDescending;

    int cmp;

    if(left->exponent==right->exponent)
        cmp=(left->mantissa>right->mantissa)-(left->mantissa<right->mantissa);
    else
        cmp=left->exponent>right->exponent ? 1 : -1;

    if(left->negative)
        cmp=-cmp;

    return cmp;
}

NSString *NSDecimalString(const NSDecimal *decimal,id locale) {
    if(decimal->isNaN)
        return @"NaN";

    char digits[32];

    snprintf(digits,sizeof(digits),"%llu",(unsigned long long)decimal->mantissa);

    int length=(int)strlen(digits);
    int exponent=decimal->exponent;
    BOOL negative=decimal->negative;

    while(length>1 && digits[length-1]=='0') {
        length--;
        exponent++;
    }

    char buffer[64];
    char *out=buffer;

    if(negative)
        *out++='-';

    if(exponent>=0) {
        memcpy(out,digits,length);
        out+=length;

        for(int i=0;i<exponent;i++)
            *out++='0';
    } else {
        int point=length+exponent;

        if(point<=0) {
            *out++='0';
            *out++='.';

            for(int i=0;i<-point;i++)
                *out++='0';

            memcpy(out,digits,length);
            out+=length;
        } else {
            memcpy(out,digits,point);
            out+=point;
            *out++='.';
            memcpy(out,digits+point,length-point);
            out+=length-point;
        }
    }

    *out='\0';

    return [NSString stringWithCString:buffer];
}
