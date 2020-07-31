#ifndef ALARM_FSM_H_
#define ALARM_FSM_H_

/* INCLUDES ***********************************************************/
#include "ses_button.h"
#include "ses_scheduler.h"
#include "ses_lcd.h"
#include "ses_led.h"
#include "ses_uart.h"
#include "ses_rotary.h"

/* TYPE DEFINES ***********************************************************/
typedef struct fsm_s Fsm;                               //< typedef for alarm clock state machine
typedef struct event_s Event;                           //< event type for alarm clock fsm
typedef uint8_t fsmReturnStatus;                        //< typedef to be used with above enum
typedef fsmReturnStatus (*State)(Fsm *, const Event *); //<typedef for state event handler functions

/* ENUMS ***********************************************************/
/** Enum for possible events*/
enum
{
    ENTRY,                   //< state enter event
    EXIT,                    //< state exit event
    ROTARY_PRESSED,          //< rotary button is pressed
    JOYSTICK_PRESSED,        //< joystick button is pressed
    TIMEOUT_5SEC,            //< a timer of 5 seconds expires
    ALARM_MATCH_CLOCK,       //< clock matches alarm set time
    ROTARY_CLOCKWISE,        //< rotary rotated clockwise
    ROTARY_COUNTER_CLOCKWISE //< rotary rotated counter-clockwise
};

/** FSM event return values */
enum
{
    RET_HANDLED,   //< event was handled
    RET_IGNORED,   //< event was ignored; not used in this implementation
    RET_TRANSITION //< event was handled and a state transition occurred
};

/* STRUCTS ***********************************************************/
struct fsm_s
{
    State state;         //< current state, pointer to event handler
    bool isAlarmEnabled; //< flag for the alarm status
    time_t timeSet;      //< multi-purpose var for system time
    time_t alarmSet;     //< multi-purpose var for alarm time
};

struct event_s
{
    uint8_t signal; //< identifies the type of event
};

/* dispatches events to state machine, called in application*/
inline static void fsm_dispatch(Fsm *fsm, const Event *event)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        static Event entryEvent = {.signal = ENTRY};
        static Event exitEvent = {.signal = EXIT};
        State s = fsm->state;
        fsmReturnStatus r = fsm->state(fsm, event);
        if (r == RET_TRANSITION)
        {
            s(fsm, &exitEvent);           //< call exit action of last state
            fsm->state(fsm, &entryEvent); //< call entry action of new state
        }
    }
}
/* sets and calls initial state of state machine */
inline static void fsm_init(Fsm *fsm, State init)
{
    //... other initialization
    Event entryEvent = {.signal = ENTRY};
    fsm->state = init;
    fsm->state(fsm, &entryEvent);
}

#endif /* ALARM_FSM_H_ */
