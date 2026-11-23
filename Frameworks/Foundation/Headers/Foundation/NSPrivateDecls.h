/* Private declarations shared between the OpenDirectory framework and
   the Foundation private API it builds against. */
#ifndef __FOUNDATION_NSPRIVATEDECLS_H
#define __FOUNDATION_NSPRIVATEDECLS_H

#import <Foundation/Foundation.h>

/* The OD* framework classes are toll-free bridged with their Core
   counterparts: the Core side calls _CFRuntimeBridgeClasses(typeID,
   "NSODX") to wire the CF class to the ObjC class.  These Apple-internal
   macros were never vendored upstream; the ObjC side needs no extra glue
   beyond the class itself, so they expand to nothing. */
#ifndef CF_CLASSDECLARATION
#define CF_CLASSDECLARATION(name)
#endif

#ifndef CF_CLASSIMPLEMENTATION
#define CF_CLASSIMPLEMENTATION(name)
#endif

#endif
