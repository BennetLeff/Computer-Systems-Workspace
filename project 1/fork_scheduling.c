/**
 * This program forks many new child processes
 * in order to see how the children are scheduled
 * relative to the parent. After each fork, both
 * the child and the parent will print a message to
 * the console containing their PID. Afterward, each
 * child process will immediately exit, but the parent
 * will continue spawning children until finished.
 */

#include <stdio.h> // Standard IO using printf
#include <sys/unistd.h> // Imports fork and related functions
#include <stdlib.h>
/**
 * Functions that you may need: fork, getpid, getppid 
 */

// Number of processes to fork
#define NUM_CHILDREN 10

int main() {
	int i; // Loop variable
	printf("Parent process %d will launch %d child processes\n", getpid(), NUM_CHILDREN);

	for (i = 0; i < NUM_CHILDREN; i++) {

		// YOUR CODE HERE
		int pid = fork();
		if (pid == 0)
		{
			printf("Parent %d launched %d \n", getppid(), getpid());
			exit(1);
		}
		if (pid == -1)
		{
			printf("Error when forking... \n");
		}

	}
	return 0; // Exit main program with success
}

