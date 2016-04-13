#include <stdlib.h>
#include <stdio.h>
#include "cs402.h"
#include "my402list.h"

/*******************************************

 function to count the number of elements

 ******************************************/

int My402ListLength(My402List *list)
{
  return list->num_members;
}

/*******************************************

 function to check if list is empty

******************************************/

int My402ListEmpty(My402List *list)
{

  if (list->num_members == 0)
     return TRUE;
  else
     return FALSE;
}

/*******************************************

 function to return the first element of the list

******************************************/

My402ListElem *My402ListFirst(My402List *list)
{
  return list->anchor.next;
}

/*******************************************

 function to return the last element of the list

******************************************/

My402ListElem *My402ListLast(My402List *list)
{
  return list->anchor.prev;
}

/*******************************************

 function to return the prev element

******************************************/

My402ListElem *My402ListPrev(My402List *list, My402ListElem *elem)
{
  if (elem->prev == &(list->anchor))
  {
    return NULL;
  }
  else
  {
    return elem->prev;
  }
}

/*******************************************

 function to return the next element

******************************************/

My402ListElem *My402ListNext(My402List *list, My402ListElem *elem)
{
  if (elem->next == &(list->anchor))
  {
    return NULL;
  }
  else
  {
    return elem->next;
  }
}
/*******************************************

 function to find an element in the list

******************************************/

My402ListElem *My402ListFind(My402List *list, void *obj)
{
  My402ListElem *elem=NULL;
  /* Return NULL if list is empty*/
  if (list->num_members == 0)
  {
    return elem;
  }
  else
  {
    /* Traverse the list for finding the element
       return the element if the object is found
    */
    for (elem=My402ListFirst(list);
         elem != NULL;
         elem=My402ListNext(list, elem)) {
            if (elem->obj == obj)
            {
              return elem;
            }
       }
    /*
     Return the elem = NULL if the element is not found in
     the list
    */
    return elem;
  }

}
/*******************************************

 function to initialize a blank list
******************************************/

int My402ListInit(My402List *list)
{
  list->num_members = 0;
  /*
   Initialize Anchor
  */
  list->anchor.prev = &(list->anchor);
  list->anchor.next = &(list->anchor);
  /*
   Initialize function pointers??
  */
  return TRUE;
}

/*******************************************

Function to append the objects to the list

******************************************/

int My402ListAppend(My402List *list, void *obj)
{
  My402ListElem *elem = malloc(sizeof(My402ListElem));
  if(elem != NULL)
  {
     if (My402ListEmpty(list) == 1)
      {

       /* Assign values to the elem to be add into the empty list*/
       elem->obj = obj;
       elem->prev = &(list->anchor);
       elem->next = &(list->anchor);

       /*Update the List anchor and num_members*/
       list->anchor.prev = elem;
       list->anchor.next = elem;

       (list->num_members)++;

      }
     else
      {
        /* Assign values to the elem to be added to the end of the list*/
       elem->obj = obj;
       elem->prev = list->anchor.prev;
       elem->next = &(list->anchor);

       /*Update second last element of the list*/
       (list->anchor.prev)->next = elem;

       /*Update the list Anchor and num_members*/
       list->anchor.prev = elem;

       (list->num_members)++;

      }
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*******************************************

Function to Prepend the objects to the list

******************************************/

int My402ListPrepend(My402List *list, void *obj)
{
  My402ListElem *elem = malloc(sizeof(My402ListElem));
  if(elem != NULL)
  {
     if (My402ListEmpty(list) == 1)
      {

       /* Assign values to the elem to be add into the empty list*/
       elem->obj = obj;
       elem->prev = &(list->anchor);
       elem->next = &(list->anchor);

       /*Update the List anchor and num_members*/
       list->anchor.prev = elem;
       list->anchor.next = elem;

       (list->num_members)++;

      }
     else
      {
        /* Assign values to the elem to be added to the start of the list*/
       elem->obj = obj;
       elem->next = list->anchor.next;
       elem->prev = &(list->anchor);

       /*Update second element of the list*/
       (list->anchor.next)->prev = elem;

       /*Update the list Anchor and num_members*/
       list->anchor.next =  elem;

       (list->num_members)++;

      }
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*******************************************

Function to Unlink an element from the list

******************************************/

void My402ListUnlink(My402List *list, My402ListElem *elem)
{
  /*
  if there is only one element in the list
  */
  if (list->num_members == 1)
  {
    list->anchor.prev = &(list->anchor);
    list->anchor.next = &(list->anchor);
  }
  /*
  Check if element to be unlinked is first in the list
  */
  else if (elem == My402ListFirst(list))
  {
    list->anchor.next = elem->next;
    (elem->next)->prev = &(list->anchor);
  }
  /*
  Check if element to be unlinked is last in the list
  */
  else if (elem == My402ListLast(list))
  {
    list->anchor.prev = elem->prev;
    (elem->prev)->next = &(list->anchor);
  }
  else
  /*
  For an arbitrary element in the list
  */
  {
    (elem->prev)->next = elem->next;
    (elem->next)->prev = elem->prev;
  }

 /*
 Free the allocated memory
 */
  /*
  free(elem->next);
  free(elem->prev);
  */
  free(elem);

  /* Update the count of the members in the list
  */
  (list->num_members)--;
}

/*******************************************

Function to Unlink all elements from the list

******************************************/

void My402ListUnlinkAll(My402List *list)
{
 My402ListElem *elem = list->anchor.next;
 My402ListElem *head = elem->next;
 list->anchor.next = &(list->anchor);
 while (elem != &(list->anchor))
   {
     head = elem->next;
     elem->next = NULL;
     elem->prev = NULL;
     free(elem);
     elem = head;
   }
 /*
  Free the head and elem memory

 free(elem);
 free(head);
*/
 /*
 Initialize the empty list
 Rewriting the inline function
 */
 list->anchor.next = &(list->anchor);
 list->anchor.prev = &(list->anchor);

 list->num_members = 0;

}

/*******************************************

Function to Insert before an element in the list

******************************************/

int My402ListInsertBefore(My402List *list , void *obj, My402ListElem *elem)
{  My402ListElem *newElem;
  /*
    If the elem is NULL or the elem is the first element of the list

  */
  if (elem == NULL || elem == My402ListFirst(list))
  {
     My402ListPrepend(list,obj);
  }
  else
  {   /*
         Assign new memory to newElem
      */
      newElem = malloc(sizeof(My402ListElem));
      /*
         Assign Object
      */
      newElem->obj = obj;
      /*
        Assign pointers of the new element
      */
      newElem->prev = elem->prev;
      newElem->next = elem;
      /*
      update the pointer of elem->prev
      */
      (elem->prev)->next = newElem;
      /*
        Update pointers of the old element
      */
      elem->prev = newElem;
      (list->num_members)++;
  }
  return TRUE;
}

/*******************************************

Function to Insert after an element in the list

******************************************/

int My402ListInsertAfter(My402List *list , void *obj, My402ListElem *elem)
{  My402ListElem *newElem;
  /*
    If the elem is NULL or the elem is the first element of the list

  */
  if (elem == NULL || elem == My402ListLast(list))
  {
     My402ListAppend(list,obj);
  }
  else
  {   /*
         Assign new memory to newElem
      */
      newElem = malloc(sizeof(My402ListElem));
      /*
         Assign Object
      */
      newElem->obj = obj;
      /*
        Assign pointers of the new element
      */
      newElem->next = elem->next;
      newElem->prev = elem;
      /*
       Update the pointer of elem-> next
      */
      (elem->next)->prev = newElem;
      /*
        Update pointers of the old element
      */
      elem->next = newElem;

      (list->num_members)++;
  }
  return TRUE;
}

/*

Test Main function
Remove before file submission


int main()
{

  int i=0;
  int num_items = 10;
  My402List *pList = malloc(sizeof(My402List));
  My402ListElem *elem;
  printf("Initialize the list");
  My402ListInit(pList);
  printf("Initialization complete");
//  Create the list

  printf("Creation of the list\n");
  for (i=0; i < num_items; i++) {
      (void)My402ListPrepend(pList, (void*)i);
  }

  //Traverse and print the list

  for (elem=My402ListFirst(pList);
       elem != NULL;
      elem=My402ListNext(pList, elem)) {
      printf("%d\n",(int)(elem->obj));
     }
     printf("length:%d\n",pList->num_members);

  My402ListUnlink(pList,pList->anchor.next->next->next);

  //Traverse and print the list

  for (elem=My402ListFirst(pList);
       elem != NULL;
      elem=My402ListNext(pList, elem)) {
      printf("%d\n",(int)(elem->obj));
     }

     printf("length:%d\n",pList->num_members);
  return 0;
}
*/
