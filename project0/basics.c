// Last modified: 1/25/2018

#include <stdio.h> // Standard IO using printf

// Example constant: Maximum allowed random integer
#define MAX_RAND 100
// Example constant: Maximum factorial to print
#define MAX_FACT 10

/**
 * Part 1
 *
 * Calculates and returns the sum of numbers from
 * 1 up to and including n, using a for loop.
 *
 * param: int n: last value in sum
 * return: 1 + ... + n
 **/
int sum1ToN(int n) {
	// TODO
	return (n*(n+1))/2; // replace
}

/**
 * Part 2
 *
 * Calculates and returns the sum of numbers 
 * in the array.
 *
 * param: int a[]: array of ints
 * param: int size: number of elements in a
 * return: a[0] + ... + a[size - 1]
 **/
int arraySum(int a[], int size) {
	// TODO
	int sum = 0;
	for (int i = 0; i < size; i++)
		sum += a[i];
	return sum; // replace
}

/**
 * Part 3
 *
 * Recursive implementation of factorial function.
 * factorial(n) is typically denoted n! in mathematics.
 * The recursive definition of factorial is as follows:
 *
 * 0! = 1
 * n! = n*(n-1)!
 *
 * param: int n: Integer to get the factorial of. Cannot be negative.
 * return: n!
 **/
int factorial(int n) {
	// TODO
	if (n == 0)
		return 1;
	else
		return n * factorial(n-1);
}

/**
 * Helper function used for Part 4
 *
 * Indicate success or failure of recursiveBinarySearch with console output.
 *
 * param: int key: value that was searched for
 * param: int index: index in array where value was found, 
 *                   or -1 if the value was not found.
 *
 * No return
 **/
void found(int key, int index) {
	if(index == -1) { 
		printf("Value %d not found\n", key);
	} else {
		printf("Value %d found at index %d\n",key,index);
	}
}

/**
 * Part 4
 *
 * Given a sorted array, recursively search the array
 * for a specified key value. You should be able to
 * easily find this function online, but make sure you
 * understand the solution that you implement. Provide
 * a URL/Link or other reference to the source of the 
 * code you base your code on.
 *
 * URL: <https://en.wikipedia.org/wiki/Binary_search_algorithm#Procedure>
 *
 * param: int a[]: sorted array (ascending)
 * param: int key: value to search for in array
 * param: int iMin: lowest index in a that is still under consideration
 * param: int iMax: highest index in a that is still under consideration
 * return: if key is found, return the index, else return -1
 **/
int recursiveBinarySearch(int a[], int key, int iMin, int iMax) {
	// TODO
	if (iMin <= iMax)
	{
		int middle = (iMin + iMax) / 2;
		if (a[middle] < key)
			return recursiveBinarySearch(a, key, middle + 1, iMax);
		else if (a[middle] > key)
			return recursiveBinarySearch(a, key, iMin, middle - 1);
		else
			return middle;
	}
	return -1; // did not find key
}

/**
 * Part 5
 * 
 * Receives a pointer to a variable and adds another int
 * to the value pointed to.
 *
 * param: int *toChange: memory reference (pointer) to an int
 * param: int op: value to add to the value pointed to by toChange
 *
 * No return
 **/
void add(int *toChange, int op) { // The * indicates a pointer variable
	// TODO
	*toChange += op;
}

/**
 * This main function calls several simple functions
 * that you must define above. Besides calling the methods
 * you must define, it also has examples of how to do
 * basic printing to the console using printf.
 **/
void main() {
	int i; // loop variable

	// Part 1
	printf("\nPart 1\n");
	int n = 100;
	printf("Sum from 1 to %d = %d\n", n, sum1ToN(n));

	// Part 2
	printf("\nPart 2\n");
	const int sizeA = 100;
	int a[sizeA]; // Notice the different syntax for array initialization
	for(i = 0; i < sizeA; i++) {
		a[i] = i+1;
	} 
	printf("Sum across array = %d\n", arraySum(a,sizeA)); // same sum
	const int sizeB = 50;
	int b[sizeB]; 
	srand(time(NULL)); // give seed to random number generator
	for(i = 0; i < sizeB; i++) {
		b[i] = rand() % MAX_RAND;
	} 
	printf("Sum across array = %d\n", arraySum(b,sizeB)); 

	// Part 3
	printf("\nPart 3\n");
	for(i = 0; i <= MAX_FACT; i++) {
		printf("%d! = %d\n", i, factorial(i));
	}

	// Part 4
	printf("\nPart 4\n");
	int key1 = 20;
	int index = recursiveBinarySearch(a, key1, 0, sizeA - 1);
	found(key1,index);
	int key2 = 100;
	index = recursiveBinarySearch(a, key2, 0, sizeA - 1);
	found(key2,index);
	int key3 = 1;
	index = recursiveBinarySearch(a, key3, 0, sizeA - 1);
	found(key3,index);
	int key4 = 500;
	index = recursiveBinarySearch(a, key4, 0, sizeA - 1);
	found(key4,index);

	// Part 5
	printf("\nPart 5\n");
	int x = 20;
	int y = 15;
	printf("%d + %d = ",x,y);
	add(&x, y); // &x is a reference/pointer to the variable x
	printf("%d\n",x);
	x = 100;
	y = 13;
	printf("%d + %d = ",x,y);
	add(&x, y); // &x is a reference/pointer to the variable x
	printf("%d\n",x);
}

