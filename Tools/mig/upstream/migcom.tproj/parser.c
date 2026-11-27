/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 156 "parser.y"


#include <stdio.h>
#include "lexxer.h"
#include "strdefs.h"
#include "type.h"
#include "routine.h"
#include "statement.h"
#include "global.h"
#include "error.h"

static char *import_name(statement_kind_t sk);
extern int yylex(void);

/* forward declaration */
void yyerror(char *s);


#line 90 "parser.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif


/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    sySkip = 258,                  /* sySkip  */
    syRoutine = 259,               /* syRoutine  */
    sySimpleRoutine = 260,         /* sySimpleRoutine  */
    sySubsystem = 261,             /* sySubsystem  */
    syKernelUser = 262,            /* syKernelUser  */
    syKernelServer = 263,          /* syKernelServer  */
    syMsgOption = 264,             /* syMsgOption  */
    syUseSpecialReplyPort = 265,   /* syUseSpecialReplyPort  */
    syConsumeOnSendError = 266,    /* syConsumeOnSendError  */
    syMsgSeqno = 267,              /* syMsgSeqno  */
    syWaitTime = 268,              /* syWaitTime  */
    sySendTime = 269,              /* sySendTime  */
    syNoWaitTime = 270,            /* syNoWaitTime  */
    syNoSendTime = 271,            /* syNoSendTime  */
    syErrorProc = 272,             /* syErrorProc  */
    syServerPrefix = 273,          /* syServerPrefix  */
    syUserPrefix = 274,            /* syUserPrefix  */
    syServerDemux = 275,           /* syServerDemux  */
    syRCSId = 276,                 /* syRCSId  */
    syImport = 277,                /* syImport  */
    syUImport = 278,               /* syUImport  */
    sySImport = 279,               /* sySImport  */
    syIImport = 280,               /* syIImport  */
    syDImport = 281,               /* syDImport  */
    syIn = 282,                    /* syIn  */
    syOut = 283,                   /* syOut  */
    syInOut = 284,                 /* syInOut  */
    syUserImpl = 285,              /* syUserImpl  */
    syServerImpl = 286,            /* syServerImpl  */
    syRequestPort = 287,           /* syRequestPort  */
    syReplyPort = 288,             /* syReplyPort  */
    sySReplyPort = 289,            /* sySReplyPort  */
    syUReplyPort = 290,            /* syUReplyPort  */
    syType = 291,                  /* syType  */
    syArray = 292,                 /* syArray  */
    syStruct = 293,                /* syStruct  */
    syOf = 294,                    /* syOf  */
    syInTran = 295,                /* syInTran  */
    syOutTran = 296,               /* syOutTran  */
    syDestructor = 297,            /* syDestructor  */
    syCType = 298,                 /* syCType  */
    syCUserType = 299,             /* syCUserType  */
    syUserTypeLimit = 300,         /* syUserTypeLimit  */
    syOnStackLimit = 301,          /* syOnStackLimit  */
    syCServerType = 302,           /* syCServerType  */
    syPointerTo = 303,             /* syPointerTo  */
    syPointerToIfNot = 304,        /* syPointerToIfNot  */
    syValueOf = 305,               /* syValueOf  */
    syCString = 306,               /* syCString  */
    sySecToken = 307,              /* sySecToken  */
    syUserSecToken = 308,          /* syUserSecToken  */
    syServerSecToken = 309,        /* syServerSecToken  */
    syAuditToken = 310,            /* syAuditToken  */
    syUserAuditToken = 311,        /* syUserAuditToken  */
    syServerAuditToken = 312,      /* syServerAuditToken  */
    syServerContextToken = 313,    /* syServerContextToken  */
    syColon = 314,                 /* syColon  */
    sySemi = 315,                  /* sySemi  */
    syComma = 316,                 /* syComma  */
    syPlus = 317,                  /* syPlus  */
    syMinus = 318,                 /* syMinus  */
    syStar = 319,                  /* syStar  */
    syDiv = 320,                   /* syDiv  */
    syLParen = 321,                /* syLParen  */
    syRParen = 322,                /* syRParen  */
    syEqual = 323,                 /* syEqual  */
    syCaret = 324,                 /* syCaret  */
    syTilde = 325,                 /* syTilde  */
    syLAngle = 326,                /* syLAngle  */
    syRAngle = 327,                /* syRAngle  */
    syLBrack = 328,                /* syLBrack  */
    syRBrack = 329,                /* syRBrack  */
    syBar = 330,                   /* syBar  */
    syError = 331,                 /* syError  */
    syNumber = 332,                /* syNumber  */
    sySymbolicType = 333,          /* sySymbolicType  */
    syIdentifier = 334,            /* syIdentifier  */
    syString = 335,                /* syString  */
    syQString = 336,               /* syQString  */
    syFileName = 337,              /* syFileName  */
    syIPCFlag = 338                /* syIPCFlag  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 176 "parser.y"

    u_int number;
    identifier_t identifier;
    string_t string;
    statement_kind_t statement_kind;
    ipc_type_t *type;
    struct
    {
      u_int innumber;		/* msgt_name value, when sending */
      string_t instr;
      u_int outnumber;	/* msgt_name value, when receiving */
      string_t outstr;
      u_int size;		/* 0 means there is no default size */
    } symtype;
    routine_t *routine;
    arg_kind_t direction;
    argument_t *argument;
    ipc_flags_t flag;

#line 240 "parser.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);



/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_sySkip = 3,                     /* sySkip  */
  YYSYMBOL_syRoutine = 4,                  /* syRoutine  */
  YYSYMBOL_sySimpleRoutine = 5,            /* sySimpleRoutine  */
  YYSYMBOL_sySubsystem = 6,                /* sySubsystem  */
  YYSYMBOL_syKernelUser = 7,               /* syKernelUser  */
  YYSYMBOL_syKernelServer = 8,             /* syKernelServer  */
  YYSYMBOL_syMsgOption = 9,                /* syMsgOption  */
  YYSYMBOL_syUseSpecialReplyPort = 10,     /* syUseSpecialReplyPort  */
  YYSYMBOL_syConsumeOnSendError = 11,      /* syConsumeOnSendError  */
  YYSYMBOL_syMsgSeqno = 12,                /* syMsgSeqno  */
  YYSYMBOL_syWaitTime = 13,                /* syWaitTime  */
  YYSYMBOL_sySendTime = 14,                /* sySendTime  */
  YYSYMBOL_syNoWaitTime = 15,              /* syNoWaitTime  */
  YYSYMBOL_syNoSendTime = 16,              /* syNoSendTime  */
  YYSYMBOL_syErrorProc = 17,               /* syErrorProc  */
  YYSYMBOL_syServerPrefix = 18,            /* syServerPrefix  */
  YYSYMBOL_syUserPrefix = 19,              /* syUserPrefix  */
  YYSYMBOL_syServerDemux = 20,             /* syServerDemux  */
  YYSYMBOL_syRCSId = 21,                   /* syRCSId  */
  YYSYMBOL_syImport = 22,                  /* syImport  */
  YYSYMBOL_syUImport = 23,                 /* syUImport  */
  YYSYMBOL_sySImport = 24,                 /* sySImport  */
  YYSYMBOL_syIImport = 25,                 /* syIImport  */
  YYSYMBOL_syDImport = 26,                 /* syDImport  */
  YYSYMBOL_syIn = 27,                      /* syIn  */
  YYSYMBOL_syOut = 28,                     /* syOut  */
  YYSYMBOL_syInOut = 29,                   /* syInOut  */
  YYSYMBOL_syUserImpl = 30,                /* syUserImpl  */
  YYSYMBOL_syServerImpl = 31,              /* syServerImpl  */
  YYSYMBOL_syRequestPort = 32,             /* syRequestPort  */
  YYSYMBOL_syReplyPort = 33,               /* syReplyPort  */
  YYSYMBOL_sySReplyPort = 34,              /* sySReplyPort  */
  YYSYMBOL_syUReplyPort = 35,              /* syUReplyPort  */
  YYSYMBOL_syType = 36,                    /* syType  */
  YYSYMBOL_syArray = 37,                   /* syArray  */
  YYSYMBOL_syStruct = 38,                  /* syStruct  */
  YYSYMBOL_syOf = 39,                      /* syOf  */
  YYSYMBOL_syInTran = 40,                  /* syInTran  */
  YYSYMBOL_syOutTran = 41,                 /* syOutTran  */
  YYSYMBOL_syDestructor = 42,              /* syDestructor  */
  YYSYMBOL_syCType = 43,                   /* syCType  */
  YYSYMBOL_syCUserType = 44,               /* syCUserType  */
  YYSYMBOL_syUserTypeLimit = 45,           /* syUserTypeLimit  */
  YYSYMBOL_syOnStackLimit = 46,            /* syOnStackLimit  */
  YYSYMBOL_syCServerType = 47,             /* syCServerType  */
  YYSYMBOL_syPointerTo = 48,               /* syPointerTo  */
  YYSYMBOL_syPointerToIfNot = 49,          /* syPointerToIfNot  */
  YYSYMBOL_syValueOf = 50,                 /* syValueOf  */
  YYSYMBOL_syCString = 51,                 /* syCString  */
  YYSYMBOL_sySecToken = 52,                /* sySecToken  */
  YYSYMBOL_syUserSecToken = 53,            /* syUserSecToken  */
  YYSYMBOL_syServerSecToken = 54,          /* syServerSecToken  */
  YYSYMBOL_syAuditToken = 55,              /* syAuditToken  */
  YYSYMBOL_syUserAuditToken = 56,          /* syUserAuditToken  */
  YYSYMBOL_syServerAuditToken = 57,        /* syServerAuditToken  */
  YYSYMBOL_syServerContextToken = 58,      /* syServerContextToken  */
  YYSYMBOL_syColon = 59,                   /* syColon  */
  YYSYMBOL_sySemi = 60,                    /* sySemi  */
  YYSYMBOL_syComma = 61,                   /* syComma  */
  YYSYMBOL_syPlus = 62,                    /* syPlus  */
  YYSYMBOL_syMinus = 63,                   /* syMinus  */
  YYSYMBOL_syStar = 64,                    /* syStar  */
  YYSYMBOL_syDiv = 65,                     /* syDiv  */
  YYSYMBOL_syLParen = 66,                  /* syLParen  */
  YYSYMBOL_syRParen = 67,                  /* syRParen  */
  YYSYMBOL_syEqual = 68,                   /* syEqual  */
  YYSYMBOL_syCaret = 69,                   /* syCaret  */
  YYSYMBOL_syTilde = 70,                   /* syTilde  */
  YYSYMBOL_syLAngle = 71,                  /* syLAngle  */
  YYSYMBOL_syRAngle = 72,                  /* syRAngle  */
  YYSYMBOL_syLBrack = 73,                  /* syLBrack  */
  YYSYMBOL_syRBrack = 74,                  /* syRBrack  */
  YYSYMBOL_syBar = 75,                     /* syBar  */
  YYSYMBOL_syError = 76,                   /* syError  */
  YYSYMBOL_syNumber = 77,                  /* syNumber  */
  YYSYMBOL_sySymbolicType = 78,            /* sySymbolicType  */
  YYSYMBOL_syIdentifier = 79,              /* syIdentifier  */
  YYSYMBOL_syString = 80,                  /* syString  */
  YYSYMBOL_syQString = 81,                 /* syQString  */
  YYSYMBOL_syFileName = 82,                /* syFileName  */
  YYSYMBOL_syIPCFlag = 83,                 /* syIPCFlag  */
  YYSYMBOL_YYACCEPT = 84,                  /* $accept  */
  YYSYMBOL_Statements = 85,                /* Statements  */
  YYSYMBOL_Statement = 86,                 /* Statement  */
  YYSYMBOL_Subsystem = 87,                 /* Subsystem  */
  YYSYMBOL_SubsystemStart = 88,            /* SubsystemStart  */
  YYSYMBOL_SubsystemMods = 89,             /* SubsystemMods  */
  YYSYMBOL_SubsystemMod = 90,              /* SubsystemMod  */
  YYSYMBOL_SubsystemName = 91,             /* SubsystemName  */
  YYSYMBOL_SubsystemBase = 92,             /* SubsystemBase  */
  YYSYMBOL_MsgOption = 93,                 /* MsgOption  */
  YYSYMBOL_UseSpecialReplyPort = 94,       /* UseSpecialReplyPort  */
  YYSYMBOL_ConsumeOnSendError = 95,        /* ConsumeOnSendError  */
  YYSYMBOL_UserTypeLimit = 96,             /* UserTypeLimit  */
  YYSYMBOL_OnStackLimit = 97,              /* OnStackLimit  */
  YYSYMBOL_WaitTime = 98,                  /* WaitTime  */
  YYSYMBOL_SendTime = 99,                  /* SendTime  */
  YYSYMBOL_Error = 100,                    /* Error  */
  YYSYMBOL_ServerPrefix = 101,             /* ServerPrefix  */
  YYSYMBOL_UserPrefix = 102,               /* UserPrefix  */
  YYSYMBOL_ServerDemux = 103,              /* ServerDemux  */
  YYSYMBOL_Import = 104,                   /* Import  */
  YYSYMBOL_ImportIndicant = 105,           /* ImportIndicant  */
  YYSYMBOL_RCSDecl = 106,                  /* RCSDecl  */
  YYSYMBOL_TypeDecl = 107,                 /* TypeDecl  */
  YYSYMBOL_NamedTypeSpec = 108,            /* NamedTypeSpec  */
  YYSYMBOL_TransTypeSpec = 109,            /* TransTypeSpec  */
  YYSYMBOL_TypeSpec = 110,                 /* TypeSpec  */
  YYSYMBOL_NativeTypeSpec = 111,           /* NativeTypeSpec  */
  YYSYMBOL_BasicTypeSpec = 112,            /* BasicTypeSpec  */
  YYSYMBOL_PrimIPCType = 113,              /* PrimIPCType  */
  YYSYMBOL_IPCType = 114,                  /* IPCType  */
  YYSYMBOL_PrevTypeSpec = 115,             /* PrevTypeSpec  */
  YYSYMBOL_VarArrayHead = 116,             /* VarArrayHead  */
  YYSYMBOL_ArrayHead = 117,                /* ArrayHead  */
  YYSYMBOL_StructHead = 118,               /* StructHead  */
  YYSYMBOL_CStringSpec = 119,              /* CStringSpec  */
  YYSYMBOL_TypePhrase = 120,               /* TypePhrase  */
  YYSYMBOL_IntExp = 121,                   /* IntExp  */
  YYSYMBOL_RoutineDecl = 122,              /* RoutineDecl  */
  YYSYMBOL_Routine = 123,                  /* Routine  */
  YYSYMBOL_SimpleRoutine = 124,            /* SimpleRoutine  */
  YYSYMBOL_Arguments = 125,                /* Arguments  */
  YYSYMBOL_ArgumentList = 126,             /* ArgumentList  */
  YYSYMBOL_Argument = 127,                 /* Argument  */
  YYSYMBOL_Trailer = 128,                  /* Trailer  */
  YYSYMBOL_Direction = 129,                /* Direction  */
  YYSYMBOL_TrImplKeyword = 130,            /* TrImplKeyword  */
  YYSYMBOL_ArgumentType = 131,             /* ArgumentType  */
  YYSYMBOL_IPCFlags = 132,                 /* IPCFlags  */
  YYSYMBOL_LookString = 133,               /* LookString  */
  YYSYMBOL_LookFileName = 134,             /* LookFileName  */
  YYSYMBOL_LookQString = 135               /* LookQString  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   271

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  84
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  52
/* YYNRULES -- Number of rules.  */
#define YYNRULES  134
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  255

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   338


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   198,   198,   199,   202,   203,   204,   205,   206,   207,
     208,   209,   210,   211,   212,   213,   214,   215,   225,   227,
     228,   229,   230,   234,   246,   257,   258,   261,   271,   279,
     282,   285,   300,   307,   323,   326,   330,   336,   344,   350,
     358,   366,   374,   382,   390,   401,   402,   403,   404,   405,
     408,   418,   428,   432,   434,   451,   468,   481,   493,   501,
     513,   515,   517,   519,   521,   523,   525,   527,   531,   533,
     536,   540,   546,   553,   559,   563,   565,   587,   591,   593,
     595,   600,   604,   608,   610,   615,   617,   621,   623,   625,
     627,   629,   631,   636,   637,   640,   644,   648,   650,   655,
     657,   659,   664,   671,   691,   701,   702,   703,   704,   705,
     706,   707,   708,   709,   710,   711,   712,   713,   714,   715,
     716,   717,   718,   719,   724,   725,   729,   735,   737,   742,
     743,   750,   759,   763,   767
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "sySkip", "syRoutine",
  "sySimpleRoutine", "sySubsystem", "syKernelUser", "syKernelServer",
  "syMsgOption", "syUseSpecialReplyPort", "syConsumeOnSendError",
  "syMsgSeqno", "syWaitTime", "sySendTime", "syNoWaitTime", "syNoSendTime",
  "syErrorProc", "syServerPrefix", "syUserPrefix", "syServerDemux",
  "syRCSId", "syImport", "syUImport", "sySImport", "syIImport",
  "syDImport", "syIn", "syOut", "syInOut", "syUserImpl", "syServerImpl",
  "syRequestPort", "syReplyPort", "sySReplyPort", "syUReplyPort", "syType",
  "syArray", "syStruct", "syOf", "syInTran", "syOutTran", "syDestructor",
  "syCType", "syCUserType", "syUserTypeLimit", "syOnStackLimit",
  "syCServerType", "syPointerTo", "syPointerToIfNot", "syValueOf",
  "syCString", "sySecToken", "syUserSecToken", "syServerSecToken",
  "syAuditToken", "syUserAuditToken", "syServerAuditToken",
  "syServerContextToken", "syColon", "sySemi", "syComma", "syPlus",
  "syMinus", "syStar", "syDiv", "syLParen", "syRParen", "syEqual",
  "syCaret", "syTilde", "syLAngle", "syRAngle", "syLBrack", "syRBrack",
  "syBar", "syError", "syNumber", "sySymbolicType", "syIdentifier",
  "syString", "syQString", "syFileName", "syIPCFlag", "$accept",
  "Statements", "Statement", "Subsystem", "SubsystemStart",
  "SubsystemMods", "SubsystemMod", "SubsystemName", "SubsystemBase",
  "MsgOption", "UseSpecialReplyPort", "ConsumeOnSendError",
  "UserTypeLimit", "OnStackLimit", "WaitTime", "SendTime", "Error",
  "ServerPrefix", "UserPrefix", "ServerDemux", "Import", "ImportIndicant",
  "RCSDecl", "TypeDecl", "NamedTypeSpec", "TransTypeSpec", "TypeSpec",
  "NativeTypeSpec", "BasicTypeSpec", "PrimIPCType", "IPCType",
  "PrevTypeSpec", "VarArrayHead", "ArrayHead", "StructHead", "CStringSpec",
  "TypePhrase", "IntExp", "RoutineDecl", "Routine", "SimpleRoutine",
  "Arguments", "ArgumentList", "Argument", "Trailer", "Direction",
  "TrImplKeyword", "ArgumentType", "IPCFlags", "LookString",
  "LookFileName", "LookQString", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-148)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-135)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -148,     2,  -148,   -56,   -46,   -43,   -39,  -148,   -35,  -148,
    -148,   -34,   -15,    11,    36,    46,    29,    59,  -148,  -148,
      24,  -148,    80,    92,    97,    98,    99,   100,   117,   118,
     120,   121,   123,   137,   144,   146,   147,  -148,  -148,    74,
     177,   163,  -148,  -148,   142,   142,  -148,  -148,  -148,  -148,
    -148,   141,  -148,  -148,  -148,  -148,    -7,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,   130,   132,   133,   134,  -148,  -148,  -148,  -148,
    -148,   129,   135,    64,  -148,  -148,   113,  -148,  -148,  -148,
    -148,   138,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,   150,
     158,   159,   143,   145,   148,   152,   154,   157,   160,   155,
      27,   113,  -148,  -148,  -148,    67,  -148,  -148,  -148,   156,
    -148,  -148,   113,   113,   113,  -148,  -148,  -148,  -148,   114,
     114,   168,   168,   -23,    47,   151,   151,   151,   -27,   171,
    -148,   170,   174,   175,   176,   178,   179,    27,  -148,  -148,
    -148,  -148,  -148,   -18,  -148,  -148,   -25,    47,   197,  -148,
      38,    91,  -148,   -21,   -26,    -8,   180,   111,    47,   161,
     162,   164,   165,   166,   167,  -148,   141,  -148,  -148,   181,
      47,   208,    70,  -148,    47,    47,    47,    47,   209,   210,
    -148,  -148,   151,  -148,    47,  -148,     5,   172,   173,   184,
    -148,  -148,  -148,   182,   124,  -148,  -148,    65,    65,  -148,
    -148,  -148,  -148,     7,   131,    -1,   187,   188,   183,   185,
     216,  -148,  -148,  -148,   189,   190,   192,   186,  -148,   194,
     196,  -148,  -148,  -148,  -148
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,    24,     0,    37,
      39,     0,     0,     0,     0,     0,     0,     0,    21,     3,
       0,    25,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    93,    94,     0,
       0,     0,    22,    18,     0,     0,    32,    40,    41,    42,
      43,     0,    51,    34,    35,     4,     0,     7,     8,     9,
      10,    11,     5,     6,    12,    13,    14,    15,    19,    20,
      16,    17,     0,     0,     0,     0,    45,    46,    47,    48,
      49,     0,     0,   105,    95,    96,     0,    27,    28,    29,
      26,     0,    31,    33,    36,    38,    44,    50,   115,   123,
     113,   114,   106,   107,   108,   125,   124,   109,   110,   111,
     112,   116,   118,   117,   119,   121,   120,   122,    97,     0,
      99,   100,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    73,    74,    77,    52,    53,    67,    60,    75,
      71,    61,     0,     0,     0,    66,    30,    23,    98,   105,
     105,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      64,     0,     0,     0,     0,     0,     0,     0,    62,    63,
      65,   101,   102,     0,   129,   104,     0,     0,     0,    91,
       0,     0,    85,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    76,   126,   127,   128,   103,
       0,     0,     0,    78,     0,     0,     0,     0,     0,     0,
      68,    86,     0,    70,     0,    83,   129,     0,     0,     0,
      57,    58,    59,     0,     0,    79,    92,    87,    88,    89,
      90,    81,    82,     0,     0,     0,     0,     0,     0,   130,
       0,    69,    84,    72,     0,     0,     0,     0,    80,     0,
       0,    56,   131,    54,    55
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,    83,  -148,   -87,    84,  -148,   103,
     136,  -148,  -148,  -148,  -148,  -148,  -147,  -125,  -148,  -148,
    -148,   219,   -11,  -148,  -148,  -148,  -148,   115,    55,  -148,
    -148,  -148
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     1,    19,    20,    21,    56,    90,    91,   147,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    81,    34,    35,    52,   135,   136,   137,   138,   139,
     140,   141,   142,   143,   144,   145,   183,   180,    36,    37,
      38,    84,   119,   120,   121,   122,   123,   174,   199,    39,
      40,    41
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      87,    88,     2,     3,    42,     4,     5,     6,     7,   184,
     185,  -132,     8,  -132,    43,  -132,  -132,     9,    10,    11,
      12,    13,    14,  -134,  -133,  -133,  -133,  -133,  -133,   181,
     126,   127,   128,   187,   200,   212,    44,   186,    15,   177,
      45,   176,    46,   177,   160,    47,   210,    16,    17,   201,
     179,   178,   202,   211,   179,   168,   169,   170,   211,   213,
     223,   196,    18,   216,    48,   233,   243,   204,   205,   206,
     207,   211,    89,    98,   241,   224,    99,   100,   101,   227,
     228,   229,   230,    72,    55,    73,   211,    74,    75,   234,
      49,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     204,   205,   206,   207,   132,   133,    53,   161,   162,   163,
     164,   165,   208,   177,   166,    50,   111,   112,   113,   114,
     115,   116,   117,    98,   179,    51,    99,   100,   101,   206,
     207,   118,   204,   205,   206,   207,    54,   226,   171,   172,
      57,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     124,   125,    58,   204,   205,   206,   207,    59,    60,    61,
      62,   126,   127,   128,   129,   209,   111,   112,   113,   114,
     115,   116,   117,   204,   205,   206,   207,    63,    64,   130,
      65,    66,   131,    67,    82,   215,   204,   205,   206,   207,
     132,   133,   134,   204,   205,   206,   207,    68,   240,    76,
      77,    78,    79,    80,    69,   242,    70,    71,    83,    86,
      92,    96,    93,    94,    95,   146,    97,   148,   149,   150,
     155,   153,   151,   156,   152,   154,   157,   173,   158,   189,
     182,   167,   188,   190,   191,   192,   203,   193,   194,   214,
     217,   218,   223,   219,   220,   221,   222,   225,   231,   232,
     238,   236,   237,   244,   245,   248,   197,   198,   247,   251,
     252,   253,   246,   254,    85,   239,   159,   175,   249,   250,
     195,   235
};

static const yytype_uint8 yycheck[] =
{
       7,     8,     0,     1,    60,     3,     4,     5,     6,   156,
     157,     9,    10,    11,    60,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,   154,
      48,    49,    50,   158,    59,    61,    79,    64,    36,    66,
      79,    64,    77,    66,   131,    79,    67,    45,    46,    74,
      77,    74,   177,    79,    77,   142,   143,   144,    79,    67,
      61,    79,    60,   188,    79,   212,    67,    62,    63,    64,
      65,    79,    79,     9,    67,   200,    12,    13,    14,   204,
     205,   206,   207,     9,    60,    11,    79,    13,    14,   214,
      79,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      62,    63,    64,    65,    77,    78,    77,    40,    41,    42,
      43,    44,    74,    66,    47,    79,    52,    53,    54,    55,
      56,    57,    58,     9,    77,    79,    12,    13,    14,    64,
      65,    67,    62,    63,    64,    65,    77,    67,   149,   150,
      60,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      37,    38,    60,    62,    63,    64,    65,    60,    60,    60,
      60,    48,    49,    50,    51,    74,    52,    53,    54,    55,
      56,    57,    58,    62,    63,    64,    65,    60,    60,    66,
      60,    60,    69,    60,    21,    74,    62,    63,    64,    65,
      77,    78,    79,    62,    63,    64,    65,    60,    74,    22,
      23,    24,    25,    26,    60,    74,    60,    60,    66,    68,
      80,    82,    80,    80,    80,    77,    81,    67,    60,    60,
      66,    73,    79,    66,    79,    73,    66,    59,    73,    59,
      79,    75,    61,    59,    59,    59,    39,    59,    59,    59,
      79,    79,    61,    79,    79,    79,    79,    39,    39,    39,
      66,    79,    79,    66,    66,    39,   173,   173,    73,    67,
      74,    67,    79,    67,    45,    83,   130,   152,    79,    79,
     167,   216
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    85,     0,     1,     3,     4,     5,     6,    10,    15,
      16,    17,    18,    19,    20,    36,    45,    46,    60,    86,
      87,    88,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   106,   107,   122,   123,   124,   133,
     134,   135,    60,    60,    79,    79,    77,    79,    79,    79,
      79,    79,   108,    77,    77,    60,    89,    60,    60,    60,
      60,    60,    60,    60,    60,    60,    60,    60,    60,    60,
      60,    60,     9,    11,    13,    14,    22,    23,    24,    25,
      26,   105,    21,    66,   125,   125,    68,     7,     8,    79,
      90,    91,    80,    80,    80,    80,    82,    81,     9,    12,
      13,    14,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    52,    53,    54,    55,    56,    57,    58,    67,   126,
     127,   128,   129,   130,    37,    38,    48,    49,    50,    51,
      66,    69,    77,    78,    79,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,    77,    92,    67,    60,
      60,    79,    79,    73,    73,    66,    66,    66,    73,   114,
     110,    40,    41,    42,    43,    44,    47,    75,   110,   110,
     110,   126,   126,    59,   131,   131,    64,    66,    74,    77,
     121,   121,    79,   120,   120,   120,    64,   121,    61,    59,
      59,    59,    59,    59,    59,   113,    79,   108,   111,   132,
      59,    74,   121,    39,    62,    63,    64,    65,    74,    74,
      67,    79,    61,    67,    59,    74,   121,    79,    79,    79,
      79,    79,    79,    61,   121,    39,    67,   121,   121,   121,
     121,    39,    39,   120,   121,   132,    79,    79,    66,    83,
      74,    67,    74,    67,    66,    66,    79,    73,    39,    79,
      79,    67,    74,    67,    67
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,    84,    85,    85,    86,    86,    86,    86,    86,    86,
      86,    86,    86,    86,    86,    86,    86,    86,    86,    86,
      86,    86,    86,    87,    88,    89,    89,    90,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    98,    99,    99,
     100,   101,   102,   103,   104,   105,   105,   105,   105,   105,
     106,   107,   108,   109,   109,   109,   109,   109,   109,   109,
     110,   110,   110,   110,   110,   110,   110,   110,   111,   111,
     111,   112,   112,   113,   113,   114,   114,   115,   116,   116,
     116,   117,   118,   119,   119,   120,   120,   121,   121,   121,
     121,   121,   121,   122,   122,   123,   124,   125,   125,   126,
     126,   126,   126,   127,   128,   129,   129,   129,   129,   129,
     129,   129,   129,   129,   129,   129,   129,   129,   129,   129,
     129,   129,   129,   129,   130,   130,   131,   131,   131,   132,
     132,   132,   133,   134,   135
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     1,     2,     4,     1,     0,     2,     1,     1,     1,
       1,     3,     2,     3,     2,     2,     3,     1,     3,     1,
       2,     2,     2,     2,     3,     1,     1,     1,     1,     1,
       3,     2,     3,     1,     8,     8,     7,     4,     4,     4,
       1,     1,     2,     2,     2,     2,     1,     1,     4,     6,
       4,     1,     6,     1,     1,     1,     3,     1,     4,     5,
       7,     5,     5,     4,     6,     1,     2,     3,     3,     3,
       3,     1,     3,     1,     1,     3,     3,     2,     3,     1,
       1,     3,     3,     4,     3,     0,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     2,     2,     0,
       3,     5,     0,     0,     0
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 17: /* Statement: RoutineDecl sySemi  */
#line 216 "parser.y"
{
    statement_t *st = stAlloc();

    st->stKind = skRoutine;
    st->stRoutine = (yyvsp[-1].routine);
    rtCheckRoutine((yyvsp[-1].routine));
    if (BeVerbose)
      rtPrintRoutine((yyvsp[-1].routine));
}
#line 1562 "parser.c"
    break;

  case 18: /* Statement: sySkip sySemi  */
#line 226 "parser.y"
                                { rtSkip(); }
#line 1568 "parser.c"
    break;

  case 22: /* Statement: error sySemi  */
#line 231 "parser.y"
                                { yyerrok; }
#line 1574 "parser.c"
    break;

  case 23: /* Subsystem: SubsystemStart SubsystemMods SubsystemName SubsystemBase  */
#line 236 "parser.y"
{
  if (BeVerbose) {
    printf("Subsystem %s: base = %u%s%s\n\n",
    SubsystemName, SubsystemBase,
    IsKernelUser ? ", KernelUser" : "",
    IsKernelServer ? ", KernelServer" : "");
  }
}
#line 1587 "parser.c"
    break;

  case 24: /* SubsystemStart: sySubsystem  */
#line 247 "parser.y"
{
    if (SubsystemName != strNULL) {
      warn("previous Subsystem decl (of %s) will be ignored", SubsystemName);
      IsKernelUser = FALSE;
      IsKernelServer = FALSE;
      strfree(SubsystemName);
    }
}
#line 1600 "parser.c"
    break;

  case 27: /* SubsystemMod: syKernelUser  */
#line 262 "parser.y"
{
    if (IsKernelUser)
      warn("duplicate KernelUser keyword");
    if (!UseMsgRPC) {
      warn("with KernelUser the -R option is meaningless");
      UseMsgRPC = TRUE;
    }
    IsKernelUser = TRUE;
}
#line 1614 "parser.c"
    break;

  case 28: /* SubsystemMod: syKernelServer  */
#line 272 "parser.y"
{
    if (IsKernelServer)
      warn("duplicate KernelServer keyword");
    IsKernelServer = TRUE;
}
#line 1624 "parser.c"
    break;

  case 29: /* SubsystemName: syIdentifier  */
#line 279 "parser.y"
                                                { SubsystemName = (yyvsp[0].identifier); }
#line 1630 "parser.c"
    break;

  case 30: /* SubsystemBase: syNumber  */
#line 282 "parser.y"
                                                { SubsystemBase = (yyvsp[0].number); }
#line 1636 "parser.c"
    break;

  case 31: /* MsgOption: LookString syMsgOption syString  */
#line 286 "parser.y"
{
    if (streql((yyvsp[0].string), "MACH_MSG_OPTION_NONE")) {
      MsgOption = strNULL;
      if (BeVerbose)
          printf("MsgOption: canceled\n\n");
    }
    else {
      MsgOption = (yyvsp[0].string);
      if (BeVerbose)
          printf("MsgOption %s\n\n",(yyvsp[0].string));
    }
}
#line 1653 "parser.c"
    break;

  case 32: /* UseSpecialReplyPort: syUseSpecialReplyPort syNumber  */
#line 301 "parser.y"
{
    UseSpecialReplyPort = ((yyvsp[0].number) != 0);
    HasUseSpecialReplyPort |= UseSpecialReplyPort;
}
#line 1662 "parser.c"
    break;

  case 33: /* ConsumeOnSendError: LookString syConsumeOnSendError syString  */
#line 308 "parser.y"
{
    if (strcasecmp((yyvsp[0].string), "None") == 0) {
        ConsumeOnSendError = ConsumeOnSendErrorNone;
    } else if (strcasecmp((yyvsp[0].string), "Timeout") == 0) {
        ConsumeOnSendError = ConsumeOnSendErrorTimeout;
        HasConsumeOnSendError = TRUE;
    } else if (strcasecmp((yyvsp[0].string), "Any") == 0) {
        ConsumeOnSendError = ConsumeOnSendErrorAny;
        HasConsumeOnSendError = TRUE;
    } else {
        error("syntax error");
    }
}
#line 1680 "parser.c"
    break;

  case 34: /* UserTypeLimit: syUserTypeLimit syNumber  */
#line 324 "parser.y"
                                {UserTypeLimit = (yyvsp[0].number); }
#line 1686 "parser.c"
    break;

  case 35: /* OnStackLimit: syOnStackLimit syNumber  */
#line 327 "parser.y"
                                {MaxMessSizeOnStack = (yyvsp[0].number); }
#line 1692 "parser.c"
    break;

  case 36: /* WaitTime: LookString syWaitTime syString  */
#line 331 "parser.y"
{
    WaitTime = (yyvsp[0].string);
    if (BeVerbose)
      printf("WaitTime %s\n\n", WaitTime);
}
#line 1702 "parser.c"
    break;

  case 37: /* WaitTime: syNoWaitTime  */
#line 337 "parser.y"
{
    WaitTime = strNULL;
    if (BeVerbose)
      printf("NoWaitTime\n\n");
}
#line 1712 "parser.c"
    break;

  case 38: /* SendTime: LookString sySendTime syString  */
#line 345 "parser.y"
{
    SendTime = (yyvsp[0].string);
    if (BeVerbose)
      printf("SendTime %s\n\n", SendTime);
}
#line 1722 "parser.c"
    break;

  case 39: /* SendTime: syNoSendTime  */
#line 351 "parser.y"
{
    SendTime = strNULL;
    if (BeVerbose)
      printf("NoSendTime\n\n");
}
#line 1732 "parser.c"
    break;

  case 40: /* Error: syErrorProc syIdentifier  */
#line 359 "parser.y"
{
    ErrorProc = (yyvsp[0].identifier);
    if (BeVerbose)
      printf("ErrorProc %s\n\n", ErrorProc);
}
#line 1742 "parser.c"
    break;

  case 41: /* ServerPrefix: syServerPrefix syIdentifier  */
#line 367 "parser.y"
{
    ServerPrefix = (yyvsp[0].identifier);
    if (BeVerbose)
      printf("ServerPrefix %s\n\n", ServerPrefix);
}
#line 1752 "parser.c"
    break;

  case 42: /* UserPrefix: syUserPrefix syIdentifier  */
#line 375 "parser.y"
{
    UserPrefix = (yyvsp[0].identifier);
    if (BeVerbose)
      printf("UserPrefix %s\n\n", UserPrefix);
}
#line 1762 "parser.c"
    break;

  case 43: /* ServerDemux: syServerDemux syIdentifier  */
#line 383 "parser.y"
{
    ServerDemux = (yyvsp[0].identifier);
    if (BeVerbose)
      printf("ServerDemux %s\n\n", ServerDemux);
}
#line 1772 "parser.c"
    break;

  case 44: /* Import: LookFileName ImportIndicant syFileName  */
#line 391 "parser.y"
{
    statement_t *st = stAlloc();
    st->stKind = (yyvsp[-1].statement_kind);
    st->stFileName = (yyvsp[0].string);

    if (BeVerbose)
      printf("%s %s\n\n", import_name((yyvsp[-1].statement_kind)), (yyvsp[0].string));
}
#line 1785 "parser.c"
    break;

  case 45: /* ImportIndicant: syImport  */
#line 401 "parser.y"
                                                { (yyval.statement_kind) = skImport; }
#line 1791 "parser.c"
    break;

  case 46: /* ImportIndicant: syUImport  */
#line 402 "parser.y"
                                                { (yyval.statement_kind) = skUImport; }
#line 1797 "parser.c"
    break;

  case 47: /* ImportIndicant: sySImport  */
#line 403 "parser.y"
                                                { (yyval.statement_kind) = skSImport; }
#line 1803 "parser.c"
    break;

  case 48: /* ImportIndicant: syIImport  */
#line 404 "parser.y"
                                                { (yyval.statement_kind) = skIImport; }
#line 1809 "parser.c"
    break;

  case 49: /* ImportIndicant: syDImport  */
#line 405 "parser.y"
                                                { (yyval.statement_kind) = skDImport; }
#line 1815 "parser.c"
    break;

  case 50: /* RCSDecl: LookQString syRCSId syQString  */
#line 409 "parser.y"
{
    if (RCSId != strNULL)
      warn("previous RCS decl will be ignored");
    if (BeVerbose)
      printf("RCSId %s\n\n", (yyvsp[0].string));
    RCSId = (yyvsp[0].string);
}
#line 1827 "parser.c"
    break;

  case 51: /* TypeDecl: syType NamedTypeSpec  */
#line 419 "parser.y"
{
    identifier_t name = (yyvsp[0].type)->itName;

    if (itLookUp(name) != itNULL)
      warn("overriding previous definition of %s", name);
    itInsert(name, (yyvsp[0].type));
}
#line 1839 "parser.c"
    break;

  case 52: /* NamedTypeSpec: syIdentifier syEqual TransTypeSpec  */
#line 429 "parser.y"
                                { itTypeDecl((yyvsp[-2].identifier), (yyval.type) = (yyvsp[0].type)); }
#line 1845 "parser.c"
    break;

  case 53: /* TransTypeSpec: TypeSpec  */
#line 433 "parser.y"
                                { (yyval.type) = itResetType((yyvsp[0].type)); }
#line 1851 "parser.c"
    break;

  case 54: /* TransTypeSpec: TransTypeSpec syInTran syColon syIdentifier syIdentifier syLParen syIdentifier syRParen  */
#line 436 "parser.y"
{
    (yyval.type) = (yyvsp[-7].type);

    if (((yyval.type)->itTransType != strNULL) && !streql((yyval.type)->itTransType, (yyvsp[-4].identifier)))
      warn("conflicting translation types (%s, %s)", (yyval.type)->itTransType, (yyvsp[-4].identifier));
    (yyval.type)->itTransType = (yyvsp[-4].identifier);

    if (((yyval.type)->itInTrans != strNULL) && !streql((yyval.type)->itInTrans, (yyvsp[-3].identifier)))
      warn("conflicting in-translation functions (%s, %s)", (yyval.type)->itInTrans, (yyvsp[-3].identifier));
    (yyval.type)->itInTrans = (yyvsp[-3].identifier);

    if (((yyval.type)->itServerType != strNULL) && !streql((yyval.type)->itServerType, (yyvsp[-1].identifier)))
      warn("conflicting server types (%s, %s)", (yyval.type)->itServerType, (yyvsp[-1].identifier));
    (yyval.type)->itServerType = (yyvsp[-1].identifier);
}
#line 1871 "parser.c"
    break;

  case 55: /* TransTypeSpec: TransTypeSpec syOutTran syColon syIdentifier syIdentifier syLParen syIdentifier syRParen  */
#line 453 "parser.y"
{
    (yyval.type) = (yyvsp[-7].type);

    if (((yyval.type)->itServerType != strNULL) && !streql((yyval.type)->itServerType, (yyvsp[-4].identifier)))
      warn("conflicting server types (%s, %s)", (yyval.type)->itServerType, (yyvsp[-4].identifier));
    (yyval.type)->itServerType = (yyvsp[-4].identifier);

    if (((yyval.type)->itOutTrans != strNULL) && !streql((yyval.type)->itOutTrans, (yyvsp[-3].identifier)))
      warn("conflicting out-translation functions (%s, %s)", (yyval.type)->itOutTrans, (yyvsp[-3].identifier));
    (yyval.type)->itOutTrans = (yyvsp[-3].identifier);

    if (((yyval.type)->itTransType != strNULL) && !streql((yyval.type)->itTransType, (yyvsp[-1].identifier)))
      warn("conflicting translation types (%s, %s)", (yyval.type)->itTransType, (yyvsp[-1].identifier));
    (yyval.type)->itTransType = (yyvsp[-1].identifier);
}
#line 1891 "parser.c"
    break;

  case 56: /* TransTypeSpec: TransTypeSpec syDestructor syColon syIdentifier syLParen syIdentifier syRParen  */
#line 470 "parser.y"
{
    (yyval.type) = (yyvsp[-6].type);

    if (((yyval.type)->itDestructor != strNULL) && !streql((yyval.type)->itDestructor, (yyvsp[-3].identifier)))
      warn("conflicting destructor functions (%s, %s)", (yyval.type)->itDestructor, (yyvsp[-3].identifier));
    (yyval.type)->itDestructor = (yyvsp[-3].identifier);

    if (((yyval.type)->itTransType != strNULL) && !streql((yyval.type)->itTransType, (yyvsp[-1].identifier)))
      warn("conflicting translation types (%s, %s)", (yyval.type)->itTransType, (yyvsp[-1].identifier));
    (yyval.type)->itTransType = (yyvsp[-1].identifier);
}
#line 1907 "parser.c"
    break;

  case 57: /* TransTypeSpec: TransTypeSpec syCType syColon syIdentifier  */
#line 482 "parser.y"
{
    (yyval.type) = (yyvsp[-3].type);

    if (((yyval.type)->itUserType != strNULL) && !streql((yyval.type)->itUserType, (yyvsp[0].identifier)))
      warn("conflicting user types (%s, %s)", (yyval.type)->itUserType, (yyvsp[0].identifier));
    (yyval.type)->itUserType = (yyvsp[0].identifier);

    if (((yyval.type)->itServerType != strNULL) && !streql((yyval.type)->itServerType, (yyvsp[0].identifier)))
      warn("conflicting server types (%s, %s)", (yyval.type)->itServerType, (yyvsp[0].identifier));
    (yyval.type)->itServerType = (yyvsp[0].identifier);
}
#line 1923 "parser.c"
    break;

  case 58: /* TransTypeSpec: TransTypeSpec syCUserType syColon syIdentifier  */
#line 494 "parser.y"
{
    (yyval.type) = (yyvsp[-3].type);

    if (((yyval.type)->itUserType != strNULL) && !streql((yyval.type)->itUserType, (yyvsp[0].identifier)))
      warn("conflicting user types (%s, %s)", (yyval.type)->itUserType, (yyvsp[0].identifier));
    (yyval.type)->itUserType = (yyvsp[0].identifier);
}
#line 1935 "parser.c"
    break;

  case 59: /* TransTypeSpec: TransTypeSpec syCServerType syColon syIdentifier  */
#line 503 "parser.y"
{
    (yyval.type) = (yyvsp[-3].type);

    if (((yyval.type)->itServerType != strNULL) && !streql((yyval.type)->itServerType, (yyvsp[0].identifier)))
	warn("conflicting server types (%s, %s)",
	     (yyval.type)->itServerType, (yyvsp[0].identifier));
    (yyval.type)->itServerType = (yyvsp[0].identifier);
}
#line 1948 "parser.c"
    break;

  case 60: /* TypeSpec: BasicTypeSpec  */
#line 514 "parser.y"
                                { (yyval.type) = (yyvsp[0].type); }
#line 1954 "parser.c"
    break;

  case 61: /* TypeSpec: PrevTypeSpec  */
#line 516 "parser.y"
                                { (yyval.type) = (yyvsp[0].type); }
#line 1960 "parser.c"
    break;

  case 62: /* TypeSpec: VarArrayHead TypeSpec  */
#line 518 "parser.y"
                                { (yyval.type) = itVarArrayDecl((yyvsp[-1].number), (yyvsp[0].type)); }
#line 1966 "parser.c"
    break;

  case 63: /* TypeSpec: ArrayHead TypeSpec  */
#line 520 "parser.y"
                                { (yyval.type) = itArrayDecl((yyvsp[-1].number), (yyvsp[0].type)); }
#line 1972 "parser.c"
    break;

  case 64: /* TypeSpec: syCaret TypeSpec  */
#line 522 "parser.y"
                                { (yyval.type) = itPtrDecl((yyvsp[0].type)); }
#line 1978 "parser.c"
    break;

  case 65: /* TypeSpec: StructHead TypeSpec  */
#line 524 "parser.y"
                                { (yyval.type) = itStructDecl((yyvsp[-1].number), (yyvsp[0].type)); }
#line 1984 "parser.c"
    break;

  case 66: /* TypeSpec: CStringSpec  */
#line 526 "parser.y"
                                { (yyval.type) = (yyvsp[0].type); }
#line 1990 "parser.c"
    break;

  case 67: /* TypeSpec: NativeTypeSpec  */
#line 528 "parser.y"
                                { (yyval.type) = (yyvsp[0].type); }
#line 1996 "parser.c"
    break;

  case 68: /* NativeTypeSpec: syPointerTo syLParen TypePhrase syRParen  */
#line 532 "parser.y"
                                { (yyval.type) = itNativeType((yyvsp[-1].identifier), TRUE, 0); }
#line 2002 "parser.c"
    break;

  case 69: /* NativeTypeSpec: syPointerToIfNot syLParen TypePhrase syComma TypePhrase syRParen  */
#line 535 "parser.y"
                                { (yyval.type) = itNativeType((yyvsp[-3].identifier), TRUE, (yyvsp[-1].identifier)); }
#line 2008 "parser.c"
    break;

  case 70: /* NativeTypeSpec: syValueOf syLParen TypePhrase syRParen  */
#line 537 "parser.y"
                                { (yyval.type) = itNativeType((yyvsp[-1].identifier), FALSE, 0); }
#line 2014 "parser.c"
    break;

  case 71: /* BasicTypeSpec: IPCType  */
#line 541 "parser.y"
{
    (yyval.type) = itShortDecl((yyvsp[0].symtype).innumber, (yyvsp[0].symtype).instr,
		     (yyvsp[0].symtype).outnumber, (yyvsp[0].symtype).outstr,
		     (yyvsp[0].symtype).size);
}
#line 2024 "parser.c"
    break;

  case 72: /* BasicTypeSpec: syLParen IPCType syComma IntExp IPCFlags syRParen  */
#line 548 "parser.y"
{
    error("Long form type declarations aren't allowed any longer\n");
}
#line 2032 "parser.c"
    break;

  case 73: /* PrimIPCType: syNumber  */
#line 554 "parser.y"
{
    (yyval.symtype).innumber = (yyval.symtype).outnumber = (yyvsp[0].number);
    (yyval.symtype).instr = (yyval.symtype).outstr = strNULL;
    (yyval.symtype).size = 0;
}
#line 2042 "parser.c"
    break;

  case 74: /* PrimIPCType: sySymbolicType  */
#line 560 "parser.y"
                                { (yyval.symtype) = (yyvsp[0].symtype); }
#line 2048 "parser.c"
    break;

  case 75: /* IPCType: PrimIPCType  */
#line 564 "parser.y"
                                { (yyval.symtype) = (yyvsp[0].symtype); }
#line 2054 "parser.c"
    break;

  case 76: /* IPCType: PrimIPCType syBar PrimIPCType  */
#line 566 "parser.y"
{
  if ((yyvsp[-2].symtype).size != (yyvsp[0].symtype).size) {
    if ((yyvsp[-2].symtype).size == 0)
      (yyval.symtype).size = (yyvsp[0].symtype).size;
    else if ((yyvsp[0].symtype).size == 0)
      (yyval.symtype).size = (yyvsp[-2].symtype).size;
    else {
      error("sizes in IPCTypes (%d, %d) aren't equal",
      (yyvsp[-2].symtype).size, (yyvsp[0].symtype).size);
      (yyval.symtype).size = 0;
    }
  }
  else
    (yyval.symtype).size = (yyvsp[-2].symtype).size;
  (yyval.symtype).innumber = (yyvsp[-2].symtype).innumber;
  (yyval.symtype).instr = (yyvsp[-2].symtype).instr;
  (yyval.symtype).outnumber = (yyvsp[0].symtype).outnumber;
  (yyval.symtype).outstr = (yyvsp[0].symtype).outstr;
}
#line 2078 "parser.c"
    break;

  case 77: /* PrevTypeSpec: syIdentifier  */
#line 588 "parser.y"
                                { (yyval.type) = itPrevDecl((yyvsp[0].identifier)); }
#line 2084 "parser.c"
    break;

  case 78: /* VarArrayHead: syArray syLBrack syRBrack syOf  */
#line 592 "parser.y"
                                { (yyval.number) = 0; }
#line 2090 "parser.c"
    break;

  case 79: /* VarArrayHead: syArray syLBrack syStar syRBrack syOf  */
#line 594 "parser.y"
                                { (yyval.number) = 0; }
#line 2096 "parser.c"
    break;

  case 80: /* VarArrayHead: syArray syLBrack syStar syColon IntExp syRBrack syOf  */
#line 597 "parser.y"
                                { (yyval.number) = (yyvsp[-2].number); }
#line 2102 "parser.c"
    break;

  case 81: /* ArrayHead: syArray syLBrack IntExp syRBrack syOf  */
#line 601 "parser.y"
                                { (yyval.number) = (yyvsp[-2].number); }
#line 2108 "parser.c"
    break;

  case 82: /* StructHead: syStruct syLBrack IntExp syRBrack syOf  */
#line 605 "parser.y"
                                { (yyval.number) = (yyvsp[-2].number); }
#line 2114 "parser.c"
    break;

  case 83: /* CStringSpec: syCString syLBrack IntExp syRBrack  */
#line 609 "parser.y"
                                { (yyval.type) = itCStringDecl((yyvsp[-1].number), FALSE); }
#line 2120 "parser.c"
    break;

  case 84: /* CStringSpec: syCString syLBrack syStar syColon IntExp syRBrack  */
#line 612 "parser.y"
                                { (yyval.type) = itCStringDecl((yyvsp[-1].number), TRUE); }
#line 2126 "parser.c"
    break;

  case 85: /* TypePhrase: syIdentifier  */
#line 616 "parser.y"
                                { (yyval.identifier) = (yyvsp[0].identifier); }
#line 2132 "parser.c"
    break;

  case 86: /* TypePhrase: TypePhrase syIdentifier  */
#line 618 "parser.y"
                                { (yyval.identifier) = strphrase((yyvsp[-1].identifier), (yyvsp[0].identifier)); strfree((yyvsp[0].identifier)); }
#line 2138 "parser.c"
    break;

  case 87: /* IntExp: IntExp syPlus IntExp  */
#line 622 "parser.y"
                                { (yyval.number) = (yyvsp[-2].number) + (yyvsp[0].number);	}
#line 2144 "parser.c"
    break;

  case 88: /* IntExp: IntExp syMinus IntExp  */
#line 624 "parser.y"
                                { (yyval.number) = (yyvsp[-2].number) - (yyvsp[0].number);	}
#line 2150 "parser.c"
    break;

  case 89: /* IntExp: IntExp syStar IntExp  */
#line 626 "parser.y"
                                { (yyval.number) = (yyvsp[-2].number) * (yyvsp[0].number);	}
#line 2156 "parser.c"
    break;

  case 90: /* IntExp: IntExp syDiv IntExp  */
#line 628 "parser.y"
                                { (yyval.number) = (yyvsp[-2].number) / (yyvsp[0].number);	}
#line 2162 "parser.c"
    break;

  case 91: /* IntExp: syNumber  */
#line 630 "parser.y"
                                { (yyval.number) = (yyvsp[0].number);	}
#line 2168 "parser.c"
    break;

  case 92: /* IntExp: syLParen IntExp syRParen  */
#line 632 "parser.y"
                                { (yyval.number) = (yyvsp[-1].number);	}
#line 2174 "parser.c"
    break;

  case 93: /* RoutineDecl: Routine  */
#line 636 "parser.y"
                                                        { (yyval.routine) = (yyvsp[0].routine); }
#line 2180 "parser.c"
    break;

  case 94: /* RoutineDecl: SimpleRoutine  */
#line 637 "parser.y"
                                                        { (yyval.routine) = (yyvsp[0].routine); }
#line 2186 "parser.c"
    break;

  case 95: /* Routine: syRoutine syIdentifier Arguments  */
#line 641 "parser.y"
                                { (yyval.routine) = rtMakeRoutine((yyvsp[-1].identifier), (yyvsp[0].argument)); }
#line 2192 "parser.c"
    break;

  case 96: /* SimpleRoutine: sySimpleRoutine syIdentifier Arguments  */
#line 645 "parser.y"
                                { (yyval.routine) = rtMakeSimpleRoutine((yyvsp[-1].identifier), (yyvsp[0].argument)); }
#line 2198 "parser.c"
    break;

  case 97: /* Arguments: syLParen syRParen  */
#line 649 "parser.y"
                                { (yyval.argument) = argNULL; }
#line 2204 "parser.c"
    break;

  case 98: /* Arguments: syLParen ArgumentList syRParen  */
#line 651 "parser.y"
                                { (yyval.argument) = (yyvsp[-1].argument); }
#line 2210 "parser.c"
    break;

  case 99: /* ArgumentList: Argument  */
#line 656 "parser.y"
                                { (yyval.argument) = (yyvsp[0].argument); }
#line 2216 "parser.c"
    break;

  case 100: /* ArgumentList: Trailer  */
#line 658 "parser.y"
                                { (yyval.argument) = (yyvsp[0].argument); }
#line 2222 "parser.c"
    break;

  case 101: /* ArgumentList: Argument sySemi ArgumentList  */
#line 660 "parser.y"
{
  (yyval.argument) = (yyvsp[-2].argument);
  (yyval.argument)->argNext = (yyvsp[0].argument);
}
#line 2231 "parser.c"
    break;

  case 102: /* ArgumentList: Trailer sySemi ArgumentList  */
#line 665 "parser.y"
{
  (yyval.argument) = (yyvsp[-2].argument);
  (yyval.argument)->argNext = (yyvsp[0].argument);
}
#line 2240 "parser.c"
    break;

  case 103: /* Argument: Direction syIdentifier ArgumentType IPCFlags  */
#line 672 "parser.y"
{
  (yyval.argument) = argAlloc();
  (yyval.argument)->argKind = (yyvsp[-3].direction);
  (yyval.argument)->argName = (yyvsp[-2].identifier);
  (yyval.argument)->argType = (yyvsp[-1].type);
  (yyval.argument)->argFlags = (yyvsp[0].flag);
  if ((yyvsp[-1].type) && (yyvsp[-1].type)->itNative) {
      if ((yyvsp[-3].direction) != akIn && (yyvsp[-3].direction) != akOut && (yyvsp[-3].direction) != akInOut)
        error("Illegal direction specified");

      if (!((yyvsp[-1].type)->itNativePointer) && (yyvsp[-3].direction) != akIn)
        error("ValueOf only valid for in");

      if (((yyvsp[-1].type)->itBadValue) != NULL && (yyvsp[-3].direction) != akIn)
        error("PointerToIfNot only valid for in");
  }
}
#line 2262 "parser.c"
    break;

  case 104: /* Trailer: TrImplKeyword syIdentifier ArgumentType  */
#line 692 "parser.y"
{
    (yyval.argument) = argAlloc();
    (yyval.argument)->argKind = (yyvsp[-2].direction);
    (yyval.argument)->argName = (yyvsp[-1].identifier);
    (yyval.argument)->argType = (yyvsp[0].type);
}
#line 2273 "parser.c"
    break;

  case 105: /* Direction: %empty  */
#line 701 "parser.y"
                                                { (yyval.direction) = akNone; }
#line 2279 "parser.c"
    break;

  case 106: /* Direction: syIn  */
#line 702 "parser.y"
                                                { (yyval.direction) = akIn; }
#line 2285 "parser.c"
    break;

  case 107: /* Direction: syOut  */
#line 703 "parser.y"
                                                { (yyval.direction) = akOut; }
#line 2291 "parser.c"
    break;

  case 108: /* Direction: syInOut  */
#line 704 "parser.y"
                                                { (yyval.direction) = akInOut; }
#line 2297 "parser.c"
    break;

  case 109: /* Direction: syRequestPort  */
#line 705 "parser.y"
                                                { (yyval.direction) = akRequestPort; }
#line 2303 "parser.c"
    break;

  case 110: /* Direction: syReplyPort  */
#line 706 "parser.y"
                                                { (yyval.direction) = akReplyPort; }
#line 2309 "parser.c"
    break;

  case 111: /* Direction: sySReplyPort  */
#line 707 "parser.y"
                                                { (yyval.direction) = akSReplyPort; }
#line 2315 "parser.c"
    break;

  case 112: /* Direction: syUReplyPort  */
#line 708 "parser.y"
                                                { (yyval.direction) = akUReplyPort; }
#line 2321 "parser.c"
    break;

  case 113: /* Direction: syWaitTime  */
#line 709 "parser.y"
                                                { (yyval.direction) = akWaitTime; }
#line 2327 "parser.c"
    break;

  case 114: /* Direction: sySendTime  */
#line 710 "parser.y"
                                                { (yyval.direction) = akSendTime; }
#line 2333 "parser.c"
    break;

  case 115: /* Direction: syMsgOption  */
#line 711 "parser.y"
                                                { (yyval.direction) = akMsgOption; }
#line 2339 "parser.c"
    break;

  case 116: /* Direction: sySecToken  */
#line 712 "parser.y"
                                                        { (yyval.direction) = akSecToken; }
#line 2345 "parser.c"
    break;

  case 117: /* Direction: syServerSecToken  */
#line 713 "parser.y"
                                                        { (yyval.direction) = akServerSecToken; }
#line 2351 "parser.c"
    break;

  case 118: /* Direction: syUserSecToken  */
#line 714 "parser.y"
                                                        { (yyval.direction) = akUserSecToken; }
#line 2357 "parser.c"
    break;

  case 119: /* Direction: syAuditToken  */
#line 715 "parser.y"
                                                        { (yyval.direction) = akAuditToken; }
#line 2363 "parser.c"
    break;

  case 120: /* Direction: syServerAuditToken  */
#line 716 "parser.y"
                                                        { (yyval.direction) = akServerAuditToken; }
#line 2369 "parser.c"
    break;

  case 121: /* Direction: syUserAuditToken  */
#line 717 "parser.y"
                                                        { (yyval.direction) = akUserAuditToken; }
#line 2375 "parser.c"
    break;

  case 122: /* Direction: syServerContextToken  */
#line 718 "parser.y"
                                                        { (yyval.direction) = akServerContextToken; }
#line 2381 "parser.c"
    break;

  case 123: /* Direction: syMsgSeqno  */
#line 719 "parser.y"
                                                { (yyval.direction) = akMsgSeqno; }
#line 2387 "parser.c"
    break;

  case 124: /* TrImplKeyword: syServerImpl  */
#line 724 "parser.y"
                                                { (yyval.direction) = akServerImpl; }
#line 2393 "parser.c"
    break;

  case 125: /* TrImplKeyword: syUserImpl  */
#line 725 "parser.y"
                                                { (yyval.direction) = akUserImpl; }
#line 2399 "parser.c"
    break;

  case 126: /* ArgumentType: syColon syIdentifier  */
#line 730 "parser.y"
{
  (yyval.type) = itLookUp((yyvsp[0].identifier));
  if ((yyval.type) == itNULL)
    error("type '%s' not defined", (yyvsp[0].identifier));
}
#line 2409 "parser.c"
    break;

  case 127: /* ArgumentType: syColon NamedTypeSpec  */
#line 736 "parser.y"
                                { (yyval.type) = (yyvsp[0].type); }
#line 2415 "parser.c"
    break;

  case 128: /* ArgumentType: syColon NativeTypeSpec  */
#line 738 "parser.y"
                                { (yyval.type) = (yyvsp[0].type); }
#line 2421 "parser.c"
    break;

  case 129: /* IPCFlags: %empty  */
#line 742 "parser.y"
                                { (yyval.flag) = flNone; }
#line 2427 "parser.c"
    break;

  case 130: /* IPCFlags: IPCFlags syComma syIPCFlag  */
#line 744 "parser.y"
{
  if ((yyvsp[-2].flag) & (yyvsp[0].flag))
    warn("redundant IPC flag ignored");
  else
    (yyval.flag) = (yyvsp[-2].flag) | (yyvsp[0].flag);
}
#line 2438 "parser.c"
    break;

  case 131: /* IPCFlags: IPCFlags syComma syIPCFlag syLBrack syRBrack  */
#line 751 "parser.y"
{
  if ((yyvsp[-2].flag) != flDealloc)
    warn("only Dealloc is variable");
  else
    (yyval.flag) = (yyvsp[-4].flag) | flMaybeDealloc;
}
#line 2449 "parser.c"
    break;

  case 132: /* LookString: %empty  */
#line 759 "parser.y"
                                { LookString(); }
#line 2455 "parser.c"
    break;

  case 133: /* LookFileName: %empty  */
#line 763 "parser.y"
                                { LookFileName(); }
#line 2461 "parser.c"
    break;

  case 134: /* LookQString: %empty  */
#line 767 "parser.y"
                                { LookQString(); }
#line 2467 "parser.c"
    break;


#line 2471 "parser.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 770 "parser.y"


void
yyerror(char *s)
{
  error(s);
}

static char *
import_name(statement_kind_t sk)
{
  switch (sk) {

    case skImport:
      return "Import";

    case skSImport:
      return "SImport";

    case skUImport:
      return "UImport";

    case skIImport:
      return "IImport";

    case skDImport:
      return "DImport";

    default:
      fatal("import_name(%d): not import statement", (int) sk);
      /*NOTREACHED*/
      return strNULL;
  }
}
