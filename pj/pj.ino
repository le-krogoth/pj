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

#include <ESP8266WiFi.h>
#include "Esp.h" // deep sleep and system info
#include <EEPROM.h> // to store status/registration in
#include "global.h"
#include "flow.h"
#include "comm.h"
#include "handler.h"
#include "moviehelper.h"

// ESP8266 SDK headers are in C, they need to be included externally or else 
// they will throw an error compiling/linking.
//
// all SDK .c files required can be added between the { }
//extern "C" {
// #include "user_interface.h"
//}

// ------------------------------------------------------------------

String sPJDeviceId = "notinitialised";

state *gs = (state *)calloc(1, sizeof(state));

// ------------------------------------------------------------------
// ------------------------------------------------------------------


// ------------------------------------------------------------------
void rightPinChanges()
{
  Serial.println(" - rightPinChanges() - ");
  // no delay functions allowed in here!
  //remember when the last button was changed for voting
  gs->ulLastButtonChangeAt = millis();
  if(digitalRead(RIGHT_BUTTON) == LOW)
  {
    Serial.println("** Falling wink from right pin.");
    gs->bRightButtonDown = true;
    gs->sVote=2;
    //do a short blink, delayMicroseconds works, but its blocking!
    digitalWrite(LED_RIGHT_B, LOW);
    delayMicroseconds(1000);
    digitalWrite(LED_RIGHT_B, HIGH);
  }
}

// ------------------------------------------------------------------
void leftPinChanges()
{
  Serial.println(" - leftPinChanges() - ");
  // no delay functions allowed in here!
  //remember when the last button was changed for voting
  gs->ulLastButtonChangeAt = millis();
  if(digitalRead(LEFT_BUTTON) == LOW)
  {
    Serial.println("** Falling wink from left pin.");
    gs->bLeftButtonDown = true;
    gs->sVote=1;
    //do a short blink, delayMicroseconds works, but its blocking!
    digitalWrite(LED_LEFT_B, LOW);
    delayMicroseconds(1000);
    digitalWrite(LED_LEFT_B, HIGH);
  }
}

// ------------------------------------------------------------------
void setupPins() 
{
  // we only go for FALLING instead of CHANGING to have less issues with multiple calls, and more time to work on the button
  attachInterrupt(digitalPinToInterrupt(LEFT_BUTTON), leftPinChanges, FALLING);
  attachInterrupt(digitalPinToInterrupt(RIGHT_BUTTON), rightPinChanges, FALLING);

  // analogWrite(pin, value) enables software PWM on the given pin. PWM may be used on pins 0 to 16.
  // Call analogWrite(pin, 0) to disable PWM on the pin. value may be in range from 0 to PWMRANGE, 
  // which is equal to 1023 by default. PWM range may be changed by calling analogWriteRange(new_range).

  // we know 0 to 255 as colour intensity values
  analogWriteRange(255);
  
  // PWM frequency is 1kHz by default. Call analogWriteFreq(new_frequency) to change the frequency.
  // 50 Hz is more than OK for us for now
  analogWriteFreq(50);

  //set LEDs as output and set them to HIGH = 255
  pinMode(LED_LEFT_R, OUTPUT);
  pinMode(LED_LEFT_G, OUTPUT);
  pinMode(LED_LEFT_B, OUTPUT);

  //set to high keep it dark
  analogWrite(LED_LEFT_R, 255);
  analogWrite(LED_LEFT_G, 255);
  analogWrite(LED_LEFT_B, 255);

  pinMode(LED_RIGHT_R, OUTPUT);
  pinMode(LED_RIGHT_G, OUTPUT);
  pinMode(LED_RIGHT_B, OUTPUT);

  //set to high keep it dark
  analogWrite(LED_RIGHT_R, 255);
  analogWrite(LED_RIGHT_G, 255);
  analogWrite(LED_RIGHT_B, 255);

  // set buttons as input, to be safe
  pinMode(RIGHT_BUTTON, INPUT);
  pinMode(LEFT_BUTTON, INPUT);
}

// ------------------------------------------------------------------
// is called once after powering up, sets up the wireless network 
// checks a few setting, especially if already registered
void setup() {

  // start a serial console for debugging purposes
  // todo: remove when going into production?
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println(" -=- POWER UP -=- ");
  delay(1000); //initial bootup, give it some time

  // set all needed ports and disable unneeded output ports to save power
  setupPins();

  // initialise memory structure (global state)
  gs->ulLoopStartAt = 0;
  gs->ulLastModeUpdateAt = 0;
  gs->ulLastButtonChangeAt = 0;
  gs->bytCurrentMode = MODE_INIT;
  gs->sVote = 0;
  gs->iCycleLength = STANDARD_CYCLE_LENGTH;
  gs->bLeftButtonDown = false;
  gs->bRightButtonDown = false;

  // movies
  gs->iMovieLeftFrameCount = 0;
  gs->iMovieLeftFrameCapacity = 2;
  gs->movieLeft = (struct movieframe *)malloc(sizeof(movieframe) * gs->iMovieLeftFrameCapacity);

  gs->iMovieRightFrameCount = 0;
  gs->iMovieRightFrameCapacity = 2;
  gs->movieRight = (struct movieframe *)malloc(sizeof(movieframe) * gs->iMovieRightFrameCapacity);

  gs->lMoviePosition = 0; 
  gs->lMovieLength = 0; 

  gs->sMovieReplayCount = 0;
  playStockMovie(0);

  //for easteregg code tracking
  gs->ulEgg = 0;

  // --------------------
  // start wifi
  WiFi.mode(WIFI_STA);
  
  // get the wifi up and running
  WiFi.begin(SSID, PWD);

  Serial.print("Connecting to network: ");
  Serial.println(SSID);

  // TODO: this loops until things get connected to the wireless network
  // should we cancel after a few attempts and sleep instead to not drain
  // the battery too much when out of reach?
  unsigned long lStart = millis();
  while (WiFi.status() != WL_CONNECTED && lStart + (20 * SECONDS_TO_MILLIS) > millis()) 
  {
    delay(500);
    Serial.print(".");
  }

  // go back into deep sleep mode for now when there is no wifi
  // TODO: add this check to the state machine?
  if(WiFi.status() != WL_CONNECTED)
  {
    Serial.println("");
    Serial.println("Wifi not available, going to sleep.");

    // todo: blink in a whimpy fashion!
    playBootError();
    ESP.deepSleep(1 * 10 * 1000000, WAKE_RF_DEFAULT);
    delay(1000);  
  }

  // print out stats
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");

  // calculate device id
  // Chip ID: 000000
  // Flash ID: 0000000
  // Serial.print("Chip ID: ");
  // Serial.println(ESP.getChipId());
  // Serial.print("Flash ID: ");
  // Serial.println(ESP.getFlashChipId());
  sPJDeviceId = String(ESP.getChipId()) + "_" + String(ESP.getFlashChipId());
  Serial.println("#########################");
  Serial.println("PJId: " + sPJDeviceId);
  Serial.println("#########################");

  delay(100);
  
  // check if registered already (check EEPROM), register with HARDAC otherwise.
  // TODO: catch error and add default
  if(sPJDeviceId.length() <= 1)
  {
    Serial.println("sDeviceId is too small. " + sPJDeviceId);
  }
  else
  {
    // register EEPROM for registration status and last stock movie
    EEPROM.begin(16);
  
    byte byRegKey = EEPROM.read(EEPROM_REGKEY_ADDRESS);
    
    Serial.print("EEPROM Value (0x5F) at EEPROM_REGKEY_ADDRESS: 0x");
    Serial.println(byRegKey, HEX);
    
    if(byRegKey != REGISTERED)
    {
      Serial.println("PJ is not registered yet.");
      registerPJ(sPJDeviceId);
    }
  }

  // read last active movie from EEPROM and load it
  loadActiveMovieFromEEPROM();
}

// ------------------------------------------------------------------
// will be called right after setup and then in a loop after every deep sleep cycle
// since we (currently) deep sleep after every cycle...
void loop() {

  gs->ulLoopStartAt = millis();

  Serial.println("--------------------------------------------------------------------");
  Serial.print("-- loop(");
  Serial.print(millis());
  Serial.println(") --");
  Serial.println("Modes");
  Serial.print("- Current Mode: ");
  Serial.println(gs->bytCurrentMode);
  Serial.print("- Button Left:  ");
  Serial.println(gs->bLeftButtonDown);
  Serial.print("- Button Right: ");
  Serial.println(gs->bRightButtonDown);

  // handle all button presses (easter egg as well as votes)
  handleInput();

  // if not in easteregg mode then switch
  if(gs->bytCurrentMode != MODE_EASTEREGG)
  {
    switch(gs->bytCurrentMode)
    {
      default:
      case MODE_IDLE:
        // show movie, change mode to Update after N loops
        handleIdle();
      break;
      case MODE_INIT:
        // Initialise something? otherwise head right into Update mode.
        gs->bytCurrentMode = MODE_UPDATE;
      break;
      case MODE_UPDATE:
        gs->ulLastModeUpdateAt = millis();
        gs->bytCurrentMode = MODE_IDLE;
        handleUpdate();
      break;
    }
  }

  // clear last vote if not button was pressed for X minutes
  if((millis() - gs->ulLastButtonChangeAt > VOTE_CLEAR_TIME * SECONDS_TO_MILLIS) && gs->sVote != EMPTY_VOTE)
  {
    Serial.print("### Resetting vote, it was:");
    Serial.println(gs->sVote);
    gs->sVote = 0;
    gs->ulLastButtonChangeAt = millis();
  }

  // clear easteregg mode if not button was pressed for Y minutes
  if((millis() - gs->ulLastButtonChangeAt > EASTEREGG_CLEAR_TIME * SECONDS_TO_MILLIS) && gs->bytCurrentMode == MODE_EASTEREGG)
  {
    Serial.println("### Leaving easteregg mode due to inactivity -> IDLE");
    gs->bytCurrentMode = MODE_IDLE;
    // reset buttons as well, just in case
    gs->bLeftButtonDown = false;
    gs->bRightButtonDown = false;
    gs->sVote = 0;
  }
  
  Serial.println();
  Serial.println("Ending loop -> clean up -> stats");

  // try to have a constant "frame" rate
  long lDelayTime = gs->iCycleLength - (millis() - gs->ulLoopStartAt);

  if(lDelayTime > 0)
  {
    delay(lDelayTime);
  }
  else
  {
    delay(100);
  }

  Serial.print("Free heap:");
  Serial.println(ESP.getFreeHeap(), DEC);

  Serial.print("Delayed for: ");
  Serial.println(lDelayTime);

  Serial.print("Time used: ");
  Serial.println(millis() - gs->ulLoopStartAt);
}

//EOF
