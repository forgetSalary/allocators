#ifndef MEM_MAN_H
#define MEM_MAN_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#define MAX(x, y) ((x) >= (y) ? (x) : (y))
#define STD_ALIGN_DOWN(n, a) ((n) & ~((a) - 1))
#define STD_ALIGN_UP(n, a) STD_ALIGN_DOWN((n) + (a) - 1, (a))
#define STD_ALIGN_DOWN_PTR(p, a) ((void *)STD_ALIGN_DOWN((uintptr_t)(p), (a)))
#define STD_ALIGN_UP_PTR(p, a) ((void *)STD_ALIGN_UP((uintptr_t)(p), (a)))

typedef struct Arena {
    char *ptr;
    char *end;
    char **blocks;
} Arena;

#define STD_ARENA_ALIGNMENT 8
#define STD_ARENA_BLOCK_SIZE 1024

typedef struct BufHdr_s{
    size_t len;
    size_t cap;
    char buf[];
}BufHdr;

void* buf__grow(const void* buf, size_t new_len, size_t elem_size);

#define buf__hdr(b) ((BufHdr *)((char *)(b) - offsetof(BufHdr, buf)))

#define buf_len(b) ((b) ? buf__hdr(b)->len : 0)
#define buf_cap(b) ((b) ? buf__hdr(b)->cap : 0)
#define buf_end(b) ((b) + buf_len(b))
#define buf_sizeof(b) ((b) ? buf_len(b)*sizeof(*b) : 0)
#define buf_fits(b,n) (buf_cap(b)-buf_len(b) > (n) ? 1 : 0)

#define buf_free(b) ((b) ? (free(buf__hdr(b)), (b) = NULL) : 0)
#define buf_fit(b, n) ((n) <= buf_cap(b) ? 0 : ((b) = buf__grow((b), (n), sizeof(*(b)))))
#define buf_push(b, ...) (buf_fit((b), 1 + buf_len(b)), (b)[buf__hdr(b)->len++] = (__VA_ARGS__))
#define buf_printf(b, ...) ((b) = buf__printf((b), __VA_ARGS__))
#define buf_clear(b) ((b) ? buf__hdr(b)->len = 0 : 0)

#define empty_arena {NULL,NULL,NULL}

char *buf__printf(char *buf, const char *fmt, ...);
void buf_append(void* _dst,void* _src, size_t elem_size);

void *std_arena_alloc(Arena *arena, size_t size);

void std_arena_free(Arena *arena);

#endif //MEM_MAN_H

