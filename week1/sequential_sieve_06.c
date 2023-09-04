/* sequential_sieve_07.c */
/*
   This is a sequential version of the Sieve of Eratosthenes,
   where the fundamental operations on the sieve have been encapsulated
   into functions.

   By Andrew A.
   Last edit: Tue 18 Jul 2023 22:29:14 (Singapore)

   The style has been standardized using: astyle --style=gnu
   This code has been checked, using splint

   compilation advice:

   gcc -Wall -Wextra -Werror -o sequential_sieve_06 sequential_sieve_06.c

*/

#include <stdio.h>
#include <stdlib.h>

/* Looking for primes less than 100,489
   This has been cheked against the table of primes
   in "Handbook of mathematical Functions" by Abramowitz & Stegun
   Page 870
*/

/* BYTE is GNU-speak for a "byte" */
#define BYTE unsigned char

#define BIG_FAC 317
#define N_VALS (BIG_FAC*BIG_FAC+1)
/* note that BIG_FAC*BIG_FAC will never be prime*/

#define PRIME     (BYTE) 1
#define NOT_PRIME (BYTE) 0

/* This program should identify all prime numbers less than N_VALS */


static BYTE sieve[N_VALS];
/* This allocation of memory is inefficient, but it is quick to run
   because groups of bits are not being constantly unplacked, and re-packed... */

/* function prototypes */
static int sieve_initialise(BYTE*);
static int sieve_out(int);
static int sieve_list(BYTE*);

/* main program starts here */
int main(void)
{

    int k ;         /* a counter */
    int big_prime ; /* The largest prime so far ...*/

    printf("A table of primes less than %d\n", (int) (BIG_FAC*BIG_FAC+1) );

    (void) sieve_initialise(sieve); /* initialise the sieve */
    /* note that an array identifier is really a pointer */

    /* sieve-out (ie: eliminate) a couple of special cases */
    sieve[0]= NOT_PRIME ; /* 0 is not a prime*/
    sieve[1]= NOT_PRIME ; /* 1 is not a prime*/

    /* start by eliminiating all even numbers*/
    (void) sieve_out(2);

    /* and multiples of 3 */
    (void) sieve_out(3);
    big_prime = 3 ;


    /* sieve out all the other values, one prime at a time*/
    while(big_prime < BIG_FAC)
        {
            /* search for the next prime*/
            k = big_prime +1 ;
            while(sieve[k]==NOT_PRIME) k++;
            /* k is now the smallest prime that has not yet been sieved out */
            big_prime = k ;
            (void) sieve_out(big_prime);  /* remove multiples of big_prime from the sieve*/
        }

    /* display all the primes that have been found */
    (void) sieve_list( sieve);

    exit(EXIT_SUCCESS);
}
/* main program ends here */


/* function bodies begin here */

/* initialise the sieve to an initial value of: PRIME*/
static int sieve_initialise(BYTE *sieve)
{
    int k ; /* a loop counter */

    /*initialise the sieve to possibly_prime == 1 */
    for (k=0; k<N_VALS ; k++) sieve[k] = PRIME;

    return EXIT_SUCCESS ;
}

/* sieve out multiples of k*/
static int sieve_out(int k)
{
    /* remove multiples of k from the sieve*/
    /* This is a sieving process that can be executed in parallel in threads*/
    int j ; /* a counter */

    /* sieve out all non-primes that are (multiple) factors of k*/
    for(j=2*k; j < N_VALS; j += k) sieve[j]= NOT_PRIME ;

    return EXIT_SUCCESS;
}

/* list the contents of the sieve */
static int sieve_list(BYTE *sieve)
{
    /* sieve is a pointer to the location where the sieve is stored */
    /* sieve_list() lists all the numbers in the sieve
       that have been identified as being prime */

    int k, j ; /* loop counters */
    k=1; /* k is just a counter for the number of primes... */

    for(j=2; j < N_VALS; j++)
        if(sieve[j]==PRIME)
            {
                printf("%d",j); /* print out the next prime */

                if(k>=10)
                    {
                        /* terminate a line with \n */
                        k=1;
                        printf("\n");
                    }
                else
                    {
                        /* normally just insert a tab between numbers */
                        k++;
                        printf("\t");
                    }
            }
    printf("\n");

    return EXIT_SUCCESS ;
}
