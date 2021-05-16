// This is a demonstration code for a two-wire analog keypad for Arduino.
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License <https://www.gnu.org/licenses/> for more details.
//
//	 (c) 2021, Lauri Pirttiaho

void setup() {
  Serial.begin(9600);
  Serial.print("Two-Wire Keypad Demo\n");
}

enum {
  TWKP_BEGIN,
  TWKP_CAL,
  TWKP_RUN
} twkp_main_state = TWKP_BEGIN;

enum {
  TWKP_UP,
  TWKP_GOING_DOWN,
  TWKP_DOWN,
  TWKP_GOING_UP
} twkp_button_state = TWKP_UP;

bool button_event = false;
unsigned long debounce_done = 0;
int analog_value = 0;
int calib_offset = 0;
int calib_range = 0;

const int DOWN_THRESHOLD = 1023 / (4 * 11);
const int DEBOUNCE_TIME = 50; // ms
const char BUTTONS[12] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '*', '0', '#'};

void loop() {
  switch (twkp_main_state) {
    case TWKP_BEGIN:
      button_event = false;
      twkp_main_state = TWKP_CAL;
      Serial.print("Press and hold the lowest voltage button\n");
      break;
      
    case TWKP_CAL:
      if (button_event) {
        calib_offset = analog_value;
        calib_range = 1023 - analog_value;
        button_event = false;
        twkp_main_state = TWKP_RUN;
        Serial.print("Calibration ready (offset = ");
        Serial.print(calib_offset);
        Serial.print(")\nRelease the button\n");
      }
      break;
      
    case TWKP_RUN:
      if (button_event) {
        int button = ((analog_value - calib_offset) * 11 + (calib_range / 2)) / calib_range;
        if (button < 0 || button > 11) {
          Serial.print("\n");
        }
        else {
          Serial.print(BUTTONS[button]);
          Serial.print('\n');
        }
        button_event = false;
      }
      break;
  }

  int analog_in = analogRead(0);

  switch (twkp_button_state) {
    case TWKP_UP:
      if (analog_in > DOWN_THRESHOLD)
      {
        debounce_done = millis() + DEBOUNCE_TIME;
        twkp_button_state = TWKP_GOING_DOWN;
      }
      break;
      
    case TWKP_GOING_DOWN:
      if (analog_in < DOWN_THRESHOLD)
      {
        twkp_button_state = TWKP_UP;
      }
      else if (millis() > debounce_done) { // Subject to glitch caused by the 50-day wrap-around of the millis() count
        analog_value = analog_in;
        button_event = true;
        twkp_button_state = TWKP_DOWN;
      }
      break;
      
    case TWKP_DOWN:
      if (analog_in < DOWN_THRESHOLD)
      {
        debounce_done = millis() + DEBOUNCE_TIME;
        twkp_button_state = TWKP_GOING_UP;
      }
      break;
      
    case TWKP_GOING_UP:
      if (analog_in > DOWN_THRESHOLD)
      {
        twkp_button_state = TWKP_DOWN;
      }
      else if (millis() > debounce_done) { // Subject to glitch caused by the 50-day wrap-around of the millis() count
        twkp_button_state = TWKP_UP;
      }
      break;
  }
}
