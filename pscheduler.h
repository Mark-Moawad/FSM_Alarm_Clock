// #ifndef PSCHEDULER_H
// #define PSCHEDULER_H

// /*INCLUDES *******************************************************************/
// #include <avr/io.h>
// #include <stdint.h>

// /*DEFINITIONS *******************************************************************/
// #define TASK_STACK_SIZE 256

// typedef void (*task_t)(void);

// typedef struct context_t
// {
//   uint8_t stack[TASK_STACK_SIZE];
//   void *pstack;
//   struct context_t *next;
// } context;

// void pscheduler_run(task_t taskList[], uint8_t numTasks);
// void vPortYieldFromTick(void);
// void portSAVE_CONTEXT(void);
// void vTaskIncrementTick(void);
// void vTaskSwitchContext(void);
// void portRESTORE_CONTEXT(void);
// #endif /* PSCHEDULER_H */
