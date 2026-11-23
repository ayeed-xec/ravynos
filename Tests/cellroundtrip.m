#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>
#import <stdio.h>

static int failures=0;
#define CHECK(cond,msg) do{ if(cond) printf("ok: %s\n",msg); else { printf("FAIL: %s\n",msg); failures++; } }while(0)

@interface TestCell : NSTextFieldCell
{ BOOL _drawsBackground; NSColor *_backgroundColor; NSColor *_textColor; }
@end
@implementation TestCell
-(instancetype)initTextCell:(NSString*)t{
  self=[super initTextCell:t]; _drawsBackground=NO; return self;
}
-(BOOL)drawsBackground{ return _drawsBackground; }
-(void)setDrawsBackground:(BOOL)v{ _drawsBackground=v; }
-(NSColor*)backgroundColor{ return _backgroundColor; }
-(void)setBackgroundColor:(NSColor*)c{ [_backgroundColor release]; _backgroundColor=[c retain]; }
-(NSColor*)textColor{ return _textColor; }
-(void)setTextColor:(NSColor*)c{ [_textColor release]; _textColor=[c retain]; }
-(NSTextFieldBezelStyle)bezelStyle{ return _bezelStyle; }
-(void)setBezelStyle:(NSTextFieldBezelStyle)s{ _bezelStyle=s; }
-(NSString*)placeholderString{ return [_placeholder isKindOfClass:[NSAttributedString class]] ? [(NSAttributedString*)_placeholder string] : (NSString*)_placeholder; }
-(void)setPlaceholderString:(NSString*)s{ [_placeholder release]; _placeholder=[s copy]; }

-(void)encodeWithCoder:(NSCoder *)coder {
   if([coder allowsKeyedCoding]){
    NSKeyedArchiver *keyed=(NSKeyedArchiver *)coder;
    [keyed encodeBool:_drawsBackground forKey:@"NSDrawsBackground"];
    [keyed encodeObject:_backgroundColor forKey:@"NSBackgroundColor"];
    [keyed encodeObject:_textColor forKey:@"NSTextColor"];
    [keyed encodeInt:_bezelStyle forKey:@"NSTextBezelStyle"];
    if([_placeholder isKindOfClass:[NSAttributedString class]])
     [keyed encodeObject:_placeholder forKey:@"NSPlaceholderAttributedString"];
    else
     [keyed encodeObject:_placeholder forKey:@"NSPlaceholderString"];
   }
   else {
    [NSException raise:NSInvalidArgumentException format:@"%@ can not encodeWithCoder:%@",[self class],[coder class]];
   }
}
-(instancetype)initWithCoder:(NSCoder *)coder {
  self=[super initWithCoder:coder];
  if([coder allowsKeyedCoding]){
    _drawsBackground=[coder decodeBoolForKey:@"NSDrawsBackground"];
    _backgroundColor=[[coder decodeObjectForKey:@"NSBackgroundColor"] retain];
    _textColor=[[coder decodeObjectForKey:@"NSTextColor"] retain];
    _bezelStyle=[coder decodeIntegerForKey:@"NSTextBezelStyle"];
    id ph=[coder decodeObjectForKey:@"NSPlaceholderAttributedString"];
    if(!ph) ph=[coder decodeObjectForKey:@"NSPlaceholderString"];
    _placeholder=[ph copy];
  }
  return self;
}
@end

int main(void){
  NSAutoreleasePool *pool=[NSAutoreleasePool new];
  [NSApplication sharedApplication];

  TestCell *cell=[[TestCell alloc] initTextCell:@"hello"];
  [cell setDrawsBackground:YES];
  [cell setBackgroundColor:[NSColor whiteColor]];
  [cell setTextColor:[NSColor blackColor]];
  [cell setBezelStyle:NSTextFieldRoundedBezel];
  [cell setPlaceholderString:@"type here"];

  NSMutableData *data=[NSMutableData data];
  NSKeyedArchiver *archiver=[[NSKeyedArchiver alloc] initForWritingWithMutableData:data];
  [archiver encodeObject:cell forKey:@"root"];
  [archiver finishEncoding];
  printf("encoded %lu bytes\n",(unsigned long)[data length]);

  NSKeyedUnarchiver *unarchiver=[[NSKeyedUnarchiver alloc] initForReadingWithData:data];
  TestCell *back=[unarchiver decodeObjectForKey:@"root"];
  CHECK(back!=nil,"decode produced a cell");
  if(back){
    CHECK([back drawsBackground]==YES,"drawsBackground round-trips");
    CHECK([back bezelStyle]==NSTextFieldRoundedBezel,"bezelStyle round-trips");
    CHECK([[back placeholderString] isEqualToString:@"type here"],"placeholder round-trips");
    CHECK([[back backgroundColor] isEqual:[NSColor whiteColor]],"backgroundColor round-trips");
    CHECK([[back textColor] isEqual:[NSColor blackColor]],"textColor round-trips");
    TestCell *plain=[[TestCell alloc] initTextCell:@""];
    NSMutableData *d2=[NSMutableData data];
    NSKeyedArchiver *a2=[[NSKeyedArchiver alloc] initForWritingWithMutableData:d2];
    [a2 encodeObject:plain forKey:@"root"]; [a2 finishEncoding];
    NSKeyedUnarchiver *u2=[[NSKeyedUnarchiver alloc] initForReadingWithData:d2];
    TestCell *back2=[u2 decodeObjectForKey:@"root"];
    CHECK(back2!=nil && [back2 placeholderString]==nil,"string-only cell round-trips");
  }

  if(failures==0) printf("ALL CELL CODER TESTS PASSED\n");
  return failures==0?0:1;
}
