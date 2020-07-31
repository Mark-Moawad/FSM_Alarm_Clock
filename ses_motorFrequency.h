#ifndef SES_MOTOR_FREQ_H_
#define SES_MOTOR_FREQ_H_

/* INCLUDES ******************************************************************/
#include <avr/io.h>

/* FUNCTION PROTOTYPES *******************************************************/
/**
 * initialize motor frequency measuring module.
 */
void motorFrequency_init();
/**
 * return the most recent measurement in Hertz.
 */
uint16_t motorFrequency_getRecent();
/**
 * return calculates the median of the last N interval measurements.
 */
uint16_t motorFrequency_getMedian();

void calcMotorFrequency();
#endif /* SES_MOTOR_FREQ_H_ */