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
DFS search traverse
*/
int gen_tree_depth_traverse(tGenTreeNode * root_node, gen_tree_cb cb, void * pri){
    tGenTreeNode * tmp_left_child = NULL, * tmp_next_sibling = NULL;
    if(!root_node) return 0;
    tmp_left_child = root_node->left_child;
    tmp_next_sibling = root_node->next_sibling;
    if(cb) cb(root_node, pri);
    if(tmp_left_child)
        gen_tree_depth_traverse(tmp_left_child, cb, pri);
    if(tmp_next_sibling)
        gen_tree_depth_traverse(tmp_next_sibling, cb, pri);
    return 0;
}

/*
* BFS search traverse
*/
int gen_tree_breadth_traverse(tGenTreeNode * root_node, gen_tree_cb cb, void * pri){

    tLinkList queue_list;
    tNode * list_node = NULL;
    tGenTreeNode *tree_node = NULL, *tmp_node = NULL;
    if(!root_node) return 0;
    memset(&queue_list, 0 ,sizeof(queue_list));

    listAppend((void *) root_node, &queue_list);
    
    while(!!(list_node = listPoll(&queue_list))){
        tree_node = (tGenTreeNode *)list_node->data;
        while(tree_node){
            if(tree_node->left_child){
                listAppend((void *) tree_node->left_child, &queue_list);
            }
            tmp_node = tree_node->next_sibling;
            if(cb) cb(tree_node, pri);
            tree_node = tmp_node;
        }
        
        X_FREE(list_node);
    }
    return 0;
}


int gen_tree_post_traverse(tGenTreeNode * root_node, gen_tree_cb cb, void * pri){
    tGenTreeNode * sibling_node = NULL;
    tGenTreeNode * next_node = NULL;
    if(!root_node) return 0;

    if(root_node->left_child){
        sibling_node = root_node->left_child;
        while(sibling_node){
            next_node = sibling_node->next_sibling; /*save it, sibling_node maybe freed in the traverse*/
            gen_tree_post_traverse(sibling_node, cb, pri);
            sibling_node = next_node;
        }
    }

    if(cb) cb(root_node, pri);
    return 0;
}


int gen_tree_find(tGenTreeNode * root_node, tGenTreeNode * target_node, int * found){
    tGenTreeNode * tmp_left_child = NULL, * tmp_next_sibling = NULL;
    printf("gen_tree_find come in : %p != %p\n", root_node, target_node);
    if(!root_node || (*found)) return 0;
    tmp_left_child = root_node->left_child;
    tmp_next_sibling = root_node->next_sibling;
    if(root_node == target_node){
        *found = 1;
        return 0;
    }
    if(tmp_left_child)
        gen_tree_find(tmp_left_child, target_node, found);
    if(tmp_next_sibling)
        gen_tree_find(tmp_next_sibling, target_node, found);
    return 0;
}

