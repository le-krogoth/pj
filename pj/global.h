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
static const int LEFT_BUTTON = 4;
static const int RIGHT_BUTTON = 5;

// Pins for the LEDs
static const int LED_LEFT_R = 12;
static const int LED_LEFT_G = 13;
static const int LED_LEFT_B = 14;

static const int LED_RIGHT_R = 10;
static const int LED_RIGHT_G = 10;
static const int LED_RIGHT_B = 10;

// EEPROM addressing
static const int EEPROM_REGKEY_ADDRESS = 0;

// IDLE Mode Loop Time in Seconds (for now)
static const long IDLE_MODE_LOOP_TIME = 10;
static const int SECONDS_TO_MILLIS = 1000;

// value in EEPROM if this pj is registered
static const byte REGISTERED = 0x5F;

#endif // H_CONST
