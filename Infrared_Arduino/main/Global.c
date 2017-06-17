/*
 * Global.c
 *
 * Created: 6/16/2017 9:43:34 PM
 *  Author: Rchan
 */ 

#include "Global.h"

// Current selected option index
int selectedMenuOption;

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
