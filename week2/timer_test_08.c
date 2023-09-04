/* timer_test_08.c */
/* create a simple single main() task that
   evaluates a large number of elapsed times
   and computes some statistics. */

/* created by Andrew A. 
   Sun 30 Jul 2023 21:26:24 ACST (Adelaide) */

/*

Compilation advice:

  gcc -std=gnu99 -Wall -Wextra -Werror -o timer_test_08 timer_test_08.c -lm

Please note the requirement for -lm

*/

#include <stdio.h>     /* needed for printf() */
#include <stdint.h>    /* defines some types, including: int64_t */
#include <time.h>      /* needed for clock_gettime() and  CLOCK_MONOTONIC*/
#include <unistd.h>    /* needed for usleep() */
#include <math.h>      /* needed for sqrt() */

#define SLEEP_TIME 100000
/* in microseconds

   100000 microseconds corresponds to a sleep of 0.1 second, 
   which is within the range of easy human perception.
*/

#define N_STAMPS 600
/* should be able to complete this run in about 60 seconds, or one minute.*/

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
 
    int k;   /* a counter */

    int64_t x ; /* the over-run*/
   
    /* The code that we are interested in measuring, starts here... */
    for (k=0; k<N_STAMPS; k++)
        {

          /* start the clock, and set "start", pointed to by "&start"*/
          clock_gettime(CLOCK_MONOTONIC, &start);
	  
          /* sleep for a while...  zzzz... Use up some time...*/
	       (void) usleep(SLEEP_TIME); /* suspend execution for a defined time interval */

	       /* Calculate the over-run for this machine*/
	       x = ( elapsed_time() - SLEEP_TIME*1000 );

	       /* log the results to stdio */
          printf("%ld\n", (long) x);
  
        }

    /* note that 1000 instances of 0.1 second takes 100 seconds to execute.*/
    
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
