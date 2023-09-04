/* timer_test_09.c */
/* create a simple single main() task that prints out elapsed times
   with busy-wait to improve regularity */

/* created by Andrew A.
   Sun 07 Aug 2022 22:00:21 ACST */

/*

Compilation advice:

  gcc -std=gnu99 -Wall -Wextra -Werror -o timer_test_10 timer_test_10.c

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
   This is a large fraction because the jitter is quite bad in a GPOS (such as MacOS, or windows).
   Over-runs of 14% have been observed */ 


/* 
   100000 microseconds corresponds to a sleep of 0.1 second, 
   which is within the range of easy human perception.

   The value 83160 is 16140 microseconds short of 100000
   This is enough time to guarantee a very small rate of over-run
   even with the unreliability of usleep(), plus jitter, in a GPOS ... 
   
*/

#define N_STAMPS 10

/* Set up structures for the starting and stopping times */
static struct timespec start, end;
/* notice that these are global, and they are quite complicated */

/* 
   A user function to extract the number of nanoseconds that have elapsed,
   between two times.
*/
static int64_t timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p);

/* A user-defined function to extract the number of nanoseconds that have elapsed, in total*/
static int64_t elapsed_time(void);
/* This is just a "wrapper" for timespecDiff() */

/* A main() process that prints out a process ID, and time-stamps */
int main(void)
{
  /* counters */
  int k = 0 ;
  int j = 0 ;  /* a counter*/  
  long t_val ; /* a time value */ 
  time_t t;    /* a different time value */
  long sample_val ; /* rand_val is a temporary storage value for the latest random number, as a virtual sample */

    /* Intializes random number generator */
    srand((unsigned) time(&t));


    int64_t target_time ;
    
    /* start the clock, and set "start", pointed to by "&start"*/
    clock_gettime(CLOCK_MONOTONIC, &start);

    target_time = elapsed_time();
    
    /* The code that we are interested in measuring, starts here... */
    for (k=0; k<N_STAMPS; k++)
        {
	         target_time = target_time + (TARGET_STEP*1000) ;
	         /* Note that there are 1000 nanoseconds in a microsecond. */
	  
          /* sleep for a while...  zzzz... Use up some time...*/
	        (void) usleep(SLEEP_TIME); /* suspend execution for a defined time interval */
	        /* In principle a task could be performing real useful work here, mining Bitcoin, whatever... */

          /* perform a short busy-wait, in order to regulate the timing */
	        while (elapsed_time() < target_time) j++ ;
          /* j++ very minimal amount of "busy-work" ...*/

          /* collect the current time value */
	        t_val      = (long) elapsed_time();

          /* collect a virtual (random) sample */
          sample_val = (long) rand() ;

          /* print the time-stamp and the sample-value . */
          printf("%ld\t%ld\n", t_val, sample_val );
        }
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
