#ifndef ALLOCS_OS_H
#define ALLOCS_OS_H

size_t system_page_size();

void* os_alloc_page();

void* os_alloc_large(size_t size);

void os_free(void* addr,size_t size);

#endif //ALLOCS_OS_H
