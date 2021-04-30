#ifndef ALLOCS_POOL_ALLOC_H
#define ALLOCS_POOL_ALLOC_H


#include <stddef.h>
#include <stdint.h>

struct pool_free_list_s{
    struct pool_free_list_s* next;
};

typedef struct pool_list{
    struct pool_free_list_s* head;
}pool_t;
#define pool_meta sizeof(struct pool_free_list_s)

int pool_int(pool_t* pool,uint8_t* buffer, size_t buffer_size,size_t block_size);

void* pool_alloc(pool_t* pool);

void pool_free(pool_t* pool,void* block_ptr);

#endif //ALLOCS_POOL_ALLOC_H
