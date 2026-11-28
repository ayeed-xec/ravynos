// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2026 The ravynOS Authors
//
// Hand-written scanner for the vendored mig grammar. It implements the rules
// of lexxer.l without flex (ADR-17): the generated scanner is pruned from the
// vendored tree by `rv vendors check`. The path is historical; the host-tool
// gate requires a checked-in scanner here so the build never needs flex.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "global.h"
#include "lexxer.h"
#include "mig_machine.h"
#include "statement.h"
#include "strdefs.h"
#include "type.h"
#include "y.tab.h"
#include <mach/message.h>
#include <mach/std_types.h>
#include <sys/types.h>

#define PortSize (sizeof(mach_port_t) * NBBY)

int lineno = 0;

enum scan_state {
    ST_INITIAL,
    ST_NORMAL,
    ST_STRING,
    ST_FILENAME,
    ST_QSTRING,
    ST_SKIPTEOL,
};

enum kw_action { KW_RETURN, KW_TP, KW_FLAG };

struct keyword {
    const char *text;
    int token;
    enum kw_action action;
    unsigned in_number;
    unsigned out_number;
    unsigned size;
    string_t in_str;
    string_t out_str;
    ipc_flags_t flag;
    int fold;
};

#define RV_STR2(x) #x
#define RV_STR(x) RV_STR2(x)
#define K(text, tok)                                                                               \
    {                                                                                              \
        text, tok, KW_RETURN, 0, 0, 0, NULL, NULL, 0, 1                                            \
    }
#define KS(text, tok)                                                                              \
    {                                                                                              \
        text, tok, KW_RETURN, 0, 0, 0, NULL, NULL, 0, 0                                            \
    }
#define FL(text, val)                                                                              \
    {                                                                                              \
        text, syIPCFlag, KW_FLAG, 0, 0, 0, NULL, NULL, val, 1                                      \
    }
#define TP(text, inn, out, sz)                                                                     \
    {                                                                                              \
        text, sySymbolicType, KW_TP, inn, out, sz, RV_STR(inn), RV_STR(out), 0, 1                  \
    }
#define TR(text, type, sz)                                                                         \
    {                                                                                              \
        text, sySymbolicType, KW_TP, type, type, sz, RV_STR(type), RV_STR(type), 0, 1              \
    }

static const struct keyword keywords[] = {
    K("ROUTINE", syRoutine),
    K("SIMPLEROUTINE", sySimpleRoutine),
    K("SUBSYSTEM", sySubsystem),
    K("MSGOPTION", syMsgOption),
    K("USESPECIALREPLYPORT", syUseSpecialReplyPort),
    K("CONSUMEONSENDERROR", syConsumeOnSendError),
    K("MSGSEQNO", syMsgSeqno),
    K("WAITTIME", syWaitTime),
    K("SENDTIME", sySendTime),
    K("NOWAITTIME", syNoWaitTime),
    K("NOSENDTIME", syNoSendTime),
    K("IN", syIn),
    K("OUT", syOut),
    K("USERIMPL", syUserImpl),
    K("SERVERIMPL", syServerImpl),
    K("SECTOKEN", sySecToken),
    K("SERVERSECTOKEN", syServerSecToken),
    K("USERSECTOKEN", syUserSecToken),
    K("AUDITTOKEN", syAuditToken),
    K("SERVERAUDITTOKEN", syServerAuditToken),
    K("USERAUDITTOKEN", syUserAuditToken),
    K("SERVERCONTEXTTOKEN", syServerContextToken),
    K("INOUT", syInOut),
    K("REQUESTPORT", syRequestPort),
    K("REPLYPORT", syReplyPort),
    K("UREPLYPORT", syUReplyPort),
    K("SREPLYPORT", sySReplyPort),
    K("ARRAY", syArray),
    K("OF", syOf),
    K("ERROR", syErrorProc),
    K("SERVERPREFIX", syServerPrefix),
    K("USERPREFIX", syUserPrefix),
    K("SERVERDEMUX", syServerDemux),
    K("RCSID", syRCSId),
    K("IMPORT", syImport),
    K("UIMPORT", syUImport),
    K("SIMPORT", sySImport),
    K("DIMPORT", syDImport),
    K("IIMPORT", syIImport),
    K("TYPE", syType),
    K("KERNELSERVER", syKernelServer),
    K("KERNELUSER", syKernelUser),
    K("SKIP", sySkip),
    K("STRUCT", syStruct),
    K("INTRAN", syInTran),
    K("OUTTRAN", syOutTran),
    K("DESTRUCTOR", syDestructor),
    K("CTYPE", syCType),
    K("CUSERTYPE", syCUserType),
    K("CSERVERTYPE", syCServerType),
    K("C_STRING", syCString),
    FL("SAMECOUNT", flSameCount),
    FL("RETCODE", flRetCode),
    FL("PHYSICALCOPY", flPhysicalCopy),
    FL("OVERWRITE", flOverwrite),
    FL("DEALLOC", flDealloc),
    FL("NOTDEALLOC", flNotDealloc),
    FL("COUNTINOUT", flCountInOut),
    TP("POLYMORPHIC", MACH_MSG_TYPE_POLYMORPHIC, MACH_MSG_TYPE_POLYMORPHIC, PortSize),
    FL("AUTO", flAuto),
    FL("CONST", flConst),
    KS("PointerTo", syPointerTo),
    KS("PointerToIfNot", syPointerToIfNot),
    KS("ValueOf", syValueOf),
    KS("UserTypeLimit", syUserTypeLimit),
    KS("OnStackLimit", syOnStackLimit),
    TR("MACH_MSG_TYPE_UNSTRUCTURED", MACH_MSG_TYPE_UNSTRUCTURED, 0),
    TR("MACH_MSG_TYPE_BIT", MACH_MSG_TYPE_BIT, 1),
    TR("MACH_MSG_TYPE_BOOLEAN", MACH_MSG_TYPE_BOOLEAN, 32),
    TR("MACH_MSG_TYPE_INTEGER_8", MACH_MSG_TYPE_INTEGER_8, 8),
    TR("MACH_MSG_TYPE_INTEGER_16", MACH_MSG_TYPE_INTEGER_16, 16),
    TR("MACH_MSG_TYPE_INTEGER_32", MACH_MSG_TYPE_INTEGER_32, 32),
    TR("MACH_MSG_TYPE_INTEGER_64", MACH_MSG_TYPE_INTEGER_64, 64),
    TR("MACH_MSG_TYPE_REAL_32", MACH_MSG_TYPE_REAL_32, 32),
    TR("MACH_MSG_TYPE_REAL_64", MACH_MSG_TYPE_REAL_64, 64),
    TR("MACH_MSG_TYPE_CHAR", MACH_MSG_TYPE_CHAR, 8),
    TR("MACH_MSG_TYPE_BYTE", MACH_MSG_TYPE_BYTE, 8),
    TP("MACH_MSG_TYPE_MOVE_RECEIVE", MACH_MSG_TYPE_MOVE_RECEIVE, MACH_MSG_TYPE_PORT_RECEIVE,
       PortSize),
    TP("MACH_MSG_TYPE_COPY_SEND", MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_PORT_SEND, PortSize),
    TP("MACH_MSG_TYPE_MAKE_SEND", MACH_MSG_TYPE_MAKE_SEND, MACH_MSG_TYPE_PORT_SEND, PortSize),
    TP("MACH_MSG_TYPE_MOVE_SEND", MACH_MSG_TYPE_MOVE_SEND, MACH_MSG_TYPE_PORT_SEND, PortSize),
    TP("MACH_MSG_TYPE_MAKE_SEND_ONCE", MACH_MSG_TYPE_MAKE_SEND_ONCE, MACH_MSG_TYPE_PORT_SEND_ONCE,
       PortSize),
    TP("MACH_MSG_TYPE_MOVE_SEND_ONCE", MACH_MSG_TYPE_MOVE_SEND_ONCE, MACH_MSG_TYPE_PORT_SEND_ONCE,
       PortSize),
    TR("MACH_MSG_TYPE_PORT_NAME", MACH_MSG_TYPE_PORT_NAME, PortSize),
    TP("MACH_MSG_TYPE_PORT_RECEIVE", MACH_MSG_TYPE_POLYMORPHIC, MACH_MSG_TYPE_PORT_RECEIVE,
       PortSize),
    TP("MACH_MSG_TYPE_PORT_SEND", MACH_MSG_TYPE_POLYMORPHIC, MACH_MSG_TYPE_PORT_SEND, PortSize),
    TP("MACH_MSG_TYPE_PORT_SEND_ONCE", MACH_MSG_TYPE_POLYMORPHIC, MACH_MSG_TYPE_PORT_SEND_ONCE,
       PortSize),
    TP("MACH_MSG_TYPE_POLYMORPHIC", MACH_MSG_TYPE_POLYMORPHIC, MACH_MSG_TYPE_POLYMORPHIC, PortSize),
    KS(":", syColon),
    KS(";", sySemi),
    KS(",", syComma),
    KS("+", syPlus),
    KS("-", syMinus),
    KS("*", syStar),
    KS("/", syDiv),
    KS("(", syLParen),
    KS(")", syRParen),
    KS("=", syEqual),
    KS("^", syCaret),
    KS("~", syTilde),
    KS("<", syLAngle),
    KS(">", syRAngle),
    KS("[", syLBrack),
    KS("]", syRBrack),
    KS("|", syBar),
};

#define KEYWORDS (sizeof keywords / sizeof keywords[0])

static int state = ST_INITIAL;
static int old_state = ST_INITIAL;
static char *input;
static size_t input_len;
static size_t input_pos;
static int loaded;

extern void yyerror(char *);

static int ascii_alpha(int c)
{
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

static int ascii_digit(int c)
{
    return c >= '0' && c <= '9';
}

static int ident_start(int c)
{
    return ascii_alpha(c) || c == '_';
}

static int ident_char(int c)
{
    return ident_start(c) || ascii_digit(c);
}

static int string_char(int c)
{
    return ident_char(c) || c == '-' || c == '/' || c == '.' || c == '$';
}

static void slurp(void)
{
    size_t cap = 65536;
    char *buf = malloc(cap);

    if (buf == NULL)
        fatal("out of memory");
    for (;;) {
        size_t n;
        if (input_len == cap) {
            char *bigger;
            cap *= 2;
            bigger = realloc(buf, cap);
            if (bigger == NULL)
                fatal("out of memory");
            buf = bigger;
        }
        n = fread(buf + input_len, 1, cap - input_len, stdin);
        if (n == 0)
            break;
        input_len += n;
    }
    input = buf;
    loaded = 1;
}

static char *text_at(size_t at, size_t n)
{
    char *s = malloc(n + 1);

    if (s == NULL)
        fatal("out of memory");
    memcpy(s, input + at, n);
    s[n] = '\0';
    return s;
}

static int fold_match(size_t at, size_t avail, const char *text, int fold, size_t *len)
{
    size_t i;
    size_t n = strlen(text);

    if (avail < n)
        return 0;
    for (i = 0; i < n; i++) {
        int a = (unsigned char)input[at + i];
        int b = (unsigned char)text[i];
        if (fold) {
            if (a >= 'A' && a <= 'Z')
                a += 'a' - 'A';
            if (b >= 'A' && b <= 'Z')
                b += 'a' - 'A';
        }
        if (a != b)
            return 0;
    }
    *len = n;
    return 1;
}

static size_t quoted_len(size_t at, size_t avail, int open, int close)
{
    size_t i;

    if (input[at] != open)
        return 0;
    for (i = 1; i < avail; i++) {
        if (input[at + i] == '\n')
            return 0;
        if (input[at + i] == close)
            return i + 1;
    }
    return 0;
}

static size_t sharp_match(size_t at, size_t avail, int *has_name)
{
    size_t i;
    size_t start;

    *has_name = 0;
    i = 1;
    while (i < avail && (input[at + i] == ' ' || input[at + i] == '\t'))
        i++;
    start = i;
    while (i < avail && ascii_digit((unsigned char)input[at + i]))
        i++;
    if (i > start) {
        size_t q = i;
        while (q < avail && (input[at + q] == ' ' || input[at + q] == '\t'))
            q++;
        if (q < avail && input[at + q] == '"') {
            size_t end = q + 1;
            while (end < avail && input[at + end] != '"')
                end++;
            if (end < avail) {
                *has_name = 1;
                return end + 1;
            }
        }
    }

    i = 1;
    while (i < avail && input[at + i] == ' ')
        i++;
    start = i;
    while (i < avail && ascii_digit((unsigned char)input[at + i]))
        i++;
    if (i > start)
        return i;

    if (avail >= 7 && strncmp(input + at, "#pragma", 7) == 0)
        return 7;
    return 1;
}

static void sharp_directive(size_t at, size_t n, int has_name)
{
    char *body = text_at(at, n);
    char *start;
    char *end;

    lineno = atoi(body);
    if (has_name) {
        start = strchr(body, '"');
        end = strrchr(body, '"');
        if (start != NULL && end != NULL && end > start) {
            *end = '\0';
            strfree(yyinname);
            yyinname = strmake(start + 1);
        }
    }
    free(body);
}

static int keyword_result(const struct keyword *kw)
{
    switch (kw->action) {
    case KW_RETURN:
        return kw->token;
    case KW_FLAG:
        yylval.flag = kw->flag;
        return syIPCFlag;
    case KW_TP:
        yylval.symtype.innumber = kw->in_number;
        yylval.symtype.instr = kw->in_str;
        yylval.symtype.outnumber = kw->out_number;
        yylval.symtype.outstr = kw->out_str;
        yylval.symtype.size = kw->size;
        return sySymbolicType;
    }
    return syError;
}

static void enter(int new_state)
{
    old_state = new_state;
    state = new_state;
}

static void restore_normal(void)
{
    input_pos++;
    enter(ST_NORMAL);
}

int yylex(void)
{
    if (!loaded)
        slurp();
    for (;;) {
        size_t avail = input_len - input_pos;
        size_t len = 0;
        char c;

        if (avail == 0)
            return 0;
        c = input[input_pos];

        if (state == ST_SKIPTEOL) {
            input_pos++;
            if (c == '\n') {
                lineno++;
                state = old_state;
            }
            continue;
        }

        if ((input_pos == 0 || input[input_pos - 1] == '\n') && c == '#') {
            int has_name;
            len = sharp_match(input_pos, avail, &has_name);
            sharp_directive(input_pos + 1, len - 1, has_name);
            input_pos += len;
            state = ST_SKIPTEOL;
            continue;
        }

        if (c == ' ' || c == '\t') {
            input_pos++;
            continue;
        }
        if (c == '\n') {
            input_pos++;
            lineno++;
            continue;
        }

        if (state == ST_STRING) {
            if (string_char((unsigned char)c)) {
                len = 1;
                while (len < avail && string_char((unsigned char)input[input_pos + len]))
                    len++;
                yylval.string = text_at(input_pos, len);
                input_pos += len;
                enter(ST_NORMAL);
                return syString;
            }
            restore_normal();
            return syError;
        }

        if (state == ST_FILENAME) {
            len = quoted_len(input_pos, avail, '"', '"');
            if (len == 0)
                len = quoted_len(input_pos, avail, '<', '>');
            if (len != 0) {
                yylval.string = text_at(input_pos, len);
                input_pos += len;
                enter(ST_NORMAL);
                return syFileName;
            }
            restore_normal();
            return syError;
        }

        if (state == ST_QSTRING) {
            len = quoted_len(input_pos, avail, '"', '"');
            if (len != 0) {
                yylval.string = text_at(input_pos, len);
                input_pos += len;
                enter(ST_NORMAL);
                return syQString;
            }
            restore_normal();
            return syError;
        }

        if (state == ST_NORMAL) {
            const struct keyword *kw = NULL;
            int kind = 0;
            size_t i;

            for (i = 0; i < KEYWORDS; i++) {
                size_t m;
                if (fold_match(input_pos, avail, keywords[i].text, keywords[i].fold, &m) &&
                    m > len) {
                    len = m;
                    kw = &keywords[i];
                    kind = 1;
                }
            }
            if (ident_start((unsigned char)c)) {
                size_t m = 1;
                while (m < avail && ident_char((unsigned char)input[input_pos + m]))
                    m++;
                if (m > len) {
                    len = m;
                    kind = 2;
                }
            }
            else if (ascii_digit((unsigned char)c)) {
                size_t m = 1;
                while (m < avail && ascii_digit((unsigned char)input[input_pos + m]))
                    m++;
                if (m > len) {
                    len = m;
                    kind = 3;
                }
            }

            if (kind == 1) {
                input_pos += len;
                return keyword_result(kw);
            }
            if (kind == 2) {
                yylval.identifier = text_at(input_pos, len);
                input_pos += len;
                return syIdentifier;
            }
            if (kind == 3) {
                char *text = text_at(input_pos, len);
                yylval.number = atoi(text);
                free(text);
                input_pos += len;
                return syNumber;
            }
        }

        restore_normal();
        return syError;
    }
}

void LookNormal(void)
{
    enter(ST_NORMAL);
}

void LookString(void)
{
    enter(ST_STRING);
}

void LookQString(void)
{
    enter(ST_QSTRING);
}

void LookFileName(void)
{
    enter(ST_FILENAME);
}
