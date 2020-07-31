#ifndef SES_ROTARY_H_
#define SES_ROTARY_H_

/* INCLUDES ******************************************************************/
#include "ses_common.h"
#include "ses_lcd.h"
#include "ses_uart.h"
//Type Definitions
typedef void (*pTypeRotaryCallback)();
/* FUNCTION PROTOTYPES *******************************************************/

/**
 * Initializes rotary encoder buttons
 */
void rotary_init();

/**
 * Sets the callback functions of the rotary direction buttons
 */
void rotary_setClockwiseCallback(pTypeRotaryCallback);
/**
 * Sets the callback functions of the rotary direction buttons
 */
void rotary_setCounterClockwiseCallback(pTypeRotaryCallback);
/** 
 * Get the state of the rotary clockwise
 */
bool rotary_isClockwisePressed(void);

/** 
 * Get the state of the rotary counter-clockwise
 */
bool rotary_isCounterClockwisePressed(void);
/**
 * Debounce the signal from the buttons
 */
void rotary_checkState();

/**
* Callback-procedure to plot the samples of the rotary pins on the LCD after first change
**/
void check_rotary();

#endif /* SES_ROTARY_H_ */
