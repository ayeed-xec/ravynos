/* Copyright (c) 2009 Christopher J. W. Lloyd

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */
#import <AppKit/NSCollectionViewItem.h>
#import <AppKit/NSRaise.h>
#import <Foundation/NSKeyedArchiver.h>

@implementation NSCollectionViewItem

-(void)encodeWithCoder:(NSCoder *)coder {
   if([coder allowsKeyedCoding]){
    NSKeyedArchiver *keyed=(NSKeyedArchiver *)coder;

    [keyed encodeObject:_representedObject forKey:@"NSRepresentedObject"];
    [keyed encodeObject:_collectionView forKey:@"NSCollectionView"];
   }
   else {
    [NSException raise:NSInvalidArgumentException format:@"-[%@ %s] is not implemented for coder %@",[self class],sel_getName(_cmd),coder];
   }
}

-initWithCoder:(NSCoder *)coder {
   if([coder allowsKeyedCoding]){
    NSKeyedUnarchiver *keyed=(NSKeyedUnarchiver *)coder;

    _representedObject=[keyed decodeObjectForKey:@"NSRepresentedObject"];
    _collectionView=[keyed decodeObjectForKey:@"NSCollectionView"];
   }
   else {
    [NSException raise:NSInvalidArgumentException format:@"-[%@ %s] is not implemented for coder %@",[self class],sel_getName(_cmd),coder];
   }

   return self;
}

-copyWithZone:(NSZone *)zone {
   NSCollectionViewItem *copy=[[[self class] allocWithZone:zone] init];

   [copy setRepresentedObject:[self representedObject]];
   copy->_collectionView=[[self collectionView] retain];
   return copy;
}

-(NSCollectionView *)collectionView {
   return _collectionView;
}

-(BOOL)isSelected {
   return _isSelected;
}

-(void)setSelected:(BOOL)value {
   _isSelected=value;
}

@end
