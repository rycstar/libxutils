
/*
 A linked list implementation in C.
*/

#include "x_linklist.h"
#include "x_util_common.h"

tNode * createnode(void *data);

tNode * createnode(void * data){
    tNode * newNode = X_MALLOC(sizeof(tNode));
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

void listAppend(void * data, tLinkList * list){
    tNode * current = NULL;
    if(list->head == NULL){
        list->head = createnode(data);
    }
    else {
        current = list->head; 
        while (current->next!=NULL){
            current = current->next;
        }
        current->next = createnode(data);
    }
}

void listReverse(tLinkList * list){
    tNode * reversed = NULL;
    tNode * current = list->head;
    tNode * temp = NULL;
    while(current != NULL){
        temp = current;
        current = current->next;
        temp->next = reversed;
        reversed = temp;
    }
    list->head = reversed;
}

int    listLenGet(tLinkList * list){
    
    tNode *current = list->head;
    int i = 0;
    while (current)
    {
        i++;
        current = current->next;
    }
    return i;
}


tNode * listSearch(tLinkList * list, int index){
    tNode *current = list->head;
    int i = 0;
    while (current)
    {
        if(i == index)  break;
        i++;
        current = current->next;
    }
    return current;
}

void listForeach(tLinkList *list, listCb func)
{
    tNode *current = list->head;
    while (current)
    {
        func(current->data);
        current = current->next;
    }
}

void listDestroy(tLinkList * list){
    tNode * current = list->head;
    tNode * next = current;
    while(current != NULL){
        next = current->next;
        X_FREE(current);
        current = next;
    }
}

