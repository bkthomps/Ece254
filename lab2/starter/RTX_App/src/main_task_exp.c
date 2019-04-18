/**
 * @brief: ECE254 Keil ARM RL-RTX Task Management Lab starter file that calls os_tsk_get()
 * @file: main_task_exp.c
 * @date: 2015/09/13
 */
/* NOTE: This release is for students to use */

#include <LPC17xx.h>
#include "uart_polling.h" 
#include <RTL.h>
#include "../../RTX_CM3/INC/RTL_ext.h" /* extended interface header file */
#include <stdio.h>
#include <string.h>

#define NUM_NAMES 4

struct func_info {
  void (*p)();      /* function pointer */
  char name[16];    /* name of the function */
};

extern void os_idle_demon(void);
__task void task1(void);
__task void task2(void);
__task void init (void);
 
extern char *state2str(unsigned char state, char *str);
char *fp2name(void (*p)(), char *str);

OS_MUT g_mut_uart;
OS_MUT g_mut_evnt;
OS_TID g_tid = 255;

int  g_counter = 0;  // a global counter
char g_str[16];
char g_tsk_name[16];

struct func_info g_task_map[NUM_NAMES] = \
{
  /* os_idle_demon function ptr to be initialized in main */
  {NULL,  "os_idle_demon"}, \
  {task1, "task1"},   \
  {task2, "task2"},   \
  {init,  "init" }
};

/* no local variables defined, use one global var */
__task void task1(void)
{
	os_mut_wait(g_mut_uart, 0xFFFF);
	printf("Taks1: I am wating for task 2 to finish\n");
	os_mut_release(g_mut_uart);
	
	os_dly_wait(2);
	
	/****/os_mut_wait(g_mut_evnt, 0xFFFF);
	
	os_mut_wait(g_mut_uart, 0xFFFF);
	printf("Taks1: I am Doing Stuff\n");
	os_mut_release(g_mut_uart);
	
	/****/os_mut_release(g_mut_evnt);
	
	
	os_mem_alloc(NULL);
	os_mem_free(NULL,NULL);
	for (;;) {
		g_counter++;
	}
}


/*--------------------------- task2 -----------------------------------*/
/* checking states of all tasks in the system                          */
/*---------------------------------------------------------------------*/
__task void task2(void)
{
	U8 i=1;
	RL_TASK_INFO task_info;
	/****/os_mut_wait(g_mut_evnt, 0xFFFF);
	
	os_mut_wait(g_mut_uart, 0xFFFF);
	printf("Taks2: I am Doing Stuff that must occur before Task 1 runs\n");
	os_mut_release(g_mut_uart);
	
	/****/os_mut_release(g_mut_evnt);
	
  for(;;) {
		os_mut_wait(g_mut_uart, 0xFFFF);
		printf("\nTID\tNAME\t\tPRIO\tSTATE\n");
		os_mut_release(g_mut_uart);
			
		for(i = 0; i <3; i++) { // this is a lazy way of doing loop.
			if (os_tsk_get(i+1, &task_info) == OS_R_OK) {
				os_mut_wait(g_mut_uart, 0xFFFF);  
				printf("%d\t%s\t\t%d\t%s\n", \
							 task_info.task_id, \
							 fp2name(task_info.ptask, g_tsk_name), \
							 task_info.prio, \
							 state2str(task_info.state, g_str));
				os_mut_release(g_mut_uart);
			} 
		}
			
		if (os_tsk_get(0xFF, &task_info) == OS_R_OK) {
			os_mut_wait(g_mut_uart, 0xFFFF);  
			printf("%d\t%s\t\t%d\t%s\n", \
						 task_info.task_id, \
						 fp2name(task_info.ptask, g_tsk_name), \
						 task_info.prio, \
						 state2str(task_info.state, g_str));
			os_mut_release(g_mut_uart);
		} 
		#ifdef MYSIM
		os_dly_wait(2);
		#else
		os_dly_wait(200);
		#endif
	}
}

/*--------------------------- init ------------------------------------*/
/* initialize system resources and create other tasks                  */
/*---------------------------------------------------------------------*/
__task void init(void)
{
	os_mut_init(&g_mut_uart);
	os_mut_init(&g_mut_evnt);
	
	os_mut_wait(g_mut_uart, 0xFFFF);
	printf("init: the init TID is %d\n", os_tsk_self());
	os_mut_release(g_mut_uart);
  
	g_tid = os_tsk_create(task1, 1);  /* task 1 at priority 1 */
	os_mut_wait(g_mut_uart, 0xFFFF);
	printf("init: created task1 with TID %d\n", g_tid);
	os_mut_release(g_mut_uart);
  
	g_tid = os_tsk_create(task2, 1);  /* task 2 at priority 1 */
	os_mut_wait(g_mut_uart, 0xFFFF);
	printf("init: created task2 with TID %d\n", g_tid);
	os_mut_release(g_mut_uart);
  
	os_tsk_delete_self();     /* task MUST delete itself before exiting */
}

/** 
 * @brief: get function name by function pointer
 * @param: p the entry point of a function (i.e. function pointer)
 * @param: str the buffer to return the function name
 * @return: the function name string starting address
 */
char *fp2name(void (*p)(), char *str)
{
	int i;
  
	for ( i = 0; i < NUM_NAMES; i++) {
		if (g_task_map[i].p == p) {
			str = strcpy(str, g_task_map[i].name);  
			return str;
		}
	}
	strcpy(str, "ghost");
	return str;
}

int main(void)
{
	SystemInit();         /* initialize the LPC17xx MCU */
	uart0_init();         /* initialize the first UART */
  
  
	/* fill the fname map with os_idle_demon entry point */
	g_task_map[0].p = os_idle_demon;
  
	printf("Calling os_sys_init()...\n");
	os_sys_init(init);    /* initialize the OS and start the first task */
}
