/* Copyright (c) 2006-2007 Christopher J. W. Lloyd

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

// Original - Christopher Lloyd <cjwl@objc.net>
#import <Foundation/NSPortCoder.h>
#import <Foundation/NSRaise.h>
#import <Foundation/NSRaiseException.h>
#import <Foundation/NSData.h>
#import <Foundation/NSMutableData.h>
#import <Foundation/NSMutableArray.h>
#import <Foundation/NSArray.h>
#import <Foundation/NSString.h>
#import <Foundation/NSDate.h>
#import <Foundation/NSKeyedArchiver.h>
#import <Foundation/NSKeyedUnarchiver.h>
#import <Foundation/NSPortMessage.h>
#import <Foundation/NSConnection.h>
#import <Foundation/NSSocketPort.h>
#import <Foundation/NSPortNameServer.h>
#import <Foundation/NSProcessInfo.h>
#import <string.h>
#import <stdlib.h>

@interface NSPortCoder () {
    NSPort *_receivePort;
    NSPort *_sendPort;
    NSMutableData *_data;
    NSUInteger _offset;
    NSMutableArray *_decodedObjects;
    BOOL _isBycopy;
    BOOL _isByref;
}
@end

@implementation NSPortCoder

+portCoderWithReceivePort:(NSPort *)receivePort sendPort:(NSPort *)sendPort components:(NSArray *)components {
   return [[self alloc] initWithReceivePort:receivePort sendPort:sendPort components:components];
}

-initWithReceivePort:(NSPort *)receivePort sendPort:(NSPort *)sendPort components:(NSArray *)components {
   [super init];

   _receivePort=[receivePort retain];
   _sendPort=[sendPort retain];
   _decodedObjects=[[NSMutableArray alloc] init];
   _offset=0;

   if(components!=nil) {
      _data=[[NSMutableData alloc] initWithCapacity:1024];

      for(NSUInteger count=[components count],index=0;index<count;index++) {
         NSData *component=[components objectAtIndex:index];

         [_data appendBytes:[component bytes] length:[component length]];
      }
   } else {
      _data=[[NSMutableData alloc] initWithCapacity:1024];
   }

   return self;
}

-(void)dealloc {
   [_receivePort release];
   [_sendPort release];
   [_data release];
   [_decodedObjects release];
   [super dealloc];
}

-(NSConnection *)connection {
   NSConnection *result=nil;
   NSArray *allConnections=[NSConnection _allConnections];

   for(NSUInteger count=[allConnections count],index=0;index<count;index++) {
      NSConnection *connection=[allConnections objectAtIndex:index];

      if([connection receivePort]==_receivePort || [connection sendPort]==_sendPort) {
         result=connection;
         break;
      }
   }

   return result;
}

-(BOOL)isBycopy {
   return _isBycopy;
}

-(BOOL)isByref {
   return _isByref;
}

-(void)encodeBycopyObject:object {
   BOOL wasBycopy=_isBycopy;

   _isBycopy=YES;

   [self encodeObject:object];

   _isBycopy=wasBycopy;
}

-(void)encodeByrefObject:object {
   BOOL wasByref=_isByref;

   _isByref=YES;

   [self encodeObject:object];

   _isByref=wasByref;
}

-(void)appendBytes:(const void *)bytes length:(NSUInteger)length {
   [_data appendBytes:bytes length:length];
}

-(void)readBytes:(void *)bytes length:(NSUInteger)length {
   if(_offset+length>[_data length])
      NSRaiseException(NSRangeException,nil,0,@"port coder read past end of data");

   memcpy(bytes,((const char *)[_data bytes])+_offset,length);

   _offset+=length;
}

-(NSData *)readDataOfLength:(NSUInteger)length {
   if(_offset+length>[_data length])
      NSRaiseException(NSRangeException,nil,0,@"port coder read past end of data");

   NSData *data=[NSData dataWithBytes:((const char *)[_data bytes])+_offset length:length];

   _offset+=length;

   return data;
}

-(void)encodeObjectArchive:(id)object {
   NSData *archive=(object==nil)?nil:[NSKeyedArchiver archivedDataWithRootObject:object];
   NSUInteger length=(archive==nil)?0:[archive length];

   [self appendBytes:&length length:sizeof(length)];

   if(length>0)
      [self appendBytes:[archive bytes] length:length];
}

-(void)encodeValueOfObjCType:(const char *)type at:(const void *)ptr {
   switch(type[0]) {
      case '@': {
         [self encodeObjectArchive:*(id *)ptr];
      } break;

      case '#': {
         Class classObject=*(Class *)ptr;

         [self encodeObjectArchive:classObject];
      } break;

      case ':': {
         SEL selector=*(SEL *)ptr;
         NSString *name=(selector==NULL)?nil:NSStringFromSelector(selector);

         [self encodeObjectArchive:name];
      } break;

      case '[': {
         char *endPointer=NULL;
         NSUInteger count=strtoul(type+1,&endPointer,10);
         const char *itemType=endPointer;
         size_t itemTypeLength=strlen(itemType)-1;
         char itemBuffer[itemTypeLength+1];

         memcpy(itemBuffer,itemType,itemTypeLength);
         itemBuffer[itemTypeLength]='\0';

         NSUInteger itemSize=0;

         NSGetSizeAndAlignment(itemBuffer,&itemSize,NULL);

         const char *bytes=ptr;

         for(NSUInteger index=0;index<count;index++)
            [self encodeValueOfObjCType:itemBuffer at:bytes+index*itemSize];
      } break;

      case '{':
      case '(': {
         const char *cursor=type+1;
         NSUInteger offset=0;

         while(*cursor!='}' && *cursor!=')' && *cursor!='\0') {
            NSUInteger fieldSize=0, fieldAlignment=0;
            const char *rest=NSGetSizeAndAlignment(cursor,&fieldSize,&fieldAlignment);
            char fieldBuffer[rest-cursor+1];

            memcpy(fieldBuffer,cursor,rest-cursor);
            fieldBuffer[rest-cursor]='\0';

            offset=(offset+fieldAlignment-1)&~(fieldAlignment-1);

            [self encodeValueOfObjCType:fieldBuffer at:((const char *)ptr)+offset];

            offset+=fieldSize;
            cursor=rest;
         }
      } break;

      default: {
         NSUInteger size=0;

         NSGetSizeAndAlignment(type,&size,NULL);

         [self appendBytes:ptr length:size];
      } break;
   }
}

-(void)decodeValueOfObjCType:(const char *)type at:(void *)ptr {
   switch(type[0]) {
      case '@':
      case '#': {
         NSUInteger length=0;

         [self readBytes:&length length:sizeof(length)];

         id object=nil;

         if(length>0) {
            NSData *archive=[self readDataOfLength:length];

            object=[[NSKeyedUnarchiver unarchiveObjectWithData:archive] retain];

            [_decodedObjects addObject:object];
            [object release];
         }

         *(id *)ptr=object;
      } break;

      case ':': {
         NSUInteger length=0;

         [self readBytes:&length length:sizeof(length)];

         SEL selector=NULL;

         if(length>0) {
            NSData *archive=[self readDataOfLength:length];
            NSString *name=[NSKeyedUnarchiver unarchiveObjectWithData:archive];

            selector=NSSelectorFromString(name);
         }

         *(SEL *)ptr=selector;
      } break;

      case '[': {
         char *endPointer=NULL;
         NSUInteger count=strtoul(type+1,&endPointer,10);
         const char *itemType=endPointer;
         size_t itemTypeLength=strlen(itemType)-1;
         char itemBuffer[itemTypeLength+1];

         memcpy(itemBuffer,itemType,itemTypeLength);
         itemBuffer[itemTypeLength]='\0';

         NSUInteger itemSize=0;

         NSGetSizeAndAlignment(itemBuffer,&itemSize,NULL);

         char *bytes=ptr;

         for(NSUInteger index=0;index<count;index++)
            [self decodeValueOfObjCType:itemBuffer at:bytes+index*itemSize];
      } break;

      case '{':
      case '(': {
         const char *cursor=type+1;
         NSUInteger offset=0;

         while(*cursor!='}' && *cursor!=')' && *cursor!='\0') {
            NSUInteger fieldSize=0, fieldAlignment=0;
            const char *rest=NSGetSizeAndAlignment(cursor,&fieldSize,&fieldAlignment);
            char fieldBuffer[rest-cursor+1];

            memcpy(fieldBuffer,cursor,rest-cursor);
            fieldBuffer[rest-cursor]='\0';

            offset=(offset+fieldAlignment-1)&~(fieldAlignment-1);

            [self decodeValueOfObjCType:fieldBuffer at:((char *)ptr)+offset];

            offset+=fieldSize;
            cursor=rest;
         }
      } break;

      default: {
         NSUInteger size=0;

         NSGetSizeAndAlignment(type,&size,NULL);

         [self readBytes:ptr length:size];
      } break;
   }
}

-(void)encodeDataObject:(NSData *)data {
   NSUInteger length=(data==nil)?0:[data length];

   [self appendBytes:&length length:sizeof(length)];

   if(length>0)
      [self appendBytes:[data bytes] length:length];
}

-(NSData *)decodeDataObject {
   NSUInteger length=0;

   [self readBytes:&length length:sizeof(length)];

   if(length==0)
      return [NSData data];

   return [self readDataOfLength:length];
}

-(void)encodePortObject:(NSPort *)port {
   NSString *name=nil;

   if(port!=nil) {
      name=[[NSProcessInfo processInfo] globallyUniqueString];

      [[NSPortNameServer systemDefaultPortNameServer] registerPort:port name:name];
   }

   [self encodeObjectArchive:name];
}

-(NSPort *)decodePortObject {
   NSUInteger length=0;

   [self readBytes:&length length:sizeof(length)];

   if(length==0)
      return nil;

   NSData *archive=[self readDataOfLength:length];
   NSString *name=[NSKeyedUnarchiver unarchiveObjectWithData:archive];
   NSPort *port=[[NSPortNameServer systemDefaultPortNameServer] portForName:name host:nil];

   if(port==nil) {
      port=[[NSSocketPort alloc] init];

      [[NSPortNameServer systemDefaultPortNameServer] registerPort:port name:name];
      [port release];
   }

   return port;
}

-(void)dispatch {
   NSPortMessage *message=[[NSPortMessage alloc] initWithSendPort:_sendPort receivePort:_receivePort components:[NSArray arrayWithObject:_data]];

   [message sendBeforeDate:[NSDate date]];

   [message release];
}

@end
