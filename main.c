#include "alarm_fsm.h"

typedef enum
{
    red,
    yellow,
    green
} led_color;

/*Macros for the task periods *********************************************************************************/
#define task1_PERIOD 500
#define TASK2_PERIOD 250
#define TASK3_PERIOD 5000
#define TASK4_PERIOD 1000
#define TASK5_PERIOD 500
#define TASK6_PERIOD 500
#define TASK7_PERIOD 1000
#define TASK8_PERIOD 500
#define TASK9_PERIOD 500
#define TASK10_PERIOD 1000
#define TASK11_PERIOD 5
#define TASK12_PERIOD 1000

/* Macro for for the task transition ****************************************************************************/
#define TRANSITION(newState) (fsm->state = newState, RET_TRANSITION)

/*Variables Declaration ******************************************************************/
taskDescriptor task1, task2, task3, task4, task5, task6, task7, task8, task9, task10, task11, task12;

static Fsm alarm_clock;
static Event alarm_event;

led_color task2_param = red;
led_color task12_param = green;

/*State handler fuction Declaration  ******************************************************************/
// State functions definitions
fsmReturnStatus clock_init(Fsm *fsm, const Event *e);
fsmReturnStatus clock_hr_set(Fsm *fsm, const Event *e);
fsmReturnStatus clock_min_set(Fsm *fsm, const Event *e);
fsmReturnStatus normal_operation(Fsm *fsm, const Event *e);
fsmReturnStatus alarm_hr_set(Fsm *fsm, const Event *e);
fsmReturnStatus alarm_min_set(Fsm *fsm, const Event *e);
fsmReturnStatus alarm_ring(Fsm *fsm, const Event *e);

// callback fuction declarations
void joystick_pressed_dispatch(void);
void rotary_pressed_dispatch(void);
void rotary_Clockwise_dispatch(void);
void rotary_Counter_Clockwise_dispatch(void);

// schedule tasks callback functions declarations
void led_toggle(void *param);
void timeout_dispatch(void *param);
void alarm_clock_match_dispatch(void *param);
void lcd_clock_set_hr_display(void *param);
void lcd_clock_set_min_display(void *param);
void lcd_clock_display(void *param);
void lcd_alarm_set_hr_display(void *param);
void lcd_alarm_set_min_display(void *param);
void udpate_system_time(void *param);
void alarm_ring_display(void *param);
void rotary_debounce(void *param);

int main()
{
    // LCD initialization
    lcd_init();

    // LEDs initialization
    led_redInit();
    led_yellowInit();
    led_greenInit();

    // Button initialization and setting callbacks
    button_init(true);
    button_setJoystickButtonCallback(&joystick_pressed_dispatch);
    button_setRotaryButtonCallback(&rotary_pressed_dispatch);

    // Rotary initialization and setting callbacks
    rotary_init();
    rotary_setClockwiseCallback(&rotary_Clockwise_dispatch);
    rotary_setCounterClockwiseCallback(&rotary_Counter_Clockwise_dispatch);

    //initializing taskDescriptor 1 - task to display the on LCD during the alarm ring state
    task1.task = alarm_ring_display;
    task1.param = NULL;
    task1.period = task1_PERIOD;
    task1.expire = task1.period;
    task1.execute = false;

    //initializing taskDescriptor 2 - task to toggle the red LED ever 250ms
    task2.task = led_toggle;
    task2.param = &task2_param;
    task2.period = TASK2_PERIOD;
    task2.expire = task2.period;
    task2.execute = false;

    //initializing taskDescriptor 3 - task to change the state from alarm ringing to normal operation after 5 sec
    task3.task = timeout_dispatch;
    task3.param = NULL;
    task3.period = TASK3_PERIOD;
    task3.expire = task3.period;
    task3.execute = false;

    //initializing taskDescriptor 4 - task to check if the time matches the alarm time
    task4.task = alarm_clock_match_dispatch;
    task4.param = NULL;
    task4.period = TASK4_PERIOD;
    task4.expire = task4.period;
    task4.execute = false;

    //initializing taskDescriptor 5 - task to display on LCD during clock set hour time
    task5.task = lcd_clock_set_hr_display;
    task5.param = NULL;
    task5.period = TASK5_PERIOD;
    task5.expire = task5.period;
    task5.execute = false;

    //initializing taskDescriptor 6 - task to display on LCD during clock set minute time
    task6.task = lcd_clock_set_min_display;
    task6.param = NULL;
    task6.period = TASK6_PERIOD;
    task6.expire = task6.period;
    task6.execute = false;

    //initializing taskDescriptor 7 - task to display on LCD during normal operation
    task7.task = lcd_clock_display;
    task7.param = NULL;
    task7.period = TASK7_PERIOD;
    task7.expire = task7.period;
    task7.execute = false;

    //initializing taskDescriptor 8 - task to display on LCD during alarm set minute time
    task8.task = lcd_alarm_set_hr_display;
    task8.param = NULL;
    task8.period = TASK8_PERIOD;
    task8.expire = task8.period;
    task8.execute = false;

    //initializing taskDescriptor 9 - task to display on LCD during alarm set minute time
    task9.task = lcd_alarm_set_min_display;
    task9.param = NULL;
    task9.period = TASK9_PERIOD;
    task9.expire = task9.period;
    task9.execute = false;

    //initializing taskDescriptor 10 - task to update the system time
    task10.task = udpate_system_time;
    task10.param = NULL;
    task10.period = TASK10_PERIOD;
    task10.expire = task10.period;
    task10.execute = false;

    //initializing taskDescriptor 11 - task used to conduct debounce for the rotary left and right buttons
    task11.task = rotary_debounce;
    task11.param = NULL;
    task11.period = TASK11_PERIOD;
    task11.expire = task11.period;
    task11.execute = false;

    //initializing taskDescriptor 12 - task to toggle the green LED every one sec
    task12.task = led_toggle;
    task12.param = &task12_param;
    task12.period = TASK12_PERIOD;
    task12.expire = task12.period;
    task12.execute = false;

    scheduler_init();
    scheduler_add(&task11);

    fsm_init(&alarm_clock, &clock_init);
    alarm_event.signal = ENTRY;
    fsm_dispatch(&alarm_clock, &alarm_event);
    sei();
    while (1)
    {
        scheduler_run();
    }
    return 0;
}
/* State functions definitions ******************************************************************/
fsmReturnStatus clock_init(Fsm *fsm, const Event *e)
{
    if (fsm == NULL)
    {
        return RET_IGNORED;
    }
    if (e->signal == ENTRY)
    {
        fsm->isAlarmEnabled = false;
        fsm->timeSet.milli = 0;
        fsm->timeSet.second = 0;
        fsm->timeSet.minute = 0;
        fsm->timeSet.hour = 0;
        fsm->alarmSet.milli = 0;
        fsm->alarmSet.second = 0;
        fsm->alarmSet.minute = 0;
        fsm->alarmSet.hour = 0;
        return TRANSITION(clock_hr_set);
    }
    return RET_IGNORED;
}

fsmReturnStatus clock_hr_set(Fsm *fsm, const Event *e)
{
    if (fsm == NULL)
    {
        return RET_IGNORED;
    }
    switch (e->signal)
    {
    case ENTRY:
        scheduler_add(&task5);
        break;
    case ROTARY_PRESSED:
        fsm->timeSet.hour++;
        if (fsm->timeSet.hour >= HR_IN_A_DAY)
        {
            fsm->timeSet.hour = 0;
        }
        return RET_HANDLED;
    case JOYSTICK_PRESSED:
        return TRANSITION(clock_min_set);
    case ROTARY_CLOCKWISE:
        fsm->timeSet.hour++;
        if (fsm->timeSet.hour >= HR_IN_A_DAY)
        {
            fsm->timeSet.hour = 0;
        }
        return RET_HANDLED;
    case ROTARY_COUNTER_CLOCKWISE:
        fsm->timeSet.hour--;
        if (fsm->timeSet.hour == 255)
        {
            fsm->timeSet.hour = HR_IN_A_DAY - 1;
        }
        return RET_HANDLED;
    case EXIT:
        scheduler_remove(&task5);
        break;
    default:
        return RET_IGNORED;
    }
    return RET_IGNORED;
}

fsmReturnStatus clock_min_set(Fsm *fsm, const Event *e)
{
    if (fsm == NULL)
    {
        return RET_IGNORED;
    }
    switch (e->signal)
    {
    case ENTRY:
        scheduler_add(&task6);
        break;
    case ROTARY_PRESSED:
        fsm->timeSet.minute++;
        if (fsm->timeSet.minute >= MIN_IN_AN_HR)
        {
            fsm->timeSet.minute = 0;
        }
        return RET_HANDLED;
    case JOYSTICK_PRESSED:
        scheduler_setTime((fsm->timeSet.hour * MILSEC_TO_HR) + (fsm->timeSet.minute * MILSEC_TO_MIN));
        return TRANSITION(normal_operation);
    case ROTARY_CLOCKWISE:
        fsm->timeSet.minute++;
        if (fsm->timeSet.minute >= MIN_IN_AN_HR)
        {
            fsm->timeSet.minute = 0;
        }
        return RET_HANDLED;
    case ROTARY_COUNTER_CLOCKWISE:
        fsm->timeSet.minute--;
        if (fsm->timeSet.minute == 255)
        {
            fsm->timeSet.minute = MIN_IN_AN_HR - 1;
        }
        return RET_HANDLED;
    case EXIT:
        scheduler_remove(&task6);
        break;
    default:
        return RET_IGNORED;
    }
    return RET_IGNORED;
}

fsmReturnStatus normal_operation(Fsm *fsm, const Event *e)
{
    if (fsm == NULL)
    {
        return RET_IGNORED;
    }
    switch (e->signal)
    {
    case ENTRY:
        scheduler_add(&task7);
        scheduler_add(&task10);
        scheduler_add(&task12);
        if (fsm->isAlarmEnabled)
        {
            led_yellowOn();
        }
        else
        {
            led_yellowOff();
        }
        break;
    case ROTARY_PRESSED:
        fsm->isAlarmEnabled = !fsm->isAlarmEnabled;
        if (fsm->isAlarmEnabled)
        {
            led_yellowOn();
            scheduler_add(&task4);
        }
        else
        {
            led_yellowOff();
            scheduler_remove(&task4);
        }
        return RET_HANDLED;
    case JOYSTICK_PRESSED:
        return TRANSITION(alarm_hr_set);
    case ALARM_MATCH_CLOCK:
        return TRANSITION(alarm_ring);
    case EXIT:
        scheduler_remove(&task4);
        scheduler_remove(&task7);
        break;
    default:
        return RET_IGNORED;
    }
    return RET_IGNORED;
}

fsmReturnStatus alarm_hr_set(Fsm *fsm, const Event *e)
{
    if (fsm == NULL)
    {
        return RET_IGNORED;
    }
    switch (e->signal)
    {
    case ENTRY:
        scheduler_add(&task8);
        break;
    case ROTARY_PRESSED:
        fsm->alarmSet.hour++;
        if (fsm->alarmSet.hour >= HR_IN_A_DAY)
        {
            fsm->alarmSet.hour = 0;
        }
        return RET_HANDLED;
    case JOYSTICK_PRESSED:
        return TRANSITION(alarm_min_set);
    case ROTARY_CLOCKWISE:
        fsm->alarmSet.hour++;
        if (fsm->alarmSet.hour >= HR_IN_A_DAY)
        {
            fsm->alarmSet.hour = 0;
        }
        return RET_HANDLED;
    case ROTARY_COUNTER_CLOCKWISE:
        fsm->alarmSet.hour--;
        if (fsm->alarmSet.hour == 255)
        {
            fsm->alarmSet.hour = HR_IN_A_DAY - 1;
        }
        return RET_HANDLED;
    case EXIT:
        scheduler_remove(&task8);
        break;
    default:
        return RET_IGNORED;
    }
    return RET_IGNORED;
}

fsmReturnStatus alarm_min_set(Fsm *fsm, const Event *e)
{
    if (fsm == NULL)
    {
        return RET_IGNORED;
    }
    switch (e->signal)
    {
    case ENTRY:
        scheduler_add(&task9);
        break;
    case ROTARY_PRESSED:
        fsm->alarmSet.minute++;
        if (fsm->alarmSet.minute >= MIN_IN_AN_HR)
        {
            fsm->alarmSet.minute = 0;
        }
        return RET_HANDLED;
    case JOYSTICK_PRESSED:
        fsm->isAlarmEnabled = true;
        return TRANSITION(normal_operation);
    case ROTARY_CLOCKWISE:
        fsm->alarmSet.minute++;
        if (fsm->alarmSet.minute >= MIN_IN_AN_HR)
        {
            fsm->alarmSet.minute = 0;
        }
        return RET_HANDLED;
    case ROTARY_COUNTER_CLOCKWISE:
        fsm->alarmSet.minute--;
        if (fsm->alarmSet.minute == 255)
        {
            fsm->alarmSet.minute = MIN_IN_AN_HR - 1;
        }
        return RET_HANDLED;
    case EXIT:
        scheduler_remove(&task9);
        scheduler_add(&task4);
        break;
    default:
        return RET_IGNORED;
    }
    return RET_IGNORED;
}

fsmReturnStatus alarm_ring(Fsm *fsm, const Event *e)
{
    if (fsm == NULL)
    {
        return RET_IGNORED;
    }
    switch (e->signal)
    {
    case ENTRY:
        scheduler_add(&task1);
        scheduler_add(&task2);
        scheduler_add(&task3);
        fsm->isAlarmEnabled = false;
        break;
    case ROTARY_PRESSED:
    case JOYSTICK_PRESSED:
    case TIMEOUT_5SEC:
        return TRANSITION(normal_operation);
    case EXIT:
        scheduler_remove(&task1);
        scheduler_remove(&task2);
        scheduler_remove(&task3);
        led_redOff();
        break;
    default:
        return RET_IGNORED;
    }
    return RET_IGNORED;
}

// callback functions declarations
void joystick_pressed_dispatch(void)
{
    alarm_event.signal = JOYSTICK_PRESSED;
    fsm_dispatch(&alarm_clock, &alarm_event);
}

void rotary_pressed_dispatch(void)
{
    alarm_event.signal = ROTARY_PRESSED;
    fsm_dispatch(&alarm_clock, &alarm_event);
}

void rotary_Clockwise_dispatch(void)
{
    if (rotary_isCounterClockwisePressed())
    {
        alarm_event.signal = ROTARY_CLOCKWISE;
        fsm_dispatch(&alarm_clock, &alarm_event);
        fprintf(uartout, "Clockwise\n");
    }
}

void rotary_Counter_Clockwise_dispatch(void)
{
    if (rotary_isClockwisePressed())
    {
        alarm_event.signal = ROTARY_COUNTER_CLOCKWISE;
        fsm_dispatch(&alarm_clock, &alarm_event);
        fprintf(uartout, "Counter Clockwise\n");
    }
}

// tasks callback functions declarations
void timeout_dispatch(void *param)
{
    alarm_event.signal = TIMEOUT_5SEC;
    fsm_dispatch(&alarm_clock, &alarm_event);
}

void alarm_clock_match_dispatch(void *param)
{
    if (alarm_clock.timeSet.hour == alarm_clock.alarmSet.hour && alarm_clock.timeSet.minute == alarm_clock.alarmSet.minute)
    {
        alarm_event.signal = ALARM_MATCH_CLOCK;
        fsm_dispatch(&alarm_clock, &alarm_event);
    }
}

void lcd_clock_set_hr_display(void *param)
{
    static bool hr_show = true;
    lcd_clear();
    lcd_setCursor(5, 0);
    if (hr_show)
    {
        fprintf(lcdout, "Time = %02u:MM", alarm_clock.timeSet.hour);
        fprintf(uartout, "Time = %02u:MM \n", alarm_clock.timeSet.hour);
    }
    else
    {
        fprintf(lcdout, "Time =   :MM");
    }
    lcd_setCursor(3, 1);
    fprintf(lcdout, "Please set hours");
    lcd_setCursor(0, 2);
    fprintf(lcdout, "RB: Change");
    lcd_setCursor(0, 3);
    fprintf(lcdout, "JS: Confirm");
    hr_show = !hr_show;
}

void lcd_clock_set_min_display(void *param)
{
    static bool min_show = true;
    lcd_clear();
    lcd_setCursor(5, 0);
    if (min_show)
    {
        fprintf(lcdout, "Time = %02u:%02u", alarm_clock.timeSet.hour, alarm_clock.timeSet.minute);
        fprintf(uartout, "Time = %02u:%02u \n", alarm_clock.timeSet.hour, alarm_clock.timeSet.minute);
    }
    else
    {
        fprintf(lcdout, "Time = %02u:  ", alarm_clock.timeSet.hour);
    }
    lcd_setCursor(2, 1);
    fprintf(lcdout, "Please set minutes");
    lcd_setCursor(0, 2);
    fprintf(lcdout, "RB: Change");
    lcd_setCursor(0, 3);
    fprintf(lcdout, "JS: Confirm");
    min_show = !min_show;
}

void lcd_alarm_set_hr_display(void *param)
{
    static bool hr_show = true;
    lcd_clear();
    lcd_setCursor(5, 0);
    if (hr_show)
    {
        fprintf(lcdout, "alarm = %02u:MM", alarm_clock.alarmSet.hour);
    }
    else
    {
        fprintf(lcdout, "alarm =   :MM");
    }
    lcd_setCursor(3, 1);
    fprintf(lcdout, "Please set hours");
    lcd_setCursor(3, 2);
    fprintf(lcdout, "Time = %02u:%02u:%02u", alarm_clock.timeSet.hour, alarm_clock.timeSet.minute, alarm_clock.timeSet.second);
    lcd_setCursor(0, 3);
    fprintf(lcdout, "RB:Change");
    lcd_setCursor(10, 3);
    fprintf(lcdout, "JS:Confirm");
    hr_show = !hr_show;
}

void lcd_alarm_set_min_display(void *param)
{
    static bool min_show = true;
    lcd_clear();
    lcd_setCursor(5, 0);
    if (min_show)
    {
        fprintf(lcdout, "alarm = %02u:%02u", alarm_clock.alarmSet.hour, alarm_clock.alarmSet.minute);
    }
    else
    {
        fprintf(lcdout, "alarm = %02u:  ", alarm_clock.alarmSet.hour);
    }
    lcd_setCursor(2, 1);
    fprintf(lcdout, "Please set minutes");
    lcd_setCursor(3, 2);
    fprintf(lcdout, "Time = %02u:%02u:%02u", alarm_clock.timeSet.hour, alarm_clock.timeSet.minute, alarm_clock.timeSet.second);
    lcd_setCursor(0, 3);
    fprintf(lcdout, "RB:Change");
    lcd_setCursor(10, 3);
    fprintf(lcdout, "JS:Confirm");
    min_show = !min_show;
}

void lcd_clock_display(void *param)
{
    lcd_clear();
    lcd_setCursor(3, 0);
    fprintf(lcdout, "Time = %02u:%02u:%02u", alarm_clock.timeSet.hour, alarm_clock.timeSet.minute, alarm_clock.timeSet.second);
    lcd_setCursor(0, 2);
    fprintf(lcdout, "RB: Toggle Alarm");
    lcd_setCursor(0, 3);
    fprintf(lcdout, "JS: Set Alarm");
    //led_greenToggle();
}

void led_toggle(void *param)
{
    uint8_t leds = *((uint8_t *)param);
    if (leds == red)
    {
        led_redToggle();
    }
    else if (leds == green)
    {
        led_greenToggle();
    }
    else if (leds == yellow)
    {
        led_yellowToggle();
    }
}

void udpate_system_time(void *param)
{
    alarm_clock.timeSet = scheduler_calc_time();
}

void alarm_ring_display(void *param)
{
    static bool wake_up_show = true;
    lcd_clear();
    lcd_setCursor(7, 3);
    if (wake_up_show)
    {
        fprintf(lcdout, "WAKE UP!!");
    }
    lcd_setCursor(0, 1);
    fprintf(lcdout, "Time = %02u:%02u:%02u", alarm_clock.timeSet.hour, alarm_clock.timeSet.minute, alarm_clock.timeSet.second);
    wake_up_show = !wake_up_show;
}

void rotary_debounce(void *param)
{
    rotary_checkState();
}