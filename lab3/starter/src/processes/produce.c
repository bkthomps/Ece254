#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <math.h>
#include <errno.h>

#define EPS			1.E-7
#define TEN_TO_THE_SIX_DECIMAL	1000000.0
#define QUEUE_NAME		"/processes_queue_ece254_fall_2018_group32_3"
#define MAX_MSG_SIZE		256

void busy_loop(int iters)
{
	volatile int sink;
  	do {
    		sink = 0;
  	} while (--iters > 0);
  	(void) sink;
}

static void producer(mqd_t queue,
		     const int id,
		     const int num_items,
		     const int num_producers)
{
	int i;
	// Put items into our message queue
	for (i = id; i < num_items; i += num_producers) {
		busy_loop(40000);
		if (mq_send(queue, (char *) &i, sizeof(int), 0) < 0) {
			printf("Message send failed\n");
			exit(1);
		}
	}
}

static void consumer(mqd_t queue,
		     const int cid,
		     const int buffer_size,
		     const int num_items,
		     const int num_consumers)
{
	int i;
	// Remove number of items from the queue corresponding to the cid
	for (i = cid; i < num_items; i += num_consumers) {
		int num;
		if (mq_receive(queue, (char *) &num, sizeof(int), 0) < 0) {
			printf("Message send failed\n");
			exit(1);
		}

		busy_loop(50000);

		// Check if the square root is an integer
		const double val = sqrt(num);
		if (fabs(val - round(val)) < EPS) {
			printf("%d %d %d\n", cid, num, (int) round(val));
		}

	}
}

int main(int argc, char **argv)
{
	if (argc != 5) {
		printf("Usage: %s <N> <B> <P> <C>\n", argv[0]);
		exit(1);
	}
	const int num_items	= atoi(argv[1]);
	const int buffer_size	= atoi(argv[2]);
	const int num_producers	= atoi(argv[3]);
	const int num_consumers	= atoi(argv[4]);

	struct timeval tv;
	gettimeofday(&tv, NULL);
	const double start_time_in_seconds
		= (tv.tv_sec) + tv.tv_usec / TEN_TO_THE_SIX_DECIMAL;

	int p_cnt = 0;
	int c_cnt = 0;
	// keep track of the children pids
	int child_pid[num_producers + num_consumers];
	int pid;
	int i;

	// Create Queue
	struct mq_attr attr;
	attr.mq_flags	= 0;
	attr.mq_maxmsg	= buffer_size;
	attr.mq_msgsize	= sizeof(int);
	attr.mq_curmsgs	= 0;
	mqd_t queue = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0644, &attr);
	if (queue < 0) {
		printf("Queue open failed\n");
		exit(1);
	}

	// parent process will fork children
fork_process:
	pid = fork();
	if (pid < 0) {
		printf("Fork failed\n");
		exit(1);
	} else if (pid) {	/* parent */
		if (c_cnt < num_consumers) {
			// while we're forking consumers
			// save the pid of our children
			child_pid[c_cnt] = pid;
			c_cnt++;
			goto fork_process;
		}
		if (p_cnt < num_producers - 1) {
			// while we're forking producers
			// save the pid of our children
			child_pid[p_cnt + c_cnt] = pid;
			p_cnt++;
			goto fork_process;
		}
	} else {	/* child */
		if (c_cnt < num_consumers) {
			// produce consumers up to a certain amount
			consumer(queue, c_cnt, buffer_size, num_items, num_consumers);
		} else  {
			// produce producers up to a certain amount
			producer(queue, p_cnt, num_items, num_producers);
		}
		// Close queue
		if (mq_close(queue) < 0) {
			printf("Queue close failed\n");
			exit(1);
		}
		exit(0);
	}

	// wait for all the children processes to complete
	for (i = 0; i < num_producers + num_consumers; i++) {
		waitpid(child_pid[i], NULL, 0);
	}

	// Close queue
	if (mq_close(queue) < 0) {
		printf("Queue close failed\n");
		exit(1);
	}
	if (mq_unlink(QUEUE_NAME) < 0) {
		printf("Unlink failed\n");
		exit(1);
	}

	gettimeofday(&tv, NULL);
	const double end_time_in_seconds
		= (tv.tv_sec) + tv.tv_usec / TEN_TO_THE_SIX_DECIMAL;
	printf("System execution time: %.6lf seconds\n",
	       end_time_in_seconds - start_time_in_seconds);
	exit(0);
}
