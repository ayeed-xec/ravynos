/* Copyright (c) 2006-2007 Christopher J. W. Lloyd

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */
#import <Foundation/NSURLCredentialStorage.h>
#import <Foundation/NSString.h>
#import <Foundation/NSMutableDictionary.h>
#import <Foundation/NSLock.h>
#import <Foundation/NSNotificationCenter.h>
#import <Foundation/NSRaise.h>
#import <Foundation/NSURL.h>

static NSURLCredentialStorage *_sharedCredentialStorage=nil;
static NSMutableDictionary *_allCredentials=nil;
static NSMutableDictionary *_defaultCredentials=nil;
static NSRecursiveLock *_credentialStorageLock=nil;

static NSString *keyForProtectionSpace(NSURLProtectionSpace *protectionSpace) {
   return [NSString stringWithFormat:@"%@|%d|%@|%@|%@",[protectionSpace host],[protectionSpace port],[protectionSpace protocol],[protectionSpace realm],[protectionSpace authenticationMethod]];
}

static NSMutableDictionary *spaceDictionaryForProtectionSpace(NSURLProtectionSpace *protectionSpace) {
   NSMutableDictionary *spaceDictionary=[_allCredentials objectForKey:keyForProtectionSpace(protectionSpace)];

   if(spaceDictionary==nil){
    spaceDictionary=[NSMutableDictionary dictionary];
    [_allCredentials setObject:spaceDictionary forKey:keyForProtectionSpace(protectionSpace)];
   }

   return spaceDictionary;
}

static void postStorageChanged() {
   [[NSNotificationCenter defaultCenter] postNotificationName:NSURLCredentialStorageChangedNotification object:_sharedCredentialStorage];
}

@implementation NSURLCredentialStorage

+(void)initialize {
   if(self==[NSURLCredentialStorage class]){
    _allCredentials=[NSMutableDictionary dictionary];
    _defaultCredentials=[NSMutableDictionary dictionary];
    _credentialStorageLock=[[NSRecursiveLock alloc] init];
   }
}

+(NSURLCredentialStorage *)sharedCredentialStorage {
   if(_sharedCredentialStorage==nil)
    _sharedCredentialStorage=[[NSURLCredentialStorage alloc] init];

   return _sharedCredentialStorage;
}

-init {
   [super init];
   return self;
}

-(NSDictionary *)allCredentials {
   [_credentialStorageLock lock];
   NSDictionary *result=[_allCredentials copy];
   [_credentialStorageLock unlock];

   return result;
}

-(NSDictionary *)credentialsForProtectionSpace:(NSURLProtectionSpace *)protectionSpace {
   [_credentialStorageLock lock];
   NSMutableDictionary *result=[_allCredentials objectForKey:keyForProtectionSpace(protectionSpace)];
   [_credentialStorageLock unlock];

   if(result==nil)
    return [NSDictionary dictionary];

   return [result copy];
}

-(NSURLCredential *)defaultCredentialForProtectionSpace:(NSURLProtectionSpace *)protectionSpace {
   [_credentialStorageLock lock];
   NSURLCredential *result=[_defaultCredentials objectForKey:keyForProtectionSpace(protectionSpace)];
   [_credentialStorageLock unlock];

   return result;
}

-(void)setCredential:(NSURLCredential *)credential forProtectionSpace:(NSURLProtectionSpace *)protectionSpace {
   if(credential==nil || protectionSpace==nil)
    [NSException raise:NSInvalidArgumentException format:@"credential and protectionSpace must not be nil"];

   [_credentialStorageLock lock];

   NSMutableDictionary *spaceDictionary=spaceDictionaryForProtectionSpace(protectionSpace);
   NSString *spaceKey=keyForProtectionSpace(protectionSpace);

   [spaceDictionary setObject:credential forKey:[credential user]];

   if([_defaultCredentials objectForKey:spaceKey]==nil)
    [_defaultCredentials setObject:credential forKey:spaceKey];

   [_credentialStorageLock unlock];

   postStorageChanged();
}

-(void)setDefaultCredential:(NSURLCredential *)credential forProtectionSpace:(NSURLProtectionSpace *)protectionSpace {
   if(credential==nil || protectionSpace==nil)
    [NSException raise:NSInvalidArgumentException format:@"credential and protectionSpace must not be nil"];

   [_credentialStorageLock lock];

   NSMutableDictionary *spaceDictionary=spaceDictionaryForProtectionSpace(protectionSpace);
   NSString *spaceKey=keyForProtectionSpace(protectionSpace);

   [spaceDictionary setObject:credential forKey:[credential user]];
   [_defaultCredentials setObject:credential forKey:spaceKey];

   [_credentialStorageLock unlock];

   postStorageChanged();
}

-(void)removeCredential:(NSURLCredential *)credential forProtectionSpace:(NSURLProtectionSpace *)protectionSpace {
   if(credential==nil || protectionSpace==nil)
    [NSException raise:NSInvalidArgumentException format:@"credential and protectionSpace must not be nil"];

   [_credentialStorageLock lock];

   NSString *spaceKey=keyForProtectionSpace(protectionSpace);

   [[_allCredentials objectForKey:spaceKey] removeObjectForKey:[credential user]];

   if([_defaultCredentials objectForKey:spaceKey]==credential)
    [_defaultCredentials removeObjectForKey:spaceKey];

   [_credentialStorageLock unlock];

   postStorageChanged();
}

@end
