/* original parser id follows */
/* yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93" */
/* (use YYMAJOR/YYMINOR for ifdefs dependent on parser version) */

#define YYBYACC 1
#define YYMAJOR 2
#define YYMINOR 0
#define YYPATCH 20241231

#define YYEMPTY        (-1)
#define yyclearin      (yychar = YYEMPTY)
#define yyerrok        (yyerrflag = 0)
#define YYRECOVERING() (yyerrflag != 0)
#define YYENOMEM       (-2)
#define YYEOF          0
#undef YYBTYACC
#define YYBTYACC 0
#define YYDEBUGSTR YYPREFIX "debug"
#define YYPREFIX "yy"

#define YYPURE 0

#line 156 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"

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

#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#line 174 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
typedef union YYSTYPE
{
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
} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
#line 69 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"

/* compatibility with bison */
#ifdef YYPARSE_PARAM
/* compatibility with FreeBSD */
# ifdef YYPARSE_PARAM_TYPE
#  define YYPARSE_DECL() yyparse(YYPARSE_PARAM_TYPE YYPARSE_PARAM)
# else
#  define YYPARSE_DECL() yyparse(void *YYPARSE_PARAM)
# endif
#else
# define YYPARSE_DECL() yyparse(void)
#endif

/* Parameters sent to lex. */
#ifdef YYLEX_PARAM
# define YYLEX_DECL() yylex(void *YYLEX_PARAM)
# define YYLEX yylex(YYLEX_PARAM)
#else
# define YYLEX_DECL() yylex(void)
# define YYLEX yylex()
#endif

#if !(defined(yylex) || defined(YYSTATE))
int YYLEX_DECL();
#endif

/* Parameters sent to yyerror. */
#ifndef YYERROR_DECL
#define YYERROR_DECL() yyerror(const char *s)
#endif
#ifndef YYERROR_CALL
#define YYERROR_CALL(msg) yyerror(msg)
#endif

extern int YYPARSE_DECL();

#define sySkip 257
#define syRoutine 258
#define sySimpleRoutine 259
#define sySubsystem 260
#define syKernelUser 261
#define syKernelServer 262
#define syMsgOption 263
#define syUseSpecialReplyPort 264
#define syMsgSeqno 265
#define syWaitTime 266
#define sySendTime 267
#define syNoWaitTime 268
#define syNoSendTime 269
#define syErrorProc 270
#define syServerPrefix 271
#define syUserPrefix 272
#define syServerDemux 273
#define syRCSId 274
#define syImport 275
#define syUImport 276
#define sySImport 277
#define syIImport 278
#define syDImport 279
#define syIn 280
#define syOut 281
#define syInOut 282
#define syUserImpl 283
#define syServerImpl 284
#define syRequestPort 285
#define syReplyPort 286
#define sySReplyPort 287
#define syUReplyPort 288
#define syType 289
#define syArray 290
#define syStruct 291
#define syOf 292
#define syInTran 293
#define syOutTran 294
#define syDestructor 295
#define syCType 296
#define syCUserType 297
#define syUserTypeLimit 298
#define syOnStackLimit 299
#define syCServerType 300
#define syPointerTo 301
#define syPointerToIfNot 302
#define syValueOf 303
#define syCString 304
#define sySecToken 305
#define syUserSecToken 306
#define syServerSecToken 307
#define syAuditToken 308
#define syUserAuditToken 309
#define syServerAuditToken 310
#define syServerContextToken 311
#define syColon 312
#define sySemi 313
#define syComma 314
#define syPlus 315
#define syMinus 316
#define syStar 317
#define syDiv 318
#define syLParen 319
#define syRParen 320
#define syEqual 321
#define syCaret 322
#define syTilde 323
#define syLAngle 324
#define syRAngle 325
#define syLBrack 326
#define syRBrack 327
#define syBar 328
#define syError 329
#define syNumber 330
#define sySymbolicType 331
#define syIdentifier 332
#define syString 333
#define syQString 334
#define syFileName 335
#define syIPCFlag 336
#define YYERRCODE 256
typedef int YYINT;
static const YYINT yylhs[] = {                           -1,
    0,    0,   27,   27,   27,   27,   27,   27,   27,   27,
   27,   27,   27,   27,   27,   27,   27,   27,   27,   27,
   28,   42,   43,   43,   46,   46,   44,   45,   31,   32,
   33,   34,   29,   29,   30,   30,   35,   36,   37,   38,
   40,    1,    1,    1,    1,    1,   41,   39,    6,    7,
    7,    7,    7,    7,    7,    7,    9,    9,    9,    9,
    9,    9,    9,    9,    8,    8,    8,   11,   11,   15,
   15,   16,   16,   12,    2,    2,    2,    3,    4,   10,
   10,   14,   14,    5,    5,    5,    5,    5,    5,   17,
   17,   18,   19,   24,   24,   25,   25,   25,   25,   22,
   23,   20,   20,   20,   20,   20,   20,   20,   20,   20,
   20,   20,   20,   20,   20,   20,   20,   20,   20,   20,
   21,   21,   13,   13,   13,   26,   26,   26,   47,   48,
   49,
};
static const YYINT yylen[] = {                            2,
    0,    2,    2,    2,    2,    2,    2,    2,    2,    2,
    2,    2,    2,    2,    2,    2,    2,    2,    1,    2,
    4,    1,    0,    2,    1,    1,    1,    1,    3,    2,
    2,    2,    3,    1,    3,    1,    2,    2,    2,    2,
    3,    1,    1,    1,    1,    1,    3,    2,    3,    1,
    8,    8,    7,    4,    4,    4,    1,    1,    2,    2,
    2,    2,    1,    1,    4,    6,    4,    1,    6,    1,
    1,    1,    3,    1,    4,    5,    7,    5,    5,    4,
    6,    1,    2,    3,    3,    3,    3,    1,    3,    1,
    1,    3,    3,    2,    3,    1,    1,    3,    3,    4,
    3,    0,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    2,    2,    2,    0,    3,    5,    0,    0,
    0,
};
static const YYINT yydefred[] = {                         1,
    0,    0,    0,    0,    0,   22,    0,   34,   36,    0,
    0,    0,    0,    0,    0,    0,   19,    0,   90,   91,
    2,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   23,    0,    0,    0,   20,
   16,    0,    0,   30,   37,   38,   39,   40,    0,   48,
   31,   32,   15,    3,    4,    5,    6,    7,    8,    9,
   10,   11,   12,   13,   14,   17,   18,    0,    0,    0,
    0,   42,   43,   44,   45,   46,    0,    0,    0,   92,
   93,    0,   25,   26,   27,    0,   24,   29,   33,   35,
   41,   47,  112,  120,  110,  111,  103,  104,  105,  122,
  121,  106,  107,  108,  109,  113,  115,  114,  116,  118,
  117,  119,   94,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   70,   71,   74,    0,
    0,    0,    0,   64,   50,   63,   57,   58,    0,   68,
   28,   21,    0,    0,    0,    0,   95,    0,    0,    0,
    0,    0,    0,    0,   61,   59,   60,   62,    0,    0,
    0,    0,    0,    0,    0,    0,  126,  101,   98,   99,
    0,    0,    0,   88,    0,    0,   82,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   73,
    0,  124,  125,    0,    0,    0,    0,   75,    0,    0,
    0,    0,    0,    0,   65,   83,    0,   67,    0,   80,
    0,    0,    0,    0,   54,   55,   56,    0,    0,   76,
   89,    0,    0,   86,   87,   78,   79,    0,    0,    0,
    0,    0,    0,    0,    0,   66,   81,   69,    0,    0,
    0,    0,   77,    0,    0,   53,  128,   51,   52,
};
#if defined(YYDESTRUCT_CALL) || defined(YYSTYPE_TOSTRING)
static const YYINT yystos[] = {                           0,
  338,  256,  257,  258,  259,  260,  264,  268,  269,  270,
  271,  272,  273,  289,  298,  299,  313,  355,  356,  357,
  365,  366,  367,  368,  369,  370,  371,  372,  373,  374,
  375,  376,  377,  378,  379,  380,  385,  386,  387,  313,
  313,  332,  332,  330,  332,  332,  332,  332,  332,  344,
  330,  330,  313,  313,  313,  313,  313,  313,  313,  313,
  313,  313,  313,  313,  313,  313,  313,  381,  263,  266,
  267,  275,  276,  277,  278,  279,  339,  274,  319,  362,
  362,  321,  261,  262,  332,  382,  384,  333,  333,  333,
  335,  334,  263,  265,  266,  267,  280,  281,  282,  283,
  284,  285,  286,  287,  288,  305,  306,  307,  308,  309,
  310,  311,  320,  358,  359,  360,  361,  363,  290,  291,
  301,  302,  303,  304,  319,  322,  330,  331,  332,  340,
  341,  342,  345,  346,  347,  348,  349,  350,  353,  354,
  330,  383,  332,  332,  313,  313,  320,  326,  326,  319,
  319,  319,  326,  354,  347,  347,  347,  347,  293,  294,
  295,  296,  297,  300,  328,  312,  351,  351,  363,  363,
  317,  319,  327,  330,  343,  343,  332,  352,  352,  352,
  317,  343,  314,  312,  312,  312,  312,  312,  312,  353,
  332,  344,  346,  364,  312,  327,  343,  292,  315,  316,
  317,  318,  327,  327,  320,  332,  314,  320,  312,  327,
  343,  332,  332,  332,  332,  332,  332,  314,  343,  292,
  320,  343,  343,  343,  343,  292,  292,  352,  343,  364,
  332,  332,  319,  336,  327,  320,  327,  320,  319,  319,
  332,  326,  292,  332,  332,  320,  327,  320,  320,
};
#endif /* YYDESTRUCT_CALL || YYSTYPE_TOSTRING */
static const YYINT yydgoto[] = {                          1,
   77,  130,  131,  132,  175,   50,  133,  134,  135,  136,
  137,  138,  167,  178,  139,  140,   18,   19,   20,  114,
  115,  116,  117,   80,  118,  194,   21,   22,   23,   24,
   25,   26,   27,   28,   29,   30,   31,   32,   33,   34,
   35,   36,   68,   86,  142,   87,   37,   38,   39,
};
static const YYINT yysindex[] = {                         0,
 -249, -295, -287, -285, -281,    0, -263,    0,    0, -262,
 -245, -243, -236, -216, -212, -211,    0, -193,    0,    0,
    0, -177, -175, -157, -154, -152, -151, -149, -125, -124,
 -108, -107, -105, -104,  -78,    0, -126,  -49, -119,    0,
    0,  -83,  -83,    0,    0,    0,    0,    0,  -82,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0, -261,  -93,  -92,
  -91,    0,    0,    0,    0,    0,  -98,  -90, -207,    0,
    0, -159,    0,    0,    0,  -87,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  -94,  -86,  -68,  -66,  -72,  -77,  -76,
  -67,  -65,  -64,  -75, -288, -159,    0,    0,    0, -159,
 -159, -159,  -81,    0,    0,    0,    0,    0,  -71,    0,
    0,    0,  -59,  -59, -158, -158,    0, -289, -258,  -74,
  -74,  -74, -284,  -58,    0,    0,    0,    0,  -53,  -52,
  -51,  -50,  -48,  -47, -288, -298,    0,    0,    0,    0,
 -296, -258,  -29,    0, -141, -137,    0, -303, -300, -293,
  -46, -133, -258,  -63,  -62,  -61,  -57,  -56,  -55,    0,
  -82,    0,    0,  -42, -258,  -25,  -95,    0, -258, -258,
 -258, -258,  -24,  -19,    0,    0,  -74,    0, -258,    0,
  -84,  -54,  -45,  -40,    0,    0,    0,  -44, -120,    0,
    0, -252, -252,    0,    0,    0,    0, -275, -116, -251,
  -39,  -38,  -43,  -41,  -18,    0,    0,    0,  -37,  -36,
  -34,  -33,    0,  -32,  -30,    0,    0,    0,    0,
};
static const YYINT yyrindex[] = {                         0,
 -109,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  -35,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  -27,  -22,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0, -199,    0,    0,    0,    0,    0, -203,    0,
    0,    0,    0,    0,  -35,  -35,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
 -110,    0,    0, -276,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
 -208,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0, -232, -181,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  -96,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,
};
#if YYBTYACC
static const YYINT yycindex[] = {                         0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,
};
#endif
static const YYINT yygindex[] = {                         0,
    0,    0,    0,    0, -147,  116,    0,  117,   61,    0,
    0,    0,  140, -139,  126,  174,    0,    0,    0,    0,
    0,    0,    0,  257,  -60,   90,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,
};
#define YYTABLESIZE 301
static const YYINT yytable[] = {                         83,
   84,  176,  121,  122,  123,  182,    2,    3,    4,    5,
    6,  179,  180,  207,    7,  195,  205,   40,    8,    9,
   10,   11,   12,   13,  197,   41,  208,  171,  206,  172,
  196,  206,  181,  191,  172,  211,  100,  173,  206,   14,
  174,  127,  128,  100,  236,  174,   42,  219,   15,   16,
   43,  222,  223,  224,  225,   93,  206,   94,   95,   96,
  172,  229,  218,   17,  201,  202,   44,  228,  238,   45,
   85,  174,   97,   98,   99,  100,  101,  102,  103,  104,
  105,   84,   84,   84,  169,  170,   46,   84,   47,   72,
   72,   72,   72,   72,   84,   48,   72,  106,  107,  108,
  109,  110,  111,  112,   93,  126,   94,   95,   96,   72,
   72,  126,  113,   49,   49,   49,   72,   51,   52,   53,
   49,   97,   98,   99,  100,  101,  102,  103,  104,  105,
  119,  120,   85,   85,   85,   54,   69,   55,   85,   70,
   71,  121,  122,  123,  124,   85,  106,  107,  108,  109,
  110,  111,  112,  129,   78,   56,  129,  129,   57,  125,
   58,   59,  126,   60,  131,  130,  130,  130,  130,  130,
  127,  128,  129,  199,  200,  201,  202,  199,  200,  201,
  202,  199,  200,  201,  202,  203,  155,   61,   62,  204,
  156,  157,  158,  210,  199,  200,  201,  202,  199,  200,
  201,  202,  123,  123,   63,   64,  235,   65,   66,  123,
  237,  159,  160,  161,  162,  163,  127,  127,  164,  199,
  200,  201,  202,  127,  221,   72,   73,   74,   75,   76,
  199,  200,  201,  202,   67,   79,   91,  143,   82,   88,
   89,   90,  141,   92,  145,  144,  146,  147,  148,  149,
  153,  150,  166,  151,  152,  183,  165,  177,  184,  185,
  186,  187,  198,  188,  189,  209,  220,  226,  212,  213,
  214,  218,  227,  243,  215,  216,  217,  231,  233,  239,
  240,  192,  193,  168,  242,  246,  232,  248,  241,  249,
  190,  234,   96,  247,  244,  245,  102,   97,  154,   81,
  230,
};
static const YYINT yycheck[] = {                        261,
  262,  149,  301,  302,  303,  153,  256,  257,  258,  259,
  260,  151,  152,  314,  264,  312,  320,  313,  268,  269,
  270,  271,  272,  273,  172,  313,  320,  317,  332,  319,
  327,  332,  317,  332,  319,  183,  313,  327,  332,  289,
  330,  330,  331,  320,  320,  330,  332,  195,  298,  299,
  332,  199,  200,  201,  202,  263,  332,  265,  266,  267,
  319,  209,  314,  313,  317,  318,  330,  207,  320,  332,
  332,  330,  280,  281,  282,  283,  284,  285,  286,  287,
  288,  314,  315,  316,  145,  146,  332,  320,  332,  293,
  294,  295,  296,  297,  327,  332,  300,  305,  306,  307,
  308,  309,  310,  311,  263,  314,  265,  266,  267,  313,
  314,  320,  320,  313,  314,  332,  320,  330,  330,  313,
  320,  280,  281,  282,  283,  284,  285,  286,  287,  288,
  290,  291,  314,  315,  316,  313,  263,  313,  320,  266,
  267,  301,  302,  303,  304,  327,  305,  306,  307,  308,
  309,  310,  311,  263,  274,  313,  266,  267,  313,  319,
  313,  313,  322,  313,  274,  275,  276,  277,  278,  279,
  330,  331,  332,  315,  316,  317,  318,  315,  316,  317,
  318,  315,  316,  317,  318,  327,  126,  313,  313,  327,
  130,  131,  132,  327,  315,  316,  317,  318,  315,  316,
  317,  318,  313,  314,  313,  313,  327,  313,  313,  320,
  327,  293,  294,  295,  296,  297,  313,  314,  300,  315,
  316,  317,  318,  320,  320,  275,  276,  277,  278,  279,
  315,  316,  317,  318,  313,  319,  335,  332,  321,  333,
  333,  333,  330,  334,  313,  332,  313,  320,  326,  326,
  326,  319,  312,  319,  319,  314,  328,  332,  312,  312,
  312,  312,  292,  312,  312,  312,  292,  292,  332,  332,
  332,  314,  292,  292,  332,  332,  332,  332,  319,  319,
  319,  166,  166,  144,  326,  320,  332,  320,  332,  320,
  165,  336,  320,  327,  332,  332,  332,  320,  125,   43,
  211,
};
#if YYBTYACC
static const YYINT yyctable[] = {                        -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
};
#endif
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 336
#define YYUNDFTOKEN 388
#define YYTRANSLATE(a) ((a) > YYMAXTOKEN ? YYUNDFTOKEN : (a))
#if YYDEBUG
#ifndef NULL
#define NULL (void*)0
#endif
static const char *const yyname[] = {

"$end",NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,"error","sySkip","syRoutine","sySimpleRoutine","sySubsystem",
"syKernelUser","syKernelServer","syMsgOption","syUseSpecialReplyPort",
"syMsgSeqno","syWaitTime","sySendTime","syNoWaitTime","syNoSendTime",
"syErrorProc","syServerPrefix","syUserPrefix","syServerDemux","syRCSId",
"syImport","syUImport","sySImport","syIImport","syDImport","syIn","syOut",
"syInOut","syUserImpl","syServerImpl","syRequestPort","syReplyPort",
"sySReplyPort","syUReplyPort","syType","syArray","syStruct","syOf","syInTran",
"syOutTran","syDestructor","syCType","syCUserType","syUserTypeLimit",
"syOnStackLimit","syCServerType","syPointerTo","syPointerToIfNot","syValueOf",
"syCString","sySecToken","syUserSecToken","syServerSecToken","syAuditToken",
"syUserAuditToken","syServerAuditToken","syServerContextToken","syColon",
"sySemi","syComma","syPlus","syMinus","syStar","syDiv","syLParen","syRParen",
"syEqual","syCaret","syTilde","syLAngle","syRAngle","syLBrack","syRBrack",
"syBar","syError","syNumber","sySymbolicType","syIdentifier","syString",
"syQString","syFileName","syIPCFlag","$accept","Statements","ImportIndicant",
"VarArrayHead","ArrayHead","StructHead","IntExp","NamedTypeSpec",
"TransTypeSpec","NativeTypeSpec","TypeSpec","CStringSpec","BasicTypeSpec",
"PrevTypeSpec","ArgumentType","TypePhrase","PrimIPCType","IPCType",
"RoutineDecl","Routine","SimpleRoutine","Direction","TrImplKeyword","Argument",
"Trailer","Arguments","ArgumentList","IPCFlags","Statement","Subsystem",
"WaitTime","SendTime","MsgOption","UseSpecialReplyPort","UserTypeLimit",
"OnStackLimit","Error","ServerPrefix","UserPrefix","ServerDemux","TypeDecl",
"Import","RCSDecl","SubsystemStart","SubsystemMods","SubsystemName",
"SubsystemBase","SubsystemMod","LookString","LookFileName","LookQString",
"illegal-symbol",
};
static const char *const yyrule[] = {
"$accept : Statements",
"Statements :",
"Statements : Statements Statement",
"Statement : Subsystem sySemi",
"Statement : WaitTime sySemi",
"Statement : SendTime sySemi",
"Statement : MsgOption sySemi",
"Statement : UseSpecialReplyPort sySemi",
"Statement : UserTypeLimit sySemi",
"Statement : OnStackLimit sySemi",
"Statement : Error sySemi",
"Statement : ServerPrefix sySemi",
"Statement : UserPrefix sySemi",
"Statement : ServerDemux sySemi",
"Statement : TypeDecl sySemi",
"Statement : RoutineDecl sySemi",
"Statement : sySkip sySemi",
"Statement : Import sySemi",
"Statement : RCSDecl sySemi",
"Statement : sySemi",
"Statement : error sySemi",
"Subsystem : SubsystemStart SubsystemMods SubsystemName SubsystemBase",
"SubsystemStart : sySubsystem",
"SubsystemMods :",
"SubsystemMods : SubsystemMods SubsystemMod",
"SubsystemMod : syKernelUser",
"SubsystemMod : syKernelServer",
"SubsystemName : syIdentifier",
"SubsystemBase : syNumber",
"MsgOption : LookString syMsgOption syString",
"UseSpecialReplyPort : syUseSpecialReplyPort syNumber",
"UserTypeLimit : syUserTypeLimit syNumber",
"OnStackLimit : syOnStackLimit syNumber",
"WaitTime : LookString syWaitTime syString",
"WaitTime : syNoWaitTime",
"SendTime : LookString sySendTime syString",
"SendTime : syNoSendTime",
"Error : syErrorProc syIdentifier",
"ServerPrefix : syServerPrefix syIdentifier",
"UserPrefix : syUserPrefix syIdentifier",
"ServerDemux : syServerDemux syIdentifier",
"Import : LookFileName ImportIndicant syFileName",
"ImportIndicant : syImport",
"ImportIndicant : syUImport",
"ImportIndicant : sySImport",
"ImportIndicant : syIImport",
"ImportIndicant : syDImport",
"RCSDecl : LookQString syRCSId syQString",
"TypeDecl : syType NamedTypeSpec",
"NamedTypeSpec : syIdentifier syEqual TransTypeSpec",
"TransTypeSpec : TypeSpec",
"TransTypeSpec : TransTypeSpec syInTran syColon syIdentifier syIdentifier syLParen syIdentifier syRParen",
"TransTypeSpec : TransTypeSpec syOutTran syColon syIdentifier syIdentifier syLParen syIdentifier syRParen",
"TransTypeSpec : TransTypeSpec syDestructor syColon syIdentifier syLParen syIdentifier syRParen",
"TransTypeSpec : TransTypeSpec syCType syColon syIdentifier",
"TransTypeSpec : TransTypeSpec syCUserType syColon syIdentifier",
"TransTypeSpec : TransTypeSpec syCServerType syColon syIdentifier",
"TypeSpec : BasicTypeSpec",
"TypeSpec : PrevTypeSpec",
"TypeSpec : VarArrayHead TypeSpec",
"TypeSpec : ArrayHead TypeSpec",
"TypeSpec : syCaret TypeSpec",
"TypeSpec : StructHead TypeSpec",
"TypeSpec : CStringSpec",
"TypeSpec : NativeTypeSpec",
"NativeTypeSpec : syPointerTo syLParen TypePhrase syRParen",
"NativeTypeSpec : syPointerToIfNot syLParen TypePhrase syComma TypePhrase syRParen",
"NativeTypeSpec : syValueOf syLParen TypePhrase syRParen",
"BasicTypeSpec : IPCType",
"BasicTypeSpec : syLParen IPCType syComma IntExp IPCFlags syRParen",
"PrimIPCType : syNumber",
"PrimIPCType : sySymbolicType",
"IPCType : PrimIPCType",
"IPCType : PrimIPCType syBar PrimIPCType",
"PrevTypeSpec : syIdentifier",
"VarArrayHead : syArray syLBrack syRBrack syOf",
"VarArrayHead : syArray syLBrack syStar syRBrack syOf",
"VarArrayHead : syArray syLBrack syStar syColon IntExp syRBrack syOf",
"ArrayHead : syArray syLBrack IntExp syRBrack syOf",
"StructHead : syStruct syLBrack IntExp syRBrack syOf",
"CStringSpec : syCString syLBrack IntExp syRBrack",
"CStringSpec : syCString syLBrack syStar syColon IntExp syRBrack",
"TypePhrase : syIdentifier",
"TypePhrase : TypePhrase syIdentifier",
"IntExp : IntExp syPlus IntExp",
"IntExp : IntExp syMinus IntExp",
"IntExp : IntExp syStar IntExp",
"IntExp : IntExp syDiv IntExp",
"IntExp : syNumber",
"IntExp : syLParen IntExp syRParen",
"RoutineDecl : Routine",
"RoutineDecl : SimpleRoutine",
"Routine : syRoutine syIdentifier Arguments",
"SimpleRoutine : sySimpleRoutine syIdentifier Arguments",
"Arguments : syLParen syRParen",
"Arguments : syLParen ArgumentList syRParen",
"ArgumentList : Argument",
"ArgumentList : Trailer",
"ArgumentList : Argument sySemi ArgumentList",
"ArgumentList : Trailer sySemi ArgumentList",
"Argument : Direction syIdentifier ArgumentType IPCFlags",
"Trailer : TrImplKeyword syIdentifier ArgumentType",
"Direction :",
"Direction : syIn",
"Direction : syOut",
"Direction : syInOut",
"Direction : syRequestPort",
"Direction : syReplyPort",
"Direction : sySReplyPort",
"Direction : syUReplyPort",
"Direction : syWaitTime",
"Direction : sySendTime",
"Direction : syMsgOption",
"Direction : sySecToken",
"Direction : syServerSecToken",
"Direction : syUserSecToken",
"Direction : syAuditToken",
"Direction : syServerAuditToken",
"Direction : syUserAuditToken",
"Direction : syServerContextToken",
"Direction : syMsgSeqno",
"TrImplKeyword : syServerImpl",
"TrImplKeyword : syUserImpl",
"ArgumentType : syColon syIdentifier",
"ArgumentType : syColon NamedTypeSpec",
"ArgumentType : syColon NativeTypeSpec",
"IPCFlags :",
"IPCFlags : IPCFlags syComma syIPCFlag",
"IPCFlags : IPCFlags syComma syIPCFlag syLBrack syRBrack",
"LookString :",
"LookFileName :",
"LookQString :",

};
#endif

#if YYDEBUG
int      yydebug;
#endif

int      yyerrflag;
int      yychar;
YYSTYPE  yyval;
YYSTYPE  yylval;
int      yynerrs;

#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
YYLTYPE  yyloc; /* position returned by actions */
YYLTYPE  yylloc; /* position from the lexer */
#endif

#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
#ifndef YYLLOC_DEFAULT
#define YYLLOC_DEFAULT(loc, rhs, n) \
do \
{ \
    if (n == 0) \
    { \
        (loc).first_line   = YYRHSLOC(rhs, 0).last_line; \
        (loc).first_column = YYRHSLOC(rhs, 0).last_column; \
        (loc).last_line    = YYRHSLOC(rhs, 0).last_line; \
        (loc).last_column  = YYRHSLOC(rhs, 0).last_column; \
    } \
    else \
    { \
        (loc).first_line   = YYRHSLOC(rhs, 1).first_line; \
        (loc).first_column = YYRHSLOC(rhs, 1).first_column; \
        (loc).last_line    = YYRHSLOC(rhs, n).last_line; \
        (loc).last_column  = YYRHSLOC(rhs, n).last_column; \
    } \
} while (0)
#endif /* YYLLOC_DEFAULT */
#endif /* defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED) */
#if YYBTYACC

#ifndef YYLVQUEUEGROWTH
#define YYLVQUEUEGROWTH 32
#endif
#endif /* YYBTYACC */

/* define the initial stack-sizes */
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH  YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 10000
#define YYMAXDEPTH  10000
#endif
#endif

#ifndef YYINITSTACKSIZE
#define YYINITSTACKSIZE 200
#endif

typedef struct {
    unsigned stacksize;
    YYINT    *s_base;
    YYINT    *s_mark;
    YYINT    *s_last;
    YYSTYPE  *l_base;
    YYSTYPE  *l_mark;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    YYLTYPE  *p_base;
    YYLTYPE  *p_mark;
#endif
} YYSTACKDATA;
#if YYBTYACC

struct YYParseState_s
{
    struct YYParseState_s *save;    /* Previously saved parser state */
    YYSTACKDATA            yystack; /* saved parser stack */
    int                    state;   /* saved parser state */
    int                    errflag; /* saved error recovery status */
    int                    lexeme;  /* saved index of the conflict lexeme in the lexical queue */
    YYINT                  ctry;    /* saved index in yyctable[] for this conflict */
};
typedef struct YYParseState_s YYParseState;
#endif /* YYBTYACC */
/* variables for the parser stack */
static YYSTACKDATA yystack;
#if YYBTYACC

/* Current parser state */
static YYParseState *yyps = NULL;

/* yypath != NULL: do the full parse, starting at *yypath parser state. */
static YYParseState *yypath = NULL;

/* Base of the lexical value queue */
static YYSTYPE *yylvals = NULL;

/* Current position at lexical value queue */
static YYSTYPE *yylvp = NULL;

/* End position of lexical value queue */
static YYSTYPE *yylve = NULL;

/* The last allocated position at the lexical value queue */
static YYSTYPE *yylvlim = NULL;

#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
/* Base of the lexical position queue */
static YYLTYPE *yylpsns = NULL;

/* Current position at lexical position queue */
static YYLTYPE *yylpp = NULL;

/* End position of lexical position queue */
static YYLTYPE *yylpe = NULL;

/* The last allocated position at the lexical position queue */
static YYLTYPE *yylplim = NULL;
#endif

/* Current position at lexical token queue */
static YYINT  *yylexp = NULL;

static YYINT  *yylexemes = NULL;
#endif /* YYBTYACC */
#line 753 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"

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
#line 827 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"

/* For use in generated program */
#define yydepth (int)(yystack.s_mark - yystack.s_base)
#if YYBTYACC
#define yytrial (yyps->save)
#endif /* YYBTYACC */

#if YYDEBUG
#include <stdio.h>	/* needed for printf */
#endif

#include <stdlib.h>	/* needed for malloc, etc */
#include <string.h>	/* needed for memset */

/* allocate initial stack or double stack size, up to YYMAXDEPTH */
static int yygrowstack(YYSTACKDATA *data)
{
    int i;
    unsigned newsize;
    YYINT *newss;
    YYSTYPE *newvs;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    YYLTYPE *newps;
#endif

    if ((newsize = data->stacksize) == 0)
        newsize = YYINITSTACKSIZE;
    else if (newsize >= YYMAXDEPTH)
        return YYENOMEM;
    else if ((newsize *= 2) > YYMAXDEPTH)
        newsize = YYMAXDEPTH;

    i = (int) (data->s_mark - data->s_base);
    newss = (YYINT *)realloc(data->s_base, newsize * sizeof(*newss));
    if (newss == NULL)
        return YYENOMEM;

    data->s_base = newss;
    data->s_mark = newss + i;

    newvs = (YYSTYPE *)realloc(data->l_base, newsize * sizeof(*newvs));
    if (newvs == NULL)
        return YYENOMEM;

    data->l_base = newvs;
    data->l_mark = newvs + i;

#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    newps = (YYLTYPE *)realloc(data->p_base, newsize * sizeof(*newps));
    if (newps == NULL)
        return YYENOMEM;

    data->p_base = newps;
    data->p_mark = newps + i;
#endif

    data->stacksize = newsize;
    data->s_last = data->s_base + newsize - 1;

#if YYDEBUG
    if (yydebug)
        fprintf(stderr, "%sdebug: stack size increased to %d\n", YYPREFIX, newsize);
#endif
    return 0;
}

#if YYPURE || defined(YY_NO_LEAKS)
static void yyfreestack(YYSTACKDATA *data)
{
    free(data->s_base);
    free(data->l_base);
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    free(data->p_base);
#endif
    memset(data, 0, sizeof(*data));
}
#else
#define yyfreestack(data) /* nothing */
#endif /* YYPURE || defined(YY_NO_LEAKS) */
#if YYBTYACC

static YYParseState *
yyNewState(unsigned size)
{
    YYParseState *p = (YYParseState *) malloc(sizeof(YYParseState));
    if (p == NULL) return NULL;

    p->yystack.stacksize = size;
    if (size == 0)
    {
        p->yystack.s_base = NULL;
        p->yystack.l_base = NULL;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
        p->yystack.p_base = NULL;
#endif
        return p;
    }
    p->yystack.s_base    = (YYINT *) malloc(size * sizeof(YYINT));
    if (p->yystack.s_base == NULL) return NULL;
    p->yystack.l_base    = (YYSTYPE *) malloc(size * sizeof(YYSTYPE));
    if (p->yystack.l_base == NULL) return NULL;
    memset(p->yystack.l_base, 0, size * sizeof(YYSTYPE));
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    p->yystack.p_base    = (YYLTYPE *) malloc(size * sizeof(YYLTYPE));
    if (p->yystack.p_base == NULL) return NULL;
    memset(p->yystack.p_base, 0, size * sizeof(YYLTYPE));
#endif

    return p;
}

static void
yyFreeState(YYParseState *p)
{
    yyfreestack(&p->yystack);
    free(p);
}
#endif /* YYBTYACC */

#define YYABORT  goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR  goto yyerrlab
#if YYBTYACC
#define YYVALID        do { if (yyps->save)            goto yyvalid; } while(0)
#define YYVALID_NESTED do { if (yyps->save && \
                                yyps->save->save == 0) goto yyvalid; } while(0)
#endif /* YYBTYACC */

int
YYPARSE_DECL()
{
    int yym, yyn, yystate, yyresult;
#if YYBTYACC
    int yynewerrflag;
    YYParseState *yyerrctx = NULL;
#endif /* YYBTYACC */
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    YYLTYPE  yyerror_loc_range[3]; /* position of error start/end (0 unused) */
#endif
#if YYDEBUG
    const char *yys;

    if ((yys = getenv("YYDEBUG")) != NULL)
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
    if (yydebug)
        fprintf(stderr, "%sdebug[<# of symbols on state stack>]\n", YYPREFIX);
#endif
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    memset(yyerror_loc_range, 0, sizeof(yyerror_loc_range));
#endif

#if YYBTYACC
    yyps = yyNewState(0); if (yyps == NULL) goto yyenomem;
    yyps->save = NULL;
#endif /* YYBTYACC */
    yym = 0;
    /* yyn is set below */
    yynerrs = 0;
    yyerrflag = 0;
    yychar = YYEMPTY;
    yystate = 0;

#if YYPURE
    memset(&yystack, 0, sizeof(yystack));
#endif

    if (yystack.s_base == NULL && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
    yystack.s_mark = yystack.s_base;
    yystack.l_mark = yystack.l_base;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    yystack.p_mark = yystack.p_base;
#endif
    yystate = 0;
    *yystack.s_mark = 0;

yyloop:
    if ((yyn = yydefred[yystate]) != 0) goto yyreduce;
    if (yychar < 0)
    {
#if YYBTYACC
        do {
        if (yylvp < yylve)
        {
            /* we're currently re-reading tokens */
            yylval = *yylvp++;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            yylloc = *yylpp++;
#endif
            yychar = *yylexp++;
            break;
        }
        if (yyps->save)
        {
            /* in trial mode; save scanner results for future parse attempts */
            if (yylvp == yylvlim)
            {   /* Enlarge lexical value queue */
                size_t p = (size_t) (yylvp - yylvals);
                size_t s = (size_t) (yylvlim - yylvals);

                s += YYLVQUEUEGROWTH;
                if ((yylexemes = (YYINT *)realloc(yylexemes, s * sizeof(YYINT))) == NULL) goto yyenomem;
                if ((yylvals   = (YYSTYPE *)realloc(yylvals, s * sizeof(YYSTYPE))) == NULL) goto yyenomem;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                if ((yylpsns   = (YYLTYPE *)realloc(yylpsns, s * sizeof(YYLTYPE))) == NULL) goto yyenomem;
#endif
                yylvp   = yylve = yylvals + p;
                yylvlim = yylvals + s;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                yylpp   = yylpe = yylpsns + p;
                yylplim = yylpsns + s;
#endif
                yylexp  = yylexemes + p;
            }
            *yylexp = (YYINT) YYLEX;
            *yylvp++ = yylval;
            yylve++;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            *yylpp++ = yylloc;
            yylpe++;
#endif
            yychar = *yylexp++;
            break;
        }
        /* normal operation, no conflict encountered */
#endif /* YYBTYACC */
        yychar = YYLEX;
#if YYBTYACC
        } while (0);
#endif /* YYBTYACC */
        if (yychar < 0) yychar = YYEOF;
#if YYDEBUG
        if (yydebug)
        {
            if ((yys = yyname[YYTRANSLATE(yychar)]) == NULL) yys = yyname[YYUNDFTOKEN];
            fprintf(stderr, "%s[%d]: state %d, reading token %d (%s)",
                            YYDEBUGSTR, yydepth, yystate, yychar, yys);
#ifdef YYSTYPE_TOSTRING
#if YYBTYACC
            if (!yytrial)
#endif /* YYBTYACC */
                fprintf(stderr, " <%s>", YYSTYPE_TOSTRING(yychar, yylval));
#endif
            fputc('\n', stderr);
        }
#endif
    }
#if YYBTYACC

    /* Do we have a conflict? */
    if (((yyn = yycindex[yystate]) != 0) && (yyn += yychar) >= 0 &&
        yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yychar)
    {
        YYINT ctry;

        if (yypath)
        {
            YYParseState *save;
#if YYDEBUG
            if (yydebug)
                fprintf(stderr, "%s[%d]: CONFLICT in state %d: following successful trial parse\n",
                                YYDEBUGSTR, yydepth, yystate);
#endif
            /* Switch to the next conflict context */
            save = yypath;
            yypath = save->save;
            save->save = NULL;
            ctry = save->ctry;
            if (save->state != yystate) YYABORT;
            yyFreeState(save);

        }
        else
        {

            /* Unresolved conflict - start/continue trial parse */
            YYParseState *save;
#if YYDEBUG
            if (yydebug)
            {
                fprintf(stderr, "%s[%d]: CONFLICT in state %d. ", YYDEBUGSTR, yydepth, yystate);
                if (yyps->save)
                    fputs("ALREADY in conflict, continuing trial parse.\n", stderr);
                else
                    fputs("Starting trial parse.\n", stderr);
            }
#endif
            save                  = yyNewState((unsigned)(yystack.s_mark - yystack.s_base + 1));
            if (save == NULL) goto yyenomem;
            save->save            = yyps->save;
            save->state           = yystate;
            save->errflag         = yyerrflag;
            save->yystack.s_mark  = save->yystack.s_base + (yystack.s_mark - yystack.s_base);
            memcpy (save->yystack.s_base, yystack.s_base, (size_t) (yystack.s_mark - yystack.s_base + 1) * sizeof(YYINT));
            save->yystack.l_mark  = save->yystack.l_base + (yystack.l_mark - yystack.l_base);
            memcpy (save->yystack.l_base, yystack.l_base, (size_t) (yystack.l_mark - yystack.l_base + 1) * sizeof(YYSTYPE));
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            save->yystack.p_mark  = save->yystack.p_base + (yystack.p_mark - yystack.p_base);
            memcpy (save->yystack.p_base, yystack.p_base, (size_t) (yystack.p_mark - yystack.p_base + 1) * sizeof(YYLTYPE));
#endif
            ctry                  = yytable[yyn];
            if (yyctable[ctry] == -1)
            {
#if YYDEBUG
                if (yydebug && yychar >= YYEOF)
                    fprintf(stderr, "%s[%d]: backtracking 1 token\n", YYDEBUGSTR, yydepth);
#endif
                ctry++;
            }
            save->ctry = ctry;
            if (yyps->save == NULL)
            {
                /* If this is a first conflict in the stack, start saving lexemes */
                if (!yylexemes)
                {
                    yylexemes = (YYINT *) malloc((YYLVQUEUEGROWTH) * sizeof(YYINT));
                    if (yylexemes == NULL) goto yyenomem;
                    yylvals   = (YYSTYPE *) malloc((YYLVQUEUEGROWTH) * sizeof(YYSTYPE));
                    if (yylvals == NULL) goto yyenomem;
                    yylvlim   = yylvals + YYLVQUEUEGROWTH;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                    yylpsns   = (YYLTYPE *) malloc((YYLVQUEUEGROWTH) * sizeof(YYLTYPE));
                    if (yylpsns == NULL) goto yyenomem;
                    yylplim   = yylpsns + YYLVQUEUEGROWTH;
#endif
                }
                if (yylvp == yylve)
                {
                    yylvp  = yylve = yylvals;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                    yylpp  = yylpe = yylpsns;
#endif
                    yylexp = yylexemes;
                    if (yychar >= YYEOF)
                    {
                        *yylve++ = yylval;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                        *yylpe++ = yylloc;
#endif
                        *yylexp  = (YYINT) yychar;
                        yychar   = YYEMPTY;
                    }
                }
            }
            if (yychar >= YYEOF)
            {
                yylvp--;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                yylpp--;
#endif
                yylexp--;
                yychar = YYEMPTY;
            }
            save->lexeme = (int) (yylvp - yylvals);
            yyps->save   = save;
        }
        if (yytable[yyn] == ctry)
        {
#if YYDEBUG
            if (yydebug)
                fprintf(stderr, "%s[%d]: state %d, shifting to state %d\n",
                                YYDEBUGSTR, yydepth, yystate, yyctable[ctry]);
#endif
            if (yychar < 0)
            {
                yylvp++;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                yylpp++;
#endif
                yylexp++;
            }
            if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM)
                goto yyoverflow;
            yystate = yyctable[ctry];
            *++yystack.s_mark = (YYINT) yystate;
            *++yystack.l_mark = yylval;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            *++yystack.p_mark = yylloc;
#endif
            yychar  = YYEMPTY;
            if (yyerrflag > 0) --yyerrflag;
            goto yyloop;
        }
        else
        {
            yyn = yyctable[ctry];
            goto yyreduce;
        }
    } /* End of code dealing with conflicts */
#endif /* YYBTYACC */
    if (((yyn = yysindex[yystate]) != 0) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yychar)
    {
#if YYDEBUG
        if (yydebug)
            fprintf(stderr, "%s[%d]: state %d, shifting to state %d\n",
                            YYDEBUGSTR, yydepth, yystate, yytable[yyn]);
#endif
        if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
        yystate = yytable[yyn];
        *++yystack.s_mark = yytable[yyn];
        *++yystack.l_mark = yylval;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
        *++yystack.p_mark = yylloc;
#endif
        yychar = YYEMPTY;
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if (((yyn = yyrindex[yystate]) != 0) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag != 0) goto yyinrecovery;
#if YYBTYACC

    yynewerrflag = 1;
    goto yyerrhandler;
    goto yyerrlab; /* redundant goto avoids 'unused label' warning */

yyerrlab:
    /* explicit YYERROR from an action -- pop the rhs of the rule reduced
     * before looking for error recovery */
    yystack.s_mark -= yym;
    yystate = *yystack.s_mark;
    yystack.l_mark -= yym;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    yystack.p_mark -= yym;
#endif

    yynewerrflag = 0;
yyerrhandler:
    while (yyps->save)
    {
        int ctry;
        YYParseState *save = yyps->save;
#if YYDEBUG
        if (yydebug)
            fprintf(stderr, "%s[%d]: ERROR in state %d, CONFLICT BACKTRACKING to state %d, %d tokens\n",
                            YYDEBUGSTR, yydepth, yystate, yyps->save->state,
                    (int)(yylvp - yylvals - yyps->save->lexeme));
#endif
        /* Memorize most forward-looking error state in case it's really an error. */
        if (yyerrctx == NULL || yyerrctx->lexeme < yylvp - yylvals)
        {
            /* Free old saved error context state */
            if (yyerrctx) yyFreeState(yyerrctx);
            /* Create and fill out new saved error context state */
            yyerrctx                 = yyNewState((unsigned)(yystack.s_mark - yystack.s_base + 1));
            if (yyerrctx == NULL) goto yyenomem;
            yyerrctx->save           = yyps->save;
            yyerrctx->state          = yystate;
            yyerrctx->errflag        = yyerrflag;
            yyerrctx->yystack.s_mark = yyerrctx->yystack.s_base + (yystack.s_mark - yystack.s_base);
            memcpy (yyerrctx->yystack.s_base, yystack.s_base, (size_t) (yystack.s_mark - yystack.s_base + 1) * sizeof(YYINT));
            yyerrctx->yystack.l_mark = yyerrctx->yystack.l_base + (yystack.l_mark - yystack.l_base);
            memcpy (yyerrctx->yystack.l_base, yystack.l_base, (size_t) (yystack.l_mark - yystack.l_base + 1) * sizeof(YYSTYPE));
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            yyerrctx->yystack.p_mark = yyerrctx->yystack.p_base + (yystack.p_mark - yystack.p_base);
            memcpy (yyerrctx->yystack.p_base, yystack.p_base, (size_t) (yystack.p_mark - yystack.p_base + 1) * sizeof(YYLTYPE));
#endif
            yyerrctx->lexeme         = (int) (yylvp - yylvals);
        }
        yylvp          = yylvals   + save->lexeme;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
        yylpp          = yylpsns   + save->lexeme;
#endif
        yylexp         = yylexemes + save->lexeme;
        yychar         = YYEMPTY;
        yystack.s_mark = yystack.s_base + (save->yystack.s_mark - save->yystack.s_base);
        memcpy (yystack.s_base, save->yystack.s_base, (size_t) (yystack.s_mark - yystack.s_base + 1) * sizeof(YYINT));
        yystack.l_mark = yystack.l_base + (save->yystack.l_mark - save->yystack.l_base);
        memcpy (yystack.l_base, save->yystack.l_base, (size_t) (yystack.l_mark - yystack.l_base + 1) * sizeof(YYSTYPE));
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
        yystack.p_mark = yystack.p_base + (save->yystack.p_mark - save->yystack.p_base);
        memcpy (yystack.p_base, save->yystack.p_base, (size_t) (yystack.p_mark - yystack.p_base + 1) * sizeof(YYLTYPE));
#endif
        ctry           = ++save->ctry;
        yystate        = save->state;
        /* We tried shift, try reduce now */
        if ((yyn = yyctable[ctry]) >= 0) goto yyreduce;
        yyps->save     = save->save;
        save->save     = NULL;
        yyFreeState(save);

        /* Nothing left on the stack -- error */
        if (!yyps->save)
        {
#if YYDEBUG
            if (yydebug)
                fprintf(stderr, "%sdebug[%d,trial]: trial parse FAILED, entering ERROR mode\n",
                                YYPREFIX, yydepth);
#endif
            /* Restore state as it was in the most forward-advanced error */
            yylvp          = yylvals   + yyerrctx->lexeme;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            yylpp          = yylpsns   + yyerrctx->lexeme;
#endif
            yylexp         = yylexemes + yyerrctx->lexeme;
            yychar         = yylexp[-1];
            yylval         = yylvp[-1];
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            yylloc         = yylpp[-1];
#endif
            yystack.s_mark = yystack.s_base + (yyerrctx->yystack.s_mark - yyerrctx->yystack.s_base);
            memcpy (yystack.s_base, yyerrctx->yystack.s_base, (size_t) (yystack.s_mark - yystack.s_base + 1) * sizeof(YYINT));
            yystack.l_mark = yystack.l_base + (yyerrctx->yystack.l_mark - yyerrctx->yystack.l_base);
            memcpy (yystack.l_base, yyerrctx->yystack.l_base, (size_t) (yystack.l_mark - yystack.l_base + 1) * sizeof(YYSTYPE));
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            yystack.p_mark = yystack.p_base + (yyerrctx->yystack.p_mark - yyerrctx->yystack.p_base);
            memcpy (yystack.p_base, yyerrctx->yystack.p_base, (size_t) (yystack.p_mark - yystack.p_base + 1) * sizeof(YYLTYPE));
#endif
            yystate        = yyerrctx->state;
            yyFreeState(yyerrctx);
            yyerrctx       = NULL;
        }
        yynewerrflag = 1;
    }
    if (yynewerrflag == 0) goto yyinrecovery;
#endif /* YYBTYACC */

    YYERROR_CALL("syntax error");
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    yyerror_loc_range[1] = yylloc; /* lookahead position is error start position */
#endif

#if !YYBTYACC
    goto yyerrlab; /* redundant goto avoids 'unused label' warning */
yyerrlab:
#endif
    ++yynerrs;

yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if (((yyn = yysindex[*yystack.s_mark]) != 0) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    fprintf(stderr, "%s[%d]: state %d, error recovery shifting to state %d\n",
                                    YYDEBUGSTR, yydepth, *yystack.s_mark, yytable[yyn]);
#endif
                if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
                yystate = yytable[yyn];
                *++yystack.s_mark = yytable[yyn];
                *++yystack.l_mark = yylval;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                /* lookahead position is error end position */
                yyerror_loc_range[2] = yylloc;
                YYLLOC_DEFAULT(yyloc, yyerror_loc_range, 2); /* position of error span */
                *++yystack.p_mark = yyloc;
#endif
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    fprintf(stderr, "%s[%d]: error recovery discarding state %d\n",
                                    YYDEBUGSTR, yydepth, *yystack.s_mark);
#endif
                if (yystack.s_mark <= yystack.s_base) goto yyabort;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                /* the current TOS position is the error start position */
                yyerror_loc_range[1] = *yystack.p_mark;
#endif
#if defined(YYDESTRUCT_CALL)
#if YYBTYACC
                if (!yytrial)
#endif /* YYBTYACC */
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                    YYDESTRUCT_CALL("error: discarding state",
                                    yystos[*yystack.s_mark], yystack.l_mark, yystack.p_mark);
#else
                    YYDESTRUCT_CALL("error: discarding state",
                                    yystos[*yystack.s_mark], yystack.l_mark);
#endif /* defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED) */
#endif /* defined(YYDESTRUCT_CALL) */
                --yystack.s_mark;
                --yystack.l_mark;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                --yystack.p_mark;
#endif
            }
        }
    }
    else
    {
        if (yychar == YYEOF) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            if ((yys = yyname[YYTRANSLATE(yychar)]) == NULL) yys = yyname[YYUNDFTOKEN];
            fprintf(stderr, "%s[%d]: state %d, error recovery discarding token %d (%s)\n",
                            YYDEBUGSTR, yydepth, yystate, yychar, yys);
        }
#endif
#if defined(YYDESTRUCT_CALL)
#if YYBTYACC
        if (!yytrial)
#endif /* YYBTYACC */
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            YYDESTRUCT_CALL("error: discarding token", yychar, &yylval, &yylloc);
#else
            YYDESTRUCT_CALL("error: discarding token", yychar, &yylval);
#endif /* defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED) */
#endif /* defined(YYDESTRUCT_CALL) */
        yychar = YYEMPTY;
        goto yyloop;
    }

yyreduce:
    yym = yylen[yyn];
#if YYDEBUG
    if (yydebug)
    {
        fprintf(stderr, "%s[%d]: state %d, reducing by rule %d (%s)",
                        YYDEBUGSTR, yydepth, yystate, yyn, yyrule[yyn]);
#ifdef YYSTYPE_TOSTRING
#if YYBTYACC
        if (!yytrial)
#endif /* YYBTYACC */
            if (yym > 0)
            {
                int i;
                fputc('<', stderr);
                for (i = yym; i > 0; i--)
                {
                    if (i != yym) fputs(", ", stderr);
                    fputs(YYSTYPE_TOSTRING(yystos[yystack.s_mark[1-i]],
                                           yystack.l_mark[1-i]), stderr);
                }
                fputc('>', stderr);
            }
#endif
        fputc('\n', stderr);
    }
#endif
    if (yym > 0)
        yyval = yystack.l_mark[1-yym];
    else
        memset(&yyval, 0, sizeof yyval);
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)

    /* Perform position reduction */
    memset(&yyloc, 0, sizeof(yyloc));
#if YYBTYACC
    if (!yytrial)
#endif /* YYBTYACC */
    {
        YYLLOC_DEFAULT(yyloc, &yystack.p_mark[-yym], yym);
        /* just in case YYERROR is invoked within the action, save
           the start of the rhs as the error start position */
        yyerror_loc_range[1] = yystack.p_mark[1-yym];
    }
#endif

    switch (yyn)
    {
case 15:
#line 214 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{
    register statement_t *st = stAlloc();

    st->stKind = skRoutine;
    st->stRoutine = yystack.l_mark[-1].routine;
    rtCheckRoutine(yystack.l_mark[-1].routine);
    if (BeVerbose)
      rtPrintRoutine(yystack.l_mark[-1].routine);
}
#line 1508 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 16:
#line 224 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ rtSkip(); }
#line 1513 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 20:
#line 229 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyerrok; }
#line 1518 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 21:
#line 234 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{
  if (BeVerbose) {
    printf("Subsystem %s: base = %u%s%s\n\n",
    SubsystemName, SubsystemBase,
    IsKernelUser ? ", KernelUser" : "",
    IsKernelServer ? ", KernelServer" : "");
  }
}
#line 1530 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 22:
#line 245 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{
    if (SubsystemName != strNULL) {
      warn("previous Subsystem decl (of %s) will be ignored", SubsystemName);
      IsKernelUser = FALSE;
      IsKernelServer = FALSE;
      strfree(SubsystemName);
    }
}
#line 1542 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 25:
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
#line 1555 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 26:
#line 270 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{
    if (IsKernelServer)
      warn("duplicate KernelServer keyword");
    IsKernelServer = TRUE;
}
#line 1564 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 27:
#line 277 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ SubsystemName = yystack.l_mark[0].identifier; }
#line 1569 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 28:
#line 280 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ SubsystemBase = yystack.l_mark[0].number; }
#line 1574 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 29:
#line 284 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{
    if (streql(yystack.l_mark[0].string, "MACH_MSG_OPTION_NONE")) {
      MsgOption = strNULL;
      if (BeVerbose)
          printf("MsgOption: canceled\n\n");
    }
    else {
      MsgOption = yystack.l_mark[0].string;
      if (BeVerbose)
          printf("MsgOption %s\n\n",yystack.l_mark[0].string);
    }
}
#line 1590 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 30:
#line 299 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{
    UseSpecialReplyPort = (yystack.l_mark[0].number != 0);
    HasUseSpecialReplyPort |= UseSpecialReplyPort;
}
#line 1598 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 31:
#line 306 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{UserTypeLimit = yystack.l_mark[0].number; }
#line 1603 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 32:
#line 309 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{MaxMessSizeOnStack = yystack.l_mark[0].number; }
#line 1608 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 33:
#line 313 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{
    WaitTime = yystack.l_mark[0].string;
    if (BeVerbose)
      printf("WaitTime %s\n\n", WaitTime);
}
#line 1617 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 34:
#line 319 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{
    WaitTime = strNULL;
    if (BeVerbose)
      printf("NoWaitTime\n\n");
}
#line 1626 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 35:
#line 327 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{
    SendTime = yystack.l_mark[0].string;
    if (BeVerbose)
      printf("SendTime %s\n\n", SendTime);
}
#line 1635 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 36:
#line 333 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{
    SendTime = strNULL;
    if (BeVerbose)
      printf("NoSendTime\n\n");
}
#line 1644 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 37:
#line 341 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{
    ErrorProc = yystack.l_mark[0].identifier;
    if (BeVerbose)
      printf("ErrorProc %s\n\n", ErrorProc);
}
#line 1653 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 38:
#line 349 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{
    ServerPrefix = yystack.l_mark[0].identifier;
    if (BeVerbose)
      printf("ServerPrefix %s\n\n", ServerPrefix);
}
#line 1662 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 39:
#line 357 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{
    UserPrefix = yystack.l_mark[0].identifier;
    if (BeVerbose)
      printf("UserPrefix %s\n\n", UserPrefix);
}
#line 1671 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 40:
#line 365 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{
    ServerDemux = yystack.l_mark[0].identifier;
    if (BeVerbose)
      printf("ServerDemux %s\n\n", ServerDemux);
}
#line 1680 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 41:
#line 373 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{
    register statement_t *st = stAlloc();
    st->stKind = yystack.l_mark[-1].statement_kind;
    st->stFileName = yystack.l_mark[0].string;

    if (BeVerbose)
      printf("%s %s\n\n", import_name(yystack.l_mark[-1].statement_kind), yystack.l_mark[0].string);
}
#line 1692 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 42:
#line 383 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.statement_kind = skImport; }
#line 1697 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 43:
#line 384 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.statement_kind = skUImport; }
#line 1702 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 44:
#line 385 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.statement_kind = skSImport; }
#line 1707 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 45:
#line 386 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.statement_kind = skIImport; }
#line 1712 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 46:
#line 387 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.statement_kind = skDImport; }
#line 1717 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 47:
#line 391 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{
    if (RCSId != strNULL)
      warn("previous RCS decl will be ignored");
    if (BeVerbose)
      printf("RCSId %s\n\n", yystack.l_mark[0].string);
    RCSId = yystack.l_mark[0].string;
}
#line 1728 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 48:
#line 401 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{
    register identifier_t name = yystack.l_mark[0].type->itName;

    if (itLookUp(name) != itNULL)
      warn("overriding previous definition of %s", name);
    itInsert(name, yystack.l_mark[0].type);
}
#line 1739 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 49:
#line 411 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ itTypeDecl(yystack.l_mark[-2].identifier, yyval.type = yystack.l_mark[0].type); }
#line 1744 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 50:
#line 415 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.type = itResetType(yystack.l_mark[0].type); }
#line 1749 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 51:
#line 418 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{
    yyval.type = yystack.l_mark[-7].type;

    if ((yyval.type->itTransType != strNULL) && !streql(yyval.type->itTransType, yystack.l_mark[-4].identifier))
      warn("conflicting translation types (%s, %s)", yyval.type->itTransType, yystack.l_mark[-4].identifier);
    yyval.type->itTransType = yystack.l_mark[-4].identifier;

    if ((yyval.type->itInTrans != strNULL) && !streql(yyval.type->itInTrans, yystack.l_mark[-3].identifier))
      warn("conflicting in-translation functions (%s, %s)", yyval.type->itInTrans, yystack.l_mark[-3].identifier);
    yyval.type->itInTrans = yystack.l_mark[-3].identifier;

    if ((yyval.type->itServerType != strNULL) && !streql(yyval.type->itServerType, yystack.l_mark[-1].identifier))
      warn("conflicting server types (%s, %s)", yyval.type->itServerType, yystack.l_mark[-1].identifier);
    yyval.type->itServerType = yystack.l_mark[-1].identifier;
}
#line 1768 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 52:
#line 435 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{
    yyval.type = yystack.l_mark[-7].type;

    if ((yyval.type->itServerType != strNULL) && !streql(yyval.type->itServerType, yystack.l_mark[-4].identifier))
      warn("conflicting server types (%s, %s)", yyval.type->itServerType, yystack.l_mark[-4].identifier);
    yyval.type->itServerType = yystack.l_mark[-4].identifier;

    if ((yyval.type->itOutTrans != strNULL) && !streql(yyval.type->itOutTrans, yystack.l_mark[-3].identifier))
      warn("conflicting out-translation functions (%s, %s)", yyval.type->itOutTrans, yystack.l_mark[-3].identifier);
    yyval.type->itOutTrans = yystack.l_mark[-3].identifier;

    if ((yyval.type->itTransType != strNULL) && !streql(yyval.type->itTransType, yystack.l_mark[-1].identifier))
      warn("conflicting translation types (%s, %s)", yyval.type->itTransType, yystack.l_mark[-1].identifier);
    yyval.type->itTransType = yystack.l_mark[-1].identifier;
}
#line 1787 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 53:
#line 452 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{
    yyval.type = yystack.l_mark[-6].type;

    if ((yyval.type->itDestructor != strNULL) && !streql(yyval.type->itDestructor, yystack.l_mark[-3].identifier))
      warn("conflicting destructor functions (%s, %s)", yyval.type->itDestructor, yystack.l_mark[-3].identifier);
    yyval.type->itDestructor = yystack.l_mark[-3].identifier;

    if ((yyval.type->itTransType != strNULL) && !streql(yyval.type->itTransType, yystack.l_mark[-1].identifier))
      warn("conflicting translation types (%s, %s)", yyval.type->itTransType, yystack.l_mark[-1].identifier);
    yyval.type->itTransType = yystack.l_mark[-1].identifier;
}
#line 1802 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 54:
#line 464 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{
    yyval.type = yystack.l_mark[-3].type;

    if ((yyval.type->itUserType != strNULL) && !streql(yyval.type->itUserType, yystack.l_mark[0].identifier))
      warn("conflicting user types (%s, %s)", yyval.type->itUserType, yystack.l_mark[0].identifier);
    yyval.type->itUserType = yystack.l_mark[0].identifier;

    if ((yyval.type->itServerType != strNULL) && !streql(yyval.type->itServerType, yystack.l_mark[0].identifier))
      warn("conflicting server types (%s, %s)", yyval.type->itServerType, yystack.l_mark[0].identifier);
    yyval.type->itServerType = yystack.l_mark[0].identifier;
}
#line 1817 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 55:
#line 476 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{
    yyval.type = yystack.l_mark[-3].type;

    if ((yyval.type->itUserType != strNULL) && !streql(yyval.type->itUserType, yystack.l_mark[0].identifier))
      warn("conflicting user types (%s, %s)", yyval.type->itUserType, yystack.l_mark[0].identifier);
    yyval.type->itUserType = yystack.l_mark[0].identifier;
}
#line 1828 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 56:
#line 485 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{
    yyval.type = yystack.l_mark[-3].type;

    if ((yyval.type->itServerType != strNULL) && !streql(yyval.type->itServerType, yystack.l_mark[0].identifier))
	warn("conflicting server types (%s, %s)",
	     yyval.type->itServerType, yystack.l_mark[0].identifier);
    yyval.type->itServerType = yystack.l_mark[0].identifier;
}
#line 1840 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 57:
#line 496 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.type = yystack.l_mark[0].type; }
#line 1845 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 58:
#line 498 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.type = yystack.l_mark[0].type; }
#line 1850 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 59:
#line 500 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.type = itVarArrayDecl(yystack.l_mark[-1].number, yystack.l_mark[0].type); }
#line 1855 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 60:
#line 502 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.type = itArrayDecl(yystack.l_mark[-1].number, yystack.l_mark[0].type); }
#line 1860 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 61:
#line 504 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.type = itPtrDecl(yystack.l_mark[0].type); }
#line 1865 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 62:
#line 506 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.type = itStructDecl(yystack.l_mark[-1].number, yystack.l_mark[0].type); }
#line 1870 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 63:
#line 508 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.type = yystack.l_mark[0].type; }
#line 1875 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 64:
#line 510 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.type = yystack.l_mark[0].type; }
#line 1880 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 65:
#line 514 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.type = itNativeType(yystack.l_mark[-1].identifier, TRUE, 0); }
#line 1885 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 66:
#line 517 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.type = itNativeType(yystack.l_mark[-3].identifier, TRUE, yystack.l_mark[-1].identifier); }
#line 1890 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 67:
#line 519 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.type = itNativeType(yystack.l_mark[-1].identifier, FALSE, 0); }
#line 1895 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 68:
#line 523 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{
    yyval.type = itShortDecl(yystack.l_mark[0].symtype.innumber, yystack.l_mark[0].symtype.instr,
		     yystack.l_mark[0].symtype.outnumber, yystack.l_mark[0].symtype.outstr,
		     yystack.l_mark[0].symtype.size);
}
#line 1904 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 69:
#line 530 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{
    error("Long form type declarations aren't allowed any longer\n");
}
#line 1911 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 70:
#line 536 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{
    yyval.symtype.innumber = yyval.symtype.outnumber = yystack.l_mark[0].number;
    yyval.symtype.instr = yyval.symtype.outstr = strNULL;
    yyval.symtype.size = 0;
}
#line 1920 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 71:
#line 542 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.symtype = yystack.l_mark[0].symtype; }
#line 1925 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 72:
#line 546 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.symtype = yystack.l_mark[0].symtype; }
#line 1930 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 73:
#line 548 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{
  if (yystack.l_mark[-2].symtype.size != yystack.l_mark[0].symtype.size) {
    if (yystack.l_mark[-2].symtype.size == 0)
      yyval.symtype.size = yystack.l_mark[0].symtype.size;
    else if (yystack.l_mark[0].symtype.size == 0)
      yyval.symtype.size = yystack.l_mark[-2].symtype.size;
    else {
      error("sizes in IPCTypes (%d, %d) aren't equal",
      yystack.l_mark[-2].symtype.size, yystack.l_mark[0].symtype.size);
      yyval.symtype.size = 0;
    }
  }
  else
    yyval.symtype.size = yystack.l_mark[-2].symtype.size;
  yyval.symtype.innumber = yystack.l_mark[-2].symtype.innumber;
  yyval.symtype.instr = yystack.l_mark[-2].symtype.instr;
  yyval.symtype.outnumber = yystack.l_mark[0].symtype.outnumber;
  yyval.symtype.outstr = yystack.l_mark[0].symtype.outstr;
}
#line 1953 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 74:
#line 570 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.type = itPrevDecl(yystack.l_mark[0].identifier); }
#line 1958 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 75:
#line 574 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.number = 0; }
#line 1963 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 76:
#line 576 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.number = 0; }
#line 1968 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 77:
#line 579 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.number = yystack.l_mark[-2].number; }
#line 1973 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 78:
#line 583 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.number = yystack.l_mark[-2].number; }
#line 1978 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 79:
#line 587 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.number = yystack.l_mark[-2].number; }
#line 1983 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 80:
#line 591 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.type = itCStringDecl(yystack.l_mark[-1].number, FALSE); }
#line 1988 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 81:
#line 594 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.type = itCStringDecl(yystack.l_mark[-1].number, TRUE); }
#line 1993 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 82:
#line 598 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.identifier = yystack.l_mark[0].identifier; }
#line 1998 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 83:
#line 600 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.identifier = strphrase(yystack.l_mark[-1].identifier, yystack.l_mark[0].identifier); strfree(yystack.l_mark[0].identifier); }
#line 2003 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 84:
#line 604 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.number = yystack.l_mark[-2].number + yystack.l_mark[0].number;	}
#line 2008 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 85:
#line 606 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.number = yystack.l_mark[-2].number - yystack.l_mark[0].number;	}
#line 2013 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 86:
#line 608 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.number = yystack.l_mark[-2].number * yystack.l_mark[0].number;	}
#line 2018 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 87:
#line 610 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.number = yystack.l_mark[-2].number / yystack.l_mark[0].number;	}
#line 2023 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 88:
#line 612 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.number = yystack.l_mark[0].number;	}
#line 2028 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 89:
#line 614 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.number = yystack.l_mark[-1].number;	}
#line 2033 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 90:
#line 618 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.routine = yystack.l_mark[0].routine; }
#line 2038 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 91:
#line 619 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.routine = yystack.l_mark[0].routine; }
#line 2043 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 92:
#line 623 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.routine = rtMakeRoutine(yystack.l_mark[-1].identifier, yystack.l_mark[0].argument); }
#line 2048 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 93:
#line 627 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.routine = rtMakeSimpleRoutine(yystack.l_mark[-1].identifier, yystack.l_mark[0].argument); }
#line 2053 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 94:
#line 631 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.argument = argNULL; }
#line 2058 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 95:
#line 633 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.argument = yystack.l_mark[-1].argument; }
#line 2063 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 96:
#line 638 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.argument = yystack.l_mark[0].argument; }
#line 2068 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 97:
#line 640 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.argument = yystack.l_mark[0].argument; }
#line 2073 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 98:
#line 642 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{
  yyval.argument = yystack.l_mark[-2].argument;
  yyval.argument->argNext = yystack.l_mark[0].argument;
}
#line 2081 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 99:
#line 647 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{
  yyval.argument = yystack.l_mark[-2].argument;
  yyval.argument->argNext = yystack.l_mark[0].argument;
}
#line 2089 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 100:
#line 654 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{
  yyval.argument = argAlloc();
  yyval.argument->argKind = yystack.l_mark[-3].direction;
  yyval.argument->argName = yystack.l_mark[-2].identifier;
  yyval.argument->argType = yystack.l_mark[-1].type;
  yyval.argument->argFlags = yystack.l_mark[0].flag;
  if (yystack.l_mark[-1].type && yystack.l_mark[-1].type->itNative) {
      if (yystack.l_mark[-3].direction != akIn && yystack.l_mark[-3].direction != akOut && yystack.l_mark[-3].direction != akInOut)
        error("Illegal direction specified");

      if (!(yystack.l_mark[-1].type->itNativePointer) && yystack.l_mark[-3].direction != akIn)
        error("ValueOf only valid for in");

      if ((yystack.l_mark[-1].type->itBadValue) != NULL && yystack.l_mark[-3].direction != akIn)
        error("PointerToIfNot only valid for in");
  }
}
#line 2110 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 101:
#line 674 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{
    yyval.argument = argAlloc();
    yyval.argument->argKind = yystack.l_mark[-2].direction;
    yyval.argument->argName = yystack.l_mark[-1].identifier;
    yyval.argument->argType = yystack.l_mark[0].type;
}
#line 2120 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 102:
#line 683 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.direction = akNone; }
#line 2125 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 103:
#line 684 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.direction = akIn; }
#line 2130 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 104:
#line 685 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.direction = akOut; }
#line 2135 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 105:
#line 686 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.direction = akInOut; }
#line 2140 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 106:
#line 687 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.direction = akRequestPort; }
#line 2145 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 107:
#line 688 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.direction = akReplyPort; }
#line 2150 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 108:
#line 689 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.direction = akSReplyPort; }
#line 2155 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 109:
#line 690 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.direction = akUReplyPort; }
#line 2160 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 110:
#line 691 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.direction = akWaitTime; }
#line 2165 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 111:
#line 692 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.direction = akSendTime; }
#line 2170 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 112:
#line 693 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.direction = akMsgOption; }
#line 2175 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 113:
#line 694 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.direction = akSecToken; }
#line 2180 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 114:
#line 695 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.direction = akServerSecToken; }
#line 2185 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 115:
#line 696 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.direction = akUserSecToken; }
#line 2190 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 116:
#line 697 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.direction = akAuditToken; }
#line 2195 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 117:
#line 698 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.direction = akServerAuditToken; }
#line 2200 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 118:
#line 699 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.direction = akUserAuditToken; }
#line 2205 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 119:
#line 700 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.direction = akServerContextToken; }
#line 2210 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 120:
#line 701 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.direction = akMsgSeqno; }
#line 2215 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 121:
#line 706 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.direction = akServerImpl; }
#line 2220 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 122:
#line 707 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.direction = akUserImpl; }
#line 2225 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 123:
#line 712 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{
  yyval.type = itLookUp(yystack.l_mark[0].identifier);
  if (yyval.type == itNULL)
    error("type '%s' not defined", yystack.l_mark[0].identifier);
}
#line 2234 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 124:
#line 718 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.type = yystack.l_mark[0].type; }
#line 2239 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 125:
#line 720 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.type = yystack.l_mark[0].type; }
#line 2244 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 126:
#line 724 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ yyval.flag = flNone; }
#line 2249 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 127:
#line 726 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{
  if (yystack.l_mark[-2].flag & yystack.l_mark[0].flag)
    warn("redundant IPC flag ignored");
  else
    yyval.flag = yystack.l_mark[-2].flag | yystack.l_mark[0].flag;
}
#line 2259 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 128:
#line 733 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{
  if (yystack.l_mark[-2].flag != flDealloc)
    warn("only Dealloc is variable");
  else
    yyval.flag = yystack.l_mark[-4].flag | flMaybeDealloc;
}
#line 2269 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 129:
#line 741 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ LookString(); }
#line 2274 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 130:
#line 745 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ LookFileName(); }
#line 2279 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
case 131:
#line 749 "/vercel/sandbox/ravynos/Kernel/build/mig/src/parser.y"
	{ LookQString(); }
#line 2284 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
break;
#line 2286 "/vercel/sandbox/ravynos/Kernel/build/mig/generated/y.tab.c"
    default:
        break;
    }
    yystack.s_mark -= yym;
    yystate = *yystack.s_mark;
    yystack.l_mark -= yym;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    yystack.p_mark -= yym;
#endif
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
        {
            fprintf(stderr, "%s[%d]: after reduction, ", YYDEBUGSTR, yydepth);
#ifdef YYSTYPE_TOSTRING
#if YYBTYACC
            if (!yytrial)
#endif /* YYBTYACC */
                fprintf(stderr, "result is <%s>, ", YYSTYPE_TOSTRING(yystos[YYFINAL], yyval));
#endif
            fprintf(stderr, "shifting from state 0 to final state %d\n", YYFINAL);
        }
#endif
        yystate = YYFINAL;
        *++yystack.s_mark = YYFINAL;
        *++yystack.l_mark = yyval;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
        *++yystack.p_mark = yyloc;
#endif
        if (yychar < 0)
        {
#if YYBTYACC
            do {
            if (yylvp < yylve)
            {
                /* we're currently re-reading tokens */
                yylval = *yylvp++;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                yylloc = *yylpp++;
#endif
                yychar = *yylexp++;
                break;
            }
            if (yyps->save)
            {
                /* in trial mode; save scanner results for future parse attempts */
                if (yylvp == yylvlim)
                {   /* Enlarge lexical value queue */
                    size_t p = (size_t) (yylvp - yylvals);
                    size_t s = (size_t) (yylvlim - yylvals);

                    s += YYLVQUEUEGROWTH;
                    if ((yylexemes = (YYINT *)realloc(yylexemes, s * sizeof(YYINT))) == NULL)
                        goto yyenomem;
                    if ((yylvals   = (YYSTYPE *)realloc(yylvals, s * sizeof(YYSTYPE))) == NULL)
                        goto yyenomem;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                    if ((yylpsns   = (YYLTYPE *)realloc(yylpsns, s * sizeof(YYLTYPE))) == NULL)
                        goto yyenomem;
#endif
                    yylvp   = yylve = yylvals + p;
                    yylvlim = yylvals + s;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                    yylpp   = yylpe = yylpsns + p;
                    yylplim = yylpsns + s;
#endif
                    yylexp  = yylexemes + p;
                }
                *yylexp = (YYINT) YYLEX;
                *yylvp++ = yylval;
                yylve++;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                *yylpp++ = yylloc;
                yylpe++;
#endif
                yychar = *yylexp++;
                break;
            }
            /* normal operation, no conflict encountered */
#endif /* YYBTYACC */
            yychar = YYLEX;
#if YYBTYACC
            } while (0);
#endif /* YYBTYACC */
            if (yychar < 0) yychar = YYEOF;
#if YYDEBUG
            if (yydebug)
            {
                if ((yys = yyname[YYTRANSLATE(yychar)]) == NULL) yys = yyname[YYUNDFTOKEN];
                fprintf(stderr, "%s[%d]: state %d, reading token %d (%s)\n",
                                YYDEBUGSTR, yydepth, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == YYEOF) goto yyaccept;
        goto yyloop;
    }
    if (((yyn = yygindex[yym]) != 0) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
    {
        fprintf(stderr, "%s[%d]: after reduction, ", YYDEBUGSTR, yydepth);
#ifdef YYSTYPE_TOSTRING
#if YYBTYACC
        if (!yytrial)
#endif /* YYBTYACC */
            fprintf(stderr, "result is <%s>, ", YYSTYPE_TOSTRING(yystos[yystate], yyval));
#endif
        fprintf(stderr, "shifting from state %d to state %d\n", *yystack.s_mark, yystate);
    }
#endif
    if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
    *++yystack.s_mark = (YYINT) yystate;
    *++yystack.l_mark = yyval;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    *++yystack.p_mark = yyloc;
#endif
    goto yyloop;
#if YYBTYACC

    /* Reduction declares that this path is valid. Set yypath and do a full parse */
yyvalid:
    if (yypath) YYABORT;
    while (yyps->save)
    {
        YYParseState *save = yyps->save;
        yyps->save = save->save;
        save->save = yypath;
        yypath = save;
    }
#if YYDEBUG
    if (yydebug)
        fprintf(stderr, "%s[%d]: state %d, CONFLICT trial successful, backtracking to state %d, %d tokens\n",
                        YYDEBUGSTR, yydepth, yystate, yypath->state, (int)(yylvp - yylvals - yypath->lexeme));
#endif
    if (yyerrctx)
    {
        yyFreeState(yyerrctx);
        yyerrctx = NULL;
    }
    yylvp          = yylvals + yypath->lexeme;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    yylpp          = yylpsns + yypath->lexeme;
#endif
    yylexp         = yylexemes + yypath->lexeme;
    yychar         = YYEMPTY;
    yystack.s_mark = yystack.s_base + (yypath->yystack.s_mark - yypath->yystack.s_base);
    memcpy (yystack.s_base, yypath->yystack.s_base, (size_t) (yystack.s_mark - yystack.s_base + 1) * sizeof(YYINT));
    yystack.l_mark = yystack.l_base + (yypath->yystack.l_mark - yypath->yystack.l_base);
    memcpy (yystack.l_base, yypath->yystack.l_base, (size_t) (yystack.l_mark - yystack.l_base + 1) * sizeof(YYSTYPE));
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    yystack.p_mark = yystack.p_base + (yypath->yystack.p_mark - yypath->yystack.p_base);
    memcpy (yystack.p_base, yypath->yystack.p_base, (size_t) (yystack.p_mark - yystack.p_base + 1) * sizeof(YYLTYPE));
#endif
    yystate        = yypath->state;
    goto yyloop;
#endif /* YYBTYACC */

yyoverflow:
    YYERROR_CALL("yacc stack overflow");
#if YYBTYACC
    goto yyabort_nomem;
yyenomem:
    YYERROR_CALL("memory exhausted");
yyabort_nomem:
#endif /* YYBTYACC */
    yyresult = 2;
    goto yyreturn;

yyabort:
    yyresult = 1;
    goto yyreturn;

yyaccept:
#if YYBTYACC
    if (yyps->save) goto yyvalid;
#endif /* YYBTYACC */
    yyresult = 0;

yyreturn:
#if defined(YYDESTRUCT_CALL)
    if (yychar != YYEOF && yychar != YYEMPTY)
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
        YYDESTRUCT_CALL("cleanup: discarding token", yychar, &yylval, &yylloc);
#else
        YYDESTRUCT_CALL("cleanup: discarding token", yychar, &yylval);
#endif /* defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED) */

    {
        YYSTYPE *pv;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
        YYLTYPE *pp;

        for (pv = yystack.l_base, pp = yystack.p_base; pv <= yystack.l_mark; ++pv, ++pp)
             YYDESTRUCT_CALL("cleanup: discarding state",
                             yystos[*(yystack.s_base + (pv - yystack.l_base))], pv, pp);
#else
        for (pv = yystack.l_base; pv <= yystack.l_mark; ++pv)
             YYDESTRUCT_CALL("cleanup: discarding state",
                             yystos[*(yystack.s_base + (pv - yystack.l_base))], pv);
#endif /* defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED) */
    }
#endif /* defined(YYDESTRUCT_CALL) */

#if YYBTYACC
    if (yyerrctx)
    {
        yyFreeState(yyerrctx);
        yyerrctx = NULL;
    }
    while (yyps)
    {
        YYParseState *save = yyps;
        yyps = save->save;
        save->save = NULL;
        yyFreeState(save);
    }
    while (yypath)
    {
        YYParseState *save = yypath;
        yypath = save->save;
        save->save = NULL;
        yyFreeState(save);
    }
#endif /* YYBTYACC */
    yyfreestack(&yystack);
    return (yyresult);
}
