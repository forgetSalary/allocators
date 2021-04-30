#ifndef ALLOCS_BUDDY_TYPES_H
#define ALLOCS_BUDDY_TYPES_H

#include <stdbool.h>
#include "bitmap.h"
#include "arena.h"
#include "rbtrees.h"

//для примера, будем работать только со страницами размером 4кб
#define DEFAULT_PAGE_SIZE 4096
#define MIN_ALLOC_SIZE 4

#define PAGE_BITMAP_LENGTH DEFAULT_PAGE_SIZE/MIN_ALLOC_SIZE/BMP_SIZE

Struct(default_page_node,{
    void* ptr;
    bitmap_t bitmaps[PAGE_BITMAP_LENGTH];
    struct default_page_node* next;
});

Struct(default_page_list,{
    default_page_node* head;
    default_page_node* tail;
});

Struct(large_page_node,{
    void* ptr;
    size_t size;
    struct large_page_node* next;
    struct large_page_node* last;
});

Struct(large_page_list,{
    large_page_node* head;
    large_page_node* tail;
});

Enum(buddy_grow_dir,{
    buddy_right,
    buddy_left
});

Struct(bin_node,{
    void* ptr;
    default_page_node* page;
    struct bin_node* next;
    struct bin_node* last;
});

Struct(block_t,{
    bin_node* binNode;
    size_t size;
});

Struct(bin_t,{
    bin_node* head;
    unsigned count;
});

Struct(block_tree_node,{
    rb_node rb_node;
    block_t block;
    struct block_tree_node* next;
});

#define BINS_COUNT 11
Struct(heap_t,{
    default_page_list default_pages;
    large_page_list large_pages;
    bin_t bins[BINS_COUNT];
    rb_tree blocks;
});

// обычно количество куч ограничивается количеством ядер процессора,
// т.к. каждая куча принадлежит отдельному процессу
// для примера возьмем 32

#define HEAPS_COUNT 32
Struct(alloc_meta_buffer,{
    arena_t meta_info;

    block_tree_node* tree_node_pool;
    bin_node* bin_nodes_pool;

    heap_t heaps[HEAPS_COUNT];
    uint32_t heaps_len;
});

#endif //ALLOCS_BUDDY_TYPES_H
