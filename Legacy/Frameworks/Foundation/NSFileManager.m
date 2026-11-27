/* Copyright (c) 2006-2007 Christopher J. W. Lloyd

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#import <Foundation/NSFileManager.h>
#import <Foundation/NSThread.h>
#import <Foundation/NSRaise.h>
#import <Foundation/NSNumber.h>
#import <Foundation/NSData.h>
#import <Foundation/NSArray.h>
#import <Foundation/NSConcreteDirectoryEnumerator.h>
#import <Foundation/NSBundle.h>
#import <Foundation/NSURL.h>
#import <Foundation/NSPathUtilities.h>
#import <Foundation/NSDictionary.h>
#import <Foundation/NSDate.h>
#import <Foundation/NSError.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/time.h>
#include <utime.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <limits.h>

@interface NSFileManager () {
    id _delegate;
}
@end

static void SetPOSIXError(NSError **error, int code, NSString *path) {
   if(error==NULL)
    return;
   *error=[NSError errorWithDomain:NSPOSIXErrorDomain code:code
              userInfo:[NSDictionary dictionaryWithObject:[NSString stringWithFormat:@"%s: %@",strerror(code),path]
                                                   forKey:NSLocalizedDescriptionKey]];
}

NSString * const NSFileType = @"NSFileType";
NSString * const NSFileTypeRegular = @"NSFileTypeRegular";
NSString * const NSFileTypeDirectory = @"NSFileTypeDirectory";
NSString * const NSFileTypeSymbolicLink = @"NSFileTypeSymbolicLink";
NSString * const NSFileTypeCharacterSpecial = @"NSFileTypeCharacterSpecial";
NSString * const NSFileTypeBlockSpecial = @"NSFileTypeBlockSpecial";
NSString * const NSFileTypeFIFO = @"NSFileTypeFIFO";
NSString * const NSFileTypeSocket = @"NSFileTypeSocket";
NSString * const NSFileTypeUnknown = @"NSFileTypeUnknown";

NSString * const NSFileSize = @"NSFileSize";
NSString * const NSFileCreationDate = @"NSFileCreationDate";
NSString * const NSFileModificationDate = @"NSFileModificationDate";
NSString * const NSFileOwnerAccountName = @"NSFileOwnerAccountName";
NSString * const NSFileGroupOwnerAccountName = @"NSFileGroupOwnerAccountName";

NSString * const NSFileReferenceCount = @"NSFileReferenceCount";
NSString * const NSFileIdentifier = @"NSFileIdentifier";
NSString * const NSFileDeviceIdentifier = @"NSFileDeviceIdentifier";
NSString * const NSFilePosixPermissions = @"NSFilePosixPermissions";
NSString * const NSFileHFSCreatorCode = @"NSFileHFSCreatorCode";
NSString * const NSFileHFSTypeCode = @"NSFileHFSTypeCode";

NSString * const NSFileSystemNumber=@"NSFileSystemNumber";
NSString * const NSFileSystemSize=@"NSFileSystemSize";
NSString * const NSFileSystemFreeSize=@"NSFileSystemFreeSize";

@implementation NSFileManager

+(NSFileManager *)defaultManager {
   return NSThreadSharedInstance(@"NSFileManager");
}

-delegate {
   return _delegate;
}
-(void)setDelegate:delegate {
   _delegate=delegate;
}

-(NSDictionary *)attributesOfFileSystemForPath:(NSString *)path error:(NSError **)error {
   struct statvfs buf;
   NSMutableDictionary *result;

   if(statvfs([path fileSystemRepresentation],&buf)!=0) {
      SetPOSIXError(error,errno,path);
      return nil;
   }

   unsigned long long blockSize=buf.f_frsize ? buf.f_frsize : buf.f_bsize;
   result=[NSMutableDictionary dictionary];
   [result setObject:[NSNumber numberWithUnsignedLongLong:(unsigned long long)buf.f_blocks*blockSize]
              forKey:NSFileSystemSize];
   [result setObject:[NSNumber numberWithUnsignedLongLong:(unsigned long long)buf.f_bavail*blockSize]
              forKey:NSFileSystemFreeSize];
   [result setObject:[NSNumber numberWithUnsignedLong:buf.f_fsid]
              forKey:NSFileSystemNumber];

   return result;
}

-(NSDictionary *)attributesOfItemAtPath:(NSString *)path error:(NSError **)error {
   struct stat buf;

   if(stat([path fileSystemRepresentation],&buf)!=0) {
      SetPOSIXError(error,errno,path);
      return nil;
   }

   NSMutableDictionary *result=[NSMutableDictionary dictionary];
   struct passwd *pwd;
   struct group *grp;

   [result setObject:[NSNumber numberWithUnsignedLongLong:buf.st_size]
              forKey:NSFileSize];
   [result setObject:[NSDate dateWithTimeIntervalSince1970:buf.st_mtime]
              forKey:NSFileModificationDate];
   [result setObject:[NSDate dateWithTimeIntervalSince1970:buf.st_ctime]
              forKey:NSFileCreationDate];

   pwd=getpwuid(buf.st_uid);
   if(pwd!=NULL)
      [result setObject:[NSString stringWithCString:pwd->pw_name]
                 forKey:NSFileOwnerAccountName];

   grp=getgrgid(buf.st_gid);
   if(grp!=NULL)
      [result setObject:[NSString stringWithCString:grp->gr_name]
                 forKey:NSFileGroupOwnerAccountName];

   [result setObject:[NSNumber numberWithUnsignedLong:buf.st_nlink]
              forKey:NSFileReferenceCount];
   [result setObject:[NSNumber numberWithUnsignedLongLong:buf.st_ino]
              forKey:NSFileIdentifier];
   [result setObject:[NSNumber numberWithUnsignedLong:buf.st_dev]
              forKey:NSFileDeviceIdentifier];
   [result setObject:[NSNumber numberWithUnsignedLong:buf.st_mode&07777]
              forKey:NSFilePosixPermissions];

   if(S_ISREG(buf.st_mode))
      [result setObject:NSFileTypeRegular forKey:NSFileType];
   else if(S_ISDIR(buf.st_mode))
      [result setObject:NSFileTypeDirectory forKey:NSFileType];
   else if(S_ISCHR(buf.st_mode))
      [result setObject:NSFileTypeCharacterSpecial forKey:NSFileType];
   else if(S_ISBLK(buf.st_mode))
      [result setObject:NSFileTypeBlockSpecial forKey:NSFileType];
   else if(S_ISFIFO(buf.st_mode))
      [result setObject:NSFileTypeFIFO forKey:NSFileType];
   else if(S_ISLNK(buf.st_mode))
      [result setObject:NSFileTypeSymbolicLink forKey:NSFileType];
   else if(S_ISSOCK(buf.st_mode))
      [result setObject:NSFileTypeSocket forKey:NSFileType];
   else
      [result setObject:NSFileTypeUnknown forKey:NSFileType];

   return result;
}

-(BOOL)changeCurrentDirectoryPath:(NSString *)path {
   return chdir([path fileSystemRepresentation])==0;
}

-(NSArray *)componentsToDisplayForPath:(NSString *)path {
   NSArray *components=[path pathComponents];
   NSUInteger i,count=[components count],start=0;
   NSMutableArray *result;

   if(path==nil || count==0)
    return nil;

   if([[components objectAtIndex:0] isEqualToString:@"/"])
    start=1;

   if(start==count)
    return nil;

   result=[NSMutableArray array];
   for(i=start;i<count;i++)
    [result addObject:[components objectAtIndex:i]];

   return result;
}

-(BOOL)contentsEqualAtPath:(NSString *)path1 andPath:(NSString *)path2 {
   struct stat buf1,buf2;

   if(path1==nil || path2==nil)
    return NO;

   if(stat([path1 fileSystemRepresentation],&buf1)!=0)
    return NO;
   if(stat([path2 fileSystemRepresentation],&buf2)!=0)
    return NO;

   if((buf1.st_mode&S_IFMT)!=(buf2.st_mode&S_IFMT))
    return NO;

   if(S_ISREG(buf1.st_mode)) {
      int fd1,fd2;
      char b1[8192],b2[8192];
      ssize_t n1,n2;
      BOOL equal=YES;

      if(buf1.st_size!=buf2.st_size)
       return NO;

      fd1=open([path1 fileSystemRepresentation],O_RDONLY);
      fd2=open([path2 fileSystemRepresentation],O_RDONLY);

      if(fd1<0 || fd2<0) {
         if(fd1>=0) close(fd1);
         if(fd2>=0) close(fd2);
         return NO;
      }

      while((n1=read(fd1,b1,sizeof(b1)))>0) {
         n2=read(fd2,b2,sizeof(b2));
         if(n1!=n2 || memcmp(b1,b2,n1)!=0) {
            equal=NO;
            break;
         }
      }

      close(fd1);
      close(fd2);

      return equal;
   }

   if(S_ISDIR(buf1.st_mode)) {
      NSArray *contents1=[self contentsOfDirectoryAtPath:path1 error:NULL];
      NSArray *contents2=[self contentsOfDirectoryAtPath:path2 error:NULL];
      NSUInteger i,count;

      if(contents1==nil || contents2==nil)
       return NO;

      count=[contents1 count];
      if(count!=[contents2 count])
       return NO;

      for(i=0;i<count;i++) {
         NSString *name=[contents1 objectAtIndex:i];

         if(![contents2 containsObject:name])
          return NO;

         if(![self contentsEqualAtPath:[path1 stringByAppendingPathComponent:name]
                              andPath:[path2 stringByAppendingPathComponent:name]])
          return NO;
      }
   }

   return YES;
}

-(NSArray *)contentsOfDirectoryAtPath:(NSString *)path error:(NSError **)error {
   NSMutableArray *result=nil;
   DIR *dirp=NULL;
   struct dirent *dire;

   if(path==nil)
    return nil;

   dirp=opendir([path fileSystemRepresentation]);

   if(dirp==NULL) {
      SetPOSIXError(error,errno,path);
      return nil;
   }

   result=[NSMutableArray array];

   while((dire=readdir(dirp))) {
      if(strcmp(".",dire->d_name)==0)
       continue;
      if(strcmp("..",dire->d_name)==0)
       continue;
      [result addObject:[NSString stringWithCString:dire->d_name]];
   }

   closedir(dirp);

   return result;
}

-(BOOL)copyItemAtPath:(NSString *)fromPath toPath:(NSString *)toPath error:(NSError **)error {
   struct stat buf;

   if(lstat([fromPath fileSystemRepresentation],&buf)!=0) {
      SetPOSIXError(error,errno,fromPath);
      return NO;
   }

   if(S_ISLNK(buf.st_mode)) {
      char linkbuf[PATH_MAX+1];
      ssize_t length;

      length=readlink([fromPath fileSystemRepresentation],linkbuf,PATH_MAX);

      if(length<0) {
         SetPOSIXError(error,errno,fromPath);
         return NO;
      }

      linkbuf[length]=0;

      if(symlink(linkbuf,[toPath fileSystemRepresentation])!=0) {
         SetPOSIXError(error,errno,toPath);
         return NO;
      }

      return YES;
   }

   if(S_ISDIR(buf.st_mode)) {
      DIR *dirp;
      struct dirent *dire;

      if(mkdir([toPath fileSystemRepresentation],buf.st_mode&07777)!=0) {
         SetPOSIXError(error,errno,toPath);
         return NO;
      }

      dirp=opendir([fromPath fileSystemRepresentation]);

      if(dirp==NULL) {
         SetPOSIXError(error,errno,fromPath);
         return NO;
      }

      while((dire=readdir(dirp))) {
         NSString *name;

         if(strcmp(".",dire->d_name)==0)
          continue;
         if(strcmp("..",dire->d_name)==0)
          continue;

         name=[NSString stringWithCString:dire->d_name];

         if(![self copyItemAtPath:[fromPath stringByAppendingPathComponent:name]
                           toPath:[toPath stringByAppendingPathComponent:name]
                            error:error]) {
            closedir(dirp);
            return NO;
         }
      }

      closedir(dirp);

      return YES;
   }

   int r=open([fromPath fileSystemRepresentation],O_RDONLY);

   if(r<0) {
      SetPOSIXError(error,errno,fromPath);
      return NO;
   }

   int w=open([toPath fileSystemRepresentation],O_WRONLY|O_CREAT|O_TRUNC,buf.st_mode&07777);

   if(w<0) {
      SetPOSIXError(error,errno,toPath);
      close(r);
      return NO;
   }

   char buffer[65536];
   ssize_t n;
   BOOL ok=YES;

   while((n=read(r,buffer,sizeof(buffer)))>0) {
      if(write(w,buffer,n)!=n) {
         ok=NO;
         break;
      }
   }

   if(n<0)
    ok=NO;

   close(r);
   close(w);

   if(!ok) {
      SetPOSIXError(error,errno,fromPath);
      return NO;
   }

   return YES;
}
-(NSString *)destinationOfSymbolicLinkAtPath:(NSString *)path error:(NSError **)error {
   //NSInvalidAbstractInvocation();
   return 0;
}

-(NSString *)displayNameAtPath:(NSString *)path {
   NSBundle *bundle=[NSBundle bundleWithPath:path];
   NSString *name=nil;
   if(bundle) {
    NSDictionary *localizedInfo=[bundle localizedInfoDictionary];
    name=[localizedInfo objectForKey:@"CFBundleDisplayName"];
    if(!name)
     name=[localizedInfo objectForKey:@"CFBundleName"];
   }
   if(!name)
    name=[path lastPathComponent];
   return name;
}

-(NSDictionary *)fileSystemAttributesAtPath:(NSString *)path
{
	return [self attributesOfFileSystemForPath: path error: NULL];
}

-(BOOL)isDeletableFileAtPath:(NSString *)path {
   NSString *parent;
   struct stat buf;

   if(path==nil)
    return NO;

   if(lstat([path fileSystemRepresentation],&buf)!=0)
    return NO;

   parent=[path stringByDeletingLastPathComponent];

   if([parent length]==0)
    parent=@".";

   if(access([parent fileSystemRepresentation],W_OK|X_OK)!=0)
    return NO;

   if(S_ISDIR(buf.st_mode))
    return access([path fileSystemRepresentation],W_OK|X_OK)==0;

   return YES;
}

-(BOOL)linkItemAtPath:(NSString *)fromPath toPath:(NSString *)toPath error:(NSError **)error {
   struct stat buf;

   if(lstat([fromPath fileSystemRepresentation],&buf)!=0) {
      SetPOSIXError(error,errno,fromPath);
      return NO;
   }

   if(S_ISDIR(buf.st_mode)) {
      DIR *dirp;
      struct dirent *dire;

      if(mkdir([toPath fileSystemRepresentation],buf.st_mode&07777)!=0) {
         SetPOSIXError(error,errno,toPath);
         return NO;
      }

      dirp=opendir([fromPath fileSystemRepresentation]);

      if(dirp==NULL) {
         SetPOSIXError(error,errno,fromPath);
         return NO;
      }

      while((dire=readdir(dirp))) {
         NSString *name;

         if(strcmp(".",dire->d_name)==0)
          continue;
         if(strcmp("..",dire->d_name)==0)
          continue;

         name=[NSString stringWithCString:dire->d_name];

         if(![self linkItemAtPath:[fromPath stringByAppendingPathComponent:name]
                           toPath:[toPath stringByAppendingPathComponent:name]
                            error:error]) {
            closedir(dirp);
            return NO;
         }
      }

      closedir(dirp);

      return YES;
   }

   if(link([fromPath fileSystemRepresentation],[toPath fileSystemRepresentation])!=0) {
      SetPOSIXError(error,errno,toPath);
      return NO;
   }

   return YES;
}
-(BOOL)linkPath:(NSString *)source toPath:(NSString *)destination handler:handler {
   if(link([source fileSystemRepresentation],[destination fileSystemRepresentation])==0)
    return YES;

   if(handler!=nil && [handler respondsToSelector:@selector(fileManager:shouldProceedAfterError:)]) {
      NSDictionary *errorInfo=[NSDictionary dictionaryWithObjectsAndKeys:
         source,@"Path",
         destination,@"ToPath",
         [NSString stringWithFormat:@"link: %s",strerror(errno)],@"Error",
         nil];

      if([handler fileManager:self shouldProceedAfterError:errorInfo])
       return [self linkPath:source toPath:destination handler:handler];

      return NO;
   }

   return NO;
}
-(BOOL)moveItemAtPath:(NSString *)fromPath toPath:(NSString *)toPath error:(NSError **)error {
   if(rename([fromPath fileSystemRepresentation],[toPath fileSystemRepresentation])==0)
    return YES;

   if(errno==EXDEV) {
      if(![self copyItemAtPath:fromPath toPath:toPath error:error])
       return NO;

      return [self removeItemAtPath:fromPath error:error];
   }

   SetPOSIXError(error,errno,fromPath);

   return NO;
}
-(BOOL)removeItemAtPath:(NSString *)path error:(NSError **)error {
   struct stat buf;

   if(lstat([path fileSystemRepresentation],&buf)!=0) {
      SetPOSIXError(error,errno,path);
      return NO;
   }

   if(S_ISDIR(buf.st_mode)) {
      DIR *dirp;
      struct dirent *dire;

      dirp=opendir([path fileSystemRepresentation]);

      if(dirp==NULL) {
         SetPOSIXError(error,errno,path);
         return NO;
      }

      while((dire=readdir(dirp))) {
         NSString *name;

         if(strcmp(".",dire->d_name)==0)
          continue;
         if(strcmp("..",dire->d_name)==0)
          continue;

         name=[NSString stringWithCString:dire->d_name];

         if(![self removeItemAtPath:[path stringByAppendingPathComponent:name] error:error]) {
            closedir(dirp);
            return NO;
         }
      }

      closedir(dirp);

      if(rmdir([path fileSystemRepresentation])!=0) {
         SetPOSIXError(error,errno,path);
         return NO;
      }

      return YES;
   }

   if(unlink([path fileSystemRepresentation])!=0) {
      SetPOSIXError(error,errno,path);
      return NO;
   }

   return YES;
}

-(BOOL)removeItemAtURL:(NSURL *)url error:(NSError **)error {
    // Default to the AtPath implementation
    return [self removeItemAtPath:[url path] error:error];
}

-(BOOL)moveItemAtURL:(NSURL *)url toURL:(NSURL *)dstURL error:(NSError **)error {
    // Default to the AtPath implementation
    return [self moveItemAtPath:[url path] toPath:[dstURL path] error:error];
}

-(BOOL)linkItemAtURL:(NSURL *)url toURL:(NSURL *)dstURL error:(NSError **)error {
    // Default to the AtPath implementation
    return [self linkItemAtPath:[url path] toPath:[dstURL path] error:error];
}

-(BOOL)copyItemAtURL:(NSURL *)url toURL:(NSURL *)dstURL error:(NSError **)error {
    // Default to the AtPath implementation
    return [self copyItemAtPath:[url path] toPath:[dstURL path] error:error];
}

-(BOOL)setAttributes:(NSDictionary *)attributes ofItemAtPath:(NSString *)path error:(NSError **)error {
   const char *rep=[path fileSystemRepresentation];
   NSNumber *permissions=[attributes objectForKey:NSFilePosixPermissions];
   NSDate *date=[attributes objectForKey:NSFileModificationDate];
   NSString *owner=[attributes objectForKey:NSFileOwnerAccountName];
   NSString *group=[attributes objectForKey:NSFileGroupOwnerAccountName];

   if(permissions!=nil) {
      if(chmod(rep,[permissions unsignedShortValue])!=0) {
         SetPOSIXError(error,errno,path);
         return NO;
      }
   }

   if(date!=nil) {
      struct utimbuf tb;
      NSTimeInterval interval=[date timeIntervalSince1970];

      tb.actime=(time_t)interval;
      tb.modtime=(time_t)interval;

      if(utime(rep,&tb)!=0) {
         SetPOSIXError(error,errno,path);
         return NO;
      }
   }

   if(owner!=nil || group!=nil) {
      struct stat buf;
      struct passwd *pwd;
      struct group *grp;
      uid_t uid;
      gid_t gid;

      if(lstat(rep,&buf)!=0) {
         SetPOSIXError(error,errno,path);
         return NO;
      }

      uid=buf.st_uid;
      gid=buf.st_gid;

      if(owner!=nil) {
         pwd=getpwnam([owner fileSystemRepresentation]);

         if(pwd==NULL) {
            SetPOSIXError(error,ENOENT,owner);
            return NO;
         }

         uid=pwd->pw_uid;
      }

      if(group!=nil) {
         grp=getgrnam([group fileSystemRepresentation]);

         if(grp==NULL) {
            SetPOSIXError(error,ENOENT,group);
            return NO;
         }

         gid=grp->gr_gid;
      }

      if(chown(rep,uid,gid)!=0) {
         SetPOSIXError(error,errno,path);
         return NO;
      }
   }

   return YES;
}

-(NSString *)stringWithFileSystemRepresentation:(const char *)string length:(NSUInteger)length {
   return [[[NSString alloc] initWithBytes:string length:length encoding:NSUTF8StringEncoding] autorelease];
}

-(void)_subpathsAtPath:(NSString *)path basePath:(NSString*)basePath
paths:(NSMutableArray*)paths
{
	NSArray* files = [self directoryContentsAtPath:path];

	int x; for (x = 0; x < [files count]; x++)
	{
		[paths addObject:[basePath stringByAppendingPathComponent:[files objectAtIndex:x]]];
	}

	for (x = 0; x < [files count]; x++)
	{
		BOOL isDir = NO;
		NSString* newPath = [path stringByAppendingPathComponent:[files objectAtIndex:x]];
		[self fileExistsAtPath:newPath isDirectory:&isDir];
		if (isDir)
			[self _subpathsAtPath:newPath basePath:[basePath
stringByAppendingPathComponent:[files objectAtIndex:x]] paths:paths];
	}
}

-(NSArray *)subpathsAtPath:(NSString *)path {
	NSMutableArray *result=[NSMutableArray array];

	[self _subpathsAtPath:path basePath:@"" paths:result];
	return result;
}

-(NSArray *)subpathsOfDirectoryAtPath:(NSString *)path error:(NSError **)error {
   NSMutableArray *result;

   if(path==nil)
    return nil;

   if(![self fileExistsAtPath:path isDirectory:NULL]) {
      SetPOSIXError(error,ENOENT,path);
      return nil;
   }

   result=[NSMutableArray array];
   [self _subpathsAtPath:path basePath:@"" paths:result];

   return result;
}

-(NSData *)contentsAtPath:(NSString *)path {
   return [NSData dataWithContentsOfFile:path];
}

-(BOOL)createFileAtPath:(NSString *)path contents:(NSData *)data
             attributes:(NSDictionary *)attributes {
   //NSInvalidAbstractInvocation();
   return NO;
}

-(NSArray *)directoryContentsAtPath:(NSString *)path {
   //NSInvalidAbstractInvocation();
   return nil;
}

-(NSDirectoryEnumerator *)enumeratorAtPath:(NSString *)path {
    return [[[NSConcreteDirectoryEnumerator alloc] initWithPath: path] autorelease];
}

-(BOOL)createDirectoryAtPath:(NSString *)path attributes:(NSDictionary *)attributes {
   //NSInvalidAbstractInvocation();
   return NO;
}

-(BOOL)createDirectoryAtPath:(NSString *)path withIntermediateDirectories:(BOOL)intermediates attributes:(NSDictionary *)attributes error:(NSError **)error {
	BOOL result = YES;
	BOOL isDirectory = NO;
	if (intermediates) {
		NSArray *components = [path pathComponents];
		int i, count = [components count];
		for (i=1; i<=count && result; i++) {
			NSString *makePath = [NSString pathWithComponents:[components subarrayWithRange:NSMakeRange(0,i)]];
			if (![self fileExistsAtPath:makePath isDirectory:&isDirectory]) {
				result = [self createDirectoryAtPath:makePath attributes:attributes];
			}
		}
	}
	else {
		result = [self createDirectoryAtPath:path attributes:attributes];
	}
	// TODO: fill error
	return result;
}

-(BOOL)createDirectoryAtURL:(NSURL *)url withIntermediateDirectories:(BOOL)intermediates attributes:(NSDictionary *)attributes error:(NSError **)error
{
    return [self createDirectoryAtPath:[url path] withIntermediateDirectories:intermediates attributes:attributes error:error];
}

-(BOOL)createSymbolicLinkAtPath:(NSString *)path pathContent:(NSString *)destination {
   //NSInvalidAbstractInvocation();
   return NO;
}

-(BOOL)createSymbolicLinkAtPath:(NSString *)path withDestinationPath:(NSString *)destPath error:(NSError **)error {
   //NSUnimplementedMethod();
   return 0;
}

-(NSString *)pathContentOfSymbolicLinkAtPath:(NSString *)path {
   //NSInvalidAbstractInvocation();
   return NO;
}

-(BOOL)fileExistsAtPath:(NSString *)path {
   /* *** HACK ALERT ***
    * For some reason (likely the way this object is created), it has a nil
    * isa which causes all sorts of fun problems. This function is called
    * early in an app's initialization of NSBundle so we patch it here
    * until the root cause can be fixed.
    */
   *(uintptr_t *)self = (uintptr_t)objc_lookUpClass("NSFileManager_posix");
   BOOL foo;
   return [self fileExistsAtPath:path isDirectory:&foo];
}

-(BOOL)fileExistsAtPath:(NSString *)path isDirectory:(BOOL *)isDirectory {
   //NSInvalidAbstractInvocation();
   return NO;
}

-(BOOL)removeFileAtPath:(NSString *)path handler:handler {
   //NSInvalidAbstractInvocation();
   return NO;
}

-(BOOL)movePath:(NSString *)src toPath:(NSString *)dest handler:handler {
   //NSInvalidAbstractInvocation();
   return NO;
}

-(BOOL)copyPath:(NSString *)src toPath:(NSString *)dest handler:handler {
   //NSInvalidAbstractInvocation();
   return NO;
}

-(NSString *)currentDirectoryPath {
   //NSInvalidAbstractInvocation();
   return nil;
}

-(NSDictionary *)fileAttributesAtPath:(NSString *)path traverseLink:(BOOL)traverse {
   //NSInvalidAbstractInvocation();
   return nil;
}

-(BOOL)isReadableFileAtPath:(NSString *)path {
   //NSInvalidAbstractInvocation();
   return NO;
}

-(BOOL)isWritableFileAtPath:(NSString *)path {
   //NSInvalidAbstractInvocation();
   return NO;
}

-(BOOL)isExecutableFileAtPath:(NSString *)path {
   //NSInvalidAbstractInvocation();
   return NO;
}

-(BOOL)changeFileAttributes:(NSDictionary *)attributes atPath:(NSString *)path {
    return [self setAttributes:(NSDictionary *)attributes ofItemAtPath:(NSString *)path error:NULL];
}

-(const char *)fileSystemRepresentationWithPath:(NSString *)path {
   //NSInvalidAbstractInvocation();
   return NULL;
}

-(const uint16_t *)fileSystemRepresentationWithPathW:(NSString *)path {
   //NSInvalidAbstractInvocation();
   return NULL;
}


@end

@implementation NSDictionary(NSFileAttributes)

-(NSDate *)fileModificationDate {
   return [self objectForKey:NSFileModificationDate];
}

-(NSUInteger)filePosixPermissions {
   return [[self objectForKey:NSFilePosixPermissions] unsignedIntegerValue];
}

-(NSString *)fileOwnerAccountName {
   return [self objectForKey:NSFileOwnerAccountName];
}

-(NSString *)fileGroupOwnerAccountName {
   return [self objectForKey:NSFileGroupOwnerAccountName];
}

-(NSString *)fileType {
   return [self objectForKey:NSFileType];
}

-(uint64_t)fileSize {
   return [[self objectForKey:NSFileSize] unsignedLongLongValue];
}

@end
