
/*
* A simple implement of single linked list.
* 
*/
#ifndef __X_LINKLIST_H__
#define __X_LINKLIST_H__

typedef struct node {
  void * data;
  struct node * next;
} tNode;

typedef struct _tLinkList {
  tNode * head; 
} tLinkList;

typedef void (*listCb)(void *);

/**
 * xui_list_for_each	iterate over a list
 * @pos:	the &struct list_head to use as a loop counter.
 * @head:	the head for your list.
 */
#define x_list_for_each(pos,  head) \
	for (pos = head; pos != (NULL); \
		pos = pos->next)



void listAppend(void *data, tLinkList * list);
/*
* get the first node of the linkedlist
* caller should free the node
*/
tNode * listPoll(tLinkList * list);
void listReverse(tLinkList * list);
void listForeach(tLinkList *list, listCb func);
void listDestroy(tLinkList * list);

int    listLenGet(tLinkList * list);
tNode * listSearch(tLinkList * list, int index);


#endif
