/* Copyright (c) 2007 Christopher J. W. Lloyd

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#import <Foundation/NSDecimalNumber.h>
#import <Foundation/NSString.h>
#import <Foundation/NSRaise.h>

#import <stdio.h>
#import <string.h>
#import <math.h>

NSComparisonResult NSDecimalCompare(const NSDecimal *left,const NSDecimal *right);

NSString * const NSDecimalNumberDivideByZeroException=@"NSDecimalNumberDivideByZeroException";
NSString * const NSDecimalNumberUnderflowException=@"NSDecimalNumberUnderflowException";
NSString * const NSDecimalNumberOverflowException=@"NSDecimalNumberOverflowException";
NSString * const NSDecimalNumberExactnessException=@"NSDecimalNumberExactnessException";

static id <NSDecimalNumberBehaviors> _NSDecimalNumberDefaultBehavior=nil;

static NSDecimal NSDecimalNormalize(const NSDecimal *d,int targetExponent) {
    NSDecimal result=*d;

    while(result.exponent>targetExponent && result.mantissa/10*10==result.mantissa) {
        result.mantissa/=10;
        result.exponent--;
    }

    while(result.exponent<targetExponent) {
        if(result.mantissa>1844674407370955161ULL)
            break;

        result.mantissa*=10;
        result.exponent++;
    }

    return result;
}

static NSDecimal NSDecimalAddDecimal(const NSDecimal *left,const NSDecimal *right) {
    NSDecimal a=*left;
    NSDecimal b=*right;

    if(a.isNaN)
        return a;

    if(b.isNaN)
        return b;

    int target=a.exponent<b.exponent ? a.exponent : b.exponent;

    a=NSDecimalNormalize(&a,target);
    b=NSDecimalNormalize(&b,target);

    NSDecimal result;
    result.exponent=target;
    result.isNaN=0;

    if(a.negative==b.negative) {
        result.negative=a.negative;
        result.mantissa=a.mantissa+b.mantissa;
    } else {
        if(a.mantissa==b.mantissa) {
            result.negative=0;
            result.mantissa=0;
        } else if(a.mantissa>b.mantissa) {
            result.negative=a.negative;
            result.mantissa=a.mantissa-b.mantissa;
        } else {
            result.negative=b.negative;
            result.mantissa=b.mantissa-a.mantissa;
        }
    }

    return result;
}

static NSDecimal NSDecimalSubtractDecimal(const NSDecimal *left,const NSDecimal *right) {
    NSDecimal flipped=*right;

    flipped.negative=!flipped.negative;

    return NSDecimalAddDecimal(left,&flipped);
}

static NSDecimal NSDecimalMultiplyDecimal(const NSDecimal *left,const NSDecimal *right) {
    NSDecimal result;

    if(left->isNaN)
        return *left;

    if(right->isNaN)
        return *right;

    unsigned long long a=left->mantissa;
    unsigned long long b=right->mantissa;
    unsigned long long hi=a>>32;
    unsigned long long lo=a&0xFFFFFFFFULL;

    unsigned long long p1=hi*(b>>32);
    unsigned long long p2=hi*(b&0xFFFFFFFFULL);
    unsigned long long p3=lo*(b>>32);
    unsigned long long p4=lo*(b&0xFFFFFFFFULL);

    unsigned long long carry=p2+(p4>>32);
    if(carry<(p2&0xFFFFFFFFULL))
        carry=(1ULL<<32);

    unsigned long long overflow=(p1+(carry>>32))>>32;
    overflow+=p3>>32;

    if(overflow!=0 || p1>0 || (carry>>32)!=0) {
        result.isNaN=1;
        result.negative=0;
        result.mantissa=0;
        result.exponent=0;
        return result;
    }
    result.isNaN=0;
    result.negative=left->negative!=right->negative;
    result.mantissa=p4+(p2<<32)+(p3<<32);
    result.exponent=left->exponent+right->exponent;

    return result;
}

static NSDecimal NSDecimalDivideDecimal(const NSDecimal *left,const NSDecimal *right) {
    NSDecimal result;

    if(left->isNaN)
        return *left;

    if(right->isNaN)
        return *right;

    if(right->mantissa==0) {
        result.isNaN=1;
        result.negative=0;
        result.mantissa=0;
        result.exponent=0;
        return result;
    }

    unsigned long long dividend=left->mantissa;
    unsigned long long divisor=right->mantissa;
    int exponent=left->exponent-right->exponent;

    while(dividend/divisor<1000000000ULL && dividend<1844674407370955161ULL) {
        if(dividend>184467440737095516ULL)
            break;

        dividend*=10;
        exponent--;
    }
    result.isNaN=0;
    result.negative=left->negative!=right->negative;
    result.mantissa=dividend/divisor;
    result.exponent=exponent;

    return result;
}

@implementation NSDecimalNumber

-initWithDecimal:(NSDecimal)decimal {
   [super init];

   _decimal=decimal;

   return self;
}

-initWithMantissa:(uint64_t)mantissa exponent:(int16_t)exponent isNegative:(BOOL)flag {
   NSDecimal decimal;
   decimal.mantissa=mantissa;
   decimal.exponent=exponent;
   decimal.negative=flag;
   decimal.isNaN=0;

   return [self initWithDecimal:decimal];
}

-initWithString:(NSString *)string {
   return [self initWithString:string locale:nil];
}

-initWithString:(NSString *)string locale:(NSDictionary *)locale {
   const char *cstr=[string UTF8String];

   while(*cstr==' ')
      cstr++;

   NSDecimal decimal;
   decimal.isNaN=0;
   decimal.negative=0;
   decimal.mantissa=0;
   decimal.exponent=0;

   if(*cstr=='-') {
      decimal.negative=1;
      cstr++;
   } else if(*cstr=='+') {
      cstr++;
   }

   while(*cstr>='0' && *cstr<='9') {
      decimal.mantissa=decimal.mantissa*10+(unsigned long long)(*cstr-'0');
      cstr++;
   }

   if(*cstr=='.') {
      cstr++;

      while(*cstr>='0' && *cstr<='9') {
         if(decimal.mantissa<184467440737095516ULL) {
            decimal.mantissa=decimal.mantissa*10+(unsigned long long)(*cstr-'0');
            decimal.exponent--;
         }

         cstr++;
      }
   }

   return [self initWithDecimal:decimal];
}

+(NSDecimalNumber *)decimalNumberWithDecimal:(NSDecimal)decimal {
   return [[[self alloc] initWithDecimal:decimal] autorelease];
}

+(NSDecimalNumber *)decimalNumberWithMantissa:(uint64_t)mantissa exponent:(int16_t)exponent isNegative:(BOOL)negative {
   return [[[self alloc] initWithMantissa:mantissa exponent:exponent isNegative:negative] autorelease];
}

+(NSDecimalNumber *)decimalNumberWithString:(NSString *)string {
   return [[[self alloc] initWithString:string] autorelease];
}

+(NSDecimalNumber *)decimalNumberWithString:(NSString *)string locale:(NSDictionary *)locale {
   return [[[self alloc] initWithString:string locale:locale] autorelease];
}

+(NSDecimalNumber *)zero {
   return [self decimalNumberWithMantissa:0 exponent:0 isNegative:NO];
}

+(NSDecimalNumber *)one {
   return [self decimalNumberWithMantissa:1 exponent:0 isNegative:NO];
}

+(NSDecimalNumber *)minimumDecimalNumber {
   NSDecimal decimal;
   decimal.mantissa=18446744073709551615ULL;
   decimal.exponent=127;
   decimal.negative=1;
   decimal.isNaN=0;

   return [self decimalNumberWithDecimal:decimal];
}

+(NSDecimalNumber *)maximumDecimalNumber {
   NSDecimal decimal;
   decimal.mantissa=18446744073709551615ULL;
   decimal.exponent=127;
   decimal.negative=0;
   decimal.isNaN=0;

   return [self decimalNumberWithDecimal:decimal];
}

+(NSDecimalNumber *)notANumber {
   NSDecimal decimal;
   decimal.mantissa=0;
   decimal.exponent=0;
   decimal.negative=0;
   decimal.isNaN=1;

   return [self decimalNumberWithDecimal:decimal];
}

+(id <NSDecimalNumberBehaviors>)defaultBehavior {
   return _NSDecimalNumberDefaultBehavior;
}

+(void)setDefaultBehavior:(id <NSDecimalNumberBehaviors>)behavior {
   _NSDecimalNumberDefaultBehavior=behavior;
}

-(NSComparisonResult)compare:(NSNumber *)other {
   if([other isKindOfClass:[NSDecimalNumber class]]) {
      NSDecimal otherDecimal=((NSDecimalNumber *)other)->_decimal;

      return NSDecimalCompare(&_decimal,&otherDecimal);
   }

   double a=[self doubleValue];
   double b=[other doubleValue];

   if(a<b)
      return NSOrderedAscending;

   if(a>b)
      return NSOrderedDescending;

   return NSOrderedSame;
}

-(NSDecimalNumber *)decimalNumberByRoundingAccordingToBehavior:(id <NSDecimalNumberBehaviors>)behavior {
   return self;
}

-(NSDecimalNumber *)decimalNumberByAdding:(NSDecimalNumber *)other {
   return [self decimalNumberByAdding:other withBehavior:[[self class] defaultBehavior]];
}

-(NSDecimalNumber *)decimalNumberByAdding:(NSDecimalNumber *)other withBehavior:(id <NSDecimalNumberBehaviors>)behavior {
   return [[self class] decimalNumberWithDecimal:NSDecimalAddDecimal(&_decimal,&other->_decimal)];
}

-(NSDecimalNumber *)decimalNumberBySubtracting:(NSDecimalNumber *)other {
   return [self decimalNumberBySubtracting:other withBehavior:[[self class] defaultBehavior]];
}

-(NSDecimalNumber *)decimalNumberBySubtracting:(NSDecimalNumber *)other withBehavior:(id <NSDecimalNumberBehaviors>)behavior {
   return [[self class] decimalNumberWithDecimal:NSDecimalSubtractDecimal(&_decimal,&other->_decimal)];
}

-(NSDecimalNumber *)decimalNumberByMultiplyingBy:(NSDecimalNumber *)other {
   return [self decimalNumberByMultiplyingBy:other withBehavior:[[self class] defaultBehavior]];
}

-(NSDecimalNumber *)decimalNumberByMultiplyingBy:(NSDecimalNumber *)other withBehavior:(id <NSDecimalNumberBehaviors>)behavior {
   return [[self class] decimalNumberWithDecimal:NSDecimalMultiplyDecimal(&_decimal,&other->_decimal)];
}

-(NSDecimalNumber *)decimalNumberByDividingBy:(NSDecimalNumber *)other {
   return [self decimalNumberByDividingBy:other withBehavior:[[self class] defaultBehavior]];
}

-(NSDecimalNumber *)decimalNumberByDividingBy:(NSDecimalNumber *)other withBehavior:(id <NSDecimalNumberBehaviors>)behavior {
   return [[self class] decimalNumberWithDecimal:NSDecimalDivideDecimal(&_decimal,&other->_decimal)];
}

-(NSDecimalNumber *)decimalNumberByMultiplyingByPowerOf10:(int16_t)power {
   return [self decimalNumberByMultiplyingByPowerOf10:power withBehavior:[[self class] defaultBehavior]];
}

-(NSDecimalNumber *)decimalNumberByMultiplyingByPowerOf10:(int16_t)power withBehavior:(id <NSDecimalNumberBehaviors>)behavior {
   NSDecimal result=_decimal;

   result.exponent+=power;

   return [[self class] decimalNumberWithDecimal:result];
}

-(NSDecimalNumber *)decimalNumberByRaisingToPower:(NSUInteger)power {
   return [self decimalNumberByRaisingToPower:power withBehavior:[[self class] defaultBehavior]];
}

-(NSDecimalNumber *)decimalNumberByRaisingToPower:(NSUInteger)power withBehavior:(id <NSDecimalNumberBehaviors>)behavior {
   NSDecimalNumber *result=[self class];

   result=[(NSDecimalNumber *)[result one] retain];

   for(NSUInteger i=0;i<power;i++)
      result=[result decimalNumberByMultiplyingBy:self];

   return [result autorelease];
}

-(double)doubleValue {
   if(_decimal.isNaN)
      return NAN;

   double value=(double)_decimal.mantissa;

   for(int i=0;i<_decimal.exponent;i++)
      value*=10.0;

   for(int i=0;i<_decimal.exponent;i++)
      ;

   int exp=_decimal.exponent;

   if(exp<0) {
      for(int i=0;i<-exp;i++)
         value/=10.0;
   } else {
      for(int i=0;i<exp;i++)
         value*=10.0;
   }

   return _decimal.negative ? -value : value;
}

-(const char *)objCType {
   return @encode(double);
}

-(NSString *)descriptionWithLocale:(NSDictionary *)locale {
   return NSDecimalString(&_decimal,locale);
}

@end
