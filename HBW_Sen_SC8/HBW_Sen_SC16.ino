//*******************************************************************
//
// HBW-Sen-SC8.cpp
//
// Homematic Wired Hombrew Hardware
// Arduino Nano als Homematic-Device
// HBW-Sen-SC8 zum Einlesen von 8 Tastern
// - Active HIGH oder LOW kann �ber das FHEM-Webfrontend konfiguriert werden
// - Pullup kann �ber das FHEM-Webfrontend aktiviert werden
// - Erkennung von Doppelklicks
// - Zus�tzliches Event beim Loslassen einer lang gedr�ckten Taste
//TODO: Softserial wird noch verwendet ???
//TODO auslastung über pin toogeln ermitteln.
//TODO watchdog hinzufügen ? was ist mit bootloader?
//*******************************************************************

#include "HBW_Sen_SC8.h"

#if DEBUG_VERSION == DEBUG_UNO || DEBUG_VERSION == DEBUG_UNIV
  #include <SoftwareSerial.h>	  // TODO: Eigene SoftwareSerial
#endif

#include "HMWDebug.h"	// debug-Funktion
#include <EEPROM.h>

#include "HMWRS485.h"	// HM Wired Protokoll
#include "HMWModule.h"	// default module methods
#include "Bounce2.h"	// Input Module for setup/default Key

#include "ClickButton.h"	// Input Module for Buttons

// Das folgende Define kann benutzt werden, wenn ueber die Kanaele "geloopt" werden soll
// als Define, damit es zentral definiert werden kann, aber keinen (globalen) Speicherplatz braucht
#define CHANNEL_PORTS byte channelPorts[CHANNEL_IO_COUNT] = {In1, In2, In3, In4, In5, In6, In7, In8, In9, In10, In11, In12, In13, In14, In15, In16};

// Port Status, d.h. Port ist auf 0 oder 1
//byte portStatus[CHANNEL_IO_BYTES]; nicht benutzt ?


#if DEBUG_VERSION == DEBUG_UNO
 SoftwareSerial rs485(RS485_RXD, RS485_TXD); // RX, TX
 HMWRS485 hmwrs485(&rs485, RS485_TXEN);
#elif DEBUG_VERSION == DEBUG_UNIV
 HMWRS485 hmwrs485(&Serial, RS485_TXEN);
#else
 HMWRS485 hmwrs485(&Serial, RS485_TXEN);  // keine Debug-Ausgaben
#endif
HMWModule* hmwmodule;   // wird in setup initialisiert

hmw_config config;

// Instantiate ClickButton objects in an array
ClickButton button[HMW_CONFIG_NUM_KEYS] = {
  ClickButton (In1, LOW, CLICKBTN_PULLUP),
  ClickButton (In2, LOW, CLICKBTN_PULLUP),
  ClickButton (In3, LOW, CLICKBTN_PULLUP),
  ClickButton (In4, LOW, CLICKBTN_PULLUP),
  ClickButton (In5, LOW, CLICKBTN_PULLUP),
  ClickButton (In6, LOW, CLICKBTN_PULLUP),
  ClickButton (In7, LOW, CLICKBTN_PULLUP),
  ClickButton (In8, LOW, CLICKBTN_PULLUP),
  ClickButton (In9, LOW, CLICKBTN_PULLUP),
  ClickButton(In10, LOW, CLICKBTN_PULLUP),
  ClickButton(In11, LOW, CLICKBTN_PULLUP),
  ClickButton(In12, LOW, CLICKBTN_PULLUP),
  ClickButton(In13, LOW, CLICKBTN_PULLUP),
  ClickButton(In14, LOW, CLICKBTN_PULLUP),
  ClickButton(In15, LOW, CLICKBTN_PULLUP),
  ClickButton(In16, LOW, CLICKBTN_PULLUP),
};

Bounce cfgbutton;

// write complete config to EEPROM in a hopefully smart way
void writeConfig(){
  byte* ptr;
  ptr = (byte*)(&config);
  for(unsigned int address = 0x01; address <= sizeof(config); address++){
    hmwmodule->writeEEPROM(address, *ptr);
    ptr++;
    };
};

void setDefaults(){
  // defaults setzen
  if(config.logging_time == 0xFF) config.logging_time = 20;
  if(config.central_address == 0xFFFFFFFF) config.central_address = 0x00000001;
  for(byte channel = 0; channel < HMW_CONFIG_NUM_KEYS; channel++){
	  if(config.keys[channel].long_press_time == 0xFF) config.keys[channel].long_press_time = 0x32; // 50 => 500ms with *10
	  if(config.keys[channel].input_locked == 0xFF) config.keys[channel].input_locked = 0; // not locked
	  if(config.keys[channel].inverted == 0xFF) config.keys[channel].inverted = 1; // inverted
	  if(config.keys[channel].pullup == 0xFF) config.keys[channel].pullup = 1; // with Pullup
	  if(config.keys[channel].multiclick_time == 0xFF) config.keys[channel].multiclick_time = 125; // 125 => 250 ms with *2
	  if(config.keys[channel].longclickaction == 0xFF) config.keys[channel].longclickaction = 0 ; // default action
	  if(config.keys[channel].doubleclickaction == 0xFF) config.keys[channel].doubleclickaction = 0; //default action
  }
};


// Klasse fuer Callbacks vom Protokoll
class HMWDevice : public HMWDeviceBase {
  public:
  void setLevel(byte channel,unsigned int level) {
        return;  // there is nothing to set
  }
  unsigned int getLevel(byte channel) {
    // everything in the right limits?
    return 0;
  };

  void readConfig(){
    byte* ptr;
	// EEPROM lesen
	hmwdebug(F("read config from E2\n"));
	ptr = (byte*)(&config);
    for(unsigned int address = 0; address < sizeof(config); address++){
	  *ptr = EEPROM.read(address + 0x01);
	  hmwdebug(F("Addr:"));
	  hmwdebug(address + 1, HEX);
	  hmwdebug(F(" Val:"));
	  hmwdebug(*ptr, HEX);
	  hmwdebug(F("\n"));
      ptr++;
    };

    // defaults setzen, falls nicht sowieso klar
     setDefaults();

     CHANNEL_PORTS

     for (int i=0; i < HMW_CONFIG_NUM_KEYS; i++) {
       // Setup button timers (all in milliseconds / ms)
       // (These are default if not set, but changeable for convenience)
       button[i] = ClickButton (channelPorts[i], ! config.keys[i].inverted, config.keys[i].pullup);
       button[i].debounceTime  = 20;   // Debounce timer in ms
       button[i].multiclickTime = config.keys[i].multiclick_time * 2;  // Time limit for multi clicks
       button[i].longClickTime  = config.keys[i].long_press_time * 10; // Time until long clicks register // 255 => 2550ms (2.5s)
     }
  };
};

// The device will be created in setup()
HMWDevice hmwdevice;

void factoryReset() {
  // writes FF into config
  for(unsigned int i=0; i < sizeof(config); i++) {
    hmwmodule->writeEEPROM(i, 0xFF, false);
  }
  // set defaults
  setDefaults();
}

void handleButton() {
  // langer Tastendruck (5s) -> LED blinkt hektisch (vorher leuchtet die LED solange gedrückt)
  // dann innerhalb 10s langer Tastendruck (3s) ->  LED blinkt langsam
  // loslassen -> LED geht aus, EEPROM-Reset
	static unsigned long  lastTime;
  static byte status = 0;  // 0: normal, 1: Taste erstes mal gedr�ckt, 2: erster langer Druck erkannt
                           // 3: Warte auf zweiten Tastendruck, 4: Taste zweites Mal gedr�ckt
                           // 5: zweiter langer Druck erkannt

  if (cfgbutton.istimetoupdate()) {
	  cfgbutton.update(analogRead(BUTTON) < (int16_t)512); } //10 bit wandler, pullup => ADC<512 ist gedrückt


  switch(status) {
    case 0:
      if(cfgbutton.read()) {status = 1;  hmwdebug(status);}
      lastTime = millis();
      break;
    case 1: //taste wurde das 1. mal gedrückt
      if(cfgbutton.read()) {   // immer noch gedrueckt
        if(millis() - lastTime > 5000) {status = 2;   hmwdebug(status);}
	  }
	  else {              // nicht mehr gedr�ckt
	 // determine sensors and send announce on short press
		  hmwmodule->broadcastAnnounce(0);
		  status = 0;
		  hmwdebug(status);
	  }
      break;
    case 2: // taste nach 5s losgelassen
      if(!cfgbutton.read()) {  // losgelassen
      status = 3;
      hmwdebug(status);
      lastTime = millis();
      };
      break;
    case 3: //taste 2. mal gedrückt
      if(cfgbutton.read()) {   // zweiter Tastendruck
      status = 4;
      hmwdebug(status);
      lastTime = millis();
      }else{              // noch nicht gedrueckt
      if(millis() - lastTime > 10000) {status = 0;   hmwdebug(status);}   // give up
      };
      break;
    case 4: // immernoch gedrückt
      if(cfgbutton.read()) {   // immer noch gedrueckt
        if(millis() - lastTime > 3000) {status = 5;  hmwdebug(status);}
      }else{              // nicht mehr gedr�ckt
        status = 0;
        hmwdebug(status);
      };
      break;
    case 5:   // zweiter Druck erkannt
      if(!cfgbutton.read()) {    //erst wenn losgelassen
      // Factory-Reset          !!!!!!  TODO: Gehoert das ins Modul?
      factoryReset();
      hmwmodule->setNewId();
      status = 0;
      hmwdebug(status);
      }
      break;
	default:
		status = 0;
		break;
  }

  // control LED
  static long lastLEDtime = 0;
  switch(status) {
    case 0:
      digitalWrite(LED, LOW);
      break;
    case 1:
      digitalWrite(LED, HIGH);
      break;
    case 2:
    case 3:
    case 4:
      if(millis() - lastLEDtime > 100) {  // schnelles Blinken
      digitalWrite(LED,!digitalRead(LED));
      lastLEDtime = millis();
      };
      break;
    case 5:
      if(millis() - lastLEDtime > 750) {  // langsames Blinken
        digitalWrite(LED,!digitalRead(LED));
        lastLEDtime = millis();
      };
  }
};

#if DEBUG_VERSION != DEBUG_NONE
void printChannelConf(){
  for(byte channel = 0; channel < HMW_CONFIG_NUM_KEYS; channel++) {
    hmwdebug("Channel     : "); hmwdebug(channel); hmwdebug("\r\n");
    hmwdebug("\r\n");
  }
}
#endif

// Tasten
void handleKeys() {

  static unsigned long lastSentLong[HMW_CONFIG_NUM_KEYS] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
  long now = millis();
  static int keyState[HMW_CONFIG_NUM_KEYS];
  static byte keyPressNum[HMW_CONFIG_NUM_KEYS] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

  for (int i =  0; i < HMW_CONFIG_NUM_KEYS; i++) {
  if(!config.keys[i].input_locked){ //do this only if NOT locked
    button[i].Update();

    if (button[i].clicks != 0) {
      keyState[i] = button[i].clicks;

      if (button[i].clicks == 1) { // Einfachklick
        keyPressNum[i]++;
        hmwmodule->broadcastKeyEvent(i,keyPressNum[i], 2); // Taste gedr�ckt
        }
      if (button[i].clicks >= 2) {  // Mehrfachklick
		  switch (config.keys[i].doubleclickaction) {
			case 0: //double click
				keyPressNum[i]++;
				hmwmodule->broadcastKeyEvent(i, keyPressNum[i], 1); // Taste gedr�ckt
				break;
			case 1: // send n times normal press
				for (int j = 0; j < button[i].clicks; j++) {
					keyPressNum[i]++;
					hmwmodule->broadcastKeyEvent(i, keyPressNum[i], 2); // Taste gedr�ckt					
				}
				break;
			case 2: // send one time normal press
				keyPressNum[i]++;
				hmwmodule->broadcastKeyEvent(i, keyPressNum[i], 2); // Taste gedr�ckt					
				break;
			case 3: // send nothing
				break;
		}
      }
      else if (button[i].clicks < 0) {  // erstes LONG
		keyPressNum[i]++;
		lastSentLong[i] = millis();
		switch (config.keys[i].longclickaction) {
		case 0: // long_hold_release
			hmwmodule->broadcastKeyEvent(i, keyPressNum[i], 3);
			break;
		case 1: // normal_hold_release
			hmwmodule->broadcastKeyEvent(i, keyPressNum[i], 2);
			break;
		case 2: // normal
			hmwmodule->broadcastKeyEvent(i, keyPressNum[i], 2);
			break;
		case 3: // normal__normal
			hmwmodule->broadcastKeyEvent(i, keyPressNum[i], 2);
			break;
		case 4: // normal+release
			hmwmodule->broadcastKeyEvent(i, keyPressNum[i], 2);
			break;
		case 5:	// nothing
			keyPressNum[i]--;
			break;
		} 
      }
    } //else if  von if(clicks !=0
	else if (keyState[i] < 0) {   // Taste ist oder war lang gedr�ckt
		if (button[i].depressed == true) {  // ist noch immer gedr�ckt --> alle 300ms senden
			if ((now - lastSentLong[i] >= 300) && (config.keys[i].longclickaction < 2)) { // alle 300ms erneut senden
				lastSentLong[i] = now ? now : 1;
				hmwmodule->broadcastKeyEvent(i, keyPressNum[i], 3);
			}
		}
		else {    // "Losgelassen" senden
			// Taste wurde vorher lange gehalten (-1, -2 oder -3)
			switch (config.keys[i].longclickaction) {
			case 0:  //long_hold_release
				hmwmodule->broadcastKeyEvent(i, keyPressNum[i], 0);
				break;
			case 1: //normal_hold_release
				hmwmodule->broadcastKeyEvent(i, keyPressNum[i], 0);
				break;
			case 2: //normal
				break;
			case 3: //normal_normal
				keyPressNum[i]++;
				hmwmodule->broadcastKeyEvent(i, keyPressNum[i], 2);
				break;
			case 4: //normal_release
				hmwmodule->broadcastKeyEvent(i, keyPressNum[i], 0);
				break;
			case 5:
				break;
			}
			lastSentLong[i] = 0;
			keyState[i] = 0;
			//  und 0 und 1 sind key sim events - über befehlsbyte noch zu unterscheiden
			// 2 bit nur möglich -- 0...3
			// 0 = losgelassen (long)
			// 1 = double press
			// 2 = short press
			// 3 = long press
		}
	  }
	}
  }
}


void setup()
{
#if DEBUG_VERSION == DEBUG_UNO
  pinMode(RS485_RXD, INPUT);
  pinMode(RS485_TXD, OUTPUT);
#elif DEBUG_VERSION == DEBUG_UNIV
  pinMode(DEBUG_RXD, INPUT);
  pinMode(DEBUG_TXD, OUTPUT);
#endif
  pinMode(RS485_TXEN, OUTPUT);
  digitalWrite(RS485_TXEN, LOW);

  pinMode(BUTTON, INPUT);
  analogReference(DEFAULT);
  cfgbutton.init(0);
  cfgbutton.interval(5);
  pinMode(LED, OUTPUT);

#if DEBUG_VERSION == DEBUG_UNO
   hmwdebugstream = &Serial;
   Serial.begin(19200);
   rs485.begin(19200);    // RS485 via SoftwareSerial
#elif DEBUG_VERSION == DEBUG_UNIV
   SoftwareSerial* debugSerial = new SoftwareSerial(DEBUG_RXD, DEBUG_TXD);
   debugSerial->begin(19200);
   hmwdebugstream = debugSerial;
   Serial.begin(19200, SERIAL_8E1);
#else
   Serial.begin(19200, SERIAL_8E1);
#endif

   hmwdebug(F("Booting..."));
   // config aus EEPROM lesen
   hmwdevice.readConfig();

   hmwmodule = new HMWModule(&hmwdevice, &hmwrs485, MODULE_ID, DEVICE_FIRMWARE_VERSION);

    // send announce message
  hmwmodule->broadcastAnnounce(0);
#if DEBUG_VERSION != DEBUG_NONE
  printChannelConf();
#endif
}

// The loop function is called in an endless loop
void loop()
{
  // Daten empfangen und alles, was zur Kommunikationsschicht geh�rt
  // processEvent vom Modul wird als Callback aufgerufen
  hmwrs485.loop();

  // Bedienung ueber Button (Factory Reset ....)
  handleButton();

  // Tasten
  handleKeys();

};