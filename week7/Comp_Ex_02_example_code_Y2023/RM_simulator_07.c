/* RM_simulator_07.c */

/* by Andrew A.
   Last edit: Mon Oct 10 12:52:43 ACDT 2022

   compilation advice:
   gcc -Werror -Wall -Wextra -o RM_simulator_07 RM_simulator_07.c

   an execution suggestion:
   ./RM_simulator_07 RM_example_data_s44_t3.txt > RM_example_data_s44_t3_out.txt

   The output file, RM_example_data_s44_t3_out.txt , is a choice that you can make.
   It is helpful to store the data to a file for later plotting, or analysis.
   See Some_useful_graphing_techniques.zip on the My-Uni page

   There also is a hard-wired output file: simulator_tasks_out_data.txt,
   which logs the completed tasks, after they have completed,
   in order of arrival time.
   */

/*
This took about 21 seconds of real time to run on my "barbie" machine 
(which is a LINUX sub-system wunning under Windows 11):

barbie@DESKTOP-DL95D7D:~/RTES_S2_Y2023/Comp_Ex_03_solution$ time ./RM_simulator_07 RM_example_data_s44_t3.txt > RM_example_data_s44_t3_out.txt

real    0m21.005s
user    0m11.582s
sys     0m9.421s
barbie@DESKTOP-DL95D7D:~/RTES_S2_Y2023/Comp_Ex_03_solution$
*/


/* include files */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>    /* used to unpack struct timespec */
#include <inttypes.h>  /* defines the types int64_t, for the time functions */
#include <time.h>      /* needed for clock_gettime(), and CLOCK_MONOTONIC */
#include <sys/types.h> /* needed for getpid() */
#include <unistd.h>    /* needed for usleep() &c.. &c.. */
#include <sys/wait.h>  /* needed for usleep() */
#include <fcntl.h>      /* file control optopns, for pipes*/

/* constant identifiers */

#define TIME_TICK                            10000 /* in usec*/
#define MAX_TIME                          60000000 /* Don't want the simulation to run longer than, say..., a minute, 60000 usec without time-out*/

/* tesk_description_structure, here, similar role to a Task Control Block (TCB) in a real RTOS*/

struct task_description
{
    long task_type;                        /* The type of task */
    long absolute_arrival_time;            /* The absolute arrival time of the task, since the start of the program, in usec*/
    long recurrence_time ;                 /* The period with which this type of task recurs, can be used to set priorities, in usec */
    long remaining_computing_time ;        /* The remaining time, to be processed, initially like the c_k values in lectures, in usec*/
    long waiting_time;                     /* The total time, between arrival and dispatch,
                                             that the job waited until it has finally completed, in usec*/
    struct task_description* next_tds_ptr; /* A pointer to the the next tds that may be inserted in a list, after this structure */
} ;


struct task_description tds ; /* tds is a Task Description Structure */

/* function templates */
void error_exit(char *);
long elapsed_time_us();
void print_tds( struct task_description);
long gcd(long, long);

/* function headers for utility functions, for handling queues, as linked-lists */
struct task_description* copy_task_description_structure( struct task_description );
int insert_task_by_arrival(struct task_description *(*first_out_ptr), struct task_description *new_task_ptr );
int insert_task_by_rate(struct task_description *(*first_out_ptr), struct task_description *new_task_ptr );
struct task_description* pop_top_task( struct task_description *(*first_out_ptr) );
void traverse_list(struct task_description *(*first_out_ptr));


/* global variables */

struct timespec start, end; /* for starting the clock, and taking splits */

#define MIN_ARGV   2  /* There is one command-line parameter, so we require argv >= 2*/
#define MAX_TASKS 12  /* the maximum number of tasks that we plan to consider */

int main (int argc, char *argv[] )
{

    /* declations in main() */
    int   i = 0;                 /* a loop counter */
    int   pd[2];                 /* pipe descriptors; two ends of a single pipe */
    long  absolute_arrival_time; /* To save on heartache, and unit conversions, all times are in usec*/

    int   status;                /* a variable for a return status value, of waitpid() */
    pid_t pid1;                  /* process id, return value from fork() */

    /* Define some variables (with informative names) for the input data*/
    long        task_type_in[MAX_TASKS] ; /* labelks the type of task*/
    long   computing_time_in[MAX_TASKS] ; /* the computing time that the task requires */
    long  recurrence_time_in[MAX_TASKS] ; /* the recurrence time for this type of task*/

    /* used for calculating the LCM and the stopping time */
    long H  = 0 ; /* H will become the LCM of the periods*/
    long G  = 0 ; /* G will become the GCD of the periods*/
    long T_STOP ; /* the actual stopping time */
    long expected_completion_time = 0 ;



    /* The total number of task types */
    int N_tasks = 0 ; /* We will count in the number of tasks*/

    /* fscanf() needs a return value, which we need to control the loop*/
    int scanval = 0 ;

    /*  In this programming paradigm, a "list" is just a pointer to a task drescription structure*/
    /*  The scheduler is alerted to tasks by data from the pipe
        These tasks are then sorted and inserted into queues,
        a  ready queue, a running queue and a completed queue.
        At the appropriate time, they are moved to the running queue
        to the running queue,
        and finally to the completed list.),

        */

    /* we need to store the heads of the queues */
    struct task_description *ready_first_out_ptr     = NULL ; /* points to the head of the ready queue*/
    struct task_description *running_ptr             = NULL ; /* point to the task which is currently running*/
    struct task_description *completed_first_out_ptr = NULL ; /* points to a list of completed tasks */

    /* The task at the head of the ready queue is deemed to be "running".
       In this simulation, we just keep statistics, we do not lainch actual tasks. */

    /* When a task is completed, it is popped from the queue, after all statistics have been collected. */
    struct task_description *popped_task_description_ptr; /* we will need pointer to a popped job */
    struct task_description *new_tds_ptr   ; /* Need a pointer to the new task */
    struct task_description *temp_tds_ptr  ; /* Need a temporary pointer, for moving tasks around */

    /* start the clock */
    clock_gettime(CLOCK_MONOTONIC, &start);

    /* trace print the startup of main() tasks */
    /* printf("trace: PID number = %ld\tstarting_time=%ld\n",(long) getpid(), (long) elapsed_time_us() ); */


    /* check for the correct number of arguments */
    if (argc < MIN_ARGV)
        {
            fprintf(stderr,"Usage is: ./RM_file_reader input_file \n");
            return  EXIT_FAILURE;
        }

    /* attempt to open the input file for reading, and define a file-handle: in_fp */
    FILE* in_fp = fopen(argv[1], "r");

    /* check for an error while opening */
    if(!in_fp)
        {
            perror("File opening failed");
            return EXIT_FAILURE;
        }

    /* attempt to read formatted items from the input file */
    i = 0 ; /* reset ythe loop counter*/
    scanval = fscanf(in_fp,"%ld\t%ld\t%ld\n", &task_type_in[i], &computing_time_in[i], &recurrence_time_in[i]);

    /* Read strings until an empty line, or EOF is encountered*/
    while ((scanval>0) & (i < MAX_TASKS))
        {

            i++ ;

            /* read formatted items from the input file */
            scanval = fscanf(in_fp,"%ld\t%ld\t%ld\n", &task_type_in[i], &computing_time_in[i], &recurrence_time_in[i]);

        }

    N_tasks = i ;


    /* We want to calculate the LCM of the periods so that we can simulate an entire cycle
      if that does not go on for too long. Otherwise, we will just time-out at T_STOP = MAX_TIME */
    H = recurrence_time_in[0];
    for (i=0; i<N_tasks ; i++)
        {
            /* printf("trace: %ld\t%ld\t%ld\n",task_type_in[i], computing_time_in[i], recurrence_time_in[i]); */
            G = gcd(H, recurrence_time_in[i]);
            H  =  (H * recurrence_time_in[i])/G ; /* estimate H successively*/
        }

    T_STOP = H * TIME_TICK ;
    if(T_STOP > MAX_TIME) T_STOP = MAX_TIME ; /* prevent the simulation from going on for too long */

    /* printf("trace:: T_STOP = %ld\n", T_STOP); */

    /* We have read in the specifications for the task set */

    /* A technical note, if all that we want to do is to test schedulability, we only need to calculate the timer-line up to
       the point where we include the completion of the first instance of the first task of each type.*/

    /* Further technical note, if *ALL* that we wanted to do was to check scedulability, then we could use the completion-time
       algorithm. without the need to actually contruct the time-line. */

    if(pipe(pd) == -1) /* instantiate pd as a pipe */
        error_exit("pipe() failed");


    /* trace print the startup of main() tasks */
    /* printf("trace: (pre-fork) PID number = %ld\tstarting_time=%ld\n", (long) getpid(), (long) elapsed_time_us() ); */


    /* fork() the required number of child processes */
    for (i=0; i< N_tasks; ++i)
        if(fork()==0)
            {
                /* child of type i*/
                /*  When I was a child, I spake as a child... */

                /* We could trace the start up of the children */
                /* printf("trace: starting child\tPID number = %ld\tstarting_time=%ld\n",(long) getpid(), (long) elapsed_time_us() ); */

                /* pack the data for this type of task, into a task description structure, tds*/
                tds.task_type                = task_type_in[i];
                tds.absolute_arrival_time    = elapsed_time_us();
                tds.recurrence_time          = recurrence_time_in[i] * TIME_TICK;
                tds.remaining_computing_time = computing_time_in[i]  * TIME_TICK;
                tds.waiting_time             = 0 ;
                tds.next_tds_ptr             = NULL ;

                /* It is probably best to store all times in the same unit of us, rather than TIME_TICKs */

                /* We will write the tds, representing demands for work,
                   onto a pipe, at regular time intervals */
                while(tds.absolute_arrival_time <= T_STOP )
                    {

                        /* write the tds onto the pipe, to represent the arrival of a task */
                        if(write(pd[1],&tds,sizeof(tds)) == -1)
                            error_exit("write() failed");
                        /* wait for the required number of time ticks */
                        usleep( (useconds_t) tds.recurrence_time );
                        /* we know that usleep() is not exact, but it is good enough, for now...*/

                        /* practical experiments, on a laptop,
                           show that the standard deviation of the measured delay is
                           about 20 usec, which is fairly small compared with the TIME_TICK, of 1000 usec.
                           the sleep tends to over-shoot by about 0.07%,
                           which would be adequate for many applications. */

                        tds.absolute_arrival_time    = elapsed_time_us();
                    }
                /* printf("trace: exiting child\tPID number = %ld\tstarting_time=%ld\n", (long) getpid(), (long) elapsed_time_us() ); */
                exit(0) ; /* exit from this child */
            }

    /* parent process (parent only, all "child" processes have exited by this point) */
    /*  I put away childish things... */



    /* set the flag for non-blocking of pd[] */
    (void) fcntl( pd[0], F_SETFL, fcntl(pd[0], F_GETFL) | O_NONBLOCK);
    /* It is a commpn problem for the parent to get blocked, attempting to read an empty pipe */

    int nread; /* number of items obtained from a pipe */

    absolute_arrival_time = elapsed_time_us();

    expected_completion_time = 0; /* no tasks detected yet...*/

    /* read things from the pipe until the time expires*/
    while(absolute_arrival_time<=T_STOP)
        {

            /* SCHEDULING STARTS HERE */

            /* New tasks can enter the system, as long as time has not expired*/


            /* Scheduling part1: attempt to read from the pipe, and acquire new tasks */
            nread = (int) read(pd[0], &tds, sizeof(tds));
            if ( nread > 0 )
                {
                    /* At this point we are in posession of a task description structure, tds.
                               This can be inserted into the ready queue, in order of arrival, for example. */

                    /* We will reckon time, from the scheduler's point of view. */
                    tds.absolute_arrival_time = elapsed_time_us();

                    /* create a new task-drescription structure and retain a link to this data */
                    new_tds_ptr  = copy_task_description_structure( tds ); /* uses malloc() */

                    /* The newly arrived task could be the task with the highest priority....
                            We need to check!! */

                    /* is any task running?*/
                    if(running_ptr!=NULL)
                        {

                            /* A task is running, and may need to be preempted... */
                            if((new_tds_ptr->recurrence_time) < (running_ptr->recurrence_time)  )
                                {
                                    /* The new task does have strictly higher prority.
                                                   It is ready to run and must run */

                                    /* Save the state of the existing task and pit it back on the ready queue*/

                                    /* log the task that is currently running*/
                                    printf("%f\t%ld\n", ((float) elapsed_time_us())/((float) TIME_TICK), (running_ptr->task_type) );

                                    /* pop the task out of the running queue */
                                    temp_tds_ptr = pop_top_task(  &running_ptr );
                                    /* Update the amount of remaining time */
                                    temp_tds_ptr->remaining_computing_time = expected_completion_time - elapsed_time_us() ;

                                    /* It can happen that the arrival of th enew task has just preemped the exiry of th eolf task
                                                   It would be neat to dasl with that.*/
                                    if((temp_tds_ptr->remaining_computing_time) <= 0 )
                                        {
                                            /* The old task had expired, just as the new task arrived*/

                                            /* Calculate the waiting time for this task */
                                            ( temp_tds_ptr->waiting_time ) = elapsed_time_us() - ( temp_tds_ptr->absolute_arrival_time );
                                            ( temp_tds_ptr->remaining_computing_time ) = 0 ;

                                            /* insert this drescription into the completed queue, for possible later reference */
                                            (void) insert_task_by_arrival( &completed_first_out_ptr, temp_tds_ptr);

                                        }
                                    else
                                        {
                                            /* The old pre-empted task still has some way to go...*/
                                            /* Insert the pre-empted item into the ready queue in priority order */
					  (void) insert_task_by_rate( &ready_first_out_ptr, temp_tds_ptr);
                                        }

                                    /* Momentarily log the return to a task of type zero...*/
                                    printf("%f\t0\n", ((float) elapsed_time_us())/((float) TIME_TICK) );

                                    /* Insert the newly arrived task in the running queue*/
                                    (void) insert_task_by_rate( &running_ptr, new_tds_ptr );
                                    /* estimate time to completion for this new task*/
                                    expected_completion_time = elapsed_time_us()+ (running_ptr->remaining_computing_time);
                                    /* The new task is deemed to be running */

                                    /* Log this event to stdout*/
                                    /* Momentarily log the return to a task of type zero, for consistency*/
                                    printf("%f\t0\n", ((float) elapsed_time_us())/((float) TIME_TICK) );
                                    /* log the fact that a new task has started, to stdout  */
                                    printf("%f\t%ld\n", ((float) elapsed_time_us())/((float) TIME_TICK), (running_ptr->task_type) );
                                }
                            else
                                {

                                    /* The newly arrived task is not of the highest priority */

                                    /* We only need to insert the new task description into the ready queue,
                                                  in order of priority, ie: highest rate first
                                                  or longest recurrence_time last*/
				  (void) insert_task_by_rate( &ready_first_out_ptr, new_tds_ptr);

                                    /* There is no change to the task that is running*/
                                    /* Keep calm, and carry on... */
                                }

                        }
                    else
                        {

                            /* The running queue is empty and a new task has arrived */

                            /* Momentarily return to a task of type zero...*/
                            printf("%f\t0\n", ((float) elapsed_time_us())/((float) TIME_TICK ) );

                            /* insert the new task description into the short running queue */
                            (void)  insert_task_by_rate( &running_ptr, new_tds_ptr);

                            /* log the fact that a new task has started, to stdout  */
                            printf("%f\t%ld\n", ((float) elapsed_time_us())/((float) TIME_TICK), (running_ptr->task_type) );

                            /* update the estimated time to completion for this new task*/
                            expected_completion_time = elapsed_time_us()+ (running_ptr->remaining_computing_time);

                        }
                }


            /* Scheduling part 2: Manage the transition from a ready task to a running task */
            if( (ready_first_out_ptr != NULL) & (running_ptr == NULL))
                {
                    /* There is a new task description at the head of the ready queue
                       and no actual "running" task, to stop it from possibly running .*/

                    /* Note that there is no task actually running, now...*/
                    printf("%f\t0\n", ((float) elapsed_time_us())/((float) TIME_TICK) );

                    /* Pop the next task from the ready queue */
                    popped_task_description_ptr =  pop_top_task( &ready_first_out_ptr );

                    /* insert the next task description into the short running queue */
                    (void)  insert_task_by_rate( &running_ptr, popped_task_description_ptr);

                    /* log the fact that a new task has started, to stdout  */
                    printf("%f\t%ld\n", ((float) elapsed_time_us())/((float) TIME_TICK), (running_ptr->task_type) );

                    /* estimate time to completion for this new task*/
                    expected_completion_time = elapsed_time_us()+ (running_ptr->remaining_computing_time);

                }

            /* Scheduling Part 3: Manage transition from a running task to a completed task*/
            if( (running_ptr != NULL) & (elapsed_time_us() >= expected_completion_time ))
                {
                    /* There is a task that is deemed to be running, and it is now
                               deemed to have finished, at (or shortly after...) the expected time*/

                    /* log the fact that the task was still running, to stdout */
                    printf("%f\t%ld\n", ((float) elapsed_time_us())/((float) TIME_TICK), (running_ptr->task_type) );

                    /* log the fact that the task has now stopped, to stdout */
                    /* A task of type "0" is deemed to be no task running at all... */
                    printf("%f\t0\n", ((float) elapsed_time_us()) / ((float) TIME_TICK) );

                    /* There is no task running and no "expected" completion time*/
                    expected_completion_time = 0;

                    /* While we are here, we may as well calculate the waiting time for this task,
                       ie: What is the delay between the arrival and the completion of this task? */
                    ( running_ptr->waiting_time ) = elapsed_time_us() - ( running_ptr->absolute_arrival_time );

                    /* also there is nothing left to run*/
                    ( running_ptr->remaining_computing_time ) = 0 ;

                    /* Pop the finished task from the running queue */
                    popped_task_description_ptr =  pop_top_task( &running_ptr );
                    /* insert this drescription into the completed queue, for possible later reference */
                    (void)  insert_task_by_arrival( &completed_first_out_ptr, popped_task_description_ptr);

                }

            /* SCHEDULING ENDS HERE */

            /* Note: We should not prevent the scheduler from checking for new tasks coming in on the pipe. */

            /* get the time, and do it again, wheels turning round and round... */
            absolute_arrival_time = elapsed_time_us();
        }


    /* Time is up, and everything is shutting down soon... */

    /* wait for children to quit */
    for (i=0; i < N_tasks; i++ )
        {
            /* wait for a child to finish */
            if ((pid1 = wait(&status)) < 0)
                perror ("waitpid error");
        }

    /* Print the final list of completed tasks to a text file */
    traverse_list( &completed_first_out_ptr );
    /* We could print all outputs to data files, if we wanted.... just saying...  */


    exit(0);
} /* end of main() */


/*-----------------------------------------------------------------------------*/

/* A standard formatter for printing eror messages, and exiting */
void error_exit(char *s)
{
    fprintf(stderr, "\nerror: %s - bye!\n",s);
    exit(1);
}

/*-----------------------------------------------------------------------------*/

/* determine the elapsed time, since the program was started, in usec. */
long elapsed_time_us()
{
    clock_gettime (CLOCK_MONOTONIC, &end); /* obtain the current time-data and store it in "end"*/
    /* calculate delay in ns */
    long time_ns = (long) (( end.tv_sec  * 1000000000) + end.tv_nsec) - ((start.tv_sec * 1000000000) + start.tv_nsec);
    /* return delay truncated to microseconds */
    return time_ns/1000 ;
}

/*-----------------------------------------------------------------------------*/
/* print a Task Description Structure, tds, to a file, on a single line, in append mode */
void print_tds( struct task_description tds2 )
{
    /* The output file is hardwired here.
       A "DELUXE" feature might be to include more parameters in the ARGV[] parameter list...  */

    /* The output format is:
       task_type \t absolute_arrival_time \t recurrence_time \t  remaining_computing_time \t waiting_time \t (void*) tds2.waiting_time \n */

    FILE* out_fp = fopen("simulator_tasks_out_data.txt", "a");

    /* check for an error while opening */
    if(!out_fp)
        {
            perror("File opening failed");
            return;
        }

    fprintf(out_fp, "%ld\t", tds2.task_type );
    fprintf(out_fp, "%ld\t", tds2.absolute_arrival_time );
    fprintf(out_fp, "%ld\t", tds2.recurrence_time );
    fprintf(out_fp, "%ld\t", tds2.remaining_computing_time );
    fprintf(out_fp, "%ld\t", tds2.waiting_time );
    fprintf(out_fp, "%p\n", (void*) tds2.waiting_time );

    /* close the putput file */
    fclose(out_fp);

}

/*-----------------------------------------------------------------------------*/
/* Andrew's quick & dirty GCD calculator,
   which might be handy because math.h does not have gcd() */
long gcd(long a, long b )
{

    long c;
    /* deal with boundary cases */
    if(a<0) a = -a;
    if(b<0) b= -b;
    if(a<b)
        {
            /* swap a and b */
            c=a;
            a=b;
            b=c;
        }
    if (b==0) return 0 ;

    /*implement Euclid's algorithm*/
    c = a % b;
    while(c != 0 )
        {
            a = b;
            b = c;
            c = a % b ;
        }
    /* at this point: c==0 ; and a%b == 0; and b is the gcd */
    return b;
}
/*-----------------------------------------------------------------------------*/

/* create a structure to represent a new task_description and return a pointer to that structure */
struct task_description* copy_task_description_structure( struct task_description tds1  )
{

    /*allocate some permanent storage for the new data */
    struct task_description *new_task_ptr = (struct task_description*) malloc(sizeof(struct task_description));

    /* allocate values to the fields, from the argument values to this function */
    new_task_ptr->task_type                =  tds1.task_type ;
    new_task_ptr->absolute_arrival_time    =  tds1.absolute_arrival_time ;
    new_task_ptr->recurrence_time          =  tds1.recurrence_time ;
    new_task_ptr->remaining_computing_time =  tds1.remaining_computing_time ;
    new_task_ptr->waiting_time             =  tds1.waiting_time ;
    /* This task drescription has no successor, yet.*/
    new_task_ptr->next_tds_ptr             =  (struct task_description *) NULL;

    return new_task_ptr;
}
/* end of create_task_description */
/*-----------------------------------------------------------------------------*/


int insert_task_by_arrival(struct task_description *(*first_out_ptr), struct task_description *new_task_ptr )
{
    /* insert a new task into a queue, non-recursively, in order of absolute_arrival_time */

    /* note:  first_out_ptr is a pointer to a pointer which points to the structure containing the data */
    /*       *first_out_ptr is a pointer which points to the structure containing the data */
    /*       *(*first_out_ptr) is the the structure containing the data, of type:  struct task_description */
    /* This unusual arrangement is needed in order to make changes to the list permanent. */
    struct task_description *task_ptr;
    task_ptr = *first_out_ptr ; /* use a temporary variable as a pointer into a list*/

    struct task_description *last_task_ptr;
    last_task_ptr = NULL ; /* initially there is no "last" task */


    /* this algorithm inserts the new task into the list, without using recursion */
    if(task_ptr==NULL)
        {
            /* insert an item into an empty list*/
            *first_out_ptr = new_task_ptr ;
            return 0;
        }
    else
        {
            /* The list is not empty*/
            if((new_task_ptr->absolute_arrival_time) < (task_ptr -> absolute_arrival_time)  )
                {
                    /* Insert the new task at the head of a non-empty list*/
                    new_task_ptr->next_tds_ptr = *first_out_ptr ;
                    *first_out_ptr               = new_task_ptr ;
                    return 4 ;
                }
            else
                {
                    /*Insert the new task further into a non-empty list */
                    while((task_ptr->next_tds_ptr) != NULL )
                        {
                            /* step through the list*/
                            last_task_ptr = task_ptr ;
                            task_ptr      = task_ptr->next_tds_ptr;
                            if((task_ptr-> absolute_arrival_time)>(new_task_ptr->absolute_arrival_time))
                                {
                                    /* insert the new task in the middle of a non_empty list*/
                                    new_task_ptr->next_tds_ptr  = task_ptr ;
                                    last_task_ptr->next_tds_ptr = new_task_ptr ;
                                    return 6;
                                }

                        } /* end of while*/
                    /* Insert the new task at the end of a non_empty list*/
                    task_ptr->next_tds_ptr = new_task_ptr ;
                    return 7;

                }
        }
}
/* end of insert_task_by_arrival */

/*-----------------------------------------------------------------------------*/

struct task_description* pop_top_task( struct task_description *(*first_out_ptr) )
{

    /* excise the top item from the top of the linked-list, task_description
       and return a link to that item, as the return value
       The input link to the list is doubly indirected, *(*first_out_ptr)
       The pointer in the calling routine, *first_out_ptr will be singly indirected,
       and will point to the new shorter linked-list, even if that is NULL. */


    /* note:  first_out_ptr is a pointer to a pointer which points to the structure containing the data */
    /*       *first_out_ptr is a pointer which points to the structure containing the data */
    /*       *(*first_out_ptr) is the the structure containing the data, of type  struct task_description */

    /* use a default return value of NULL, if the list is empty */
    struct task_description *popped_task_ptr;
    popped_task_ptr = NULL ;

    if(*first_out_ptr != NULL)
        {
            /* There is at least one task to pop*/
            /* The pointer, *first_out_ptr, now points to the next link in the list*/

            popped_task_ptr  = *first_out_ptr ; /* Use the return value to store the value of the previous head of the queue*/
            *first_out_ptr   = (*first_out_ptr)->next_tds_ptr ; /* move the head of the queue on to the next link in the list  */

            /* The popped task no longer has a successor*/
            popped_task_ptr->next_tds_ptr = NULL ;
        }

    return popped_task_ptr ;

    /* Note that there is no call to free() here... */

}

/*-----------------------------------------------------------------------------*/

/* traverse a linked list, starting at the head */
void traverse_list(struct task_description *(*first_out_ptr))
{
    /* Kernighan and Ritchie recommend the for loop as the standard idiom for walking along a linked list */
    struct task_description *ptr ;
    /* print out the contents of the completed queue*/
    for (ptr = *first_out_ptr; ptr !=  NULL ; ptr = ptr->next_tds_ptr)
        {
            /* ptr now points to a location in the list */
            print_tds( *ptr  );

            /* In this case, the traversal only trace prints the item*/
            /* It would be possible to use function pointers to use a general function,
               to perform a general operation on the item, pointed to by ptr.   */
        }
}
/* end of traverse_list*/
/*-----------------------------------------------------------------------------*/

/* Scheduling Part 4: Sort the queue in priority order */

/* In this case priority is in reverse order of the recurrence_time
   say: priority = VERY_LARGE_CONSTANT - (task_ptr->recurrence_time) ... */


int insert_task_by_rate(struct task_description *(*first_out_ptr), struct task_description *new_task_ptr )
{
    /* insert a new task into a queue, non-recursively, in order of absolute_arrival_time */

    /* note:  first_out_ptr is a pointer to a pointer which points to the structure containing the data */
    /*       *first_out_ptr is a pointer which points to the structure containing the data */
    /*       *(*first_out_ptr) is the the structure containing the data, of type:  struct task_description */
    /* This unusual arrangement is needed in order to make changes to the list permanent. */
    struct task_description *task_ptr;
    task_ptr = *first_out_ptr ; /* use a temporary variable as a pointer into a list*/

    struct task_description *last_task_ptr;
    last_task_ptr = NULL ; /* initially there is no "last" task */

    /* this algorithm inserts the new task into the list, without using recursion */
    if(task_ptr==NULL)
        {
            /* insert an item into an empty list*/
            *first_out_ptr = new_task_ptr ;
            return 0;
        }
    else
        {
            /* The list is not empty*/
            if((new_task_ptr->recurrence_time) < (task_ptr->recurrence_time)  )
                /* Note the absence of "=" here, tasks of equal period get inserted later,
                       giving the older tasks a chance to complete.*/
                {
                    /* Insert the new task at the head of a non-empty list*/
                    new_task_ptr->next_tds_ptr = *first_out_ptr ;
                    *first_out_ptr               = new_task_ptr ;
                    return 4 ;
                }
            else
                {
                    /*Insert the new task further into a non-empty list */
                    while((task_ptr->next_tds_ptr) != NULL )
                        {
                            /* step through the list*/
                            last_task_ptr = task_ptr ;
                            task_ptr      = task_ptr->next_tds_ptr;

                            if((task_ptr-> absolute_arrival_time)>(new_task_ptr->absolute_arrival_time))
                                if( (task_ptr-> recurrence_time) > (new_task_ptr->recurrence_time) )
                                    /* Note also the absence of "=" here... */
                                    {
                                        /* insert the new task in the middle of a non_empty list*/
                                        new_task_ptr->next_tds_ptr  = task_ptr ;
                                        last_task_ptr->next_tds_ptr = new_task_ptr ;
                                        return 6;
                                    }

                        } /* end of while*/
                    /* Insert the new task at the end of a non_empty list*/
                    task_ptr->next_tds_ptr = new_task_ptr ;
                    return 7;
                }
        }
}
/* end of insert_task_by_rate() */
/* very little change from insert_task_by_arrival() */
/*-----------------------------------------------------------------------------*/
