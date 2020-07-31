/* INCLUDES ******************************************************************/

#include "ses_common.h"
#include "ses_led.h"

/*Macro to to initialize port pin *********************************************
#define PORT_PIN_INIT(PORT_DIRECTION_REG, PIN_NUMBER, PIN_DIRECTION) (PORT_DIRECTION_REG = (PORT_DIRECTION_REG & ~(0x01 << PIN_NUMBER)) | (PIN_DIRECTION << PIN_NUMBER))
#define PORT_PIN_WRITE(PORT_DATA_REG, PIN_NUMBER, WRITE_VALUE) (PORT_DATA_REG = (PORT_DATA_REG & ~(0x01 << PIN_NUMBER)) | (WRITE_VALUE << PIN_NUMBER))
#define PORT_PIN_READ(PORT_DATA_REG, PIN_NUMBER) ((PORT_DATA_REG & (0x01 << PIN_NUMBER)) >> PIN_NUMBER)
*/
/* FUNCTION DEFINITION *********************************************************/

void led_redInit(void)
{
    // PIN configuration
    DDRG |= (0x01 << DDG1);
    //Start the LED in an OFF state
    led_redOff();
}

void led_redToggle(void)
{
    //Toggle Red LED
    PORTG ^= (0x01 << PORTG1);
}

void led_redOn(void)
{
    // Turn ON Red LED, active low!
    PORTG &= ~(0x01 << PORTG1);
}

void led_redOff(void)
{
    // Turn OFF Red LED
    PORTG |= (0x01 << PORTG1);
}

// -------------------------Yellow LED --------------------------------
void led_yellowInit(void)
{
    DDRF |= (0x01 << DDF7);
    led_yellowOff();
}

void led_yellowToggle(void)
{
    PORTF ^= (0x01 << PORTF7);
}

void led_yellowOn(void)
{
    PORTF &= ~(0x01 << PORTF7);
}

void led_yellowOff(void)
{
    PORTF |= (0x01 << PORTF7);
}

// -------------------------Green LED --------------------------------
void led_greenInit(void)
{
    // PIN configuration
    DDRF |= (0x01 << DDF6);
    led_greenOff();
}

void led_greenToggle(void)
{
   PORTF ^= (0x01 << PORTF6);
}

void led_greenOn(void)
{
    PORTF &= ~(0x01 << PORTF6);
}

void led_greenOff(void)
{
    PORTF |= (0x01 << PORTF6);
}