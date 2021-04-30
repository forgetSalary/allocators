#ifndef ALLOCS_STACK_ALLOC_H
#define ALLOCS_STACK_ALLOC_H

#include <stddef.h>
#include <stdint.h>

typedef struct Buffer {
    uint8_t *mem;
    uint32_t totalSize;
    uint32_t bottom_offset;
    uint32_t top_offset;
}mem_buf_t;

void* linear_alloc(mem_buf_t* buf,size_t size);
#define linear_free(buf) (buf).offset = 0;

typedef int32_t StackHandle;
typedef enum double_stack_offset{bottom = 0,top = 1} stack_offset_side;

void* stack_alloc(mem_buf_t* buf,size_t size,StackHandle* handle,stack_offset_side side);
void stack_set_offset(mem_buf_t* buf, StackHandle handle);

#endif //ALLOCS_STACK_ALLOC_H
