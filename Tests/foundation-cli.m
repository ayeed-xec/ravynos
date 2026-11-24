#import <Foundation/Foundation.h>
#import <stdio.h>
#import <string.h>

static int failures=0;
#define CHECK(cond,msg) do{ if(cond) printf("ok: %s\n",msg); else { printf("FAIL: %s\n",msg); failures++; } }while(0)

int main(int argc,char **argv){
  NSAutoreleasePool *pool=[NSAutoreleasePool new];

  NSMutableArray *items=[NSMutableArray arrayWithObjects:@"delta",@"alpha",@"charlie",nil];
  [items addObject:@"bravo"];
  NSArray *sorted=[items sortedArrayUsingSelector:@selector(compare:)];
  CHECK([sorted count]==4,"array holds four strings");
  CHECK([[sorted objectAtIndex:0] isEqualToString:@"alpha"],"first sorted element");
  CHECK([[sorted objectAtIndex:3] isEqualToString:@"delta"],"last sorted element");
  NSString *joined=[sorted componentsJoinedByString:@", "];
  CHECK([joined isEqualToString:@"alpha, bravo, charlie, delta"],"joined separator string");

  NSString *formatted=[NSString stringWithFormat:@"%d-%@-%.2f",7,joined,3.5];
  puts([formatted UTF8String]);
  CHECK([formatted isEqualToString:@"7-alpha, bravo, charlie, delta-3.50"],"stringWithFormat output");

  NSProcessInfo *pi=[NSProcessInfo processInfo];
  CHECK(pi!=nil,"process info exists");
  CHECK([pi arguments]!=nil && [[pi arguments] count]>=1,"arguments available");
  if([[pi arguments] count]>=1){
    NSString *arg0=[[pi arguments] objectAtIndex:0];
    puts([arg0 UTF8String]);
    CHECK([arg0 containsString:@"foundation-cli"],"argv[0] names this tool");
  }
  NSDictionary *env=[pi environment];
  CHECK(env!=nil,"environment available");
  NSString *path=[env objectForKey:@"PATH"];
  CHECK(path!=nil && [path length]>0,"PATH readable from environment");

  NSString *now=[[NSDate date] description];
  puts([now UTF8String]);
  CHECK(now!=nil && [now length]>=19,"date description non-empty");

  (void)argc;(void)argv;
  [pool release];
  if(failures==0) printf("FOUNDATION-CLI-OK\n");
  return failures==0?0:1;
}
