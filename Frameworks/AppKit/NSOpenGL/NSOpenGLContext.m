/* Copyright (c) 2006-2007 Christopher J. W. Lloyd

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software") to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */
#import <AppKit/NSOpenGLContext.h>
#import <AppKit/NSOpenGLPixelFormat.h>
#import <AppKit/NSGraphicsContext.h>
#import <AppKit/NSRaise.h>
#import <AppKit/NSView.h>
#import <AppKit/NSWindow-Private.h>
#import <OpenGL/OpenGL.h>
#import <Foundation/NSThread-Private.h>

@interface NSOpenGLContext(private)
-(void)_clearCurrentContext;
@end

// State that lives beside _glContext because the header does not reserve
// space for it. All of it is pure bookkeeping that any CGL backend can honor.
@implementation NSOpenGLContext {
    NSOpenGLPixelBuffer *_pixelBuffer;
    unsigned long _pixelBufferCubeMapFace;
    long _pixelBufferMipMapLevel;
    int _virtualScreen;
    void *_offscreenBytes;
    long _offscreenWidth;
    long _offscreenHeight;
    long _offscreenRowBytes;
}

static inline NSOpenGLContext *_currentContext(){
   return (NSOpenGLContext *)NSThreadSharedInstanceDoNotCreate(@"NSOpenGLContext");
}

static void _setCurrentContext(NSOpenGLContext *context){
   [NSCurrentThread() setSharedObject:context forClassName:@"NSOpenGLContext"];
}

static inline void _clearCurrentContext(){
   [_currentContext() _clearCurrentContext];
   _setCurrentContext(nil);
}

+(NSOpenGLContext *)currentContext {
   return _currentContext();
}

+(void)clearCurrentContext {
   _clearCurrentContext();
}

-initWithFormat:(NSOpenGLPixelFormat *)pixelFormat shareContext:(NSOpenGLContext *)shareContext {
   CGLError error;

   if(pixelFormat==nil){
      NSLog(@"NSOpenGLContext initWithFormat:shareContext: requires a pixel format");
      return nil;
   }

   if(_pixelFormat!=nil){
      // Cocoa's NSOpenGLContext can withstand a double init and I know of at least one app that does it
      // Maybe Cocoa just leaks, we don't
      [_pixelFormat release];
      _pixelFormat=nil;
      CGLReleaseContext(_glContext);
      _glContext=NULL;
   }

   _pixelFormat=[pixelFormat retain];
   if((error=CGLCreateContext([_pixelFormat CGLPixelFormatObj],[shareContext CGLContextObj],(CGLContextObj *)&_glContext))!=kCGLNoError){
      NSLog(@"CGLCreateContext failed with %d in %s %d",error,__FILE__,__LINE__);
      [_pixelFormat release];
      _pixelFormat=nil;
      return nil;
   }

   _pixelBuffer=nil;
   _pixelBufferCubeMapFace=0;
   _pixelBufferMipMapLevel=0;
   _virtualScreen=0;
   _offscreenBytes=NULL;
   _offscreenWidth=0;
   _offscreenHeight=0;
   _offscreenRowBytes=0;

   return self;
}

-(void)dealloc {
// FIXME: this doesn't actually work because if we are the current context we're retained by the thread shared object dict
   if(_currentContext()==self)
      _clearCurrentContext();
   [_pixelBuffer release];
   _pixelBuffer=nil;
   [_pixelFormat release];
   _pixelFormat=nil;
   _view=nil;

   CGLReleaseContext(_glContext);
   _glContext=NULL;
   [super dealloc];
}

-(NSView *)view {
   return _view;
}

-(NSOpenGLPixelBuffer *)pixelBuffer {
   return _pixelBuffer;
}

-(unsigned long)pixelBufferCubeMapFace {
   return _pixelBufferCubeMapFace;
}

-(long)pixelBufferMipMapLevel {
   return _pixelBufferMipMapLevel;
}

-(void *)CGLContextObj {
   return _glContext;
}

-(void)getValues:(GLint *)vals forParameter:(NSOpenGLContextParameter)parameter {
   CGLGetParameter(_glContext,parameter,vals);
}

-(void)setValues:(const GLint *)vals forParameter:(NSOpenGLContextParameter)parameter {
   CGLSetParameter(_glContext,parameter,vals);
}

-(void)updateViewParameters {
    NSRect rect=[_view bounds];

    if([_view window]!=nil)
        rect=[_view convertRect:rect toView:nil];

   GLint size[2]={
    rect.size.width,
    rect.size.height };
   GLint origin[2]={
    rect.origin.x,
    rect.origin.y };
   GLint hidden[1]= {
    [_view isHidden] ? 1 : 0
   };

   CGLSetParameter(_glContext,kCGLCPSurfaceBackingSize,size);
   CGLSetParameter(_glContext,kCGLCPSurfaceBackingOrigin,origin);
   CGLSetParameter(_glContext,kCGLCPSurfaceHidden,hidden);
}

-(void)setView:(NSView *)view {
   if(_view!=view)
    _hasPrepared=NO;

   _view=view;

   CGLLockContext(_glContext);

   GLint num[1]={[[view window] windowNumber]};

   CGLSetParameter(_glContext,kCGLCPSurfaceWindowNumber,num);

   [self update];

   CGLUnlockContext(_glContext);
}

-(void)makeCurrentContext {
   CGLError error;

   if((error=CGLSetCurrentContext(_glContext))!=kCGLNoError)
    NSLog(@"CGLSetCurrentContext failed with %d in %s %d",error,__FILE__,__LINE__);

   _setCurrentContext(self);

   if(!_hasPrepared){
    _hasPrepared=YES;

// NSOpenGLContext will call prepareOpenGL on any view, not just NSOpenGLView
    if([_view respondsToSelector:@selector(prepareOpenGL)])
     [_view performSelector:@selector(prepareOpenGL)];
   }

}

-(void)_clearCurrentContext {
   CGLError error;

   if((error=CGLSetCurrentContext(NULL))!=kCGLNoError)
    NSLog(@"CGLSetCurrentContext failed with %d in %s %d",error,__FILE__,__LINE__);
}

-(int)currentVirtualScreen {
   return _virtualScreen;
}

-(void)setCurrentVirtualScreen:(int)screen {
   GLint numberOfScreens=1;

   if(_pixelFormat!=nil)
    numberOfScreens=[_pixelFormat numberOfVirtualScreens];

   if(numberOfScreens<1)
    numberOfScreens=1;

   if(screen<0 || screen>=numberOfScreens){
      NSLog(@"setCurrentVirtualScreen: %d out of range, pixel format has %d virtual screen(s)",screen,numberOfScreens);
      return;
   }

   _virtualScreen=screen;
}

-(void)setFullScreen {
   if(_view!=nil)
    NSLog(@"NSOpenGLContext has view, full-screen not allowed");

   // Needs a renderer bound to a real display; there is no in-tree CGL
   // backend that can capture one, so the context keeps its current target.
   NSUnimplementedMethod();
}

-(void)setOffscreen:(void *)bytes width:(long)width height:(long)height rowbytes:(long)rowbytes {
   if(bytes==NULL || width<=0 || height<=0 || rowbytes<width*4){
      NSLog(@"setOffscreen:width:height:rowbytes: invalid geometry %p %ldx%ld rb=%ld",bytes,width,height,rowbytes);
      return;
   }

   // Record the target so getValues:/update report consistent state; binding
   // the raster to the renderer happens in CGL, which has no in-tree backend.
   _offscreenBytes=bytes;
   _offscreenWidth=width;
   _offscreenHeight=height;
   _offscreenRowBytes=rowbytes;
}

-(void)setPixelBuffer:(NSOpenGLPixelBuffer *)pixelBuffer cubeMapFace:(unsigned long)cubeMapFace mipMapLeve:(long)mipMapLevel currentVirtualScreen:(int)screen {
   if(pixelBuffer!=nil && screen>=0){
      [pixelBuffer retain];
      [_pixelBuffer release];
      _pixelBuffer=pixelBuffer;
      _pixelBufferCubeMapFace=cubeMapFace;
      _pixelBufferMipMapLevel=mipMapLevel;
      [self setCurrentVirtualScreen:screen];
   }
   else
      NSLog(@"setPixelBuffer: needs a pixel buffer and a non-negative virtual screen");
}

-(void)setTextureImageToPixelBuffer:(NSOpenGLPixelBuffer *)pixelBuffer colorBuffer:(unsigned long)source {
   if(pixelBuffer==nil){
      NSLog(@"setTextureImageToPixelBuffer: nil pixel buffer");
      return;
   }

   // Real GPU work: CGLTexImagePBuffer needs a live CGL backend plus a
   // CGLPBufferObj, which NSOpenGLPixelBuffer does not expose in-tree.
   NSUnimplementedMethod();
}

-(void)update {
    [self updateViewParameters];
}

-(void)clearDrawable {
    [self setView:nil];
}

-(void)copyAttributesFromContext:(NSOpenGLContext *)context withMask:(unsigned long)mask {
   if(context==nil){
      NSLog(@"copyAttributesFromContext: nil context");
      return;
   }

   // mask follows the CGLContextBit scheme; 0 or the context bit copies every
   // parameter we can query, anything else selects none we know about.
   static const CGLContextParameter parameters[]={
    kCGLCPSurfaceBackingSize,
    kCGLCPSurfaceBackingOrigin,
    kCGLCPSurfaceHidden,
    kCGLCPSurfaceWindowNumber,
    kCGLCPSurfaceOpacity,
   };
   unsigned i;

   if(mask!=0 && mask!=1)
    return;

   for(i=0;i<sizeof(parameters)/sizeof(parameters[0]);i++){
      GLint value[4]={0,0,0,0};

      CGLGetParameter([context CGLContextObj],parameters[i],value);
      CGLSetParameter(_glContext,parameters[i],value);
   }
}

-(void)createTexture:(unsigned long)identifier fromView:(NSView *)view internalFormat:(unsigned long)internalFormat {
   // Real GPU work: renders the view into a texture through the renderer.
   NSUnimplementedMethod();
}

-(void)flushBuffer {
   CGLFlushDrawable(_glContext);

}

@end
