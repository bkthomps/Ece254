#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <math.h>

#define EPS			1.E-7
#define TEN_TO_THE_SIX_DECIMAL	1000000.0


// Global Variables
static int num_items;
static int buffer_size;
static int num_producers;
static int num_consumers;

static sem_t producer_sem;
static sem_t consumer_sem;
static pthread_mutex_t mutex;
static int *buffer;
static int head_index = 1;
static int tail_index = 0;

void busy_loop(int iters)
{
	volatile int sink;
  	do {
    		sink = 0;
  	} while (--iters > 0);
  	(void) sink;
}


static void *producer(void *const param)
{
	// Get the process id
	const int id = *(int *) param;
	int i;

	// Generate values to put in our buffer
	for (i = id; i < num_items; i += num_producers) {
		busy_loop(40000);
		// decrement to indicate less space
		sem_wait(&producer_sem);
		// lock the mutex to ensure mutual exclusion
		pthread_mutex_lock(&mutex);
		// adding the item to the end of the queue
		tail_index = (tail_index + 1) % buffer_size;
		// update our tail index to indicate it's now occupied
		buffer[tail_index] = i;
		pthread_mutex_unlock(&mutex);
		// increment to indicate item is put in buffer
		sem_post(&consumer_sem);
	}

	// After generating all possible values finish
	pthread_exit(param);
}

static void *consumer(void *const param)
{
	// Get the consumer id
	const int cid = *(int *) param;
	int i;
	
	// Remove a certain number of items from buffer corresp to cid
	for (i = cid; i < num_items; i += num_consumers) {
		// decrement to indicate less items in buffer
		sem_wait(&consumer_sem);
		// lock the mutex to ensure mutual exclusion
		pthread_mutex_lock(&mutex);
		// grab the head of the queue
		const int num = buffer[head_index];
		// remove it from the queue by updating our head
		head_index = (head_index + 1) % buffer_size;
		pthread_mutex_unlock(&mutex);
		// check if the removed number is a square
		busy_loop(50000);
		
		const double val = sqrt(num);
		if (fabs(val - round(val)) < EPS) {
			printf("%d %d %d\n", cid, num, (int) round(val));
		}
		// increment to indicate we have a space in the buffer
		sem_post(&producer_sem);
	}
	// After removing all our allowed values finish
	pthread_exit(param);

}

int main(int argc, char **argv)
{
	if (argc != 5) {
		printf("Usage: %s <N> <B> <P> <C>\n", argv[0]);
		exit(1);
	}
	num_items	= atoi(argv[1]);
	buffer_size	= atoi(argv[2]);
	num_producers	= atoi(argv[3]);
	num_consumers	= atoi(argv[4]);

	struct timeval tv;
	gettimeofday(&tv, NULL);
	const double start_time_in_seconds
		= (tv.tv_sec) + tv.tv_usec / TEN_TO_THE_SIX_DECIMAL;
	
	// keep track of the threads we are creating
	pthread_t producers[num_producers];
	pthread_t consumers[num_consumers];
	int i;
	buffer = malloc(buffer_size * sizeof(int));

	// initialize our semaphores
	pthread_mutex_init(&mutex, NULL);
	// producer semaphore indicates number of spaces in buffer
	sem_init(&producer_sem, 0, buffer_size);
	// consumer semaphore indicates number of items in buffer
	sem_init(&consumer_sem, 0, 0);

	// creating the threads
	for (i = 0; i < num_producers; i++) {
		int *id = malloc(sizeof(int));
		*id = i;
		pthread_create(&producers[i], NULL, producer, id);
	}
	for (i = 0; i < num_consumers; i++) {
		int *cid = malloc(sizeof(int));
		*cid = i;
		pthread_create(&consumers[i], NULL, consumer, cid);
	}

	// joining the threads
	for (i = 0; i < num_producers; i++) {
		void *ret;
        	pthread_join(producers[i], &ret);
        	free((int *) ret);
	}
	for (i = 0; i < num_consumers; i++) {
		void *ret;
		pthread_join(consumers[i], &ret);
		free((int *) ret);
	}

	free(buffer);

	gettimeofday(&tv, NULL);
	const double end_time_in_seconds
		= (tv.tv_sec) + tv.tv_usec / TEN_TO_THE_SIX_DECIMAL;
	printf("System execution time: %.6lf seconds\n", \
	       end_time_in_seconds - start_time_in_seconds);
	exit(0);
}
