/* Copyright (c) 2007 Christopher J. W. Lloyd

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */
#import <AppKit/NSTextAttachmentCell.h>
#import <AppKit/NSRaise.h>
#import <AppKit/NSTextAttachment.h>
#import <AppKit/NSFileWrapper.h>
#import <AppKit/NSImage.h>
#import <AppKit/NSGraphics.h>

@implementation NSTextAttachmentCell

-(NSTextAttachment *)attachment {
   return _attachment;
}

-(void)setAttachment:(NSTextAttachment *)attachment {
   _attachment=attachment;
}

-(NSImage *)attachmentImage {
   NSFileWrapper *wrapper=[[self attachment] fileWrapper];
   NSImage *image=[wrapper icon];

   if(image==nil && wrapper!=nil){
      NSData *contents=[wrapper regularFileContents];

      if(contents!=nil)
         image=[[[NSImage alloc] initWithData:contents] autorelease];
   }

   return image;
}

-(NSSize)cellSize {
   NSImage *image=[self attachmentImage];
   NSSize size=NSMakeSize(0,0);

   if(image!=nil)
      size=[image size];

   return size;
}

-(NSPoint)cellBaselineOffset {
   return NSMakePoint(0,0);
}

-(NSRect)cellFrameForTextContainer:(NSTextContainer *)textContainer proposedLineFragment:(NSRect)proposedRect glyphPosition:(NSPoint)glyphPoint characterIndex:(unsigned)characterIndex {
   NSSize size=[self cellSize];
   NSRect frame=NSMakeRect(glyphPoint.x,glyphPoint.y,size.width,size.height);

   if(frame.size.width>proposedRect.size.width)
      frame.size.width=proposedRect.size.width;

   if(frame.origin.x+frame.size.width>proposedRect.origin.x+proposedRect.size.width)
      frame.origin.x=proposedRect.origin.x+proposedRect.size.width-frame.size.width;

   return frame;
}

-(BOOL)wantsToTrackMouse {
   return YES;
}

-(BOOL)wantsToTrackMouseForEvent:(NSEvent *)event inRect:(NSRect)rect ofView:(NSView *)view atCharacterIndex:(unsigned)characterIndex {
   return [self wantsToTrackMouse];
}

-(BOOL)trackMouse:(NSEvent *)event inRect:(NSRect)rect ofView:(NSView *)view atCharacterIndex:(unsigned)characterIndex untilMouseUp:(BOOL)untilMouseUp {
   return NO;
}

-(BOOL)trackMouse:(NSEvent *)event inRect:(NSRect)rect ofView:(NSView *)view untilMouseUp:(BOOL)untilMouseUp {
   return NO;
}

-(void)highlight:(BOOL)highlight withFrame:(NSRect)frame inView:(NSView *)view {
}

-(void)drawWithFrame:(NSRect)frame inView:(NSView *)view characterIndex:(unsigned)characterIndex layoutManager:(NSLayoutManager *)layoutManager {
   NSImage *image=[self attachmentImage];

   if(image!=nil)
      [image drawInRect:frame fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0];
}

-(void)drawWithFrame:(NSRect)frame inView:(NSView *)view characterIndex:(unsigned)characterIndex {
   [self drawWithFrame:frame inView:view characterIndex:characterIndex layoutManager:nil];
}

-(void)drawWithFrame:(NSRect)frame inView:(NSView *)view {
   [self drawWithFrame:frame inView:view characterIndex:NSNotFound layoutManager:nil];
}

@end
