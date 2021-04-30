#include <stdio.h>

#if defined(_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // VirtualAlloc,VirtualFree,GetSystemInfo

#define win_error_handle(expr,text) (!(expr) ? fprintf(stderr,"%s\nError code:%lu",text,GetLastError()): 0)

size_t system_page_size(){
    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);
    return systemInfo.dwPageSize;
}

void* os_alloc_page(){
    void* ptr = VirtualAlloc(NULL,1,MEM_COMMIT | MEM_RESERVE,PAGE_READWRITE);
    win_error_handle(ptr,"Page allocation failed");
    return ptr;
}

void* os_alloc_large(size_t size){
    void* ptr = VirtualAlloc(NULL,size,MEM_COMMIT | MEM_RESERVE,PAGE_READWRITE);
    win_error_handle(ptr,"Large allocation failed");
    return ptr;
}

void os_free(void* addr,size_t size){
    int failed = VirtualFree(addr,size,MEM_RELEASE);
    win_error_handle(failed,"Memory releasing failed");
}

#elif defined(__linux__)
...
#elif defined(__APPLE__)
#endif