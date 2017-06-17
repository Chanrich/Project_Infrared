/*
 * Global.h
 *
 * Created: 6/16/2017 9:05:19 PM
 *  Author: Rchan
 */ 
#include <stdbool.h>

#ifndef GLOBAL_H_
#define GLOBAL_H_

// Pushbuttons pin
// buttonSelect_PIN: Select button
// buttonNext_PIN: Scroll Down button (Next option)
#define buttonSelect_PIN 8
#define buttonNext_PIN 7
#define BUTTON_DEBOUNCE_DELAY 50

// Declare 8-bit shifter control pin
#define SER_PIN 13	// Serial input pin
#define SRCLK_PIN 9	// Shift register clock
#define RCLK_PIN 10	// Storage register clock

#define HIGH 1
#define LOW 0

// Menu option array
#define NumberOfItems_MenuArray 3

extern char *MenuArray[];
extern char Symbol_Greater_Than;

// Current selected option index
extern int selectedMenuOption;

// Button variable declare
extern int buttonSelect;
extern int buttonSelect_Last_State;
extern unsigned long buttonSelect_Last_Debounce_Time;
extern int buttonNext;
extern int buttonNext_Last_State;
extern unsigned long buttonNext_Last_Debounce_Time;
extern bool buttonSelect_Flag;
extern bool buttonNext_Flag;
extern bool buttonSelect_1_Pulse;
extern bool buttonNext_1_Pulse;

#endif /* GLOBAL_H_ */