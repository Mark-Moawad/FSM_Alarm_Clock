/* INCLUDES ******************************************************************/
#include "ses_pwm.h"
#include "ses_timer.h"
#include "ses_led.h"
#include "ses_uart.h"
/* DEFINES **********************************************************/
volatile static uint8_t motorstate = 0;

/* FUNCTION DEFINITION *******************************************************/
void pwm_init(void)
{
    //initialize the motor at stop state
    OCR0B = 0xFF;

    //set pin 5 in PORTG to output
    DDRG |= (0x01 << DDG5);

    
    pwm_setDutyCycle(0);

    //use Timer 0 to perform PWM
    timer0_start();   
}
void pwm_setDutyCycle(uint8_t dutyCycle)
{
    OCR0B = dutyCycle;
}
void motor_changestate()
{
    if (!motorstate)
    {
        // set the duty cycle
        pwm_setDutyCycle(170);
        motorstate = 1;
    }
    else
    {
        //sets duty cycle of the PWM to 0% to stop the motor
        pwm_setDutyCycle(0);
        motorstate = 0;
    }
}