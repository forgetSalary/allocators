#ifndef ALLOCS_BITMAP_H
#define ALLOCS_BITMAP_H

#include "common.h"

typedef uintptr_t   bitmap_t;
typedef uint32_t     bmp_idx_t;

#define BMP_SIZE (sizeof(bitmap_t)*8)
#define BMP_MASK ((bitmap_t)INTPTR_MIN)

#define BITMAP_EMPTY ((bitmap_t)0)
#define BITMAP_FULL ((bitmap_t)UINTPTR_MAX)

inline bool bitmap_scan(bitmap_t bmp, bmp_idx_t idx){
    return ((bmp<<(BMP_SIZE-1-idx)&BMP_MASK) ? 1 : 0);
}

inline void bitmap_set(bitmap_t* bmp, bmp_idx_t idx){
    *bmp |= (1<<idx);
}

inline void bitmap_reset(bitmap_t* bmp, bmp_idx_t idx){
    *bmp ^= (1<<idx);
}

inline void bitmap_clear_bit(bitmap_t* bmp, bmp_idx_t idx){
    *bmp &= (BITMAP_FULL^(1<<idx));
}

#endif //ALLOCS_BITMAP_H
