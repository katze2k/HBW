/*
 * HMWDebug.h
 *
 *  Created on: 20.07.2014
 *      Author: thorsten
 */

#ifndef HMWDEBUG_H_
#define HMWDEBUG_H_

//TODO bei DEBUG_NONE sollten alle hmwdebugs ersetzt werden.....
//#if DEBUG_VERSION == DEBUG_NONE
// #define hmwdebug(x) hmwdebug2 x
//#else
//#define hmwdebug(x)
//#endif

#include "Arduino.h"

extern Stream* hmwdebugstream;

template <typename T>
void hmwdebug(T msg) { if(hmwdebugstream) hmwdebugstream->print(msg); };

template <typename T>
void hmwdebug(T msg, int base) { if(hmwdebugstream) hmwdebugstream->print(msg, base); };


#endif /* HMWDEBUG_H_ */
