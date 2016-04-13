#ifndef TRUE
#define FALSE 0
#define TRUE 1
#endif /* ~TRUE */

#define BUFFER 1050

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>
#include "my402list.h"



// Global Structure

typedef struct Argument_Struct{
  int total_packets;
  double p_inter_arr_time;
  int tok_req;
  double serv_time;
} arguments;
// Customer Structure

typedef struct Customers
{
  int packet_no;
  int tok_req;
  long arrv_time;
  long t_entered_q1;
  long t_leaves_q1;
  long t_entered_q2;
  long t_leaves_q2;
  long t_enters_ser;
  long t_leaves_ser;
  double serv_time;
} Customer;
// Global mode to be used by all threads
int mode;
int B = 10;
int all_packets_arrived = 0;
long emuln_start_time;
long emuln_end_time;

//Depth of token bucket
int curr_token_depth = 0;
int shutdown = 0;

// Rate of tokens being generated
double r = (double)1.5;

int all_packets = 0;
int arrived_packets = 0;
int packets_completed = 0;
int packets_dropped  = 0;
int total_tokens = 0;
int tokens_dropped = 0;
int packets_removed = 0;
long total_int_arrv_time = 0;
long total_packet_serv_time = 0;
long long total_time_in_q1 = 0;
long total_time_in_q2 = 0;
long total_time_in_s1 = 0;
long total_time_in_s2 = 0;
long long total_time_system = 0;
double square_time_system = 0;

// Sigset to handle control C
sigset_t set;
int cntrl_c_flag = 0;

// initialize mutex and condition variable pthreads

pthread_t packet_thread;
pthread_t token_thread;
pthread_t server1;
pthread_t server2;
pthread_t monitor;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv = PTHREAD_COND_INITIALIZER;

My402List *queue_1,*queue_2;

/*************************************************

     This Function prints the Parameters
     in the supplied to the start of the program

*************************************************/
void print_parameters(void *obj,int mode){


    if (mode == 0){
      fputs("Emulation Parameters:\n",stdout);
       arguments *p1 = (arguments*) obj;
       all_packets =  p1->total_packets;

       fprintf(stdout,"\tnumber to arrive = %d\n",p1->total_packets);
       fprintf(stdout,"\tlambda = %0.6g\n",((double)1000*1000/(double)p1->p_inter_arr_time));
       fprintf(stdout,"\tmu = %0.6g\n",((double)1000/(double)p1->serv_time));
       fprintf(stdout,"\tr = %0.6g\n",r);
       fprintf(stdout,"\tB = %d\n",B);
       fprintf(stdout,"\tP = %d\n",p1->tok_req);
    }
    else{
      FILE *fp;
      char line[BUFFER];
      char *temp;
      fp = (FILE*)obj;
      fgets(line,BUFFER,fp);
      //temp = line;
      temp = strtok(line," \n");

      //printf("First line %s",temp);
      all_packets = (strtol(temp,NULL,10));
      if(all_packets == 0){
        fputs("Input file is not in the right format\n",stderr);
        exit(EXIT_FAILURE);
      }
      fputs("Emulation Parameters:\n",stdout);
      fprintf(stdout,"\tnumber to arrive = %d\n",all_packets);
      fprintf(stdout,"\tr = %0.6g\n",r);
      fprintf(stdout,"\tB = %d\n",B);
    }

  }


/*************************************************

     This Function prints the Parameters
     in the supplied to the start of the program

*************************************************/
void std_dev(){
  double average_square_total;
  double square_average_total;

  if(packets_completed == 0)
  {
    fputs("\tstandard deviation for time spent in system = N/A , No packets have been completed\n",stdout);
  }
  else{
  average_square_total = ((double)square_time_system/(double)(packets_completed));
  square_average_total = (((double)total_time_system/(double)(packets_completed*1000*1000))*((double)total_time_system/(double)(packets_completed*1000*1000)));


  fprintf(stdout,"\tstandard deviation for time spent in system = %0.6g\n",sqrt((average_square_total - square_average_total)));
 }
}
/*************************************************

    This Function prints the stats
    in the supplied to the start of the program

*************************************************/
void gather_stats()
{
  fputs("\n",stdout);
  fputs("Statistics: \n",stdout);
  fputs("\n",stdout);
  if(all_packets == 0){
    fputs("\taverage packet inter-arrival time = N/A. No packets have arrived\n",stdout);
  }
  else{
  fprintf(stdout,"\taverage packet inter-arrival time = %0.6g\n",((double)total_int_arrv_time/(double)(arrived_packets*1000*1000)));
  }
  if(packets_completed == 0){
    fputs("\taverage packet service time = N/A. No packets have been completed\n",stdout);
  }
  else{
  fprintf(stdout,"\taverage packet service time = %0.6g\n",((double)total_packet_serv_time/(double)(packets_completed*1000*1000)));
  }
  fputs("\n",stdout);
  //printf("total_time_in_q1 %lld\n",total_time_in_q1);
  //printf("Denominator %0.6g\n",(double)(emuln_end_time-emuln_start_time));
  //printf("Denominator %0.6g\n",((double)total_time_in_q1/(double)(emuln_end_time-emuln_start_time)));

  fprintf(stdout,"\taverage number of packets in Q1 = %0.6g\n",((double)total_time_in_q1/(double)(emuln_end_time-emuln_start_time)));
  fprintf(stdout,"\taverage number of packets in Q2 = %0.6g\n",((double)total_time_in_q2/(double)(emuln_end_time-emuln_start_time)));
  fprintf(stdout,"\taverage number of packets in S1 = %0.6g\n",((double)total_time_in_s1/(double)(emuln_end_time-emuln_start_time)));
  fprintf(stdout,"\taverage number of packets in S2 = %0.6g\n",((double)total_time_in_s2/(double)(emuln_end_time-emuln_start_time)));
  fputs("\n",stdout);
  if(packets_completed == 0){
     fputs("\taverage time a packet spent in system  = N/A. No packets have been completed\n",stdout);
  }else{
    fprintf(stdout,"\taverage time a packet spent in system  =  %0.6g\n",((double)total_time_system/(double)(packets_completed*1000*1000)));
  }
std_dev();
  fputs("\n",stdout);
  if(total_tokens == 0){
    fputs("\ttoken drop probability = N/A No tokens arrived.",stdout);
  }
  else{
    fprintf(stdout,"\ttoken drop probability = %0.6g\n",((double)tokens_dropped/(double)total_tokens));
  }
  if(arrived_packets == 0){
    fputs("\tpacket drop probability = N/A No packets arrived.\n",stdout);
  }
  else{
  fprintf(stdout,"\tpacket drop probability = %0.6g\n",((double)packets_dropped/(double)arrived_packets));
 }
}
/*************************************************

    This Function checks if the current values
    are integers or not
    return type is TRUE or FALSE

*************************************************/

int isInteger(char *arg)
{
  int i;
  int int_val;
  /*
    Check each argument given is a number/digit
    rejecting negative integers and real numbers.
    Only integers accepted
    Also Check for value greater than 2147483647
  */

  for(i= 0;i<strlen(arg);i++)
  {
    if(isdigit(arg[i]) == 0)
    {
        return FALSE;
    }
  }

  int_val = strtol(arg,NULL,10);
  if(int_val > 2147483647 )
  {
     return FALSE;
  }

  return TRUE;

}
/*************************************************

   This Function Checks if arg sent is a positive
   value or not
   Returns TRUE or FALSE accordingly

*************************************************/

int isPositive(char *arg)
{
  int i;
  /*
    Check each argument given is a number/digit
    Handling negative integers also
  */

  for(i= 0;i<strlen(arg);i++)
  {
    if(isdigit(arg[i]) == 0)
    {
      if(arg[i] != '.')
      {
        return FALSE;
      }
    }
  }
  return TRUE;

}

/*********************************************************
return timeval in microseconds
*********************************************************/

double gettime()
{
  struct timeval val;

  gettimeofday(&val,NULL);
  return (((double)(val.tv_sec*1000*1000) + (double)(val.tv_usec)));
}
/*********************************************************
  Error handling for each line of the file
  1) Check the number of tabs
*********************************************************/

int errorCheckLine (char *line)
{
  char *temp;
  int tabcount;
    /*
    Count the number of tabs in each line
    if the number of tabs is greater than 3
    print error and exit
    */
    tabcount = 0;
    temp = line;
    fprintf(stdout, "%s\n", strchr(temp,'\t'));
    while(strchr(temp,' ') != NULL)
    {
       temp++;
       tabcount++;
    }
    /*
     The number of elements should be 3
     else should exit with error
    */
    if (tabcount == 2)
    {
      return TRUE;
    }
    else
    {
      fprintf(stdout,"\nTabcount %d\n",tabcount);
      fputs("Incorrect elements in the line of file\n",stderr);
      return FALSE;
    }

}

/*************************************************
This Function creates a packet node to be
added to the list
Run for both Modes
*************************************************/

arguments *create_packet_node(double arr_time,int token_num, double service_time,int packet_num)
{
  arguments *p1 = malloc(sizeof(arguments));
  p1->p_inter_arr_time = arr_time;
  p1->total_packets = packet_num;
  p1->serv_time = service_time;
  p1->tok_req = token_num;
  return p1;

}
/*************************************************
  Moves packet from q1 to q2 and broadcasts
  to server threads to wakeup if q2 was empty
*************************************************/
void Move_Packet()
{
  /*
     Check the head of the packet
     and Send it to queue_2 if it has current depth
  */
  My402ListElem *elem =  My402ListFirst(queue_1);
  Customer *head = malloc(sizeof(Customer));
  head = (Customer *)(elem->obj);

  if(head->tok_req <= curr_token_depth)
  {
      My402ListUnlink(queue_1,elem);
      head->t_leaves_q1 = gettime();
      curr_token_depth = curr_token_depth - head->tok_req;
      if(My402ListAppend(queue_2,(void *)head) == 0)
      {
          fputs("Error while inserting into queue_2\n",stderr);
      }
      else
      {
          head->t_entered_q2 =gettime();
          fprintf(stdout,"%012.3lfms: p%d leaves Q1, time in Q1 = %0.3fms,token bucket now has %d token\n",((double)(gettime()-emuln_start_time)/(double)1000),head->packet_no,((double)(head->t_leaves_q1 - head->t_entered_q1)/(double)1000),curr_token_depth);
          fprintf(stdout,"%012.3lfms: p%d enters Q2\n",((double)(gettime()-emuln_start_time)/(double)1000),head->packet_no);
          //printf("total_time in Q1 %lld\n",total_time_in_q1);

      }
  }

}

/*************************************************
This is a cleanup function
*************************************************/
/*{
  pthread_mutex_unlock(&m);
}
*/

/*************************************************
This function is a wrapper of the steps of the
packet arrival thread.
Run for both Modes
*************************************************/

void *packet_process(void *obj)
{
  FILE *fp;
  int i;
  long sleep_time = 0;
  long d_inter_arrv_time =0;
  long serv_time = 0;
  long arrv_time = 0;
  long temp = 0;
  long prev_arrv_time = emuln_start_time;
  int tok_req = 0;
  char line[BUFFER];
  char *tokens,*c_inter_arrv_time,*c_serv_time,*c_tok_req;
  //int errCheck = 0;

  //pthread_cleanup_push(cleanup,NULL);
  if (mode == 0)
  {
    arguments *p = (arguments *)obj;
    for(i = 1; i <=((arguments *)obj)->total_packets;i++)
   {
     /*
      Sleep for certain arrival time of the packet
    */
     temp = gettime();

    /*
      Get inter arrival time
    */
    sleep_time = p->p_inter_arr_time-(temp - prev_arrv_time);


   if(sleep_time >0)
   {
    if(usleep((sleep_time)) != 0)
    {
      fputs("Error while sleeping",stderr);
      exit(0);
    }
  }

    arrv_time = gettime();
    total_int_arrv_time = total_int_arrv_time + (arrv_time - prev_arrv_time);
    arrived_packets++;

    if(p->tok_req <= B){

      fprintf(stdout,"%012.3lfms: p%d arrives, needs %d tokens, inter-arrival time = %0.3lfms\n",((double)(arrv_time-emuln_start_time)/(double)1000),i,p->tok_req,((double)(arrv_time - prev_arrv_time)/(double)1000));
      prev_arrv_time = arrv_time;
    }
    else{

      fprintf(stdout,"%012.3lfms: p%d arrives, needs %d tokens, inter-arrival time = %0.3lfms, dropped\n",((double)(arrv_time-emuln_start_time)/(double)1000),i,p->tok_req,((double)(arrv_time - prev_arrv_time)/(double)1000));
      packets_dropped++;
      //fprintf(stdout,"%d\n",packets_dropped);
      prev_arrv_time = arrv_time;

      continue;
    }


    /* Create Customer Object
    */
    Customer *cust = malloc(sizeof(Customer));
    cust->arrv_time = arrv_time;
    cust->packet_no = i;
    cust->tok_req = p->tok_req;
    cust->serv_time = p->serv_time;


    pthread_mutex_lock(&m);

    cust->t_entered_q1 = gettime();

    if(My402ListAppend(queue_1,(void*)cust) == 0)
    {
      fputs("Error while inserting into queue_1\n",stdout);
    }
    else
    {
      fprintf(stdout,"%012.3lfms: p%d enters Q1\n",((double)(gettime()-emuln_start_time)/(double)1000),cust->packet_no);
    }
   if(My402ListEmpty(queue_1) == 0)
   {
    Move_Packet();
   }
   if(queue_2->num_members == 1)
   {
     pthread_cond_broadcast(&cv);
   }
    pthread_mutex_unlock(&m);
  }

  /*
    Update global variable all packets arrived
  */
   all_packets_arrived = 1;
  }
  else// for mode 1
  {
    fp = (FILE *)obj;
    i = 1;
    //printf("All packets %d\n",all_packets);
    while(i <= all_packets)
    {


      fgets(line,BUFFER,fp);
      //printf("enter pkt ");

      //fprintf(stdout,"Line in file %s\n",line);
      /*errCheck = errorCheckLine(line);

      if (errCheck == 0)
     {
        exit(0);
     }
     */

      tokens = malloc(strlen(line));
      strcpy(tokens,line);

      c_inter_arrv_time = strtok(tokens," \t\n");
      c_tok_req = strtok(NULL," \t\n");
      c_serv_time = strtok(NULL," \t\n");
      //printf("Inter Arrive time %s\n",c_inter_arrv_time);
      //printf("C_tok_Req %s\n",c_tok_req);
      //printf("c_serv_times %s\n",c_serv_time);

      if(isInteger(c_inter_arrv_time) == 0 ||
         isInteger(c_tok_req) == 0 ||
         isInteger(c_serv_time) == 0)
      {
        fputs("Line is in incorrect format",stderr);
        exit(0);
      }

      d_inter_arrv_time = (strtod(c_inter_arrv_time,NULL)*1000);
      tok_req  = strtol(c_tok_req,NULL,10);
      serv_time = strtol(c_serv_time,NULL,10);

      temp = gettime();
      sleep_time = d_inter_arrv_time - (temp - prev_arrv_time);

     if(sleep_time > 0)
     {
      if(usleep(sleep_time) != 0)
      {
        fputs("Error while sleeping",stderr);
        exit(0);
      }
     }
     else
     {
       usleep(0);
     }

      arrv_time = gettime();
      total_int_arrv_time = total_int_arrv_time + (arrv_time-prev_arrv_time);
      arrived_packets++;


      if(tok_req <= B){

        fprintf(stdout,"%012.3lfms: p%d arrives, needs %d tokens, inter-arrival time = %0.3lfms\n",((double)(arrv_time-emuln_start_time)/(double)1000),i,tok_req,((double)(arrv_time-prev_arrv_time)/(double)1000));
        prev_arrv_time = arrv_time;
      }
      else{

        fprintf(stdout,"%012.3lfms: p%d arrives, needs %d tokens, inter-arrival time = %0.3lfms, dropped\n",((double)(arrv_time-emuln_start_time)/(double)1000),i,tok_req,((double)(arrv_time-prev_arrv_time)/(double)1000));
        packets_dropped++;

        //fprintf(stdout,"%d\n",packets_dropped);
        i++;
        prev_arrv_time = arrv_time;
        continue;
      }
      /*
         Create customer object
      */
      Customer *cust = malloc(sizeof(Customer));
      cust->arrv_time = arrv_time;
      cust->tok_req  = tok_req;
      cust->serv_time = serv_time;
      cust->packet_no = i;

      i++;
      pthread_mutex_lock(&m);

      cust->t_entered_q1 = gettime();
      if(My402ListAppend(queue_1,(void*)cust) == 0)
      {
        fputs("Error while inserting into queue_1\n",stdout);
      }
      else
      {
        fprintf(stdout,"%012.3lfms: p%d enters Q1\n",((double)(gettime()-emuln_start_time)/(double)1000),cust->packet_no);
      }
      if(My402ListEmpty(queue_1) == 0)
      {
       Move_Packet();
      }
      if(queue_2->num_members == 1)
      {
        pthread_cond_broadcast(&cv);
      }
      pthread_mutex_unlock(&m);
      free(tokens);

    }

      /*
        Update global variable all packets arrived
      */
       all_packets_arrived = 1;
  }
  //pthread_cleanup_pop(0);
  return 0;
}
/*************************************************
This function is the token bucket thread
Runs in both modes.
*************************************************/
void *token_bucket_process()
{
  //pthread_cleanup_push(cleanup(),NULL);
  long token_arrive = 0;
  long prev_token_arrive = emuln_start_time;
  long temp = 0;
  double token_sleep_time = 0;
  total_tokens = 0;
  while(all_packets_arrived == 0 || My402ListEmpty(queue_1) == 0)
  {
    //printf("enter tok \n");
    temp = gettime();
    token_sleep_time = ((double)(1*1000*1000/r)) - (temp - prev_token_arrive);

    if (token_sleep_time > 0){
         if(usleep(token_sleep_time) != 0)
         {
           fputs("Error while sleeping",stderr);
           exit(0);
         }
       }
       else
       {
         usleep(0);
       }
    pthread_mutex_lock(&m);
    if(all_packets_arrived == 1 && My402ListEmpty(queue_1) == 1)
    {
        //shutdown = 1;
        if(My402ListEmpty(queue_2) == 1)
        {
          shutdown = 1;
        }
        //cntrl_c_flag = 1;
        pthread_cond_broadcast(&cv);
        pthread_mutex_unlock(&m);
        break;
    }
    token_arrive = gettime();
    total_tokens++;
    if (curr_token_depth < B){
      curr_token_depth++;
      fprintf(stdout,"%012.3lfms: token t%d arrives, token bucket now has %d tokens\n",((double)(gettime()-emuln_start_time)/(double)1000),total_tokens,curr_token_depth);
    }
    else{
      fprintf(stdout,"%012.3lfms: token t%d arrives, dropped\n",((double)(gettime()-emuln_start_time)/(double)1000),total_tokens);
      tokens_dropped++;
    }
    prev_token_arrive = token_arrive;

    if(My402ListEmpty(queue_1) == 0)
    {
     Move_Packet();
    }

    if(queue_2->num_members == 1)
    {
      pthread_cond_broadcast(&cv);
    }
    pthread_mutex_unlock(&m);

  }
/*
  if(My402ListEmpty(queue_1) == 1 && My402ListEmpty(queue_2) == 1 && all_packets_arrived == 1)
  {
                  shutdown = 1;
  }
*/

  //pthread_cleanup_pop(0);
  pthread_exit(NULL);
}
/*************************************************
This function is the server processing thread
Runs in both modes.
*************************************************/
void *server_process(void *obj)
{
  int server_no;
  double serv_time;

  server_no =  (int)(obj);
  while(shutdown == 0 || My402ListEmpty(queue_2) == 0 /*|| cntrl_c_flag  == 0*/)
  {

    //printf("locking mutex server  no %d\n", server_no);
    pthread_mutex_lock(&m);
    /*
     Condition wait if Q2 is empty
    */

    //printf("Next IF\n");
    if((My402ListEmpty(queue_1) == 1 && My402ListEmpty(queue_2) == 1 && all_packets_arrived == 1)|| cntrl_c_flag == 1)
    {
            shutdown =1;
            /*cntrl_c_flag=1;*/
            pthread_cond_broadcast(&cv);
            pthread_mutex_unlock(&m);
            pthread_exit(NULL);
    }

      while( My402ListEmpty(queue_2) == 1 && shutdown == 0)
      {
        if(cntrl_c_flag == 1)
        {

          pthread_mutex_unlock(&m);
          pthread_exit(NULL);
        }

          pthread_cond_wait(&cv,&m);
        if(cntrl_c_flag == 1)
        {

            pthread_mutex_unlock(&m);
            pthread_exit(NULL);
        }
      }

    if(My402ListEmpty(queue_2) == 0)
    {
           //printf("Inside IF\n");
            My402ListElem *head = My402ListFirst(queue_2);

            Customer *cust = malloc(sizeof(Customer));
            cust = (Customer *)(head->obj);
            serv_time = cust->serv_time;
            My402ListUnlink(queue_2,head);
            cust->t_leaves_q2 = gettime();
            cust->t_enters_ser =gettime();
            fprintf(stdout,"%012.3lfms: p%d leaves Q2, time in Q2 = %0.3lfms\n",((double)(gettime()-emuln_start_time)/(double)1000),cust->packet_no,((double)(cust->t_leaves_q2 - cust->t_entered_q2)/(double)1000));
            /*
              Record the total_time in Q2
            */
            total_time_in_q2 = total_time_in_q2 + (cust->t_leaves_q2 - cust->t_entered_q2);
            fprintf(stdout,"%012.3lfms: p%d begins service at S%d, requesting %0.3lfms of service\n",((double)(gettime()-emuln_start_time)/(double)1000),cust->packet_no,server_no,cust->serv_time);

            pthread_mutex_unlock(&m);
            usleep((serv_time*1000));
            cust->t_leaves_ser = gettime();
            total_packet_serv_time = total_packet_serv_time + (cust->t_leaves_ser - cust->t_enters_ser);

            pthread_mutex_lock(&m);
            fprintf(stdout,"%012.3lfms: p%d departs from S%d, service time = %0.3lfms , time in system = %0.3lf\n",((double)(gettime()-emuln_start_time)/(double)1000),cust->packet_no,server_no,((double)(cust->t_leaves_ser - cust->t_enters_ser)/(double)1000),((double)(cust->t_leaves_ser - cust->arrv_time)/(double)1000));
            /*
                record total time in server 1 or server 2 based on number
            */
            total_time_in_q1 =  total_time_in_q1 + (cust->t_leaves_q1 - cust->t_entered_q1);
            total_time_system = total_time_system + (cust->t_leaves_ser - cust->arrv_time);
            square_time_system = square_time_system + (((double)(cust->t_leaves_ser - cust->arrv_time)/(double)(1000*1000))*((double)(cust->t_leaves_ser - cust->arrv_time)/(double)(1000*1000)));

            packets_completed++;

            pthread_mutex_unlock(&m);
            if(server_no  == 1){

              total_time_in_s1 = total_time_in_s1 + (cust->t_leaves_ser - cust->t_enters_ser);
            }
            else{

              total_time_in_s2 = total_time_in_s2 + (cust->t_leaves_ser - cust->t_enters_ser);

            }

            free(cust);
      }else
      {
        pthread_mutex_unlock(&m);
      }

  }
  pthread_exit((void *)1);
}

/*************************************************

  Do steps of control C handling

*************************************************/
void remove_packets(My402List *pList, int queue_no)
{
  int packet_no;
   while(My402ListEmpty(pList) == 0)
   {

     packet_no = ((Customer *)(My402ListFirst(pList)->obj))->packet_no;
     My402ListUnlink(pList,My402ListFirst(pList));
     fprintf(stdout,"%012.3lfms: p%d is removed from Q%d\n",((double)(gettime()-emuln_start_time)/(double)1000),packet_no,queue_no);
   }
}
/*************************************************

   This function monitors Control C and does
   Processing once Control C is received

*************************************************/
void *monitor_process()
{
  //int sig = 0;
     sigwait(&set);

     pthread_mutex_lock(&m);
     //printf("Monitor Lock\n");
     pthread_cancel(token_thread);
     //printf("Token Thread Cancel\n");
     pthread_cancel(packet_thread);
     //printf("Packet Threads Cancel\n");
     remove_packets(queue_2, 2);
     //printf("Remove Packets queue_2\n");
     remove_packets(queue_1, 1);
     //printf("Remove Packets queue_1\n");
     shutdown = 1;
     cntrl_c_flag = 1;
     pthread_cond_broadcast(&cv);
     pthread_mutex_unlock(&m);
     //printf("Mutex Unlock\n");
     pthread_exit(NULL);
}

/*************************************************
This function is the main driver thread which
creates all the threads needed to service the packet_list
Runs in both modes.
Input = Argument structure for mode 0
        File pointer for mode 1
*************************************************/

void emulator_thread(void *obj)
{

 /*
   Initialise all the lists
 */
 //memset(queue_1,0,sizeof(My402List));
 //memset(queue_2,0,sizeof(My402List));

 queue_1 = malloc(sizeof(My402List));
 queue_2 = malloc(sizeof(My402List));
 (void)My402ListInit(queue_1);
 (void)My402ListInit(queue_2);
 emuln_start_time =  gettime();
 fputs("00000000.000ms: emulation begins\n",stdout);
 /*
   Do processing based on mode
 */

 pthread_create(&monitor,NULL,&monitor_process,NULL);
 pthread_create(&packet_thread,NULL,&packet_process,obj);
 pthread_create(&token_thread,NULL,&token_bucket_process,NULL);
 pthread_create(&server1,NULL,&server_process,(void*)1);
 pthread_create(&server2,NULL,&server_process,(void*)2);
/*
  Join the threads once they are complete
*/
pthread_join(packet_thread,0);
pthread_join(token_thread,0);
//printf(" before s1\n");

pthread_join(server1,0);
//printf("after s1\n");

pthread_join(server2,0);
pthread_cancel(monitor);

//printf("after s2\n");


fprintf(stdout,"%012.3lfms: emulation ends\n",((double)(gettime()-emuln_start_time)/(double)1000));
emuln_end_time = gettime();
gather_stats();

free(queue_1);
free(queue_2);
}

/*************************************************
This Function runs when the mode is 0.
Reads all the arguments from the commandline
and generates an argument node
Mode 0 = Deterministic Mode
*************************************************/

void check_arguments(int argc,char *argv[])
{
  int i;
  arguments *p1;
  /*
   Default Values for each of the commandline arguments
  */
  double lambda = (double)1;
  double mu = (double)0.35;
  int num = 20;
  int P = 3;
/*
    Check each of the commandline arguments
    to determine if they are positive numbers
  */
  for (i = 0 ;i < argc ; i++)
  {
    if(strcmp(argv[i],"-lambda") == 0)
    {
      if(isPositive(argv[i+1]) == 0)
      {
        fputs("lambda value is incorrect\n",stderr);
        exit(0);
      }
      else
      {
        sscanf(argv[i+1],"%lf",&lambda);
      }
        i++;
    }


    if(strcmp(argv[i],"-mu") == 0)
    {
      if(isPositive(argv[i+1]) == 0)
      {
        fputs("mu value is incorrect\n",stderr);
        exit(0);
      }
      else
      {
        sscanf(argv[i+1],"%lf",&mu);
      }
        i++;
    }

    if(strcmp(argv[i],"-r") == 0)
    {
      if(isPositive(argv[i+1]) == 0)
      {
        fputs("r value is incorrect\n",stderr);
        exit(0);
      }
      else
      {
        sscanf(argv[i+1],"%lf",&r);
      }
        i++;
    }

    if(strcmp(argv[i],"-B") == 0)
    {
      if(isPositive(argv[i+1]) == 0 || isInteger(argv[i+1]) == 0)
      {
        fputs("B value is incorrect\n",stderr);
        exit(0);
      }
      else
      {
        sscanf(argv[i+1],"%d",&B);
      }
        i++;
    }

    if(strcmp(argv[i],"-P") == 0)
    {
      if(isPositive(argv[i+1]) == 0 || isInteger(argv[i+1]) == 0)
      {
        fputs("P value is incorrect\n",stderr);
        exit(0);
      }
      else
      {
        sscanf(argv[i+1],"%d",&P);
      }
        i++;
    }

    if(strcmp(argv[i],"-n") == 0)
    {
      if(isPositive(argv[i+1]) == 0 || isInteger(argv[i+1]) == 0)
      {
        fputs("num value is incorrect\n",stderr);
        exit(0);
      }
      else
      {
        sscanf(argv[i+1],"%d",&num);
      }
        i++;
    }

  }
  /*
    Create a single packet for this mode and send it for processing
  */
    p1 = malloc(sizeof(arguments));
    if(lambda < 0.1)
    {
      lambda = 0.1;
    }
    p1->p_inter_arr_time = (double)(1*1000*1000/lambda);
    p1->total_packets = num;
    if(mu < 0.1 )
    {
      mu = 0.1;
    }
    p1->serv_time = (double)(1*1000/mu);
    p1->tok_req = P;
    if(r < 0.1)
    {
      r = 0.1;
    }

  print_parameters(p1,mode);

  emulator_thread((void *)p1);
  free(p1);
}


/*************************************************
The main function takes the commandline
arguments and processes them to determine
which mode to Run
Mode 0 = Deterministic Mode
Mode 1 = Trace Driven Mode
*************************************************/

int main(int argc, char *argv[]) {
int i,j;
char *filename;
FILE *fp;
/*
   Block Signal Control C
*/

sigemptyset(&set);
sigaddset(&set, SIGINT);
sigprocmask(SIG_BLOCK, &set, 0);

/*
  Check if the usage information is correct.
  i.e. check if the commandline arguments are correct
*/

if((argc%2) == 0)
{
        fputs("Incorrect command line options\n",stderr);
        fputs("Usage :  warmup2 [-lambda lambda] [-mu mu] [-r r] [-B B] [-P P] [-n num] [-t tsfile]\n",stderr);
        exit(0);
}
for (i = 1; i< argc; i++)
{
  if (strcmp(argv[i],"-t") == 0 || (strcmp(argv[i],"-lambda") == 0)
      || (strcmp(argv[i],"-mu") == 0) || (strcmp(argv[i],"-B") == 0)
      || (strcmp(argv[i],"-P") == 0) || (strcmp(argv[i],"-r") == 0)
      || (strcmp(argv[i],"-n") == 0))
      {
        /*
         Don't do anything yet.
         Just Checking if arguments are correct.
        */
        i++;
      }
  else
     {
       fputs("Incorrect command line options\n",stderr);
       fputs("Usage :  warmup2 [-lambda lambda] [-mu mu] [-r r] [-B B] [-P P] [-n num] [-t tsfile]\n",stderr);
       exit(0);
     }
}
for (i = 0 ; i< argc ; i++)
{
    if (strcmp(argv[i],"-t") == 0)
    {
      filename = argv[i+1];
      /*
        Check file
      */
      struct stat buf;

      stat(filename,&buf);
      if(S_ISDIR(buf.st_mode))
      {
        fprintf(stderr,"%s:File is a directory\n",filename);
        exit(EXIT_FAILURE);
      }

      fp = fopen(filename,"r");
      if(fp ==  NULL)
      {
        fprintf(stderr,"%s:Could not open file\n",filename);
        exit(EXIT_FAILURE);
      }
      mode = 1;
      for(j = 0 ; j<argc; j++)
      {
        if(strcmp(argv[j],"-r") == 0)
        {
          if(isPositive(argv[j+1]) == 0)
          {
            fputs("r value is incorrect\n",stderr);
            exit(0);
          }
          else
          {
            sscanf(argv[j+1],"%lf",&r);
          }
            j++;
        }

        if(strcmp(argv[j],"-B") == 0)
        {
          if(isPositive(argv[j+1]) == 0 || isInteger(argv[j+1]) == 0)
          {
            fputs("B value is incorrect\n",stderr);
            exit(0);
          }
          else
          {
            sscanf(argv[j+1],"%d",&B);
          }
            j++;
        }

      }
      print_parameters(fp,mode);
      fprintf(stdout,"\ttsfile = %s\n",filename);

      fputs("\n",stdout);
      emulator_thread((void *)fp);
      fclose(fp);
      break;
    }
}
if (mode == 0)
{
  check_arguments(argc ,argv);
}
  return 0;
}
