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
#line 155 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"


#include <stdio.h>
#include "lexxer.h"
#include "strdefs.h"
#include "type.h"
#include "routine.h"
#include "statement.h"
#include "global.h"
#include "error.h"

static char *import_name();
extern int yylex(void);

/* forward declaration */
void yyerror(char *s);


#line 90 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"

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

#include "y.tab.h"
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
  YYSYMBOL_syMsgSeqno = 11,                /* syMsgSeqno  */
  YYSYMBOL_syWaitTime = 12,                /* syWaitTime  */
  YYSYMBOL_sySendTime = 13,                /* sySendTime  */
  YYSYMBOL_syNoWaitTime = 14,              /* syNoWaitTime  */
  YYSYMBOL_syNoSendTime = 15,              /* syNoSendTime  */
  YYSYMBOL_syErrorProc = 16,               /* syErrorProc  */
  YYSYMBOL_syServerPrefix = 17,            /* syServerPrefix  */
  YYSYMBOL_syUserPrefix = 18,              /* syUserPrefix  */
  YYSYMBOL_syServerDemux = 19,             /* syServerDemux  */
  YYSYMBOL_syRCSId = 20,                   /* syRCSId  */
  YYSYMBOL_syImport = 21,                  /* syImport  */
  YYSYMBOL_syUImport = 22,                 /* syUImport  */
  YYSYMBOL_sySImport = 23,                 /* sySImport  */
  YYSYMBOL_syIImport = 24,                 /* syIImport  */
  YYSYMBOL_syDImport = 25,                 /* syDImport  */
  YYSYMBOL_syIn = 26,                      /* syIn  */
  YYSYMBOL_syOut = 27,                     /* syOut  */
  YYSYMBOL_syInOut = 28,                   /* syInOut  */
  YYSYMBOL_syUserImpl = 29,                /* syUserImpl  */
  YYSYMBOL_syServerImpl = 30,              /* syServerImpl  */
  YYSYMBOL_syRequestPort = 31,             /* syRequestPort  */
  YYSYMBOL_syReplyPort = 32,               /* syReplyPort  */
  YYSYMBOL_sySReplyPort = 33,              /* sySReplyPort  */
  YYSYMBOL_syUReplyPort = 34,              /* syUReplyPort  */
  YYSYMBOL_syType = 35,                    /* syType  */
  YYSYMBOL_syArray = 36,                   /* syArray  */
  YYSYMBOL_syStruct = 37,                  /* syStruct  */
  YYSYMBOL_syOf = 38,                      /* syOf  */
  YYSYMBOL_syInTran = 39,                  /* syInTran  */
  YYSYMBOL_syOutTran = 40,                 /* syOutTran  */
  YYSYMBOL_syDestructor = 41,              /* syDestructor  */
  YYSYMBOL_syCType = 42,                   /* syCType  */
  YYSYMBOL_syCUserType = 43,               /* syCUserType  */
  YYSYMBOL_syUserTypeLimit = 44,           /* syUserTypeLimit  */
  YYSYMBOL_syOnStackLimit = 45,            /* syOnStackLimit  */
  YYSYMBOL_syCServerType = 46,             /* syCServerType  */
  YYSYMBOL_syPointerTo = 47,               /* syPointerTo  */
  YYSYMBOL_syPointerToIfNot = 48,          /* syPointerToIfNot  */
  YYSYMBOL_syValueOf = 49,                 /* syValueOf  */
  YYSYMBOL_syCString = 50,                 /* syCString  */
  YYSYMBOL_sySecToken = 51,                /* sySecToken  */
  YYSYMBOL_syUserSecToken = 52,            /* syUserSecToken  */
  YYSYMBOL_syServerSecToken = 53,          /* syServerSecToken  */
  YYSYMBOL_syAuditToken = 54,              /* syAuditToken  */
  YYSYMBOL_syUserAuditToken = 55,          /* syUserAuditToken  */
  YYSYMBOL_syServerAuditToken = 56,        /* syServerAuditToken  */
  YYSYMBOL_syServerContextToken = 57,      /* syServerContextToken  */
  YYSYMBOL_syColon = 58,                   /* syColon  */
  YYSYMBOL_sySemi = 59,                    /* sySemi  */
  YYSYMBOL_syComma = 60,                   /* syComma  */
  YYSYMBOL_syPlus = 61,                    /* syPlus  */
  YYSYMBOL_syMinus = 62,                   /* syMinus  */
  YYSYMBOL_syStar = 63,                    /* syStar  */
  YYSYMBOL_syDiv = 64,                     /* syDiv  */
  YYSYMBOL_syLParen = 65,                  /* syLParen  */
  YYSYMBOL_syRParen = 66,                  /* syRParen  */
  YYSYMBOL_syEqual = 67,                   /* syEqual  */
  YYSYMBOL_syCaret = 68,                   /* syCaret  */
  YYSYMBOL_syTilde = 69,                   /* syTilde  */
  YYSYMBOL_syLAngle = 70,                  /* syLAngle  */
  YYSYMBOL_syRAngle = 71,                  /* syRAngle  */
  YYSYMBOL_syLBrack = 72,                  /* syLBrack  */
  YYSYMBOL_syRBrack = 73,                  /* syRBrack  */
  YYSYMBOL_syBar = 74,                     /* syBar  */
  YYSYMBOL_syError = 75,                   /* syError  */
  YYSYMBOL_syNumber = 76,                  /* syNumber  */
  YYSYMBOL_sySymbolicType = 77,            /* sySymbolicType  */
  YYSYMBOL_syIdentifier = 78,              /* syIdentifier  */
  YYSYMBOL_syString = 79,                  /* syString  */
  YYSYMBOL_syQString = 80,                 /* syQString  */
  YYSYMBOL_syFileName = 81,                /* syFileName  */
  YYSYMBOL_syIPCFlag = 82,                 /* syIPCFlag  */
  YYSYMBOL_YYACCEPT = 83,                  /* $accept  */
  YYSYMBOL_Statements = 84,                /* Statements  */
  YYSYMBOL_Statement = 85,                 /* Statement  */
  YYSYMBOL_Subsystem = 86,                 /* Subsystem  */
  YYSYMBOL_SubsystemStart = 87,            /* SubsystemStart  */
  YYSYMBOL_SubsystemMods = 88,             /* SubsystemMods  */
  YYSYMBOL_SubsystemMod = 89,              /* SubsystemMod  */
  YYSYMBOL_SubsystemName = 90,             /* SubsystemName  */
  YYSYMBOL_SubsystemBase = 91,             /* SubsystemBase  */
  YYSYMBOL_MsgOption = 92,                 /* MsgOption  */
  YYSYMBOL_UseSpecialReplyPort = 93,       /* UseSpecialReplyPort  */
  YYSYMBOL_UserTypeLimit = 94,             /* UserTypeLimit  */
  YYSYMBOL_OnStackLimit = 95,              /* OnStackLimit  */
  YYSYMBOL_WaitTime = 96,                  /* WaitTime  */
  YYSYMBOL_SendTime = 97,                  /* SendTime  */
  YYSYMBOL_Error = 98,                     /* Error  */
  YYSYMBOL_ServerPrefix = 99,              /* ServerPrefix  */
  YYSYMBOL_UserPrefix = 100,               /* UserPrefix  */
  YYSYMBOL_ServerDemux = 101,              /* ServerDemux  */
  YYSYMBOL_Import = 102,                   /* Import  */
  YYSYMBOL_ImportIndicant = 103,           /* ImportIndicant  */
  YYSYMBOL_RCSDecl = 104,                  /* RCSDecl  */
  YYSYMBOL_TypeDecl = 105,                 /* TypeDecl  */
  YYSYMBOL_NamedTypeSpec = 106,            /* NamedTypeSpec  */
  YYSYMBOL_TransTypeSpec = 107,            /* TransTypeSpec  */
  YYSYMBOL_TypeSpec = 108,                 /* TypeSpec  */
  YYSYMBOL_NativeTypeSpec = 109,           /* NativeTypeSpec  */
  YYSYMBOL_BasicTypeSpec = 110,            /* BasicTypeSpec  */
  YYSYMBOL_PrimIPCType = 111,              /* PrimIPCType  */
  YYSYMBOL_IPCType = 112,                  /* IPCType  */
  YYSYMBOL_PrevTypeSpec = 113,             /* PrevTypeSpec  */
  YYSYMBOL_VarArrayHead = 114,             /* VarArrayHead  */
  YYSYMBOL_ArrayHead = 115,                /* ArrayHead  */
  YYSYMBOL_StructHead = 116,               /* StructHead  */
  YYSYMBOL_CStringSpec = 117,              /* CStringSpec  */
  YYSYMBOL_TypePhrase = 118,               /* TypePhrase  */
  YYSYMBOL_IntExp = 119,                   /* IntExp  */
  YYSYMBOL_RoutineDecl = 120,              /* RoutineDecl  */
  YYSYMBOL_Routine = 121,                  /* Routine  */
  YYSYMBOL_SimpleRoutine = 122,            /* SimpleRoutine  */
  YYSYMBOL_Arguments = 123,                /* Arguments  */
  YYSYMBOL_ArgumentList = 124,             /* ArgumentList  */
  YYSYMBOL_Argument = 125,                 /* Argument  */
  YYSYMBOL_Trailer = 126,                  /* Trailer  */
  YYSYMBOL_Direction = 127,                /* Direction  */
  YYSYMBOL_TrImplKeyword = 128,            /* TrImplKeyword  */
  YYSYMBOL_ArgumentType = 129,             /* ArgumentType  */
  YYSYMBOL_IPCFlags = 130,                 /* IPCFlags  */
  YYSYMBOL_LookString = 131,               /* LookString  */
  YYSYMBOL_LookFileName = 132,             /* LookFileName  */
  YYSYMBOL_LookQString = 133               /* LookQString  */
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
#define YYLAST   272

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  83
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  51
/* YYNRULES -- Number of rules.  */
#define YYNRULES  132
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  251

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   337


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
      75,    76,    77,    78,    79,    80,    81,    82
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   197,   197,   198,   201,   202,   203,   204,   205,   206,
     207,   208,   209,   210,   211,   212,   213,   223,   225,   226,
     227,   228,   232,   244,   255,   256,   259,   269,   277,   280,
     283,   298,   305,   308,   312,   318,   326,   332,   340,   348,
     356,   364,   372,   383,   384,   385,   386,   387,   390,   400,
     410,   414,   416,   433,   450,   463,   475,   483,   495,   497,
     499,   501,   503,   505,   507,   509,   513,   515,   518,   522,
     528,   535,   541,   545,   547,   569,   573,   575,   577,   582,
     586,   590,   592,   597,   599,   603,   605,   607,   609,   611,
     613,   618,   619,   622,   626,   630,   632,   637,   639,   641,
     646,   653,   673,   683,   684,   685,   686,   687,   688,   689,
     690,   691,   692,   693,   694,   695,   696,   697,   698,   699,
     700,   701,   706,   707,   711,   717,   719,   724,   725,   732,
     741,   745,   749
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
  "syMsgOption", "syUseSpecialReplyPort", "syMsgSeqno", "syWaitTime",
  "sySendTime", "syNoWaitTime", "syNoSendTime", "syErrorProc",
  "syServerPrefix", "syUserPrefix", "syServerDemux", "syRCSId", "syImport",
  "syUImport", "sySImport", "syIImport", "syDImport", "syIn", "syOut",
  "syInOut", "syUserImpl", "syServerImpl", "syRequestPort", "syReplyPort",
  "sySReplyPort", "syUReplyPort", "syType", "syArray", "syStruct", "syOf",
  "syInTran", "syOutTran", "syDestructor", "syCType", "syCUserType",
  "syUserTypeLimit", "syOnStackLimit", "syCServerType", "syPointerTo",
  "syPointerToIfNot", "syValueOf", "syCString", "sySecToken",
  "syUserSecToken", "syServerSecToken", "syAuditToken", "syUserAuditToken",
  "syServerAuditToken", "syServerContextToken", "syColon", "sySemi",
  "syComma", "syPlus", "syMinus", "syStar", "syDiv", "syLParen",
  "syRParen", "syEqual", "syCaret", "syTilde", "syLAngle", "syRAngle",
  "syLBrack", "syRBrack", "syBar", "syError", "syNumber", "sySymbolicType",
  "syIdentifier", "syString", "syQString", "syFileName", "syIPCFlag",
  "$accept", "Statements", "Statement", "Subsystem", "SubsystemStart",
  "SubsystemMods", "SubsystemMod", "SubsystemName", "SubsystemBase",
  "MsgOption", "UseSpecialReplyPort", "UserTypeLimit", "OnStackLimit",
  "WaitTime", "SendTime", "Error", "ServerPrefix", "UserPrefix",
  "ServerDemux", "Import", "ImportIndicant", "RCSDecl", "TypeDecl",
  "NamedTypeSpec", "TransTypeSpec", "TypeSpec", "NativeTypeSpec",
  "BasicTypeSpec", "PrimIPCType", "IPCType", "PrevTypeSpec",
  "VarArrayHead", "ArrayHead", "StructHead", "CStringSpec", "TypePhrase",
  "IntExp", "RoutineDecl", "Routine", "SimpleRoutine", "Arguments",
  "ArgumentList", "Argument", "Trailer", "Direction", "TrImplKeyword",
  "ArgumentType", "IPCFlags", "LookString", "LookFileName", "LookQString", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-150)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-133)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -150,    60,  -150,   -42,   -35,   -38,    10,  -150,   -29,  -150,
    -150,    12,    16,    23,    24,    46,    52,    63,  -150,  -150,
      84,  -150,    85,    86,    88,    94,    95,   113,   114,   115,
     116,   118,   119,   120,   141,   142,  -150,  -150,   117,   127,
     182,  -150,  -150,   139,   139,  -150,  -150,  -150,  -150,  -150,
     138,  -150,  -150,  -150,  -150,    -7,  -150,  -150,  -150,  -150,
    -150,  -150,  -150,  -150,  -150,  -150,  -150,  -150,  -150,  -150,
     131,   132,   133,  -150,  -150,  -150,  -150,  -150,   125,   128,
      80,  -150,  -150,   -27,  -150,  -150,  -150,  -150,   137,  -150,
    -150,  -150,  -150,  -150,  -150,  -150,  -150,  -150,  -150,  -150,
    -150,  -150,  -150,  -150,  -150,  -150,  -150,  -150,  -150,  -150,
    -150,  -150,  -150,  -150,  -150,   143,   155,   156,   140,   144,
     145,   147,   151,   158,   159,   148,   -19,   -27,  -150,  -150,
    -150,    57,  -150,  -150,  -150,   152,  -150,  -150,   -27,   -27,
     -27,  -150,  -150,  -150,  -150,   129,   129,   163,   163,   -58,
     -31,   149,   149,   149,   -30,   165,  -150,   170,   171,   172,
     173,   174,   175,   -19,  -150,  -150,  -150,  -150,  -150,   -36,
    -150,  -150,   -44,   -31,   196,  -150,    54,   103,  -150,   -50,
     -52,   -39,   177,   126,   -31,   160,   161,   162,   164,   166,
     167,  -150,   138,  -150,  -150,   176,   -31,   199,    59,  -150,
     -31,   -31,   -31,   -31,   203,   205,  -150,  -150,   149,  -150,
     -31,  -150,   107,   168,   169,   183,  -150,  -150,  -150,   178,
     130,  -150,  -150,     4,     4,  -150,  -150,  -150,  -150,   -22,
     134,   -23,   184,   185,   179,   180,   213,  -150,  -150,  -150,
     181,   186,   187,   188,  -150,   189,   190,  -150,  -150,  -150,
    -150
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,    23,     0,    35,
      37,     0,     0,     0,     0,     0,     0,     0,    20,     3,
       0,    24,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    91,    92,     0,     0,
       0,    21,    17,     0,     0,    31,    38,    39,    40,    41,
       0,    49,    32,    33,     4,     0,     7,     8,     9,    10,
       5,     6,    11,    12,    13,    14,    18,    19,    15,    16,
       0,     0,     0,    43,    44,    45,    46,    47,     0,     0,
     103,    93,    94,     0,    26,    27,    28,    25,     0,    30,
      34,    36,    42,    48,   113,   121,   111,   112,   104,   105,
     106,   123,   122,   107,   108,   109,   110,   114,   116,   115,
     117,   119,   118,   120,    95,     0,    97,    98,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    71,    72,
      75,    50,    51,    65,    58,    73,    69,    59,     0,     0,
       0,    64,    29,    22,    96,   103,   103,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    62,     0,     0,     0,
       0,     0,     0,     0,    60,    61,    63,    99,   100,     0,
     127,   102,     0,     0,     0,    89,     0,     0,    83,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    74,   124,   125,   126,   101,     0,     0,     0,    76,
       0,     0,     0,     0,     0,     0,    66,    84,     0,    68,
       0,    81,   127,     0,     0,     0,    55,    56,    57,     0,
       0,    77,    90,    85,    86,    87,    88,    79,    80,     0,
       0,     0,     0,     0,     0,   128,     0,    67,    82,    70,
       0,     0,     0,     0,    78,     0,     0,    54,   129,    52,
      53
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -150,  -150,  -150,  -150,  -150,  -150,  -150,  -150,  -150,  -150,
    -150,  -150,  -150,  -150,  -150,  -150,  -150,  -150,  -150,  -150,
    -150,  -150,  -150,    89,  -150,  -108,    93,  -150,    91,   146,
    -150,  -150,  -150,  -150,  -150,  -149,  -148,  -150,  -150,  -150,
     219,   -59,  -150,  -150,  -150,  -150,   121,    53,  -150,  -150,
    -150
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     1,    19,    20,    21,    55,    87,    88,   143,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      78,    33,    34,    51,   131,   132,   133,   134,   135,   136,
     137,   138,   139,   140,   141,   179,   176,    35,    36,    37,
      81,   115,   116,   117,   118,   119,   170,   195,    38,    39,
      40
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      84,    85,   177,   180,   181,   172,   183,   173,   208,   120,
     121,   122,   123,   124,   196,   174,   206,    41,   175,   156,
     122,   123,   124,   125,    42,   198,   207,   209,   207,   197,
     164,   165,   166,   182,   173,   173,   212,   219,   126,   207,
      43,   127,   192,   239,   237,   175,   175,    45,   220,   128,
     129,   130,   223,   224,   225,   226,   207,   128,   129,   229,
       2,     3,   230,     4,     5,     6,     7,   202,   203,  -130,
       8,    86,  -130,  -130,     9,    10,    11,    12,    13,    14,
    -132,  -131,  -131,  -131,  -131,  -131,   167,   168,    44,    94,
      46,    95,    96,    97,    47,    15,   157,   158,   159,   160,
     161,    48,    49,   162,    16,    17,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   200,   201,   202,   203,    18,
     200,   201,   202,   203,    50,   222,    70,   204,    52,    71,
      72,   107,   108,   109,   110,   111,   112,   113,    94,    53,
      95,    96,    97,    54,    56,    57,   114,    58,    73,    74,
      75,    76,    77,    59,    60,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   200,   201,   202,   203,   200,   201,
     202,   203,    61,    62,    63,    64,   205,    65,    66,    67,
     107,   108,   109,   110,   111,   112,   113,   200,   201,   202,
     203,   200,   201,   202,   203,   200,   201,   202,   203,   211,
      68,    69,    79,   236,    80,    83,    92,   238,    93,   144,
      89,    90,    91,   142,   145,   146,   151,   149,   147,   150,
     154,   169,   148,   152,   153,   184,   163,   178,   185,   186,
     187,   188,   189,   190,   199,   210,   219,   221,   213,   214,
     215,   227,   216,   228,   217,   218,   232,   233,   234,   240,
     241,   244,   243,   247,   191,   249,   250,   242,   193,   245,
     235,   248,   194,    82,   246,   231,     0,     0,     0,   171,
       0,     0,   155
};

static const yytype_int16 yycheck[] =
{
       7,     8,   150,   152,   153,    63,   154,    65,    60,    36,
      37,    47,    48,    49,    58,    73,    66,    59,    76,   127,
      47,    48,    49,    50,    59,   173,    78,    66,    78,    73,
     138,   139,   140,    63,    65,    65,   184,    60,    65,    78,
      78,    68,    78,    66,    66,    76,    76,    76,   196,    76,
      77,    78,   200,   201,   202,   203,    78,    76,    77,   208,
       0,     1,   210,     3,     4,     5,     6,    63,    64,     9,
      10,    78,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,   145,   146,    78,     9,
      78,    11,    12,    13,    78,    35,    39,    40,    41,    42,
      43,    78,    78,    46,    44,    45,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    61,    62,    63,    64,    59,
      61,    62,    63,    64,    78,    66,     9,    73,    76,    12,
      13,    51,    52,    53,    54,    55,    56,    57,     9,    76,
      11,    12,    13,    59,    59,    59,    66,    59,    21,    22,
      23,    24,    25,    59,    59,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    61,    62,    63,    64,    61,    62,
      63,    64,    59,    59,    59,    59,    73,    59,    59,    59,
      51,    52,    53,    54,    55,    56,    57,    61,    62,    63,
      64,    61,    62,    63,    64,    61,    62,    63,    64,    73,
      59,    59,    20,    73,    65,    67,    81,    73,    80,    66,
      79,    79,    79,    76,    59,    59,    65,    72,    78,    72,
      72,    58,    78,    65,    65,    60,    74,    78,    58,    58,
      58,    58,    58,    58,    38,    58,    60,    38,    78,    78,
      78,    38,    78,    38,    78,    78,    78,    78,    65,    65,
      65,    38,    72,    66,   163,    66,    66,    78,   169,    78,
      82,    73,   169,    44,    78,   212,    -1,    -1,    -1,   148,
      -1,    -1,   126
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    84,     0,     1,     3,     4,     5,     6,    10,    14,
      15,    16,    17,    18,    19,    35,    44,    45,    59,    85,
      86,    87,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   104,   105,   120,   121,   122,   131,   132,
     133,    59,    59,    78,    78,    76,    78,    78,    78,    78,
      78,   106,    76,    76,    59,    88,    59,    59,    59,    59,
      59,    59,    59,    59,    59,    59,    59,    59,    59,    59,
       9,    12,    13,    21,    22,    23,    24,    25,   103,    20,
      65,   123,   123,    67,     7,     8,    78,    89,    90,    79,
      79,    79,    81,    80,     9,    11,    12,    13,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    51,    52,    53,
      54,    55,    56,    57,    66,   124,   125,   126,   127,   128,
      36,    37,    47,    48,    49,    50,    65,    68,    76,    77,
      78,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,    76,    91,    66,    59,    59,    78,    78,    72,
      72,    65,    65,    65,    72,   112,   108,    39,    40,    41,
      42,    43,    46,    74,   108,   108,   108,   124,   124,    58,
     129,   129,    63,    65,    73,    76,   119,   119,    78,   118,
     118,   118,    63,   119,    60,    58,    58,    58,    58,    58,
      58,   111,    78,   106,   109,   130,    58,    73,   119,    38,
      61,    62,    63,    64,    73,    73,    66,    78,    60,    66,
      58,    73,   119,    78,    78,    78,    78,    78,    78,    60,
     119,    38,    66,   119,   119,   119,   119,    38,    38,   118,
     119,   130,    78,    78,    65,    82,    73,    66,    73,    66,
      65,    65,    78,    72,    38,    78,    78,    66,    73,    66,
      66
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,    83,    84,    84,    85,    85,    85,    85,    85,    85,
      85,    85,    85,    85,    85,    85,    85,    85,    85,    85,
      85,    85,    86,    87,    88,    88,    89,    89,    90,    91,
      92,    93,    94,    95,    96,    96,    97,    97,    98,    99,
     100,   101,   102,   103,   103,   103,   103,   103,   104,   105,
     106,   107,   107,   107,   107,   107,   107,   107,   108,   108,
     108,   108,   108,   108,   108,   108,   109,   109,   109,   110,
     110,   111,   111,   112,   112,   113,   114,   114,   114,   115,
     116,   117,   117,   118,   118,   119,   119,   119,   119,   119,
     119,   120,   120,   121,   122,   123,   123,   124,   124,   124,
     124,   125,   126,   127,   127,   127,   127,   127,   127,   127,
     127,   127,   127,   127,   127,   127,   127,   127,   127,   127,
     127,   127,   128,   128,   129,   129,   129,   130,   130,   130,
     131,   132,   133
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       1,     2,     4,     1,     0,     2,     1,     1,     1,     1,
       3,     2,     2,     2,     3,     1,     3,     1,     2,     2,
       2,     2,     3,     1,     1,     1,     1,     1,     3,     2,
       3,     1,     8,     8,     7,     4,     4,     4,     1,     1,
       2,     2,     2,     2,     1,     1,     4,     6,     4,     1,
       6,     1,     1,     1,     3,     1,     4,     5,     7,     5,
       5,     4,     6,     1,     2,     3,     3,     3,     3,     1,
       3,     1,     1,     3,     3,     2,     3,     1,     1,     3,
       3,     4,     3,     0,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     2,     2,     0,     3,     5,
       0,     0,     0
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
  case 16: /* Statement: RoutineDecl sySemi  */
#line 214 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
{
    register statement_t *st = stAlloc();

    st->stKind = skRoutine;
    st->stRoutine = (yyvsp[-1].routine);
    rtCheckRoutine((yyvsp[-1].routine));
    if (BeVerbose)
      rtPrintRoutine((yyvsp[-1].routine));
}
#line 1416 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 17: /* Statement: sySkip sySemi  */
#line 224 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { rtSkip(); }
#line 1422 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 21: /* Statement: error sySemi  */
#line 229 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { yyerrok; }
#line 1428 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 22: /* Subsystem: SubsystemStart SubsystemMods SubsystemName SubsystemBase  */
#line 234 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
{
  if (BeVerbose) {
    printf("Subsystem %s: base = %u%s%s\n\n",
    SubsystemName, SubsystemBase,
    IsKernelUser ? ", KernelUser" : "",
    IsKernelServer ? ", KernelServer" : "");
  }
}
#line 1441 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 23: /* SubsystemStart: sySubsystem  */
#line 245 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
{
    if (SubsystemName != strNULL) {
      warn("previous Subsystem decl (of %s) will be ignored", SubsystemName);
      IsKernelUser = FALSE;
      IsKernelServer = FALSE;
      strfree(SubsystemName);
    }
}
#line 1454 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 26: /* SubsystemMod: syKernelUser  */
#line 260 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
{
    if (IsKernelUser)
      warn("duplicate KernelUser keyword");
    if (!UseMsgRPC) {
      warn("with KernelUser the -R option is meaningless");
      UseMsgRPC = TRUE;
    }
    IsKernelUser = TRUE;
}
#line 1468 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 27: /* SubsystemMod: syKernelServer  */
#line 270 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
{
    if (IsKernelServer)
      warn("duplicate KernelServer keyword");
    IsKernelServer = TRUE;
}
#line 1478 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 28: /* SubsystemName: syIdentifier  */
#line 277 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                                { SubsystemName = (yyvsp[0].identifier); }
#line 1484 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 29: /* SubsystemBase: syNumber  */
#line 280 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                                { SubsystemBase = (yyvsp[0].number); }
#line 1490 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 30: /* MsgOption: LookString syMsgOption syString  */
#line 284 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
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
#line 1507 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 31: /* UseSpecialReplyPort: syUseSpecialReplyPort syNumber  */
#line 299 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
{
    UseSpecialReplyPort = ((yyvsp[0].number) != 0);
    HasUseSpecialReplyPort |= UseSpecialReplyPort;
}
#line 1516 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 32: /* UserTypeLimit: syUserTypeLimit syNumber  */
#line 306 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                {UserTypeLimit = (yyvsp[0].number); }
#line 1522 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 33: /* OnStackLimit: syOnStackLimit syNumber  */
#line 309 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                {MaxMessSizeOnStack = (yyvsp[0].number); }
#line 1528 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 34: /* WaitTime: LookString syWaitTime syString  */
#line 313 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
{
    WaitTime = (yyvsp[0].string);
    if (BeVerbose)
      printf("WaitTime %s\n\n", WaitTime);
}
#line 1538 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 35: /* WaitTime: syNoWaitTime  */
#line 319 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
{
    WaitTime = strNULL;
    if (BeVerbose)
      printf("NoWaitTime\n\n");
}
#line 1548 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 36: /* SendTime: LookString sySendTime syString  */
#line 327 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
{
    SendTime = (yyvsp[0].string);
    if (BeVerbose)
      printf("SendTime %s\n\n", SendTime);
}
#line 1558 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 37: /* SendTime: syNoSendTime  */
#line 333 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
{
    SendTime = strNULL;
    if (BeVerbose)
      printf("NoSendTime\n\n");
}
#line 1568 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 38: /* Error: syErrorProc syIdentifier  */
#line 341 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
{
    ErrorProc = (yyvsp[0].identifier);
    if (BeVerbose)
      printf("ErrorProc %s\n\n", ErrorProc);
}
#line 1578 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 39: /* ServerPrefix: syServerPrefix syIdentifier  */
#line 349 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
{
    ServerPrefix = (yyvsp[0].identifier);
    if (BeVerbose)
      printf("ServerPrefix %s\n\n", ServerPrefix);
}
#line 1588 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 40: /* UserPrefix: syUserPrefix syIdentifier  */
#line 357 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
{
    UserPrefix = (yyvsp[0].identifier);
    if (BeVerbose)
      printf("UserPrefix %s\n\n", UserPrefix);
}
#line 1598 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 41: /* ServerDemux: syServerDemux syIdentifier  */
#line 365 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
{
    ServerDemux = (yyvsp[0].identifier);
    if (BeVerbose)
      printf("ServerDemux %s\n\n", ServerDemux);
}
#line 1608 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 42: /* Import: LookFileName ImportIndicant syFileName  */
#line 373 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
{
    register statement_t *st = stAlloc();
    st->stKind = (yyvsp[-1].statement_kind);
    st->stFileName = (yyvsp[0].string);

    if (BeVerbose)
      printf("%s %s\n\n", import_name((yyvsp[-1].statement_kind)), (yyvsp[0].string));
}
#line 1621 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 43: /* ImportIndicant: syImport  */
#line 383 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                                { (yyval.statement_kind) = skImport; }
#line 1627 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 44: /* ImportIndicant: syUImport  */
#line 384 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                                { (yyval.statement_kind) = skUImport; }
#line 1633 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 45: /* ImportIndicant: sySImport  */
#line 385 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                                { (yyval.statement_kind) = skSImport; }
#line 1639 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 46: /* ImportIndicant: syIImport  */
#line 386 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                                { (yyval.statement_kind) = skIImport; }
#line 1645 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 47: /* ImportIndicant: syDImport  */
#line 387 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                                { (yyval.statement_kind) = skDImport; }
#line 1651 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 48: /* RCSDecl: LookQString syRCSId syQString  */
#line 391 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
{
    if (RCSId != strNULL)
      warn("previous RCS decl will be ignored");
    if (BeVerbose)
      printf("RCSId %s\n\n", (yyvsp[0].string));
    RCSId = (yyvsp[0].string);
}
#line 1663 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 49: /* TypeDecl: syType NamedTypeSpec  */
#line 401 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
{
    register identifier_t name = (yyvsp[0].type)->itName;

    if (itLookUp(name) != itNULL)
      warn("overriding previous definition of %s", name);
    itInsert(name, (yyvsp[0].type));
}
#line 1675 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 50: /* NamedTypeSpec: syIdentifier syEqual TransTypeSpec  */
#line 411 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { itTypeDecl((yyvsp[-2].identifier), (yyval.type) = (yyvsp[0].type)); }
#line 1681 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 51: /* TransTypeSpec: TypeSpec  */
#line 415 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { (yyval.type) = itResetType((yyvsp[0].type)); }
#line 1687 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 52: /* TransTypeSpec: TransTypeSpec syInTran syColon syIdentifier syIdentifier syLParen syIdentifier syRParen  */
#line 418 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
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
#line 1707 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 53: /* TransTypeSpec: TransTypeSpec syOutTran syColon syIdentifier syIdentifier syLParen syIdentifier syRParen  */
#line 435 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
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
#line 1727 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 54: /* TransTypeSpec: TransTypeSpec syDestructor syColon syIdentifier syLParen syIdentifier syRParen  */
#line 452 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
{
    (yyval.type) = (yyvsp[-6].type);

    if (((yyval.type)->itDestructor != strNULL) && !streql((yyval.type)->itDestructor, (yyvsp[-3].identifier)))
      warn("conflicting destructor functions (%s, %s)", (yyval.type)->itDestructor, (yyvsp[-3].identifier));
    (yyval.type)->itDestructor = (yyvsp[-3].identifier);

    if (((yyval.type)->itTransType != strNULL) && !streql((yyval.type)->itTransType, (yyvsp[-1].identifier)))
      warn("conflicting translation types (%s, %s)", (yyval.type)->itTransType, (yyvsp[-1].identifier));
    (yyval.type)->itTransType = (yyvsp[-1].identifier);
}
#line 1743 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 55: /* TransTypeSpec: TransTypeSpec syCType syColon syIdentifier  */
#line 464 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
{
    (yyval.type) = (yyvsp[-3].type);

    if (((yyval.type)->itUserType != strNULL) && !streql((yyval.type)->itUserType, (yyvsp[0].identifier)))
      warn("conflicting user types (%s, %s)", (yyval.type)->itUserType, (yyvsp[0].identifier));
    (yyval.type)->itUserType = (yyvsp[0].identifier);

    if (((yyval.type)->itServerType != strNULL) && !streql((yyval.type)->itServerType, (yyvsp[0].identifier)))
      warn("conflicting server types (%s, %s)", (yyval.type)->itServerType, (yyvsp[0].identifier));
    (yyval.type)->itServerType = (yyvsp[0].identifier);
}
#line 1759 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 56: /* TransTypeSpec: TransTypeSpec syCUserType syColon syIdentifier  */
#line 476 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
{
    (yyval.type) = (yyvsp[-3].type);

    if (((yyval.type)->itUserType != strNULL) && !streql((yyval.type)->itUserType, (yyvsp[0].identifier)))
      warn("conflicting user types (%s, %s)", (yyval.type)->itUserType, (yyvsp[0].identifier));
    (yyval.type)->itUserType = (yyvsp[0].identifier);
}
#line 1771 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 57: /* TransTypeSpec: TransTypeSpec syCServerType syColon syIdentifier  */
#line 485 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
{
    (yyval.type) = (yyvsp[-3].type);

    if (((yyval.type)->itServerType != strNULL) && !streql((yyval.type)->itServerType, (yyvsp[0].identifier)))
	warn("conflicting server types (%s, %s)",
	     (yyval.type)->itServerType, (yyvsp[0].identifier));
    (yyval.type)->itServerType = (yyvsp[0].identifier);
}
#line 1784 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 58: /* TypeSpec: BasicTypeSpec  */
#line 496 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { (yyval.type) = (yyvsp[0].type); }
#line 1790 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 59: /* TypeSpec: PrevTypeSpec  */
#line 498 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { (yyval.type) = (yyvsp[0].type); }
#line 1796 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 60: /* TypeSpec: VarArrayHead TypeSpec  */
#line 500 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { (yyval.type) = itVarArrayDecl((yyvsp[-1].number), (yyvsp[0].type)); }
#line 1802 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 61: /* TypeSpec: ArrayHead TypeSpec  */
#line 502 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { (yyval.type) = itArrayDecl((yyvsp[-1].number), (yyvsp[0].type)); }
#line 1808 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 62: /* TypeSpec: syCaret TypeSpec  */
#line 504 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { (yyval.type) = itPtrDecl((yyvsp[0].type)); }
#line 1814 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 63: /* TypeSpec: StructHead TypeSpec  */
#line 506 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { (yyval.type) = itStructDecl((yyvsp[-1].number), (yyvsp[0].type)); }
#line 1820 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 64: /* TypeSpec: CStringSpec  */
#line 508 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { (yyval.type) = (yyvsp[0].type); }
#line 1826 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 65: /* TypeSpec: NativeTypeSpec  */
#line 510 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { (yyval.type) = (yyvsp[0].type); }
#line 1832 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 66: /* NativeTypeSpec: syPointerTo syLParen TypePhrase syRParen  */
#line 514 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { (yyval.type) = itNativeType((yyvsp[-1].identifier), TRUE, 0); }
#line 1838 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 67: /* NativeTypeSpec: syPointerToIfNot syLParen TypePhrase syComma TypePhrase syRParen  */
#line 517 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { (yyval.type) = itNativeType((yyvsp[-3].identifier), TRUE, (yyvsp[-1].identifier)); }
#line 1844 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 68: /* NativeTypeSpec: syValueOf syLParen TypePhrase syRParen  */
#line 519 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { (yyval.type) = itNativeType((yyvsp[-1].identifier), FALSE, 0); }
#line 1850 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 69: /* BasicTypeSpec: IPCType  */
#line 523 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
{
    (yyval.type) = itShortDecl((yyvsp[0].symtype).innumber, (yyvsp[0].symtype).instr,
		     (yyvsp[0].symtype).outnumber, (yyvsp[0].symtype).outstr,
		     (yyvsp[0].symtype).size);
}
#line 1860 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 70: /* BasicTypeSpec: syLParen IPCType syComma IntExp IPCFlags syRParen  */
#line 530 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
{
    error("Long form type declarations aren't allowed any longer\n");
}
#line 1868 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 71: /* PrimIPCType: syNumber  */
#line 536 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
{
    (yyval.symtype).innumber = (yyval.symtype).outnumber = (yyvsp[0].number);
    (yyval.symtype).instr = (yyval.symtype).outstr = strNULL;
    (yyval.symtype).size = 0;
}
#line 1878 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 72: /* PrimIPCType: sySymbolicType  */
#line 542 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { (yyval.symtype) = (yyvsp[0].symtype); }
#line 1884 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 73: /* IPCType: PrimIPCType  */
#line 546 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { (yyval.symtype) = (yyvsp[0].symtype); }
#line 1890 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 74: /* IPCType: PrimIPCType syBar PrimIPCType  */
#line 548 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
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
#line 1914 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 75: /* PrevTypeSpec: syIdentifier  */
#line 570 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { (yyval.type) = itPrevDecl((yyvsp[0].identifier)); }
#line 1920 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 76: /* VarArrayHead: syArray syLBrack syRBrack syOf  */
#line 574 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { (yyval.number) = 0; }
#line 1926 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 77: /* VarArrayHead: syArray syLBrack syStar syRBrack syOf  */
#line 576 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { (yyval.number) = 0; }
#line 1932 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 78: /* VarArrayHead: syArray syLBrack syStar syColon IntExp syRBrack syOf  */
#line 579 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { (yyval.number) = (yyvsp[-2].number); }
#line 1938 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 79: /* ArrayHead: syArray syLBrack IntExp syRBrack syOf  */
#line 583 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { (yyval.number) = (yyvsp[-2].number); }
#line 1944 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 80: /* StructHead: syStruct syLBrack IntExp syRBrack syOf  */
#line 587 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { (yyval.number) = (yyvsp[-2].number); }
#line 1950 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 81: /* CStringSpec: syCString syLBrack IntExp syRBrack  */
#line 591 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { (yyval.type) = itCStringDecl((yyvsp[-1].number), FALSE); }
#line 1956 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 82: /* CStringSpec: syCString syLBrack syStar syColon IntExp syRBrack  */
#line 594 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { (yyval.type) = itCStringDecl((yyvsp[-1].number), TRUE); }
#line 1962 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 83: /* TypePhrase: syIdentifier  */
#line 598 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { (yyval.identifier) = (yyvsp[0].identifier); }
#line 1968 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 84: /* TypePhrase: TypePhrase syIdentifier  */
#line 600 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { (yyval.identifier) = strphrase((yyvsp[-1].identifier), (yyvsp[0].identifier)); strfree((yyvsp[0].identifier)); }
#line 1974 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 85: /* IntExp: IntExp syPlus IntExp  */
#line 604 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { (yyval.number) = (yyvsp[-2].number) + (yyvsp[0].number);	}
#line 1980 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 86: /* IntExp: IntExp syMinus IntExp  */
#line 606 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { (yyval.number) = (yyvsp[-2].number) - (yyvsp[0].number);	}
#line 1986 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 87: /* IntExp: IntExp syStar IntExp  */
#line 608 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { (yyval.number) = (yyvsp[-2].number) * (yyvsp[0].number);	}
#line 1992 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 88: /* IntExp: IntExp syDiv IntExp  */
#line 610 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { (yyval.number) = (yyvsp[-2].number) / (yyvsp[0].number);	}
#line 1998 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 89: /* IntExp: syNumber  */
#line 612 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { (yyval.number) = (yyvsp[0].number);	}
#line 2004 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 90: /* IntExp: syLParen IntExp syRParen  */
#line 614 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { (yyval.number) = (yyvsp[-1].number);	}
#line 2010 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 91: /* RoutineDecl: Routine  */
#line 618 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                                        { (yyval.routine) = (yyvsp[0].routine); }
#line 2016 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 92: /* RoutineDecl: SimpleRoutine  */
#line 619 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                                        { (yyval.routine) = (yyvsp[0].routine); }
#line 2022 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 93: /* Routine: syRoutine syIdentifier Arguments  */
#line 623 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { (yyval.routine) = rtMakeRoutine((yyvsp[-1].identifier), (yyvsp[0].argument)); }
#line 2028 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 94: /* SimpleRoutine: sySimpleRoutine syIdentifier Arguments  */
#line 627 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { (yyval.routine) = rtMakeSimpleRoutine((yyvsp[-1].identifier), (yyvsp[0].argument)); }
#line 2034 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 95: /* Arguments: syLParen syRParen  */
#line 631 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { (yyval.argument) = argNULL; }
#line 2040 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 96: /* Arguments: syLParen ArgumentList syRParen  */
#line 633 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { (yyval.argument) = (yyvsp[-1].argument); }
#line 2046 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 97: /* ArgumentList: Argument  */
#line 638 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { (yyval.argument) = (yyvsp[0].argument); }
#line 2052 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 98: /* ArgumentList: Trailer  */
#line 640 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { (yyval.argument) = (yyvsp[0].argument); }
#line 2058 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 99: /* ArgumentList: Argument sySemi ArgumentList  */
#line 642 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
{
  (yyval.argument) = (yyvsp[-2].argument);
  (yyval.argument)->argNext = (yyvsp[0].argument);
}
#line 2067 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 100: /* ArgumentList: Trailer sySemi ArgumentList  */
#line 647 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
{
  (yyval.argument) = (yyvsp[-2].argument);
  (yyval.argument)->argNext = (yyvsp[0].argument);
}
#line 2076 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 101: /* Argument: Direction syIdentifier ArgumentType IPCFlags  */
#line 654 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
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
#line 2098 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 102: /* Trailer: TrImplKeyword syIdentifier ArgumentType  */
#line 674 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
{
    (yyval.argument) = argAlloc();
    (yyval.argument)->argKind = (yyvsp[-2].direction);
    (yyval.argument)->argName = (yyvsp[-1].identifier);
    (yyval.argument)->argType = (yyvsp[0].type);
}
#line 2109 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 103: /* Direction: %empty  */
#line 683 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                                { (yyval.direction) = akNone; }
#line 2115 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 104: /* Direction: syIn  */
#line 684 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                                { (yyval.direction) = akIn; }
#line 2121 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 105: /* Direction: syOut  */
#line 685 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                                { (yyval.direction) = akOut; }
#line 2127 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 106: /* Direction: syInOut  */
#line 686 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                                { (yyval.direction) = akInOut; }
#line 2133 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 107: /* Direction: syRequestPort  */
#line 687 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                                { (yyval.direction) = akRequestPort; }
#line 2139 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 108: /* Direction: syReplyPort  */
#line 688 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                                { (yyval.direction) = akReplyPort; }
#line 2145 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 109: /* Direction: sySReplyPort  */
#line 689 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                                { (yyval.direction) = akSReplyPort; }
#line 2151 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 110: /* Direction: syUReplyPort  */
#line 690 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                                { (yyval.direction) = akUReplyPort; }
#line 2157 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 111: /* Direction: syWaitTime  */
#line 691 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                                { (yyval.direction) = akWaitTime; }
#line 2163 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 112: /* Direction: sySendTime  */
#line 692 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                                { (yyval.direction) = akSendTime; }
#line 2169 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 113: /* Direction: syMsgOption  */
#line 693 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                                { (yyval.direction) = akMsgOption; }
#line 2175 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 114: /* Direction: sySecToken  */
#line 694 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                                        { (yyval.direction) = akSecToken; }
#line 2181 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 115: /* Direction: syServerSecToken  */
#line 695 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                                        { (yyval.direction) = akServerSecToken; }
#line 2187 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 116: /* Direction: syUserSecToken  */
#line 696 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                                        { (yyval.direction) = akUserSecToken; }
#line 2193 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 117: /* Direction: syAuditToken  */
#line 697 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                                        { (yyval.direction) = akAuditToken; }
#line 2199 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 118: /* Direction: syServerAuditToken  */
#line 698 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                                        { (yyval.direction) = akServerAuditToken; }
#line 2205 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 119: /* Direction: syUserAuditToken  */
#line 699 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                                        { (yyval.direction) = akUserAuditToken; }
#line 2211 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 120: /* Direction: syServerContextToken  */
#line 700 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                                        { (yyval.direction) = akServerContextToken; }
#line 2217 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 121: /* Direction: syMsgSeqno  */
#line 701 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                                { (yyval.direction) = akMsgSeqno; }
#line 2223 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 122: /* TrImplKeyword: syServerImpl  */
#line 706 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                                { (yyval.direction) = akServerImpl; }
#line 2229 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 123: /* TrImplKeyword: syUserImpl  */
#line 707 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                                { (yyval.direction) = akUserImpl; }
#line 2235 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 124: /* ArgumentType: syColon syIdentifier  */
#line 712 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
{
  (yyval.type) = itLookUp((yyvsp[0].identifier));
  if ((yyval.type) == itNULL)
    error("type '%s' not defined", (yyvsp[0].identifier));
}
#line 2245 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 125: /* ArgumentType: syColon NamedTypeSpec  */
#line 718 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { (yyval.type) = (yyvsp[0].type); }
#line 2251 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 126: /* ArgumentType: syColon NativeTypeSpec  */
#line 720 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { (yyval.type) = (yyvsp[0].type); }
#line 2257 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 127: /* IPCFlags: %empty  */
#line 724 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { (yyval.flag) = flNone; }
#line 2263 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 128: /* IPCFlags: IPCFlags syComma syIPCFlag  */
#line 726 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
{
  if ((yyvsp[-2].flag) & (yyvsp[0].flag))
    warn("redundant IPC flag ignored");
  else
    (yyval.flag) = (yyvsp[-2].flag) | (yyvsp[0].flag);
}
#line 2274 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 129: /* IPCFlags: IPCFlags syComma syIPCFlag syLBrack syRBrack  */
#line 733 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
{
  if ((yyvsp[-2].flag) != flDealloc)
    warn("only Dealloc is variable");
  else
    (yyval.flag) = (yyvsp[-4].flag) | flMaybeDealloc;
}
#line 2285 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 130: /* LookString: %empty  */
#line 741 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { LookString(); }
#line 2291 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 131: /* LookFileName: %empty  */
#line 745 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { LookFileName(); }
#line 2297 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;

  case 132: /* LookQString: %empty  */
#line 749 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
                                { LookQString(); }
#line 2303 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    break;


#line 2307 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"

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

#line 752 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"


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
