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

    doFactorial(arg, print);

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
// static void doFactorial(int n, int doPrint)
// {
// 	int status = 0;
// 	int total = 0;

// 	if (n == 1)
// 		exit(n);

// 	if (n > 1)
// 	{		
// 		int pid = fork();
// 		if (pid == 0)
// 		{
// 			doFactorial(n-1, doPrint);
// 			exit(n-1);
// 		}

// 		if (pid > 0)
// 		{	
// 			if (WIFEXITED(status))
// 			{
// 			    total += WEXITSTATUS(status);
// 				printf("%d\n", status);
// 			}
// 			exit(total - 1);
// 		}

// 	}
// }

static void doFactorial(int n, int doPrint)
{
	int status = 0;
	int total = 0;
	int pid = fork();
	if (pid == 0)
	{
		exit(n);
	}
	if (pid > 0)
	{
		wait(&status);
		if (WIFEXITED(status))
		{
		    total += WEXITSTATUS(status);
			printf("%d %d\n", status, total);
		}
	}
}