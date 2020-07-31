/*INCLUDES ************************************************************/
#include "ses_scheduler.h"

/* PRIVATE VARIABLES **************************************************/
/** list of scheduled tasks */
/**
 * we need to create an empty linked list.
 * we loop on this list by address where the condition of the while loop is (taskDescriptor_s * next != NULL),
 * if its next is pointing to NULL, then we set the next to the first task
 */
static taskDescriptor *taskList = NULL;
static volatile systemTime_t millisec = 0;
/*FUNCTION DEFINITION *************************************************/
/**
 *
 */
static void scheduler_update(void)
{
	//This function decrements the expire of each task by 1 with every Timer trigger and marks expired tasks for execution
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		millisec++;
		taskDescriptor *current = taskList;
		while (current != NULL)
		{
			if (current->expire != 0)
			{
				current->expire--;
			}
			if (current->expire == 0)
			{
				current->execute = 1;
				current->expire = current->period;
			}
			current = current->next;
		}
	}
}
/**
 * Initializes the task scheduler by starting Timer2 and setting its callback.
 */
void scheduler_init()
{
	timer2_setCallback(&scheduler_update);
	timer2_start();
}
/**
 * Runs the schedules with all the tasks marked for execution in an infinite loop
 */
void scheduler_run()
{
	taskDescriptor *current = taskList;
	if (current == NULL)
	{
		return;
	}
	while (current != NULL)
	{
		if (current->execute == 1)
		{
			if (current->period == 0)
			{
				current->task(current->param);
				scheduler_remove(current);
			}
			else
			{
				current->task(current->param);
				current->execute = 0;
			}
		}
		current = current->next;
	}
	return;
}

bool scheduler_add(taskDescriptor *toAdd)
{
	//UART_DEBUG(toAdd->period);
	if (toAdd == NULL)
	{
		return false;
	}
	//ATOMIC_BLOCK is used to avoid reacting to external interrupts while doing the task
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		taskDescriptor *current = taskList;
		if (taskList == NULL)
		{
			taskList = toAdd;
			taskList->next = NULL;
			return true;
		}
		else
		{
			while (current != toAdd)
			{
				if (current->next == NULL)
				{
					current->next = toAdd;
					current->next->next = NULL;
					return true;
				}
				current = current->next;
			}
		}
	}
	return true;
}
void scheduler_remove(taskDescriptor *toRemove)
{
	if (toRemove == NULL)
	{
		return;
	}
	//ATOMIC_BLOCK is used to avoid reacting to external interrupts while doing the task
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{

		taskDescriptor *current = taskList;
		if (taskList == NULL)
		{
			return;
		}
		if (current == toRemove)
		{
			taskList = taskList->next;
		}
		else
		{
			while (current->next != toRemove)
			{
				if (current->next == NULL)
				{
					return;
				}
				current = current->next;
			}
			current->next = toRemove->next;
		}
	}
}

systemTime_t scheduler_getTime()
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		return millisec;
	}
}
void scheduler_setTime(systemTime_t time)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		millisec = time;
	}
}

time_t scheduler_calc_time(void)
{
	time_t time;
	time.milli = scheduler_getTime() % MILSEC_TO_SEC;
	time.second = (scheduler_getTime() / MILSEC_TO_SEC) % SEC_IN_A_MIN;
	time.minute = (scheduler_getTime() / (MILSEC_TO_MIN)) % MIN_IN_AN_HR;
	time.hour = (scheduler_getTime() / MILSEC_TO_HR) % HR_IN_A_DAY;
	return time;
}