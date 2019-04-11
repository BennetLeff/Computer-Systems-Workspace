
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
	printf("completed vacating... \n" );
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

int find_longest_contiguous_empty_space()
{

	int largest_space = 0;
	int temp_largest_space = 0;

	int last_index_largest_space = 0;

	for (int i = 0; i < MEM_SIZE; i++)
	{
		if (memory[i] == 0)
		{
			// printf("Got a zero.... \n");
			temp_largest_space += 1;
			// printf("temp_largest_space == %d\n", temp_largest_space);
		}

		if (memory[i] != 0)
		{
			if (temp_largest_space > largest_space)
			{
				largest_space = temp_largest_space;
				last_index_largest_space = i - largest_space;
			}

			temp_largest_space = 0;
		}

		if (temp_largest_space > largest_space)
		{
			largest_space = temp_largest_space;
			last_index_largest_space =  i - largest_space;
		}
	}
	
	return last_index_largest_space+1;
}


// size == tightest space, i.e. size of space necessary for blocks i.e. size
int find_tightest_space(int size)
{
	int smallest = MEM_SIZE;
	int current_smallest = 0;

	int last_index = -1;

	for (int i = 0; i < MEM_SIZE; i++)
	{
		if (memory[i] == 0)
		{
			current_smallest += 1;
		}

		if (memory[i] != 0)
		{
			if (current_smallest < smallest && current_smallest > size)
			{
				smallest = current_smallest;
				last_index = i - current_smallest;
			}

			current_smallest = 0;
		}

		if (current_smallest < smallest && current_smallest > size)
		{
			smallest = current_smallest;
			last_index = i - current_smallest;
		}
	}

	return last_index + 1;
}

int find_first_index(int id)
{
	int i = 0;
	while (i < MEM_SIZE && memory[i] != id)
		i++;
	return i;
}

// Given an ID, find it's last index in memory
int find_last_index(int id)
{
	// printf("looking for id %d\n", id);
	int i = 0;
	int found_id = 0;
	while (i < MEM_SIZE)
	{

		if (found_id == 0 && memory[i] == id)
		{
			// printf("Found first of ID %d at: %d \n", id, i);
			found_id = 1;
		}

		if (found_id && memory[i] != id)
		{
			return i;
		}

		i++;
	}

	return i;
}

int count_number_of_zeroes()
{
	int count = 0;
	for (int i = 0; i < MEM_SIZE; i++)
		if (memory[i] == 0)
			count++;

	return count;
}

int count_contiguous_zeroes_from_index(int index)
{
	int count = 0;
	while (index < MEM_SIZE && memory[index] == 0)
	{
		count++;
		index++;
	}

	return count;
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
		// printf("GOt here 2222... \n");

		// keep incrementing until we get a zero so there's room
		if (memory[index] != 0)
		{
			index++;
		}
		// if we hit a zero
		else
		{
			// replace the zero with an id
			for (int i = 0; i < size; i++)
			{
				memory[index+i] = id;
			}

			return true;
		}
	}

	return false;
}



bool nextFit(int id, int size) {
	int index = 0;
	bool start_inserting = false;
	int blocks_left = size;

	// count up to MEM_SIZE - size, because past that index, the number of blocks
	// requested will not fit.
	for (int i = 0; i < MEM_SIZE; i++)
	{
		if (memory[i] == 0 && blocks_left > 0)
		{
			memory[i] = id;
			blocks_left--;
		}
	}
 
	return true;
}

bool bestFit(int id, int size) {
	int index = find_tightest_space(size);

	// count up to MEM_SIZE - size, because past that index, the number of blocks
	// requested will not fit.
	for (int i = index; i < index+size; i++)
	{
		memory[i] = id;
	}
 
	return false;
}

bool worstFit(int id, int size) {
	int index = find_longest_contiguous_empty_space();

	// count up to MEM_SIZE - size, because past that index, the number of blocks
	// requested will not fit.
	for (int i = index; i < index+size; i++)
	{
		memory[i] = id;
	}
 
	return true;
}

/**
 * The pages policy is special in that it may allocate
 * non-contiguous regions of memory to the process id.
 * However, a full frame must be reserved for each chunk
 * of allocation, even if it is not used by the process.
 */
// bool pages(int id, int size) {
// 	int index = 0;

// 	int frames_left = size / 2;

// 	// count up to MEM_SIZE - size, because past that index, the number of blocks
// 	// requested will not fit.
// 	while (index < (MEM_SIZE - frames_left) && frames_left > 0)
// 	{
// 		// keep incrementing until we get a zero so there's room
// 		if (memory[index] != 0)
// 		{
// 			index += FRAME_SIZE;
// 		}

// 		// if the entire frame is clear
// 		if (memory[index] == 0 && memory[index + 1] == 0)
// 		{
// 			memory[index] = id;
			
// 			if (frames_left > 1)
// 				memory[index+1] = id;
// 			else
// 			{
// 				if (size % 2 == 1)
// 				{
// 					memory[index+1] = 0;
// 				}
// 				else
// 				{
// 					memory[index+1] = id;
// 				}
// 			}

// 			frames_left -= 1;

// 			index += FRAME_SIZE;
// 		}
// 	}

// 	if (frames_left > 0)
// 	{

// 		// if we didn't return true, we need to evict a process
// 		int id_to_evict = find_longest_contiguous_seq();
		
// 		vacateProcess(id_to_evict);
// 		pages(id, frames_left * 2);
// 	}

// 	return false;
// }

bool pages(int id, int size)
{
	int pages_left = size;
	bool leave_empty = false;
	for (int i = 0; i < MEM_SIZE - 1; i += 2)
	{
		if (memory[i] == 0 && memory[i+1] == 0)
		{
			if (pages_left == 0)
			{
				if (leave_empty)
				{	memory[i] = id;
					memory[i+1] = 0;
					leave_empty = false;
				}

				return true;
			}
			if (pages_left > 1)
			{
				memory[i] = id;
				memory[i+1] = id;

				pages_left -= 2;
			}
			if (pages_left == 1)
			{
				printf("got here .. \n");
				memory[i] = id;
				memory[i + 1] = id;
				leave_empty = true;
				pages_left--;
			}
		}
	}

	// if we made it through memory without allocating every page
	// we need to evict a page
	int id_to_evict = find_longest_contiguous_seq();
		
	vacateProcess(id_to_evict);
	
	pages(id, pages_left);

	return true;
}

// loop through memory and determine if it's compacted or not.
// return 0 for not compacted, 1 for is compacted
int is_compact()
{
	int got_to_zero = 0;
	for (int i = 0; i < MEM_SIZE; i++)
	{
		if (memory[i] == 0 && got_to_zero == 0)
		{
			got_to_zero = 1;
		}

		if (memory[i] != 0 && got_to_zero == 1)
		{
			return 0;
		}
	}

	return 1;
}

// Track the number of compaction events
int compactionEvents = 0;

/**
 * Compact memory: This function should never be used in
 * conjunction with the paging policy, but all of the others
 * need it.
 */
void compaction() {
	int hit_zero = 0;
	int count = 0;
	int id_to_move_back = 0;
	for (int i = 1; i < MEM_SIZE; i++)
	{
		if (memory[i] == 0 && hit_zero == 0)
		{
			count = 0;
			hit_zero = 1;
		}

		if (memory[i] == 0 && hit_zero == 1)
		{
			count++;
		}

		if (memory[i] == 0 &&  memory[i - 1] == id_to_move_back)
		{
			hit_zero = 0;
		}

		if (memory[i] != 0)
		{
			if (hit_zero == 1)
			{
				id_to_move_back = memory[i];
				memory[i - count] = id_to_move_back;
			}
		}
	}
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
	if (paging == false)
	{
		if (size < count_number_of_zeroes())
		{
			if (is_compact() != 1)
			{
				printf("Compacting .... \n");

				compaction();
			}
		}
		else
		{
			int id_to_evict = find_longest_contiguous_seq();
			printf("Vacating id == %d .... \n", id_to_evict);
			vacateProcess(id_to_evict);
		}
	}

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
