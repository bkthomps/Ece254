/**
 * @file memory.c
 * @brief: ECE254 Lab: memory allocation algorithm comparison template file
 * @author: 
 * @date: 2015-11-20
 */

/* includes */
#include <stdio.h> 
#include <stdlib.h> 
#include <stdint.h>
#include <memory.h>
#include "mem.h"

/* defines */

#define SMALLEST_SIZE	4
#define FREE		0
#define USED		1

/* structures */

struct node {
	struct node *prev_node;
	struct node *next_node;
	size_t data_size;
	void *data_start;
	uint8_t state;
};

/* global variables */

void *best_fit_memory;
void *worst_fit_memory;

/* Functions */

static inline size_t get_aligned(size_t size)
{
	return size % SMALLEST_SIZE == 0
		? size : size + SMALLEST_SIZE - size % SMALLEST_SIZE;
}

/* memory initializer */

int best_fit_memory_init(size_t size)
{
	if (size < sizeof(struct node)) {
		return -1;
	}
	best_fit_memory = malloc(size);
	if (!best_fit_memory) {
		return -1;
	}
	struct node *head = best_fit_memory;
	head->prev_node = NULL;
	head->next_node = NULL;
	head->data_size = size - sizeof(struct node);
	head->data_start = best_fit_memory + sizeof(struct node);
	head->state = FREE;
	return 0;
}

int worst_fit_memory_init(size_t size)
{
	if (size < sizeof(struct node)) {
		return -1;
	}
	worst_fit_memory = malloc(size);
	if (!worst_fit_memory) {
		return -1;
	}
	struct node *head = worst_fit_memory;
	head->prev_node = NULL;
	head->next_node = NULL;
	head->data_size = size - sizeof(struct node);
	head->data_start = worst_fit_memory + sizeof(struct node);
	head->state = FREE;
	return 0;

}

/* memory allocators */

void *best_fit_alloc(size_t old_size)
{
	if (!old_size) {
		return NULL;
	}
	const size_t aligned_size = get_aligned(old_size);
	struct node *traverse = best_fit_memory;
	struct node *closest = NULL;
	size_t cur_size = SIZE_MAX;
	while (traverse) {
		if (traverse->state == FREE
				&& traverse->data_size >= aligned_size
				&& traverse->data_size < cur_size) {
			closest = traverse;
			cur_size = traverse->data_size;
		}
		traverse = traverse->next_node;
	}
	if (!closest) {
		return NULL;
	}
	if (closest->data_size == aligned_size) {
		closest->state = USED;
		return closest->data_start;
	}
	const int remaining_size = closest->data_size - aligned_size - sizeof(struct node);
	if (remaining_size < 0) {
		closest->state = USED;
		closest->data_size += closest->data_size - aligned_size;
	} else {
		// new block
		struct node *insert = (void *) closest->data_start + aligned_size;
		insert->prev_node = closest;
		insert->next_node = closest->next_node;
		insert->data_size = remaining_size;
		
		insert->data_start = insert + 1;
		insert->state = FREE;
		// old block
		closest->state = USED;
		closest->data_size = aligned_size;
		closest->next_node = insert;
	}
	return closest->data_start;
}

void *worst_fit_alloc(size_t old_size)
{
	if (!old_size) {
		return NULL;
	}
	const size_t aligned_size = get_aligned(old_size);
	struct node *traverse = worst_fit_memory;
	struct node *biggest = NULL;
	size_t cur_size = aligned_size;
	while (traverse) {
		if (traverse->state == FREE && traverse->data_size > cur_size) {
			biggest = traverse;
			cur_size = traverse->data_size;
		}
		traverse = traverse->next_node;
	}
	if (!biggest) {
		return NULL;
	}
	if (biggest->data_size == aligned_size) {
		biggest->state = USED;
		return biggest->data_start;
	}
	const int remaining_size = biggest->data_size - aligned_size - sizeof(struct node);
	if (remaining_size < 0) {
		biggest->state = USED;
		biggest->data_size += biggest->data_size - aligned_size;
	} else {
		// new block
		struct node *insert = (void *) biggest->data_start + aligned_size;
		insert->prev_node = biggest;
		insert->next_node = biggest->next_node;
		insert->data_size = biggest->data_size - aligned_size - sizeof(struct node);
		insert->data_start = insert + 1;
		insert->state = FREE;
		// old block
		biggest->state = USED;
		biggest->data_size = aligned_size;
		biggest->next_node = insert;
	}
	return biggest->data_start;
}

/* memory de-allocator */

void best_fit_dealloc(void *ptr) 
{
	// make sure the given address is 4 byte aligned
	if ((ptr - best_fit_memory) % SMALLEST_SIZE != 0) {
		return;
	}
	struct node *control = (struct node *) ptr - 1;
	control->state = FREE;

	// coalescence
	if (control->next_node && control->next_node->state == FREE) {
		struct node *control_next = control->next_node;
		if (control_next->next_node) {
			control_next->next_node->prev_node = control;
		}
		control->next_node = control_next->next_node;
		control->data_size += sizeof(struct node) + control_next->data_size;
	} else {

	}
	if (control->prev_node && control->prev_node->state == FREE) {
		struct node *control_prev = control->prev_node;
		if (control->next_node) {
			control->next_node->prev_node = control_prev;
		}
		control_prev->next_node = control->next_node;
		control_prev->data_size += sizeof(struct node) + control->data_size;
	}
}

void worst_fit_dealloc(void *ptr) 
{
	// make sure the given address is 4 byte aligned
	if ((ptr - worst_fit_memory) % SMALLEST_SIZE != 0) {
		return;
	}
	struct node *control = (struct node *) ptr - 1;
	control->state = FREE;
	// coalescence
	if (control->next_node && control->next_node->state == FREE) {
		struct node *control_next = control->next_node;
		if (control_next->next_node) {
			control_next->next_node->prev_node = control;
		}
		control->next_node = control_next->next_node;
		control->data_size += sizeof(struct node) + control_next->data_size;
	}

	if (control->prev_node && control->prev_node->state == FREE) {
		struct node *control_prev = control->prev_node;
		if (control->next_node) {
			control->next_node->prev_node = control_prev;
		}
		control_prev->next_node = control->next_node;
		control_prev->data_size += sizeof(struct node) + control->data_size;
	}
}

/* memory algorithm metric utility function(s) */
/* count how many free blocks are less than the input size */ 

int best_fit_count_extfrag(size_t size)
{
	int count = 0;
	struct node *traverse = best_fit_memory;
	while (traverse) {
		if (traverse->state == FREE && traverse->data_size < size) {
			count++;
		}
		traverse = traverse->next_node;
	}
	return count;
}

int worst_fit_count_extfrag(size_t size)
{
	int count = 0;
	struct node *traverse = worst_fit_memory;
	while (traverse) {
		if (traverse->state == FREE && traverse->data_size < size) {
			count++;
		}
		traverse = traverse->next_node;
	}
	return count;
}
