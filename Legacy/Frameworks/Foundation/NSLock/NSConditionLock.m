/* Copyright (c) 2007 Christopher J. W. Lloyd

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */
#import <Foundation/NSConditionLock.h>
#import <Foundation/NSRaise.h>
#import <Foundation/NSPlatform.h>
#import <Foundation/NSException.h>
#import <Foundation/NSThread.h>

#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <math.h>

@class NSDate;

/* Ivar names are deliberately distinct from the ones used by the platform
   subclass NSConditionLock_posix so neither translation unit can bind to the
   other's storage. */
@implementation NSConditionLock {
    pthread_mutex_t _clMutex;
    pthread_cond_t _clCond;
    NSInteger _clCondition;
    BOOL _clInitialized;
    NSThread *_clThread;
}

+allocWithZone:(NSZone *)zone {
   if(self==[NSConditionLock class])
      return NSAllocateObject([[NSPlatform currentPlatform] conditionLockClass],0,zone);
   else
      return NSAllocateObject(self,0,zone);
}

-initWithCondition:(NSInteger)condition {
   [super init];
   pthread_mutex_init(&_clMutex,NULL);
   pthread_cond_init(&_clCond,NULL);
   _clCondition=condition;
   _clInitialized=YES;
   return self;
}

/* The public header only reserves _name, so the pthread storage lives in the
   implementation. Entries lazy-initialize as well, so a subclass that reaches
   us via plain -init still gets usable primitives. */
static void CLEnsureInit(NSConditionLock *self) {
   if(!self->_clInitialized) {
      pthread_mutex_init(&self->_clMutex,NULL);
      pthread_cond_init(&self->_clCond,NULL);
      self->_clInitialized=YES;
   }
}

/* Absolute deadline on CLOCK_REALTIME, matching the default pthread_condattr,
   clamped to now for past dates. */
static void CLDeadlineFromInterval(NSTimeInterval interval,struct timespec *out) {
   struct timespec now;
   NSTimeInterval deadline;
   time_t sec;
   long nsec;

   clock_gettime(CLOCK_REALTIME,&now);
   if(interval>0.0)
      deadline=interval;
   else
      deadline=0.0;
   sec=now.tv_sec+(time_t)floor(deadline);
   nsec=now.tv_nsec+(long)((deadline-floor(deadline))*1000000000.0);
   if(nsec>=1000000000L) {
      sec+=nsec/1000000000L;
      nsec%=1000000000L;
   }
   out->tv_sec=sec;
   out->tv_nsec=nsec;
}

-(void)dealloc
{
    if(_clInitialized) {
        pthread_cond_destroy(&_clCond);
        pthread_mutex_destroy(&_clMutex);
    }
    [_name release];
    [_clThread release];
    [super dealloc];
}

-(NSString *)name
{
    return _name;
}

-(void)setName:(NSString *)value
{
    if(value!=_name)
    {
        [_name release];
        _name=[value retain];
    }
}

-(NSInteger)condition {
   CLEnsureInit(self);
   return _clCondition;
}

-(void)lock {
   CLEnsureInit(self);
   int rc;

   if((rc=pthread_mutex_lock(&_clMutex))!=0)
      [NSException raise:NSInvalidArgumentException format:@"failed to lock %@ (errno: %d)",self,rc];
}

-(void)unlock {
   int rc;

   if((rc=pthread_mutex_unlock(&_clMutex))!=0)
      [NSException raise:NSInvalidArgumentException format:@"failed to unlock %@ (errno: %d)",self,rc];
}

-(BOOL)tryLock {
   CLEnsureInit(self);
   return pthread_mutex_trylock(&_clMutex)==0;
}

-(BOOL)tryLockWhenCondition:(NSInteger)condition {
   if(![self tryLock])
      return NO;
   if(_clCondition==condition)
      return YES;
   [self unlock];
   return NO;
}

-(void)lockWhenCondition:(NSInteger)condition {
   int rc;

   CLEnsureInit(self);
   if((rc=pthread_mutex_lock(&_clMutex))!=0)
      [NSException raise:NSInvalidArgumentException format:@"failed to lock %@ (errno: %d)",self,rc];

   while(_clCondition!=condition) {
      if((rc=pthread_cond_wait(&_clCond,&_clMutex))!=0) {
         pthread_mutex_unlock(&_clMutex);
         [NSException raise:NSInvalidArgumentException format:@"failed to wait on %@ (errno: %d)",self,rc];
      }
   }
}

-(void)unlockWithCondition:(NSInteger)condition {
   int rc;

   CLEnsureInit(self);
   _clCondition=condition;
   if((rc=pthread_mutex_unlock(&_clMutex))!=0)
      [NSException raise:NSInvalidArgumentException format:@"failed to unlock %@ (errno: %d)",self,rc];
   if((rc=pthread_cond_broadcast(&_clCond))!=0)
      [NSException raise:NSInvalidArgumentException format:@"failed to broadcast on %@ (errno: %d)",self,rc];
}

-(BOOL)lockBeforeDate:(NSDate *)date {
   struct timespec deadline;
   struct timespec now;
   struct timespec poll;

   CLEnsureInit(self);
   /* No condition is involved: poll the mutex until the deadline. */
   CLDeadlineFromInterval([date timeIntervalSinceNow],&deadline);
   for(;;) {
      if(pthread_mutex_trylock(&_clMutex)==0)
         return YES;
      clock_gettime(CLOCK_REALTIME,&now);
      if(now.tv_sec>deadline.tv_sec ||
         (now.tv_sec==deadline.tv_sec && now.tv_nsec>=deadline.tv_nsec))
         return NO;
      poll.tv_sec=0;
      poll.tv_nsec=10000000; /* 10ms */
      nanosleep(&poll,NULL);
   }
}

-(BOOL)lockWhenCondition:(NSInteger)condition beforeDate:(NSDate *)date {
   struct timespec deadline;
   int rc;

   CLEnsureInit(self);
   CLDeadlineFromInterval([date timeIntervalSinceNow],&deadline);
   if((rc=pthread_mutex_lock(&_clMutex))!=0)
      [NSException raise:NSInvalidArgumentException format:@"failed to lock %@ (errno: %d)",self,rc];

   while(_clCondition!=condition) {
      rc=pthread_cond_timedwait(&_clCond,&_clMutex,&deadline);
      if(rc!=0) {
         pthread_mutex_unlock(&_clMutex);
         return NO;
      }
   }
   return YES;
}

@end
