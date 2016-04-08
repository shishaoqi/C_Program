/* 
 * File: iterator.c
 * -----------------
 * This file implements a polymorphic version of the iterator 
 * and the tools necessary to construct types that support
 * iteration. This module implements both the iterator.h and
 * itertype.h interfaces.
 */

#include <stdio.h>
#include <string.h>
#include "genlib.h"
#include "cmpfn.h"
#include "iterator2.h"
#include "itertype.h"
/*
 * Constant: IteratorPassword
 * --------------------------
 * This constant is stored in the header block of collection
 * types to catch the case when a client tries to call
 * NewIterator on a value for which iteration is not defined
 */
#define IteratorPassword 3141582653UL

typedef struct cellT{
  struct cellT *link;
  /* The actual data is alllocated here */
}cellT;

struct iteratorCDT{
	int elementSize;
	cmpFnT cmpFn; 
	//对于在NewIterator原型中声明为void * 的聚集参数的匿名字符，NewIterator并不知道如何初始化一个适合传入的聚集类型的迭代器：解决方法是：保证聚集类型的底层数据结构包含一个指向创建新迭代器的回调函数指针。
	cellT *head, *tail;
};

iteratorADT NewIterator(void *collection)
{
	iteratorHeaderT *hp = (iteratorHeaderT *)collection;

	if(hp->password != IteratorPassword){
	  	Error("Itertion is not defined for this type");
	}
	return (hp->newFn(collection));
}

bool StepIterator(iteratorADT iterator, void **ep)
{
	cellT *cp;
	void *dp;

	cp = iterator->head;
	if(cp == NULL){
		iterator->tail = NULL;
		return (FALSE);
	}

	dp = ((char *) cp) + sizeof(cellT);
	/**ep = GetBlock(iterator->elementSize);
	   memcpy(*ep, dp, iterator->elementSize);*/
	*ep = dp;
	iterator->head = cp->link;
	FreeBlock(cp);
	return (TRUE);
}

void FreeIterator(iteratorADT iterator)
{
	cellT *cp;
	 
	while((cp = iterator->head) != NULL){
		iterator->head = cp->link;
		FreeBlock(cp);
	}
	FreeBlock(iterator);
}

/* 
 * File: itertype.h中定义的函数
 */
void EnableIteration(void *collection, newIteratorFnT newFn)
{
	iteratorHeaderT *hp = (iteratorHeaderT *) collection;

	hp->password = IteratorPassword;
	hp->newFn = newFn;
}

iteratorADT NewIteratorList(int size, cmpFnT cmpFn)
{
	iteratorADT iterator;

	iterator = New(iteratorADT);
	iterator->elementSize = size;
	iterator->cmpFn = cmpFn;
	iterator->head = iterator->tail = NULL;
	return iterator;
}

/* 
 * Implementtation notes: AddToIteratorList
 * ---------------------------------------------------
 * Most of the word of the package occers in this function, which
 * inserts the element addressed by ep into its corret position.
 * if the list is unordered or if the element belongs at the end,
 * the element is immediately inserted at the tail. If not, the
 * implementation walks the list to find the correct position.
 * The local pointers have the following interpretations:
 * np  --  pointer to the newly allocated cell
 * pp  --  pointer to the cell preceding the insertion point
 * ip  --  pointer used as an index in the for loop
 * dp  --  pointer to the data field in the block
 */
void AddToIteratorList(iteratorADT iterator, void *ep)
{
    cellT *np, *pp, *ip, *temp;
	void *dp;
    int size=0;

	size = (int)sizeof(cellT)+iterator->elementSize;
	np =  GetBlock (size);
	dp = ((char *)np) + sizeof(cellT);
	memcpy(dp, ep, iterator->elementSize);
	pp = NULL;
	if(iterator->tail != NULL){
		if(iterator->cmpFn == UnsortedFn){
			pp = iterator->tail;
		}else{
			dp = ((char *) iterator->tail)+sizeof(cellT);
			if(iterator->cmpFn(ep, dp) >= 0) pp = iterator->tail;
		}
	}
	if(pp == NULL){
		for(ip = iterator->head; ip != NULL; ip= ip->link){
			dp = ((char *)ip) + sizeof(cellT);
			if(iterator->cmpFn(ep, dp) < 0) break;
			pp = ip;
		}
	}
	if(pp == NULL){
	  	if(iterator->head == NULL){
            iterator->head=iterator->tail = np;
		    iterator->head->link=iterator->tail->link=NULL;
	  	}else{
             temp = iterator->head;
		    iterator->head = np;
		    iterator->head->link = temp;
        }
	}else{
		np->link = pp->link;
		if(pp->link == NULL){
			iterator->tail = np;
			iterator->tail->link=NULL;
		}
		pp->link = np;
	}
}
