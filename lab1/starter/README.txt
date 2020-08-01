Summary of Tests:

task_count_v:
Asserts that there are the correct amount of active tasks running. We set up 6 tasks not including the idle task.
Then we call os_tsk_count_get() to verify the amount of active tasks.

task_usage:
Asserts that stack usage is reporting within the expected range by getting the stack_usage of a running task.
Checks it against the range of values from 97-100.

task_get_invalid:
Asserts that when trying to get a task with an invalid TID that the os_tsk_get function returns in invalid result code.

task_get_give_null:
Asserts that when trying to get a task with a null RL_TASK_INFO pointer that an invalid result code is returned.

task_get_all:
Periodically prints out the task status of every active task in the system, including the idle task.