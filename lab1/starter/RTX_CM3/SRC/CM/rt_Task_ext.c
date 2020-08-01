/**
 * @file: rt_Task_ext.c
 */
#include "rt_TypeDef.h"
#include "RTX_Config.h"
#include "rt_System.h"
#include "rt_Task.h"
#include "rt_List.h"
#include "rt_Robin.h"
#include "rt_HAL_CM.h"
#include "rt_Task_ext.h"

/*----------------------- rt_tsk_count_get --------------------------*/
/* added for ECE254 lab */
int rt_tsk_count_get (void) {
	int cnt = 0;
	int i;
	for (i = 0; i < os_maxtaskrun; i++) {
		P_TCB ptr = os_active_TCB[i];
		if (ptr && ptr->state != INACTIVE) {
			cnt++;
		}
	}
	if (os_idle_TCB.state != INACTIVE) {
		cnt++;
	}
	return cnt;
}

/*----------------------- rt_tsk_get --------------------------------*/

OS_RESULT rt_tsk_get (OS_TID task_id, RL_TASK_INFO *p_task_info) {
	P_TCB ptr;
	U16 sz;
	U32 top_tsk;
	U32 top;
	U32 used;
	if (!p_task_info || task_id < 1 || task_id > 0xFF) {
		return OS_R_NOK;
	}
	ptr = (task_id == 0xFF) ? &os_idle_TCB : os_active_TCB[task_id - 1];
	if (!ptr) {
		return OS_R_NOK;
	}
	p_task_info->state   = ptr->state;
	p_task_info->prio    = ptr->prio;
	p_task_info->task_id = ptr->task_id;
	p_task_info->ptask   = ptr->ptask;
	// get stack usage
	sz = ptr->priv_stack >> 2;
	if (!sz) {
		// if no private stack then use the default stack info
		sz = os_stackinfo >> 2;
	}
	top_tsk = (ptr->state == RUNNING) ? rt_get_PSP() : ptr->tsk_stack;
	top = (U32) ptr->stack;
	used = sz - (top_tsk - top) / 4;
	p_task_info->stack_usage = used * 100 / sz;
	return OS_R_OK;
}
/* end of file */
