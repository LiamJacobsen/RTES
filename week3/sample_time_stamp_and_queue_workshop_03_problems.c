/* sample_time_test_and_queue.c */

/* created by Andrew A.
   Sun Aug  6 09:39:42 ACST 2023 */

/*

There are 6 problems to fix or to think about, for 3 marks weichting

PROBLEM #1 to think about : Is counting the best way to determine when processes should stop.
   What happens if there are concurrent processes?

PROBLEM #2 to think aboout: The target times are increasing, but what happens if this increase starts late?

PROBLEM #3 to fix : For how long should the application, ie the "usleep()" actually run? 

PROBLEM #4 to fix : We need to insert a "busy" wait loop , and stop this loop at the correct time.
                    How do we know what the correct time is?
                    Are "busy-waits" a waste of processor time?
                    
PROBLEM #5 to fix : capture the actual time, not a "0", place-holder.

PROBLEM #6 to fix : capture the actual random number, to serve as a sample, not a "0", place-holder.

*/



/*

Compilation advice:

gcc -std=gnu99 -Wall -Wextra -Werror -o sample_time_stamp_and_queue_workshop_03_problems sample_time_stamp_and_queue_workshop_03_problems.c


Times are in nanoseconds.
*/

#include <stdio.h>     /* needed for printf() */
#include <stdint.h>    /* defines some types, including: int64_t */
#include <time.h>      /* needed for clock_gettime() and  CLOCK_MONOTONIC*/
#include <unistd.h>    /* needed for usleep() */
#include <stdlib.h>

/* Some standard time durations, in microseconds */
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
/* PROBLEM #1 to think about : Is counting the best way to determine when processes should stop.
   What happens if there are concurrent processes? */
   //No, counting is computationally wasteful and if another program is running, the time taken for each iteration may be undefined due to other processes taking place
   //at the same time. 
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
  int j = 0 ;  /* a counter*/  
  time_t t;    /* a different time value */

    /* Intializes random number generator */
    srand((unsigned) time(&t));

    int64_t target_time ;

    queue_item input_value ;     /* reserved for input values */
    int        error_code = 0;   /* used for return-values, error codes */
    
    /* initialise the temporary storage values */
    input_value.time_value    = 0;
    input_value.sample_value  = 0;
    
    /* start the clock, and set "start", pointed to by "&start"*/
    clock_gettime(CLOCK_MONOTONIC, &start);

    target_time = elapsed_time();
    
    /* The code that we are interested in measuring, starts here... */
    for (k=0; k<N_STAMPS; k++)
        {
            /* PROBLEM #2 to think aboout: The target times are increasing, but what happens if this increase starts late? */
            //the target time will increasingly shift away from the actual intended target time, resulting in the program taking gradually more time than was desired/intended.
	         target_time = target_time + (TARGET_STEP*1000) ;
	         /* Note that there are 1000 nanoseconds in a microsecond. */

	  
          /* sleep for a while...  zzzz... Use up some time...*/
             /* PROBLEM #3 to fix : For how long should the application, ie the "usleep()" actually run?  */
	        (void) usleep(SLEEP_TIME); /* suspend execution for a defined time interval */
	        /* In principle a task could be performing real useful work here, mining Bitcoin, whatever... */

          /* perform a short busy-wait, in order to regulate the timing */
           /* PROBLEM #4 to fix : need to insert a "busy" wait loop here, and stop this loop at the correct time?
                                  How do we know what the correct time is?
                                  Are "busy-waits" a waste of processor time? */
	        while (elapsed_time() < target_time) j++ ;
          /* j++ represents a very minimal amount of "busy-work" ...*/

          /* get the next list item*/
          (void) get_queue_item(&input_value);

          /* add the latest item to the list*/
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
    printf("The list is: (time_value,sample_value) :\n{\n");

    for (k=1; k <= n_items; k++)
    {
        sub = (next_sub+(MAX_LENGTH-k)) % MAX_LENGTH ;
        printf("   (%d,%d)\n", (queue_store[ sub ]).time_value, (queue_store[ sub ]).sample_value );
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

    /* collect a time value */
    /* PROBLEM #5 to fix : capture the actual time, not "0"*/
    (*input_value).time_value = (long) elapsed_time();

    /* collect a virtual (random) sample */
    /* PROBLEM #6 to fix : capture the actual random number, to serve as a sample, not "0"*/
    (*input_value).sample_value = (long) rand() ;

    return 0 ;

}


/*
We want the output to look like:

$ ./sample_time_stamp_and_queue_solution

The list is:(time_value,sample_value) :
{
   (900000496,2053413389)
   (800000500,1529450557)
   (700000463,179815384)
   (600000412,489136646)
   (500000263,1141979128)
   (400000412,1418511951)
   (300000246,1750479408)
   (200000548,1390924493)
   (100000385,1030816114)
}

$


Notes: 

#1: Please note how the times are very regular, but not perfect.
A timing loop removes most of th ejitter, but not all of it.

#2: Please note how the samples are "random", but only pseudo random in a range.

#3: Please note that all values were fored as long in values. It is typical in low-level programming that 
numberts are of some sort of "int" type, bacause of the way in which clocks and data-acquisition are generally built.

*/