#include "Global.h"
//#include "buttonRead.h"

// Declare empty main function other Atmel Studio Compiler doesn't recognize other functions within this file
void main(){
	
}

void DebounceReadButton(){
	// Read current state
	int currentRead_A = digitalRead(buttonSelect_PIN);
	int currentRead_B = digitalRead(buttonNext_PIN);

	// If button A is pressed or released
	if (buttonSelect_Last_State != currentRead_A && buttonSelect_Flag == 0){
		// Record current time
		buttonSelect_Last_Debounce_Time = millis();
		
		// Set flag to prevent program from entering again
		buttonSelect_Flag = 1;
	}
	// If button B is pressed or released
	if (buttonNext_Last_State != currentRead_B && buttonNext_Flag ==0){
		// Record current time
		buttonNext_Last_Debounce_Time = millis();
		
		// Set flag to prevent program from entering again
		buttonNext_Flag = 1;
	}
	
	// Clear Button A if 1 pulse flag is set
	if (buttonSelect_1_Pulse){
		buttonSelect = 0;
	}
	
	// Clear Button B if 1 pulse flag is set
	if (buttonNext_1_Pulse){
		buttonNext = 0;
	}

	// When time specified by Debounce delay time elapsed, check button status again and determine if button is pressed or not
	if ((millis() - buttonSelect_Last_Debounce_Time) > BUTTON_DEBOUNCE_DELAY){
		// If the button state stay unchanged
		if (currentRead_A == HIGH && buttonSelect_1_Pulse == 0){
			// Hold Button A to high state
			buttonSelect = 1;
			
			// Set 1 pulse flag
			buttonSelect_1_Pulse = 1;
			} else if (currentRead_A == LOW){
			// Hold Button A to low state
			buttonSelect = 0;
			
			// Clear flag
			buttonSelect_Flag = 0;
			buttonSelect_1_Pulse = 0;
		}
	}
	// When time specified by Debounce delay time elapsed, check button status again and determine if button is pressed or not
	if ((millis() - buttonNext_Last_Debounce_Time) > BUTTON_DEBOUNCE_DELAY){
		// If the button state changed
		if (currentRead_B == HIGH && buttonNext_1_Pulse == 0){
			// Hold Button B to high state
			buttonNext = 1;
			
			// Set 1 pulse flag
			buttonNext_1_Pulse = 1;
			} else if (currentRead_B == LOW){
			// Hold Button A to low state
			buttonNext = 0;
			
			// Clear flag
			buttonNext_Flag = 0;
			buttonNext_1_Pulse = 0;
		}
	}
	
	buttonSelect_Last_State = currentRead_A;
	buttonNext_Last_State = currentRead_B;
}

