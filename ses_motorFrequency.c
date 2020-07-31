/* INCLUDES ******************************************************************/

#include "ses_led.h"
#include "ses_timer.h"
#include "ses_motorFrequency.h"
#include "ses_uart.h"
#include "util/atomic.h"

/* variable definitions *******************************************************/
volatile static uint16_t motorSpikeCounter = 0;
volatile static uint16_t freqency = 0;

volatile static uint8_t index = 0;
//static uint16_t cycleTime = 0;

#define SPIKES_PER_REVOLUTION 6
#define REV_PER_SEC 10
#define NUMBER_OF_SAMPLES 10
#define FREQ_CALC_FACTOR 250000
#define CYCLETIME_CALC_FACTOR 1000000

volatile static uint16_t freqArray[NUMBER_OF_SAMPLES] = {0};

/* FUNCTION DEFINITION *******************************************************/
void motorFrequency_init()
{
    // setting interrupt to rising edge.
    EICRA |= (0x01 << ISC00);
    EICRA |= (0x01 << ISC01);

    // enable interrupt INT0 and disabling INT7:1
    EIMSK = 0x01;
}
/**
 * return the most recent measurement in Hertz.
 */

uint16_t motorFrequency_getRecent()
{
    return freqency; // reset motorSpikeCounter
}
/**
 * return calculates the median of the last N interval measurements.
 */
uint16_t motorFrequency_getMedian()
{
    uint16_t medianFreq = 0;

    for (uint8_t i = 0; i < NUMBER_OF_SAMPLES; i++)
    {
        medianFreq += freqArray[i];
    }

    return medianFreq /= NUMBER_OF_SAMPLES;
}
/**
 * Interrupt sevice routine
 */
ISR(INT0_vect)
{
    motorSpikeCounter++;
    led_yellowToggle();
}
/*ISR(INT0_vect)
{
    if (motorSpikeCounter == SPIKES_PER_REVOLUTION)
    {
        freqency = FREQ_CALC_FACTOR / TCNT5;            // save the frequency of the last cycle in Hz
        //cycleTime = CYCLETIME_CALC_FACTOR / freqency; //  save the time of the last cycle in us.
        TCNT5 = 0;                                      // reset timer 0
        motorSpikeCounter = 0;                          // reset motorSpikeCounter
    }
    else
    {
        motorSpikeCounter++;
    }
    led_yellowToggle();
    led_greenOff();
}*/

void calcMotorFrequency()
{
    freqency = (motorSpikeCounter / SPIKES_PER_REVOLUTION) * REV_PER_SEC; //get frequency every 1 s
    freqArray[index] = freqency;
    motorSpikeCounter = 0; // reset motorSpikeCounter
    index++;
    if (!freqency)
    {
        led_greenOn();
    }
    else
    {
        led_greenOff();
    }
    if (index >= NUMBER_OF_SAMPLES)
    {
        index = 0; //reset index
    }
}
