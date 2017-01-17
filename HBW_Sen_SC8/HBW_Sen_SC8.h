// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef _HMWOneWire_H_
#define _HMWOneWire_H_
#include "Arduino.h"
//add your includes for the project HMWHomebrew here

// Konfigurationsdaten als C++-Struktur



// Anzahl Tastereingaenge
#define HMW_CONFIG_NUM_KEYS 8

// Taster
struct hmw_config_key {
	byte input_locked;   		// 0x07:1    0=LOCKED, 1=UNLOCKED
	byte inverted;
	byte pullup;
	byte long_press_time;       // 0x08
};

struct hmw_config {
	byte logging_time;     // 0x01
	long central_address;  // 0x02 - 0x05
	byte direct_link_deactivate : 1;   // 0x06:0
	byte : 7;   // 0x06:1-7
		   hmw_config_key keys[HMW_CONFIG_NUM_KEYS];

};



//end of add your includes here
#ifdef __cplusplus
extern "C" {
#endif
void loop();
void setup();
#ifdef __cplusplus
} // extern "C"
#endif

//add your function definitions for the project HMWHomebrew here

//Do not add code below this line
#endif /* _HMWOneWire_H_ */
