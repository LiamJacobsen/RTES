/* sample_time_test_and_queue_workshop_01.c */

/* created by Andrew A.
   Sun 07 Aug 2022 22:00:21 ACST */

/*

Compilation advice:

gcc -std=gnu99 -Wall -Wextra -Werror -o sample_time_stamp_and_queue_workshop_01 sample_time_stamp_and_queue_workshop_01.c

Times are in nanoseconds.
*/

#include <stdio.h>     /* needed for printf() */
#include <stdint.h>    /* defines some types, including: int64_t */
#include <time.h>      /* needed for clock_gettime() and  CLOCK_MONOTONIC*/
#include <unistd.h>    /* needed for usleep() */
#include <stdlib.h>

#define TARGET_STEP 100000
#define SLEEP_TIME   83160
/* allow a 20% margin 
   This means that the process is performing the task (really) a sleep for about 80% of the time 
   and performing the bust loop for 20% of the time.
   This is a large fraction because the jitter is quite bad in a GPOS (such as MacOS).
   Over-runs of 14% have been observed */ 


/* 
   100000 microseconds corresponds to a sleep of 0.1 second, 
   which is within the range of easy human perception.

   The value 83160 is 16140 microseconds short of 100000
   This is enough time to guarantee a very small rate of over-run
   even with the unreliability of usleep(), plus jitter, in a GPOS ... 
   
*/

/* number of items to generate */
#define N_STAMPS    9

/* total capacity of the queue */
#define MAX_LENGTH  32


/* Set up structures for the starting and stopping times */
static struct timespec start, end;
/* notice that these are global, and they are quite complicated */

/* We are free to define the items that will be in a queue_item */
typedef struct
{
    int time_value;
    int sample_value;
} queue_item;

/* function prototypes */

/* 
   A user function to extract the number of nanoseconds that have elapsed,
   between two times.
*/
static int64_t timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p);

/* A user-defined function to extract the number of nanoseconds that have elapsed, in total*/
static int64_t elapsed_time(void);
/* This is just a "wrapper" for timespecDiff() */

int  add_list_item(queue_item);
void print_list_items(void);
int  dispatch_list_item(queue_item *);
int  get_queue_item(queue_item* );

queue_item queue_store[MAX_LENGTH]; /* Defines the memory-space to be used by the queue */
int next_sub = 0 ; /* The subscript at which we should store the next item*/
int n_items  = 0 ; /* There are initially no items in the queue*/
/* In general, a circular queue needs two pointer type items.*/





/* A main() process that prints out random data, and time-stamps */
int main(void)
{
  /* counters */
  int k = 0 ;
  time_t t;    /* a different time value */

    /* Intializes random number generator */
    srand((unsigned) time(&t));

    int64_t target_time ;

    queue_item input_value ;     /* reserved for input values */
    int        error_code = 0;   /* used for return-values, error codes */
    queue_item output_value;     /* reserved for output values */

    /* initialise the temporary storage values */
    input_value.time_value    = 0;
    input_value.sample_value  = 0;
    output_value.time_value   = 0;
    
    /* start the clock, and set "start", pointed to by "&start"*/
    clock_gettime(CLOCK_MONOTONIC, &start);

    target_time = elapsed_time();
    
    /* The code that we are interested in measuring, starts here... */
    for (k=0; k<N_STAMPS; k++)
        {
	         target_time = target_time + (TARGET_STEP*1000) ;
	         /* Note that there are 1000 nanoseconds in a microsecond. */
	  
          /* sleep for a while...  zzzz... Use up some time...*/


          /* ### Action point: insert code here to change th esleep value to allow time for the busy loop */ 

	        (void) usleep(SLEEP_TIME); /* suspend execution for a defined time interval */
	        /* In principle a task could be performing real useful work here, mining Bitcoin, whatever... */


          /* ### Action point: insert code here to regulate the timing of the data collection */

          printf("%d\t%ld\n",k,(long) elapsed_time());


          /* get the next list item*/
          (void) get_queue_item(&input_value);

          /* add th elatest item to the list*/
          error_code = add_list_item(input_value);
             if(error_code != 0) break ; /* We can perform limited trapping of exceptions by checking return values */
  
        }

        /* print out the final list */
        print_list_items();

    return 0;
}


/* 
   A user function to extract the number of nanoseconds that have elapsed,
   between two times, *timeA_p and *timeB_p.
   The timespec structures are complicated. This function interprets them. 
   Note that the arguments are pointers to the actual structures, 
   which are global and static.
*/ 
static int64_t timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
    return (int64_t) ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
           ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec);
}


/* 
   A user function to extract the number of nanoseconds that have elapsed,
   since the starting time was reset.
   This is a convenient "wrapper" for evaluating elapsed time.
*/ 
static int64_t elapsed_time(void)
{
           /* evaluate the elapsed time, in nanoseconds, a type, similar to a long integer, called: int64_t */
            clock_gettime(CLOCK_MONOTONIC, &end);
            return (int64_t) timespecDiff(&end, &start);
}



/* //////////////////////////////////////////////////////////////////////////// */

/* insert an item into the queue */
/* This could be a First-In First-Out (FIFO) queue*/
int  add_list_item(queue_item input_value)
{

    if (n_items < MAX_LENGTH)
    {
        queue_store[next_sub].time_value = input_value.time_value;
        queue_store[next_sub].sample_value = input_value.sample_value;

        next_sub = (next_sub+1) % MAX_LENGTH ; /* increment the next_sub subscript*/
        n_items++ ;
        return 0 ; /* normal completion */
    }
    else
        return 1 ; /* return an error code, the queue is full*/
}


/* list all the items in the queue*/
void print_list_items(void)
{
    int k;
    int sub ;
    printf("The list is: {");

    for (k=1; k <= n_items; k++)
    {
        sub = (next_sub+(MAX_LENGTH-k)) % MAX_LENGTH ;
        printf(" (%d,%d) ", (queue_store[ sub ]).time_value, (queue_store[ sub ]).sample_value );
    }

    printf("}\n");

}

/* dispatch an item from the queue on an FIFO basis*/
int  dispatch_list_item(queue_item *output_value)

/* the return value is an error-code */
/* output_value is referenced by address, after the fashion of scanf() */
{

    int sub;
    if (n_items > 0 )
    {
        /* There is at least one item in the queue */

        /* access the item at the top or "head" of the queue*/
        sub = (next_sub+(MAX_LENGTH-n_items))%MAX_LENGTH ;
        (*output_value).time_value = queue_store[sub].time_value;
        (*output_value).sample_value = queue_store[sub].sample_value;

        n_items-- ; /* decrement the next_sub subscript*/

        return 0 ; /* normal completion */
    }
    else
        return 1 ; /* return an error code, the queue is full*/
}


/* Get the data for a single queue item, from stdio, and assemble it into a queue item */
int get_queue_item(queue_item* input_value)
{

     /* ### Action point: insert code here to collect time-stamps and random sample-values */

    /* collect a time value */
    (*input_value).time_value = (long) elapsed_time();

    /* collect a virtual (random) sample */
    (*input_value).sample_value = (long) rand() ;

    return 0 ;

}


/* 
> ./sample_time_stamp_and_queue_workshop_01 

0 100093000
1 200233000
2 300385000
3 402363000
4 502477000
5 602574000
6 707473000
7 812566000
8 915568000

The list is: { (0,0)  (0,0)  (0,0)  (0,0)  (0,0)  (0,0)  (0,0)  (0,0)  (0,0) }


Note that the times have jitter, and the queue does not store any useful information.

*/
