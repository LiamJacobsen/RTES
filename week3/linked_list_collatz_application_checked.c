/* linked_list_example_checked.c */

/* by Andrew A.*/
/* Last edit: Sun 30 Jul 2023 21:05:51 ACST (Adelaide) */

/* A simple demonstration of a linked-list
   to store values from the Collatz iteration 

   The purpose of this adaption is to enable students to adapt a storage mechanism (linked list)
   to data from a different source (the Collatz iteraiton)

   The locations of major edits are indicated by comments with ###

   */

/* Compilation advice:
   gcc -Wall -Wextra -Werror -o linked_list_collatz_application_checked linked_list_collatz_application_checked.c
*/ 

#include <stdio.h>
#include <stdlib.h>

/* create a type for a singly-linked list of long integers */
typedef struct _list_item {
  long val;
  struct _list_item *next;
} list_item;


/* function prototypes */
list_item *add_list_item(list_item *head,long input_value);
void print_list_items(list_item *head);
long collatz_iteration(long);


/* create a linked list, using entries from stdin */
int main()
{
  /* create pointers for the head of the list. */
  list_item *head=NULL; /* define the head of the list */

  /* ### we need a variable, m, to temporarily store incoming data */
  long m;     /* The equivalence-class to be investigated */
  long k = 0; /* the number of Collatz iterations before m=1 is reached */

/* ### the old linked-list main has been left here, just as a reference
  in general it is better to delete old, dead code, rather than comment it out...  */

/*

  printf("Please enter a positive integer: ");
  if( !scanf("%ld", &input_value)) return 1;
  while(input_value > 0)

    {
        head = add_list_item(head,input_value);
        print_list_items(head); 

        printf("Please enter a positive integer: ");
        if( !scanf("%ld", &input_value)) return 1;
    }
   */

/* ### The new main program comes from the Collatz example
   note the change of the variable-name for the temporary input variable from "input_value" to "m"*/
printf("Please enter an integer value for m : ");
   scanf("%ld",&m);
   printf("m = %ld\n",m);

   if(m<=0) exit(0); /* The Collatz conjecture only applies to Natural numbers*/ 

  /* At this point, we know that the value of m is a valid Natural number*/  
  head = add_list_item(head,m);
  /* Only needed for trace-print */
  print_list_items(head);

   /* perform one initial iteration*/
   m = collatz_iteration(m);
   k++ ;

   /* In principle this iteration could run for ever, but no example has been found */
   while (m != 1)
   {

      head = add_list_item(head,m);
      /* Only needed for trace-print */
      print_list_items(head);

      m = collatz_iteration(m);
      k++ ;   
   }

   head = add_list_item(head,m);
   print_list_items(head);
   
   printf("\nnumber of iterations: k = %ld\n",k);

  printf("\n\n");

  return 0;
}
/* end of main() */

/* ### These are just the standard functions from the linked-list example */
/* insert items into a list 
 ### Please note that these items are stored in order of the numbers, not in irder of the iteration number
*/
list_item *add_list_item(list_item *head, long input_value)
{
  /* use this pointer to examine possible entry points in the list */
  list_item *entry;

  /* use this pointer to traverse the list, and to record the last possible entry point. */
  list_item *last_entry;

  /* this creates a pointer to a new variable of type list_item item but does *NOT* allocate space for any data, for the item */
  list_item *new_list_item;
  /* We use malloc() to allocate some space for the next item in the list, but not to assign values */
  new_list_item=(list_item*)malloc(sizeof(list_item));


  /* traverse the list until we can find the a correct place to insert the new item */
  if (head==NULL)
    {
      /* the list is empty, starting a new list and allocating a value to head*/
      head                = new_list_item;
      new_list_item->val  = input_value;
      new_list_item->next = NULL;
    }
  else
    {
      /* The list is non-empty, Start the traverse at the head of the list */
      entry = head;

      if ( (head->val) > input_value)
        {
	  /* the input value is small, insert the input value at the head of the list */
           new_list_item->next  = head ;
           head                 = new_list_item;
           new_list_item->val   = input_value;

        }
      else
        {
          /* traverse the non-empty list as far as required  */
           while(  (entry->val) <= input_value )  
              {
                 /* keep track of the possible entry points */
                 last_entry = entry ;
                 /* We may examine the next record, during the next iteration of the loop */ 
                 entry = entry->next ; 
	         if( entry == NULL ) break ;
               }
           /* the pointer  "last_entry"  will now point to the correct entry point for new data*/
           /* the pointer  "entry"       will now point to the next item in the list, if any */
           new_list_item->next = last_entry->next ;
           last_entry->next    = new_list_item;
           new_list_item->val  = input_value;
        }
    }


  return head;
}

/* trace print out the list to stdout */
void print_list_items(list_item *head)
{
  list_item *ptr_to_list_item;

  printf("The list is: {");

  /* Traverse the list. */
  for ( ptr_to_list_item= head;
        ptr_to_list_item!= NULL; 
        ptr_to_list_item=ptr_to_list_item->next) 
      {
          printf(" %ld ", ptr_to_list_item->val);
      }
  
  printf("}\n");
}


/* #### This function defines the Collatz iteration */
long collatz_iteration(long n)
{
   if((n%2)==0)
      return (n/2) ;
   else
      return ((3*n)+1);
}


