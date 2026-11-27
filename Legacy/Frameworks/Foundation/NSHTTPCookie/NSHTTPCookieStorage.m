/* Copyright (c) 2006-2007 Christopher J. W. Lloyd

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */
#import <Foundation/NSHTTPCookieStorage.h>
#import <Foundation/NSHTTPCookie.h>
#import <Foundation/NSMutableArray.h>
#import <Foundation/NSLock.h>

@implementation NSHTTPCookieStorage

+sharedHTTPCookieStorage {
    static NSHTTPCookieStorage *shared=nil;

    if(shared==nil)
        shared=NSAllocateObject(self,0,NULL);

    return shared;
}

- init {
    [super init];

    _cookies=[NSMutableArray array];
    _acceptPolicy=NSHTTPCookieAcceptPolicyOnlyFromMainDocumentDomain;

    return self;
}

-(void)dealloc {
    [_cookies release];

    [super dealloc];
}

-(NSHTTPCookieAcceptPolicy)cookieAcceptPolicy {
    return _acceptPolicy;
}

-(void)setCookieAcceptPolicy:(NSHTTPCookieAcceptPolicy)value {
    _acceptPolicy=value;
}

-(NSArray *)cookies {
    return [NSArray arrayWithArray:_cookies];
}

-(NSArray *)cookiesForURL:(NSURL *)url {
    NSString *host=[url host];
    NSString *path=[url path];

    if(path==nil)
        path=@"/";

    NSMutableArray *result=[NSMutableArray array];

    for(NSHTTPCookie *cookie in [[self cookies] copy]) {
        NSString *cookieDomain=[cookie domain];

        BOOL domainMatches=NO;

        if([cookieDomain hasPrefix:@"."])
            domainMatches=[host hasSuffix:cookieDomain];
        else
            domainMatches=[host isEqualToString:cookieDomain];

        if(!domainMatches)
            continue;

        NSString *cookiePath=[cookie path];

        if(![path hasPrefix:cookiePath])
            continue;

        NSDate *expires=[cookie expiresDate];

        if(expires!=nil && [expires timeIntervalSinceNow]<0)
            continue;

        [result addObject:cookie];
    }

    return result;
}

-(void)setCookie:(NSHTTPCookie *)value {
    if(value==nil)
        return;

    for(NSHTTPCookie *cookie in _cookies) {
        if([cookie name]==nil || [value name]==nil)
            continue;

        if([cookie name] == [value name] && [cookie domain] == [value domain] && [cookie path] == [value path]) {
            [_cookies removeObject:cookie];
            break;
        }
    }

    [_cookies addObject:value];
}

-(void)setCookies:(NSArray *)cookies forURL:(NSURL *)url mainDocumentURL:(NSURL *)mainDocumentURL {
    for(NSHTTPCookie *cookie in cookies)
        [self setCookie:cookie];
}

-(void)deleteCookie:(NSHTTPCookie *)value {
    for(NSHTTPCookie *cookie in [[self cookies] copy]) {
        if([cookie name] == [value name] && [cookie domain] == [value domain] && [cookie path] == [value path]) {
            [_cookies removeObject:cookie];
            break;
        }
    }
}

@end
