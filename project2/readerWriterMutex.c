#include <pthread.h> // POSIX Thread Library
#include <stdio.h> // Standard I/O commands
#include <stdint.h> // Used to supress a warning when casting from (void*) to int
#include <stdbool.h> // Used to detect when writers have finished

/**
 * Number of times each writer thread executes its critical section
 */
#define WRITE_ACTIONS 20

/**
 * Number of reader and writer threads. It is particularly interesting
 * to have several writers because it creates a chance for different
 * values to be written to the buffer (race condition). The purpose of 
 * having multiple readers is to assure that each can access the buffer 
 * at the same time.
 */
#define NUM_READERS 3
#define NUM_WRITERS 3

/**
 * Maximum random pause that threads might be subjected to
 */
#define MAX_PAUSE 20000

// we only need one mutex now to encapsulate the critical section.
pthread_mutex_t rw_mutex;

/**
 * Block the currently running thread for a small random
 * duration in order to simulate the blocks that are
 * likely to occur in any real application that is writing
 * and reading data to and from a buffer.
 */
void randomDurationPause() {
	// Generate random pause duration
	int duration = rand() % MAX_PAUSE;
	// Block the thread to encourage race conditions
	usleep(duration); 
}

/**
 * This simple buffer is designed to hold two copies of an executing
 * writer's identifier. Random pauses will be used to delay writing to 
 * and reading from the buffer, which will encourage it to contain
 * inconsistent data if proper synchronization is lacking.
 */
int buffer[2];

/**
 * Will be true as long as any writer thread is active. Access to this
 * variable does not need to be synchronized because it will only ever
 * be written to once.
 */
bool stillWriting = true;

/**
 * Each writer thread should execute its critical section WRITE_ACTIONS
 * number of times. The critical section should write the threadID to
 * both indices of the buffer, but a random pause is needed to encourage
 * a race condition.
 */
void * writer(void * id) {
	// Unique integer ID is sent to each writer thread
	int threadID = (intptr_t) id; // intptr_r used to supress a casting warning
	// Announce that the thread has started executing
	printf("W%d entered\n", threadID); 

	// write WRITE_ACTIONS number of times.
	for (int i = 0; i < WRITE_ACTIONS; i++)
	{
		// lock on rw_mutex to enforce mutual exclusion within the critical section
		pthread_mutex_lock(&rw_mutex);


		// Get both buffer elements which should be the same value.
		buffer[0] = threadID;
		buffer[1] = threadID;

		// we can exit the critical section by unlocking rw_mutex.
		pthread_mutex_unlock(&rw_mutex);


		// pause to encourage a race condition.
		randomDurationPause();

		// we're in the for loop so we're still writing.
		stillWriting = true;
	}

	stillWriting = false;
	
	printf("W%d finished\n", threadID); 
	return NULL;
}

/**
 * Each reader thread should execute its critical section as long as there 
 * are active writer threads. The critical section should read the values from
 * both indices of the buffer, but there should be a random pause to 
 * encourage a race condition. In addition, each read action should
 * send output to the console indicating whether the values read were
 * valid or erroneous.
 */
void * reader(void * id) {
	// Unique integer ID is sent to each reader thread
	int threadID = (intptr_t) id; // intptr_r used to supress a casting warning
	// Announce that the thread has started executing
	printf("R%d entered\n", threadID); 

	// while the writer is still writing
	while (stillWriting){

		// reading is fine to do outside of the critical section
		// when using a mutex to wrap the write
		int b_0 = buffer[0];
		int b_1 = buffer[1];

		// we've succeeded if the buffer holds the same values.
		if (b_0 == b_1)
		{
			printf("Successful read of buffer.\n");
		}
		else 
		{
			printf("Error reading buffer: \n");
			printf("the first read in thread: %d Consumed threadID %d\n but ", threadID, b_0);
			printf("the second read in thread: %d Consumed threadID %d\n\n", threadID, b_1);	
		}

		// pause to encourage data races
		randomDurationPause();
	}

	printf("R%d finished\n", threadID); 
	return NULL;
}

/**
 * main function initializes reader and writer threads. 
 */
int main() {
	printf("Readers/Writers Program Launched\n");

	pthread_t readerThread[NUM_READERS], writerThread[NUM_WRITERS];
	int id;

	// init the mutex to default
	pthread_mutex_init(&rw_mutex, NULL);

	// Launch writers
	for(id = 1; id <= NUM_WRITERS; id++) {
		if(pthread_create(&writerThread[id - 1], NULL, &writer, (void*)(intptr_t) id)) {
			printf("Could not create thread\n");
			return -1;
		}
	}

	// Launch readers
	for(id = 1; id <= NUM_READERS; id++) {
		if(pthread_create(&readerThread[id - 1], NULL, &reader, (void*)(intptr_t) id)) {
			printf("Could not create thread\n");
			return -1;
		}
	}

	printf("Threads initialized\n");

	// Writers complete
	for(id = 1; id <= NUM_WRITERS; id++) {
		if(pthread_join(writerThread[id - 1], NULL)) {
			printf("Could not join thread\n");
			return -1;
		}
	}

	stillWriting = false; // Let readers know that writing is finished

	// Readers complete
	for(id = 1; id <= NUM_READERS; id++) {
		if(pthread_join(readerThread[id - 1], NULL)) {
			printf("Could not join thread\n");
			return -1;
		}
	}

	// cleanup
	pthread_mutex_destroy(&rw_mutex);

	return 0; // Successful termination
}
