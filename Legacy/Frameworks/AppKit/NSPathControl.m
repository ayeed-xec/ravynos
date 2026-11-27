#import <AppKit/NSPathControl.h>

#import "AppKit/NSRaise.h"

@implementation NSPathControl

-(BOOL)isFlipped {
   return YES;
}

- (NSPathStyle)pathStyle;
{
	NSPathCell *cell = [self cell];
	return [cell pathStyle];
}

- (void)setPathStyle:(NSPathStyle)style;
{
	NSPathCell *cell = [self cell];
	[cell setPathStyle: style];
}

- (NSColor *) backgroundColor;
{
	NSPathCell *cell = [self cell];
	return [cell backgroundColor];
}

- (void)setBackgroundColor:(NSColor *)color;
{
	NSPathCell *cell = [self cell];
	[cell setBackgroundColor: color];
}

- (NSPathComponentCell *)clickedPathComponentCell
{
	NSPathCell *cell = [self cell];
	return [cell clickedPathComponentCell];
}

- (NSArray *)pathComponentCells;
{
	NSPathCell *cell = [self cell];
	return [cell pathComponentCells];
}

- (void)setPathComponentCells:(NSArray *)cells;
{
	NSPathCell *cell = [self cell];
	[cell setPathComponentCells: cells];
}


- (SEL)doubleAction;
{
	NSPathCell *cell = [self cell];
	return [cell doubleAction];
}

- (void)setDoubleAction:(SEL)action;
{
	NSPathCell *cell = [self cell];
	[cell setDoubleAction: action];
}

- (NSURL *)URL;
{
	NSPathCell *cell = [self cell];
	return [cell URL];
}
- (void)setURL:(NSURL *)url;
{
	NSPathCell *cell = [self cell];
	[cell setURL: url];
}

- (id < NSPathControlDelegate >)delegate;
{
	NSPathCell *cell = [self cell];
	return [cell delegate];
}

- (void)setDelegate:(id < NSPathControlDelegate >)delegate;
{
	NSPathCell *cell = [self cell];
	[cell setDelegate: delegate];
}

- (void)setDraggingSourceOperationMask:(NSDragOperation)mask forLocal:(BOOL)isLocal;
{
	if (isLocal)
		_dragSourceOperationMaskForLocal = mask;
	else
		_dragSourceOperationMaskForNonLocal = mask;
}

- (NSDragOperation)draggingSourceOperationMaskForLocal:(BOOL)isLocal;
{
	return isLocal ? _dragSourceOperationMaskForLocal
	               : _dragSourceOperationMaskForNonLocal;
}

- initWithFrame:(NSRect)frame;
{
	self = [super initWithFrame: frame];
	if (self) {
		_dragSourceOperationMaskForLocal = NSDragOperationEvery;
		_dragSourceOperationMaskForNonLocal = NSDragOperationNone;
	}

	return self;
}

- (NSMenu *)menu;
{
	NSPathCell *cell = [self cell];
//	return [cell menu];
	return nil;
}

- (void)setMenu:(NSMenu *)menu;
{
	NSPathCell *cell = [self cell];
//	[cell setMenu: menu];
}

+ (void) initialize;
{
	if (self == [NSPathControl class]) {
		[self setCellClass: [NSPathCell class]];
	}
}

@end
