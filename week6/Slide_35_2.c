
/*
   Cyclic_Executive_example_Slide_25.c
   demonstration of the principle of the cyclic executive

   Using a switch & cases rather than cut and paste

   This code has been checked with lint ( splint )
   compiled with "picky compilation" ( -Wall -Wextra -Werror ) 
   and re-foratted to 'gnu' format using "astyle"
   
   last edit: Wed 30 Aug 2023 00:22:27 ACST

   for RTES Comp. Ex. 01 in Y2023

   by Andrew A.

   compilation advice:

   gcc -Wall -Wextra -Werror -o Cyclic_Executive_example_Slide_25_V02 Cyclic_Executive_example_Slide_25_V02.c

*/

#include <stdio.h>     /* needed for printf() */
#include <stdint.h>    /* defines some types, including: int64_t */
#include <time.h>      /* needed for clock_gettime() and  CLOCK_MONOTONIC*/
#include <unistd.h>    /* needed for usleep() */
#include <stdlib.h>    /* for exit()*/

/* Say, one millisecond per time tick, get everything done quickly... */
#define TIME_TICK 1000
/* 1000 us un a ms*/

/* The target times step in multiples of "f" the minor-frame size, in micro seconds*/
# define f (75*TIME_TICK)

/* Define some time intervals in microseconds  */
#define H (300*TIME_TICK)

/* The number of tabs to use across the screen */
#define TAB_STEP  7

/* The number of times to complete the main loop*/
#define n_loops 1

/* Define some parameters for the pseudo-tasks */
int C[] = {15, 20} ; /* Computation times */
int T[] = {75, 100} ; /* Periods */

static int count[]    = {0,0}; /* stores an internal state of each process, really just the execution count*/
static int n_steps[]  = {1,1}; /* stores the number of steps that each process will take*/



/* function definitions*/
static void tab_space(int k);
static void task_simulation (int proc_no);
static int64_t timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p);
static int64_t elapsed_time(void);

/* Set up structures for the starting and stopping times */
static struct timespec start, end;

/* If we want to regulate the times, then we need realistic targets to aim at */
static int64_t target_time ;

int N_frames = (int) (((int) H)/ ((int) f)); /* use integer division to estimate the number of complete cycles, or frames */

int main(void)
{
    int k;      /* a counter, for the number of loops */
    int j;      /* a counter, for the number of the minor-frame, within a super-loop */
    int l = 0 ; /* a dummy counter, just for the busy loop*/

    /* start the clock, and set "start", pointed to by "&start"*/
    clock_gettime(CLOCK_MONOTONIC, &start);
    /* begin at the start... */

    /* initialise the target time, if needed*/
    target_time = elapsed_time()+(f*1000); /* Time at the end of the next minor frame */
    /* Note that there are 1000 ns in a us*/

    /*
        This is a finite loop, which terminates after a finite number of cycles, n_loops.
        we could create an infinte loop using: while(1) { ... }
        instead of the for loop.
    */

    /* step through all of the major cycles */
    for ( k=1; k<=n_loops; k++ )
        for(j=0; j<N_frames; j++)
            {
                /* simulate the activities for this minor cycle, or "frame" */
                switch(j)
                    {
                    case 0:
                        task_simulation (0);
                        task_simulation (1);
                        break;
                    case 2:
                    case 3:
                        task_simulation(0);
                        //task_simulation(1);
                        break;

                    case 1:
                        task_simulation(0);
                        break;
                    
                    default:
                        /* no action is required */
                        break;
                    }
                /* possibly wait for the end of the current minor cycle, with a busy loop*/
                while (elapsed_time() < target_time ) l++ ;
                target_time = target_time +(f*1000); /* Time at the end of the next minor frame */
                /* Note that there are 1000 ns in a us*/

            }

    return EXIT_SUCCESS ;
}

/* tab across the screen, by an amount, of k tabs
   this visual spacing helps with the illusion that separate tasks are
   being executed. */
static void tab_space(int k)
{
    int j ;
    for (j=0; j<k; j++) printf("\t");
}


/* Simulate a separate pseudo-task or "application".
   Please note, that in reality, there is only one task running at a time.
   They are all instances of the one function, but they can be made individual, using a switch statement, for example*/
static void task_simulation (int proc_no)
{

  int SHORT_TIME_TICK = (int) (0.8* ((float) TIME_TICK)) ;
  /* Adjust for the tendency of uspeep() to over-sleep
     One convenient idea is to shave the time given to each task in irder
     to limit the effect of over-run */

    int k;

    for (k=0; k< n_steps[proc_no]; k++) /* we can repeat this a number of times, if required*/
        {

            /* update the count, for this proc_no */
            count[proc_no] =  count[proc_no] + 1 ;

            /* Display some results on the correct part of the screen */
            tab_space( TAB_STEP*proc_no);

            /* We can make the processes individual, and of arbitrary complexity, if required*/
            switch(proc_no)
                {
                case 0:
                    printf("T1:\t "); /* print a label */
                    break;

                case 1:
                    printf("T2:\t "); /* print a label */
                    break;

                default:
                    printf("Fault!\n");
                    /* It is not clear what to do here,
                       but we could include some code to trap errors, or "exceptions" */
                    exit(EXIT_FAILURE); /* do nothing and exit() */
                }

            /* can trace print information about the current pseudo-process */
            printf("%d\t", proc_no);        /* an I.D number for this task*/
            printf("%d\t", count[proc_no]); /* the number of the instance of this task */
            printf("%ld\t",(long) elapsed_time()); /* The starting time for this task*/

	    (void) usleep((useconds_t) (C[proc_no]*SHORT_TIME_TICK) );
	    /* sleep for the required number of time ticks, to simulate work*/
	    /* This virtual work cannot be interrupted. It will run to completion.*/

            /*
               Some of the simulated "real work" is just sleeping
               like certain members of parliament...

               Other useful things could be done at this point,
               if the usleep() were trimmed, appropriately.
               We could mine Bit-Coin, for example....
            */

            printf("%ld\n",(long) elapsed_time()); /* The completion time for this task*/

            /* The tasks generally do not contain the "busy-loops"*/

        }

}

/*
   A user function to extract the number of nanoseconds that have elapsed,
   between two times, *timeA_p and *timeB_p.
*/
static int64_t timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
    return (int64_t) ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
           ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec);
}

/*
   A user function to extract the number of nanoseconds that have elapsed,
   since the starting time was reset.
   This is a convenient "wrapper" for evaluating elapsed time, in nanoseconds.
*/
static int64_t elapsed_time(void)
{
    /* evaluate the elapsed time, in nanoseconds, a type, similar to a long integer, called: int64_t */
    clock_gettime(CLOCK_MONOTONIC, &end);
    return (int64_t) timespecDiff(&end, &start);
}
