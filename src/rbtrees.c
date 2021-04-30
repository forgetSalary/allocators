#include "../include/rbtrees.h"

// Get the child direction (∈ { LEFT, RIGHT })
//   of the non-root non-NIL  rb_node* N:
#define childDir(N) ( (N) == ((N)->parent)->rb_right ? RIGHT : LEFT )

// Helper functions:

#define get_color(N) ((N) != NIL ? (N)->color : rb_BLACK)

inline rb_node* tree_get_parent(rb_node* N){
    assert(N != NULL);
    return (N)->parent;
}

inline rb_node* tree_get_grand_parent(rb_node* N) {
    rb_node* P = tree_get_parent(N);
    assert(P != NULL);
    return P->parent;
}

rb_node* get_sibling(rb_node* N) {
    rb_node* P = tree_get_parent(N);
    // No parent means no sibling:
    assert(P != NULL);
    return P->child[1-childDir(N)];
}
// If parent P and child direction dir is available, same as:
//   P->child[1-dir]


rb_node* dir_rotate(
        rb_tree* T,   // red–black tree
        rb_node* P,   // root of subtree (may be the root of T)
        int dir) {   // dir ∈ { LEFT, RIGHT }

    rb_node* G = P->parent;
    rb_node* S = P->child[1-dir];
    rb_node* C;
    assert(S != NIL); // pointer to true node required
    C = S->child[dir];
    P->child[1-dir] = C;
    if (C != NIL){
        C->parent = P;
    }

    S->child[dir] = P; P->parent = S;
    S->parent = G;

    if (G != NULL)
        G->child[ P == G->rb_right ? RIGHT : LEFT ] = S;
    else
        T->root = S;

    return S; // new root of subtree
}

static void RBT_insert_fixup(rb_tree* tree, rb_node* node){
    rb_node* y = NIL;
    rb_node* parent = NIL;
    rb_node* g_parent = NIL;

    while (get_color(node->parent) == rb_RED){
        parent = tree_get_parent(node);
        g_parent = tree_get_grand_parent(node);
        if (parent == g_parent->rb_left){
            y = g_parent->rb_right;
            if (get_color(y) == rb_RED){
                parent->color = rb_BLACK;
                y->color = rb_BLACK;
                g_parent->color = rb_RED;
                node = g_parent;
            }
            else{
                if(node == parent->rb_right){
                    node = parent;
                    dir_rotate(tree, node, LEFT);
                }
                tree_get_parent(node)->color = rb_BLACK;
                tree_get_grand_parent(node)->color = rb_RED;
                dir_rotate(tree, tree_get_grand_parent(node), RIGHT);
            }

        }
        else{
            y = g_parent->rb_left;
            if (get_color(y) == rb_RED){
                parent->color = rb_BLACK;
                y->color = rb_BLACK;
                g_parent->color = rb_RED;
                node = g_parent;
            }
            else{
                if(node == parent->rb_left){
                    node = parent;
                    dir_rotate(tree, node, RIGHT);
                }
                tree_get_parent(node)->color = rb_BLACK;
                tree_get_grand_parent(node)->color = rb_RED;
                dir_rotate(tree, tree_get_grand_parent(node), LEFT);
            }
        }
    }
    tree->root->color = rb_BLACK;
}

void RBT_delete_fixup(rb_tree* tree,rb_node* node){
    rb_node* w;
    rb_node* parent;

    while (node && node != tree->root && get_color(node) == rb_BLACK){
        parent = tree_get_parent(node);
        if(node == parent->rb_left){
            w = node->parent->rb_right;
            if (get_color(w) == rb_RED){
                w->color = rb_BLACK;
                parent->color = rb_RED;
                dir_rotate(tree, parent, LEFT);
                w = tree_get_parent(node)->rb_right;
            }
            if (get_color(w->rb_left) == rb_BLACK && get_color(w->rb_right) == rb_BLACK){
                w->color = rb_RED;
                node = tree_get_parent(node);
            }else{
                parent = tree_get_parent(node);
                if (get_color(node->rb_right) == rb_BLACK){
                    w->rb_left->color = rb_BLACK;
                    w->color = rb_RED;
                    dir_rotate(tree, w, RIGHT);
                }
                w->color = get_color(parent);
                parent->color = rb_BLACK;
                w->rb_right->color = rb_BLACK;
                dir_rotate(tree,node->parent,LEFT);
                node = tree->root;
            }
        }
        else{
            w = parent->rb_left;
            if (get_color(w) == rb_RED){
                w->color = rb_BLACK;
                parent->color = rb_RED;
                dir_rotate(tree, parent, RIGHT);
                w = tree_get_parent(node)->rb_left;
            }
            if (get_color(w->rb_right) == rb_BLACK && get_color(w->rb_left) == rb_BLACK){
                w->color = rb_RED;
                node = tree_get_parent(node);
            }else{
                parent = tree_get_parent(node);
                if (get_color(node->rb_left) == rb_BLACK){
                    w->rb_right->color = rb_BLACK;
                    w->color = rb_RED;
                    dir_rotate(tree, w, LEFT);
                }
                w->color = get_color(parent);
                parent->color = rb_BLACK;
                w->rb_left->color = rb_BLACK;
                dir_rotate(tree,node->parent,RIGHT);
                node = tree->root;
            }
        }
    }
    if (node){
        node->color = rb_BLACK;
    }
}

void node_init(rb_node* node,void* key, void* value){
    node->item.key = key;
    node->item.value = value;
    node->color = rb_BLACK;
    node->parent = NIL;
    node->rb_left = NIL;
    node->rb_right = NIL;
}

rb_node* _alloc_new_node(
        Arena* arena,
        void* key,
        void* value,
        size_t value_size){
    rb_node* new_node = std_arena_alloc(arena, sizeof(rb_node));

    void* copy_of_value = std_arena_alloc(arena, value_size);
    memcpy(copy_of_value, value, value_size);

    node_init(new_node,key,copy_of_value);

    return new_node;
}

void RBT_insert(rb_tree* tree,rb_node* node){
    rb_node* y = NIL;
    rb_node* x = tree->root;

    while(x!=NIL){
        y = x;
        if (node->item.key < x->item.key){
            x = x->rb_left;
        }else{
            x = x->rb_right;
        }
    }
    node->parent = y;

    if (y==NIL){
        tree->root = node;
    }else if(node->item.key < y->item.key){
        y->rb_left = node;
    }else{
        y->rb_right = node;
    }

    node->rb_left = NIL;
    node->rb_right = NIL;
    node->color = rb_RED;
    RBT_insert_fixup(tree,node);
}

rb_node* tree_minimum(rb_node* node){
    for(;node->rb_left != NIL;){
        node = node->rb_left;
    }
    return node;
}

void _RBT_transplant(rb_tree* tree,rb_node* node_u,rb_node* node_v){
    if (tree_get_parent(node_u) == NIL){
        tree->root = node_v;
    }else if(node_u == tree_get_parent(node_u)->rb_left){
        node_u->parent->rb_left = node_v;
    }else{
        node_u->parent->rb_right = node_v;
    }
    if(node_v){
        node_v->parent = node_u->parent;
    }
}

void RBT_delete(rb_tree* tree,rb_node* node){
    rb_node* x;
    rb_node* y = node;
    rb_color_t y_original_color = get_color(y);

    if (node->rb_left == NIL){
        x = node->rb_right;
        _RBT_transplant(tree,node,node->rb_right);
    }else if(node->rb_right == NIL){
        x = node->rb_left;
        _RBT_transplant(tree,node,node->rb_left);
    }else{
        y = tree_minimum(node->rb_right);
        y_original_color = get_color(y);
        x = y->rb_right;
        if (y->parent == node){
            x->parent = y;
        }else{
            _RBT_transplant(tree,y,y->rb_right);
            y->rb_right = node->rb_right;
            y->rb_right->parent = y;
        }
        _RBT_transplant(tree,node,y);
        y->rb_left = node->rb_left;
        y->rb_left->parent = y;
        y->color = get_color(node);
    }
    if (y_original_color == rb_BLACK){
        RBT_delete_fixup(tree,x);
    }
}

rb_node* RBT_search(rb_tree* tree, void* key){
    rb_node* x = tree->root;
    while (x != NIL && x->item.key != key){
        if(key < x->item.key){
            x = x->rb_left;
        } else{
            x = x->rb_right;
        }
    }
    return x;
}
