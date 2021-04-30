#include "../include/mem_man.h"

void *xcalloc(size_t num_elems, size_t elem_size) {
    void *ptr = calloc(num_elems, elem_size);
    if (!ptr) {
        perror("xcalloc failed");
        exit(1);
    }
    return ptr;
}

void *xrealloc(void *ptr, size_t num_bytes) {
    ptr = realloc(ptr, num_bytes);
    if (!ptr) {
        perror("xrealloc failed");
        exit(1);
    }
    return ptr;
}

void *xmalloc(size_t num_bytes) {
    void *ptr = malloc(num_bytes);
    if (!ptr) {
        perror("xmalloc failed");
        exit(1);
    }
    return ptr;
}


void *buf__grow(const void *buf, size_t new_len, size_t elem_size) {
    assert(buf_cap(buf) <= (SIZE_MAX - 1)/2);
    size_t new_cap = MAX(16, MAX(1 + 2*buf_cap(buf), new_len));
    assert(new_len <= new_cap);
    assert(new_cap <= (SIZE_MAX - offsetof(BufHdr, buf))/elem_size);
    size_t new_size = offsetof(BufHdr, buf) + new_cap*elem_size;
    BufHdr *new_hdr;
    if (buf) {
        new_hdr = xrealloc(buf__hdr(buf), new_size);
    } else {
        new_hdr = xmalloc(new_size);
        new_hdr->len = 0;
    }
    new_hdr->cap = new_cap;
    return new_hdr->buf;
}


// Arena allocator
void std_arena_grow(Arena *arena, size_t min_size) {
    size_t size = STD_ALIGN_UP(MAX(STD_ARENA_BLOCK_SIZE, min_size), STD_ARENA_ALIGNMENT);
    arena->ptr = xmalloc(size);
    arena->end = arena->ptr + size;
    buf_push(arena->blocks, arena->ptr);
}

void *std_arena_alloc(Arena *arena, size_t size) {
    if (size > (size_t)(arena->end - arena->ptr)) {
        std_arena_grow(arena, size);
        assert(size <= (size_t)(arena->end - arena->ptr));
    }
    void *ptr = arena->ptr;
    arena->ptr = STD_ALIGN_UP_PTR(arena->ptr + size, STD_ARENA_ALIGNMENT);
    assert(arena->ptr <= arena->end);
    assert(ptr == STD_ALIGN_DOWN_PTR(ptr, STD_ARENA_ALIGNMENT));
    return ptr;
}

void std_arena_free(Arena *arena) {
    for (char **it = arena->blocks; it != buf_end(arena->blocks); it++) {
        free(*it);
    }
}


char *buf__printf(char *buf, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    size_t cap = buf_cap(buf) - buf_len(buf);
    size_t n = 1 + vsnprintf(buf_end(buf), cap, fmt, args);
    va_end(args);
    if (n > cap) {
        buf_fit(buf, n + buf_len(buf));
        va_start(args, fmt);
        size_t new_cap = buf_cap(buf) - buf_len(buf);
        n = 1 + vsnprintf(buf_end(buf), new_cap, fmt, args);
        assert(n <= new_cap);
        va_end(args);
    }
    buf__hdr(buf)->len += n - 1;
    return buf;
}

void buf_append(void* _dst,void* _src, size_t elem_size){
    if (_src){
        _dst = buf__grow(_dst,(buf_len(_dst)+buf_len(_src)),elem_size);
        memcpy(((char*)_dst+buf_len(_dst)),_src,(buf_len(_dst)+buf_len(_src)*elem_size));
        buf__hdr(_dst)->len += buf_len(_src);
    }
}