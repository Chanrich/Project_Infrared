/*
 * Global.h
 *
 * Created: 6/16/2017 9:05:19 PM
 *  Author: Rchan
 */ 

#include <stdbool.h>


#ifndef GLOBAL_H_
#define GLOBAL_H_

// buttonSelect_PIN: Select button
#define buttonSelect_PIN 8

// buttonNext_PIN: Scroll Down button (Next option)
#define buttonNext_PIN 7
#define BUTTON_DEBOUNCE_DELAY 50

// Declare 8-bit shifter control pin
#define SER_PIN 13	// Serial input pin
#define SRCLK_PIN A5	// Shift register clock // Change pin from 9 to 1
#define RCLK_PIN 10	// Storage register clock

// Define IR LED pins - register level
#define IR_LED_PORT PORTB
#define IR_LED_PORT_PIN PB1
#define IR_LED_Digital_Pin 9
#define OCIEA 1
// Define IR receiver pins - register level
#define IR_Receive_PORT PORTD
#define IR_Receive_PIN PIND
#define IR_Receive_DDR DDRD
#define IR_Receive_PORT_PIN PD6

#define HIGH 1
#define LOW 0

// Menu option array
#define NumberOfItems_MenuArray 3

// Infrared parameters
#define PATTERN_TO_RECORD 100

// Define States:
// 0: Idle
//	During Idle state, the micro-controller awaits for inputs from push buttons
// 1: Recording
//	Record feature is triggered
//	Record infrared signal.
// 2: Play
//	Play feature is triggered
//	Playback recorded signal through IR led
// 3: Clear
//	Clear feature is triggered
//	Delete all recorded signal

// Declare state variables
extern char currentState;

// Array element 0 is the time the signal spent HIGH (infrared will read 0 as it is active low)
// Array element 1 is the time the signal spend LOW (infrared will read 1 as it is active low)
extern unsigned char switchTimingPatternIndex;
extern unsigned int switchTimingPattern[PATTERN_TO_RECORD];

// Declare char array to store labels for menu options
extern char *MenuArray[];
// Store ASCII number for symbol Greater Than
extern char Symbol_Greater_Than;

// Index of currently selected option
extern int selectedMenuOptionIndex;

// Button variables, de-bounce logics
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

// Infrared receive signals
extern bool infraredValueRead;
extern unsigned long currentTime;
extern unsigned long lastIR_Timestamp;
extern unsigned long initialStartTimestamp;
extern bool bStartRecording;
extern bool bInitialTimestamp;
extern char last_IR_Read;

// A toggle flag to flip signal for during interrupt
extern volatile unsigned char v_interrupt_toggle;

// A flag to detect if signal is received during recording
extern bool bGotSignal;

#endif /* GLOBAL_H_ */