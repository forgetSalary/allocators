#include "../include/stack_alloc.h"
#include "../include/pool_alloc.h"
#include "../include/fit_alloc.h"
#include "../include/buddy_alloc.h"
#include "../include/arena.h"

#define buff_size 1024*1024
uint8_t buffer[buff_size];

void test_stack_alloc(){
    mem_buf_t buf = {buffer,buff_size,0};
    StackHandle stack;
    char* str = stack_alloc(&buf,16,&stack,bottom);
    memcpy(str,"Hello!",16);
}

void test_pool_alloc(){
    pool_t pool;
    int error = pool_int(&pool,buffer,512,16);
    void* ptrs[8];

    ptrs[0] = pool_alloc(&pool);
    ptrs[1] = pool_alloc(&pool);
    pool_free(&pool,ptrs[0]);
    ptrs[2] = pool_alloc(&pool);
    pool_free(&pool,ptrs[1]);
    ptrs[3] = pool_alloc(&pool);
}

void test_first_fit_alloc(){
    heap main_heap = {buffer, NULL, buff_size, buff_size, none};
    void* ptrs[10];

    ptrs[0] = first_fit_alloc(&main_heap,2);
    fit_free(&main_heap,ptrs[0]);
    ptrs[1] = first_fit_alloc(&main_heap,2);
    ptrs[2] = first_fit_alloc(&main_heap,4);
    ptrs[3] = first_fit_alloc(&main_heap,128);
    ptrs[4] = first_fit_alloc(&main_heap,64);

    fit_free(&main_heap,ptrs[3]);
    fit_free(&main_heap,ptrs[4]);

    ptrs[5] = first_fit_alloc(&main_heap,100);
    ptrs[6] = first_fit_alloc(&main_heap,20);
}

void test_arena_alloc(){
    arena_t arena = EMPTY_ARENA;

    void* p;
    for (int i = 0; i < 10000; ++i) {
        p = arena_alloc(&arena,500);
    }

    arena_free(&arena);
}

void test_buddy_alloc1(){
    void* ptrs[50];

    hHeap heap1 = buddy_heap_create();

    ptrs[0] = buddy_heap_alloc(heap1, 10);
    buddy_heap_free(heap1,ptrs[0]);
    buddy_heap_stat(heap1);
    ptrs[1] = buddy_heap_alloc(heap1, 50);
    ptrs[2] = buddy_heap_alloc(heap1, 150);
    ptrs[3] = buddy_heap_alloc(heap1, 2000);
    buddy_heap_free(heap1,ptrs[2]);
    buddy_heap_stat(heap1);
    ptrs[4] = buddy_heap_alloc(heap1, 500);
    ptrs[5] = buddy_heap_alloc(heap1, 3000);
    buddy_heap_free(heap1,ptrs[5]);
    buddy_heap_stat(heap1);
    ptrs[6] = buddy_heap_alloc(heap1, 3);
    ptrs[7] = buddy_heap_alloc(heap1, 3);
    ptrs[8] = buddy_heap_alloc(heap1, 3);
    ptrs[9] = buddy_heap_alloc(heap1, 3);
    ptrs[10] = buddy_heap_alloc(heap1, 3);
    ptrs[11] = buddy_heap_alloc(heap1, 3);
    ptrs[12] = buddy_heap_alloc(heap1, 3);
    ptrs[13] = buddy_heap_alloc(heap1, 3000);
    ptrs[14] = buddy_heap_alloc(heap1, 200);
    buddy_heap_stat(heap1);
    buddy_alloc_stat();

}

void test_buddy_alloc2(){
    hHeap heap = buddy_heap_create();
    void* p;

    size_t count = 500;
    size_t size = 500;

    for (int i = 0; i < count; ++i) {
        p = buddy_heap_alloc(heap,size);
    }
    buddy_alloc_stat();
}
