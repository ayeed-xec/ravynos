// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 The ravynOS Authors
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STRIP 64

struct line {
    const char *p;
    size_t n;
    int nl;
};

struct buf {
    char *p;
    size_t len;
    size_t cap;
};

static const char *patch_path;

static void die(const char *msg, const char *detail)
{
    fprintf(stderr, "rvpatch: %s%s%s\n", msg, detail ? ": " : "", detail ? detail : "");
    exit(1);
}

static void *xmalloc(size_t n)
{
    void *p = malloc(n ? n : 1);
    if (p == NULL)
        die("out of memory", NULL);
    return p;
}

static void grow(struct buf *b, size_t need)
{
    if (b->len + need <= b->cap)
        return;
    size_t cap = b->cap ? b->cap : 4096;
    while (cap < b->len + need)
        cap *= 2;
    char *p = realloc(b->p, cap);
    if (p == NULL)
        die("out of memory", NULL);
    b->p = p;
    b->cap = cap;
}

static void put(struct buf *b, const char *s, size_t n, int nl)
{
    grow(b, n + 1);
    memcpy(b->p + b->len, s, n);
    b->len += n;
    if (nl)
        b->p[b->len++] = '\n';
}

static char *read_file(const char *path, size_t *len)
{
    FILE *f = fopen(path, "rb");
    if (f == NULL)
        die("cannot open", path);
    if (fseek(f, 0, SEEK_END) != 0)
        die("cannot seek", path);
    long sz = ftell(f);
    if (sz < 0 || fseek(f, 0, SEEK_SET) != 0)
        die("cannot size", path);
    char *p = xmalloc((size_t)sz + 1);
    if (sz > 0 && fread(p, 1, (size_t)sz, f) != (size_t)sz)
        die("cannot read", path);
    p[sz] = '\0';
    fclose(f);
    *len = (size_t)sz;
    return p;
}

static struct line *split(char *p, size_t len, size_t *count)
{
    size_t cap = 64;
    struct line *l = xmalloc(cap * sizeof(*l));
    size_t n = 0;
    size_t i = 0;
    while (i < len) {
        size_t start = i;
        while (i < len && p[i] != '\n')
            i++;
        if (n == cap) {
            cap *= 2;
            l = realloc(l, cap * sizeof(*l));
            if (l == NULL)
                die("out of memory", NULL);
        }
        l[n].p = p + start;
        l[n].n = i - start;
        l[n].nl = i < len && p[i] == '\n';
        n++;
        if (i < len)
            i++;
    }
    *count = n;
    return l;
}

static int starts(const struct line *l, const char *prefix, size_t plen)
{
    return l->n >= plen && memcmp(l->p, prefix, plen) == 0;
}

static char *header_path(const struct line *l, size_t skip)
{
    size_t len = l->n - skip;
    const char *s = l->p + skip;
    const char *tab = memchr(s, '\t', len);
    if (tab != NULL)
        len = (size_t)(tab - s);
    if (len == 0)
        die("empty file name in patch header", patch_path);
    if (memchr(s, '\0', len) != NULL)
        die("NUL byte in patch header", patch_path);
    char *out = xmalloc(len + 1);
    memcpy(out, s, len);
    out[len] = '\0';
    return out;
}

static const char *strip_components(const char *path, long p)
{
    const char *s = path;
    for (long i = 0; i < p; i++) {
        const char *slash = strchr(s, '/');
        if (slash == NULL)
            die("too few path components", path);
        s = slash + 1;
    }
    return s;
}

static int is_safe_relative_path(const char *path)
{
    if (path[0] == '\0' || path[0] == '/')
        return 0;
    const char *s = path;
    while (*s != '\0') {
        const char *slash = strchr(s, '/');
        size_t n = slash ? (size_t)(slash - s) : strlen(s);
        if (n == 0 || (n == 2 && s[0] == '.' && s[1] == '.'))
            return 0;
        if (slash == NULL)
            break;
        s = slash + 1;
    }
    return 1;
}

static long parse_strip(const char *s)
{
    char *end = NULL;
    long v = strtol(s, &end, 10);
    if (*s == '\0' || *end != '\0' || v < 0 || v > MAX_STRIP)
        die("bad -p value", s);
    return v;
}

static size_t parse_range(const char *s, size_t *value, size_t *count)
{
    size_t venue = 0;
    while (s[venue] >= '0' && s[venue] <= '9')
        venue++;
    if (venue == 0)
        die("malformed hunk header", patch_path);
    *value = strtoul(s, NULL, 10);
    *count = 1;
    s += venue;
    if (*s == ',') {
        s++;
        if (!(*s >= '0' && *s <= '9'))
            die("malformed hunk header", patch_path);
        *count = strtoul(s, NULL, 10);
    }
    return venue;
}

static void apply_file(struct line *pl, size_t plines, size_t *idx, long pcomps)
{
    size_t i = *idx;
    char *header = header_path(&pl[i + 1], 4);
    const char *target = strip_components(header, pcomps);
    if (!is_safe_relative_path(target))
        die("refusing path outside the tree", target);
    i += 2;

    size_t flen = 0;
    char *fbuf = read_file(target, &flen);
    size_t flines = 0;
    struct line *file = split(fbuf, flen, &flines);

    struct buf out = {0};
    size_t cursor = 0;
    size_t hunks = 0;

    while (i < plines && !starts(&pl[i], "--- ", 4)) {
        if (!starts(&pl[i], "@@ ", 3)) {
            i++;
            continue;
        }
        const char *h = pl[i].p + 3;
        size_t old_start, old_count, new_start, new_count;
        if (*h != '-')
            die("malformed hunk header", patch_path);
        h++;
        parse_range(h, &old_start, &old_count);
        while (*h != ' ' && *h != '\0')
            h++;
        if (*h != ' ')
            die("malformed hunk header", patch_path);
        h++;
        if (*h != '+')
            die("malformed hunk header", patch_path);
        h++;
        parse_range(h, &new_start, &new_count);
        (void)new_start;
        i++;

        if (old_start == 0)
            old_start = 1;
        if (old_start - 1 < cursor)
            die("overlapping hunks", patch_path);
        while (cursor < old_start - 1 && cursor < flines)
            put(&out, file[cursor].p, file[cursor].n, file[cursor].nl), cursor++;

        size_t body_old = 0, body_new = 0;
        while (i < plines && pl[i].n > 0 && pl[i].p[0] != '@' && !starts(&pl[i], "--- ", 4)) {
            char tag = pl[i].p[0];
            const char *text = pl[i].p + 1;
            size_t tlen = pl[i].n - 1;
            if (tag == ' ') {
                if (cursor >= flines || file[cursor].n != tlen ||
                    memcmp(file[cursor].p, text, tlen) != 0)
                    die("context mismatch", target);
                put(&out, file[cursor].p, file[cursor].n, file[cursor].nl);
                cursor++;
                body_old++;
                body_new++;
            }
            else if (tag == '-') {
                if (cursor >= flines || file[cursor].n != tlen ||
                    memcmp(file[cursor].p, text, tlen) != 0)
                    die("removed line not found", target);
                cursor++;
                body_old++;
            }
            else if (tag == '+') {
                put(&out, text, tlen, 1);
                body_new++;
            }
            else {
                break;
            }
            i++;
        }
        if (body_old != old_count)
            die("hunk line count mismatch", target);
        if (body_new != new_count)
            die("hunk line count mismatch", target);
        hunks++;
    }
    if (hunks == 0)
        die("patch has no hunks for", target);

    while (cursor < flines)
        put(&out, file[cursor].p, file[cursor].n, file[cursor].nl), cursor++;

    FILE *o = fopen(target, "wb");
    if (o == NULL)
        die("cannot write", target);
    if (out.len > 0 && fwrite(out.p, 1, out.len, o) != out.len)
        die("cannot write", target);
    if (fclose(o) != 0)
        die("cannot close", target);

    printf("patching file %s\n", target);
    free(out.p);
    free(file);
    free(fbuf);
    free(header);
    *idx = i > 0 ? i - 1 : 0;
}

int main(int argc, char **argv)
{
    long pcomps = 1;
    const char *file = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0) {
            if (i + 1 >= argc)
                die("-p needs a number", NULL);
            pcomps = parse_strip(argv[++i]);
        }
        else if (strncmp(argv[i], "-p", 2) == 0) {
            pcomps = parse_strip(argv[i] + 2);
        }
        else if (argv[i][0] == '-' && argv[i][1] != '\0') {
            fprintf(stderr, "usage: rvpatch [-p components] patch-file\n");
            return 2;
        }
        else {
            file = argv[i];
        }
    }
    if (file == NULL) {
        fprintf(stderr, "usage: rvpatch [-p components] patch-file\n");
        return 2;
    }
    patch_path = file;

    size_t plen = 0;
    char *pbuf = read_file(file, &plen);
    size_t plines = 0;
    struct line *pl = split(pbuf, plen, &plines);

    size_t applied = 0;
    for (size_t i = 0; i < plines; i++) {
        if (starts(&pl[i], "--- ", 4) && i + 1 < plines && starts(&pl[i + 1], "+++ ", 4)) {
            apply_file(pl, plines, &i, pcomps);
            applied++;
        }
    }
    if (applied == 0)
        die("no file headers found in patch", file);

    free(pl);
    free(pbuf);
    return 0;
}
