/* Copyright (c) 2006-2007 Christopher J. W. Lloyd

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */
#import <Foundation/NSHTTPURLResponse.h>
#import <Foundation/NSEnumerator.h>
#import <Foundation/NSDictionary.h>
#import <Foundation/NSRaise.h>

@implementation NSHTTPURLResponse

static NSString *valueInHeaders(NSDictionary *headers,NSString *key){
   NSEnumerator *state=[headers keyEnumerator];
   NSString     *checkKey;
   
   key=[key lowercaseString];
   
   while((checkKey=[state nextObject])!=nil){
    if([[checkKey lowercaseString] isEqualToString:key])
     return [headers objectForKey:checkKey];
   }
   
   return nil;
}

static NSString *mimeTypeFromContentType(NSString *contentType){
// FIXME
   return contentType;
}

static NSString *textEncodingNameFromContentType(NSString *contentType){
// FIXME
   return contentType;
}

-initWithURL:(NSURL *)url statusCode:(NSInteger)statusCode headers:(NSDictionary *)headers {
   NSString *contentType=valueInHeaders(headers,@"content-type");
   NSInteger contentLength=[valueInHeaders(headers,@"content-length") integerValue];
   NSString *mimeType=mimeTypeFromContentType(contentType);
   NSString *textEncodingName=textEncodingNameFromContentType(contentType);
   
   [super initWithURL:url MIMEType:mimeType expectedContentLength:contentLength textEncodingName:textEncodingName];
   
   _statusCode=statusCode;
   _allHeaderFields=[headers retain];
   return self;
}

+(NSString *)localizedStringForStatusCode:(NSInteger)statusCode {
   switch(statusCode){
    case 100: return @"Continue";
    case 101: return @"Switching Protocols";
    case 200: return @"OK";
    case 201: return @"Created";
    case 202: return @"Accepted";
    case 203: return @"Non-Authoritative Information";
    case 204: return @"No Content";
    case 205: return @"Reset Content";
    case 206: return @"Partial Content";
    case 300: return @"Multiple Choices";
    case 301: return @"Moved Permanently";
    case 302: return @"Found";
    case 303: return @"See Other";
    case 304: return @"Not Modified";
    case 305: return @"Use Proxy";
    case 307: return @"Temporary Redirect";
    case 400: return @"Bad Request";
    case 401: return @"Unauthorized";
    case 402: return @"Payment Required";
    case 403: return @"Forbidden";
    case 404: return @"Not Found";
    case 405: return @"Method Not Allowed";
    case 406: return @"Not Acceptable";
    case 407: return @"Proxy Authentication Required";
    case 408: return @"Request Timeout";
    case 409: return @"Conflict";
    case 410: return @"Gone";
    case 411: return @"Length Required";
    case 412: return @"Precondition Failed";
    case 413: return @"Request Entity Too Large";
    case 414: return @"Request URI Too Long";
    case 415: return @"Unsupported Media Type";
    case 416: return @"Requested Range Not Satisfiable";
    case 417: return @"Expectation Failed";
    case 500: return @"Internal Server Error";
    case 501: return @"Not Implemented";
    case 502: return @"Bad Gateway";
    case 503: return @"Service Unavailable";
    case 504: return @"Gateway Timeout";
    case 505: return @"HTTP Version Not Supported";
   }

   if(statusCode>=100 && statusCode<200)
    return @"Informational";

   if(statusCode>=200 && statusCode<300)
    return @"Success";

   if(statusCode>=300 && statusCode<400)
    return @"Redirection";

   if(statusCode>=400 && statusCode<500)
    return @"Client Error";

   if(statusCode>=500 && statusCode<600)
    return @"Server Error";

   return nil;
}

-(NSDictionary *)allHeaderFields {
   return _allHeaderFields;
}

-(NSInteger)statusCode {
   return _statusCode;
}

@end
