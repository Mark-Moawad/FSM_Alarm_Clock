/* INCLUDES ******************************************************************/

#include "ses_common.h"
#include "ses_adc.h"
#include "ses_uart.h"

/* DEFINES & MACROS **********************************************************/
static int16_t ADC_TEMP_FACTOR = 100;
static int16_t SLOPE_CORRECTION_FACTOR = 1;
static int16_t ADC_TEMP_MAX = 313.15;
static int16_t ADC_TEMP_MIN = 293.15;
static int16_t ADC_TEMP_RAW_MAX = 256;
static int16_t ADC_TEMP_RAW_MIN = 479;

#define SENSOR_TEMPRATURE_PIN 2
#define SENSOR_LIGHT_PIN 4
#define BUTTON_JOYSTICK_PIN 5
#define MICROPHONE_PIN0 0
#define MICROPHONE_PIN1 1

#define REG_OVERWRITE_VAL(REG, REG_VAL_BIT7, REG_VAL_BIT6, REG_VAL_BIT5, REG_VAL_BIT4, REG_VAL_BIT3, REG_VAL_BIT2, REG_VAL_BIT1, REG_VAL_BIT0) (REG = ((REG_VAL_BIT7 << 7) | (REG_VAL_BIT6 << 6) | (REG_VAL_BIT5 << 5) | (REG_VAL_BIT4 << 4) | (REG_VAL_BIT3 << 3) | (REG_VAL_BIT2 << 2) | (REG_VAL_BIT1 << 1) | (REG_VAL_BIT0)))

/* ADC Run mode selection**********************************************/
#define Autorun 0

/* FUNCTION DEFINITION *******************************************************/
void adc_init(void)
{
    // Temprature sensor init
    DDRF &= ~(0x01 << SENSOR_TEMPRATURE_PIN);
    PORTF &= ~(0x01 << SENSOR_TEMPRATURE_PIN);

    // Light sensor init
    DDRF &= ~(0x01 << SENSOR_LIGHT_PIN);
    PORTF &= ~(0x01 << SENSOR_LIGHT_PIN);

    // Joystick init
    DDRF &= ~(0x01 << BUTTON_JOYSTICK_PIN);
    PORTF &= ~(0x01 << BUTTON_JOYSTICK_PIN);

    // Microphone init
    DDRF &= ~(0x01 << MICROPHONE_PIN0);
    DDRF &= ~(0x01 << MICROPHONE_PIN1);
    PORTF &= ~(0x01 << MICROPHONE_PIN0);
    PORTF &= ~(0x01 << MICROPHONE_PIN1);

    // ADC POWER REDUCTION disable
    PRR0 &= ~(0x01 << PRADC);

    // ADC POWER REF SET to 1.6
    ADMUX |= (0x01 << REFS0);
    ADMUX |= (0x01 << REFS1);

    // ADC Result right adjusted
    ADMUX &= ~(0x01 << ADLAR);

    // ADC Clock Prescale
    ADCSRA |= (0x01 << ADPS0);
    ADCSRA |= (0x01 << ADPS1);
    ADCSRA &= ~(0x01 << ADPS2);

//ADC Auto trigger select mode
#if Autorun == 0
    ADCSRA &= ~(0x01 << ADATE);
#else
    ADCSRA |= (0x01 << ADATE);
    ADCSRA &= ~(0x01 << ADTS0);
    ADCSRA &= ~(0x01 << ADTS1);
    ADCSRA &= ~(0x01 << ADTS2);
#endif // _DEBUG
    //ADC Enable
    ADCSRA |= (0x01 << ADEN);
}

uint16_t adc_read(uint8_t adc_channel) // 1 0 1    mux2 = 1, mux1 = 0, mux0 = 1
{
    if ((adc_channel < 0) || (adc_channel > 7))
    {
        return ADC_INVALID_CHANNEL;
    }
#if Autorun == 0
    // MUX5:0 <-  ADC_Channel
    ADCSRB &= ~(0x01 << MUX5);
    // ADMUX set value
    REG_OVERWRITE_VAL(ADMUX, REFS1, REFS0, ADLAR,
                      ((adc_channel & (0x01 << MUX4)) >> MUX4),
                      ((adc_channel & (0x01 << MUX3)) >> MUX3),
                      ((adc_channel & (0x01 << MUX2)) >> MUX2),
                      ((adc_channel & (0x01 << MUX1)) >> MUX1),
                      ((adc_channel & (0x01 << MUX0)) >> MUX0));
    //set ADSC = 1
    ADCSRA |= (0x01 << ADSC);

    //while ADSC == 1 wait
    while (ADSC)
    {
    }
#endif
    return ADC;
}

uint8_t adc_getJoystickDirection()
{
    return 0;
}
/**
 *This function is used to read the Temperature value and return it with a precision of 0.1 
 */
int16_t adc_getTemperature()
{
    int16_t adc = adc_read(ADC_TEMP_CH);
    int16_t slope = ((ADC_TEMP_MAX - ADC_TEMP_MIN) * ADC_TEMP_FACTOR / (ADC_TEMP_RAW_MAX - ADC_TEMP_RAW_MIN)) - SLOPE_CORRECTION_FACTOR;
    int16_t offset = ADC_TEMP_MAX * ADC_TEMP_FACTOR - (ADC_TEMP_RAW_MAX * slope);
    return ((adc * slope + offset) - 27315) * 10 / ADC_TEMP_FACTOR;
}
