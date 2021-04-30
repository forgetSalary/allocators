#include "../include/bitmap.h"
#include "../include/rbtrees.h"

#define template_init_random_buffer(T)\
void init_random_buffer_##T(T* buffer,T start,T end){\
    assert(start >= 0);                 \
    for (int i = 0; i < buf_cap(buffer); ++i) {\
        buffer[i] = ((T) rand()%(end-start) + (start));\
    }\
}
template_init_random_buffer(int)
template_init_random_buffer(size_t)
template_init_random_buffer(char)



void test_rb_trees(){
    Arena tree_arena = empty_arena;
    rb_tree tree = {NULL};

    clock_t start,finish;
    double wait_time;
    double sum_time = 0;

    int size = 30;
    int print_count = 15;
    int step = size/print_count;

    int step_count = 0;
    uint64_t n_count = 0;
    uint64_t n_sum = 0;

    start = clock();

#define STR_LEN 16

    char* value = NULL;
    buf_fit(value,STR_LEN+1);

    void* key;
    void** keys = NULL;
    char** values = NULL;

    rb_node* node;
    srand(time(NULL));
    for (int i = 0; i < size; ++i, step_count++) {
        n_count += i;

        init_random_buffer_char(value,'a','z');
        value[STR_LEN-1] = '\0';
        key = (void*)(i);
        node = _alloc_new_node(&tree_arena,key,value,STR_LEN);
        RBT_insert(&tree,node);

        finish = clock();

        if (step_count == step){
            wait_time = ((double)(finish-start)/CLOCKS_PER_SEC);
            buf_push(keys,key);
            buf_push(values,node->item.value);

            //printf("n: %llu; time: %f\n",n_count,wait_time);
            printf("key: %p; value: %s\n",key,(char*)(node->item.value));

            sum_time += wait_time;
            n_sum+=n_count;

            start = clock();
            step_count = 0;
        }
    }

    rb_node** found_nodes = NULL;

    printf("\n");
    for (int i = 0; i < buf_len(keys); ++i){
        buf_push(found_nodes, RBT_search(&tree, keys[i]));
        assert(found_nodes[i] != NULL);
    }
    for (int i = 0; i < buf_len(found_nodes)/2; ++i) {
        RBT_delete(&tree,found_nodes[i]);
    }

    rb_node* found;
    for (int i = 0; i < buf_len(keys); ++i) {
        found = RBT_search(&tree,keys[i]);
        if(found){
            printf("key: %p; found value: %s\n",keys[i],node_value(char*,found));
        }else{
            printf("key: %p; found value: NULL\n",keys[i]);
        }

    }

    printf("\nSUM:\nN: %llu; Time: %f",n_sum,sum_time);

    std_arena_free(&tree_arena);
}

void test_bitmap(){
    bitmap_t bitmap_to_check = 0x1110001;
    _Bool check;

    check = bitmap_scan(bitmap_to_check, 0);
    check = bitmap_scan(bitmap_to_check, 20);
    check = bitmap_scan(bitmap_to_check, 21);
    check = bitmap_scan(bitmap_to_check, 24);
    check = bitmap_scan(bitmap_to_check, 25);
    check = bitmap_scan(bitmap_to_check, 28);
    check = bitmap_scan(bitmap_to_check, 29);

    bitmap_t bitmap_to_set = BITMAP_EMPTY;
    bitmap_set(&bitmap_to_set,0);
    bitmap_set(&bitmap_to_set,1);
    bitmap_set(&bitmap_to_set,2);
    bitmap_set(&bitmap_to_set,16);
    bitmap_set(&bitmap_to_set,28);

    check = bitmap_scan(bitmap_to_set, 0);
    check = bitmap_scan(bitmap_to_set, 1);
    check = bitmap_scan(bitmap_to_set, 2);
    check = bitmap_scan(bitmap_to_set, 3);
    check = bitmap_scan(bitmap_to_set, 16);
    check = bitmap_scan(bitmap_to_set, 27);
    check = bitmap_scan(bitmap_to_set, 28);
    check = bitmap_scan(bitmap_to_set, 29);

    bitmap_reset(&bitmap_to_set,2);
    bitmap_reset(&bitmap_to_set,16);
    bitmap_reset(&bitmap_to_set,29);

    check = bitmap_scan(bitmap_to_set, 2);
    check = bitmap_scan(bitmap_to_set, 16);
    check = bitmap_scan(bitmap_to_set, 29);

    bitmap_to_set = 0x20010004;

    bitmap_clear_bit(&bitmap_to_set,2);
    bitmap_clear_bit(&bitmap_to_set,16);
    bitmap_clear_bit(&bitmap_to_set,29);
}