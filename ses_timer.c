/* INCLUDES ******************************************************************/
#include "ses_timer.h"

/* DEFINES & MACROS **********************************************************/
pTimerCallback Timer0_Callback;
pTimerCallback Timer1_Callback;
pTimerCallback Timer2_Callback;
pTimerCallback Timer5_Callback;

#define TIMER1_CYC_FOR_5MILLISEC 0x270F
#define TIMER2_CYC_FOR_1MILLISEC 0xf9
#define TIMER5_CYC_FOR_100MILLISEC 0x61A7

/*FUNCTION DEFINITION ********************************************************/
/* Timer 0 ****************/
void timer0_setCallback(pTimerCallback cb)
{
	if (cb == NULL)
	{
		return;
	}
	Timer0_Callback = cb;
}

void timer0_start()
{
	//select non-inverted PWM mode by setting COM0B0 and COM0B1 bits
	TCCR0A &= ~(0x01 << COM0B0);
	TCCR0A |= (0x01 << COM0B1);

	//select fast PWM mode by setting WGM00 and WGM01 bits and clearing WGM02 to count to max val 0xFF
	TCCR0A |= (0x01 << WGM00);
	TCCR0A |= (0x01 << WGM01);
	TCCR0B &= ~(0x01 << WGM02);

	//disable prescaler and select internal clk source
	TCCR0B |= (0x01 << CS00);
	TCCR0B &= ~(0x01 << CS01);
	TCCR0B &= ~(0x01 << CS02);

	//Write a 0 to bit PRTIM0 in PRR0 to enable Timer0
	PRR0 &= ~(0x01 << PRTIM0);
}

void timer0_stop()
{
	//Disable Timer/Counter0
	TCCR0B &= ~(0x01 << CS00);
	TCCR0B &= ~(0x01 << CS01);
	TCCR0B &= ~(0x01 << CS02);
}
/* Timer 1 ****************/
void timer1_setCallback(pTimerCallback cb)
{
	if (cb == NULL)
	{
		return;
	}
	Timer1_Callback = cb;
}

void timer1_start()
{
	//Using Clear Timer on Compare Match (CTC) mode of operation
	TCCR1A &= ~(0x01 << WGM10);
	TCCR1A &= ~(0x01 << WGM11);
	TCCR1B |= (0x01 << WGM12);
	TCCR1B &= ~(0x01 << WGM13);

	//Select Prescaler 8 and internal clk
	TCCR1B &= ~(0x01 << CS10);
	TCCR1B |= (0x01 << CS11);
	TCCR1B &= ~(0x01 << CS12);

	//Compare Match A Interrupt Enable
	TIMSK1 |= (0x01 << OCIE1A);

	//Clear the interrupt flag of Timer 1
	TIFR1 |= (0x01 << OCF1A);

	//Set interrupt every 5 ms
	OCR1A = TIMER1_CYC_FOR_5MILLISEC;

	//Enable Timer/Counter1
	PRR0 &= ~(0x01 << PRTIM1);
}

void timer1_stop()
{
	// Removing clock source (timer stop)
	TCCR1B &= ~(0x01 << CS10);
	TCCR1B &= ~(0x01 << CS11);
	TCCR1B &= ~(0x01 << CS12);
}
/* Timer 2 ****************/
void timer2_setCallback(pTimerCallback cb)
{
	if (cb == NULL)
	{
		return;
	}
	Timer2_Callback = cb;
}

void timer2_start()
{
	//Using Clear Timer on Compare Match (CTC) mode of operation
	TCCR2A &= ~(0x01 << WGM20);
	TCCR2A |= (0x01 << WGM21);
	TCCR2B &= ~(0x01 << WGM22);

	//Using CLKI/O and disabling external CLK source
	ASSR &= ~(0x01 << AS2);
	ASSR &= ~(0x01 << EXCLK);

	//Select Prescaler 64
	TCCR2B &= ~(0x01 << CS20);
	TCCR2B &= ~(0x01 << CS21);
	TCCR2B |= (0x01 << CS22);

	//Compare Match A Interrupt Enable
	TIMSK2 |= (0x01 << OCIE2A);

	//Clear the interrupt flag of Timer 2
	TIFR2 |= (0x01 << OCF2A);

	//Set interrupt every 1 ms
	OCR2A = TIMER2_CYC_FOR_1MILLISEC;

	//Enable Timer/Counter2
	PRR0 &= ~(0x01 << PRTIM2);
}

void timer2_stop()
{
	//Disable Timer/Counter2
	TCCR2B &= ~(0x01 << CS20);
	TCCR2B &= ~(0x01 << CS21);
	TCCR2B &= ~(0x01 << CS22);
}

/* Timer 5 ****************/

void timer5_setCallback(pTimerCallback cb)
{
	if (cb == NULL)
	{
		return;
	}
	Timer5_Callback = cb;
}

/**
 * Start timer 5 of MCU to trigger on compare match every 5ms.
 */
void timer5_start()
{
	//Using Clear Timer on Compare Match (CTC) mode of operation
	TCCR5A &= ~(0x01 << WGM50);
	TCCR5A &= ~(0x01 << WGM51);
	TCCR5B |= (0x01 << WGM52);
	TCCR5B &= ~(0x01 << WGM53);

	//Select Prescaler 64 and internal clk
	TCCR5B |= (0x01 << CS50);
	TCCR5B |= (0x01 << CS51);
	TCCR5B &= ~(0x01 << CS52);

	// Disable timer interrupt 5
	TIMSK5 |= (0x01 << OCIE5A);
	TIMSK5 &= ~(0x01 << OCIE5B);
	TIMSK5 &= ~(0x01 << OCIE5C);

	// Set interrupt every 100 ms
	OCR5A = TIMER5_CYC_FOR_100MILLISEC;

	// Enable timer 5
	PRR1 &= ~(0x01 << PRTIM5);
}

/**
 * Stops timer 5 of the MCU if it is no longer needed.
 */
void timer5_stop()
{
	// Removing clock source (timer stop)
	TCCR5B &= ~(0x01 << CS50);
	TCCR5B &= ~(0x01 << CS51);
	TCCR5B &= ~(0x01 << CS52);
}

ISR(TIMER1_COMPA_vect)
{
	Timer1_Callback();
}

ISR(TIMER2_COMPA_vect)
{
	Timer2_Callback();
}

ISR(TIMER5_COMPA_vect)
{
	Timer5_Callback();
}
