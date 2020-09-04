
/*
* A simple implement of single linked list.
* 
*/
#ifndef __XUI_LINKLIST_H__
#define __XUI_LINKLIST_H__

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
#define xui_list_for_each(pos,  head) \
	for (pos = head; pos != (NULL); \
		pos = pos->next)



void listAppend(void *data, tLinkList * list);
void listReverse(tLinkList * list);
void listForeach(tLinkList *list, listCb func);
void listDestroy(tLinkList * list);

int    listLenGet(tLinkList * list);
tNode * listSearch(tLinkList * list, int index);


#endif
