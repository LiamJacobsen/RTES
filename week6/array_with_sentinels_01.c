/*
array_with_sentinels_01.c

How it is possible to process computation times, et cetera, using sentinels


 compilation advice:
  
gcc -Wall -Wextra -Werror -o array_with_sentinels_01 array_with_sentinels_01.c

*/



#include<stdio.h>

#define MAX_PHASES 2 
#define MAX_PROCS  4

static int C[MAX_PHASES][MAX_PROCS] =
 {
    { 1,  3,  2, 2},
    {-1, -1, -1, 6},
 };

int main(void)
{
  int row, col ;

  printf("C[row][col]=\n");
  for (row=0; row<MAX_PHASES; row++ )
     {
        for(col=0; col<MAX_PROCS; col++)

           /* A sentinel is a special value thast convets information but is not processed in the usual way*/
           if(C[row][col] >= 0)
               /* usual procss */
               printf("%d\t",C[row][col] );
            else 
               /* spcial case, flagged by the sentinel*/
               printf(" \t");
        printf("\n");

     }    
    
    printf("\n");
    return 0;

}