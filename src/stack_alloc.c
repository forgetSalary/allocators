#include "../include/stack_alloc.h"

void* linear_alloc(mem_buf_t* buf,size_t size){
    if (buf && buf->mem && (size <= buf->totalSize - buf->bottom_offset)){
        void* ptr = buf->mem + buf->bottom_offset;
        buf->bottom_offset+=size;
        return ptr;
    }
    return NULL;
}

void* stack_alloc(mem_buf_t* buf,size_t size,StackHandle* handle,stack_offset_side side){
    if (buf){
        uint32_t top_offset = buf->top_offset, bottom_offset = buf->top_offset, total_size = buf->totalSize;
        uint8_t* mem = buf->mem;
        if (mem && size && (size <= total_size - bottom_offset - top_offset)){
            if (side == bottom){
                *handle = bottom_offset;
                void* ptr = mem + bottom_offset;
                buf->bottom_offset+=size;
                return ptr;
            }
            else if (side == top){
                *handle = -(top_offset);
                void* ptr = mem + total_size - top_offset;
                buf->top_offset+=size;
                return ptr;
            }
            else{
                return NULL;
            }
        }
    }
    return NULL;
}

void stack_set_offset(mem_buf_t* buf, StackHandle handle){
    if (buf && handle && handle<buf->bottom_offset){
        if (handle>0){
            buf->bottom_offset = handle;
        }
        else{
            buf->top_offset = (uint32_t)(-handle);
        }
    }
}