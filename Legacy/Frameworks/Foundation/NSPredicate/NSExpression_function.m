/* Copyright (c) 2007 Christopher J. W. Lloyd

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */
#import "NSExpression_function.h"
#import <Foundation/NSString.h>
#import <Foundation/NSArray.h>
#import <Foundation/NSNumber.h>
#import <Foundation/NSRaise.h>
#import <math.h>

@implementation NSExpression_function

-initWithName:(NSString *)name arguments:(NSArray *)arguments {
   [super initWithExpressionType:NSFunctionExpressionType];
   _name=[name copy];
   _arguments=[arguments retain];
   return self;
}

-(void)dealloc {
   [_name release];
   [_arguments release];
   [super dealloc];
}

-(NSString *)function {
   return _name;
}

-(NSArray *)arguments {
   return _arguments;
}

-expressionValueWithObject:object context:(NSMutableDictionary *)context {
   NSMutableArray *values=[NSMutableArray array];
   NSEnumerator *argEnum=[_arguments objectEnumerator];
   NSExpression *argument;
   NSString *name=[_name lowercaseString];

   while((argument=[argEnum nextObject])!=nil){
      id value=[argument expressionValueWithObject:object context:context];

      [values addObject:value];
   }

   if([name isEqualToString:@"count:"]){
      id collection=[values objectAtIndex:0];

      return [[NSNumber numberWithUnsignedInteger:[collection count]] autorelease];
   }
   if([name isEqualToString:@"sum:"] || [name isEqualToString:@"average:"]
    ||[name isEqualToString:@"min:"] || [name isEqualToString:@"max:"]){
      id collection=[values objectAtIndex:0];
      double total=0, best=0;
      BOOL first=YES;
      NSEnumerator *itemEnum=[collection objectEnumerator];
      id item;

      while((item=[itemEnum nextObject])!=nil){
         double itemValue=[item doubleValue];

         if(first || (itemValue>best && [name isEqualToString:@"max:"]))
          best=itemValue;
         if(first || (itemValue<best && [name isEqualToString:@"min:"]))
          best=itemValue;
         total+=itemValue;
         first=NO;
      }

      if([name isEqualToString:@"sum:"])
       return [[NSNumber numberWithDouble:total] autorelease];
      if([name isEqualToString:@"average:"])
       return [[NSNumber numberWithDouble:total/[collection count]] autorelease];
      return [[NSNumber numberWithDouble:best] autorelease];
   }
   if([name isEqualToString:@"sqrt:"])
    return [[NSNumber numberWithDouble:sqrt([[values objectAtIndex:0] doubleValue])] autorelease];
   if([name isEqualToString:@"log:"])
    return [[NSNumber numberWithDouble:log([[values objectAtIndex:0] doubleValue])] autorelease];

   [NSException raise:NSInvalidArgumentException format:@"unsupported function %@",_name];
   return nil;
}

-(NSString *)description {
   NSMutableString *result=[NSMutableString string];
   NSInteger        i,count=[_arguments count];
      
   [result appendFormat:@"%@(",_name];
   for(i=0;i<count;i++)
    [result appendFormat:@"%@%s",[_arguments objectAtIndex:i],(i+1<count)?",":""];
   [result appendFormat:@")"];
   
   return result;
}

-(NSExpression *)_expressionWithSubstitutionVariables:(NSDictionary *)variables {
   NSMutableArray *array=[NSMutableArray array];
   NSInteger       i,count=[_arguments count];
      
   for(i=0;i<count;i++)
    [array addObject:[[_arguments objectAtIndex:i] _expressionWithSubstitutionVariables:variables]];

   return [NSExpression expressionForFunction:_name arguments:array];
}
@end
