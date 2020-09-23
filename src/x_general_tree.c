#include "x_general_tree.h"
#include "x_util_common.h"
#include "x_linklist.h"

static tGenTreeNode * _new_tree_node(void *data){
    tGenTreeNode * node = (tGenTreeNode *)X_MALLOC(sizeof(tGenTreeNode));
    if(node){
        X_MEMSET(node, 0, sizeof(*node));
        node->data = data;
    }
    return node;
}

tGenTreeNode * gen_tree_add_child(tGenTreeNode * node, void * data){
    tGenTreeNode * new_node = NULL, *sibling_node = NULL;

    if(!node){
        /*create root node*/
        return _new_tree_node(data);
    }

    new_node = _new_tree_node(data);
    if(!new_node) return NULL;
    
    if(!node->left_child){
        node->left_child = new_node;
    }else{
        sibling_node = node->left_child;
        while(sibling_node->next_sibling){
            sibling_node = sibling_node->next_sibling;
        }
        sibling_node->next_sibling = new_node;
    }
    return new_node;
}

/*
DFS search
*/
int gen_tree_depth_traverse(tGenTreeNode * root_node, gen_tree_cb cb, void * pri){

    if(!root_node) return 0;
    
    if(cb) cb(root_node, pri);
    if(root_node->left_child)
        gen_tree_depth_traverse(root_node->left_child, cb, pri);
    if(root_node->next_sibling)
        gen_tree_depth_traverse(root_node->next_sibling, cb, pri);
    return 0;
}

/*
* BFS search
*/
int gen_tree_breadth_traverse(tGenTreeNode * root_node, gen_tree_cb cb, void * pri){

    tLinkList queue_list;
    tNode * list_node = NULL;
    tGenTreeNode *tree_node = NULL;
    if(!root_node) return 0;
    memset(&queue_list, 0 ,sizeof(queue_list));

    listAppend((void *) root_node, &queue_list);
    
    while(!!(list_node = listPoll(&queue_list))){
        tree_node = (tGenTreeNode *)list_node->data;
        while(tree_node){
            if(tree_node->left_child){
                listAppend((void *) tree_node->left_child, &queue_list);
            }
            if(cb) cb(tree_node, pri);
            tree_node = tree_node->next_sibling;
        }
        
        X_FREE(list_node);
    }
    return 0;
}


