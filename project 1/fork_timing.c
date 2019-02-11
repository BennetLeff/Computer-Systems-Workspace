/**
 * This program forks many new child processes
 * in order to time the cost of copying a process.
 * The fork is executed repeatedly and the time
 * taken is averaged to account for the small time
 * scales, and to get a more reliable result. 
 * To focus only on the time taken to fork a 
 * process, each child process immediately exits.
 */

#include <stdio.h> // Standard IO using printf
#include <time.h> // Imports clock function and clock_t type
#include <sys/unistd.h> // Imports fork and related functions
/**
 * Functions that you may need: fork, getpid, getppid 
 */
#include <stdlib.h>

// Number of processes to fork
#define NUM_CHILDREN 1000

int main() {
	int i; // Loop variable
	printf("Parent process %d will launch %d child processes\n", getpid(), NUM_CHILDREN);

	// Start the clock
	clock_t startTime = clock();

	for (i = 0; i < NUM_CHILDREN; i++) {

		// YOUR CODE HERE
		int pid = fork();
		if (pid == 0)
		{
			exit(1);
		}
		if (pid == -1)
		{
			printf("Error when forking... \n");
		}

	}

	// Stop the clock
	clock_t endTime = clock();
	long totalTicks = endTime - startTime; // Calculate elapsed time in CPU clock ticks
	printf("Total elapsed time is: %ld ticks\n", totalTicks);
	float ticksPerProcess = (1.0*totalTicks)/NUM_CHILDREN; // Average ticks per process
	printf("Average elapsed time is: %9.7lf ticks per process.\n", ticksPerProcess);
	float secondsPerProcess = ticksPerProcess/CLOCKS_PER_SEC; // Average seconds per process
	printf("Which is: %9.7lf seconds per process.\n", secondsPerProcess);

	return 0; // Exit main program with success
}

