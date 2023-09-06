/* concurrent_sum_03.c */

/* 	Adapted from Kelley & Pohl, by Andrew A.
	last edit and test: Mon Aug 22 00:14:01 ACST 2022

        reformatted with astyle --style=gnu

        compilation advice:

	gcc -Werror -Wall -Wextra -o concurrent_sum_03 concurrent_sum_03.c

*/

/*
	The purpose of this program is to demonstrate how pipes can be used to
	convey information between separate processes. in this case the "child"
	processes read information from their copy of an array, a[], and pass
	the row sub-totals through pipes to the parent process, which reads the sub-totals
	and then aggregates them to form a grand total, which is printed out.

	Technical note, The grand sum of this magic square is 1+2+ ... +9 which is a
	triangular number sum = N*(N+1)/2 = 9*10/2 = 45.

	For a general M by M magic square, N = M*M, then the total sum is M*M*(M*M+1)/2.
	For M=3 we have sum = 3*3*(3*3+1)/2 = 45.
*/

/*
	More documentation about pipes can be found on-line, for example
	http://tldp.org/LDP/lpg/node11.html

*/

/* Use pipes to sum N rows concurrently. */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define   N   3

static int    add_vector(int v[]);
static void   error_exit(char *s);

int main(void)
{
    int a[N][N] = {{ 8, 1, 6}, { 3, 5, 7}, { 4, 9, 2}};
    int i, row_sum;
    int sum =0 ;
    int pd[2];    /* pipe descriptors */

    if(pipe(pd) == -1) /* create a pipe */
        error_exit("pipe() failed");

    for (i=0; i< N; ++i)
        if(fork()==0)
            {
                /* child stuff goes here*/
                row_sum = add_vector(a[i]);
                if(write(pd[1],&row_sum,sizeof(int)) == -1)
                    error_exit("write() failed");
                return 0 ; /* return from the child */
            }

    /* parent process */
    for(i=0; i<N; ++i)
        {
            if(read(pd[0], &row_sum, sizeof(int)) == -1 )
                error_exit("read() failed");
            sum += row_sum;
        }

    printf("Sum of the array = %d\n",sum);
    exit(EXIT_SUCCESS);
}

static int add_vector(int v[])
{
    int i, vector_sum = 0;
    for(i=0; i<N; ++i)
        vector_sum += v[i];
    return vector_sum;
}

static void error_exit(char *s)
{
    fprintf(stderr, "\nerror: %s - bye!\n",s);
    exit(EXIT_FAILURE);
}
