#include <pthread.h> // POSIX Thread Library
#include <stdio.h> // Standard I/O commands
#include <stdint.h> // Used to supress a warning when casting from (void*) to int
#include <semaphore.h>

/**
 * Number of elements that can be produced
 * before consumption must occur in order to progress.
 */
#define BUFFER_SIZE 10
/**
 * Number of elements that each producer thread 
 * will produce.
 */
#define PRODUCTION_LIMIT 25
/**
 * Number of elements that each consumer thread
 * will consume.
 */
#define CONSUMPTION_LIMIT 35

/**
 * Number of producer and consumer threads.
 * The total number of results produced by all
 * producers needs to equal or exceed the total
 * number of results consumed by all consumers,
 * because otherwise correctly coded consumer
 * threads will block forever waiting for values
 * that are never produced.
 */
#define NUM_PRODUCERS 3
#define NUM_CONSUMERS 2

/**
 * Maximum random pause that threads might be subjected to
 */
#define MAX_PAUSE 20000

sem_t mutual_ex_sem;
sem_t buffer_size_sem;
sem_t empty_space_in_buffer_sem;

/**
 * Block the currently running thread for a small random
 * duration in order to simulate the blocks that are
 * likely to occur in any real application that is producing
 * and consuming data.
 */
void randomDurationPause() {
	// Generate random pause duration
	int duration = rand() % MAX_PAUSE;
	// Block the thread to encourage more interesting interleaving of threads
	usleep(duration); 
}

/** 
 * Used to track the value being "produced".
 * Note that this variable should only ever
 * be accessed by one thread at a time.
 */
int globalProductionCounter = 0;

/**
 * Because this is just a simple model for understanding
 * mutual exclusion, the produce command just produces a
 * sequence of increasing integers, rather than do something
 * actually useful. Unlike in the examples in the book, this
 * command should only be called inside the critical section
 * of a thread. This function also includes a thread ID to
 * output and track the way the program works.
 */
int produce(int threadID) {
	randomDurationPause();
	printf("P%d: produce %d\n", threadID, globalProductionCounter);
	return globalProductionCounter++;
}

/**
 * A value is "consumed" by printing it to the console.
 * Once again, this action is simple, but allows for easy
 * monitoring of how the code functions. The ID of the
 * thread performing the operation is also sent and printed
 * for better clarity, but this has nothing to do with
 * the canonical producer/consumer problem.
 */
void consume(int threadID, int toConsume) {
	printf("C%d: consumed %d\n", threadID, toConsume);
	randomDurationPause();
}

/**
 * buffer is a circular queue, whose "front" and "back" are
 * designated by insertAt and removeAt. The synchronization
 * mechanisms in your code must assure that elements in the
 * buffer are never overwritten before being consumed, or
 * consumed when they don't exist (which result in some leftover
 * value in the buffer being consumed).
 */
int buffer[BUFFER_SIZE];
int insertAt = 0;
int removeAt = 0;

/**
 * Adds a newly produced int to the buffer at the next
 * available slot. You do not need to change this command.
 * However, you do need to assure that it is never called
 * when the buffer is full.
 */
void append(int toAdd) {
	buffer[insertAt] = toAdd;
	// Modulus wraps around the circular queue.
	insertAt = (insertAt + 1) % BUFFER_SIZE; 
}

/**
 * Take the next available item in the circular queue buffer.
 * Value is returned, but not actually removed from the queue.
 * However, correct usage will assure that any value taken out
 * will be overwritten by a new value before a value is taken
 * from the same location again. In particular, this function
 * should never be called when the logical buffer (collection
 * of un-consumed elements) is empty. Do not change this function.
 */
int take() {
	// Result to remove is saved before updating removeAt.
	int result = buffer[removeAt];
	// Modulus wraps around the circular queue.
	removeAt = (removeAt + 1) % BUFFER_SIZE;
	return result;
}

/**
 * Producer thread keeps calling produce and appending the
 * result to the buffer. Proper synchronization must assure
 * mutually exclusive access to the buffer, and prevent
 * buffer overflow.
 * TODO: Add synchronization
 */
void * producer(void * arg) {
	// Unique integer ID is sent to each producer thread
	int threadID = (intptr_t) arg; // intptr_r used to supress a casting warning
	// Announce that the thread has started executing
	printf("P%d entered\n", threadID); 
	int i;
	for(i = 0; i < PRODUCTION_LIMIT; i++) { // Produce a set number of values
		// block on empty space in buffer to ensure we can write to the buffer
		sem_wait(&empty_space_in_buffer_sem);
		// block on mutual ex sem so that we can enforce mutual exclusion in the critical section
		sem_wait(&mutual_ex_sem);

		// Start Critical Section: produce() must appear here to protect globalProductionCounter
		int producedResult = produce(threadID); // produce new value
		append(producedResult); // add new value to buffer
		// End Critical Section

		// exit critical section and unblock
		sem_post(&mutual_ex_sem);
		sem_post(&items_in_buffer_sem);

	}
	// Announce completion of thread 
	printf("P%d finished\n", threadID); 
	return NULL; // No value is returned by thread
}

/**
 * Consumer thread keeps taking a value from the buffer
 * and consuming it. Proper synchronization must assure
 * mutually exclusive access to the buffer, and prevent
 * taking of values from an empty buffer.
 * TODO: Add synchronization
 */
void * consumer(void * arg) {
	// Unique integer ID is sent to each consumer thread
	int threadID = (intptr_t) arg; // intptr_r used to supress a casting warning 
	// Announce that the thread has started executing
	printf("C%d entered\n", threadID); 
	int i;
	for(i = 0; i < CONSUMPTION_LIMIT; i++) { // Consume a set number of values

		// Start Critical Section
		// block on items in buffer, ensuring we can take something
		sem_wait(&items_in_buffer_sem);
		// block on mutual_ex_sem as this is the main semaphore for encapsulating the critical section
		// and enforcing mutual exclusion
		sem_wait(&mutual_ex_sem);

		int consumedResult = take(); // Take from buffer
		consume(threadID, consumedResult); // Consume result

		// unblock by signaling and exiting the critical section
		sem_post(&mutual_ex_sem);
		sem_post(&empty_space_in_buffer_sem);
		// End Critical Section: consume() appears here to assure sequential ordering of output

	}	
	// Announce completion of thread 
	printf("C%d finished\n", threadID); 
	return NULL; // No value is returned by thread
}

/**
 * main function initializes three producer threads and
 * two consumer threads.
 */
int main() {
	printf("Producer/Consumer Program Launched\n");

	pthread_t producerThread[NUM_PRODUCERS], consumerThread[NUM_CONSUMERS];
	int id;

	/*
	 * Initialize the semaphore items_in_buffer with the value 0, and set the middle arg to 0,
	 * indicating, the semaphore is shared among threads.
	 */
	if(sem_init(&items_in_buffer_sem, 0, 0))
    {
        printf("Error creating semaphore n\n");
        return -1;
    }
    /*
     * mutual_ex_sem is initialized in the same fashion as above but initialize it to the value 1 instead.
     */
    if(sem_init(&mutual_ex_sem, 0, 1))
    {
        printf("Error creating semaphore s\n");
        return -1;
    }

 	/*
     * empty_space_in_buffer_sem is initialized in the same fashion as above
     * but initialize it to the value BUFFER_SIZE instead.
     */
    if(sem_init(&empty_space_in_buffer_sem, 0, BUFFER_SIZE))
    {
        printf("Error creating semaphore s\n");
        return -1;
    }

	// Launch producers
	for(id = 1; id <= NUM_PRODUCERS; id++) {
		if(pthread_create(&producerThread[id - 1], NULL, &producer, (void*)(intptr_t) id)) {
			printf("Could not create thread\n");
			return -1;
		}
	}

	// Launch consumers
	for(id = 1; id <= NUM_CONSUMERS; id++) {
		if(pthread_create(&consumerThread[id - 1], NULL, &consumer, (void*)(intptr_t) id)) {
			printf("Could not create thread\n");
			return -1;
		}
	}

	printf("Threads initialized\n");

	// Producers complete
	for(id = 1; id <= NUM_PRODUCERS; id++) {
		if(pthread_join(producerThread[id - 1], NULL)) {
			printf("Could not join thread\n");
			return -1;
		}
	}

	// Consumers complete
	for(id = 1; id <= NUM_CONSUMERS; id++) {
		if(pthread_join(consumerThread[id - 1], NULL)) {
			printf("Could not join thread\n");
			return -1;
		}
	}

	// cleanup
	sem_destroy(&items_in_buffer_sem);
	sem_destroy(&mutual_ex_sem);
	sem_destroy(&empty_space_in_buffer_sem);

	return 0; // Successful termination
}