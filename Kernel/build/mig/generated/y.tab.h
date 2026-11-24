/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_VERCEL_SANDBOX_RAVYNOS_KERNEL_BUILD_MIG_GENERATED_Y_TAB_H_INCLUDED
# define YY_YY_VERCEL_SANDBOX_RAVYNOS_KERNEL_BUILD_MIG_GENERATED_Y_TAB_H_INCLUDED
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
    syMsgSeqno = 266,              /* syMsgSeqno  */
    syWaitTime = 267,              /* syWaitTime  */
    sySendTime = 268,              /* sySendTime  */
    syNoWaitTime = 269,            /* syNoWaitTime  */
    syNoSendTime = 270,            /* syNoSendTime  */
    syErrorProc = 271,             /* syErrorProc  */
    syServerPrefix = 272,          /* syServerPrefix  */
    syUserPrefix = 273,            /* syUserPrefix  */
    syServerDemux = 274,           /* syServerDemux  */
    syRCSId = 275,                 /* syRCSId  */
    syImport = 276,                /* syImport  */
    syUImport = 277,               /* syUImport  */
    sySImport = 278,               /* sySImport  */
    syIImport = 279,               /* syIImport  */
    syDImport = 280,               /* syDImport  */
    syIn = 281,                    /* syIn  */
    syOut = 282,                   /* syOut  */
    syInOut = 283,                 /* syInOut  */
    syUserImpl = 284,              /* syUserImpl  */
    syServerImpl = 285,            /* syServerImpl  */
    syRequestPort = 286,           /* syRequestPort  */
    syReplyPort = 287,             /* syReplyPort  */
    sySReplyPort = 288,            /* sySReplyPort  */
    syUReplyPort = 289,            /* syUReplyPort  */
    syType = 290,                  /* syType  */
    syArray = 291,                 /* syArray  */
    syStruct = 292,                /* syStruct  */
    syOf = 293,                    /* syOf  */
    syInTran = 294,                /* syInTran  */
    syOutTran = 295,               /* syOutTran  */
    syDestructor = 296,            /* syDestructor  */
    syCType = 297,                 /* syCType  */
    syCUserType = 298,             /* syCUserType  */
    syUserTypeLimit = 299,         /* syUserTypeLimit  */
    syOnStackLimit = 300,          /* syOnStackLimit  */
    syCServerType = 301,           /* syCServerType  */
    syPointerTo = 302,             /* syPointerTo  */
    syPointerToIfNot = 303,        /* syPointerToIfNot  */
    syValueOf = 304,               /* syValueOf  */
    syCString = 305,               /* syCString  */
    sySecToken = 306,              /* sySecToken  */
    syUserSecToken = 307,          /* syUserSecToken  */
    syServerSecToken = 308,        /* syServerSecToken  */
    syAuditToken = 309,            /* syAuditToken  */
    syUserAuditToken = 310,        /* syUserAuditToken  */
    syServerAuditToken = 311,      /* syServerAuditToken  */
    syServerContextToken = 312,    /* syServerContextToken  */
    syColon = 313,                 /* syColon  */
    sySemi = 314,                  /* sySemi  */
    syComma = 315,                 /* syComma  */
    syPlus = 316,                  /* syPlus  */
    syMinus = 317,                 /* syMinus  */
    syStar = 318,                  /* syStar  */
    syDiv = 319,                   /* syDiv  */
    syLParen = 320,                /* syLParen  */
    syRParen = 321,                /* syRParen  */
    syEqual = 322,                 /* syEqual  */
    syCaret = 323,                 /* syCaret  */
    syTilde = 324,                 /* syTilde  */
    syLAngle = 325,                /* syLAngle  */
    syRAngle = 326,                /* syRAngle  */
    syLBrack = 327,                /* syLBrack  */
    syRBrack = 328,                /* syRBrack  */
    syBar = 329,                   /* syBar  */
    syError = 330,                 /* syError  */
    syNumber = 331,                /* syNumber  */
    sySymbolicType = 332,          /* sySymbolicType  */
    syIdentifier = 333,            /* syIdentifier  */
    syString = 334,                /* syString  */
    syQString = 335,               /* syQString  */
    syFileName = 336,              /* syFileName  */
    syIPCFlag = 337                /* syIPCFlag  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
#define sySkip 258
#define syRoutine 259
#define sySimpleRoutine 260
#define sySubsystem 261
#define syKernelUser 262
#define syKernelServer 263
#define syMsgOption 264
#define syUseSpecialReplyPort 265
#define syMsgSeqno 266
#define syWaitTime 267
#define sySendTime 268
#define syNoWaitTime 269
#define syNoSendTime 270
#define syErrorProc 271
#define syServerPrefix 272
#define syUserPrefix 273
#define syServerDemux 274
#define syRCSId 275
#define syImport 276
#define syUImport 277
#define sySImport 278
#define syIImport 279
#define syDImport 280
#define syIn 281
#define syOut 282
#define syInOut 283
#define syUserImpl 284
#define syServerImpl 285
#define syRequestPort 286
#define syReplyPort 287
#define sySReplyPort 288
#define syUReplyPort 289
#define syType 290
#define syArray 291
#define syStruct 292
#define syOf 293
#define syInTran 294
#define syOutTran 295
#define syDestructor 296
#define syCType 297
#define syCUserType 298
#define syUserTypeLimit 299
#define syOnStackLimit 300
#define syCServerType 301
#define syPointerTo 302
#define syPointerToIfNot 303
#define syValueOf 304
#define syCString 305
#define sySecToken 306
#define syUserSecToken 307
#define syServerSecToken 308
#define syAuditToken 309
#define syUserAuditToken 310
#define syServerAuditToken 311
#define syServerContextToken 312
#define syColon 313
#define sySemi 314
#define syComma 315
#define syPlus 316
#define syMinus 317
#define syStar 318
#define syDiv 319
#define syLParen 320
#define syRParen 321
#define syEqual 322
#define syCaret 323
#define syTilde 324
#define syLAngle 325
#define syRAngle 326
#define syLBrack 327
#define syRBrack 328
#define syBar 329
#define syError 330
#define syNumber 331
#define sySymbolicType 332
#define syIdentifier 333
#define syString 334
#define syQString 335
#define syFileName 336
#define syIPCFlag 337

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 175 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"

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

#line 251 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_VERCEL_SANDBOX_RAVYNOS_KERNEL_BUILD_MIG_GENERATED_Y_TAB_H_INCLUDED  */
