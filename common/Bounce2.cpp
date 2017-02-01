// Please read Bounce2.h for information about the liscence and authors

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "Bounce2.h"

#define DEBOUNCED_STATE 0
#define UNSTABLE_STATE  1
#define STATE_CHANGED   4
//TODO was ist mit 2,3 ?
// 4 - 7 ist für BOUNCE_AUTOMOTIVE

//TODO array möglichkeit um mehrere buttons mit einem interval abzufragen
Bounce::Bounce()
    : previous_millis(0)
    , interval_millis(5)
    , state(0)
    , pin(0)
{}

void Bounce::init(bool initialState) {
	state = 0;
	if (initialState) {
		state = _BV(DEBOUNCED_STATE) | _BV(UNSTABLE_STATE);
	}
#ifdef BOUNCE_LOCK_OUT
	previous_millis = 0;
#else
	previous_millis = millis();
#endif
}


void Bounce::attach(int pin) {
	this->pin = pin;
	init(digitalRead(pin));
}
/*	state = 0;
    if (digitalRead(pin)) {
        state = _BV(DEBOUNCED_STATE) | _BV(UNSTABLE_STATE);
    }
#ifdef BOUNCE_LOCK_OUT
    previous_millis = 0;
#else
    previous_millis = millis();
#endif
}*/

void Bounce::attach(int pin, int mode){
  pinMode(pin, mode);
  
  this->attach(pin);
  init(digitalRead(pin));
}

void Bounce::interval(uint16_t interval_millis)
{
    this->interval_millis = interval_millis;
}

void Bounce::attachWithInterval(int pin, int mode, uint16_t interval_millis)
{
  pinMode(pin, mode);
  this->attach(pin);
  this->interval_millis = interval_millis;
  init(digitalRead(pin));
}

bool Bounce::update(bool currentState)
{
#ifdef BOUNCE_LOCK_OUT
    state &= ~_BV(STATE_CHANGED);
    // Ignore everything if we are locked out
    if ((unsigned long)(millis() - previous_millis) >= interval_millis) {
        //bool currentState = digitalRead(pin);
        if ((bool)(state & _BV(DEBOUNCED_STATE)) != currentState) {
            previous_millis = millis();
            state ^= _BV(DEBOUNCED_STATE);
            state |= _BV(STATE_CHANGED);
        }
    }
    return state & _BV(STATE_CHANGED);

#elif defined BOUNCE_WITH_PROMPT_DETECTION
    // Read the state of the switch port into a temporary variable.
    //bool readState = digitalRead(pin); //replace readState with currentState

    // Clear Changed State Flag - will be reset if we confirm a button state change.
    state &= ~_BV(STATE_CHANGED);

    if (currentState != (bool)(state & _BV(DEBOUNCED_STATE))) {
      // We have seen a change from the current button state.

      if ((unsigned long)(millis() - previous_millis) >= interval_millis ) {
	// We have passed the time threshold, so a new change of state is allowed.
	// set the STATE_CHANGED flag and the new DEBOUNCED_STATE.
	// This will be prompt as long as there has been greater than interval_misllis ms since last change of input.
	// Otherwise debounced state will not change again until bouncing is stable for the timeout period.
	state ^= _BV(DEBOUNCED_STATE);
	state |= _BV(STATE_CHANGED);
      }
    }

    // If the readState is different from previous readState, reset the debounce timer - as input is still unstable
    // and we want to prevent new button state changes until the previous one has remained stable for the timeout.
    if (currentState != (bool)(state & _BV(UNSTABLE_STATE)) ) {
	// Update Unstable Bit to macth readState
        state ^= _BV(UNSTABLE_STATE);
        previous_millis = millis();
    }
    // return just the sate changed bit
    return state & _BV(STATE_CHANGED);
#elif defined BOUNCE_AUTOMOTIVE
	// Clear Changed State Flag - will be reset if we confirm a button state change.
	uint8_t ones, tmp;
	state &= ~_BV(STATE_CHANGED);
	if ((unsigned long)(millis() - previous_millis) >= interval_millis) {
		previous_millis = millis();
		state = ((state & 0xF0) << 1) | currentState << 4 | (state & 0x0F)
		//count ones
		tmp = state>>4;
		ones = 0;
		while (tmp)
		{
			ones += tmp & 0x01;
		}
		if (ones >= BOUNCE_A_TIMES_ON) && !(state & _BV(DEBOUNCED_STATE) { 
			state |= _BV(STATE_CHANGED);
			state |= _BV(DEBOUNCED_STATE);
		}
		else if((4 - ones) >= BOUNCE_A_TIMES_OFF) && (state & _BV(DEBOUNCED_STATE) {
			state |= _BV(STATE_CHANGED);
			state &= !_BV(DEBOUNCED_STATE);
		}
	}
	return state & _BV(STATE_CHANGED);
#else
    // Read the state of the switch in a temporary variable.
    //bool currentState = digitalRead(pin);
    state &= ~_BV(STATE_CHANGED);

    // If the reading is different from last reading, reset the debounce counter
    if ( currentState != (bool)(state & _BV(UNSTABLE_STATE)) ) {
        previous_millis = millis();
        state ^= _BV(UNSTABLE_STATE);
    } else
        if ((unsigned long)(millis() - previous_millis) >= interval_millis ) {
            // We have passed the threshold time, so the input is now stable
            // If it is different from last state, set the STATE_CHANGED flag
            if ((bool)(state & _BV(DEBOUNCED_STATE)) != currentState) {
                previous_millis = millis();
                state ^= _BV(DEBOUNCED_STATE);
                state |= _BV(STATE_CHANGED);
            }
        }

    return state & _BV(STATE_CHANGED);
#endif
}

bool Bounce::update()
{
	return update(digitalRead(pin));
}

bool Bounce::read() //returns Debounced Value
{
    return state & _BV(DEBOUNCED_STATE);
}

bool Bounce::rose() //returns true on rise event (changed && 1)
{
    return ( state & _BV(DEBOUNCED_STATE) ) && ( state & _BV(STATE_CHANGED));
}

bool Bounce::fell() //returns true on falling event (changed && 0)
{
    return !( state & _BV(DEBOUNCED_STATE) ) && ( state & _BV(STATE_CHANGED));
}
