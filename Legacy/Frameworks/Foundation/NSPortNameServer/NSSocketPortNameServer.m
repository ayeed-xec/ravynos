/* Copyright (c) 2006-2007 Christopher J. W. Lloyd

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#import <sys/socket.h>
#import <netinet/in.h>
#import <Foundation/NSSocketPortNameServer.h>
#import <Foundation/NSRaiseException.h>
#import <Foundation/NSPathUtilities.h>
#import <Foundation/NSFileManager.h>
#import <Foundation/NSPropertyList.h>
#import <Foundation/NSUserDefaults.h>
#import <Foundation/NSLock.h>
#import <Foundation/NSSocketPort.h>

@implementation NSSocketPortNameServer

+sharedInstance {
    static NSSocketPortNameServer *shared=nil;

    if(shared==nil)
        shared=NSAllocateObject(self,0,NULL);

    return shared;
}

-(uint16_t)defaultNameServerPortNumber {
    return (uint16_t)[[NSUserDefaults standardUserDefaults] integerForKey:@"NSSocketPortNameServerPort"] ?: 1024;
}

-(void)setDefaultNameServerPortNumber:(uint16_t)number {
    [[NSUserDefaults standardUserDefaults] setInteger:number forKey:@"NSSocketPortNameServerPort"];
}

-(NSString *)registryPath {
    return [NSTemporaryDirectory() stringByAppendingPathComponent:@"ravynos-socketport-registry.plist"];
}

static NSMutableDictionary *RegistryLocked(BOOL *loaded) {
    NSString *path=[[[NSSocketPortNameServer class] sharedInstance] registryPath];
    NSDictionary *stored=nil;

    if([[NSFileManager defaultManager] fileExistsAtPath:path])
        stored=[NSPropertyListSerialization propertyListFromData:[NSData dataWithContentsOfFile:path]
                                                 options:0 format:NULL errorDescription:NULL];

    if(loaded!=NULL)
        *loaded=YES;

    if([stored isKindOfClass:[NSMutableDictionary class]])
        return [stored mutableCopy];

    return [NSMutableDictionary dictionary];
}

-(NSPort *)portForName:(NSString *)name host:(NSString *)host nameServerPortNumber:(uint16_t)number {
    if(name==nil)
        NSRaiseException(NSGenericException,nil,0,@"portForName: nil name");

    NSMutableDictionary *registry=RegistryLocked(NULL);
    NSData *address=[registry objectForKey:name];

    if(address==nil)
        return nil;

    return [[NSSocketPort alloc] initRemoteWithProtocolFamily:PF_INET socketType:SOCK_STREAM protocol:IPPROTO_TCP address:address];
}

-(NSPort *)portForName:(NSString *)name host:(NSString *)host {
    return [self portForName:name host:host nameServerPortNumber:[self defaultNameServerPortNumber]];
}

-(NSPort *)portForName:(NSString *)name {
    return [self portForName:name host:nil];
}

-(BOOL)registerPort:(NSPort *)port name:(NSString *)name nameServerPortNumber:(uint16_t)number {
    if(name==nil || ![port isKindOfClass:[NSSocketPort class]])
        NSRaiseException(NSGenericException,nil,0,@"registerPort: needs a named NSSocketPort");

    NSMutableDictionary *registry=RegistryLocked(NULL);

    [registry setObject:[(NSSocketPort *)port address] forKey:name];

    NSString *path=[self registryPath];
    NSData *data=[NSPropertyListSerialization dataFromPropertyList:registry format:NSPropertyListBinaryFormat_v1_0 errorDescription:NULL];

    if(data==nil)
        return NO;

    return [data writeToFile:path atomically:YES];
}

-(BOOL)registerPort:(NSPort *)port name:(NSString *)name {
    return [self registerPort:port name:name nameServerPortNumber:[self defaultNameServerPortNumber]];
}

-(BOOL)removePortForName:(NSString *)name {
    if(name==nil)
        NSRaiseException(NSGenericException,nil,0,@"removePortForName: nil name");

    NSMutableDictionary *registry=RegistryLocked(NULL);

    if([registry objectForKey:name]==nil)
        return NO;

    [registry removeObjectForKey:name];

    NSString *path=[self registryPath];
    NSData *data=[NSPropertyListSerialization dataFromPropertyList:registry format:NSPropertyListBinaryFormat_v1_0 errorDescription:NULL];

    if(data==nil)
        return NO;

    return [data writeToFile:path atomically:YES];
}

@end
