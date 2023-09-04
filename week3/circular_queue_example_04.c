/* circular_queue_example_04.c */

/* by Andrew A.*/
/* Last edit: Tue 18 Jul 2023 22:29:14 (Singapore) */

/* A simple demonstration of a circular queue */

/*
   This version allows a buffer, based on a
   circular-queue, or "queue-array", to contain more than one item of data
   in each list-item.

   This is achieved by defining the queue_item as a struct{},
   rather than a single number.

   Compilation advice:

   gcc -Wall -Wextra -Werror -o circular_queue_example_04 circular_queue_example_04.c 
   
 
*/


/* We can define the maximum length of the queue*/
#define MAX_LENGTH 128
/* You may want to increase MAX_LENGTH, quite a lot... */


/* We are free to define the items that will be in a queue_item */
typedef struct
{
    int thing1;
    int thing2;
} queue_item;
/* Please note that your "things" might be of different types and
   the various parts of the structure should have more descriptive names. */

/* An easy solution, to the access problem, is to make the queue global */
/* This will be a First-In First-Out (FIFO) queue*/

queue_item queue_store[MAX_LENGTH]; /* Defines the memory-space to be used by the queue */
int next_sub = 0 ; /* The subscript at which we should store the next item*/
int n_items  = 0 ; /* There are initially no items in the queue*/
/* In general, a circular queue needs two pointer type items.*/


#include <stdio.h>
#include <stdlib.h>

/* function prototypes */
int  add_list_item(queue_item);
void print_list_items(void);
int  dispatch_list_item(queue_item *);
int  get_queue_item(queue_item* );

/* build up a circular queue, using entries from stdin() */
int main()
{

    queue_item input_value ; /* reserved for input values */
    int        error_code = 0;   /* used for return-values, error codes */
    queue_item output_value; /* reserved for output values */
    int        max_n_items = 0;  /* used to invert the sequence of values for trace-printing */
    int        ret_val = 0;      /* Used as a return value from get_queue_item() */

    /* initialise the temporary storage values */
    input_value.thing1  = 0;
    input_value.thing2  = 0;
    output_value.thing1 = 0;
    output_value.thing2 = 0;

    ret_val = get_queue_item( &input_value); /* Get the data for the next queue item */

    /* Insert some items */
    while((ret_val > 0) & (n_items<MAX_LENGTH))
      /* Bail out when the queue is completely full, or the queue-items are both zero */
    {
        error_code = add_list_item(input_value);
        if(error_code != 0) break ; /* We can perform limited trapping of exceptions by checking return values */
	
        print_list_items(); /* trace print the list */

        ret_val = get_queue_item( &input_value); /* Get the data for the next queue item */
    }
    printf("\n\n");


    /* Dispatch the items in the queue */
    printf("Dispatch of the items in the queue:\n");
    max_n_items = n_items ;
    printf("{ \n");

    while(n_items>0)
    {
        printf("   item number %d is ",(max_n_items-n_items+1));
        error_code = dispatch_list_item(&output_value);
	if(error_code != 0) break ; /* We can perform limited trapping of exceptions by checking return values */
        printf(" (%d,%d) , \n", output_value.thing1, output_value.thing2 );
    }
    printf(" \n}\n\n");


    /* and we are done */
    return 0;
}
/* end of main() */
/* //////////////////////////////////////////////////////////// */


/* insert an item into the queue */
/* This could be a First-In First-Out (FIFO) queue*/
int  add_list_item(queue_item input_value)
{

    if (n_items < MAX_LENGTH)
    {
        queue_store[next_sub].thing1 = input_value.thing1;
        queue_store[next_sub].thing2 = input_value.thing2;

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
        printf(" (%d,%d) ", (queue_store[ sub ]).thing1, (queue_store[ sub ]).thing2 );
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
        (*output_value).thing1 = queue_store[sub].thing1;
        (*output_value).thing2 = queue_store[sub].thing2;

        n_items-- ; /* decrement the next_sub subscript*/

        return 0 ; /* normal completion */
    }
    else
        return 1 ; /* return an error code, the queue is full*/
}


/* Get the data for a single queue item, from stdio, and assemble it into a queue item */
int get_queue_item(queue_item* input_value)
{

    int error_code = 0;

    int thingy1, thingy2;

    printf("\n\nPlease enter a positive integer, for thing1: ");
    error_code = scanf("%d", &thingy1 );
    /* later on this might be a PID using getpid() */
    if(error_code ==0) exit(error_code) ; /* bail out on error */

    printf("Please enter a positive integer, for thing2: ");
    error_code = scanf("%d", &thingy2 );
    /* later on, this might be an elapsed time, e_time*/
    if(error_code ==0) exit(error_code) ; /* bail out on error */
    
    /* Allocated the values, input from scanf(), to the return value,
       pointed to by input_value*/
    (*input_value).thing1 = thingy1;
    (*input_value).thing2 = thingy2;

    /* return the value of the smallest argument */
    if(thingy1>thingy2)
        return thingy2;
    else
        return thingy1;

    /* Note that you can use other methods, other than stdio and scanf()
           to obtain data to insert into a queue. */

}

/*
A sample output:

> gcc -Wall -Wextra -Werror -o circular_queue_example_04 circular_queue_example_04.c
> ./circular_queue_example_04 

Please enter a positive integer, for thing1: 3
Please enter a positive integer, for thing2: 5
The list is: { (3,5) }

Please enter a positive integer, for thing1: 7
Please enter a positive integer, for thing2: 11
The list is: { (7,11)  (3,5) }

Please enter a positive integer, for thing1: 0
Please enter a positive integer, for thing2: 0

Dispatch of the items in the queue:
{ 
   item number 1 is  (3,5) , 
   item number 2 is  (7,11) , 
 
}

>

*/
