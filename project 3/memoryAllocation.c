
#include <stdlib.h> // for exit
#include <stdio.h> // For IO
#include <stdbool.h> // For bool type

// Number of memory blocks
#define MEM_SIZE 128
// Each frame/page takes two memory blocks
#define FRAME_SIZE 2

// Array simulates memory
int memory[MEM_SIZE];

/**
 * Fill memory array with zeroes, which represent empty space (available for allocation)
 */
void clearMemory() {
	int i;
	for(i = 0; i < MEM_SIZE; i++) { // Initialize memory
		memory[i] = 0; // 0 indicates free memory
	}
}

// Remember last allocation for next-fit algorithm
int lastAllocationPoint = 0;

/**
 * Fill a specified chunk of memory with a "process" id, which
 * in our simple simulation means that the memory block has been
 * allocated to that process. The function fills size number of
 * slots with the id, starting at the startBlock
 */
void fillMemory(int startBlock, int id, int size) {
	printf("Allocate %d through %d to %d\n", startBlock, startBlock + size - 1, id);
	int i;
	for(i = 0; i < size; i++) {
		if(memory[startBlock + i] != 0) { // Useful check for debugging: Never fill reserved space
			printf("ERROR! Cell %d not empty. Contains %d\n", startBlock + i, memory[startBlock + i]);
			exit(1); // Exit with error
		}
		if(startBlock + i >= MEM_SIZE) { // Useful check for debugging: Never go outside of bounds
			printf("ERROR! Cell %d out of bounds %d\n", startBlock + i);
			exit(1); // Exit with error
		}
		memory[startBlock + i] = id; // "allocate" block to id
	}
	lastAllocationPoint = startBlock + size; // Information tracked for next-fit algorithm
}

/**
 * Increment every time vacateProcess is called
 */
int processesVacated = 0;

/**
 * When memory gets full, it will be necessary to vacate "processes."
 * This function scans memory and deallocates (sets to zero) all slots 
 * allocated to the process with the given id.
 */
void vacateProcess(int id) {
	printf("vacate %d\n", id);
	processesVacated++;
	for (int i = 0; i < MEM_SIZE; i++)
	{
		if (memory[i] == id)
		{
			memory[i] = 0;
		}
	}
}

/**
 * This is a pointer to a function. The policy is the memory allocation policy.
 * You will be implementing multiple policies, each with the same signature:
 * a bool is returned indicating the success of the allocation, and the two int
 * inputs are the id number to reserve the memory for, and the size 
 * (number of blocks) to reserve. This value is set in the main function based
 * on command-line parameters.
 */
bool (*policy)(int,int);


// returns the id of the longest continuous sequence in memory
int find_longest_contiguous_seq()
{
	int longest_size = 0;
	int current_size = 0;
	int id_of_longest = 0;
	int current_id = 0;

	for (int i = 0; i < MEM_SIZE; i++)
	{
		// if indexed element is not the current id, we've reached a new sequence
		if (memory[i] != current_id)
		{
			// if the size of the previous sequence was longer than the longest
			// size, we must update the id, and longest_size
			if (current_size > longest_size)
			{
				longest_size = current_size;
				id_of_longest = memory[i - 1];
				current_size = 0;
			}

			current_id = memory[i];
		}

		current_size++;
	}

	return id_of_longest;
}

/**
 * For each allocation policy below, assign the process id to
 * a contiguous region of memory with "size" number of blocks. Return
 * true on success, and false if no region large enough exists.
 */

bool firstFit(int id, int size) {
	int index = 0;

	// count up to MEM_SIZE - size, because past that index, the number of blocks
	// requested will not fit.
	while (index < (MEM_SIZE - size))
	{
		// keep incrementing until we get a zero so there's room
		if (memory[index] != 0)
		{
			index++;
		}
		// if we hit a zero
		else
		{
			for (int i = 0; i < size; i++)
			{
				memory[index+i] = id;
			}

			return true;
		}
	}
	// if we didn't return true, we need to evict a process
	int id_to_evict = find_longest_contiguous_seq();

	vacateProcess(id_to_evict);
	
	// now that we've evicted the process, we can call firstFit again
	firstFit(id, size);

	return false;
}

bool nextFit(int id, int size) {
	// TODO
	return false;
}

bool bestFit(int id, int size) {
	// TODO
	return false;
}

bool worstFit(int id, int size) {
	// TODO
	return false;
}

/**
 * The pages policy is special in that it may allocate
 * non-contiguous regions of memory to the process id.
 * However, a full frame must be reserved for each chunk
 * of allocation, even if it is not used by the process.
 */
bool pages(int id, int size) {
	// TODO
	return false;
}

// Track the number of compaction events
int compactionEvents = 0;

/**
 * Compact memory: This function should never be used in
 * conjunction with the paging policy, but all of the others
 * need it.
 */
void compaction() {
	// TODO
}

// Used to track whether the paging policy was chosen, so compaction can be avoided
bool paging = false;

/**
 * Allocate memory of appropriate size to the process with id using the
 * chosen policy. For paging, allocation should only fail if there are not
 * enough free frames, in which case the process occupying the most memory
 * should be vacated before trying again. 
 *
 * For policies that allocate in contiguous space,
 * failure to allocate should result first in a check on the number of
 * remaining free blocks in memory. If there is enough space to hold the
 * process, but the space is fragmented, then compaction should occur.
 * Otherwise, the policy occupying the most space in memory should be
 * vacated before repeating the attemt to allocate.
 */
void allocate(int id, int size) {
	// TODO
	policy(id, size);
}

/**
 * Main function runs a memory management simulation based on an input file,
 * and outputs the final state of memory to a specified output file. The command
 * line arguments dictate how to set up the simulation.
 */
int main(int argc, char *argv[]) {
	// Proper usage consists of 4 arguments:
	// 0: C file: name of program being run
	// 1: input fiename: file with sequence of memory requests (one int per line)
	// 2: output filename: file that final memory contents will be (over)written to
	// 3: memory allocation policy: ff=first-fit, bf=best-fit, nf=next-fit, wf=worst-fit, pages=paging 
	if(argc != 4) {
		printf("Incorrect arguments. Expected:\n");
		printf(" 0: C file: name of program being run\n");
		printf(" 1: input fiename: file with sequence of memory requests (one int per line)\n");
		printf(" 2: output filename: file that final memory contents will be (over)written to\n");
		printf(" 3: memory allocation policy: ff=first-fit, bf=best-fit, nf=next-fit, wf=worst-fit, pages=paging\n");
		printf("%d == argc", argc);
		return 1; // Error
	}

	// Check for valid memory allocation policy, using strcmp, then assign policy variable
	if(strcmp(argv[3], "ff") == 0) { 
		printf("first-fit allocation\n");
		policy = &firstFit;
	} else if(strcmp(argv[3], "nf") == 0) { 
		printf("next-fit allocation\n");
		policy = &nextFit;
	} else if(strcmp(argv[3], "bf") == 0) { 
		printf("best-fit allocation\n");
		policy = &bestFit;
	} else if(strcmp(argv[3], "wf") == 0) { 
		printf("worst-fit allocation\n");
		policy = &worstFit;
	} else if(strcmp(argv[3], "pages") == 0) { 
		printf("simple paging\n");
		policy = &pages;
		paging = true; // indicates that compaction should not occur
	} else {
		printf("Invalid memory allocation policy\n");
		printf(" ff=first-fit, bf=best-fit, nf=next-fit, wf=worst-fit, pages=paging\n");
		return 1; // Error
	}

	printf("Reading from file: %s\n", argv[1]); // Second argument is input filename from user
	FILE *input, *output;
	input = fopen(argv[1], "r"); // Open file in read mode
	if(input == NULL) { // Filed to read file
		printf("Problem reading file %s\n", argv[1]);
		return 1; // Error
	}

	clearMemory(); // Clear memory before allocating it for processes
	int requestID = 1; // Start IDs at 1 because 0 indicates empty memory
	int requestSize; // Holds values read from file
	while (fscanf(input, "%d", &requestSize) != EOF) { // Scan numbers into requestSize until end of file
		printf("%d requested %d blocks\n", requestID, requestSize); // Announce the request
		allocate(requestID, requestSize); // Claim space for "process"
		requestID++; // For simplicity, each request is from a new "process"
	}
	fclose(input); // Close the file

	printf("%d processes vacated\n", processesVacated);
	printf("%d compaction events\n", compactionEvents);

	// Output the state of memory at the end of the simulation
	printf("Writing to file: %s\n", argv[2]); // Third argument is output filename from user
	output = fopen(argv[2], "w"); // Open file in write mode
	int i;
	for(i = 0; i < MEM_SIZE; i++) {
		fprintf(output, "%d\n", memory[i]);
	}
	fclose(output); // Close the file

	return 0;
}