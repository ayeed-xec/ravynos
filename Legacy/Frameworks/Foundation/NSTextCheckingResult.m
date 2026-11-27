#import <Foundation/NSTextCheckingResult.h>
#import <Foundation/NSRaise.h>
#import <Foundation/NSDictionary.h>
#import <Foundation/NSMutableDictionary.h>
#import <Foundation/NSArray.h>
#import <Foundation/NSNumber.h>
#import <Foundation/NSString.h>
#import <Foundation/NSTimeZone.h>
#import <Foundation/NSURL.h>
#import <Foundation/NSOrthography.h>

/* Public text-checking component keys (see NSTextCheckingResult.h). */
NSString *const NSTextCheckingNameKey = @"NSTextCheckingNameKey";
NSString *const NSTextCheckingJobTitleKey = @"NSTextCheckingJobTitleKey";
NSString *const NSTextCheckingOrganizationKey = @"NSTextCheckingOrganizationKey";
NSString *const NSTextCheckingStreetKey = @"NSTextCheckingStreetKey";
NSString *const NSTextCheckingCityKey = @"NSTextCheckingCityKey";
NSString *const NSTextCheckingStateKey = @"NSTextCheckingStateKey";
NSString *const NSTextCheckingZIPKey = @"NSTextCheckingZIPKey";
NSString *const NSTextCheckingCountryKey = @"NSTextCheckingCountryKey";
NSString *const NSTextCheckingPhoneKey = @"NSTextCheckingPhoneKey";

/* Private keys used to store result payloads in _properties. */
static NSString * const NSTextCheckingAddressComponentsKey = @"NSTextCheckingAddressComponentsKey";
static NSString * const NSTextCheckingReplacementKey = @"NSTextCheckingReplacementKey";
static NSString * const NSTextCheckingDateKey = @"NSTextCheckingDateKey";
static NSString * const NSTextCheckingTimeZoneKey = @"NSTextCheckingTimeZoneKey";
static NSString * const NSTextCheckingDurationKey = @"NSTextCheckingDurationKey";
static NSString * const NSTextCheckingGrammarDetailsKey = @"NSTextCheckingGrammarDetailsKey";
static NSString * const NSTextCheckingOrthographyKey = @"NSTextCheckingOrthographyKey";
static NSString * const NSTextCheckingURLKey = @"NSTextCheckingURLKey";

@implementation NSTextCheckingResult

-initWithResultType:(NSTextCheckingType)resultType range:(NSRange)range properties:(NSDictionary *)properties {
   _resultType=resultType;
   _range=range;
   _properties=[properties copy];
   return self;
}

-initWithResultType:(NSTextCheckingType)resultType range:(NSRange)range property:property name:(NSString *)name {
   NSDictionary *properties=[NSDictionary dictionaryWithObject:property forKey:name];

   return [self initWithResultType:resultType range:range properties:properties];
}

-(void)dealloc {
   [_properties release];
   [super dealloc];
}

+(NSTextCheckingResult *)addressCheckingResultWithRange:(NSRange)range components:(NSDictionary *)components {
    if(components==nil)
        return nil;

    return [[[self alloc] initWithResultType:NSTextCheckingTypeAddress range:range properties:components] autorelease];
}

+(NSTextCheckingResult *)correctionCheckingResultWithRange:(NSRange)range replacementString:(NSString *)replacement {
    return [[[self alloc] initWithResultType:NSTextCheckingTypeCorrection range:range property:replacement name:NSTextCheckingReplacementKey] autorelease];
}

+(NSTextCheckingResult *)dashCheckingResultWithRange:(NSRange)range replacementString:(NSString *)replacement {
    return [[[self alloc] initWithResultType:NSTextCheckingTypeDash range:range property:replacement name:NSTextCheckingReplacementKey] autorelease];
}

+(NSTextCheckingResult *)dateCheckingResultWithRange:(NSRange)range date:(NSDate *)date {
    if(date==nil)
        return nil;

    return [[[self alloc] initWithResultType:NSTextCheckingTypeDate range:range property:date name:NSTextCheckingDateKey] autorelease];
}

+(NSTextCheckingResult *)dateCheckingResultWithRange:(NSRange)range date:(NSDate *)date timeZone:(NSTimeZone *)timeZone duration:(NSTimeInterval)duration {
    if(date==nil)
        return nil;

    NSMutableDictionary *properties=[NSMutableDictionary dictionary];

    [properties setObject:date forKey:NSTextCheckingDateKey];
    if(timeZone!=nil)
        [properties setObject:timeZone forKey:NSTextCheckingTimeZoneKey];
    [properties setObject:[NSNumber numberWithDouble:duration] forKey:NSTextCheckingDurationKey];

    return [[[self alloc] initWithResultType:NSTextCheckingTypeDate range:range properties:properties] autorelease];
}

+(NSTextCheckingResult *)grammarCheckingResultWithRange:(NSRange)range details:(NSArray *)details {
    return [[[self alloc] initWithResultType:NSTextCheckingTypeGrammar range:range property:details name:NSTextCheckingGrammarDetailsKey] autorelease];
}

+(NSTextCheckingResult *)linkCheckingResultWithRange:(NSRange)range URL:(NSURL *)url {
    if(url==nil)
        return nil;

    return [[[self alloc] initWithResultType:NSTextCheckingTypeLink range:range property:url name:NSTextCheckingURLKey] autorelease];
}

+(NSTextCheckingResult *)orthographyCheckingResultWithRange:(NSRange)range orthography:(NSOrthography *)orthography {
    if(orthography==nil)
        return nil;

    return [[[self alloc] initWithResultType:NSTextCheckingTypeOrthography range:range property:orthography name:NSTextCheckingOrthographyKey] autorelease];
}

+(NSTextCheckingResult *)quoteCheckingResultWithRange:(NSRange)range replacementString:(NSString *)replacement {
    return [[[self alloc] initWithResultType:NSTextCheckingTypeQuote range:range property:replacement name:NSTextCheckingReplacementKey] autorelease];
}

+(NSTextCheckingResult *)replacementCheckingResultWithRange:(NSRange)range replacementString:(NSString *)replacement {
    return [[[self alloc] initWithResultType:NSTextCheckingTypeReplacement range:range property:replacement name:NSTextCheckingReplacementKey] autorelease];
}

+(NSTextCheckingResult *)spellCheckingResultWithRange:(NSRange)range {
   return [[[self alloc] initWithResultType:NSTextCheckingTypeSpelling range:range properties:nil] autorelease];
}

-(NSDictionary *)addressComponents {
    if(!(_resultType & NSTextCheckingTypeAddress))
        return nil;

    return [_properties objectForKey:NSTextCheckingAddressComponentsKey] ?: _properties;
}

-(NSDate *)date {
    if(!(_resultType & NSTextCheckingTypeDate))
        return nil;

    return [_properties objectForKey:NSTextCheckingDateKey];
}

-(NSTimeInterval)duration {
    if(!(_resultType & NSTextCheckingTypeDate))
        return 0;

    return [[_properties objectForKey:NSTextCheckingDurationKey] doubleValue];
}

-(NSArray *)grammarDetails {
    if(!(_resultType & NSTextCheckingTypeGrammar))
        return nil;

    return [_properties objectForKey:NSTextCheckingGrammarDetailsKey];
}

-(NSOrthography *)orthography {
    if(!(_resultType & NSTextCheckingTypeOrthography))
        return nil;

    return [_properties objectForKey:NSTextCheckingOrthographyKey];
}

-(NSRange)range {
    return _range;
}

-(NSString *)replacementString {
    if(_resultType & (NSTextCheckingTypeCorrection | NSTextCheckingTypeDash |
                      NSTextCheckingTypeQuote | NSTextCheckingTypeReplacement))
        return [_properties objectForKey:NSTextCheckingReplacementKey];

    return nil;
}

-(NSTextCheckingType)resultType {
    return _resultType;
}

-(NSTimeZone *)timeZone {
    if(!(_resultType & NSTextCheckingTypeDate))
        return nil;

    return [_properties objectForKey:NSTextCheckingTimeZoneKey];
}

-(NSURL *)URL {
    if(!(_resultType & NSTextCheckingTypeLink))
        return nil;

    return [_properties objectForKey:NSTextCheckingURLKey];
}

-(NSDictionary *)properties {
    return _properties;
}

- (NSString *)description {
    return [NSString stringWithFormat:@"<%@ %p> type=%#llx range={%lu, %lu} properties=%@",
            [self class], self, (unsigned long long)_resultType,
            (unsigned long)_range.location, (unsigned long)_range.length, _properties];
}

@end
