/* collatz_01.c 

Investigate the Collatz conjecture, for small integer values

Proving the general case for the Collatz conjecture, for all natural numbers, is beyond the scope of the course:

https://en.wikipedia.org/wiki/Collatz_conjecture


Last edit: Tue 18 Jul 2023 22:29:14 (Singapore)

compilation advice:

gcc -Wall -Wextra -Werror -o collatz_01 collatz_01.c

*/

#include <stdio.h>
#include <stdlib.h>

long collatz_iteration(long n)
{
   if((n%2)==0)
      return (n/2) ;
   else
   	  return ((3*n)+1);
}

int main(void)
{
   long m;     /* The equivalence-class to be investigated */
   long k = 0; /* the number of Collatz iterations before m=1 is reached */

   printf("Please enter an integer value for m : ");
   scanf("%ld",&m);
   printf("m = %ld\n",m);

   if(m<=0) exit(0); /* The Collatz conjecture only applies to Natural numbers*/ 

   /* perform one initial iteration*/
   m = collatz_iteration(m);
   k++ ;

   /* In principle this iteration could run for ever, but no example has been found */
   while (m != 1)
   {
      printf("m = %ld\n",m);
      m = collatz_iteration(m);
      k++ ; 	
   }

   printf("m = %ld\n\nk = %ld\n",m,k);

   return (int) m ;
	
}