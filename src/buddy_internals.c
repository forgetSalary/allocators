#include "../include/buddy_types.h"
#include "../include/os.h"

alloc_meta_buffer ALLOC_META;
size_t SYSTEM_PAGE_SIZE = 0;

#define BIN_NODES_POOL ALLOC_META.bin_nodes_pool
#define USED_BLOCKS_POOL ALLOC_META.tree_node_pool

#define stack_push(stack,node) (xassert(node,"Invalid node"),((node)->next) = (stack), (stack) = (node))
#define stack_pop(stack,node) ((node) = (stack),(stack) = (stack)->next)

#define bin_list_pop(bin,node) (bin)

inline void is_correct_page_size(){
    if(!SYSTEM_PAGE_SIZE){
        SYSTEM_PAGE_SIZE = system_page_size();
        xassert(SYSTEM_PAGE_SIZE == DEFAULT_PAGE_SIZE, "Only "to_string(DEFAULT_PAGE_SIZE)" sized page required");
    }
}

void* meta_grow(size_t size){
    is_correct_page_size();
    return arena_alloc(&ALLOC_META.meta_info,size);
}

#define release_node(POOL,node) (memset(node,0,sizeof(*(node))),stack_push(POOL,node))

inline bin_node* get_bin_node(){
    bin_node* node;
    if (BIN_NODES_POOL){
        stack_pop(BIN_NODES_POOL, node);
        return node;
    }else{
        node = meta_grow(sizeof(bin_node));
        return node;
    }
}

inline block_tree_node* get_used_blocks_node(){
    block_tree_node* node;
    if (USED_BLOCKS_POOL){
        stack_pop(USED_BLOCKS_POOL,node);
        return node;
    }else{
        node = meta_grow(sizeof(block_tree_node));
        return node;
    }
}

inline bin_node* bin_pop(bin_t* bin){
    if(bin->head){
        bin_node* node = bin->head;
        bin->head = node->next;
        if (bin->head) bin->head->last = NULL;
        bin->count--;
        return node;
    }else{
        return NULL;
    }
}

inline void bin_push(bin_t* bin,bin_node* node){
    xassert(node,"Bin push invalid node");
    node->last = NULL;
    node->next = bin->head;
    bin->head = node;
    bin->count ++;
}

#define delete_node(list,node) \
assert(node);\
if((node)->last && (node)->next){\
    (node)->last->next = (node)->next;\
}else if (!(node)->last && (node)->next){\
    (list)->head = (node)->next;\
}else if(node->last && !node->next){\
    (node)->last->next = NULL;\
}else if ((list)->head == node){\
    (list)->head = NULL;\
}

void insert_block_to_tree(rb_tree* tree,bin_node* bin_node,size_t size){
    block_tree_node* node = get_used_blocks_node();

    rb_node* _rb_node = &node->rb_node;
    node->block.binNode = bin_node;
    node->block.size = size;

    node_init(_rb_node,bin_node->ptr,&node->block);
    RBT_insert(tree, _rb_node);
}


void* _heap_grow(heap_t* heap, size_t size){
    is_correct_page_size();

    if (size <= DEFAULT_PAGE_SIZE){
        list_append(&heap->default_pages, (default_page_node*)meta_grow(sizeof(default_page_node)));
        heap->default_pages.tail->ptr = os_alloc_page();
        memset(heap->default_pages.tail->bitmaps, 0, sizeof(bitmap_t) * PAGE_BITMAP_LENGTH);
        return heap->default_pages.tail->ptr;
    }else{
        list_append(&heap->large_pages, (large_page_node*)meta_grow(sizeof(large_page_node)));
        size = ALIGN_UP(size, DEFAULT_PAGE_SIZE);
        heap->large_pages.tail->ptr = os_alloc_large(size);
        heap->large_pages.tail->size = size;
        return heap->large_pages.tail->ptr;
    }
}

ptr_node* find_large_page(large_page_list list,void* ptr){
    for_each_node(large_page_node,node,list){
        if(node->ptr == ptr){
            return node;
        }
    }
    return NULL;
}

buddy_grow_dir block_grow_direction(void* ptr,size_t block_size,void* page){
    uint8_t* block_end = (uint8_t*)ptr+block_size;
    uint8_t* page_end = (uint8_t*)page + DEFAULT_PAGE_SIZE;

    if(block_end>page_end){
        return buddy_left;
    }
    size_t size_after_parent = page_end - block_end;
    if (size_after_parent%(block_size*2)){
        return buddy_right;
    }else{
        return buddy_left;
    }
}

void block_set_state(bin_node* block, bool state){
    unsigned position = (uintptr_t)block->ptr - (uintptr_t)block->page->ptr;
    xassert(!(position%MIN_ALLOC_SIZE),"Invalid block ptr");
    unsigned bytes_in_a_map = (BMP_SIZE*MIN_ALLOC_SIZE);

    unsigned bitmap_idx = position/bytes_in_a_map;
    bmp_idx_t bit_idx = position/MIN_ALLOC_SIZE - (bitmap_idx)*BMP_SIZE;

    if (state){
        bitmap_set(&block->page->bitmaps[bitmap_idx],bit_idx);
    }else{
        bitmap_clear_bit(&block->page->bitmaps[bitmap_idx],bit_idx);
    }
}

bool block_get_state(void* block_ptr,void* page_ptr,bitmap_t* bitmaps){
    unsigned position = (uintptr_t)block_ptr - (uintptr_t)page_ptr;
    xassert(!(position%MIN_ALLOC_SIZE),"Invalid block ptr");
    unsigned bytes_in_a_map = (BMP_SIZE*MIN_ALLOC_SIZE);

    unsigned bitmap_idx = position/bytes_in_a_map;
    bmp_idx_t bit_idx = position/MIN_ALLOC_SIZE - (bitmap_idx)*BMP_SIZE;

    return bitmap_scan(bitmaps[bitmap_idx],bit_idx);
}
