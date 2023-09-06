/* fork_and_shell_03.c 

   demonstrate the use of fork() and exec()
   to implement a rudimentary shell program.

   Adapted from a program by Richard Stevens and Stephen Rago

    last edit: Mon Aug 22 00:29:21 ACST 2022
   by A^2

   formatted with astyle --style=gnu

   compilation advice:

   gcc -Werror -Wall -Wextra -o fork_and_shell_03 fork_and_shell_03.c

*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>	/* needed for getpid() */
#include <unistd.h>	/* needed for getpid() */
#include <sys/wait.h>
#include <string.h>

#define MAXLINE   4096		/* max line length */
#define ERRLENGTH   80
int
main (void)
{
    char buf[MAXLINE];
    char str[MAXLINE+ERRLENGTH];			/* for printing error messages */
    pid_t pid;
    int status;
    printf ("%% ");		/* print prompt (printf requires %% to print %) */
    while (fgets (buf, MAXLINE, stdin) != NULL)
        {
            if (buf[strlen (buf) - 1] == '\n')
                buf[strlen (buf) - 1] = 0;	/* replace newline with null */

            if ((pid = fork ()) < 0)
                perror ("fork error");
            else if (pid == 0)
                {
                    /* we know that this process is the child */
                    execlp (buf, buf, (char *) NULL);
                    sprintf (str, "couldn’t execute: %s", buf);
                    perror (str);
                    exit (127);
                }
            else
                {
                    /* we know that this process is the parent */
                    if ((pid = waitpid (pid, &status, 0)) < 0)
                        perror ("waitpid error");
                    printf ("%% ");
                }
        }
    /* end of while() */
    exit (0);
}
/* end of main() */
