/*
 ====================================================================

 PJ

 --------------------------------------------------------------------
 Copyright (c) 2016 by Krogoth of
 Ministry of Zombie Defense - http://www.mzd.org.uk/
 and contributing authors

 This file is part of HARDAC.

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU Affero General Public License as published
 by the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Affero General Public License for more details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ====================================================================
*/
#ifndef H_CONST
#define H_CONST

#include <Arduino.h> //needed for Serial.println

// change these to your setup
static const char* SSID  = "hardac";
static const char* PWD   = "hardac";

// device_id
extern String sPJDeviceId;

// where your hardac server is running
static const char* SERVER  = "192.168.100.12";
static const int PORT      = 8765;

// Interrupt Pins for Button GPIO
static const int LEFT_BUTTON = 9;
static const int RIGHT_BUTTON = 10;

// Pins for the LEDs
static const int LED_LEFT_R = 12;
static const int LED_LEFT_G = 13;
static const int LED_LEFT_B = 14;

static const int LED_RIGHT_R = 2;
static const int LED_RIGHT_G = 4;
static const int LED_RIGHT_B = 5;

static const short ERROR_SUCCESS_BLINKCOUNT = 8;

// EEPROM addressing
static const int EEPROM_REGKEY_ADDRESS = 0;
static const int EEPROM_MOVIE_ADDRESS = 4;

// Empty Vote
static const int EMPTY_VOTE = 0;

// movie growth size
static const int MOVIE_MEMORY_GROWTH_SIZE = 4;

// IDLE Mode Loop Time in Seconds (for now)
// TODO: override this value from HARDAC
static const long IDLE_MODE_LOOP_TIME = 20;
static const long VOTE_CLEAR_TIME = IDLE_MODE_LOOP_TIME + 10;
static const long EASTEREGG_CLEAR_TIME = 20;

static const int SECONDS_TO_MILLIS = 1000;

static const int STANDARD_CYCLE_LENGTH = 200;

// value in EEPROM if this pj is registered
static const byte REGISTERED = 0x5F;
// value in EEPROM for ??
static const byte MAGICMOVIE = 0x42;

// define EASTEREGG codes here (left=2 and right=3, max 8 digits)
static const unsigned long EGG_CODE_1 = 2222;
static const unsigned long EGG_CODE_2 = 3333333;
static const unsigned long EGG_CODE_3 = 23232323;
static const unsigned long EGG_CODE_4 = 222333;

// Default Movies. Make sure that Array Size matches size of defined array...
static const int STOCK_MOVIE_ARRAY_SIZE = 6;
static String STOCK_MOVIES_LEFT[STOCK_MOVIE_ARRAY_SIZE] =  {
                                    "500255025552055200520002700100017001000170010001",
                                    "090190010091090190010091000F000F000F000F000F000F",
                                    "A005000500A5000F000F000F",
                                    "D001000500D5000F000F000F",
                                    "D005000500D5000F000F000F",
                                    "D005020500D5000F000F000F"
                                 };
static String STOCK_MOVIES_RIGHT[STOCK_MOVIE_ARRAY_SIZE] = {
                                    "500255025552055200520002700100017001000170010001",
                                    "090190010091090190010091000F000F000F000F000F000F",
                                    "A005000500A5000F000F000F",
                                    "D001000500D5000F000F000F",
                                    "D005000500D5000F000F000F",
                                    "D005020500D5000F000F000F"
                                  };

#endif // H_CONST
