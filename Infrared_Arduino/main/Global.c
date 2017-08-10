/*
 * Global.c
 *
 * Created: 6/16/2017 9:43:34 PM
 *  Author: Rchan
 */ 

#include "Global.h"

// Declare state variables
char currentState;

// Declare menu options
char *MenuArray[] = {"Record", "Play", "Clear"};
	
// ASCII number for symbol ">"
char Symbol_Greater_Than = 62;

// Current selected option index
int selectedMenuOptionIndex;

// Button variable declare
int buttonSelect;
int buttonSelect_Last_State;
unsigned long buttonSelect_Last_Debounce_Time;
int buttonNext;
int buttonNext_Last_State;
unsigned long buttonNext_Last_Debounce_Time;
bool buttonSelect_Flag;
bool buttonNext_Flag;
bool buttonSelect_1_Pulse;
bool buttonNext_1_Pulse;

// IR variables
bool infraredValueRead;
unsigned long currentTime;
unsigned long lastIR_Timestamp;
unsigned long initialStartTimestamp;
char last_IR_Read;
bool bStartRecording;
bool bInitialTimestamp;
volatile unsigned char v_interrupt_toggle;
bool bGotSignal;

// Array element 0 will be garbage value (it captures the time from beginning to first signal)
// Array element 1 is the time the signal spent HIGH (infrared will read 0 as it is active low)
// Array element 2 is the time the signal spend LOW (infrared will read 1 as it is active low)
unsigned int switchTimingPattern[PATTERN_TO_RECORD];
unsigned char switchTimingPatternIndex;


