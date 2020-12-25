#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "x_general_tree.h"

void tree_cb(tGenTreeNode * node, void * pri){
    printf("traverse node : %p\n",node);
}
void tree_free_cb(tGenTreeNode * node, void * pri){
    printf("traverse free node : %p\n",node);
    memset(node, 0x00 ,sizeof(tGenTreeNode));
    printf("child:%02x,sibling:%02x\n", node->left_child, node->next_sibling);
    free(node);
}

int main(int argc, char * argv[]){
    int found = 0;
    tGenTreeNode * tmp = NULL;
    tGenTreeNode *root = gen_tree_add_child(NULL,0);
    tGenTreeNode * layout = gen_tree_add_child(root, 0);
    printf("root %p\n layout :%p\n",root, layout);
    printf("root add child:%p\n",gen_tree_add_child(root,0));
    printf("root add child:%p\n",tmp = gen_tree_add_child(root,0));

    printf("root child3 add child:%p\n",gen_tree_add_child(tmp,0));
    printf("root child3 add child:%p\n",gen_tree_add_child(tmp,0));

    printf("layout add child:%p\n",gen_tree_add_child(layout,0));
    printf("layout add child:%p\n",layout = gen_tree_add_child(layout,0));

    printf("layout chlid 2 add child:%p\n",gen_tree_add_child(layout,0));
    printf("layout chlid 2 add child:%p\n",gen_tree_add_child(layout,0));
    gen_tree_depth_traverse(root, tree_cb, NULL);
    //printf("----------------------------------------\n");
    //gen_tree_breadth_traverse(root, tree_cb, NULL);
    gen_tree_find(root, tmp, &found);
    printf("----------------====----------------------\n");
    gen_tree_post_traverse(root, tree_free_cb, NULL);
    return 0;

}
