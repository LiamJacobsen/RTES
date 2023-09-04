/* stdio_test_01.c 

   Last edit: Tue 18 Jul 2023 22:29:14 (Singapore)

   like hello_world.c , but it also tests
   scanf() as well as printf().

   complilation advice

   gcc -Wall -Wextra -Werror -o stdio_test_01 stdio_test_01.c 

*/

#include <stdio.h>
/* 
   Please note that you cannot use functions from stdio.h without including
   the header in your code. 

   More information can be found about stdio.h by referring to the manual page:
   man -S 3 stdio
*/

int main(void)
{
   long m;
   printf("Please enter an integer value for  m   :     ");
   scanf("%ld",&m);
   /* Please note that scanf requires an address of the location for the data, &m
      not the actual variable name, m.  */

   printf("The integer value that you entered was : m = %ld\n",m);
   /* Please note that printf uses the actual variable name, m.  */

    printf("The address of m is : &m = %p ,\nwhich corresponds to decimal %ld\n",&m, (long)(&m));
   /* Please note that printf uses the actual variable name, m.  */  

   return 0 ;
	
}