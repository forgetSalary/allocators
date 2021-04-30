#include "../include/arena.h"
#include "../include/os.h"

size_t page_size = 0;
#define define_page_size (((!page_size) ? page_size = system_page_size() : 0),xassert(page_size,"Invalid page size"))

static void arena_grow(arena_t* arena, size_t min_size) {
    define_page_size;
    size_t size = ALIGN_UP(MAX(page_size, min_size), page_size);
    arena->ptr = os_alloc_page();
    memset(arena->ptr,0,page_size);
    arena->end = arena->ptr + size;
    list_append(&(arena->regions), (ptr_node*)arena->ptr);
    arena->ptr = ALIGN_UP_PTR(arena->ptr + sizeof(ptr_node), ARENA_ALIGNMENT);
}

void *arena_alloc(arena_t* arena, size_t size) {
    if (size > (size_t)(arena->end - arena->ptr)) {
        arena_grow(arena, size);
        assert(size <= (size_t)(arena->end - arena->ptr));
    }
    void *ptr = arena->ptr;
    arena->ptr = ALIGN_UP_PTR(arena->ptr + size, ARENA_ALIGNMENT);
    assert(arena->ptr <= arena->end);
    assert(ptr == ALIGN_DOWN_PTR(ptr, ARENA_ALIGNMENT));
    return ptr;
}

void arena_free(arena_t* arena) {
    ptr_node* it_ptr = arena->regions.head;
    ptr_node it;
    while(it_ptr != arena->regions.tail){
        it = *it_ptr;
        os_free(it_ptr,0);
        it_ptr = it.next;
    }
    arena->ptr = NULL;
    arena->end = NULL;
    arena->regions.head = NULL;
    arena->regions.tail = NULL;
}
