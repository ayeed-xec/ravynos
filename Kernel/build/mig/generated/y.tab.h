#ifndef _yy_defines_h_
#define _yy_defines_h_

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
#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
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
extern YYSTYPE yylval;

#endif /* _yy_defines_h_ */
