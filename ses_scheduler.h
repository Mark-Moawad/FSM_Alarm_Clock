#ifndef SCHEDULER_H_
#define SCHEDULER_H_

/*INCLUDES *******************************************************************/
#include "ses_timer.h"
#include "util/atomic.h"
#include "ses_button.h"
#include "ses_lcd.h"
#include "ses_uart.h"
/**
 * defining overflow factors
*/
#define SEC_IN_A_MIN 60
#define MIN_IN_AN_HR 60
#define HR_IN_A_DAY 24

/**
 * defining conversion factors
*/
#define MILSEC_TO_SEC 1000
#define MILSEC_TO_MIN 60000
#define MILSEC_TO_HR 3600000

/* Debugging Macros ******************************************/
#define _DEBUG 1

#if _DEBUG == 1
#define UART_DEBUG(value) fprintf(uartout, "%d   ", value)
#else
#define UART_DEBUG(value)
#endif // _DEBUG

/* TYPES ********************************************************************/

/** Task structure to schedule tasks
 */

typedef uint32_t systemTime_t;

typedef struct {
uint8_t hour;
uint8_t minute;
uint8_t second;
uint16_t milli;
}time_t;

/**type of function pointer for tasks */
typedef void (*task_t)(void *);

typedef struct taskDescriptor_s {
	task_t task;          ///< function pointer to call
	void *  param;        ///< pointer, which is passed to task when executed
	uint16_t expire;      ///< time offset in ms, after which to call the task
	uint16_t period;      ///< period of the timer after firing; 0 means exec once
	uint8_t execute:1;    ///< for internal use
	uint8_t reserved:7;   ///< reserved
	struct taskDescriptor_s * next; ///< pointer to next task, internal use
} taskDescriptor;








/* FUNCTION PROTOTYPES *******************************************************/

/**
 * Initializes the task scheduler. Uses hardware timer2 of the AVR.
 */
void scheduler_init();

/**
 * Runs scheduler in an infinite loop.
 */
void scheduler_run();

/**
 * Adds a new task to the scheduler.
 * May be called from any context (interrupt or main program)
 *
 * @param td   Pointer to taskDescriptor structure. The scheduler takes
 *             possesion of the memory pointed at by td until the task
 *             is removed by scheduler_remove or -- in case of a 
 *             non-periodic task -- the task is executed. td->expire 
 *             and td->execute are changed to by the task scheduler.
 *
 * @return     false, if task is already scheduled or invalid (NULL)
 *             true, if task was successfully added to scheduler and 
 *             will be executed after td->expire ms.
 */
bool scheduler_add(taskDescriptor * td);

/**
 * Removes a task from the scheduler.
 * May be called from any context (interrupt or main program)
 *
 * @param td	pointer to task descriptor to remove
 * */
void scheduler_remove(taskDescriptor * td);
/**
 * a function used for debugging
 **/
void scheduler_debug();
/**
* get the current time in millisec
 **/
systemTime_t scheduler_getTime();
/**
* set the current time in millisec
**/

void scheduler_setTime(systemTime_t time);
/**
* calculate time in Human readable format
**/
time_t scheduler_calc_time (void);

#endif /* SCHEDULER_H_ */
