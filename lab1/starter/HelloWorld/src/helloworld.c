/**
 * @file: helloworld.c
 * @brief: Two simple tasks running pseduo-parallelly
 */

#include <LPC17xx.h>
#include <RTL.h>
#include <stdio.h>
#include <assert.h>
#include "uart_polling.h"
#include "../../RTX_CM3/INC/RTL_ext.h"

#define IS_DEBUG  0
#if IS_DEBUG
#define DEBUG_LVL 1
#else
#define DEBUG_LVL 100
#endif

#define PRINT_LVL 1

__task void task_dummy()
{
	unsigned int i = 0;
	for(;; i++) {
		os_dly_wait(2 * DEBUG_LVL);
	}
}

__task void task6() 
{
	U8 t[440];
	U32 i = 0;
	
	t[0] = 0xAA;
	for(i = 1;i < sizeof(t) / sizeof(U8); i++) {
		t[i] = t[i - 1];
	}
	// Keep the task running forever
	for(;; i++) {
		os_dly_wait(2 * DEBUG_LVL);
	}
}

/*
 * Asserts that there are the correct amount of active tasks.
 */
__task void task_count_v() 
{
	int i;
	int tsks;
	
	for(i = 0; i < 4; i++) {
		os_tsk_create(task_dummy, 1);
	}

	tsks = os_tsk_count_get();

	assert(tsks == 7);

	for (i = 0; i < 4; i++) {
		os_tsk_delete(tsks - i - 1);
	}
	os_dly_wait(PRINT_LVL * DEBUG_LVL);
	printf("passed 1\n");
}

/*
 * Asserts that stack_usage is reporting within the expected range
 */
__task void task_usage()
{
	RL_TASK_INFO info;
	int tsk_cnt = 0xdeadbeef;
	int output = 0xdeadbeef;
	info.stack_usage = 77;
	os_tsk_create(task6, 2);
	tsk_cnt = os_tsk_count_get();
	output = os_tsk_get(tsk_cnt - 1, &info);
	assert(!output);
	// Should be 98-99 but to be safe do 97-100
	assert(info.stack_usage >= 97);
	assert(info.stack_usage <= 100);
	os_dly_wait(PRINT_LVL * DEBUG_LVL);
	printf("passed 2\n");
}

/*
 * Try to get a task with invalid id.
 */
__task void task_get_invalid()
{
	OS_TID i = 500;
	RL_TASK_INFO info;
	int output = os_tsk_get(i, &info);
	assert(output == 255);
	os_dly_wait(PRINT_LVL * DEBUG_LVL);
	printf("passed 3\n");
}

/*
 * Try to get task when providing null info.
 */
__task void task_get_give_null()
{
	assert(os_tsk_get(255, NULL));
	os_dly_wait(PRINT_LVL * DEBUG_LVL);
	printf("passed 4\n");
}

/*
 * Prints out the usage.
 */
__task void task_get_all()
{
	int i;
	RL_TASK_INFO info;
	for(;;)
	{
		os_dly_wait(3 * PRINT_LVL * DEBUG_LVL);
		for(i = 1; i <= 6; i++)
		{
			info.state = 77;
			if (!os_tsk_get(i, &info)) {
			  printf("Task %d status: %d \n", i, info.state);
			} else {
				printf("Task %d is NULL\n", i);
			}
		}
		info.state = 77;
		if (!os_tsk_get(0xFF, &info)) {
			printf("Idle task status: %d \n", info.state);
		} else {
			printf("Idle task is NULL\n");
		}
	}
}

__task void init(void)
{
  os_tsk_create(task_count_v, 1);
	os_tsk_create(task_usage, 1);
	os_tsk_create(task_get_invalid, 1);
	os_tsk_create(task_get_give_null, 1);
	os_tsk_create(task_get_all, 1);

	os_tsk_delete_self(); // must delete itself before exiting
}

int main ()
{
	SystemInit();
	uart0_init();
	os_sys_init(init);
}
