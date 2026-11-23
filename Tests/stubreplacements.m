#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>
#import <stdio.h>
#import <string.h>

static int failures=0;
#define CHECK(cond,msg) do{ if(cond) printf("ok: %s\n",msg); else { printf("FAIL: %s\n",msg); failures++; } }while(0)

static NSString *stripMnemonic(NSString *value){
  NSMutableString *stripped=[NSMutableString stringWithString:value];
  [stripped replaceOccurrencesOfString:@"&&" withString:@"\x01" options:0
                                range:NSMakeRange(0,[stripped length])];
  [stripped replaceOccurrencesOfString:@"&" withString:@"" options:0
                                range:NSMakeRange(0,[stripped length])];
  [stripped replaceOccurrencesOfString:@"\x01" withString:@"&" options:0
                                range:NSMakeRange(0,[stripped length])];
  return stripped;
}

@interface ParStyle : NSMutableParagraphStyle
{ NSInteger wd; float ps, psb; int hl; float flhi, hi, ti; int al, lbm;
  float mlh, xlh, lhm, ls, dti; float hf, tfft; }
@end
@implementation ParStyle
-(instancetype)init{ self=[super init]; wd=0; hl=0; al=2; lbm=0; ls=3.5f; dti=28.0f; hf=0.5f; return self; }
-(void)encodeWithCoder:(NSCoder *)coder {
  [coder encodeValueOfObjCType:@encode(NSInteger) at:&wd];
  [coder encodeValueOfObjCType:@encode(float) at:&ps];
  [coder encodeValueOfObjCType:@encode(float) at:&psb];
  [coder encodeObject:[self textBlocks]];
  [coder encodeObject:[self textLists]];
  [coder encodeValueOfObjCType:@encode(int) at:&hl];
  [coder encodeValueOfObjCType:@encode(float) at:&flhi];
  [coder encodeValueOfObjCType:@encode(float) at:&hi];
  [coder encodeValueOfObjCType:@encode(float) at:&ti];
  [coder encodeValueOfObjCType:@encode(int) at:&al];
  [coder encodeValueOfObjCType:@encode(int) at:&lbm];
  [coder encodeValueOfObjCType:@encode(float) at:&mlh];
  [coder encodeValueOfObjCType:@encode(float) at:&xlh];
  [coder encodeValueOfObjCType:@encode(float) at:&lhm];
  [coder encodeValueOfObjCType:@encode(float) at:&ls];
  [coder encodeValueOfObjCType:@encode(float) at:&dti];
  [coder encodeObject:[self tabStops]];
  [coder encodeValueOfObjCType:@encode(float) at:&hf];
  [coder encodeValueOfObjCType:@encode(float) at:&tfft];
}
-(float)lsVal{ return ls; }
-(float)dtiVal{ return dti; }
-(float)hfVal{ return hf; }
-(int)alVal{ return al; }
@end
@implementation ParStyle (Decode)
-(instancetype)initWithCoder:(NSCoder *)coder {
  self=[super init];
  [coder decodeValueOfObjCType:@encode(NSInteger) at:&wd];
  [coder decodeValueOfObjCType:@encode(float) at:&ps];
  [coder decodeValueOfObjCType:@encode(float) at:&psb];
  id tb=[[coder decodeObject] retain];
  id tl=[[coder decodeObject] retain];
  [coder decodeValueOfObjCType:@encode(int) at:&hl];
  [coder decodeValueOfObjCType:@encode(float) at:&flhi];
  [coder decodeValueOfObjCType:@encode(float) at:&hi];
  [coder decodeValueOfObjCType:@encode(float) at:&ti];
  [coder decodeValueOfObjCType:@encode(int) at:&al];
  [coder decodeValueOfObjCType:@encode(int) at:&lbm];
  [coder decodeValueOfObjCType:@encode(float) at:&mlh];
  [coder decodeValueOfObjCType:@encode(float) at:&xlh];
  [coder decodeValueOfObjCType:@encode(float) at:&lhm];
  [coder decodeValueOfObjCType:@encode(float) at:&ls];
  [coder decodeValueOfObjCType:@encode(float) at:&dti];
  id ts=[[coder decodeObject] retain];
  [coder decodeValueOfObjCType:@encode(float) at:&hf];
  [coder decodeValueOfObjCType:@encode(float) at:&tfft];
  (void)tb;(void)tl;(void)ts;
  return self;
}
@end

int main(void){
  NSAutoreleasePool *pool=[NSAutoreleasePool new];

  CHECK([stripMnemonic(@"Cl&ick me") isEqualToString:@"Click me"],"single mnemonic stripped");
  CHECK([stripMnemonic(@"Save && Quit") isEqualToString:@"Save & Quit"],"escaped && becomes &");
  CHECK([stripMnemonic(@"Plain") isEqualToString:@"Plain"],"no mnemonic unchanged");
  CHECK([stripMnemonic(@"A&B&C") isEqualToString:@"ABC"],"multiple mnemonics stripped");

  ParStyle *p=[ParStyle new];
  NSMutableData *data=[NSMutableData data];
  NSArchiver *arch=[[NSArchiver alloc] initForWritingWithMutableData:data];
  [arch encodeObject:p];
  [arch release];
  printf("non-keyed archive: %lu bytes\n",(unsigned long)[data length]);
  NSUnarchiver *un=[[NSUnarchiver alloc] initForReadingWithData:data];
  ParStyle *back=[un decodeObject];
  CHECK(back!=nil,"non-keyed decode produced object");
  if(back){
    CHECK([back lsVal]==3.5f,"lineSpacing survives non-keyed round-trip");
    CHECK([back dtiVal]==28.0f,"defaultTabInterval survives non-keyed round-trip");
    CHECK([back hfVal]==0.5f,"hyphenationFactor survives non-keyed round-trip");
    CHECK([back alVal]==2,"alignment survives non-keyed round-trip");
  }

  if(failures==0) printf("ALL STUB REPLACEMENT TESTS PASSED\n");
  return failures==0?0:1;
}
