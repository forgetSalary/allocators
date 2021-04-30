#include "../include/pool_alloc.h"

int pool_int(pool_t* pool,uint8_t* buffer, size_t buffer_size,size_t block_size){
    if (buffer && buffer_size && block_size){
        int blocks_count = buffer_size/(block_size + pool_meta);
        pool->head = buffer;
        struct pool_free_list_s* next_block = pool->head;
        size_t offset = 0;
        size_t step = block_size + sizeof(pool_t);
        for (size_t i = 0; i < blocks_count; ++i) {
            offset += step;
            next_block->next = (buffer + offset);
            next_block = next_block->next;
        }
        return 0;
    } else{
        return 1;
    }
}

void* pool_alloc(pool_t* pool){
    if(pool){
        if(!pool->head){
            return NULL;//out of memory
        }
        void* ptr = (uint8_t*)pool->head + pool_meta;
        if(pool->head->next){
            pool->head = pool->head->next;
        }else{
            pool->head = NULL;
        }

        return ptr;
    }
}

void pool_free(pool_t* pool,void* block_ptr){
    if (pool && block_ptr){
        struct pool_free_list_s* node = (uint8_t*)block_ptr-pool_meta;
        node->next = pool->head;
        pool->head = node;
    }
}