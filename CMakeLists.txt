cmake_minimum_required(VERSION 3.17)
project(allocs C)

set(CMAKE_C_STANDARD 99)

add_executable(allocs include/arena.h include/buddy_alloc.h include/common.h include/rbtrees.h include/os.h src/main.c src/stack_alloc.c src/pool_alloc.c src/first_best_fit.c src/mem_man.c src/os.c src/rbtrees.c src/buddy_alloc.c src/arena.c)