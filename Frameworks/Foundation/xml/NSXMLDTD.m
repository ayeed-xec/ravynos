/* Copyright (c) 2007 Christopher J. W. Lloyd

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#import <Foundation/NSXMLDTD.h>
#import <Foundation/NSXMLDTDNode.h>
#import <Foundation/NSArray.h>
#import <Foundation/NSData.h>
#import <Foundation/NSDictionary.h>
#import <Foundation/NSString.h>
#import <Foundation/NSURL.h>
#import <Foundation/NSRaise.h>

static BOOL isSpaceCharacter(unichar c) {
   return c==' ' || c=='\t' || c=='\n' || c=='\r';
}

static NSRange tokenRangeAfter(NSString *string, NSUInteger from) {
   NSUInteger length=[string length], i=from;

   while(i<length && isSpaceCharacter([string characterAtIndex:i]))
      i++;

   NSUInteger start=i;

   while(i<length && !isSpaceCharacter([string characterAtIndex:i]))
      i++;

   return NSMakeRange(start,i-start);
}

static NSRange trimmedRange(NSString *string, NSRange range) {
   NSUInteger end=NSMaxRange(range);

   while(range.length>0 && isSpaceCharacter([string characterAtIndex:range.location])) {
      range.location++;
      range.length--;
   }

   while(end>range.location && isSpaceCharacter([string characterAtIndex:end-1]))
      end--;

   return NSMakeRange(range.location,end-range.location);
}

static NSString *tokenAt(NSString *string, NSRange range) {
   return [string substringWithRange:range];
}

static NSString *nextQuoted(NSString *string, NSRange *range) {
   NSUInteger length=NSMaxRange(*range);

   for(NSUInteger i=range->location;i<length;i++) {
      unichar c=[string characterAtIndex:i];

      if(c=='"' || c=='\'') {
         NSUInteger start=i+1;
         NSUInteger j=start;

         while(j<length && [string characterAtIndex:j]!=c)
            j++;

         *range=NSMakeRange(j+1,length-j-1);

         return [string substringWithRange:NSMakeRange(start,j-start)];
      }
   }

   *range=NSMakeRange(length,0);

   return nil;
}

static NSUInteger closingGreaterThan(NSString *string, NSUInteger from) {
   NSUInteger length=[string length];
   unichar quote=0;

   for(NSUInteger i=from;i<length;i++) {
      unichar c=[string characterAtIndex:i];

      if(quote!=0) {
         if(c==quote)
            quote=0;
      } else if(c=='"' || c=='\'') {
         quote=c;
      } else if(c=='>') {
         return i;
      }
   }

   return NSNotFound;
}

static void applyExternalIDs(NSString *body, NSRange range, NSXMLDTDNode *node) {
   NSRange first=tokenRangeAfter(body,range.location);

   if(first.length==0)
      return;

   NSString *keyword=tokenAt(body,first);

   if([keyword isEqualToString:@"PUBLIC"]) {
      NSRange scan=first;

      scan.location=NSMaxRange(first);
      scan.length=[body length]-scan.location;

      [node setPublicID:nextQuoted(body,&scan)];
      [node setSystemID:nextQuoted(body,&scan)];
   } else if([keyword isEqualToString:@"SYSTEM"]) {
      NSRange scan=first;

      scan.location=NSMaxRange(first);
      scan.length=[body length]-scan.location;

      [node setSystemID:nextQuoted(body,&scan)];
   }
}

@implementation NSXMLDTD

+(NSXMLDTDNode *)predefinedEntityDeclarationForName:(NSString *)name {
   NSDictionary *table=[NSDictionary dictionaryWithObjectsAndKeys:
      @"&", @"amp",
      @"<", @"lt",
      @">", @"gt",
      @"\"", @"quot",
      @"'", @"apos",
      nil];

   NSString *string=[table objectForKey:name];

   if(string==nil)
      return nil;

   NSXMLDTDNode *node=[[NSXMLDTDNode alloc] initWithKind:NSXMLEntityDeclarationKind];

   [node setName:name];
   [node setStringValue:string];

   return [node autorelease];
}

-initWithData:(NSData *)data options:(NSUInteger)options error:(NSError **)error {
   [super initWithKind:NSXMLDTDKind options:options];

   NSString *string=[[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];

   if(string==nil) {
      if(error!=NULL)
         *error=[NSError errorWithDomain:@"NSXMLParserErrorDomain" code:1 userInfo:nil];

      [self release];

      return nil;
   }

   NSUInteger length=[string length], i=0;

   while(i<length) {
      NSUInteger open=[string rangeOfString:@"<!" options:0 range:NSMakeRange(i,length-i)].location;

      if(open==NSNotFound)
         break;

      NSUInteger close=closingGreaterThan(string,open);

      if(close==NSNotFound) {
         if(error!=NULL)
            *error=[NSError errorWithDomain:@"NSXMLParserErrorDomain" code:1 userInfo:nil];

         [string release];
         [self release];

         return nil;
      }

      NSString *body=[string substringWithRange:NSMakeRange(open+2,close-open-2)];
      NSRange keywordRange=tokenRangeAfter(body,0);
      NSString *keyword=tokenAt(body,keywordRange);
      NSXMLDTDNode *node=nil;

      if([keyword isEqualToString:@"ELEMENT"]) {
         NSRange nameRange=tokenRangeAfter(body,NSMaxRange(keywordRange));

         if(nameRange.length>0) {
            NSRange definition=trimmedRange(body,NSMakeRange(NSMaxRange(nameRange),[body length]-NSMaxRange(nameRange)));
            node=[[NSXMLDTDNode alloc] initWithKind:NSXMLElementDeclarationKind];

            [node setName:tokenAt(body,nameRange)];
            [node setStringValue:tokenAt(body,definition)];
         }
      } else if([keyword isEqualToString:@"ATTLIST"]) {
         NSRange elementRange=tokenRangeAfter(body,NSMaxRange(keywordRange));
         NSRange attributeRange=tokenRangeAfter(body,NSMaxRange(elementRange));

         if(attributeRange.length>0) {
            NSRange definition=trimmedRange(body,NSMakeRange(NSMaxRange(attributeRange),[body length]-NSMaxRange(attributeRange)));
            node=[[NSXMLDTDNode alloc] initWithKind:NSXMLAttributeDeclarationKind];

            [node setName:tokenAt(body,attributeRange)];
            [node setNotationName:tokenAt(body,elementRange)];
            [node setStringValue:tokenAt(body,definition)];
         }
      } else if([keyword isEqualToString:@"ENTITY"]) {
         NSRange nameRange=tokenRangeAfter(body,NSMaxRange(keywordRange));

         if(nameRange.length>0 && [tokenAt(body,nameRange) isEqualToString:@"%"]) {
            nameRange=tokenRangeAfter(body,NSMaxRange(nameRange));
         }

         if(nameRange.length>0) {
            NSRange definition=trimmedRange(body,NSMakeRange(NSMaxRange(nameRange),[body length]-NSMaxRange(nameRange)));
            node=[[NSXMLDTDNode alloc] initWithKind:NSXMLEntityDeclarationKind];

            [node setName:tokenAt(body,nameRange)];
            [node setStringValue:tokenAt(body,definition)];
            applyExternalIDs(body,definition,node);
         }
      } else if([keyword isEqualToString:@"NOTATION"]) {
         NSRange nameRange=tokenRangeAfter(body,NSMaxRange(keywordRange));

         if(nameRange.length>0) {
            NSRange definition=trimmedRange(body,NSMakeRange(NSMaxRange(nameRange),[body length]-NSMaxRange(nameRange)));
            node=[[NSXMLDTDNode alloc] initWithKind:NSXMLNotationDeclarationKind];

            [node setName:tokenAt(body,nameRange)];
            [node setStringValue:tokenAt(body,definition)];
            applyExternalIDs(body,definition,node);
         }
      }

      if(node!=nil) {
         [_children addObject:node];
         [node release];
      }

      i=close+1;
   }

   [string release];

   return self;
}

-initWithContentsOfURL:(NSURL *)url options:(NSUInteger)options error:(NSError **)error {
   NSData *data=[NSData dataWithContentsOfURL:url];

   if(data==nil) {
      if(error!=NULL)
         *error=[NSError errorWithDomain:@"NSCocoaErrorDomain" code:256 userInfo:nil];

      return nil;
   }

   return [self initWithData:data options:options error:error];
}

-(NSString *)publicID {
   return _publicID;
}

-(NSString *)systemID {
   return _systemID;
}

-(NSXMLDTDNode *)attributeDeclarationForName:(NSString *)attributeName elementName:(NSString *)elementName {
   for(NSXMLNode *node in _children) {
      if([node kind]!=NSXMLAttributeDeclarationKind)
         continue;

      if(![[node name] isEqualToString:attributeName])
         continue;

      if(elementName!=nil && [node notationName]!=nil && ![[node notationName] isEqualToString:elementName])
         continue;

      return (NSXMLDTDNode *)node;
   }

   return nil;
}

-(NSXMLDTDNode *)elementDeclarationForName:(NSString *)name {
   for(NSXMLNode *node in _children) {
      if([node kind]==NSXMLElementDeclarationKind && [[node name] isEqualToString:name])
         return (NSXMLDTDNode *)node;
   }

   return nil;
}

-(NSXMLDTDNode *)entityDeclarationForName:(NSString *)name {
   for(NSXMLNode *node in _children) {
      if([node kind]==NSXMLEntityDeclarationKind && [[node name] isEqualToString:name])
         return (NSXMLDTDNode *)node;
   }

   return [[self class] predefinedEntityDeclarationForName:name];
}

-(NSXMLDTDNode *)notationDeclarationForName:(NSString *)name {
   for(NSXMLNode *node in _children) {
      if([node kind]==NSXMLNotationDeclarationKind && [[node name] isEqualToString:name])
         return (NSXMLDTDNode *)node;
   }

   return nil;
}

-(void)setPublicID:(NSString *)publicID {
   publicID=[publicID copy];
   [_publicID release];
   _publicID=publicID;
}

-(void)setSystemID:(NSString *)systemID {
   systemID=[systemID copy];
   [_systemID release];
   _systemID=systemID;
}

-(void)dealloc {
   [_publicID release];
   [_systemID release];
   [super dealloc];
}

-(void)setChildren:(NSArray *)children {
   [_children setArray:children];
}

-(void)addChild:(NSXMLNode *)node {
   [_children addObject:node];
}

-(void)insertChild:(NSXMLNode *)child atIndex:(NSUInteger)index {
   [_children insertObject:child atIndex:index];
}

-(void)insertChildren:(NSArray *)children atIndex:(NSUInteger)index {
   NSInteger i,count=[children count];
   
   for(i=0;i<count;i++)
    [_children insertObject:[children objectAtIndex:i] atIndex:index+i];
}

-(void)removeChildAtIndex:(NSUInteger)index {
   [_children removeObjectAtIndex:index];
}

-(void)replaceChildAtIndex:(NSUInteger)index withNode:(NSXMLNode *)node {
   [_children replaceObjectAtIndex:index withObject:node];
}

@end
