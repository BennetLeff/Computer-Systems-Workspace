#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

const int MAX = 13;

static void doFactorial(int n, int doPrint);

/*
 * unix_error - unix-style error routine.
 */
inline static void unix_error(char *msg)
{
    fprintf(stdout, "%s: %s\n", msg, strerror(errno));
    exit(1);
}


int main(int argc, char **argv)
{
    int arg;
    int print=1;

    if(argc != 2){
        fprintf(stderr, "Usage: fib <num>\n");
        exit(-1);
    }

    arg = atoi(argv[1]);
    if(arg < 0 || arg > MAX){
        fprintf(stderr, "number must be between 0 and %d\n", MAX);
        exit(-1);
    }

    // add one to arg to account for off by one
    doFactorial(++arg, print);

    return 0;
}

/*
 * Recursively compute the specified number. If print is
 * true, print it. Otherwise, provide it to my parent process using exit (value).
 *
 * NOTE: The solution must be recursive and it must fork
 * a new child for each call. Each process should call
 * doFactorial() exactly once.
 */

static void doFactorial(int n, int doPrint)
{
	// declare exit status code which is really just the return value
	int status = 0;
	// the running total of factorial
	int total = 0;
	// the process is forked here
	int pid = fork();
	// as long as n is positive the factorial is defined
	if (n > 0)
	{
		// if we're in the child
		if (pid == 0)
		{
			// doFactorial with n-1 to decrement value and don't print child values
			doFactorial(n-1, 0);
			// exit with 1, this is the base case.
			exit(1);
		}
		if (pid > 0)
		{
			// wait until the child has exited
			wait(&status);
			// check if the child terminated normally
			if (WIFEXITED(status))
			{
				// add to the total with the least 8 bits of the exit code
			    total += WEXITSTATUS(status);
			    // if we want to print, print
				if (doPrint)
					printf("%d\n", total);
			}
			// don't print from here on out
			doPrint = 0;
			// exit with the next product
			exit (total * n);
		}
	}
}