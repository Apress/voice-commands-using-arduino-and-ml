/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
/**This file was edited by Julia Makivic 16/10/2020**/

#if defined(ARDUINO) && !defined(ARDUINO_ARDUINO_NANO33BLE)
#define ARDUINO_EXCLUDE_CODE
#endif  // defined(ARDUINO) && !defined(ARDUINO_ARDUINO_NANO33BLE)

#ifndef ARDUINO_EXCLUDE_CODE
#include "command_responder.h"
#include "Arduino.h"

/* Libraries for the LED Matrix*/
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

//initiate the LED Matrix
Adafruit_8x8matrix matrix = Adafruit_8x8matrix();

//Byte Arrays that encode patterns on the LED Matrix
//They depict a smile for the "Yes" command, a frowning face for the "No" command, a neutral expression if the voice command is unknown, 
//a default pattern for when there are no voice commands
static const uint8_t PROGMEM
  yes_bmp[] =
  { B00000000,
    B01000010,
    B10100101,
    B00000000,
    B01111110,
    B00111100,
    B00011000,
    B00000000 },
  no_bmp[] =
  { B00000000,
    B00100100,
    B01100110,
    B11000011,
    B00000000,
    B00011000,
    B00100100,
    B00000000 },
  unknown_bmp[] =
  { B00000000,
    B11100111,
    B01000010,
    B00000000,
    B01111110,
    B00000000,
    B00000000,
    B00000000 },
  waiting_bmp[]={
    B00000000,
    B01111110,
    B01111110,
    B01111110,
    B01111110,
    B01111110,
    B01111110,
    B00000000};

void RespondToCommand(tflite::ErrorReporter* error_reporter,
                      int32_t current_time, const char* found_command,
                      uint8_t score, bool is_new_command) {
  static bool is_initialized = false;
  if (!is_initialized) {
     matrix.begin(0x70); //initializing the LED Matrix
     is_initialized = true;
  }
  static int32_t last_command_time = 0;//variable that keeps track of when the last command was heard
  static int count = 0;
  static int certainty = 220;

  //this section of code checks to see whether there is a new voice command, and if so whether the voice command is a "Yes", "No", or "Unknown"
  //The matrix then displays the appropriate pattern based off of the voice command
  if (is_new_command) {
    TF_LITE_REPORT_ERROR(error_reporter, "Heard %s (%d) @%dms", found_command,
                         score, current_time);
    // if the command is a yes, then display the corresponding smiling pattern
    if (found_command[0] == 'y') {
      last_command_time = current_time;
      matrix.clear();
      matrix.drawBitmap(0, 0, yes_bmp, 8, 8, LED_ON);
      matrix.writeDisplay();
      delay(500);
    }
    //if the command is a no, then display the corresponding frowning pattern
    if (found_command[0] == 'n') {
      last_command_time = current_time;
      matrix.clear();
      matrix.drawBitmap(0, 0, no_bmp, 8, 8, LED_ON);
      matrix.writeDisplay();
      delay(500);
      
    }
    //if the command is unknown, then display the corresponding neutral pattern
    if (found_command[0] == 'u') {
      last_command_time = current_time;
      matrix.clear();
      matrix.drawBitmap(0, 0, unknown_bmp, 8, 8, LED_ON);
      matrix.writeDisplay();
      delay(500); 
     
    }
  }

  // If last_command_time is non-zero but was >3 seconds ago, zero it
  // and display the default square pattern
  if (last_command_time != 0) {
    if (last_command_time < (current_time - 3000)) {
      last_command_time = 0;
      matrix.clear();
      matrix.drawBitmap(0, 0, waiting_bmp, 8, 8, LED_ON);
      matrix.writeDisplay();
      delay(500); 
     
    }
    // If it is non-zero but <3 seconds ago, do nothing.
    return;
  }

  // Otherwise, toggle the LED every time an inference is performed.
  ++count;
  
}

#endif  // ARDUINO_EXCLUDE_CODE
