#ifndef ALLOCS_rb_trees_H
#define ALLOCS_rb_trees_H

#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include "common.h"
#include "mem_man.h"

Enum(rb_color_t,{rb_BLACK,rb_RED});

Struct(rb_node,{     // node of red–black tree
    struct rb_node* parent;   // == NULL if root of the tree
    struct rb_node* child[2]; // == NIL if child is empty
    rb_color_t color;
    struct{
        void* value;
        void* key;
    }item;
});

#define NIL   NULL // null pointer  or  pointer to sentinel node
#define LEFT  0
#define RIGHT 1
#define rb_left  child[LEFT]
#define rb_right child[RIGHT]

Struct(rb_tree,{ // red–black tree
    rb_node* root; // == NIL if tree is empty
});

void node_init(rb_node* node,void* key, void* value);

rb_node* RBT_search(rb_tree* tree, void* key);

rb_node* _alloc_new_node(
        Arena* arena,
        void* key,
        void* value,
        size_t value_size
);

#define node_value(ptr_type,node) ((ptr_type)node->item.value)

void RBT_delete(rb_tree* tree,rb_node* node);
void RBT_insert(rb_tree* tree,rb_node* node);

#endif //ALLOCS_rb_trees_H
