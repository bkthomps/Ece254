/*----------------------------------------------------------------------------
 *      ECE254 Lab Task Management
 *----------------------------------------------------------------------------
 *      Name:    RT_MEMBOX_ext.C
 *      Purpose: Interface functions for blocking 
 *               fixed memory block management system
 *      Rev.:    V4.60
 *----------------------------------------------------------------------------
 *      This code is extends the RealView Run-Time Library.
 *      Created by University of Waterloo ECE254 Lab Staff.
 *---------------------------------------------------------------------------*/
 
/*----------------------------------------------------------------------------
 *      Includes
 *---------------------------------------------------------------------------*/
 
#include "rt_TypeDef.h"
#include "RTX_Config.h"
#include "rt_System.h"
#include "rt_MemBox.h"
#include "rt_HAL_CM.h"
#include "rt_Task.h"       /* added in ECE254 lab keil_proc */ 
#include "rt_MemBox_ext.h" /* added in ECE254 lab keil_proc */
#include "rt_List.h"

/*----------------------------------------------------------------------------
 *      Global Variables
 *---------------------------------------------------------------------------*/

static int blocked_tasks_count;	// this is to keep track of how many tasks are blocked
static struct OS_XCB queue;			// this holds the blocked tasks

/*----------------------------------------------------------------------------
 *      Global Functions
 *---------------------------------------------------------------------------*/

/*==========================================================================*/
/*  The following are added for ECE254 Lab Task Management Assignmet       */
/*==========================================================================*/

/**  
 * os_mem_alloc(mem_pool) calls this function.
 * @brief: Request a block of memory from the memory pool and block the calling
 * 			task if no memory is available in the memory pool
 */
void *rt_mem_alloc (void *mem_pool) {
	void *box;
	if (!mem_pool) {
		return NULL;
	}
	box = rt_alloc_box(mem_pool);
	if (!box) {
		int id = rt_tsk_sel();							// get the id of the current task
		P_TCB tsk = os_active_TCB[id - 1];	// get the current task
		rt_put_prio(&queue, tsk);						// add the current task to the blocked tasks queue
		blocked_tasks_count++;							// increase the blocked tasks count
		rt_block(0xFFFF, 10);								// block this task
	}
	return box;
}


/**
 * os_mem_free(mem_pool, box) calls this function.
 * free the memory pointed by box, it will unblock any task that might be 
 * 		waiting for memory.
 * @return: OS_R_OK on success and OS_R_NOK if box does not belong to mem_pool 
 */
OS_RESULT rt_mem_free (void *mem_pool, void *box) {
	P_TCB tsk;
	int err;
	if (!mem_pool || !box) {
		return OS_R_OK;
	}
	err = rt_free_box(mem_pool, box);
	if (err) {
		return OS_R_NOK;
	}
	if (blocked_tasks_count) {
		tsk = rt_get_first(&queue);	// get the first blocked task
		tsk->ret_val = box;					// set the return value of the task
		rt_dispatch(tsk);						// dispatch the first blocked task
		blocked_tasks_count--;			// reduce the count because we unblocked the task
	}
	return OS_R_OK;
}
/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
