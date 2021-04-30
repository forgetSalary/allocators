#ifndef ALLOCS_BUDDY_ALLOC_H
#define ALLOCS_BUDDY_ALLOC_H

#include <stdint.h>
typedef uint32_t hHeap;

hHeap buddy_heap_create();

void* buddy_heap_alloc(hHeap arena_handle, size_t size);
void* buddy_heap_realloc(hHeap arena_handle, void* ptr, size_t new_size);
void buddy_heap_free(hHeap arena_handle, void* ptr);

void buddy_heap_stat(hHeap heap_handle);
void buddy_alloc_stat();

#endif //ALLOCS_BUDDY_ALLOC_H


