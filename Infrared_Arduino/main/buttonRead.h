// buttonRead.h
#include "Global.h"

#ifndef _BUTTONREAD_h
#define _BUTTONREAD_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

extern void DebounceReadButton();

#endif

