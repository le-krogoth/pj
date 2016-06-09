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

static const int LED_BRIGHTNESS = 10;

static const short ERROR_SUCCESS_BLINKCOUNT = 8;

// EEPROM addressing
static const int EEPROM_REGKEY_ADDRESS = 0;
static const int EEPROM_MOVIE_ADDRESS = 4;
static const int EEPROM_WIFITOGGLE_ADDRESS = 8;

// Empty Vote
static const int EMPTY_VOTE = 0;

// movie growth size
static const int MOVIE_MEMORY_GROWTH_SIZE = 4;

// IDLE Mode Loop Time in Seconds (for now)
// TODO: override this value from HARDAC
static const long IDLE_MODE_LOOP_TIME = 20;
static const long VOTE_CLEAR_TIME = IDLE_MODE_LOOP_TIME - 10;
static const long EASTEREGG_CLEAR_TIME = 20;

static const int SECONDS_TO_MILLIS = 1000;

static const int STANDARD_CYCLE_LENGTH = 200;

// value in EEPROM if this pj is registered
static const byte REGISTERED = 0x5F;
// value in EEPROM for ??
static const byte MAGICMOVIE = 0x42;

// value in EEPROM to decide to either deep sleep or not
static const byte BOOTWIFI_ON = 0x11;
static const byte BOOTWIFI_OFF = 0x22;

// define EASTEREGG codes here (left=2 and right=3, max 8 digits)
static const unsigned long EGG_CODE_1 = 3333;
static const unsigned long EGG_CODE_2 = 2222;
static const unsigned long EGG_CODE_3 = 23232323;
static const unsigned long EGG_CODE_4 = 222333;

// Default Movies. Make sure that Array Size matches size of defined array...
static const int STOCK_MOVIE_ARRAY_SIZE = 7;
static String STOCK_MOVIES_LEFT[STOCK_MOVIE_ARRAY_SIZE] =  {
                    "0001010103010501070107110731075107710571037101710071005100310011000F000F000F000F",
                    "0031000F000F000F000F000F",
                    "0102001201010011000F000F",
                    "10020012",
                    "100100111001001110010011100100111001001110010011000F000F000F000F000F000F000F000F000F",
                    "100F000F000F110F000F000F010F000F000F110F000F000F",
                    "0001100120013001400150016001700108019001910192019301940195019601970198019901890179016901590149013901290119010901091109210931094109510951096109710981099108910791069105910491039102910191009110912091309140915091609170918091909190819071906190519041903190219011900180017001600150014001300120011001000F000F000F000F000F"
                 };
static String STOCK_MOVIES_RIGHT[STOCK_MOVIE_ARRAY_SIZE] = {
                    "0001010103010501070107110731075107710571037101710071005100310011000F000F000F000F",
                    "0031000F000F000F000F000F",
                    "0102001201010011000F000F",
                    "10020012",
                    "100100111001001110010011100100111001001110010011000F000F000F000F000F000F000F000F000F",
                    "100F000F000F110F000F000F010F000F000F110F000F000F",
                    "0001100120013001400150016001700108019001910192019301940195019601970198019901890179016901590149013901290119010901091109210931094109510951096109710981099108910791069105910491039102910191009110912091309140915091609170918091909190819071906190519041903190219011900180017001600150014001300120011001000F000F000F000F000F"
                 };

#endif // H_CONST
