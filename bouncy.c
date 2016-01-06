////////////////////////////////////////////////////////////////////////////////
///																			 ///
///				 Trie-like Implementation of Bouncy Numbers					 ///
///							(Project Euler, #112)							 ///
///							 by: Kshitijh Meelu								 ///
///																			 ///
///		Description: An increasing number is one where, working left-to-	 ///
///			right, no digit is exceeded by the digit to its left, such as 	 ///
///			134468. Similarly, a decreasing number is one where no digit 	 ///
///			is exceeded by the digit to its right, such as 664210.			 ///
///																			 ///
///			If a number is neither increasing nor decreasing, we call it 	 ///
///			a bouncy number. The first bouncy number is 101. Below 1000,	 ///
///			there are 525 bouncy numbers (over half!). The least number 	 ///
///			for which the proportion of bouncy numbers first reaches 50%	 ///
///			is 538. 														 ///
///																			 ///
///		Goal: Find the least number for which the proportion of bouncy 		 ///
///			numbers is exactly 99%.											 ///
///																			 ///
///		Solution: Below is my code to answer this question. My solution 	 ///
///			incorporates a trie-like data structure. The root acts just 	 ///
///			as a root. But after that, each edge you walk down adds a 		 ///
///			digit to the right-hand side of a number. A simple example is 	 ///
///			given here. Although incomplete, we hope this explains the  	 ///
///			concepts involved. 												 ///
///																			 ///
///			                       root										 ///
///			                        +										 ///
///			            +--+--+--+-----+--+--+--+--+						 ///
///			            |  |  |  |  |  |  |  |  |  |						 ///
///			            +  +  +  +  +  +  +  +  +  +						 ///
///			            0  1  2  3  4  5  6  7  8  9						 ///
///			            +               	       +						 ///
///			+--+--+--+-----+--+--+--+--+	       |						 ///
///			|  |  |  |  |  |  |  |  |  |           |						 ///
///			+  +  +  +  +  +  +  +  +  +           |						 ///
///			00 01 02 03 04 05 06 07 08 09          |						 ///
///			                                       |						 ///
///			                                       |						 ///
///			                           +--+--+--+-----+--+--+--+--+			 ///
///			                           |  |  |  |  |  |  |  |  |  |			 ///
///			                           +  +  +  +  +  +  +  +  +  +			 ///
///			                           80 81 82 83 84 85 86 87 88 89		 ///
///																			 ///
///			At each level, we count the bouncy digits, and once the given	 ///
///			proportion is found, we exit. This trie implementation was 		 ///
///			because it's marginally faster than computing whether each		 ///
///			number is bouncy in a linear fashion. For n numbers with m 		 ///
///			as the number of digits in m, the linear implementation is 		 ///
///			roughly O(mn), while this implementation is O(n), which is 		 ///
///			slightly faster. This was also made as practice in C 			 ///
///			data structures.												 ///
///																			 ///
///		Answer: The answer to the questions is that 1587000 is the least	 ///
///			number for which the proportion of bouncy numbers is exactly	 ///
///			99%.															 ///
///																			 ///
////////////////////////////////////////////////////////////////////////////////


// Include Statements
#include "stdlib.h"
#include "stdio.h"
#include "math.h"

/* 	Refers to the proportion of integers that are bouncy out of all integers up 
	to a specific integer. Remember, in this problem, we search for the integer
	where this proportion is first found	*/
# define PERCENTAGE		.99

// The structure for each node in the trie. 
typedef struct Integer{

	// number that the trie node represents.
	int val;

	// last digit provided (this helps in figuring out if it's bouncy)
	int digit;

	// 0 if not increasing number; 1 if it is
	int isIncreasing;

	// 0 if not decreasing number; 1 if it is
	int isDecreasing;

	// The 10 children are the 10 numbers that stem from this (by adding digits
	// 0-9 to the end of this node)
	struct Integer *children[10];

	// points to the next node in sequence (at the same level), e.g. 522 -> 523
	struct Integer *next;

} *integer;

// The full trie for all integers
typedef struct IntegerDict{

	// the root is a placeholder to keep all the nodes together. It has no 
	// intuitive meaning
	integer root;

	// pointer to the '0' value at the last level constructed
	integer tail;

	// count of bouncy numbers found on the current level
	int bouncyCount;

	// first number with a proportion of bouncy numbers equal to PERCENTAGE
	int firstOver;

	// The level currently on. e.g. 0-9 (level 1), 00-99 (level 2), 000-999
	// (level 3), 0000-9999 (level 4), etc.
	int level;

} *integerDict;

// initializes an integerDict with level 1.
integerDict init (integerDict Z) {

	// allocate space
	Z = (integerDict) malloc(sizeof(struct IntegerDict));
	Z->bouncyCount = 0;
	Z->firstOver = -1;

	// assign values for the root node
	Z->root = (integer) malloc(sizeof(struct Integer));
	Z->root->val = -1;
	Z->root->digit = -1;
	Z->root->isIncreasing = 1;
	Z->root->isDecreasing = 1;
	Z->root->next = NULL;

	// loop through 10 times to allocate memory and assign values for the 10
	// children
	integer temp = NULL;
	for(int i = 9; i >= 0; i--){
		Z->root->children[i] = (integer) malloc(sizeof(struct Integer));
		Z->root->children[i]->val = i;
		Z->root->children[i]->digit = i;
		Z->root->children[i]->isIncreasing = 1;
		Z->root->children[i]->isDecreasing = 1;
		Z->root->children[i]->next = temp;

		temp = Z->root->children[i];
	}

	// assign the level and tail
	Z->level = 1;
	Z->tail = Z->root->children[0];
	
	// NOTE: we return the Z as a work around. For some reason, a void function
	// was not working
	return Z;
}

// Adds another level and thus, digits, to the integerDict. In its current 
// implementation, it will recurse and keep adding digits until Z->firstOver
// is assigned.
integerDict addDigits (integerDict Z) {

	// Assign traversal pointers, curr and temp
	integer curr = Z->tail;
	integer temp = NULL;

	// indicate the addition of another level to the integerDict
	Z->level++;

	// Goes through the current leaves and populates their children.
	while ( curr != NULL ){

		// The 10 children are each assigned here
		for(int i = 0; i < 10; i++){

			// basic memory allocation and assignment
			curr->children[i] = (integer) malloc(sizeof(struct Integer));
			curr->children[i]->digit = i;

			// multiply parent's val by 10 and add right-most digit
			curr->children[i]->val = (int)(i + 10 * curr->val);

			// initialize to not increasing and not decreasing
			curr->children[i]->isIncreasing = 0;
			curr->children[i]->isDecreasing = 0;

			// check if Decreasing
			if ( i <= curr->digit && curr->isDecreasing ){
				curr->children[i]->isDecreasing = 1;
			}

			// check if Increasing
			if ( i >= curr->digit && curr->isIncreasing ){
				curr->children[i]->isIncreasing = 1;
			}

			// Update the bouncy count if this number if bouncy
			if ( !curr->children[i]->isDecreasing && \
				!curr->children[i]->isIncreasing && \
				curr->children[i]->val > pow(10,Z->level-1)){

				// update bouncy count
				Z->bouncyCount++;

				// Check if this is the first bouncy number over the proportion
				if ( Z->firstOver == -1 && \
					Z->bouncyCount >= curr->children[i]->val * PERCENTAGE ){
					
					// Assign it
					Z->firstOver = curr->children[i]->val;

					// return Z
					return Z;
				}
			}

			// Assigns the next pointer for the previous node visited
			if ( temp ){
				temp->next = curr->children[i];
			}
			temp = curr->children[i];

		}

		// go to next used-to-be leaf
		curr = curr->next;
	}

	// Assign the new tail
	Z->tail = Z->tail->children[0];

	// Recurse deeper
	return addDigits(Z);
}

int main(int argc, char const *argv[])
{
	/* 	Create and initialize the integer dictionary, Z (the mathematical
	 	symbol for integers)	*/
	integerDict Z;
	Z = init(Z);

	/* 	Adds a digit--that is, another level to the trie--to our integer 
		dictionary	*/
	Z = addDigits(Z);

	// print out answer
	printf("The proportion needed is: %f\n", PERCENTAGE);
	printf("The first number to meet that proportion is:%i\n", Z->firstOver);

	return 0;
}



// by: Kshitijh Meelu
// E: kshitijh.meelu@yale.edu
