#include "../include/buddy_alloc.h"
#include "buddy_math.c"
#include "buddy_internals.c"

hHeap buddy_heap_create(){
    hHeap handle = ALLOC_META.heaps_len++;
    xassert(handle<HEAPS_COUNT,"Heaps out of range");

    heap_t* heap = &ALLOC_META.heaps[handle];
    heap->default_pages.head = heap->default_pages.tail = NULL;
    heap->blocks = (rb_tree){NIL};
    memset(heap->bins,0,sizeof(heap->bins));

    return handle;
}

static void blocks_tree_walk_print_meta(rb_node* node){
    if(node){
        block_t* block = node_value(block_t*,node);
        blocks_tree_walk_print_meta(node->rb_left);
        void* block_ptr = node->item.key;
        void* page_ptr = block->binNode->page->ptr;
        size_t size = block->size;
        if (!block_get_state(block_ptr,page_ptr,block->binNode->page->bitmaps)){
            printf("|0x%p\t|0x%p\t|%d\t|\n",page_ptr,block_ptr,size);
        }
        blocks_tree_walk_print_meta(node->rb_right);
    }
}

void buddy_alloc_stat(){
    ptr_list list = ALLOC_META.meta_info.regions;
    int count = 0;
    for_each_node(ptr_node,it,list){
        count++;
    }
    size_t last_page_storage = ALLOC_META.meta_info.end - ALLOC_META.meta_info.ptr;
    printf("OVERHEAD: %u bytes",last_page_storage+count*DEFAULT_PAGE_SIZE);

}

void buddy_heap_stat(hHeap heap_handle){
    xassert(heap_handle < HEAPS_COUNT && heap_handle < ALLOC_META.heaps_len, "Invalid heap");
    heap_t* heap = &ALLOC_META.heaps[heap_handle];

    printf("Heap %d\n"
                   "BINS:\n"
                   "|Size class\t|Blocks count\t|\n",heap_handle+1);
    for (int j = 0; j < BINS_COUNT; ++j) {
        printf("|%d\t\t|%d\t\t|\n",MIN_ALLOC_SIZE<<j,heap->bins[j].count);
    }
    printf("\nDATA:\n"
                    "|PAGE\t\t|PTR\t\t|SIZE\t|\n");
    blocks_tree_walk_print_meta(heap->blocks.root);
    printf("\n");
}

void* buddy_heap_alloc(hHeap heap_handle, size_t size){
    xassert(heap_handle < HEAPS_COUNT && heap_handle < ALLOC_META.heaps_len, "Invalid heap");
    heap_t* heap = &ALLOC_META.heaps[heap_handle];

    // используем buddy system только для блоков в переделах одной страниы
    if (size <= DEFAULT_PAGE_SIZE){
        if (size < MIN_ALLOC_SIZE) size = MIN_ALLOC_SIZE;
        else size = next_power_of_2(size);

        //проверяем соответсвующую корзину
        int sizes_power = log_2(size);
        bin_node* found_block = heap->bins[sizes_power - 2].head;

        int found_block_size_power = 0;
        if (found_block){
            //извлекаем блок из корзины
            found_block = bin_pop(&heap->bins[sizes_power - 2]);
            block_set_state(found_block,0);
            return found_block->ptr;
        }
        //идеального подходящий блок не найден
        //ищем наименьший из доступных
        else{
            for (int i = sizes_power+1; i < BINS_COUNT && !found_block; ++i) {
                found_block = bin_pop(&heap->bins[i]);
                found_block_size_power = i+2;
            }
        }
        //если найден, делим блок по полам до тех пор,
        //пока не достигнем нужного нам размера,
        //(т.к. размер уже выравнен до минимального, цикл конечен)
        if(found_block){
            size_t found_block_size = pow_2(found_block_size_power);
            size_t division_count = found_block_size_power - sizes_power;

            bin_node* new_half = NULL;
            bin_node* prev_half = found_block;

            for (int i = 0; i < division_count; ++i) {
                found_block_size >>= 1;

                //создаем новый эелемент для корзины
                new_half = get_bin_node();
                new_half->ptr = (uint8_t*)prev_half->ptr+found_block_size;
                new_half->page = prev_half->page;
                block_set_state(new_half,1);//помечаем, как свободный

                //добавляем эллемент в корзину
                bin_push(&heap->bins[found_block_size_power - 3 - i],new_half);
                //добавляем информацию о блоке в двоичное дерево (сортируя по указателям)
                insert_block_to_tree(&heap->blocks,new_half,found_block_size);
            }
            //изменеяем информацию о размере исходного блока
            node_value(block_t*,RBT_search(&heap->blocks,prev_half->ptr))->size = found_block_size;

            //помечаем, как занятый
            block_set_state(prev_half,0);

            return prev_half->ptr;
        }
        //все корзины пусты
        //выделяем чистую страницу памяти
        else{
            void* page = _heap_grow(heap, size);
            bin_t* page_bin = &heap->bins[BINS_COUNT - 1];
            page_bin->head = get_bin_node();

            //помещаем страницу в самую верхню корзину
            page_bin->head->page = heap->default_pages.tail;
            page_bin->head->ptr = page;

            page_bin->count = 1;
            page_bin->head->next = page_bin->head->last = NULL;

            block_set_state(page_bin->head,1);
            insert_block_to_tree(&heap->blocks,page_bin->head,DEFAULT_PAGE_SIZE);

            //повторяем цикл еще раз, с гарантией находа свободного блока в корзине
            return buddy_heap_alloc(heap_handle, size);
        }
    }
    //блоки больше страницы храняться в отдельном списке
    else{
        return _heap_grow(heap, size);
    }
}

void buddy_heap_free(hHeap heap_handle, void* ptr){
    xassert(heap_handle < HEAPS_COUNT && heap_handle < ALLOC_META.heaps_len, "Invalid heap");
    heap_t* heap = &ALLOC_META.heaps[heap_handle];

    //находим блок в дереве
    rb_node* _rb_node = RBT_search(&heap->blocks,ptr);
    rb_node* _rb_neighbour_node = NULL;

    block_t* block = ((block_t*)_rb_node->item.value);
    bin_node* block_node = ((block_t*)_rb_node->item.value)->binNode;
    block_set_state(block_node,1);

    //если размер меньше страницы, пытаемся произвести консолидацию с соседними блоками
    if (block->size <= DEFAULT_PAGE_SIZE){
        int buddy_size = (int)block->size;
        for(;;){
            //определяем с какой стороны в должен быть сосесдний блок того же размера
            buddy_grow_dir dir = block_grow_direction(block_node->ptr,block->size,block_node->page->ptr);
            if(dir == buddy_left){
                buddy_size = ~(buddy_size - 1);
            }
            //если соседний блок свободен
            if(block_get_state((uint8_t*)block_node->ptr+buddy_size,block_node->page->ptr,block_node->page->bitmaps)){
                buddy_size = block->size;

                //находим нужный блок в дереве
                _rb_neighbour_node = RBT_search(&heap->blocks,(uint8_t*)ptr+buddy_size);
                block_t* neighbour_block = ((block_t*)_rb_neighbour_node->item.value);
                bin_node* neighbour_block_node = ((block_t*)_rb_neighbour_node->item.value)->binNode;

                //удаляем информацию о блоках
                release_node(BIN_NODES_POOL,neighbour_block_node);
                delete_node(&heap->bins[log_2(buddy_size) - 2], neighbour_block_node);

                release_node(USED_BLOCKS_POOL,(block_tree_node*)_rb_neighbour_node);
                RBT_delete(&heap->blocks,_rb_neighbour_node);

                //додавляем блок в корзину выше
                block->size <<= 1;
                bin_push(&heap->bins[log_2(buddy_size)-1],block_node);

                buddy_size = block->size;
            }else{
                break;
            }
        }
    }else{
        //просто возвращаем блок обратно ОС
        large_page_node* page = find_large_page(heap->large_pages,ptr);
        delete_node(&heap->large_pages,page);
        os_free(page->ptr,page->size);
    }
}