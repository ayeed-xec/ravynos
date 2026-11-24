/* Copyright (c) 2007 Christopher J. W. Lloyd

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#import <AppKit/NSOpenGLPixelBuffer.h>
#import <AppKit/NSRaise.h>

static const unsigned long GLTexture2DTarget=0x0DE1;
static const unsigned long GLTextureCubeMapTarget=0x8513;
static const unsigned long GLTextureRectangleTarget=0x84F5;
static const unsigned long GLRGBInternalFormat=0x1907;
static const unsigned long GLRGBAInternalFormat=0x1908;
static const unsigned long GLRGB8InternalFormat=0x8051;
static const unsigned long GLRGBA8InternalFormat=0x8058;

static BOOL textureTargetIsValid(unsigned long target) {
   switch(target){
      case GLTexture2DTarget:
      case GLTextureCubeMapTarget:
      case GLTextureRectangleTarget:
    return YES;
   }

   return NO;
}

static BOOL textureInternalFormatIsValid(unsigned long format) {
   switch(format){
      case GLRGBInternalFormat:
      case GLRGBAInternalFormat:
      case GLRGB8InternalFormat:
      case GLRGBA8InternalFormat:
    return YES;
   }

   return NO;
}

@implementation NSOpenGLPixelBuffer
{
    unsigned long _textureTarget;
    unsigned long _textureInternalFormat;
    long _textureMaxMipMapLevel;
    int _pixelsWide;
    int _pixelsHigh;
}

-initWithTextureTarget:(unsigned long)textureTarget textureInternalFormat:(unsigned long)textureFormat textureMaxMipMapLevel:(long)mipMapLevel pixelsWide:(int)pixelsWide pixelsHigh:(int)pixelsHigh {
   [super init];

   if(!textureTargetIsValid(textureTarget)){
      NSLog(@"NSOpenGLPixelBuffer invalid texture target 0x%lx",textureTarget);
      return nil;
   }

   if(!textureInternalFormatIsValid(textureFormat)){
      NSLog(@"NSOpenGLPixelBuffer invalid texture internal format 0x%lx",textureFormat);
      return nil;
   }

   if(mipMapLevel<0){
      NSLog(@"NSOpenGLPixelBuffer negative mip map level %ld",mipMapLevel);
      return nil;
   }

   if(pixelsWide<=0 || pixelsHigh<=0){
      NSLog(@"NSOpenGLPixelBuffer invalid dimensions %dx%d",pixelsWide,pixelsHigh);
      return nil;
   }

   if(textureTarget==GLTextureCubeMapTarget && pixelsWide!=pixelsHigh){
      NSLog(@"NSOpenGLPixelBuffer cube map requires square dimensions %dx%d",pixelsWide,pixelsHigh);
      return nil;
   }

   _textureTarget=textureTarget;
   _textureInternalFormat=textureFormat;
   _textureMaxMipMapLevel=mipMapLevel;
   _pixelsWide=pixelsWide;
   _pixelsHigh=pixelsHigh;

   return self;
}

-(unsigned)textureTarget {
   return _textureTarget;
}

-(unsigned long)textureInternalFormat {
   return _textureInternalFormat;
}

-(long)textureMaxMipMapLevel {
   return _textureMaxMipMapLevel;
}

-(int)pixelsWide {
   return _pixelsWide;
}

-(int)pixelsHigh {
   return _pixelsHigh;
}

@end
