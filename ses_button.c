/* INCLUDES ******************************************************************/

#include "ses_common.h"
#include "ses_button.h"
#include "ses_led.h"
#include "ses_timer.h"
#include "ses_uart.h"
#include "util/atomic.h"

/* DEFINES & MACROS **********************************************************/

pButtonCallback RotaryButtonCallback;
pButtonCallback JoystickButtonCallback;

#define BUTTON_NUM_DEBOUNCE_CHECKS 5
//#define BUTTON_DEBOUNCE_PERIOD 30
#define BUTTON_DEBOUNCE_POS_JOYSTICK 0x01
#define BUTTON_DEBOUNCE_POS_ROTARY 0x02

/* FUNCTION DEFINITION *******************************************************/
//Initialize the buttons
void button_init(bool debouncing)
{
    // Setting pin 6 and 7 of port B to be input
    DDRB &= ~(0x01 << DDB7);
    DDRB &= ~(0x01 << DDB6);
    // Activating pull up resistors of pin 6 and 7 of port B.
    PORTB |= (0x01 << PORTB7);
    PORTB |= (0x01 << PORTB6);
    /**Polling on a bool to determine whether to use the debouncing mode or the normal mode using ISRs
    *uncomment the following section to make the debouncing using timer1 not instead of
    *scheduler implemented in later tasks
    */
    if (debouncing)
    {
        // TODO initialization for debouncing
        timer1_setCallback(&button_checkState);
        PCICR &= ~(0x01 << PCIE0);
        PCMSK0 &= ~(0x01 << PCINT7);
        PCMSK0 &= ~(0x01 << PCINT6);
        timer1_start();
    }
    else
    {
        //  initialization for direct interrupts (e.g. setting up the PCICR register)
        PCICR |= (0x01 << PCIE0);
        PCMSK0 |= (0x01 << PCINT7);
        PCMSK0 |= (0x01 << PCINT6);
    }
}
//A function to check if the Joystick button is pressed
bool button_isJoystickPressed(void)
{ // return the value of the joystick button pin
    return !((PINB & (0x01 << PINB7)) >> PINB7);
}
//A function to check if the Rotary button is pressed
bool button_isRotaryPressed(void)
{ // return the value of the Rottary button pin
    return !((PINB & (0x01 << PINB6)) >> PINB6);
}
ISR(PCINT0_vect)
{
    if (button_isRotaryPressed())
    {
        RotaryButtonCallback();
    }

    if (button_isJoystickPressed())
    {
        JoystickButtonCallback();
    }
    // execute callbacks here
}
void button_setRotaryButtonCallback(pButtonCallback callback)
{
    if (callback == NULL)
    {
        return;
    }
    RotaryButtonCallback = callback;
}
void button_setJoystickButtonCallback(pButtonCallback callback)
{
    if (callback == NULL)
    {
        return;
    }
    JoystickButtonCallback = callback;
}
//A function to debounce the signal of the buttons by repetitive polling on the state of the button for several times
void button_checkState()
{
    static uint8_t state[BUTTON_NUM_DEBOUNCE_CHECKS] = {};
    volatile static uint8_t index = 0;
    volatile static uint8_t debouncedState = 0;
    uint8_t j = 0xFF; // setting the variable J to all ones.
    uint8_t lastDebouncedState = debouncedState;
    // each bit in every state byte represents one button
    state[index] = 0;
    if (button_isJoystickPressed())
    {
        state[index] |= BUTTON_DEBOUNCE_POS_JOYSTICK;
    }
    if (button_isRotaryPressed())
    {
        state[index] |= BUTTON_DEBOUNCE_POS_ROTARY;
    }
    index++;
    if (index == BUTTON_NUM_DEBOUNCE_CHECKS)
    {
        index = 0;
    }
    // init compare value and compare with ALL reads, only if
    // we read BUTTON_NUM_DEBOUNCE_CHECKS consistent "1's" in the state
    // array, the button at this position is considered pressed

    for (uint8_t i = 0; i < BUTTON_NUM_DEBOUNCE_CHECKS; i++)
    {
        j = j & state[i];
    }
    debouncedState = j;
    //Joystick button only is pressed
    if (debouncedState == 0x01 && !(lastDebouncedState == 0x01))
    {

        JoystickButtonCallback();
    }
    //Rotary button only is pressed
    if (debouncedState == 0x02 && !(lastDebouncedState == 0x02))
    {
        RotaryButtonCallback();
    }
    //Both Rotary and Joystick buttons are pressed
    if (debouncedState == 0x03 && !(lastDebouncedState == 0x03))
    {
        RotaryButtonCallback();
        JoystickButtonCallback();
    }
}