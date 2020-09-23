#ifndef __X_GENERAL_TREE_H__
#define __X_GENERAL_TREE_H__

typedef struct gen_tree_node{
    void * data;
    struct gen_tree_node * left_child;
    struct gen_tree_node * next_sibling;
}tGenTreeNode;

typedef void (*gen_tree_cb)(tGenTreeNode * node,void * pri);

tGenTreeNode * gen_tree_add_child(tGenTreeNode * node, void * data);

/*
DFS search
*/
int gen_tree_depth_traverse(tGenTreeNode * node, gen_tree_cb cb, void * pri);
/*
* BFS search
*/
int gen_tree_breadth_traverse(tGenTreeNode * root_node, gen_tree_cb cb, void * pri);

#endif

