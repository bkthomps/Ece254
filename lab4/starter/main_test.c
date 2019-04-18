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
}

void test_worst_fit()
{

}

int main(int argc, char *argv[])
{
	test_best_fit();
	test_worst_fit();
	return 0;
}
