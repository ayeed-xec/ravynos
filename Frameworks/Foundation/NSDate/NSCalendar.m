/* Copyright (c) 2007 Christopher J. W. Lloyd

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */
#import <Foundation/NSCalendar.h>
#import <Foundation/NSDateComponents.h>
#import <Foundation/NSTimeZone.h>
#import <Foundation/NSLocale.h>
#import <Foundation/NSRaise.h>
#import <Foundation/NSDateFormatter.h>

NSString * const NSGregorianCalendar=@"NSGregorianCalendar";

static NSInteger calendarWeekdayOfDate(NSInteger year,NSInteger month,NSInteger day) {
   NSTimeInterval interval=NSTimeIntervalWithComponents(year,month,day,0,0,0,0);

   return NSWeekdayFromTimeInterval(interval);
}

static NSInteger calendarWeeksBetween(NSUInteger firstWeekday,NSInteger fromWeekday) {
   NSInteger result=fromWeekday-firstWeekday;

   if(result<0)
    result+=7;

   return result;
}

@implementation NSCalendar

-copyWithZone:(NSZone *)zone {
   NSCalendar *result=NSCopyObject(self,0,zone);

   result->_identifier=[_identifier copy];
   result->_timeZone=[_timeZone copy];
   result->_locale=[_locale copy];

   return result;
}

+currentCalendar {
   return [[[self alloc] initWithCalendarIdentifier:NSGregorianCalendar] autorelease];
}

-initWithCalendarIdentifier:(NSString *)identifier {
   _identifier=[identifier copy];
   _timeZone=[[NSTimeZone defaultTimeZone] copy];
   return self;
}

-(void)dealloc {
   [_identifier release];
   [_timeZone release];
   [super dealloc];
}

-(NSString *)calendarIdentifier {
   return _identifier;
}

-(NSUInteger)firstWeekday {
   return _firstWeekday;
}

-(NSUInteger)minimumDaysInFirstWeek {
   return _minimumDaysInFirstWeek;
}

-(NSTimeZone *)timeZone {
   return _timeZone;
}

-(NSLocale *)locale {
   return _locale;
}

-(void)setFirstWeekday:(NSUInteger)weekday {
   _firstWeekday=weekday;
}

-(void)setMinimumDaysInFirstWeek:(NSUInteger)days {
   _minimumDaysInFirstWeek=days;
}

-(void)setTimeZone:(NSTimeZone *)timeZone {
   timeZone=[timeZone retain];
   [_timeZone release];
   _timeZone=timeZone;
}

-(void)setLocale:(NSLocale *)locale {
   locale=[locale retain];
   [_locale release];
   _locale=locale;
}

-(NSRange)minimumRangeOfUnit:(NSCalendarUnit)unit {
   switch(unit){
   case NSEraCalendarUnit:
   case NSYearCalendarUnit:
   case NSMonthCalendarUnit:
   case NSDayCalendarUnit:
   case NSWeekCalendarUnit:
   case NSWeekdayCalendarUnit:
   case NSWeekdayOrdinalCalendarUnit:
    return NSMakeRange(1,1);
   case NSHourCalendarUnit:
   case NSMinuteCalendarUnit:
   case NSSecondCalendarUnit:
    return NSMakeRange(0,0);
   }

   return NSMakeRange(NSNotFound,0);
}

-(NSRange)maximumRangeOfUnit:(NSCalendarUnit)unit {
   switch(unit){
   case NSEraCalendarUnit:
   case NSYearCalendarUnit:
    return NSMakeRange(1,NSNotFound);
   case NSMonthCalendarUnit:
    return NSMakeRange(1,12);
   case NSDayCalendarUnit:
    return NSMakeRange(1,31);
   case NSHourCalendarUnit:
    return NSMakeRange(0,23);
   case NSMinuteCalendarUnit:
    return NSMakeRange(0,59);
   case NSSecondCalendarUnit:
    return NSMakeRange(0,59);
   case NSWeekCalendarUnit:
    return NSMakeRange(1,53);
   case NSWeekdayCalendarUnit:
    return NSMakeRange(1,7);
   case NSWeekdayOrdinalCalendarUnit:
    return NSMakeRange(1,5);
   }

   return NSMakeRange(NSNotFound,0);
}

-(NSRange)rangeOfUnit:(NSCalendarUnit)unit inUnit:(NSCalendarUnit)inUnit forDate:(NSDate *)date {
   NSTimeInterval interval=[date timeIntervalSinceReferenceDate];
   NSInteger year;
   NSInteger month;

   interval=NSMoveIntervalFromTimeZoneToGMT(interval,_timeZone);

   year=NSYearFromTimeInterval(interval);
   month=NSMonthFromTimeInterval(interval);

   switch(unit){
   case NSDayCalendarUnit:
    if(inUnit==NSMonthCalendarUnit)
     return NSMakeRange(1,NSNumberOfDaysInMonthOfYear(month,year));
    if(inUnit==NSYearCalendarUnit)
     return NSMakeRange(1,NSDayOfYearFromTimeInterval(NSTimeIntervalWithComponents(year,12,31,0,0,0,0)));
    if(inUnit==NSEraCalendarUnit)
     return NSMakeRange(1,NSNotFound);
    break;
   case NSMonthCalendarUnit:
    if(inUnit==NSYearCalendarUnit)
     return NSMakeRange(1,12);
    if(inUnit==NSEraCalendarUnit)
     return NSMakeRange(1,NSNotFound);
    break;
   case NSYearCalendarUnit:
    if(inUnit==NSEraCalendarUnit)
     return NSMakeRange(1,NSNotFound);
    break;
   case NSWeekCalendarUnit:
    if(inUnit==NSMonthCalendarUnit)
     return NSMakeRange(1,6);
    if(inUnit==NSYearCalendarUnit)
     return NSMakeRange(1,53);
    break;
   case NSWeekdayCalendarUnit:
    if(inUnit==NSWeekCalendarUnit||inUnit==NSMonthCalendarUnit||inUnit==NSYearCalendarUnit)
     return NSMakeRange(1,7);
    break;
   case NSWeekdayOrdinalCalendarUnit:
    if(inUnit==NSMonthCalendarUnit)
     return NSMakeRange(1,5);
    if(inUnit==NSYearCalendarUnit)
     return NSMakeRange(1,53);
    break;
   case NSHourCalendarUnit:
    if(inUnit==NSDayCalendarUnit)
     return NSMakeRange(0,23);
    break;
   case NSMinuteCalendarUnit:
    if(inUnit==NSHourCalendarUnit)
     return NSMakeRange(0,59);
    break;
   case NSSecondCalendarUnit:
    if(inUnit==NSMinuteCalendarUnit)
     return NSMakeRange(0,59);
    break;
   case NSEraCalendarUnit:
    return NSMakeRange(1,NSNotFound);
   }

   return NSMakeRange(NSNotFound,0);
}

-(NSUInteger)ordinalityOfUnit:(NSCalendarUnit)unit inUnit:(NSCalendarUnit)inUnit forDate:(NSDate *)date {
   NSTimeInterval interval=[date timeIntervalSinceReferenceDate];
   NSUInteger firstWeekday=(_firstWeekday>0)?_firstWeekday:1;
   NSInteger year;
   NSInteger month;
   NSInteger day;
   NSInteger dayOfYear;
   NSInteger weekday;

   interval=NSMoveIntervalFromTimeZoneToGMT(interval,_timeZone);

   year=NSYearFromTimeInterval(interval);
   month=NSMonthFromTimeInterval(interval);
   day=NSDayOfMonthFromTimeInterval(interval);
   dayOfYear=NSDayOfYearFromTimeInterval(interval);
   weekday=NSWeekdayFromTimeInterval(interval);

   switch(unit){
   case NSEraCalendarUnit:
    return NSDayOfCommonEraFromTimeInterval(interval)>0?1:0;
   case NSYearCalendarUnit:
    if(inUnit==NSEraCalendarUnit)
     return year;
    break;
   case NSMonthCalendarUnit:
    if(inUnit==NSYearCalendarUnit)
     return month;
    break;
   case NSDayCalendarUnit:
    if(inUnit==NSMonthCalendarUnit)
     return day;
    if(inUnit==NSYearCalendarUnit)
     return dayOfYear;
    if(inUnit==NSEraCalendarUnit)
     return NSDayOfCommonEraFromTimeInterval(interval);
    break;
   case NSWeekCalendarUnit:
    if(inUnit==NSMonthCalendarUnit){
     NSInteger offset=calendarWeeksBetween(firstWeekday,calendarWeekdayOfDate(year,month,1));

     return ((day-1)+offset)/7+1;
    }
    if(inUnit==NSYearCalendarUnit){
     NSInteger offset=calendarWeeksBetween(firstWeekday,calendarWeekdayOfDate(year,1,1));

     return ((dayOfYear-1)+offset)/7+1;
    }
    break;
   case NSWeekdayCalendarUnit:
    if(inUnit==NSWeekCalendarUnit||inUnit==NSMonthCalendarUnit||inUnit==NSYearCalendarUnit)
     return weekday;
    break;
   case NSWeekdayOrdinalCalendarUnit:
    if(inUnit==NSMonthCalendarUnit)
     return (day-1)/7+1;
    if(inUnit==NSYearCalendarUnit)
     return (dayOfYear-1)/7+1;
    break;
   case NSHourCalendarUnit:
    if(inUnit==NSDayCalendarUnit)
     return NS24HourFromTimeInterval(interval);
    break;
   case NSMinuteCalendarUnit:
    if(inUnit==NSHourCalendarUnit)
     return NSMinuteFromTimeInterval(interval);
    break;
   case NSSecondCalendarUnit:
    if(inUnit==NSMinuteCalendarUnit)
     return NSSecondFromTimeInterval(interval);
    break;
   }

   return NSNotFound;
}

-(NSDateComponents *)components:(NSUInteger)flags fromDate:(NSDate *)date {
   NSDateComponents *result=[[[NSDateComponents alloc] init] autorelease];
   NSTimeInterval interval=[date timeIntervalSinceReferenceDate];

   interval=NSMoveIntervalFromTimeZoneToGMT(interval,[NSTimeZone localTimeZone]);

   if(flags&NSEraCalendarUnit)
    [result setEra:NSDayOfCommonEraFromTimeInterval(interval)>0?1:0];
   if(flags&NSYearCalendarUnit)
    [result setYear:NSYearFromTimeInterval(interval)];
   if(flags&NSMonthCalendarUnit)
    [result setMonth:NSMonthFromTimeInterval(interval)];
   if(flags&NSDayCalendarUnit)
    [result setDay:NSDayOfMonthFromTimeInterval(interval)];
   if(flags&NSHourCalendarUnit)
    [result setHour:NS24HourFromTimeInterval(interval)];
   if(flags&NSMinuteCalendarUnit)
    [result setMinute:NSMinuteFromTimeInterval(interval)];
   if(flags&NSSecondCalendarUnit)
    [result setSecond:NSSecondFromTimeInterval(interval)];
   if(flags&NSWeekCalendarUnit)
    [result setWeek:[self ordinalityOfUnit:NSWeekCalendarUnit inUnit:NSYearCalendarUnit forDate:date]];
   if(flags&NSWeekdayCalendarUnit)
    [result setWeekday:NSWeekdayFromTimeInterval(interval)];
   if(flags&NSWeekdayOrdinalCalendarUnit)
    [result setWeekdayOrdinal:[self ordinalityOfUnit:NSWeekdayOrdinalCalendarUnit inUnit:NSMonthCalendarUnit forDate:date]];
#if 0
   if(flags&NSQuarterCalendarUnit)
    NSUnimplementedMethod();
#endif

   return result;
}

-(NSDateComponents *)components:(NSUInteger)flags fromDate:(NSDate *)fromDate toDate:(NSDate *)toDate options:(NSUInteger)options {
   NSDateComponents *result=[[[NSDateComponents alloc] init] autorelease];
   NSDate *start=fromDate;
   NSDate *end=toDate;
   NSDate *remaining;
   BOOL negated=NO;

   if([end compare:start]==NSOrderedAscending){
    start=toDate;
    end=fromDate;
    negated=YES;
   }

   remaining=start;

   if(flags&NSEraCalendarUnit){
    NSTimeInterval startInterval=NSMoveIntervalFromTimeZoneToGMT([start timeIntervalSinceReferenceDate],_timeZone);
    NSTimeInterval endInterval=NSMoveIntervalFromTimeZoneToGMT([end timeIntervalSinceReferenceDate],_timeZone);
    NSInteger startEra=startInterval>=0?1:0;
    NSInteger endEra=endInterval>=0?1:0;

    if(startEra!=endEra){
     [result setEra:endEra-startEra];
     remaining=[NSDate dateWithTimeIntervalSinceReferenceDate:NSMoveIntervalFromGMTToTimeZone(0,_timeZone)];
    }
   }

   if(flags&NSYearCalendarUnit){
    NSTimeInterval startInterval=NSMoveIntervalFromTimeZoneToGMT([remaining timeIntervalSinceReferenceDate],_timeZone);
    NSTimeInterval endInterval=NSMoveIntervalFromTimeZoneToGMT([end timeIntervalSinceReferenceDate],_timeZone);
    NSInteger years=NSYearFromTimeInterval(endInterval)-NSYearFromTimeInterval(startInterval);
    NSDateComponents *step=[[[NSDateComponents alloc] init] autorelease];
    NSDate *candidate;

    [step setYear:years];
    candidate=[self dateByAddingComponents:step toDate:remaining options:0];
    if(years>0&&[candidate compare:end]==NSOrderedDescending){
     years--;
     [step setYear:years];
    }
    [result setYear:years];
    remaining=[self dateByAddingComponents:step toDate:remaining options:0];
   }

   if(flags&NSMonthCalendarUnit){
    NSTimeInterval startInterval=NSMoveIntervalFromTimeZoneToGMT([remaining timeIntervalSinceReferenceDate],_timeZone);
    NSTimeInterval endInterval=NSMoveIntervalFromTimeZoneToGMT([end timeIntervalSinceReferenceDate],_timeZone);
    NSInteger months=(NSYearFromTimeInterval(endInterval)*12+NSMonthFromTimeInterval(endInterval))-(NSYearFromTimeInterval(startInterval)*12+NSMonthFromTimeInterval(startInterval));
    NSDateComponents *step=[[[NSDateComponents alloc] init] autorelease];
    NSDate *candidate;

    [step setMonth:months];
    candidate=[self dateByAddingComponents:step toDate:remaining options:0];
    if(months>0&&[candidate compare:end]==NSOrderedDescending){
     months--;
     [step setMonth:months];
    }
    [result setMonth:months];
    remaining=[self dateByAddingComponents:step toDate:remaining options:0];
   }

   if(flags&NSDayCalendarUnit){
    NSTimeInterval startInterval=NSMoveIntervalFromTimeZoneToGMT([remaining timeIntervalSinceReferenceDate],_timeZone);
    NSTimeInterval endInterval=NSMoveIntervalFromTimeZoneToGMT([end timeIntervalSinceReferenceDate],_timeZone);
    NSInteger days=NSDayOfCommonEraFromTimeInterval(endInterval)-NSDayOfCommonEraFromTimeInterval(startInterval);

    [result setDay:days];
    {
     NSDateComponents *step=[[[NSDateComponents alloc] init] autorelease];

     [step setDay:days];
     remaining=[self dateByAddingComponents:step toDate:remaining options:0];
    }
   }

   {
    NSTimeInterval remainingInterval=NSMoveIntervalFromTimeZoneToGMT([remaining timeIntervalSinceReferenceDate],_timeZone);
    NSTimeInterval endInterval=NSMoveIntervalFromTimeZoneToGMT([end timeIntervalSinceReferenceDate],_timeZone);
    NSInteger secondsLeft=(NSInteger)(endInterval-remainingInterval);

    if(flags&NSHourCalendarUnit){
     NSInteger hours=secondsLeft/3600;

     [result setHour:hours];
     secondsLeft-=hours*3600;
    }
    if(flags&NSMinuteCalendarUnit){
     NSInteger minutes=secondsLeft/60;

     [result setMinute:minutes];
     secondsLeft-=minutes*60;
    }
    if(flags&NSSecondCalendarUnit)
     [result setSecond:secondsLeft];
   }

   if(negated){
    if([result era]!=NSUndefinedDateComponent)
     [result setEra:-[result era]];
    if([result year]!=NSUndefinedDateComponent)
     [result setYear:-[result year]];
    if([result month]!=NSUndefinedDateComponent)
     [result setMonth:-[result month]];
    if([result day]!=NSUndefinedDateComponent)
     [result setDay:-[result day]];
    if([result hour]!=NSUndefinedDateComponent)
     [result setHour:-[result hour]];
    if([result minute]!=NSUndefinedDateComponent)
     [result setMinute:-[result minute]];
    if([result second]!=NSUndefinedDateComponent)
     [result setSecond:-[result second]];
   }

   return result;
}

-(NSDate *)dateByAddingComponents:(NSDateComponents *)components toDate:(NSDate *)date options:(NSUInteger)options {
   NSInteger years=[components year];
   NSInteger months=[components month];
   NSInteger weeks=[components week];
   NSInteger days=[components day];
   NSInteger hours=[components hour];
   NSInteger minutes=[components minute];
   NSInteger seconds=[components second];
   NSTimeInterval interval=[date timeIntervalSinceReferenceDate];

   interval=NSMoveIntervalFromTimeZoneToGMT(interval,_timeZone);

   if(years!=NSUndefinedDateComponent||months!=NSUndefinedDateComponent){
    NSInteger totalMonths=0;

    if(years!=NSUndefinedDateComponent)
     totalMonths+=years*12;
    if(months!=NSUndefinedDateComponent)
     totalMonths+=months;

    if(totalMonths!=0){
     NSInteger fromYear=NSYearFromTimeInterval(interval);
     NSInteger fromMonth=NSMonthFromTimeInterval(interval);
     NSInteger fromDay=NSDayOfMonthFromTimeInterval(interval);
     NSInteger newYear=fromYear+totalMonths/12;
     NSInteger newMonth0=fromMonth-1+totalMonths%12;
     NSInteger newMonth;
     NSInteger monthDays;
     NSTimeInterval timeOfDay=interval-NSTimeIntervalWithComponents(fromYear,fromMonth,fromDay,0,0,0,0);

     newYear+=newMonth0/12;
     newMonth=newMonth0%12;
     if(newMonth<0)
      newMonth+=12;
     newMonth++;

     monthDays=NSNumberOfDaysInMonthOfYear(newMonth,newYear);
     if(fromDay>monthDays)
      fromDay=monthDays;

     interval=NSTimeIntervalWithComponents(newYear,newMonth,fromDay,0,0,0,0)+timeOfDay;
    }
   }

   if(weeks!=NSUndefinedDateComponent)
    interval+=weeks*7*86400.0;
   if(days!=NSUndefinedDateComponent)
    interval+=days*86400.0;
   if(hours!=NSUndefinedDateComponent)
    interval+=hours*3600.0;
   if(minutes!=NSUndefinedDateComponent)
    interval+=minutes*60.0;
   if(seconds!=NSUndefinedDateComponent)
    interval+=seconds;

   interval=NSMoveIntervalFromGMTToTimeZone(interval,_timeZone);

   return [NSDate dateWithTimeIntervalSinceReferenceDate:interval];
}

-(NSDate *)dateFromComponents:(NSDateComponents *)components {
   NSInteger year=0;
   NSInteger month=0;
   NSInteger day=0;
   NSInteger hour=0;
   NSInteger minute=0;
   NSInteger second=0;
   NSInteger milliseconds=0;
   NSInteger check;

// FIXME: doesn't handle all components

   if((check=[components year])!=NSUndefinedDateComponent)
    year=check;
   if((check=[components month])!=NSUndefinedDateComponent)
    month=check;
   if((check=[components day])!=NSUndefinedDateComponent)
    day=check;
   if((check=[components hour])!=NSUndefinedDateComponent)
    hour=check;
   if((check=[components minute])!=NSUndefinedDateComponent)
    minute=check;
   if((check=[components second])!=NSUndefinedDateComponent)
    second=check;

   NSTimeInterval interval=NSTimeIntervalWithComponents(year,month,day,hour,minute,second,milliseconds);

   interval=NSMoveIntervalFromGMTToTimeZone(interval,[NSTimeZone localTimeZone]);

   return [NSDate dateWithTimeIntervalSinceReferenceDate:interval];
}

@end
