#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <locale.h>
/*#include "my402list.c"*/
#include "my402list.h"


#ifndef TRUE
#define FALSE 0
#define TRUE 1
#endif

#define BUFFER 1030

typedef struct transaction_t
{
  char *op;
  int timeval;
  int amt;
  char *description;
} trnx;

/*********************************************************

Function to check if the time exists in the list

*********************************************************/

int ExistTimestamp(My402List *pList,int timeval)
{
  My402ListElem *elem=NULL;
  /* Return FALSE if list is empty*/
  if (pList->num_members == 0)
  {
    return FALSE;
  }
  else
  {
    /* Traverse the list for finding the element
       return the element if the object is found
    */
    for (elem=My402ListFirst(pList);
         elem != NULL;
         elem=My402ListNext(pList, elem)) {
            if (((trnx*)(elem->obj))->timeval == timeval)
            {
              return TRUE;
            }
       }

    return FALSE;
  }
}
/*********************************************************

  Error handling for each line of the file
  1) LineSize
  2) Check the number of tabs

*********************************************************/

int errorCheckLine (char *line)
{
  char *temp;
  int tabcount;
  /* Check if line size is 1024
     1023 + /n character
  */
  if (strlen(line) < 1024)
  {
    /*
    Count the number of tabs in each line
    if the number of tabs is greater than 3
    print error and exit
    */
    tabcount = 0;
    temp = line;
    while((temp = strchr(temp,'\t')) != NULL)
    {
       temp++;
       tabcount++;
    }
    /*
     The number of elements should be 4
     else should exit with error
    */
    if (tabcount == 3)
    {
      return TRUE;
    }
    else
    {
      fputs("Incorrect elements in the line of file\n",stderr);
      return FALSE;
    }
  }
  else
  {
    fputs("Incorrect line size\n",stderr);
    return FALSE;
  }

}

/*********************************************************

  Error handling for Operator

*********************************************************/
int errCheckOp(trnx *t, char *op)
{
  if (strlen(op) == 1)
  {
    if(strcmp(op,"+") == 0 || strcmp(op,"-") == 0)
    {
      t->op = op;
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }
  else
  {
    return FALSE;
  }
}

/*********************************************************

  Error handling for Time

*********************************************************/

int errCheckTime(My402List *pList,trnx *t,char *timeval)
{
  if (strlen(timeval) < 11)
  {
    if(strtol(timeval,NULL,10) < 0 || strtol(timeval,NULL,10) > ((int)(time(NULL))) )
    {
      fputs("Timestamp Not between Zero and Current time\n",stderr);
      return FALSE;
    }
    if(ExistTimestamp(pList,strtol(timeval,NULL,10)) == 1)
    {
      fputs("Timestamp already exists in the list\n",stderr);
      return FALSE;
    }
    t->timeval = strtol(timeval,NULL,10);
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*********************************************************

  Error handling for Amount

*********************************************************/

int errCheckAmount(trnx *t, char *amt)
{
  char *dec;
  char *integer;
  //char *cents;

  //cents = malloc(sizeof(amt));
  /*
    Retrieve the decimal point
  */
  if((dec = strchr(amt,'.')) != NULL)
  {
    dec++;
  }
  else
  {
    return FALSE;
  }
  /*
    Check the integer value
  */
  integer = strtok(amt,".");
  if(strlen(integer) > 7)
  {
    return FALSE;
  }
  /*
   Check the decimal value
  */
  if (strlen(dec) > 2)
  {
    return FALSE;
  }
  strncat(integer,dec,strlen(integer)+strlen(dec)+1);
  //printf("Integer %s\n",amt);

  t->amt = strtol(integer,NULL,10);

  //printf("Amount %d\n",t->amt);

  return TRUE;
}

/*********************************************************

  Error handling for Description

*********************************************************/

int errCheckDesc(trnx *t,char *description)
{
  char *desc = malloc(strlen(description));
  desc = strdup(description);

  while (isspace(*desc))
  {
    ++desc;
  }
  if (strlen(desc) == 0)
  {
    fputs("Description cannot be NULL",stderr);
    return FALSE;
  }
  else
  {
    t->description = desc;
    return TRUE;
  }
  free(desc);
}
/*********************************************************

   Print the entire list

********************************************************

void PrintList(My402List *pList)
{
    My402ListElem *elem=NULL;
    for (elem=My402ListFirst(pList); elem != NULL; elem=My402ListNext(pList, elem)) {
        int ival=((trnx*)(elem->obj))->timeval;
        char *desc =((trnx*)(elem->obj))->description;

        //fprintf(stdout, "Time %d\n", ival);
        //fprintf(stdout, "Description %s\n",desc);
    }
}
*/
/*********************************************************

  Use Bubble sort Forward function to perform swap

*********************************************************/

void BubbleSwap(My402List *pList, My402ListElem **pp_elem1, My402ListElem **pp_elem2)
    /* (*pp_elem1) must be closer to First() than (*pp_elem2) */
{
    My402ListElem *elem1=(*pp_elem1), *elem2=(*pp_elem2);
    void *obj1=elem1->obj, *obj2=elem2->obj;
    My402ListElem *elem1prev=My402ListPrev(pList, elem1);
/*  My402ListElem *elem1next=My402ListNext(pList, elem1); */
/*  My402ListElem *elem2prev=My402ListPrev(pList, elem2); */
    My402ListElem *elem2next=My402ListNext(pList, elem2);

    My402ListUnlink(pList, elem1);
    My402ListUnlink(pList, elem2);
    if (elem1prev == NULL) {
        (void)My402ListPrepend(pList, obj2);
        *pp_elem1 = My402ListFirst(pList);
    } else {
        (void)My402ListInsertAfter(pList, obj2, elem1prev);
        *pp_elem1 = My402ListNext(pList, elem1prev);
    }
    if (elem2next == NULL) {
        (void)My402ListAppend(pList, obj1);
        *pp_elem2 = My402ListLast(pList);
    } else {
        (void)My402ListInsertBefore(pList, obj1, elem2next);
        *pp_elem2 = My402ListPrev(pList, elem2next);
    }
}

/*********************************************************

  Use Bubble sort to perform sorting of the list

*********************************************************/

void BubbleSortList(My402List *pList, int num_items)
{
    My402ListElem *elem=NULL;
    int i=0;
    for (i=0; i < num_items; i++) {
        int j=0, something_swapped=FALSE;
        My402ListElem *next_elem=NULL;

        for (elem=My402ListFirst(pList), j=0; j < num_items-i-1; elem=next_elem, j++) {
            int cur_val=((trnx*)(elem->obj))->timeval, next_val=0;

            next_elem=My402ListNext(pList, elem);
            next_val = ((trnx*)(next_elem->obj))->timeval;

            if (cur_val > next_val) {
                BubbleSwap(pList, &elem, &next_elem);
                something_swapped = TRUE;
            }
        }
        if (!something_swapped) break;
    }
}
void RepeatChar(char *c, int num)
{
  int i;
  for (i=0;i<num;i++)
  {
    fprintf(stdout,"%s",c);
  }
}

/*********************************************************
   Display Output function
*********************************************************/
void DisplayOutput(My402List *pList)
{
  My402ListElem *elem = NULL;
  time_t timestamp;
  int balance = 0;
  char *description;
  int i,j,k;
  /*
    Print the header information
  */
  fputs("+",stdout);
  RepeatChar("-",17);
  fputs("+",stdout);
  RepeatChar("-",26);
  fputs("+",stdout);
  RepeatChar("-",16);
  fputs("+",stdout);
  RepeatChar("-",16);
  fputs("+",stdout);
  fputs("\n",stdout);
  fputs("|",stdout);
  RepeatChar(" ",7);
  fputs("Date",stdout);
  RepeatChar(" ",6);
  fputs("|",stdout);
  fputs(" Description",stdout);
  RepeatChar(" ",14);
  fputs("|",stdout);
  RepeatChar(" ",9);
  fputs("Amount ",stdout);
  fputs("|",stdout);
  RepeatChar(" ",8);
  fputs("Balance ",stdout);
  fputs("|",stdout);
  fputs("\n",stdout);
  fputs("+",stdout);
  RepeatChar("-",17);
  fputs("+",stdout);
  RepeatChar("-",26);
  fputs("+",stdout);
  RepeatChar("-",16);
  fputs("+",stdout);
  RepeatChar("-",16);
  fputs("+",stdout);
  fputs("\n",stdout);
  /*
  Header Information Printed
  */
  /*
   Print each element in the list
  */
  for (elem=My402ListFirst(pList); elem != NULL; elem=My402ListNext(pList, elem)) {
  /*
   Print time
  */
  timestamp = (time_t)(((trnx*)(elem->obj))->timeval);
  fputs("| ",stdout);
  char *ctimestamp = ctime(&timestamp);
  //char *temptime;
  for(i=0;i<11;i++)
  {
    fprintf(stdout,"%c",ctimestamp[i]);
  }
  for(i=20;i<24;i++)
  {
    fprintf(stdout,"%c",ctimestamp[i]);
  }

  //fprintf(stdout,"%s here",ctimestamp);
  fputs(" | ",stdout);

  /*
   Print Description
  */

  description = (((trnx*)(elem->obj))->description);
  if (strlen(description) > 24)
  {

    for (i=0;i<24;i++)
    {
      fprintf(stdout,"%c",description[i]);
    }
    fputs(" ",stdout);
  }
  else
  {
    for (i=0;i<(strlen(description)-1);i++)
    {
      fprintf(stdout,"%c",description[i]);
    }

    for(i=0;i<=(25-strlen(description));i++)
    {
      fputs(" ",stdout);
    }

  }
  fputs("| ",stdout);

  /*
    Print Amount
  */
  char *amount= malloc(15);
  char *temp = malloc(16);
  sprintf(amount,"%d",((trnx*)(elem->obj))->amt);
  if(strcmp(((trnx*)(elem->obj))->op,"-") == 0)
  {
    fputs("(",stdout);
  }
  else
  {
    fputs(" ",stdout);
  }
  if(strlen(amount) == 2 )
  {
    temp[0] = '0';
    temp[1] ='.';
    temp[2] = amount[0];
    temp[3] = amount[1];
    k = 4;
  }
  else if (strlen(amount) == 1 )
  {
    temp[0] = '0';
    temp[1] ='.';
    temp[2] = '0';
    temp[3] = amount[0];
    k = 4;

  }
  else
  {
  for(i=strlen(amount)-1,j = 0,k=0;i>=0;i--,j++)
  {
    if(i == 8 || i == 5)
    {
      temp[k]=amount[j];
      k++;
      temp[k]= ',';
      k++;
    }
    else if( i == 2)
    {
      temp[k]=amount[j];
      k++;
      temp[k]= '.';
      k++;
    }
    else
    {
      temp[k]=amount[j];
      k++;
    }
  }
  }
  temp[k]= '\0';
  for(i=0;i<12-strlen(temp);i++)
  {
    fputs(" ",stdout);
  }

  fprintf(stdout,"%s",temp);
  if(strcmp(((trnx*)(elem->obj))->op,"-") == 0)
  {
    fputs(")",stdout);
  }
  else
  {
    fputs(" ",stdout);
  }
  fputs(" | ",stdout);
  free(temp);
  /*
    Print Balance
  */
  if(strcmp(((trnx*)(elem->obj))->op,"-") == 0)
  {
    balance = balance - ((trnx*)(elem->obj))->amt;
  }
  else
  {
    balance = balance + ((trnx*)(elem->obj))->amt;
  }
  char *bal_amount = malloc(15);
  temp = malloc(16);
  sprintf(bal_amount,"%d",abs(balance));

  if(balance < 0)
  {
    fputs("(",stdout);
  }
  else
  {
    fputs(" ",stdout);
  }
if(strlen(bal_amount)> 9)
{
  free(bal_amount);
  bal_amount = malloc(15);
  strncpy(bal_amount,"?,???,???.??",15);
  fprintf(stdout,"%s",bal_amount);
}
else
{
  for(i=strlen(bal_amount)-1,j = 0,k=0;i>=0;i--,j++)
  {
    if(i == 8 || i == 5)
    {
      temp[k]=bal_amount[j];
      k++;
      temp[k]= ',';
      k++;
    }
    else if( i == 2)
    {
      temp[k]=bal_amount[j];
      k++;
      temp[k]= '.';
      k++;
    }
    else
    {
      temp[k]=bal_amount[j];
      k++;
    }
  }
  temp[k]= '\0';
  for(i=0;i<12-strlen(temp);i++)
  {
    fputs(" ",stdout);
  }
}
  fprintf(stdout,"%s",temp);
  if(balance < 0)
  {
    fputs(")",stdout);
  }
  else
  {
    fputs(" ",stdout);
  }
  fputs(" |",stdout);
  //fputs("\n",stdout);
  free(temp);
  free(bal_amount);
  free(amount);

  fputs("\n",stdout);
}

fputs("+",stdout);
RepeatChar("-",17);
fputs("+",stdout);
RepeatChar("-",26);
fputs("+",stdout);
RepeatChar("-",16);
fputs("+",stdout);
RepeatChar("-",16);
fputs("+",stdout);
fputs("\n",stdout);


}
/*********************************************************

 Wrapper function to implement each step of the sort

 1) Read the file and load each line into the linked list
 2) Sort the linked list based on transaction date
 3) Display to stdout in the format as specified in
    the specification
*********************************************************/

void DoSort(FILE *fp)
{
   /*
     Step1 a
     Open the file and generate linked list
   */
   trnx *t;
   char line[BUFFER];
   char *tokens;
   char *op,*timeval,*amt,*description;
   int errCheck = 1;
   /*
    Generate a list and Initialise it
   */
   My402List list;
   memset(&list,0,sizeof(My402List));

   (void)My402ListInit(&list);
   /*
   */
   fgets(line,BUFFER,fp);
   while (!feof(fp))
   {
     /*
       Perform the necessary error checks for the line
     */
     errCheck = errorCheckLine(line);

     if (errCheck == 0)
     {
       exit(0);
     }
     /*
       Extract each element in the file and store it
      each attribute of transaction.

     */
     t = malloc(sizeof(trnx));
     tokens = malloc(strlen(line));
     amt = malloc(strlen(line));
     strcpy(tokens,line);

     op = strtok(tokens,"\t");
     timeval = strtok(NULL,"\t");
     amt = strtok(NULL,"\t");
     description = strtok(NULL,"\t");
     /*
     Error Check for each of the tokens in the line
     */
     /*
        Operator
     */
     errCheck = 1;
     errCheck = errCheckOp(t,op);
     if (errCheck == 0)
     {
       fputs("Operator is invalid.Must be +/-\n",stderr);
       exit(0);
     }
     /*
        Time
     */
     errCheck = 1;
     errCheck = errCheckTime(&list,t,timeval);
     if (errCheck == 0)
     {
       fputs("Timestamp is invalid.\n",stderr);
       exit(0);
     }
     /*
        Amount
     */
     errCheck = 1;
     errCheck = errCheckAmount(t,amt);
     if (errCheck == 0)
     {
       fputs("Amount is invalid.\n",stderr);
       exit(0);
     }
     /*
       Description
     */
     errCheck = errCheckDesc(t,description);
     if (errCheck == 0)
     {
       exit(0);
     }
     /*
      Create node in the list
     */

     (void)My402ListAppend(&list,(void *)t);
     /*
       Get next element in the list
     */

     //free(tokens);
     //free(amt);
     fgets(line,BUFFER,fp);

   }
   fclose(fp);
   //PrintList(&list);
   BubbleSortList(&list,list.num_members);
   //PrintList(&list);
   DisplayOutput(&list);

}

int main(int argc, char *argv[] )
{
  FILE *fp;
  if (argc == 1 || strcmp(argv[1],"sort") != 0)
  {
    /*
    If only warmup is given without using sort command
    or command other than sort is written
    */
    fputs("Incorrect command\n USAGE: should be warmup sort [tfile]\n",stderr);
  }
  else if (argc == 2 && strcmp(argv[1],"sort") == 0)
  {
    fp = stdin;
    DoSort(fp);

  }
  else if (argc == 3 && strcmp(argv[1],"sort") == 0)
  {
    /*
     Check if file is correct extension
    */
    //char *extension = strchr(argv[2],'.');
   //if (strcmp(extension,".tfile") == 0)
   //{
     char *filename = argv[2];
     /*
      Open and read the file
     */

     fp = fopen(filename,"r");
     
     if(fp == NULL)
     {
       fputs("File does not exist\n",stderr);
     }
     else
     {
     //fputs("Correct file\n",stdout);
     DoSort(fp);
     }
   //}
   //else
   //{
    // fputs("Incorrect file extension. Must be tfile\n",stderr);
   //}
  }
  else
  {
    /*
      File commands incorrect
    */
    fputs("Incorrect use of command warmup\n",stderr);
  }
  return 0;
}
