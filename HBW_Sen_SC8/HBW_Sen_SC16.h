// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef _HMWOneWire_H_
#define _HMWOneWire_H_
#include "Arduino.h"
//add your includes for the project HMWHomebrew here

// Version: //TODO Bump Version on Release
#define DEVICE_FIRMWARE_VERSION 0x01
/********************************/
/* Pinbelegung:         */
/********************************/

#define BUTTON A7            // Das Bedienelement für factory reset....
#define RS485_TXEN 2        // Transmit-Enable Pin

// Input Buttons
#define In1 A0
#define In2 A1
#define In3 A2
#define In4 A3
#define In5 A4
#define In6 A5
#define In7 12
#define In8 11
#define In9 10
#define In10 9
#define In11 8
#define In12 7
#define In13 6
#define In14 5
#define In15 4
#define In16 3

#define MODULE_ID 0x86

#define CHANNEL_IO_COUNT 16
#define CHANNEL_IO_BYTES 2 //CHANNEL_IO_COUNT / 8


#define DEBUG_NONE 0   // Kein Debug-Ausgang, RS485 auf Hardware-Serial
#define DEBUG_UNO 1    // Hardware-Serial ist Debug-Ausgang, RS485 per Soft auf pins 5/6
#define DEBUG_UNIV 2   // Hardware-Serial ist RS485, Debug per Soft auf pins 5/6

#define DEBUG_VERSION DEBUG_NONE //UNO

#if DEBUG_VERSION == DEBUG_UNO
#define RS485_RXD 4
#define RS485_TXD 3
#define LED 13        // Signal-LED
#elif DEBUG_VERSION == DEBUG_UNIV
#define DEBUG_RXD 5
#define DEBUG_TXD 6
#define LED 13
#else
#define LED 13         // Signal-LED
#endif

// Konfigurationsdaten als C++-Struktur
// Anzahl Tastereingaenge
#define HMW_CONFIG_NUM_KEYS CHANNEL_IO_COUNT

// Taster
struct hmw_config_key {
	byte input_locked;   		// (0) 0x07 1=LOCKED, 0=UNLOCKED
	byte inverted;				// (1) 0x08
	byte pullup;				// (2) 0x09
	byte long_press_time;       // (3) 0x0A
	byte multiclick_time;		// (4) 0x0B
	byte doubleclickaction;		// (5) 0x0C
	byte longclickaction;		// (6) 0x0D
};

struct hmw_config {
	//addr 0x00 is not used !!!!
	byte logging_time;     // 0x01
	uint32_t central_address;  // 0x02 - 0x05
	byte direct_link_deactivate ;   // 0x06:0
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
