/* library_random_03.c

by Andrew A.
Last edit: Sat Aug 14 20:52:19 ACST 2021

The purpose of this short program is to demonstrate the use of
pseudo-random number generation, using the library function: rand()

These numbers can be limited, and also scaled,
and converted to floating-point types, if required.

A secondary purpose of the program is to introduce students to time.h

Compilation advice:

gcc -Wall -Wextra -Werror -o library_random_03 library_random_03.c

*/

#define n          12

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
    long i; /* i is a sequence number, as a proxy for time */
    time_t t;
    long rand_val ; /* rand_val is a temporary storage value for the latest random number */

    /* Intializes random number generator */
    srand((unsigned) time(&t));

    /* Print n long-int random numbers */
    for( i = 0 ; i < n ; i++ )
        {
            rand_val = (long) rand() ;
            printf("%ld\t%ld\n", i, rand_val );
        }
    printf("\n\n");

    return(0);
}


/*
For more information try:
   man 3 rand
at a LINUX command line, giving:

RAND(3)                                                     Linux Programmer's Manual

NAME
       rand, rand_r, srand - pseudo-random number generator

SYNOPSIS
       #include <stdlib.h>

       int rand(void);

etc... etc...

*/
