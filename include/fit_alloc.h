#ifndef ALLOCS_FIT_ALLOC_H
#define ALLOCS_FIT_ALLOC_H
#include <stddef.h>
#include <stdint.h>

//данные аллокатора
typedef struct chunk{
    size_t size;
    struct chunk* next;
}chunk_t;

#define min_chunk_size 4 //минимальные размер в байтах выделяемого блока (необязательно)
#define chunk_meta_sz sizeof(chunk_t)//размер служебных данных блока

//сдвигаем увказатель с мета-данных к началу данных пользователя
#define chunk_to_data(chunk_ptr) (void*)((char*)(chunk_ptr) + chunk_meta_sz)

//мета-данные указанного блока
#define data_to_chunk(ptr) (chunk_t*)((char*)(ptr) - chunk_meta_sz)

//ошибки выделения
typedef enum alloc_error{
    invalid_heap = 0,
    none,
    heap_full,
    null_size,
}alloc_error;

//буффер (прототип кучи)
typedef struct heap{
    uint8_t* top;
    chunk_t* free_list;
    int64_t free_size;
    size_t size;
    alloc_error last_error;
}heap;

void* first_fit_alloc(heap* heap, size_t size);

void* best_fit_alloc(heap* heap, size_t size);

void fit_free(heap* heap, void* ptr);

#endif //ALLOCS_FIT_ALLOC_H

