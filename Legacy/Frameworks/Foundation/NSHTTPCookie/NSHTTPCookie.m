/* Copyright (c) 2006-2007 Christopher J. W. Lloyd

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software") to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */
#import <Foundation/NSHTTPCookie.h>

#import <Foundation/NSString.h>
#import <Foundation/NSMutableArray.h>
#import <Foundation/NSMutableDictionary.h>
#import <Foundation/NSDate.h>
#import <Foundation/NSDateFormatter.h>
#import <Foundation/NSURL.h>
#import <Foundation/NSCharacterSet.h>
#import <Foundation/NSNumber.h>

NSString *const NSHTTPCookieComment = @"Comment";
NSString *const NSHTTPCookieCommentURL = @"CommentURL";
NSString *const NSHTTPCookieDiscard = @"Discard";
NSString *const NSHTTPCookieDomain = @"Domain";
NSString *const NSHTTPCookieExpires = @"Expires";
NSString *const NSHTTPCookieMaximumAge = @"MaximumAge";
NSString *const NSHTTPCookieName = @"Name";
NSString *const NSHTTPCookieOriginURL = @"OriginURL";
NSString *const NSHTTPCookiePath = @"Path";
NSString *const NSHTTPCookiePort = @"Port";
NSString *const NSHTTPCookieSecure = @"Secure";
NSString *const NSHTTPCookieValue = @"Value";
NSString *const NSHTTPCookieVersion = @"Version";

static NSString *NSHTTPCookieTrimmedString(NSString *string) {
    return [string stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
}

static NSString *NSHTTPCookieDomainFromString(NSString *string) {
    NSString *domain = NSHTTPCookieTrimmedString(string);

    if([domain hasPrefix:@"."])
        domain = [domain substringFromIndex:1];

    return [domain lowercaseString];
}

static NSString *NSHTTPCookieDefaultPathForURL(NSURL *URL) {
    NSString *path = [URL path];

    if([path length] == 0 || ![path hasPrefix:@"/"])
        return @"/";

    NSRange lastSlash = [path rangeOfString:@"/" options:NSBackwardsSearch];

    if(lastSlash.location == 0)
        return @"/";

    return [path substringToIndex:lastSlash.location];
}

static BOOL NSHTTPCookieStringIsDigits(NSString *string) {
    NSUInteger length = [string length];

    if(length == 0)
        return NO;

    for(NSUInteger index = 0; index < length; index++) {
        unichar character = [string characterAtIndex:index];

        if(character < '0' || character > '9')
            return NO;
    }

    return YES;
}

static int NSHTTPCookieMonthFromString(NSString *string) {
    NSArray *months = [NSArray arrayWithObjects:@"Jan", @"Feb", @"Mar", @"Apr", @"May", @"Jun", @"Jul", @"Aug", @"Sep", @"Oct", @"Nov", @"Dec", nil];

    if([string length] < 3)
        return -1;

    NSString *prefix = [string substringToIndex:3];

    for(int month = 0; month < 12; month++) {
        if([prefix caseInsensitiveCompare:[months objectAtIndex:month]] == NSOrderedSame)
            return month;
    }

    return -1;
}

static int NSHTTPCookieIntegerFromString(NSString *string) {
    int result = 0;

    for(NSUInteger index = 0; index < [string length]; index++) {
        unichar character = [string characterAtIndex:index];

        if(character < '0' || character > '9')
            break;

        result = result * 10 + (character - '0');
    }

    return result;
}

static NSDate *NSHTTPCookieDateFromString(NSString *string) {
    if(string == nil)
        return nil;

    NSMutableArray *tokens = [NSMutableArray array];

    for(NSString *token in [string componentsSeparatedByCharactersInSet:[NSCharacterSet characterSetWithCharactersInString:@" ,:-"]]) {
        if([token length] > 0)
            [tokens addObject:token];
    }

    NSUInteger count = [tokens count];
    NSUInteger index = 0;

    if(count < 6)
        return nil;

    if(!NSHTTPCookieStringIsDigits([tokens objectAtIndex:0]))
        index = 1;

    if(index + 6 > count)
        return nil;

    int day, month, year, hour, minute, second;

    if(NSHTTPCookieStringIsDigits([tokens objectAtIndex:index])) {
        day = NSHTTPCookieIntegerFromString([tokens objectAtIndex:index]);
        month = NSHTTPCookieMonthFromString([tokens objectAtIndex:index + 1]);
        year = NSHTTPCookieIntegerFromString([tokens objectAtIndex:index + 2]);
        hour = NSHTTPCookieIntegerFromString([tokens objectAtIndex:index + 3]);
        minute = NSHTTPCookieIntegerFromString([tokens objectAtIndex:index + 4]);
        second = NSHTTPCookieIntegerFromString([tokens objectAtIndex:index + 5]);
    } else {
        month = NSHTTPCookieMonthFromString([tokens objectAtIndex:index]);
        day = NSHTTPCookieIntegerFromString([tokens objectAtIndex:index + 1]);
        hour = NSHTTPCookieIntegerFromString([tokens objectAtIndex:index + 2]);
        minute = NSHTTPCookieIntegerFromString([tokens objectAtIndex:index + 3]);
        second = NSHTTPCookieIntegerFromString([tokens objectAtIndex:index + 4]);
        year = NSHTTPCookieIntegerFromString([tokens objectAtIndex:index + 5]);
    }

    if(month < 0 || day < 1 || day > 31)
        return nil;

    if(year < 70)
        year += 2000;
    else if(year < 100)
        year += 1900;

    NSTimeInterval interval = NSTimeIntervalWithComponents(year, month + 1, day, hour, minute, second, 0);

    return [[NSDate alloc] initWithTimeIntervalSinceReferenceDate:interval];
}

@implementation NSHTTPCookie

-initWithProperties:(NSDictionary *)properties {
    [super init];
    _properties = [properties copy];

    return self;
}

+(id)cookieWithProperties:(NSDictionary *)properties {
    return [[[self alloc] initWithProperties:properties] autorelease];
}

+(NSArray *)cookiesWithResponseHeaderFields:(NSDictionary *)headerFields forURL:(NSURL *)URL {
    NSMutableArray *result = [NSMutableArray array];
    id header = [headerFields objectForKey:@"Set-Cookie"];

    if(header == nil)
        return result;

    NSArray *headerStrings = nil;

    if([header isKindOfClass:[NSArray class]])
        headerStrings = header;
    else
        headerStrings = [NSArray arrayWithObject:header];

    for(NSString *headerString in headerStrings)
        [result addObjectsFromArray:[self cookiesWithString:headerString forURL:URL]];

    return result;
}

+(NSArray *)cookiesWithString:(NSString *)headerString forURL:(NSURL *)URL {
    NSMutableArray *result = [NSMutableArray array];
    NSArray *components = [headerString componentsSeparatedByString:@";"];
    NSUInteger count = [components count];

    if(count < 1)
        return result;

    NSString *first = NSHTTPCookieTrimmedString([components objectAtIndex:0]);
    NSRange equalsRange = [first rangeOfString:@"="];

    if(equalsRange.length == 0)
        return result;

    NSString *name = NSHTTPCookieTrimmedString([first substringToIndex:equalsRange.location]);
    NSString *value = NSHTTPCookieTrimmedString([first substringFromIndex:equalsRange.location + 1]);

    if([name length] == 0)
        return result;

    NSMutableDictionary *properties = [NSMutableDictionary dictionary];

    [properties setObject:name forKey:NSHTTPCookieName];
    [properties setObject:value forKey:NSHTTPCookieValue];

    for(NSUInteger index = 1; index < count; index++) {
        NSString *component = NSHTTPCookieTrimmedString([components objectAtIndex:index]);
        NSRange attributeRange = [component rangeOfString:@"="];
        NSString *attributeName = nil;
        NSString *attributeValue = nil;

        if(attributeRange.length == 0) {
            attributeName = component;
        } else {
            attributeName = NSHTTPCookieTrimmedString([component substringToIndex:attributeRange.location]);
            attributeValue = NSHTTPCookieTrimmedString([component substringFromIndex:attributeRange.location + 1]);
        }

        if([attributeName caseInsensitiveCompare:@"Version"] == NSOrderedSame) {
            if(attributeValue != nil)
                [properties setObject:attributeValue forKey:NSHTTPCookieVersion];
        } else if([attributeName caseInsensitiveCompare:@"Domain"] == NSOrderedSame) {
            if(attributeValue != nil)
                [properties setObject:NSHTTPCookieDomainFromString(attributeValue) forKey:NSHTTPCookieDomain];
        } else if([attributeName caseInsensitiveCompare:@"Path"] == NSOrderedSame) {
            if(attributeValue != nil)
                [properties setObject:attributeValue forKey:NSHTTPCookiePath];
        } else if([attributeName caseInsensitiveCompare:@"Secure"] == NSOrderedSame)
            [properties setObject:@"TRUE" forKey:NSHTTPCookieSecure];
        else if([attributeName caseInsensitiveCompare:@"Discard"] == NSOrderedSame)
            [properties setObject:@"TRUE" forKey:NSHTTPCookieDiscard];
        else if([attributeName caseInsensitiveCompare:@"Expires"] == NSOrderedSame) {
            NSDate *date = NSHTTPCookieDateFromString(attributeValue);

            if(date != nil)
                [properties setObject:date forKey:NSHTTPCookieExpires];
        } else if([attributeName caseInsensitiveCompare:@"Max-Age"] == NSOrderedSame) {
            if(attributeValue != nil)
                [properties setObject:attributeValue forKey:NSHTTPCookieMaximumAge];
        } else if([attributeName caseInsensitiveCompare:@"Port"] == NSOrderedSame) {
            if(attributeValue != nil)
                [properties setObject:attributeValue forKey:NSHTTPCookiePort];
        } else if([attributeName caseInsensitiveCompare:@"Comment"] == NSOrderedSame) {
            if(attributeValue != nil)
                [properties setObject:attributeValue forKey:NSHTTPCookieComment];
        } else if([attributeName caseInsensitiveCompare:@"CommentURL"] == NSOrderedSame) {
            if(attributeValue != nil)
                [properties setObject:attributeValue forKey:NSHTTPCookieCommentURL];
        }
    }

    if([properties objectForKey:NSHTTPCookieDomain] == nil) {
        NSString *host = [URL host];

        if(host != nil)
            [properties setObject:[host lowercaseString] forKey:NSHTTPCookieDomain];
    }

    if([properties objectForKey:NSHTTPCookiePath] == nil)
        [properties setObject:NSHTTPCookieDefaultPathForURL(URL) forKey:NSHTTPCookiePath];

    [result addObject:[self cookieWithProperties:properties]];

    return result;
}

+(NSDictionary *)requestHeaderFieldsWithCookies:(NSArray *)cookies {
    NSMutableString *header = [NSMutableString string];
    BOOL first = YES;

    for(NSHTTPCookie *cookie in cookies) {
        NSString *name = [cookie name];
        NSString *value = [cookie value];

        if(name == nil || value == nil)
            continue;

        if(!first)
            [header appendString:@"; "];

        [header appendFormat:@"%@=%@", name, value];
        first = NO;
    }

    if(first)
        return [NSDictionary dictionary];

    return [NSDictionary dictionaryWithObject:header forKey:@"Cookie"];
}

-(NSString *)name {
    return [_properties objectForKey:NSHTTPCookieName];
}

-(NSString *)value {
    return [_properties objectForKey:NSHTTPCookieValue];
}

-(NSString *)domain {
    return [_properties objectForKey:NSHTTPCookieDomain];
}

-(NSString *)path {
    return [_properties objectForKey:NSHTTPCookiePath];
}

-(BOOL)isSecure {
    return [[_properties objectForKey:NSHTTPCookieSecure] boolValue];
}

-(BOOL)isSessionOnly {
    if([[_properties objectForKey:NSHTTPCookieDiscard] boolValue])
        return YES;

    return [self expiresDate] == nil;
}

-(NSDate *)expiresDate {
    id expires = [_properties objectForKey:NSHTTPCookieExpires];

    if(expires == nil) {
        NSString *maximumAge = [_properties objectForKey:NSHTTPCookieMaximumAge];

        if(maximumAge != nil)
            return [NSDate dateWithTimeIntervalSinceNow:[maximumAge intValue]];

        return nil;
    }

    if([expires isKindOfClass:[NSDate class]])
        return expires;

    return NSHTTPCookieDateFromString(expires);
}

-(NSUInteger)version {
    return [[_properties objectForKey:NSHTTPCookieVersion] intValue];
}

-(NSArray *)portList {
    NSString *portString = [_properties objectForKey:NSHTTPCookiePort];

    if(portString == nil)
        return nil;

    NSMutableArray *result = [NSMutableArray array];

    for(NSString *port in [portString componentsSeparatedByString:@","]) {
        NSString *trimmed = NSHTTPCookieTrimmedString(port);

        if([trimmed length] > 0)
            [result addObject:[NSNumber numberWithInt:[trimmed intValue]]];
    }

    return result;
}

-(NSString *)comment {
    return [_properties objectForKey:NSHTTPCookieComment];
}

-(NSURL *)commentURL {
    NSString *commentURL = [_properties objectForKey:NSHTTPCookieCommentURL];

    if(commentURL == nil)
        return nil;

    return [NSURL URLWithString:commentURL];
}

-(NSDictionary *)properties {
    return _properties;
}

@end
