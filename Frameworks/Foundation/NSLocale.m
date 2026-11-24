/* Copyright (c) 2006-2007 Christopher J. W. Lloyd

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */
#import <Foundation/NSDictionary.h>
#import <Foundation/NSLocale.h>
#import <Foundation/NSRaise.h>
#import <Foundation/NSNumber.h>
#import <Foundation/NSArray.h>
#import <Foundation/NSUserDefaults.h>
#import <Foundation/NSCharacterSet.h>

#include <locale.h>

NSString * const NSLocaleCountryCode=@"NSLocaleCountryCode";
NSString * const NSLocaleLanguageCode=@"NSLocaleLanguageCode";
NSString * const NSLocaleVariantCode=@"NSLocaleVariantCode";
NSString * const NSLocaleIdentifier=@"NSLocaleIdentifier";

NSString * const NSLocaleGroupingSeparator=@"NSLocaleGroupingSeparator";
NSString * const NSLocaleDecimalSeparator=@"NSLocaleDecimalSeparator";
NSString * const NSLocaleCalendar=@"NSLocaleCalendar";
NSString * const NSLocaleCurrencyCode=@"NSLocaleCurrencyCode";
NSString * const NSLocaleCurrencySymbol=@"NSLocaleCurrencySymbol";
NSString * const NSLocaleUsesMetricSystem=@"NSLocaleUsesMetricSystem";
NSString * const NSLocaleMeasurementSystem=@"NSLocaleMeasurementSystem";

NSString * const NSLocaleScriptCode=@"NSLocaleScriptCode";
NSString * const NSLocaleExemplarCharacterSet=@"NSLocaleExemplarCharacterSet";
NSString * const NSLocaleCollationIdentifier=@"NSLocaleCollationIdentifier";

NSString * const NSCurrentLocaleDidChangeNotification=@"NSCurrentLocaleDidChangeNotification";

BOOL NSCurrentLocaleIsMetric();

@implementation NSLocale

static NSLocale *_sharedSystemLocale  = nil;
static NSLocale *_sharedCurrentLocale = nil;

-(NSDictionary *)_locale {
   return _locale;
}

+systemLocale {
   if (_sharedSystemLocale == nil)
      _sharedSystemLocale = [[NSLocale alloc] initWithLocaleIdentifier:@"en_US"];
   return _sharedSystemLocale;
}

+currentLocale {
   if (_sharedCurrentLocale == nil)
   {
      NSString *localeIdentifier;
      
      if([self respondsToSelector:@selector(_platformCurrentLocaleIdentifier)])
       localeIdentifier=[self performSelector:@selector(_platformCurrentLocaleIdentifier)];
      else
       localeIdentifier=@"en_US";

      if(localeIdentifier==nil)
       localeIdentifier=@"en_US";
       
      _sharedCurrentLocale = [[NSLocale alloc] initWithLocaleIdentifier:localeIdentifier];
   }
   return _sharedCurrentLocale;
}

-(void)dealloc {
   [_locale release];
   [super dealloc];
}

+autoupdatingCurrentLocale {
   return [self currentLocale];
}

+(NSArray *)availableLocaleIdentifiers {
   static NSArray *identifiers=nil;

   if(identifiers==nil) {
      NSMutableArray *result=[NSMutableArray array];

      for(NSUInteger i=0;i<sizeof(NSKnownLocaleIdentifiers)/sizeof(NSKnownLocaleIdentifiers[0]);i++)
         [result addObject:NSKnownLocaleIdentifiers[i]];

      identifiers=[[NSArray arrayWithArray:result] retain];
   }

   return identifiers;
}

+(NSString *)canonicalLocaleIdentifierFromString:(NSString *)string {
   if(string==nil)
      return @"";

   return [[string lowercaseString] stringByReplacingOccurrencesOfString:@"-" withString:@"_"];
}

+(NSDictionary *)componentsFromLocaleIdentifier:(NSString *)identifier {
   if ([identifier isEqualToString:[[NSLocale currentLocale] localeIdentifier]])
      return [_sharedCurrentLocale _locale];
   else if ([identifier isEqualToString:[[NSLocale systemLocale] localeIdentifier]])
      return [_sharedSystemLocale _locale];
   else
      return [[[[NSLocale alloc] initWithLocaleIdentifier:identifier] autorelease] _locale];
   return 0;
}

+(NSString *)localeIdentifierFromComponents:(NSDictionary *)components {
   NSString *language=[components objectForKey:NSLocaleLanguageCode];
   NSString *country=[components objectForKey:NSLocaleCountryCode];

   if(language==nil)
      return @"";

   if(country==nil)
      return language;

   return [language stringByAppendingFormat:@"_%@",country];
}

static NSString *const NSKnownISOCountryCodes[]={
   @"AD",@"AE",@"AF",@"AG",@"AI",@"AL",@"AM",@"AO",@"AR",@"AS",@"AT",@"AU",@"AW",@"AZ",
   @"BA",@"BB",@"BD",@"BE",@"BF",@"BG",@"BH",@"BI",@"BJ",@"BM",@"BN",@"BO",@"BR",@"BS",@"BT",@"BW",@"BY",@"BZ",
   @"CA",@"CD",@"CF",@"CG",@"CH",@"CI",@"CK",@"CL",@"CM",@"CN",@"CO",@"CR",@"CU",@"CV",@"CW",@"CY",@"CZ",
   @"DE",@"DJ",@"DK",@"DM",@"DO",@"DZ",
   @"EC",@"EE",@"EG",@"ER",@"ES",@"ET",
   @"FI",@"FJ",@"FM",@"FO",@"FR",
   @"GA",@"GB",@"GD",@"GE",@"GH",@"GM",@"GN",@"GQ",@"GR",@"GT",@"GU",@"GW",@"GY",
   @"HK",@"HN",@"HR",@"HT",@"HU",
   @"ID",@"IE",@"IL",@"IN",@"IQ",@"IR",@"IS",@"IT",
   @"JM",@"JO",@"JP",
   @"KE",@"KG",@"KH",@"KI",@"KM",@"KN",@"KP",@"KR",@"KW",@"KY",@"KZ",
   @"LA",@"LB",@"LC",@"LI",@"LK",@"LR",@"LS",@"LT",@"LU",@"LV",@"LY",
   @"MA",@"MC",@"MD",@"ME",@"MG",@"MH",@"MK",@"ML",@"MM",@"MN",@"MO",@"MP",@"MR",@"MT",@"MU",@"MV",@"MW",@"MX",@"MY",@"MZ",
   @"NA",@"NC",@"NE",@"NG",@"NI",@"NL",@"NO",@"NP",@"NR",@"NZ",
   @"OM",
   @"PA",@"PE",@"PF",@"PG",@"PH",@"PK",@"PL",@"PR",@"PS",@"PT",@"PW",@"PY",
   @"QA",
   @"RE",@"RO",@"RS",@"RU",@"RW",
   @"SA",@"SB",@"SC",@"SD",@"SE",@"SG",@"SI",@"SK",@"SL",@"SM",@"SN",@"SO",@"SR",@"SS",@"ST",@"SV",@"SX",@"SY",@"SZ",
   @"TC",@"TD",@"TG",@"TH",@"TJ",@"TL",@"TM",@"TN",@"TO",@"TR",@"TT",@"TV",@"TW",@"TZ",
   @"UA",@"UG",@"US",@"UY",@"UZ",
   @"VA",@"VC",@"VE",@"VG",@"VI",@"VN",@"VU",
   @"WF",@"WS",
   @"YE",
   @"ZA",@"ZM",@"ZW"
};

static NSString *const NSKnownLocaleIdentifiers[]={
   @"en_US",@"en_GB",@"en_CA",@"en_AU",@"en_IN",@"en_IE",@"en_NZ",@"en_ZA",
   @"fr_FR",@"fr_CA",@"fr_BE",@"fr_CH",
   @"de_DE",@"de_AT",@"de_CH",
   @"es_ES",@"es_MX",@"es_AR",
   @"it_IT",@"it_CH",
   @"ja_JP",@"ko_KR",@"zh_CN",@"zh_TW",@"zh_HK",
   @"pt_BR",@"pt_PT",
   @"ru_RU",@"pl_PL",@"nl_NL",@"nl_BE",
   @"sv_SE",@"da_DK",@"nb_NO",@"fi_FI",@"is_IS",
   @"cs_CZ",@"hu_HU",@"ro_RO",@"bg_BG",@"el_GR",@"tr_TR",@"uk_UA",
   @"he_IL",@"ar_SA",@"hi_IN",@"th_TH",@"vi_VN",@"id_ID",@"ms_MY"
};

+(NSArray *)ISOCountryCodes {
   static NSArray *codes=nil;

   if(codes==nil)
      codes=[[NSArray arrayWithObjects:NSKnownISOCountryCodes count:sizeof(NSKnownISOCountryCodes)/sizeof(NSKnownISOCountryCodes[0])] retain];

   return codes;
}

+(NSArray *)ISOLanguageCodes {
   NSCharacterSet *underscore=[NSCharacterSet characterSetWithCharactersInString:@"_"];

   NSMutableArray *result=[NSMutableArray array];

   for(NSString *identifier in [self availableLocaleIdentifiers]) {
      NSString *language=[identifier componentsSeparatedByCharactersInSet:underscore].firstObject;

      if(language!=nil && [result containsObject:language]==NO)
         [result addObject:language];
   }

   return [NSArray arrayWithArray:result];
}

+(NSArray *)ISOCurrencyCodes {
   static NSArray *codes=nil;

   if(codes==nil)
      codes=[@[@"USD",@"EUR",@"JPY",@"GBP",@"CHF",@"CAD",@"AUD",@"CNY",@"INR",@"BRL",@"RUB",@"KRW",@"SGD",@"HKD",@"NZD",@"SEK",@"NOK",@"DKK",@"PLN",@"CZK",@"HUF",@"ILS",@"MXN",@"TRY",@"ZAR"] retain];

   return codes;
}

+(NSArray *)commonISOCurrencyCodes {
   return [self ISOCurrencyCodes];
}

+(NSArray *)preferredLanguages {
   NSUserDefaults *defaults=[NSUserDefaults standardUserDefaults];
   NSArray *languages=[defaults arrayForKey:@"AppleLanguages"];

   if(languages!=nil)
      return languages;

    NSString *language = [[NSLocale autoupdatingCurrentLocale] objectForKey:NSLocaleLanguageCode];

   if(language!=nil)
      return [NSArray arrayWithObject:language];

   return [NSArray arrayWithObject:@"en"];
}

-initWithLocaleIdentifier:(NSString *)identifier {
   [super init];

	NSArray *parts = [identifier componentsSeparatedByString:@"_"];
	NSString *language = [parts objectAtIndex:0];
	NSString *country = nil;
	if (parts.count > 1) {
		country = [parts objectAtIndex:1];
	} else {
		country = identifier;
	}

	NSMutableDictionary *localeInfo = [NSMutableDictionary dictionaryWithObjectsAndKeys:
									   identifier, NSLocaleIdentifier,
									   language, NSLocaleLanguageCode,
									   country, NSLocaleCountryCode,
									   nil
									   ];
	
	if([[self class] respondsToSelector:@selector(_platformLocaleAdditionalDescriptionForIdentifier:)]) {
		// Use any platform specific method to fill the locale info if one is defined
		NSDictionary *info = [[self class] performSelector:@selector(_platformLocaleAdditionalDescriptionForIdentifier:) withObject:identifier];
		[localeInfo addEntriesFromDictionary:info];
	} else {
		// Else use setlocale & localeconv to try to get some locale info
		char *currentLocale = setlocale(LC_ALL, NULL);
		if (setlocale(LC_ALL, [[identifier UTF8String]
                    stringByAppendingString:@".UTF-8"]) == NULL) {
			identifier = @"en_US";
		}
		struct lconv *conv = localeconv();
		
		// FIXME: This is wrong in that it is using the current locales value, not the identified one
		NSNumber *usesMetric=[NSNumber numberWithBool:NSCurrentLocaleIsMetric(identifier)];
		
		[localeInfo setObject:[NSString stringWithUTF8String:conv->decimal_point] forKey: NSLocaleDecimalSeparator];
		[localeInfo setObject:[NSString stringWithUTF8String:conv->currency_symbol] forKey: NSLocaleCurrencySymbol];
		[localeInfo setObject:usesMetric forKey: NSLocaleUsesMetricSystem];
		
		// Restore the initial locale
		setlocale(LC_ALL, currentLocale);
	}
	_locale = [[NSDictionary allocWithZone:NULL] initWithDictionary:localeInfo];
	return self;
}

-(void)encodeWithCoder:(NSCoder *)coder {
   [coder encodeObject:[self localeIdentifier] forKey:@"NSLocaleIdentifier"];
}

-initWithCoder:(NSCoder *)coder {
	// A very basic implementation that handles Locale encoding in nib files
	NSDeallocateObject(self);
	return [[NSLocale systemLocale] retain];
}

-copyWithZone:(NSZone *)zone {
   return [self retain];
}

-(NSString *)localeIdentifier {
   return [_locale objectForKey:NSLocaleIdentifier];
}

-objectForKey:key {
   return [_locale objectForKey:key];
}

-(NSString *)displayNameForKey:key value:value {
   if(key==nil || value==nil)
      return nil;

   if([key isEqualToString:NSLocaleIdentifier] || [key isEqualToString:NSLocaleLanguageCode] ||
      [key isEqualToString:NSLocaleCountryCode] || [key isEqualToString:NSLocaleCurrencyCode] ||
      [key isEqualToString:NSLocaleCurrencySymbol])
      return [[value description] copy];

   return nil;
}

-(NSString *)description
{
	return [NSString stringWithFormat:@"<%@:%p %@>", [self class], self, _locale];
}
@end
