/* INCLUDES ******************************************************************/

#include "ses_rotary.h"

/* DEFINES & MACROS **********************************************************/

static pTypeRotaryCallback RotaryClockwiseCallback;
static pTypeRotaryCallback RotaryCounterClockwiseCallback;

#define ROTARY_NUM_DEBOUNCE_CHECKS 5
#define DEBOUNCE_POS_ROTARY_COUNTER_CLOCKWISE 0x01
#define DEBOUNCE_POS_ROTARY_CLOCKWISE 0x02
#define A_ROTARY_PORT PORTB5
#define B_ROTARY_PORT PORTG2
#define A_ROTARY_PIN 5
#define B_ROTARY_PIN 2

/* FUNCTION DEFINITION *******************************************************/
//Initialize the buttons
void rotary_init()
{
    // Setting pin 5 and 2 of port B and G respectively to be input
    DDRB &= ~(0x01 << DDB5);
    DDRG &= ~(0x01 << DDG2);

    // Activating pull up resistors of pin 5 and 2 of port B and G respectively
    PORTB |= (0x01 << PORTB5);
    PORTG |= (0x01 << PORTG2);
}
//A function to check if the Rotary Clockwise button is pressed
bool rotary_isClockwisePressed(void)
{
    return !((PINB & (0x01 << A_ROTARY_PIN)) >> A_ROTARY_PIN);
}
//A function to check if the rotary counter-clockwise button is pressed
bool rotary_isCounterClockwisePressed(void)
{
    return !((PING & (0x01 << B_ROTARY_PIN)) >> B_ROTARY_PIN);
}

void rotary_setClockwiseCallback(pTypeRotaryCallback callback)
{
    if (callback == NULL)
    {
        return;
    }
    RotaryClockwiseCallback = callback;
}
void rotary_setCounterClockwiseCallback(pTypeRotaryCallback callback)
{
    if (callback == NULL)
    {
        return;
    }
    RotaryCounterClockwiseCallback = callback;
}

//A function to debounce the signal of the buttons by repetitive polling on the state of the button for several times
void rotary_checkState()
{
    static uint8_t state[ROTARY_NUM_DEBOUNCE_CHECKS] = {};
    volatile static uint8_t index = 0;
    volatile static uint8_t debouncedState = 0;
    uint8_t j = 0xFF; // setting the variable J to all ones.
    uint8_t lastDebouncedState = debouncedState;
    // each bit in every state byte represents one button
    state[index] = 0;
    if (rotary_isClockwisePressed())
    {
        state[index] |= DEBOUNCE_POS_ROTARY_CLOCKWISE;
    }
    if (rotary_isCounterClockwisePressed())
    {
        state[index] |= DEBOUNCE_POS_ROTARY_COUNTER_CLOCKWISE;
    }

    index++;
    if (index == ROTARY_NUM_DEBOUNCE_CHECKS)
    {
        index = 0;
    }
    // init compare value and compare with ALL reads, only if
    // we read ROTARY_NUM_DEBOUNCE_CHECKS consistent "1's" in the state
    // array, the button at this position is considered pressed

    for (uint8_t i = 0; i < ROTARY_NUM_DEBOUNCE_CHECKS; i++)
    {
        j = j & state[i];
    }
    debouncedState = j;

    if (debouncedState == 0x01 && !(lastDebouncedState == 0x01))
    {
        RotaryCounterClockwiseCallback();
    }
    if (debouncedState == 0x02 && !(lastDebouncedState == 0x02))
    {
        RotaryClockwiseCallback();
    }
}

/**
* Callback-procedure to plot the samples of the rotary pins on the LCD after first change
**/
void check_rotary()
{
    static uint8_t p = 0;
    static bool sampling = false;
    bool a = (PINB & (1 << A_ROTARY_PIN));
    bool b = (PING & (1 << B_ROTARY_PIN));
    if (a != b)
    {
        sampling = true;
    }
    if (sampling && p < 122)
    {
        lcd_setPixel((a) ? 0 : 1, p, true);
        lcd_setPixel((b) ? 4 : 5, p, true);
        p++;
    }
}