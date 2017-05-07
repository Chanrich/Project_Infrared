/*
IR hijack project - Richard Chan



Possible improvements:
1. Add microphone to detect claps. 

 */

// include the library code:
#include <LiquidCrystal.h>
#include "avr/interrupt.h"

// Program control
#define INITIAL_STATE_VALUE 2
// Initial state value of 0 - Infrared Recording
// Initial state value of 1 - Infrared Playing (Need to enter state 0 to record first)
// Initial state value of 2 - Developing

#define PATTERN_TO_RECORD 100
#define INFRARED_SENSOR_PIN 8
#define INFRARED_LED_PIN 9
#define OPAMP_5V_PIN 7
#define OCIEA 1
#define MICROPHONE_SENSOR_PIN A5

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

unsigned char state;
boolean infrared_value;
unsigned long current_time, time_at_high, time_at_low;
int l_microphone_analog_read_value;

// Array element 0 will be garage value (it captures the time from beginning to first signal)
// Array element 1 is the time the signal spent HIGH (infrared will read 0 as it is active low)
// Array element 2 is the time the signal spend LOW (infrared will read 1 as it is active low)
unsigned long switch_timing_pattern[PATTERN_TO_RECORD];
unsigned char switch_timing_pattern_index;
char last_state;
bool array_data_print_finished;
volatile unsigned char v_interrupt_toggle;

ISR(TIMER1_COMPA_vect, ISR_BLOCK){
  // When Timer 1 interript is triggered. Toggle output at PB1.
  if (v_interrupt_toggle == 0){
    PORTB &= ~(1 << PB1);
  } else {
    PORTB |= (1 << PB1);
  }
  v_interrupt_toggle = ~v_interrupt_toggle;
}


void setup() {
  // Disable global interrupt
  cli();
  
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("System Ready");
  lcd.setCursor(1, 1);

  // Start serial transmission
  Serial.begin(9600);
  
  // Set up Infrared LED Output Pin
  pinMode(INFRARED_SENSOR_PIN, INPUT);
  pinMode(INFRARED_LED_PIN, OUTPUT);

  // For op amp experiment
  pinMode(OPAMP_5V_PIN, OUTPUT);
  digitalWrite(OPAMP_5V_PIN, HIGH);
  
  //Initialize all variables
  current_time = 0;
  time_at_high = 0;
  time_at_low = 0;
  v_interrupt_toggle = 0;
  last_state = -1;
  state = INITIAL_STATE_VALUE;
  switch_timing_pattern_index = 0;
  array_data_print_finished = false;
  for (int i = 0; i < PATTERN_TO_RECORD ; ++i){
    switch_timing_pattern[i] = 0;
  }

  // Setup timer1 to 38Khz
  OCR1A = 209; //Set output compare register to 210 (209 + 0th cycle) 
  TCCR1A = 0; // Nothing need to be set at Register A
  TCCR1B = _BV(WGM12) | _BV(CS10) | _BV(CS12); // Enable CTC mode, 1024 pre-scaler, should get me 74.4 hz
  
  // Enable Global interrupt
  sei();
}

void loop() {

  switch (state){
    case 0: // Recording state

      // Read infrared sensor at Port B0 (or Pin 8)
      infrared_value = (PINB & (1 << PB0));

      // Read the current time
      current_time = micros();
      
      // If detected LOW and last detection is HIGH. (Infrared value is active low)
      if (infrared_value == LOW && last_state == HIGH){
        time_at_high = current_time;
        switch_timing_pattern[switch_timing_pattern_index] = time_at_high - time_at_low;
        switch_timing_pattern_index++;

        // Make sure array index will over run, if it does, restart index
        if (switch_timing_pattern_index > 99) {
          switch_timing_pattern_index = 0;
        }
      }
    
      // If detected HIGH and last detection is LOW. (Infrared value is active low)
      if (infrared_value == HIGH && last_state == LOW){
        time_at_low = current_time;
        switch_timing_pattern[switch_timing_pattern_index] = time_at_low - time_at_high;
        switch_timing_pattern_index++;
        
        // Make sure array index will over run, if it does, restart index
        if (switch_timing_pattern_index > 99) {
          switch_timing_pattern_index = 0;
        }
      }
      
      // If serial monitor idle for 3 seconds, print to serial if array not yet printed and array has size 1 and larger
      if ((current_time - time_at_low) > 3000000 && array_data_print_finished == false && switch_timing_pattern_index > 1){
        lcd.setCursor(0, 0);
        lcd.print(switch_timing_pattern_index);
        for (int i = 0; i < switch_timing_pattern_index ; ++i){
          Serial.print(i);
          Serial.print("\t");
          Serial.println(switch_timing_pattern[i]);
        }
        array_data_print_finished = true;

        // Reset index to 0
        // switch_timing_pattern_index = 0;

        // Proceed to infrared state
        state = 1;
      }
      //Remember the last state for next iteration
      last_state = infrared_value;
      break;
      
    case 1: // IR Output state
      for (int trymore = 0; trymore < 5; trymore++){
        delay(3000);
        Serial.print(trymore);
        Serial.println(" time");
        for (char i = 1; i < switch_timing_pattern_index; i += 2){
          // Start sending IR signal at 38kHz frequency
          send_IR_with_timer(switch_timing_pattern[i]);

          // Stop sending for switch_timing_pattern[i+1] microseconds
          delayMicroseconds(switch_timing_pattern[i+1]);
        }
      }
      
      // Proceed to next state
      state = 2;
      break;
    case 2: 
//        l_microphone_analog_read_value = 0;
//        for(int i_loop = 0; i_loop < 100; i_loop++){
//          l_microphone_analog_read_value += analogRead(MICROPHONE_SENSOR_PIN);
//        }
//        l_microphone_analog_read_value /= 100;

        l_microphone_analog_read_value = analogRead(MICROPHONE_SENSOR_PIN);
        Serial.println("Recorded average");
        Serial.println(l_microphone_analog_read_value);
        delay(1000);
        break;
    default:
        break;
  }


}

void send_IR_with_timer(unsigned long delay_timer){
  // Start clock for timer with no prescaler.
  TIMSK1 |= _BV(OCIEA);

  // Wait for specified time before turning off clock for timer
  delayMicroseconds(delay_timer);
  TIMSK1 &= ~(_BV(OCIEA));

  // Reset infrared output port
  PORTB &= ~(1 << PB1);
}

