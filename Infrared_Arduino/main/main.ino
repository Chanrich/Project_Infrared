/*
 * Infrared_Arduino.ino
 *
 * Created: 5/14/2017 9:12:17 PM
 * Author: Rchan
 */ 
#include "Global.h"
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include "avr/io.h"
#include "avr/interrupt.h"
#include "avr/eeprom.h"
// Create Liquid Crystal Interface, Using only 4 data pins, with RW connected to ground
// LiquidCrystal(rs, enable, d4, d5, d6, d7)
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Set new serial port (Rx, Tx)
SoftwareSerial ESP_Serial(A0, A1);

// Function description:
// Set IR high at register level for speed.
void Set_IR_High(){
	// Register level set target to high
	IR_LED_PORT |= (1 << IR_LED_PORT_PIN);
}

// Function description:
// Set IR low at register level for speed.
void Set_IR_Low(){
	// Register level set target to low
	IR_LED_PORT &= ~(1 << IR_LED_PORT_PIN);
}
// Function description:
// Enable interrupt timer to start transmission of IR signals
void send_IR_withTimerDelay(unsigned int delay_timer){
	// Start clock for timer A.
	TIMSK1 |= (1 << OCIE1A);

	// Wait for specified time before turning off clock for timer
	delayMicroseconds(delay_timer);
		
	TIMSK1 &= ~(1 << OCIE1A);

	// Reset infrared output port
	Set_IR_Low();
}

// Function description:
// Interrupt service routine to transmit IR signals.
ISR(TIMER1_COMPA_vect, ISR_BLOCK){
	// When Timer 1 interrupt is triggered. Toggle output at PD1.
	if (v_interrupt_toggle == 0){
		Set_IR_High();
		} else {
		Set_IR_Low();
	}
	v_interrupt_toggle = ~v_interrupt_toggle;
}
// Function description: Turn on/off LEDs according to parameter
// 0: All LEDs OFF
// 1: Red LEDs ON
// 2: Green LEDs ON
void ConfigLED_Shifter(char configIndex){
	// static variable to hold LED configuration
	static unsigned char cLED_Configuration;	
	
	switch (configIndex){
		case 0:
			// Turn all LEDs off
			cLED_Configuration &= 0b00000000;
			break;
		case 1:
			// Turn all red LEDs on
			cLED_Configuration |= 0b11000011;
			break;
		case 2:
			// Turn all green LEDs on
			cLED_Configuration |= 0b00111100;
			break;
	}
	
	// Storage register is positive-edge triggered. Lower it to prepare the data
	digitalWrite(RCLK_PIN, LOW);
	
	// Shift all bits to their shift register
	shiftOut(SER_PIN, SRCLK_PIN, MSBFIRST, cLED_Configuration);
	
	// Storage register is positive-edge triggered. Turn it on to store data into storage registrer
	digitalWrite(RCLK_PIN, HIGH);
}

bool Read_IR_Signal(){
	return (IR_Receive_PIN & (1 << IR_Receive_PORT_PIN));
}

// Function description:
// This function will be call to perform debounce reading on next and select buttons
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

// Function description:
// Print two menu options to LCD screen
void LCD_DisplayCurrentOption(){
	// Clean the LCD screen.
	lcd.clear();
	
	// Set cursor at location setCursor(col, row)
	lcd.setCursor(0, 0);
	lcd.write(Symbol_Greater_Than);
		
	// Set Cursor to the second character of first row, first character reserved for select indicator
	lcd.setCursor(1, 0);
	// Print Selected option with adjusted array index
	lcd.print(MenuArray[selectedMenuOptionIndex-1]);
	// Print the second option to the second character of second row
	lcd.setCursor(1, 1);
	// Print next option
	lcd.print(MenuArray[selectedMenuOptionIndex]);
}

// Function description:
// Clean the LCD and increment menu index by 1. Then, print two menu options to LCD screen
void SelectNextOptionButton(){
	// Clean the LCD screen.
	lcd.clear();
	
	// Increment to the next option
	selectedMenuOptionIndex++;
	
	// If selected option go over the max index, reset to first option
	if (selectedMenuOptionIndex > NumberOfItems_MenuArray){
		selectedMenuOptionIndex = 1;
	}
	
	// Set cursor at location setCursor(col, row)
	lcd.setCursor(0, 0);
	lcd.write(Symbol_Greater_Than);
	
	if (selectedMenuOptionIndex <= NumberOfItems_MenuArray  && selectedMenuOptionIndex >= 1){
		// Index within range
		
		// Set Cursor to the second character of first row, first character reserved for select indicator
		lcd.setCursor(1, 0);
		// Print Selected option with adjusted array index
		lcd.print(MenuArray[selectedMenuOptionIndex-1]);
		if (selectedMenuOptionIndex != NumberOfItems_MenuArray){
			// Print the second option to the second character of second row only if last item is not selected
			lcd.setCursor(1, 1);
			// Print next option
			lcd.print(MenuArray[selectedMenuOptionIndex]);
		}
	}
}

// Function description:
// Change LCD screen to display current status
void LCD_ShowBusy(char stateIndex){
	// Clean the LCD screen.
	lcd.clear();
	
	lcd.setCursor(0, 0);
	lcd.write("...");
	
	lcd.setCursor(0, 1);
	switch (stateIndex){
		case 1:
			// Recording starting state
			lcd.write("Starting");
			break;
		case 2:
			// Playing state
			lcd.write("Playing");
			break;
		case 3:
			// Clearing state
			lcd.write("Clearing");
			break;
		case 4:
			// Recording ready state
			lcd.write("Ready");
			break;
		case 5:
			// Recording saving state
			lcd.write("Saving");
			break;
	}
}

// Function description:
// Reset the IR parameter to prepare for next IR recording
void resetIR(){
	// Reset
	bInitialTimestamp = false;
	bStartRecording = false;
	last_IR_Read = -1;
	lastIR_Timestamp = 0;
	initialStartTimestamp = 0;
	bGotSignal = false;
}

// Function called one time to setup micro-processor
void setup()
{
	// Disable global interrupt
	cli();
	
	// Setup pushbuttons as input
	pinMode(buttonSelect_PIN, INPUT);
	pinMode(buttonNext_PIN, INPUT);
	
	// Set output pins to control shift register
	pinMode(SER_PIN, OUTPUT);
	pinMode(SRCLK_PIN, OUTPUT);
	pinMode(RCLK_PIN, OUTPUT);
	
	// Set up IR Receiver pin to input at register level
	IR_Receive_DDR &= (1 << IR_Receive_PORT_PIN);
	
	// Set IR LED pin to output mode
	pinMode(IR_LED_Digital_Pin, OUTPUT);
	
	// Initialize LCD interface and specifies the width and height of the display
	lcd.begin(16, 2);

	// Setup debugging serial
	Serial.begin(9600);
	// Set pin mode for software serial
	pinMode(A0, INPUT);
	pinMode(A1, OUTPUT);
	
	// Setup Esp8266 serial link
	ESP_Serial.begin(115200);

	// Initialize variables
	currentState = 0; // Initialize to idle state
	currentTime = 0;
	lastIR_Timestamp = 0;
	initialStartTimestamp = 0;
	switchTimingPatternIndex = 0;
	v_interrupt_toggle = 0;
	for (int i = 0; i < PATTERN_TO_RECORD ; ++i){
		switchTimingPattern[i] = 0;
	}
	last_IR_Read = -1;
	buttonSelect = 0;
	bGotSignal = false;
	buttonNext = 0;
	buttonSelect_1_Pulse = 0;
	buttonNext_1_Pulse = 0;
	buttonSelect_Flag = 0;
	buttonNext_Flag = 0;
	buttonSelect_Last_State = 0;
	buttonNext_Last_State = 0;
	buttonSelect_Last_Debounce_Time = 0;
	buttonNext_Last_Debounce_Time = 0;
	bStartRecording = false;
	bInitialTimestamp = false;
	
	// Initialize the LCD menu
	SelectNextOptionButton();
	
	TCCR1A = 0; // Nothing need to be set at Register A
	TCCR1B |= ((1 << CS10) | (1 << CS12)); // Set 1024 pre-scaler, should get me 74.4 Hz
	TCCR1B = (1 << WGM12); // Enable CTC mode,
	OCR1A = 209; //Set output compare register to 210 (209 + 0th cycle)	Formula = 16M / 1024 / 210 = 74.4Hz
	// Enable Global interrupt
	sei();
}


void loop(){
	char cread;
	while (ESP_Serial.available() > 0){
		cread = ESP_Serial.read();
		Serial.print(cread);
	}
	switch(currentState){
		// Idle state
		case 0:
			// Turn off all LEDs
			ConfigLED_Shifter(0);
			
			// Read all buttons
			DebounceReadButton();
			
			// Select button is pressed
			if (buttonSelect == HIGH){
				// Check current selected menu option
				
				switch(selectedMenuOptionIndex){
					case 1:
						// Record option selected, change state to recording state
						currentState = 1;
						
						// Update LCD to busy
						LCD_ShowBusy(currentState);
						break;
					case 2:
						// Play option selected, change state to playing state
						currentState = 2;
						
						// Update LCD to busy
						LCD_ShowBusy(currentState);
						break;
					case 3:
						// Clear option selected, change state to clearing state
						currentState = 3;
						
						// Update LCD to busy
						LCD_ShowBusy(currentState);
						break;
				}
			}
	
			if (buttonNext == HIGH){
				//Down button is pressed, select next option
				SelectNextOptionButton();
				
				// Debug insert
				Serial.println("Sending AT command");
				ESP_Serial.write("AT\r\n");
			}
			break;
		
		// ==============================================
		//				Record state
		// ==============================================
		case 1:
			// Read infrared sensor at Port D6 (or Pin 6)
			infraredValueRead = Read_IR_Signal();

			// Read the current time
			currentTime = micros();
			
			// Delay recording by 3 seconds
			if (bInitialTimestamp == false){
				// Record the time when it first entered 
				initialStartTimestamp = currentTime;
				
				// Set flag to prevent resetting initial timestamp
				bInitialTimestamp = true;
			}
			
			// Delay recording by 3 seconds
			if (currentTime - initialStartTimestamp > 2000000 && bStartRecording == false){
				// Set flag to start recording
				bStartRecording = true;
				
				// Set LCD screen to ready
				LCD_ShowBusy(4);
			}
			
			// When IR value changes state
			if (infraredValueRead != last_IR_Read && bStartRecording == true){
				if (switchTimingPatternIndex < PATTERN_TO_RECORD){
					// If valid signal detected, record it (First signal is not a valid signal)
					if (lastIR_Timestamp != 0){
						// Trigger signal detection flag
						bGotSignal = true;
						
						// Save timing differences into storage and increment index
						switchTimingPattern[switchTimingPatternIndex] = currentTime - lastIR_Timestamp;
						switchTimingPatternIndex++;
					}
					
					// Show red LEDs
					ConfigLED_Shifter(1);
					
					// Set LCD screen to saving to notify user
					LCD_ShowBusy(5);
					
					// Timestamp last time the IR flipped value
					lastIR_Timestamp = currentTime;
				} else {
					// When index overflow
					Serial.println("Overflow!");
					// Display message to LCD
					lcd.clear();
					lcd.setCursor(0,0);
					lcd.print("Max");
					lcd.setCursor(0,1);
					lcd.print("Reached");
				}

			}

			// If serial monitor idle for 3 seconds, print to serial if array not yet printed and array has size 1 and larger
			if ((currentTime - lastIR_Timestamp) > 3000000 && bGotSignal == true){
				//lcd.setCursor(0, 0);
				//lcd.print(switchTimingPatternIndex);
				for (int i = 0; i < switchTimingPatternIndex ; ++i){
					Serial.print(i);
					Serial.print("\t");
					Serial.println(switchTimingPattern[i]);
				}
				
				// Turn off all LEDs
				ConfigLED_Shifter(0);
				
				// Reset
				resetIR();
				
				// Proceed to idle state
				currentState = 0;
				
				// Display current selection
				LCD_DisplayCurrentOption();
			} else if ((currentTime - initialStartTimestamp) > 10000000 && bGotSignal == false){
				// Nothing is recorded for 10 seconds. Prompt user and return to idle state
				lcd.clear();
				lcd.setCursor(0,0);
				lcd.print("No data");
				lcd.setCursor(0,1);
				lcd.print("Recorded");
				
				// Delay 3 seconds for user to read message
				delay(3000);
				
				// Turn off all LEDs
				ConfigLED_Shifter(0);
				
				// Reset
				resetIR();
				
				// Proceed to idle state
				currentState = 0;
				
				// Display current selection
				LCD_DisplayCurrentOption();
			}
			
			// Record the last infrared value for next iteration
			last_IR_Read = infraredValueRead;
			
			break;
			
			
		// ==============================================
		//					Play state
		// ==============================================
		case 2:
			// If no data, quit this state
			if (switchTimingPatternIndex == 0){
				// Display message
				lcd.clear();
				lcd.print("No data");
				
				// Red LEDs
				ConfigLED_Shifter(1);
				
			} else {
				// Green LEDs
				ConfigLED_Shifter(2);

							
				for (int i = 0; i <= switchTimingPatternIndex; i += 2){
					// Start sending IR signal at 38kHz frequency
					send_IR_withTimerDelay(switchTimingPattern[i]);

					// Stop sending for switch_timing_pattern[i+1] microseconds
					delayMicroseconds(switchTimingPattern[i+1]);
				}
			
				// LCD Success message
				lcd.clear();
				lcd.print("Done...");
			}
			
			// Delay 3 seconds
			delay(3000);
			
			// Return to idle
			currentState = 0;
			
			// Display current selection
			LCD_DisplayCurrentOption();
			break;
		// ==============================================
		//					Clear state
		// ==============================================
		case 3:
			// Red LEDs
			ConfigLED_Shifter(1);
			
			// Reset
			for (int i = 0; i < PATTERN_TO_RECORD ; ++i){
				switchTimingPattern[i] = 0;
			}
			switchTimingPatternIndex = 0;
			resetIR();
			
			// LCD Success message
			lcd.clear();
			lcd.print("Done...");
			
			// Green LEDs
			ConfigLED_Shifter(2);
			
			// Delay 1 seconds to show
			delay(1000);
			
			// Proceed to idle state
			currentState = 0;	
			
			// Display current selection
			LCD_DisplayCurrentOption();
			
			//	=========== EEPROM ==============
			//uint8_t eeprom_data_8bit;
			//eeprom_data_8bit = 0x33;
			//eeprom_update_byte((uint8_t*)0, eeprom_data_8bit);
			
			unsigned int arrayEEPROM[10];
			for (int i = 0; i < 10 ; ++i){
				arrayEEPROM[i] = (i * 100);
			}
			// ==================================================================================
			// void eeprom_update_block ( const void * pointer_ram , void * pointer_eeprom , size_t n)
			//	Description:				A pointer to data,	  EEPROM internal location,		Size
			// ==================================================================================
			
			//eeprom_update_block((const void*)arrayEEPROM, (void*)0, sizeof(arrayEEPROM));
			
			uint8_t eeprom_read_data;
			unsigned int ReadarrayEEPROM[10];
			eeprom_read_block((void*)ReadarrayEEPROM, (const void*)0, sizeof(ReadarrayEEPROM));
			for (int i = 0; i < 10 ; ++i){
				Serial.println(ReadarrayEEPROM[i]);
			}
			
			// Read from address 0
			eeprom_read_data = eeprom_read_byte((uint8_t*)0);
			Serial.print("EEPROM Read:");
			Serial.println(eeprom_read_data);
					
			break;
	}

}

