/**
  * ECE254 Linux Dynamic Memory Management Lab
  * @file: main_test.c
  * @brief: The main file to write tests of memory allocation algorithms
  */ 

/* includes */
/* system provided header files. You may add more */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

/* non-system provided header files. 
   Do not include more user-defined header files here
 */
#include "mem.h"

struct node {
	struct node *prev_node;
	struct node *next_node;
	size_t data_size;
	void *data_start;
	uint8_t state;
};

void test_best_fit()
{
	int rc = best_fit_memory_init(0);
	assert(rc == -1);
	rc = best_fit_memory_init(sizeof(struct node) - 1);
	assert(rc == -1);
	rc = best_fit_memory_init(sizeof(struct node));
	assert(rc == 0);
	rc = best_fit_memory_init(440);
	assert(best_fit_count_extfrag(440) == 1);
	// test with two
	void *a = best_fit_alloc(0);
	assert(!a);
	a = best_fit_alloc(10);
	assert(a);
	assert(best_fit_count_extfrag(440) == 1);
	void *b = best_fit_alloc(15);
	assert(best_fit_count_extfrag(440) == 1);
	best_fit_dealloc(a);
	assert(best_fit_count_extfrag(440) == 2);
	best_fit_dealloc(b);
	assert(best_fit_count_extfrag(440) == 1);
	// test with three
	assert(best_fit_count_extfrag(440) == 1);
	a = best_fit_alloc(10);
	assert(best_fit_count_extfrag(440) == 1);
	b = best_fit_alloc(15);
	void *c = best_fit_alloc(10);
	assert(best_fit_count_extfrag(440) == 1);
	best_fit_dealloc(a);
	assert(best_fit_count_extfrag(440) == 2);
	best_fit_dealloc(c);
	assert(best_fit_count_extfrag(440) == 2);
	best_fit_dealloc(b);
	assert(best_fit_count_extfrag(440) == 1);
	// test with three coallescing
	a = best_fit_alloc(10);
	b = best_fit_alloc(15);
	c = best_fit_alloc(10);
	assert(best_fit_count_extfrag(440) == 1);
	best_fit_dealloc(a);
	assert(best_fit_count_extfrag(440) == 2);
	best_fit_dealloc(b);
	assert(best_fit_count_extfrag(440) == 2);
	best_fit_dealloc(c);
	assert(best_fit_count_extfrag(440) == 1);

	// check it is inserting in the best fit place
	a = best_fit_alloc(16);
	b = best_fit_alloc(4);
	c = best_fit_alloc(16);

	assert(best_fit_count_extfrag(440) == 1);
	best_fit_dealloc(b);
	assert(best_fit_count_extfrag(440)== 2);
	b = best_fit_alloc(4);
	assert(best_fit_count_extfrag(440)==1); 
}

void test_worst_fit()
{
	int rc = worst_fit_memory_init(0);
	assert(rc == -1);
	rc = worst_fit_memory_init(sizeof(struct node) - 1);
	assert(rc == -1);
	rc = worst_fit_memory_init(sizeof(struct node));
	assert(rc == 0);
	rc = worst_fit_memory_init(440);
	assert(worst_fit_count_extfrag(440) == 1);

	//test with 2
	void *a = worst_fit_alloc(0);
	assert(!a);
	a = worst_fit_alloc(10);
	assert(a);
	assert(worst_fit_count_extfrag(440) == 1);
	void *b = worst_fit_alloc(15);
	assert(worst_fit_count_extfrag(440) == 1);
	worst_fit_dealloc(a);
	assert(worst_fit_count_extfrag(440) == 2);
	worst_fit_dealloc(b);
	assert(worst_fit_count_extfrag(440) == 1);

	// test with three
	assert(worst_fit_count_extfrag(440) == 1);
	a = worst_fit_alloc(10);
	assert(worst_fit_count_extfrag(440) == 1);
	b = worst_fit_alloc(15);
	void *c = worst_fit_alloc(10);
	assert(worst_fit_count_extfrag(440) == 1);
	worst_fit_dealloc(a);
	assert(worst_fit_count_extfrag(440) == 2);
	worst_fit_dealloc(c);
	assert(worst_fit_count_extfrag(440) == 2);
	worst_fit_dealloc(b);
	assert(worst_fit_count_extfrag(440) == 1);

	// test with three coallescing
	a = worst_fit_alloc(10);
	b = worst_fit_alloc(15);
	c = worst_fit_alloc(10);
	assert(worst_fit_count_extfrag(440) == 1);
	worst_fit_dealloc(a);
	assert(worst_fit_count_extfrag(440) == 2);
	worst_fit_dealloc(b);
	assert(worst_fit_count_extfrag(440) == 2);
	worst_fit_dealloc(c);
	assert(worst_fit_count_extfrag(440) == 1);

	// check it is putting in the worst fit place
	a = worst_fit_alloc(16);
	b = worst_fit_alloc(4);
	c = worst_fit_alloc(16);

	assert(worst_fit_count_extfrag(440) == 1);
	worst_fit_dealloc(b);
	assert(worst_fit_count_extfrag(440)== 2);
	b = worst_fit_alloc(4);
	assert(worst_fit_count_extfrag(440)==2); 
}

void test_best_fit_better_fragmentation() {
	best_fit_memory_init(1024);
	worst_fit_memory_init(1024);
	void *a = best_fit_alloc(4);
	void *b = best_fit_alloc(4);
	void *c = best_fit_alloc(4);
	void *d = best_fit_alloc(4);

	best_fit_dealloc(a);
	best_fit_dealloc(c);

	a = best_fit_alloc(4);
	c = best_fit_alloc(4);

	best_fit_dealloc(a);

	a = best_fit_alloc(4);

	printf("Best fit external fragmentation: %d \n", best_fit_count_extfrag(1024));

	best_fit_dealloc(a);
	best_fit_dealloc(b);
	best_fit_dealloc(c);
	best_fit_dealloc(d);


	a = worst_fit_alloc(4);
	b = worst_fit_alloc(4);
	c = worst_fit_alloc(4);
	d = worst_fit_alloc(4);

	worst_fit_dealloc(a);
	worst_fit_dealloc(c);

	a = worst_fit_alloc(4);
	c = worst_fit_alloc(4);

	worst_fit_dealloc(a);

	a = worst_fit_alloc(4);

	printf("Worst fit external fragmentation: %d \n", worst_fit_count_extfrag(1024));
}

void test_worst_fit_better_fragmentation() {
	best_fit_memory_init(1024);
	worst_fit_memory_init(1024);

	void *a = best_fit_alloc(32);
	void *b = best_fit_alloc(32);
	void *c = best_fit_alloc(32);
	void *d = best_fit_alloc(32);
	best_fit_dealloc(b);
	best_fit_dealloc(c);
	void *e = best_fit_alloc(32);
	best_fit_dealloc(a);

	printf("Best fit external fragmentation: %d \n", best_fit_count_extfrag(1024));

	a = worst_fit_alloc(32);
	b = worst_fit_alloc(32);
	c = worst_fit_alloc(32);
	d = worst_fit_alloc(32);
	worst_fit_dealloc(b);
	worst_fit_dealloc(c);
	e = worst_fit_alloc(32);
	worst_fit_dealloc(a);

	printf("Worst fit external fragmentation: %d \n", worst_fit_count_extfrag(1024));
}

int main(int argc, char *argv[])
{
	test_best_fit();
	test_worst_fit();
	test_best_fit_better_fragmentation();
	test_worst_fit_better_fragmentation();
	return 0;
}
