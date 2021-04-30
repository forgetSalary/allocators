#include "../include/fit_alloc.h"

static chunk_t* divide_chunk(heap* heap, chunk_t** chunk, chunk_t** last, size_t user_size){
    //удаляем найденный блок из списка свободных блоков
    //если он идеально подошел (размер равен запрашиваемому
    //или в нем нет места для мета-данных нового блока)
    if ((*chunk)->size < user_size+chunk_meta_sz){
        if (last){
            *last = (*chunk)->next;
        }
        else{
            heap->free_list = (*chunk)->next;
        }

        return *chunk;
    }
        //"отрезаем" блок нужного нам размера
    else{
        (*chunk)->size -= user_size+chunk_meta_sz;
        chunk_t* to_alloc = ((*chunk)+chunk_meta_sz+(*chunk)->size);
        to_alloc->size = user_size;
        to_alloc->next = NULL;

        return (*chunk)->next;
    }
}

void* first_fit_alloc(heap* heap, size_t size){
    if (heap){
        if (!size){
            heap->last_error = null_size;
            return NULL;
        }

        //проверка на минимальные размер блока
        if(size<min_chunk_size){
            size = min_chunk_size;
        }

        //блок должен уместиться вместе с мета-данными
        if (heap->free_size - (size+chunk_meta_sz)  < 0){
            heap->last_error = heap_full;
            return NULL;
        }

        //поиск блока в free_list
        if (heap->free_list){
            chunk_t** last = NULL;
            chunk_t** next = &heap->free_list;

            //first_fit
            while (*next){
                if ((*next)->size >= size){
                    break;
                }
                last = next;
                next=&((*next)->next);
            }

            if (*next){
                chunk_t* first_fit = divide_chunk(heap,next,last,size);
                heap->last_error = none;
                return chunk_to_data(first_fit);
            }
        }

        //if not found then alloc form the top
        chunk_t* chunk = (chunk_t*)heap->top;
        chunk->size = size;
        heap->top += chunk_meta_sz + size;
        chunk->next = (chunk_t*)(heap->top);
        heap->free_size -= size+chunk_meta_sz;

        heap->last_error = none;
        return chunk_to_data(chunk);
    }
    return NULL;
}

void* best_fit_alloc(heap* heap, size_t size){
    if (heap){
        if (!size){
            heap->last_error = null_size;
            return NULL;
        }

        //проверка на минимальные размер блока
        if(size<min_chunk_size){
            size = min_chunk_size;
        }

        //блок должен уместиться вместе с мета-данными
        if (heap->free_size - (size+chunk_meta_sz)  < 0){
            heap->last_error = heap_full;
            return NULL;
        }

        //поиск блока в free_list
        if (heap->free_list){
            chunk_t** last = NULL;
            chunk_t** next = &heap->free_list;

            chunk_t** best = NULL;
            size_t best_size = SIZE_MAX;

            //best fit
            while (*next){
                if ((*next)->size >= size && (*next)->size < best_size){
                    best = next;
                    best_size = (*next)->size;
                    if (best_size == size){
                        break;
                    }
                }
                last = next;
                next=&((*next)->next);
            }
            if (*next){
                chunk_t* best_fit = divide_chunk(heap,best,last,size);
                heap->last_error = none;
                return chunk_to_data(best_fit);
            }

        }
        //выделяем новый блок из кучи
        chunk_t* chunk = (chunk_t*)heap->top;
        chunk->size = size;
        heap->top += chunk_meta_sz + size;
        chunk->next = (chunk_t*)(heap->top);
        heap->free_size -= size+chunk_meta_sz;

        heap->last_error = none;
        return chunk_to_data(chunk);
    }
    return NULL;
}

void fit_free(heap* heap, void* ptr){
    if(ptr){
        chunk_t* chunk = data_to_chunk(ptr);
        chunk->next = heap->free_list;
        heap->free_size += chunk->size;
        heap->free_list = chunk;
    }
}