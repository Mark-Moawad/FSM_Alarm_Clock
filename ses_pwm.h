#ifndef SES_PWM_H_
#define SES_PWM_H_

/* INCLUDES ******************************************************************/
#include <avr/io.h>
/* FUNCTION PROTOTYPES *******************************************************/
/**
 * Initializes PWM module
 */
void pwm_init(void);
/**
 * sets duty cycle of the PWM module
 */
void pwm_setDutyCycle(uint8_t dutyCycle);
/**
 * check the state of the motor and change it
 */
void motor_changestate();
#endif /* SES_PWM_H_ */
