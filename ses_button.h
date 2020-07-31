#ifndef SES_BUTTON_H_
#define SES_BUTTON_H_

/* INCLUDES ******************************************************************/

#include "ses_common.h"
//Type Definitions
typedef void (*pButtonCallback)();
//typedef void (*pButtonCallback_uint8)(uint8_t);
/* FUNCTION PROTOTYPES *******************************************************/

/**
 * Initializes rotary encoder and joystick button
 */
void button_init(bool debouncing);

void button_setRotaryButtonCallback(pButtonCallback callback);
void button_setJoystickButtonCallback(pButtonCallback callback);

/** 
 * Get the state of the joystick button.
 */
bool button_isJoystickPressed(void);

/** 
 * Get the state of the rotary button.
 */
bool button_isRotaryPressed(void);
/**
 * Debounce the signal from the button
 */
void button_checkState();

#endif /* SES_BUTTON_H_ */
