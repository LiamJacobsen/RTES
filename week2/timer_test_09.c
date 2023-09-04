/* timer_test_09.c */
/* create a simple single main() task that prints out elapsed times
   with busy-wait to improve regularity */

/* created by Andrew A.
   Sun 07 Aug 2022 22:00:21 ACST */

/*

Compilation advice:

  gcc -std=gnu99 -Wall -Wextra -Werror -o timer_test_09 timer_test_09.c

Times are in nanoseconds.
*/

#include <stdio.h>     /* needed for printf() */
#include <stdint.h>    /* defines some types, including: int64_t */
#include <time.h>      /* needed for clock_gettime() and  CLOCK_MONOTONIC*/
#include <unistd.h>    /* needed for usleep() */

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
  int j = 0 ; /* a counter*/   

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
	  
            /* Evaluate the elapsed time, using elapsed_time(), and then print it. */
            printf("timeElapsed: %ld\n", (long) elapsed_time() );
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


/*

An example of the output under MacOS, using a humble Macbook Air.

> ./timer_test_09
timeElapsed: 100000000
timeElapsed: 200000000
timeElapsed: 300000000
timeElapsed: 400000000
timeElapsed: 500000000
timeElapsed: 600000000
timeElapsed: 700000000
timeElapsed: 800000000
timeElapsed: 900000000
timeElapsed: 1000000000
> 

*/

/*

Notice how the elapsed times are not exactly increments of 100000000
nanoseconds, or 100000 microseconds.

The use of a "busy" wait at the end improved the regulatity of the process, 
even without an RTOS.

Some points to note:

1/ The irregularity is reduced, but not eliminated.  Can it ever be
   completely eliminated using a GPOS ?

2/ Note that some time is wasted eg: 413 microseconds out of 100000
microseconds

3/ What happens if we reduce that margin down towards zero?  Do we
   lose the regularity that is provided by the busy-wait?

4/ The process usleep() uses a call to the operating system to restart
   the process.  If the program was busy, performing actual useful
   work, then how would it know when to stop and store the state?

5/ Would it be useful to have hardware support, such as a hardware
   interrupt to launch the processes on time?

*/



