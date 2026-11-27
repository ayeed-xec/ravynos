/* Copyright (c) 2007 Christopher J. W. Lloyd

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#import <Foundation/NSXMLNode.h>
#import <Foundation/NSXMLDTDNode.h>
#import <Foundation/NSXMLElement.h>
#import <Foundation/NSXMLDocument.h>
#import <Foundation/NSRaise.h>
#import <Foundation/NSArray.h>

@implementation NSXMLNode

+document {
   return [[[NSXMLDocument alloc] init] autorelease];
}

+documentWithRootElement:(NSXMLElement *)element {
   return [[[NSXMLDocument alloc] initWithRootElement:element] autorelease];
}


+elementWithName:(NSString *)name {
   return [[[NSXMLElement alloc] initWithName:name] autorelease];
}

+elementWithName:(NSString *)name children:(NSArray *)children attributes:(NSArray *)attributes {
   NSXMLElement *result=[[[NSXMLElement alloc] initWithName:name] autorelease];
   
   [result setChildren:children];
   [result setAttributes:attributes];
   
   return result;
}

+elementWithName:(NSString *)name stringValue:(NSString *)string {
   return [[[NSXMLElement alloc] initWithName:name stringValue:string] autorelease];
}

+attributeWithName:(NSString *)name stringValue:(NSString *)string {
   NSXMLNode *result=[[[self alloc] initWithKind:NSXMLAttributeKind] autorelease];

   [result setName:name];
   [result setStringValue:string];

   return result;
}

+commentWithStringValue:(NSString *)string {
   NSXMLNode *result=[[[self alloc] initWithKind:NSXMLCommentKind] autorelease];

   [result setStringValue:string];

   return result;
}

+textWithStringValue:(NSString *)string {
   NSXMLNode *result=[[[self alloc] initWithKind:NSXMLTextKind] autorelease];

   [result setStringValue:string];

   return result;
}

+processingInstructionWithName:(NSString *)name stringValue:(NSString *)string {
   NSXMLNode *result=[[[self alloc] initWithKind:NSXMLProcessingInstructionKind] autorelease];

   [result setName:name];
   [result setStringValue:string];

   return result;
}

+DTDNodeWithXMLString:(NSString *)string {
   return [[[NSXMLDTDNode alloc] initWithXMLString:string] autorelease];
}

+namespaceWithName:(NSString *)name stringValue:(NSString *)string {
   NSXMLNode *result=[[[self alloc] initWithKind:NSXMLNamespaceKind] autorelease];

   [result setName:name];
   [result setStringValue:string];

   return result;
}

+(NSXMLNode *)predefinedNamespaceForPrefix:(NSString *)prefix {
   NSDictionary *predefined=[NSDictionary dictionaryWithObjectsAndKeys:
      @"http://www.w3.org/XML/1998/namespace",@"xml",
      @"http://www.w3.org/1999/xhtml",@"html",
      @"http://www.w3.org/2001/XMLSchema",@"xs",
      @"http://www.w3.org/2001/XMLSchema-instance",@"xsi",
      @"http://www.w3.org/2000/svg",@"svg",
      nil];
   NSString *URI=[predefined objectForKey:prefix];

   if(URI==nil)
      return nil;

   return [self namespaceWithName:prefix stringValue:URI];
}

+(NSString *)prefixForName:(NSString *)name {
   if(name==nil)
      return nil;

   NSRange colon=[name rangeOfString:@":"];

   if(colon.location==NSNotFound || colon.location==0)
      return nil;

   return [name substringToIndex:colon.location];
}

+(NSString *)localNameForName:(NSString *)name {
   if(name==nil)
      return nil;

   NSRange colon=[name rangeOfString:@":"];

   if(colon.location==NSNotFound)
      return name;

   if(colon.location+1>=[name length])
      return @"";

   return [name substringFromIndex:colon.location+1];
}


-initWithKind:(NSXMLNodeKind)kind {
   return [self initWithKind:kind options:NSXMLNodeOptionsNone];
}

-initWithKind:(NSXMLNodeKind)kind options:(NSUInteger)options {
   _parent=nil;
   _children=[[NSMutableArray alloc] init];
   _index=0;
   _kind=kind;
   _options=options;
   _name=nil;
   _value=@""; // Behavior is empty string not null
   return self;
}

-(void)dealloc {
   _parent=nil;
   [_children release];
   [_name release];
   [_value release];
   [super dealloc];
}

-copyWithZone:(NSZone *)zone {
   NSXMLNode *copy=NSCopyObject(self,0,zone);
   
   copy->_name=[_name copy];
   copy->_value=[_value copy];
   
   return copy;
}

-(NSUInteger)index {
   return _index;
}

-(NSXMLNodeKind)kind {
   return _kind;
}

-(NSUInteger)level {
   NSUInteger level=0;
   NSXMLNode *node=self;

   while([node parent]!=nil) {
      level++;
      node=[node parent];
   }

   return level;
}

-(NSString *)localName {
   return [[self class] localNameForName:[self name]];
}

-(NSString *)name {
   return _name;
}

-(NSXMLNode *)nextNode {
   NSXMLNode *sibling=[self nextSibling];

   if(sibling!=nil)
      return sibling;

   NSXMLNode *ancestor=[self parent];

   while(ancestor!=nil) {
      sibling=[ancestor nextSibling];

      if(sibling!=nil)
         return sibling;

      ancestor=[ancestor parent];
   }

   return nil;
}

-(NSXMLNode *)nextSibling {
   NSXMLNode *parent=[self parent];

   if(parent==nil)
      return nil;

   NSArray *siblings=[parent childNodes];
   NSUInteger index=[siblings indexOfObject:self];

   if(index==NSNotFound || index+1>=[siblings count])
      return nil;

   return [siblings objectAtIndex:index+1];
}

-(NSString *)stringValue {
   return _value;
}

-(NSString *)URI {
   NSString *prefix=[self prefix];

   if(prefix==nil)
      return nil;

   NSXMLNode *node=[self parent];

   while(node!=nil) {
      for(NSXMLNode *child in [node childNodes]) {
         if([child kind]!=NSXMLNamespaceKind)
            continue;

         if([[child name] isEqualToString:prefix])
            return [child stringValue];
      }

      node=[node parent];
   }

   return nil;
}

-objectValue {
   return _value;
}

-(NSXMLNode *)parent {
   return _parent;
}

-(NSString *)prefix {
   return [[self class] prefixForName:[self name]];
}

-(NSXMLNode *)previousNode {
   NSXMLNode *sibling=[self previousSibling];

   if(sibling!=nil)
      return sibling;

   return [self parent];
}

-(NSXMLNode *)previousSibling {
   NSXMLNode *parent=[self parent];

   if(parent==nil)
      return nil;

   NSArray *siblings=[parent childNodes];
   NSUInteger index=[siblings indexOfObject:self];

   if(index==NSNotFound || index==0)
      return nil;

   return [siblings objectAtIndex:index-1];
}

-(NSXMLDocument *)rootDocument {
   NSXMLNode *node=self;

   while([node parent]!=nil) {
      node=[node parent];
   }

   if([node kind]!=NSXMLDocumentKind)
      return nil;

   return (NSXMLDocument *)node;
}

-(NSUInteger)childCount {
   return [_children count];
}

-(NSArray *)children {
   return _children;
}

-(NSXMLNode *)childAtIndex:(NSUInteger)index {
   return [_children objectAtIndex:index];
}

-(void)setName:(NSString *)name {
   name=[name copy];
   [_name release];
   _name=name;
}

-(void)setObjectValue:object {
   object=[object retain];
   [_value release];
   _value=object;
}

-(void)setStringValue:(NSString *)string {
   string=[string copy];
   [_value release];
   _value=string;
}

-(void)setStringValue:(NSString *)string resolvingEntities:(BOOL)resolveEntities {
   if(!resolveEntities) {
      [self setStringValue:string];
      return;
   }

   NSMutableString *result=[NSMutableString stringWithString:string];

   NSDictionary *standard=[NSDictionary dictionaryWithObjectsAndKeys:
      @"<",@"&lt;",
      @">",@"&gt;",
      @"&",@"&amp;",
      @"'",@"&apos;",
      @"\"",@"&quot;",
      nil];

   for(NSString *entity in standard) {
      [result replaceOccurrencesOfString:entity
                              withString:[standard objectForKey:entity]
                                 options:0
                                   range:NSMakeRange(0,[result length])];
   }

   [self setStringValue:result];
}

-(void)detach {
//   [_parent removeChild:self];
   _parent=nil;
}

-(NSArray *)nodesForXPath:(NSString *)xpath error:(NSError **)error {
   NSMutableArray *result=[NSMutableArray array];
   NSRange         range=[xpath rangeOfString:@"/"];
       
   if(range.location==NSNotFound){
    
    for(NSXMLNode *node in _children){

     if([xpath isEqualToString:@"*"])
      [result addObject:node];
      
     if([xpath isEqualToString:[node name]])
      [result addObject:node];
}
   }
   else {
    NSString *firstComponent=[xpath substringWithRange:NSMakeRange(0,range.location)];
    NSString *remainder=[xpath substringFromIndex:NSMaxRange(range)];

    for(NSXMLNode *node in _children){

     if([firstComponent isEqualToString:@"*"])
      [result addObjectsFromArray:[node nodesForXPath:remainder error:error]];
      
     if([firstComponent isEqualToString:[node name]])
      [result addObjectsFromArray:[node nodesForXPath:remainder error:error]];
    }
   }
   
   return result;
}

-(NSArray *)objectsForXQuery:(NSString *)xquery constants:(NSDictionary *)constants error:(NSError **)error {
   if(error!=nil)
      *error=nil;

   return [self nodesForXPath:xquery error:error];
}

-(NSArray *)objectsForXQuery:(NSString *)xquery error:(NSError **)error {
   return [self objectsForXQuery:xquery constants:nil error:error];
}

-(NSString *)XMLString {
   return [self XMLStringWithOptions:0];
}

-(NSString *)XMLStringWithOptions:(NSUInteger)options {
   NSMutableString *result=[NSMutableString string];
   
   switch([self kind]){

    case NSXMLInvalidKind:
     break;

    case NSXMLDocumentKind:
     // Handled in subclass
     break;

    case NSXMLElementKind:
     // Handled in subclass
     break;

    case NSXMLAttributeKind:
     [result appendString:[self name]];
     [result appendString:@"=\""];
     [result appendString:[self stringValue]];
     [result appendString:@"\""];
     break;

    case NSXMLNamespaceKind:
     break;

    case NSXMLProcessingInstructionKind:
     break;

    case NSXMLCommentKind:
     break;

    case NSXMLTextKind:
     break;

    case NSXMLDTDKind:
     break;

    case NSXMLEntityDeclarationKind:
     break;

    case NSXMLAttributeDeclarationKind:
     break;

    case NSXMLElementDeclarationKind:
     break;

    case NSXMLNotationDeclarationKind:
     break;
}

   return result;
}

-(NSString *)XPath {
   NSMutableString *result=[NSMutableString string];
   NSXMLNode *node=self;
   NSMutableArray *parts=[NSMutableArray array];

   while(node!=nil) {
      NSString *name=[node name];

      if(name!=nil)
         [parts insertObject:name atIndex:0];

      node=[node parent];
   }

   for(NSString *part in parts) {
      [result appendString:@"/"];
      [result appendString:part];
   }

   if([result length]==0)
      [result appendString:@"/"];

   return result;
}

-(NSString *)canonicalXMLStringPreservingComments:(BOOL)comments {
   return [self XMLStringWithOptions:comments ? NSXMLNodePreserveQuotes : 0];
}

-(NSString *)description {
   return [self XMLString];
}

@end
