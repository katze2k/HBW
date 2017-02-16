/*
 * HMWRS485.h
 *
 *  Created on: 09.05.2014
 *      Author: thorsten
 *
 * 
 */
//14.01.2017: warte auf busruhe vorm senden hinzugefügt // SendFrame; Recieve und Headerfile
//21.01.2017: Interframespace vollständig implementiert mit Defines


#ifndef HMWRS485_H_
#define HMWRS485_H_

#include "Arduino.h"

#define MAX_RX_FRAME_LENGTH 64

#define USE_INTERFRAMESPACE
#ifdef USE_INTERFRAMESPACE
// 1/19200 * 64 * 10 = 33ms (maximale frame länge)
//TODO die funktion ist noch blockierend
// Raspberry braucht ca 0.5ms zum umschalten => 1ms würde reichen, jedoch laut protokoll 7.5ms
#define IFS_SHORTTIME 3 //zeit in ms vor ACK gesendet wird
#define IFS_NORMALTIME 5 //zeit in ms vor normaler frame gesendet wird
#endif

//TODO Device Relations einfügen (direct-link) für peering

// Abstrakte Basisklasse mit Callbacks fuer Verwender
class HMWModuleBase {
  public:
// processEvent wird nur aufgerufen, wenn es fuer das Modul was zu tun gibt
// Also nicht fuer ACKs oder duplicates
// TODO: Should return whether an ACK is needed or not
	virtual void processEvent(byte const * const frameData, byte frameDataLength, boolean isBroadcast = false) = 0;  // Data, broadcast-Flag
};


class HMWRS485 {
public:
	HMWRS485(Stream*, byte);  // RS485 interface, TX-Enable Pin
	virtual ~HMWRS485();

	void loop(); // main loop, die immer wieder aufgerufen werden muss

	// sendFrame macht ggf. Wiederholungen
	void sendFrame();
	void sendAck();  // ACK fuer gerade verarbeitete Message senden

	// Modul-Definition, wird vom Modul selbst gesetzt
	// TODO: Ist das gutes Design?
   HMWModuleBase* module;

    // Senderadresse beim Empfangen
    // TODO: Das sollte private sein, wird aber momentan noch vom Modul verwendet
	unsigned long senderAddress;

	// Senden
	unsigned long txSenderAddress;       // eigene Adresse
	unsigned long txTargetAddress;        // Adresse des Moduls
	byte txFrameControlByte;
    byte txFrameDataLength;              // Laenge der Daten + Checksum
	byte txFrameData[MAX_RX_FRAME_LENGTH];

private:
#ifdef USE_INTERFRAMESPACE
	unsigned long timelastreceive; // timestamp (millis) des letzten empfangenen byte
#endif
// Das eigentliche RS485-Interface, kann SoftSerial oder (Hardware)Serial sein
	Stream* serial;
// Pin-Nummer fuer "TX-Enable"
	byte txEnablePin;
	// Empfangs-Status
	byte frameStatus;
// Empfangene Daten
	// Empfangen
	byte frameComplete;
    unsigned long targetAddress;
	byte frameDataLength;                 // Laenge der Daten
	byte frameData[MAX_RX_FRAME_LENGTH];
	byte startByte;
	byte frameControlByte;

	// Sende-Versuch, wird ggf. wiederholt
	void receive();  // wird zyklisch aufgerufen
	boolean parseFrame();

	void sendFrameSingle();
	void sendFrameByte(byte);
	unsigned int crc16Shift(byte, unsigned int);
};

#endif /* HMWRS485_H_ */
