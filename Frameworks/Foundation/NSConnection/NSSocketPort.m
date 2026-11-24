/* Copyright (c) 2006-2007 Christopher J. W. Lloyd

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#import <Foundation/NSSocketPort.h>
#import <Foundation/NSRaise.h>
#import <Foundation/NSStream.h>
#import <Foundation/NSPlatform.h>
#import <Foundation/NSData.h>
#import <Foundation/NSString.h>
#import <Foundation/NSNotificationCenter.h>
#import <Foundation/NSPort.h>

#import <sys/types.h>
#import <sys/socket.h>
#import <netinet/in.h>
#import <arpa/inet.h>
#import <netdb.h>
#import <unistd.h>
#import <string.h>
#import <stdio.h>

@interface NSSocketPort () {
    NSSocketNativeHandle _nativeSocket;
    int _portFamily;
    int _portType;
    int _portProtocol;
    NSData *_addressData;
}
@end

static NSData *AddressDataForSocket(NSSocketNativeHandle handle) {
    struct sockaddr_storage storage;
    socklen_t length=sizeof(storage);

    if(getsockname(handle,(struct sockaddr *)&storage,&length)<0)
        return nil;

    return [NSData dataWithBytes:&storage length:length];
}

@implementation NSSocketPort

+allocWithZone:(NSZone *)zone {
    if(self==[NSSocketPort class])
        return NSAllocateObject([[NSPlatform currentPlatform] socketPortClass],0,zone);
    else
        return NSAllocateObject(self,0,zone);
}

-init {
    _nativeSocket=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);

    if(_nativeSocket<0){
        [self release];
        return nil;
    }

    _portFamily=PF_INET;
    _portType=SOCK_STREAM;
    _portProtocol=IPPROTO_TCP;
    _isValid=YES;

    return self;
}

-initWithTCPPort:(unsigned short)port {
    struct sockaddr_in address;
    int option=1;

    _nativeSocket=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);

    if(_nativeSocket<0){
        [self release];
        return nil;
    }

    setsockopt(_nativeSocket,SOL_SOCKET,SO_REUSEADDR,&option,sizeof(option));

    bzero(&address,sizeof(address));
    address.sin_family=AF_INET;
    address.sin_port=htons(port);
    address.sin_addr.s_addr=htonl(INADDR_ANY);

    if(bind(_nativeSocket,(struct sockaddr *)&address,sizeof(address))<0 || listen(_nativeSocket,5)<0){
        close(_nativeSocket);
        _nativeSocket=-1;
        [self release];
        return nil;
    }

    _portFamily=PF_INET;
    _portType=SOCK_STREAM;
    _portProtocol=IPPROTO_TCP;
    _addressData=[AddressDataForSocket(_nativeSocket) retain];
    _isValid=YES;

    return self;
}

-initRemoteWithTCPPort:(uint16_t)port host:(NSString *)hostName {
    struct addrinfo hints;
    struct addrinfo *results=NULL;
    struct sockaddr_storage storage;
    char portString[6];

    bzero(&hints,sizeof(hints));
    hints.ai_family=AF_INET;
    hints.ai_socktype=SOCK_STREAM;

    snprintf(portString,sizeof(portString),"%u",port);

    if(getaddrinfo([hostName UTF8String],portString,&hints,&results)!=0 || results==NULL){
        [self release];
        return nil;
    }

    memcpy(&storage,results->ai_addr,results->ai_addrlen);
    freeaddrinfo(results);

    _nativeSocket=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);

    if(_nativeSocket<0){
        [self release];
        return nil;
    }

    if(connect(_nativeSocket,(struct sockaddr *)&storage,sizeof(storage))<0){
        close(_nativeSocket);
        _nativeSocket=-1;
        [self release];
        return nil;
    }

    _portFamily=PF_INET;
    _portType=SOCK_STREAM;
    _portProtocol=IPPROTO_TCP;
    _addressData=[AddressDataForSocket(_nativeSocket) retain];
    _isValid=YES;

    return self;
}

-initWithProtocolFamily:(int)family socketType:(int)type protocol:(int)protocol address:(NSData *)address {
    if(address==nil || [address length]<sizeof(struct sockaddr)){
        [self release];
        return nil;
    }

    _nativeSocket=socket(family,type,protocol);

    if(_nativeSocket<0){
        [self release];
        return nil;
    }

    if(bind(_nativeSocket,[address bytes],[address length])<0 || listen(_nativeSocket,5)<0){
        close(_nativeSocket);
        _nativeSocket=-1;
        [self release];
        return nil;
    }

    _portFamily=family;
    _portType=type;
    _portProtocol=protocol;
    _addressData=[AddressDataForSocket(_nativeSocket) retain];
    _isValid=YES;

    return self;
}

-initRemoteWithProtocolFamily:(int)family socketType:(int)type protocol:(int)protocol address:(NSData *)address {
    if(address==nil || [address length]<sizeof(struct sockaddr)){
        [self release];
        return nil;
    }

    _nativeSocket=socket(family,type,protocol);

    if(_nativeSocket<0){
        [self release];
        return nil;
    }

    if(connect(_nativeSocket,[address bytes],[address length])<0){
        close(_nativeSocket);
        _nativeSocket=-1;
        [self release];
        return nil;
    }

    _portFamily=family;
    _portType=type;
    _portProtocol=protocol;
    _addressData=[AddressDataForSocket(_nativeSocket) retain];
    _isValid=YES;

    return self;
}

-initWithProtocolFamily:(int)family socketType:(int)type protocol:(int)protocol socket:(NSSocketNativeHandle)nativeSocket {
    _nativeSocket=nativeSocket;
    _portFamily=family;
    _portType=type;
    _portProtocol=protocol;
    _addressData=[AddressDataForSocket(nativeSocket) retain];
    _isValid=YES;

    return self;
}

-(NSData *)address {
    return _addressData;
}

-(int)protocol {
    return _portProtocol;
}

-(int)protocolFamily {
    return _portFamily;
}

-(NSSocketNativeHandle)socket {
    return _nativeSocket;
}

-(int)socketType {
    return _portType;
}

-delegate {
    return _delegate;
}

-(void)setDelegate:delegate {
    [delegate retain];
    [_delegate release];
    _delegate=delegate;
}

-(BOOL)isValid {
    return _isValid;
}

-(void)invalidate {
    if(_isValid){
        _isValid=NO;
        close(_nativeSocket);
        _nativeSocket=-1;
        [[NSNotificationCenter defaultCenter] postNotificationName:NSPortDidBecomeInvalidNotification object:self];
    }
}

-(void)stream:(NSStream *)stream handleEvent:(NSStreamEvent)streamEvent {
    NSUnimplementedMethod();
}

-(void)dealloc {
    if(_isValid){
        _isValid=NO;
        close(_nativeSocket);
        _nativeSocket=-1;
    }
    [_delegate release];
    [_addressData release];
    [super dealloc];
}

@end
