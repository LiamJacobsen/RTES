/* linked_list_example_04.c */

/* by Andrew A.*/
/* Last edit: Tue 18 Jul 2023 22:29:14 (Singapore) */

/* A simple demonstration of a linked-list*/

/* Compilation advice:
   gcc -Wall -Wextra -Werror -o linked_list_example_04 linked_list_example_04.c
*/ 


#include <stdio.h>
#include <stdlib.h>

/* create a type for a singly-linked list of integers */
typedef struct _list_item {
  int val;
  struct _list_item *next;
} list_item;


/* function prototypes */
list_item *add_list_item(list_item *head, int input_value);
void print_list_items(list_item *head);

/* create a linked list, using entries from stdin */
int main()
{
  /* create pointers for the head of the list. */
  list_item *head=NULL; /* define the head of the list */

  int input_value ;

  printf("Please enter a positive integer: ");
  if( !scanf("%d", &input_value)) return 1;
  while(input_value > 0)

    {
        head = add_list_item(head,input_value);
        print_list_items(head); /* trace print the list */

        printf("Please enter a positive integer: ");
        if( !scanf("%d", &input_value)) return 1;
    }

  printf("\n\n");

  return 0;
}
/* end of main() */


/* insert items into a list */
list_item *add_list_item(list_item *head, int input_value)
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
          printf(" %d ", ptr_to_list_item->val);
      }
  
  printf("}\n");
}
