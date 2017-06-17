
/*
 * Infrared_Arduino.ino
 *
 * Created: 5/14/2017 9:12:17 PM
 * Author: Rchan
 */ 
#include "Global.h"
#include "buttonRead.h"
#include <LiquidCrystal.h>
#include "avr/interrupt.h"

// Added to stop compiler from giving warning message.
//extern HardwareSerial Serial

// Create Liquid Crystal Interface, Using only 4 data pins, with RW connected to ground
// LiquidCrystal(rs, enable, d4, d5, d6, d7)
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

char *MenuArray[] = {"Record", "Play", "Clear"};
char Symbol_Greater_Than = 62;




// General variable declare
static int general_use_counter;

// Define States:
// 1: Idle
//	During Idle state, the microcontroller await for input from buttons
// 2: Recording
//	Record feature is triggered
//	Record infrared signal.
// 3: Play
//	Play feature is triggered
//	Playback recorded signal through IR led
// 4: Clear
//	Clear feature is triggered
//	Delete all recorded signal

// Declare state variables
static char currentState;


volatile unsigned char v_interrupt_toggle;

ISR(TIMER1_COMPA_vect, ISR_BLOCK){
	// When Timer 1 interrupt is triggered. Toggle output at PB1.
	if (v_interrupt_toggle == 0){
		PORTB &= ~(1 << PB1);
		} else {
		PORTB |= (1 << PB1);
	}
	v_interrupt_toggle = ~v_interrupt_toggle;
}

void setup()
{
	// Setup pushbuttons as input
	pinMode(buttonSelect_PIN, INPUT);
	pinMode(buttonNext_PIN, INPUT);
	
	// Set output pins to control shift register
	pinMode(SER_PIN, OUTPUT);
	pinMode(SRCLK_PIN, OUTPUT);
	pinMode(RCLK_PIN, OUTPUT);

	// Initialize LCD interface and specifies the width and height of the display
	lcd.begin(16, 2);
	
	// Setup debugging serial
	Serial.begin(9600);

	// Initialize
	general_use_counter = 0;
	currentState = 0; // Initialize to idle state
	
	// Initialize the LCD menu
	LCD_SelectNextItem();
	buttonSelect = 0;
	buttonNext = 0;
	buttonSelect_1_Pulse = 0;
	buttonNext_1_Pulse = 0;
	buttonSelect_Flag = 0;
	buttonNext_Flag = 0;
	buttonSelect_Last_State = 0;
	buttonNext_Last_State = 0;
	buttonSelect_Last_Debounce_Time = 0;
	buttonNext_Last_Debounce_Time = 0;

	
	// Setup timer1 to 38Khz
	OCR1A = 209; //Set output compare register to 210 (209 + 0th cycle)
	TCCR1A = 0; // Nothing need to be set at Register A
	TCCR1B = _BV(WGM12) | _BV(CS10) | _BV(CS12); // Enable CTC mode, 1024 pre-scaler, should get me 74.4 hz
	
	// Enable Global interrupt
	sei();
}


void loop()
{	
	switch(currentState){
		// Idle state
		case 0:
			// Read all buttons
			DebounceReadButton();
			
			// When select button is pressed
			if (buttonSelect == HIGH){
				switch(selectedMenuOption){
					case 0:
						break;
						
				}
			}
	
			if (buttonNext == HIGH){
			//Down button is pressed
			LCD_SelectNextItem();
			}
			break;
		
		// Record state
		case 1:
			break;
		
		// Play state
		case 2:
			break;
		
		// Clear state
		case 3:
			break;
	}
	
	


	//delay(1000);
}



void LCD_SelectNextItem (){
	// Clean the LCD screen.
	lcd.clear();
	
	// Increment to the next option
	selectedMenuOption++;
	
	// If selected option go over the max index, reset to first option
	if (selectedMenuOption > NumberOfItems_MenuArray){
		selectedMenuOption = 1;
	}
	
	// Set cursor at location setCursor(col, row)
	lcd.setCursor(0, 0);
	lcd.write(Symbol_Greater_Than);
	
	if (selectedMenuOption <= NumberOfItems_MenuArray  && selectedMenuOption >= 1){
		// Index within range
		
		// Set Cursor to the second character of first row, first character reserved for select indicator
		lcd.setCursor(1, 0);
		// Print Selected option with adjusted array index
		lcd.print(MenuArray[selectedMenuOption-1]);
		if (selectedMenuOption != NumberOfItems_MenuArray){
			// Print the second option to the second character of second row only if last item is not selected
			lcd.setCursor(1, 1);
			// Print next option
			lcd.print(MenuArray[selectedMenuOption]);
		}
	}
}
