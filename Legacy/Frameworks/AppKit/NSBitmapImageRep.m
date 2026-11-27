/* Copyright (c) 2006-2007 Christopher J. W. Lloyd

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#import <sys/param.h>
#import <AppKit/NSBitmapImageRep-Private.h>
#import <AppKit/NSGraphicsContextFunctions.h>
#import <AppKit/NSView.h>
#import <AppKit/NSColor.h>
#import <AppKit/NSRaise.h>
#import <Onyx2D/O2ImageDestination.h>
#import <AppKit/NSPasteboard.h>

NSString* NSImageCompressionFactor = @"NSImageCompressionFactor";

@implementation NSBitmapImageRep

+(NSArray *)imageUnfilteredFileTypes {
    return [NSArray arrayWithObjects:
            // Try to order them so the most used ones are at the top of the list
            @"png",
            @"tiff",
            @"tif",
            @"jpg",
            @"jpeg",
            @"icns",
            @"gif",
            @"bmp",
            @"PNG",
            @"TIFF",
            @"TIF",
            @"JPG",
            @"JPEG",
            @"ICNS",
            @"jpe",
            @"JPE",
            @"GIF",
            @"BMP",
            nil];
}

+(NSArray *)imageRepsWithContentsOfFile:(NSString *)path {
   NSData *data=[NSData dataWithContentsOfFile:path];

   if(data==nil)
    return nil;

   if([self canInitWithData:data])
    return [self imageRepsWithData:data];

   return nil;
}

+(void)getTIFFCompressionTypes:(const NSTIFFCompression **)types count:(int *)count {
   static NSTIFFCompression all[]={
       NSTIFFCompressionNone,
       NSTIFFCompressionCCITTFAX3,
       NSTIFFCompressionCCITTFAX4,
       NSTIFFCompressionLZW,
       NSTIFFCompressionJPEG,
       NSTIFFCompressionNEXT,
       NSTIFFCompressionPackBits,
       NSTIFFCompressionOldJPEG
   };

   if(types!=NULL)
       *types=all;

   if(count!=NULL)
       *count=(int)(sizeof(all)/sizeof(all[0]));
}

+(NSString *)localizedNameForTIFFCompressionType:(NSTIFFCompression)type {
   switch(type) {
   case NSTIFFCompressionNone:
       return @"No compression";
   case NSTIFFCompressionCCITTFAX3:
       return @"CCITT Group 3 fax";
   case NSTIFFCompressionCCITTFAX4:
       return @"CCITT Group 4 fax";
   case NSTIFFCompressionLZW:
       return @"LZW";
   case NSTIFFCompressionJPEG:
       return @"JPEG";
   case NSTIFFCompressionNEXT:
       return @"NeXT";
   case NSTIFFCompressionPackBits:
       return @"PackBits";
   case NSTIFFCompressionOldJPEG:
       return @"Old-style JPEG";
   default:
       return @"Unknown";
   }
}

+(NSData *)TIFFRepresentationOfImageRepsInArray:(NSArray *)array {
   return [self TIFFRepresentationOfImageRepsInArray:array usingCompression:NSTIFFCompressionNone factor:0.0];
}

+(NSData *)TIFFRepresentationOfImageRepsInArray:(NSArray *)array usingCompression:(NSTIFFCompression)compression factor:(float)factor {
   NSMutableData        *result=[NSMutableData data];
   O2ImageDestinationRef dest=O2ImageDestinationCreateWithData((CFMutableDataRef)result,(CFStringRef)@"public.tiff",[array count],NULL);

   for(NSBitmapImageRep *bitmap in array){
    O2ImageDestinationAddImage(dest,[bitmap CGImage],NULL);
   }

   O2ImageDestinationFinalize(dest);
   CFRelease(dest);

   return result;
}

+(NSData *)representationOfImageRepsInArray:(NSArray *)array usingType:(NSBitmapImageFileType)type properties:(NSDictionary *)properties {
   for(NSImageRep *rep in array) {
      if([rep isKindOfClass:[NSBitmapImageRep class]]) {
         NSBitmapImageRep *bitmap=(NSBitmapImageRep *)rep;

         switch(type) {
         case NSTIFFFileType:
            return [bitmap TIFFRepresentationUsingCompression:NSTIFFCompressionNone factor:0.0];
         case NSPNGFileType:
            return [bitmap representationUsingType:NSPNGFileType properties:properties];
         case NSJPEGFileType:
            return [bitmap representationUsingType:NSJPEGFileType properties:properties];
         default:
            return nil;
         }
      }
   }

   return nil;
}

+(NSArray *)imageUnfilteredPasteboardTypes {
    return [NSArray arrayWithObjects:
            NSTIFFPboardType,
            nil];
}

+(BOOL)canInitWithData:(NSData *)data {
   O2ImageSourceRef imageSource=[O2ImageSource newImageSourceWithData:(CFDataRef)data options:nil];
   BOOL result=(imageSource!=NULL)?YES:NO;
   CFRelease(imageSource);
   return result;
}

+(NSArray *)imageRepsWithData:(NSData *)data {
   NSMutableArray *result=[NSMutableArray array];
   O2ImageSourceRef imageSource=[O2ImageSource newImageSourceWithData:(CFDataRef)data options:nil];

   if(imageSource==nil)
    return nil;

   size_t i,count=[imageSource count];

   for(i=0;i<count;i++){
    CGImageRef cgImage=O2ImageSourceCreateImageAtIndex(imageSource,i,nil);

    if(cgImage==nil)
     break;

    CFDictionaryRef properties=[imageSource copyPropertiesAtIndex:i options:nil];
    NSNumber        *xres=[[(id)CFDictionaryGetValue(properties,kO2ImagePropertyDPIWidth) copy] autorelease];
    NSNumber        *yres=[[(id)CFDictionaryGetValue(properties,kO2ImagePropertyDPIHeight) copy] autorelease];

    CFRelease(properties);

    NSBitmapImageRep *imageRep=[[self alloc] initWithCGImage:cgImage];
    NSSize size={ CGImageGetWidth(cgImage),CGImageGetHeight(cgImage) };

    CGImageRelease(cgImage);

    if(xres!=nil && [xres doubleValue]>0)
     size.width*=72.0/[xres doubleValue];

    if(yres!=nil && [yres doubleValue]>0)
     size.height*=72.0/[yres doubleValue];

    [imageRep setSize:size];

    if(imageRep!=nil)
     [result addObject:imageRep];

       [imageRep release];
}

   CFRelease(imageSource);

   return result;
}

+imageRepWithData:(NSData *)data {
   return [[[self alloc] initWithData:data] autorelease];
}

-initWithBitmapDataPlanes:(unsigned char **)planes pixelsWide:(int)width pixelsHigh:(int)height bitsPerSample:(int)bitsPerSample samplesPerPixel:(int)samplesPerPixel hasAlpha:(BOOL)hasAlpha isPlanar:(BOOL)isPlanar colorSpaceName:(NSString *)colorSpaceName bitmapFormat:(NSBitmapFormat)bitmapFormat bytesPerRow:(int)bytesPerRow bitsPerPixel:(int)bitsPerPixel {
   int i,numberOfPlanes=isPlanar?samplesPerPixel:1;

   _size=NSMakeSize(width,height);
   _colorSpaceName=[colorSpaceName copy];
   _bitsPerSample=bitsPerSample;
   _pixelsWide=width;
   _pixelsHigh=height;
   _hasAlpha=hasAlpha;

   _samplesPerPixel=samplesPerPixel;
   _isPlanar=isPlanar;
   _bitmapFormat=bitmapFormat;

   if(bitsPerPixel!=0)
    _bitsPerPixel=bitsPerPixel;
   else
    _bitsPerPixel=_bitsPerSample*_samplesPerPixel;

   if(bytesPerRow!=0)
    _bytesPerRow=bytesPerRow;
   else
    _bytesPerRow=(_bitsPerPixel*_pixelsWide+7)/8;

   _freeWhenDone=NO;
   if(planes==NULL)
    _freeWhenDone=YES;
   else {
    int i;

    for(i=0;i<numberOfPlanes;i++)
     if(planes[i]!=NULL)
      break;

    if(i==numberOfPlanes)
     _freeWhenDone=YES;
   }

   _bitmapPlanes=NSZoneCalloc(NULL,numberOfPlanes,sizeof(unsigned char *));
   for(i=0;i<numberOfPlanes;i++){
    if(!_freeWhenDone)
     _bitmapPlanes[i]=planes[i];
    else
     _bitmapPlanes[i]=NSZoneCalloc(NULL,_bytesPerRow*_pixelsHigh,1);
   }

   return self;
}

-initWithBitmapDataPlanes:(unsigned char **)planes pixelsWide:(int)width pixelsHigh:(int)height bitsPerSample:(int)bitsPerSample samplesPerPixel:(int)samplesPerPixel hasAlpha:(BOOL)hasAlpha isPlanar:(BOOL)isPlanar colorSpaceName:(NSString *)colorSpaceName bytesPerRow:(int)bytesPerRow bitsPerPixel:(int)bitsPerPixel {
   return [self initWithBitmapDataPlanes:planes pixelsWide:width pixelsHigh:height bitsPerSample:bitsPerSample samplesPerPixel:samplesPerPixel hasAlpha:hasAlpha isPlanar:isPlanar colorSpaceName:colorSpaceName bitmapFormat:0 bytesPerRow:bytesPerRow bitsPerPixel:bitsPerPixel];
}

-initForIncrementalLoad {
   return [self initWithBitmapDataPlanes:NULL pixelsWide:0 pixelsHigh:0 bitsPerSample:0 samplesPerPixel:0 hasAlpha:NO isPlanar:NO colorSpaceName:NSCalibratedRGBColorSpace bytesPerRow:0 bitsPerPixel:0];
}

-initWithFocusedViewRect:(NSRect)rect {
   CGContextRef graphicsPort=NSCurrentGraphicsPort();

   if(graphicsPort==NULL){
    [self dealloc];
    return nil;
   }

   [self initWithData:(NSData *)CGContextCaptureBitmap(graphicsPort,rect)];

   return self;
}


-initWithData:(NSData *)data {
   O2ImageSourceRef imageSource=[O2ImageSource newImageSourceWithData:(CFDataRef)data options:nil];

   if(imageSource==nil){
    [self dealloc];
    return nil;
   }

   CGImageRef       cgImage=O2ImageSourceCreateImageAtIndex(imageSource,0,nil);

   if(cgImage==nil){
    CFRelease(imageSource);
    [self dealloc];
    return nil;
   }

   CFDictionaryRef properties=[imageSource copyPropertiesAtIndex:0 options:nil];
   NSNumber        *xres=[[(id)CFDictionaryGetValue(properties,kO2ImagePropertyDPIWidth) copy] autorelease];
   NSNumber        *yres=[[(id)CFDictionaryGetValue(properties,kO2ImagePropertyDPIHeight) copy] autorelease];

   CFRelease(properties);
   CFRelease(imageSource);

   if(cgImage==nil){
    [self dealloc];
    return nil;
   }

   if((self=[self initWithCGImage:cgImage])==nil)
    return nil;

   CGImageRelease(cgImage);

   if(xres!=nil && [xres doubleValue]>0)
    _size.width*=72.0/[xres doubleValue];

   if(yres!=nil && [yres doubleValue]>0)
    _size.height*=72.0/[yres doubleValue];

   return self;
}

-initWithContentsOfFile:(NSString *)path {
   NSData *data=[NSData dataWithContentsOfFile:path];

   if(data==nil){
    [self dealloc];
    return nil;
   }

   return [self initWithData:data];
}

-(void)createBitmapIfNeeded {
   if(_bitmapPlanes==NULL){
    _freeWhenDone=YES;
    _bitmapPlanes=NSZoneCalloc(NULL,1,sizeof(unsigned char *));
    _bitmapPlanes[0]=NSZoneCalloc(NULL,_bytesPerRow*_pixelsHigh,1);

    if(_cgImage!=NULL){
     CGBitmapInfo         bitmapInfo=CGImageGetBitmapInfo(_cgImage);
     CGDataProviderRef    provider=CGImageGetDataProvider(_cgImage);
// FIXME: inefficient but there is no API to get mutable bytes out of an image or image source
     CFDataRef            bitmapData=CGDataProviderCopyData(provider);
     const unsigned char *bytes=CFDataGetBytePtr(bitmapData);
     int                  i,length=_bytesPerRow*_pixelsHigh;

     if(bitmapInfo==(kCGImageAlphaPremultipliedLast|kCGBitmapByteOrder32Big)){
     for(i=0;i<length;i++)
      _bitmapPlanes[0][i]=bytes[i];
     }
     else {
      for(i=0;i<length;i+=4){
       unsigned char b=bytes[i+0];
       unsigned char g=bytes[i+1];
       unsigned char r=bytes[i+2];
       unsigned char a=bytes[i+3];

       _bitmapPlanes[0][i+0]=r;
       _bitmapPlanes[0][i+1]=g;
       _bitmapPlanes[0][i+2]=b;
       _bitmapPlanes[0][i+3]=a;
      }
     }
     CFRelease(bitmapData);
    }
   }
}

-initWithCGImage:(CGImageRef)cgImage {
   _cgImage=CGImageRetain(cgImage);
   _size.width=CGImageGetWidth(_cgImage);
   _size.height=CGImageGetHeight(_cgImage);

   CGColorSpaceRef colorSpace=CGImageGetColorSpace(_cgImage);

   // FIXME:
   _colorSpaceName=NSDeviceRGBColorSpace;
   _bitsPerSample=CGImageGetBitsPerComponent(_cgImage);
   _pixelsWide=CGImageGetWidth(_cgImage);
   _pixelsHigh=CGImageGetHeight(_cgImage);

   switch(CGImageGetAlphaInfo(_cgImage)){
    case kCGImageAlphaPremultipliedLast:
    case kCGImageAlphaPremultipliedFirst:
    case kCGImageAlphaLast:
    case kCGImageAlphaFirst:
     _hasAlpha=YES;
     break;
    default:
     _hasAlpha=NO;
     break;
   }

   _samplesPerPixel=CGColorSpaceGetNumberOfComponents(colorSpace);
   if(_hasAlpha)
    _samplesPerPixel++;
   _isPlanar=NO;
   _bitmapFormat=0;
   if(CGImageGetBitmapInfo(_cgImage)&kCGBitmapFloatComponents){
    _bitmapFormat|=NSFloatingPointSamplesBitmapFormat;
   }
   switch(CGImageGetAlphaInfo(_cgImage)){
    case kCGImageAlphaNone:
     break;
    case kCGImageAlphaPremultipliedLast:
     break;
    case kCGImageAlphaPremultipliedFirst:
     _bitmapFormat|=NSAlphaFirstBitmapFormat;
     break;
    case kCGImageAlphaLast:
     _bitmapFormat|=NSAlphaNonpremultipliedBitmapFormat;
     break;
    case kCGImageAlphaFirst:
     _bitmapFormat|=NSAlphaFirstBitmapFormat;
     _bitmapFormat|=NSAlphaNonpremultipliedBitmapFormat;
     break;
    case kCGImageAlphaNoneSkipLast:
    case kCGImageAlphaNoneSkipFirst:
     break;
    case kCGImageAlphaOnly:
     break;
   }

   _bitsPerPixel=CGImageGetBitsPerPixel(_cgImage);
   _bytesPerRow=CGImageGetBytesPerRow(_cgImage);
   _freeWhenDone=NO;

//   [self createBitmapIfNeeded];

   return self;
}


-(void)dealloc {
   if(_freeWhenDone){
    if(_bitmapPlanes!=NULL){
     int i,numberOfPlanes=[self numberOfPlanes];

     for(i=0;i<numberOfPlanes;i++)
      if(_bitmapPlanes[i]!=NULL)
       NSZoneFree(NULL,_bitmapPlanes[i]);

}
   }
    if(_bitmapPlanes!=NULL){
    NSZoneFree(NULL,_bitmapPlanes);
    }
   CGImageRelease(_cgImage);
   [super dealloc];
}


-(int)incrementalLoadFromData:(NSData *)data complete:(BOOL)complete {
   if(data==nil || [data length]==0)
       return 0;

   NSBitmapImageRep *loaded=[[NSBitmapImageRep alloc] initWithData:data];

   if(loaded==nil)
       return -1;

   int loadedWidth=[loaded pixelsWide];
   int loadedHeight=[loaded pixelsHigh];

   if(loadedWidth==0 || loadedHeight==0) {
       [loaded release];
       return -1;
   }

   [self setSize:NSMakeSize([self pixelsWide],[self pixelsHigh])];
   [loaded release];

   return complete ? 1 : 0;
}


-(int)bitsPerPixel {
   return _bitsPerPixel;
}

-(int)samplesPerPixel {
   return _samplesPerPixel;
}

-(int)bytesPerRow {
   return _bytesPerRow;
}

-(BOOL)isPlanar {
   return _isPlanar;
}

-(int)numberOfPlanes {
   return _isPlanar?_samplesPerPixel:1;
}

-(int)bytesPerPlane {
   return _bytesPerPlane;
}

-(NSBitmapFormat)bitmapFormat {
   return _bitmapFormat;
}

-(unsigned char *)bitmapData {
   [self createBitmapIfNeeded];

   return _bitmapPlanes[0];
}

-(void)getBitmapDataPlanes:(unsigned char **)planes {
   [self createBitmapIfNeeded];

   int i,numberOfPlanes=[self numberOfPlanes];

   for(i=0;i<numberOfPlanes;i++)
    planes[i]=_bitmapPlanes[i];

   for(;i<5;i++)
    planes[i]=NULL;
}

-(void)getPixel:(NSUInteger[])pixel atX:(NSInteger)x y:(NSInteger)y
{
   NSAssert(x>=0 && x<[self pixelsWide],@"x out of bounds");
   NSAssert(y>=0 && y<[self pixelsHigh],@"y out of bounds");

   [self createBitmapIfNeeded];

   if(_isPlanar) {
       int i;

       for(i=0;i<_samplesPerPixel;i++) {
           unsigned char *plane=_bitmapPlanes[i];

           pixel[i]=plane[_bytesPerPlane*y+x];
       }
   } else {
       if(_bitsPerPixel/_samplesPerPixel!=8) {
           [NSException raise:NSInternalInconsistencyException format:@"unsupported bits per sample %d",_bitsPerPixel/_samplesPerPixel];
       }

       unsigned char *bits=_bitmapPlanes[0]+_bytesPerRow*y+(x*_bitsPerPixel)/8;

       int i;

       for(i=0;i<_samplesPerPixel;i++) {
           pixel[i]=bits[i];
       }
   }
}

-(void)setPixel:(NSUInteger[])pixel atX:(NSInteger)x y:(NSInteger)y
{
   NSAssert(x>=0 && x<[self pixelsWide],@"x out of bounds");
   NSAssert(y>=0 && y<[self pixelsHigh],@"y out of bounds");

   [self createBitmapIfNeeded];

    if(_isPlanar) {
        int i;

        for(i=0;i<_samplesPerPixel;i++) {
            unsigned char *plane=_bitmapPlanes[i];

            plane[_bytesPerPlane*y+x]=(unsigned char)pixel[i];
        }
    } else {
        if(_bitsPerPixel/_samplesPerPixel!=8) {
            [NSException raise:NSInternalInconsistencyException format:@"unsupported bits per sample %d",_bitsPerPixel/_samplesPerPixel];
        }
        unsigned char *bits=_bitmapPlanes[0]+_bytesPerRow*y+(x*_bitsPerPixel)/8;

        int i;

        for( i = 0; i < _samplesPerPixel; i++) {
            bits[i]=pixel[i];
        }
    }
}

-(NSColor *)colorAtX:(NSInteger)x y:(NSInteger)y {
   NSUInteger pixel[5];

   [self getPixel:pixel atX:x y:y];

   if(_samplesPerPixel>=3)
       return [NSColor colorWithDeviceRed:pixel[0]/255.0 green:pixel[1]/255.0 blue:pixel[2]/255.0 alpha:_samplesPerPixel>=4 ? pixel[3]/255.0 : 1.0];

   if(_samplesPerPixel==2)
       return [NSColor colorWithDeviceWhite:pixel[0]/255.0 alpha:pixel[1]/255.0];

   if(_samplesPerPixel==1)
       return [NSColor colorWithDeviceWhite:pixel[0]/255.0 alpha:1.0];

   return nil;
}

-(void)setColor:(NSColor *)color atX:(NSInteger)x y:(NSInteger)y
{
    // Convert the color to a compatible format
   color=[color colorUsingColorSpaceName:[self colorSpaceName]];

   NSInteger i,numberOfComponents=[color numberOfComponents];
   CGFloat   components[numberOfComponents];
   unsigned  pixels[numberOfComponents];

    // Extract the RGBA components
   [color getComponents:components];

    if(!_hasAlpha) {
        // No alpha - then drop the component count
        numberOfComponents--;
    } else {
        // Deal with the alpha component
        if((_bitmapFormat & NSAlphaNonpremultipliedBitmapFormat) == NO) { // premultiplied
            CGFloat alpha=components[numberOfComponents-1];

            // Multiply through the alpha
            for(i=0;i<numberOfComponents-1;i++) {
                components[i]*=alpha;
            }
        }

        if(_bitmapFormat&NSAlphaFirstBitmapFormat) {
            // Swap the location of the alpha component
            CGFloat alpha=components[numberOfComponents-1];

            for(i=numberOfComponents;--i>=1;) {
                components[i]=components[i-1];
            }
            components[0]=alpha;
        }
    }

    if(_bitmapFormat&NSFloatingPointSamplesBitmapFormat){
        for(i=0;i<numberOfComponents;i++) {
            ((float *)pixels)[i]=MAX(0.0f,MIN(1.0f,components[i])); // clamp just in case
        }
    } else {
        int maxValue=(1<<[self bitsPerSample])-1;

        for(i=0;i<numberOfComponents;i++){
#ifdef __LITTLE_ENDIAN__
            pixels[i]=MAX(0,MIN(maxValue,(int)(components[(numberOfComponents - 1) - i]*maxValue))); // clamp just in case
#else
            pixels[i]=MAX(0,MIN(maxValue,(int)(components[i]*maxValue))); // clamp just in case
#endif
        }
   }

   [self setPixel:pixels atX:x y:y];
}

-valueForProperty:(NSString *)property {
   return [_properties objectForKey:property];
}

-(void)setProperty:(NSString *)property withValue:value {
   [_properties setObject:value forKey:property];
}

-(void)colorizeByMappingGray:(float)gray toColor:(NSColor *)color blackMapping:(NSColor *)blackMapping whiteMapping:(NSColor *)whiteMapping {
   NSInteger width=[self pixelsWide];
   NSInteger height=[self pixelsHigh];

   for(NSInteger y=0;y<height;y++) {
      for(NSInteger x=0;x<width;x++) {
         NSColor *original=[self colorAtX:x y:y];
         if(original==nil)
             continue;

         CGFloat red=0, green=0, blue=0, alpha=0;

         [original getRed:&red green:&green blue:&blue alpha:&alpha];

         CGFloat white=(red+green+blue)/3.0;

         NSColor *mapped;

         if(white<=gray)
             mapped=[blackMapping colorWithAlphaComponent:white/gray];
         else
             mapped=[whiteMapping colorWithAlphaComponent:(1.0-white)/(1.0-gray)];

         [self setColor:mapped atX:x y:y];
      }
   }
}

-(void)getCompression:(NSTIFFCompression *)compression factor:(float *)factor {
   if(compression!=NULL)
       *compression=NSTIFFCompressionNone;

   if(factor!=NULL)
       *factor=0.0;
}

-(void)setCompression:(NSTIFFCompression)compression factor:(float)factor {
}

-(BOOL)canBeCompressedUsing:(NSTIFFCompression)compression {
   return compression==NSTIFFCompressionNone ||
          compression==NSTIFFCompressionCCITTFAX3 ||
          compression==NSTIFFCompressionLZW ||
          compression==NSTIFFCompressionJPEG ||
          compression==NSTIFFCompressionPackBits;
}

-(NSData *)representationUsingType:(NSBitmapImageFileType)type properties:(NSDictionary *)properties {
   CFStringRef uti;

   switch(type){

    case NSTIFFFileType:
     uti=(CFStringRef)@"public.tiff";
     break;

    case NSBMPFileType:
     uti=(CFStringRef)@"com.microsoft.bmp";
     break;

    case NSGIFFileType:
     uti=(CFStringRef)@"com.compuserve.gif";
     break;

    case NSJPEGFileType:
     uti=(CFStringRef)@"public.jpeg";
     break;

    case NSPNGFileType:
     uti=(CFStringRef)@"public.png";
     break;

    case NSJPEG2000FileType:
     uti=(CFStringRef)@"public.jpeg-2000";
     break;

    default:
     return nil;
   }

    int dpi = 72;
    if (_size.width > 0) {
        dpi = ceilf(72 * _pixelsWide / _size.width);
    }
   NSMutableData        *result=[NSMutableData data];
    // Convert the NS options to CG options - just NSImageCompressionFactor for now
    NSDictionary *CGProperties = [NSMutableDictionary dictionary];

    [CGProperties setValue: [NSNumber numberWithInt: dpi] forKey: (id)kO2ImageDestinationDPI];

    if ([properties count]) {
        id compressionFactor = [properties valueForKey:NSImageCompressionFactor];
        if (compressionFactor) {
            [CGProperties setValue:compressionFactor forKey:(id)kO2ImageDestinationLossyCompressionQuality];
        }
    }
   O2ImageDestinationRef dest=O2ImageDestinationCreateWithData((CFMutableDataRef)result,uti,1,(CFDictionaryRef)CGProperties);

   O2ImageDestinationAddImage(dest,[self CGImage],(CFDictionaryRef)CGProperties);

   O2ImageDestinationFinalize(dest);
   CFRelease(dest);

   return result;
}

-(NSData *)TIFFRepresentation {
   return [self TIFFRepresentationUsingCompression:NSTIFFCompressionNone factor:0.0];
}

-(NSData *)TIFFRepresentationUsingCompression:(NSTIFFCompression)compression factor:(float)factor {
   return [[self class] TIFFRepresentationOfImageRepsInArray:[NSArray arrayWithObject:self]
                                            usingCompression:compression
                                                      factor:factor];
}

-(CGImageRef)createCGImageIfNeeded {
   if(_cgImage!=NULL)
    return CGImageRetain(_cgImage);

   if(_isPlanar)
    [NSException raise:NSInternalInconsistencyException format:@"createCGImageIfNeeded does not support planar formats"];

  CGDataProviderRef provider=CGDataProviderCreateWithData(NULL,_bitmapPlanes[0],_bytesPerRow*_pixelsHigh,NULL);

  CGImageRef image=CGImageCreate(_pixelsWide,_pixelsHigh,_bitsPerPixel/_samplesPerPixel,_bitsPerPixel,_bytesPerRow,[self CGColorSpace],
     [self CGBitmapInfo],provider,NULL,NO,kCGRenderingIntentDefault);

   CGDataProviderRelease(provider);

  return image;
}

-(CGImageRef)CGImage {
   if(_cgImage!=NULL)
    return _cgImage;

   return (CGImageRef)[(id)[self createCGImageIfNeeded] autorelease];
}

-(BOOL)draw {
   CGContextRef context=NSCurrentGraphicsPort();
   NSSize size=[self size];
   CGImageRef image=[self createCGImageIfNeeded];

   CGContextDrawImage(context,NSMakeRect(0,0,size.width,size.height),image);

   CGImageRelease(image);

   return YES;
}

-(CGColorSpaceRef)CGColorSpace {
   if([_colorSpaceName isEqualToString:NSDeviceRGBColorSpace])
    return (CGColorSpaceRef)[(id)CGColorSpaceCreateDeviceRGB() autorelease];
   if([_colorSpaceName isEqualToString:NSCalibratedRGBColorSpace])
    return (CGColorSpaceRef)[(id)CGColorSpaceCreateDeviceRGB() autorelease];

   return NULL;
}

-(CGBitmapInfo)CGBitmapInfo {
   CGBitmapInfo result=kCGBitmapByteOrderDefault;

   if(![self hasAlpha])
    result|=kCGImageAlphaNone;
   else {
    if(_bitmapFormat&NSAlphaFirstBitmapFormat){
     if(_bitmapFormat&NSAlphaNonpremultipliedBitmapFormat)
      result|=kCGImageAlphaFirst;
     else
      result|=kCGImageAlphaPremultipliedFirst;
    }
    else {
     if(_bitmapFormat&NSAlphaNonpremultipliedBitmapFormat)
      result|=kCGImageAlphaLast;
     else
      result|=kCGImageAlphaPremultipliedLast;
    }
   }
   if(_bitmapFormat&NSFloatingPointSamplesBitmapFormat)
    result|=kCGBitmapFloatComponents;

   return result;
}

@end
