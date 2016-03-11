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
#include "comm.h"
#include "flow.h"
#include "handler.h"

// ESP8266 SDK headers are in C, they need to be included externally or else 
// they will throw an error compiling/linking.
//
// all SDK .c files required can be added between the { }
//extern "C" {
// #include "user_interface.h"
//}

// ------------------------------------------------------------------

String sPJDeviceId = "notinitialised";

state globalState;

// ------------------------------------------------------------------
// ------------------------------------------------------------------


// ------------------------------------------------------------------
void rightPinFalling ()
{
  Serial.println("** Falling wink from right pin.");
  globalState.bRightButtonDown = true;

  checkEasterEggMode(&globalState);
}

// ------------------------------------------------------------------
void rightPinRising ()
{
  Serial.println("** Rising wink from right pin.");
  globalState.bRightButtonDown = false;

  checkVote('R', &globalState);
}

// ------------------------------------------------------------------
void leftPinFalling ()
{
  Serial.println("** Falling wink from left pin.");
  globalState.bLeftButtonDown = true;

  checkEasterEggMode(&globalState);
}

// ------------------------------------------------------------------
void leftPinRising ()
{
  Serial.println("** Rising wink from left pin.");
  globalState.bLeftButtonDown = false;

  checkVote('L', &globalState);
}

// ------------------------------------------------------------------
void setupPins() 
{  
  attachInterrupt(digitalPinToInterrupt(LEFT_BUTTON), leftPinFalling, FALLING);
  attachInterrupt(digitalPinToInterrupt(LEFT_BUTTON), leftPinRising, RISING);
  attachInterrupt(RIGHT_BUTTON, rightPinFalling, FALLING);
  attachInterrupt(RIGHT_BUTTON, rightPinRising, RISING);

  pinMode(LED_LEFT_R, OUTPUT);
  pinMode(LED_LEFT_G, OUTPUT);
  pinMode(LED_LEFT_B, OUTPUT);

  digitalWrite(LED_LEFT_R, 0);
  digitalWrite(LED_LEFT_G, 0);
  digitalWrite(LED_LEFT_B, 0);

  digitalWrite(LED_RIGHT_R, 0);
  digitalWrite(LED_RIGHT_G, 0);
  digitalWrite(LED_RIGHT_B, 0);

  // analogWrite(pin, value) enables software PWM on the given pin. PWM may be used on pins 0 to 16. 
  // Call analogWrite(pin, 0) to disable PWM on the pin. value may be in range from 0 to PWMRANGE, 
  // which is equal to 1023 by default. PWM range may be changed by calling analogWriteRange(new_range).

  // we know 0 to 255 as colour values
  analogWriteRange(255);
  
  // PWM frequency is 1kHz by default. Call analogWriteFreq(new_frequency) to change the frequency.
  // 50 Hz is more than OK for us for now
  analogWriteFreq(50);
}

// ------------------------------------------------------------------
// is called once after powering up, sets up the wireless network 
// checks a few setting, especially if already registered
void setup() {

  // start a serial console for debugging purposes
  // todo: remove when going into production?
  Serial.begin(115200);
  Serial.println("");
  delay(1000);

  // initialise state
  globalState = (state) { .ulLoopStartAt = 0,
                          .ulLastModeChangeAt = 0,
                          .bytCurrentMode = MODE_INIT, 
                          .bytNextMode = MODE_NONE,
                          .cLeft = (char) 0, 
                          .cRight = (char) 0, 
                          .cVote = (char) 0,
                          .cLastButtons = (char) 0, 
                          .shPosLeft = 0, 
                          .shPosRight = 0, 
                          .bLeftButtonDown = false, 
                          .bRightButtonDown = false };

  // set all needed ports and disable unneeded output ports to save power
  setupPins();

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
  Serial.println("DeviceId: " + sPJDeviceId);

  delay(500);
  
  // check if registered already (check EEPROM), register with HARDAC otherwise.
  if(sPJDeviceId.length() <= 1)
  {
    Serial.println("sDeviceId is too small. " + sPJDeviceId);
  }
  else
  {
    // we currently only need one byte, but 4 is the minimum to register for
    EEPROM.begin(4);
  
    byte byRegKey = EEPROM.read(EEPROM_REGKEY_ADDRESS);
    
    Serial.print("EEPROM Value at EEPROM_REGKEY_ADDRESS: 0x");
    Serial.println(byRegKey, HEX);
    
    if(byRegKey != REGISTERED)
    {
      Serial.println("PJ is not registered yet.");
      registerPJ(sPJDeviceId);
    }
  }
}

void printWIFIStrength()
{
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

// ------------------------------------------------------------------
// will be called right after setup and then in a loop after every deep sleep cycle
// since we (currently) deep sleep after every cycle...
void loop() {

  globalState.ulLoopStartAt = millis();

  Serial.println("--");
  Serial.println("--------------------------------------------------------------------");
  Serial.println("Modes");
  Serial.print("- Current Mode: ");
  Serial.println(globalState.bytCurrentMode);
  Serial.print("- Next Mode: ");
  Serial.println(globalState.bytNextMode);

  // enable wifi if next mode needs it and current has not active already
  if((globalState.bytCurrentMode != MODE_VOTE && globalState.bytCurrentMode != MODE_UPDATE)
      && (globalState.bytNextMode == MODE_VOTE || globalState.bytNextMode == MODE_UPDATE))
  {
    //WiFi.forceSleepWake();
    Serial.println("Force Sleep Wake");
  }
  else
  {
    //WiFi.forceSleepBegin();
    Serial.println("Force Sleep Begin");
  }

  // switch modes?
  if(globalState.bytNextMode != MODE_NONE)
  {
    Serial.println("Switching modes");

    globalState.ulLastModeChangeAt = millis();

    // depending on the new mode, do the transformation
    globalState.bytCurrentMode = globalState.bytNextMode;
    globalState.bytNextMode = MODE_NONE;

    Serial.print("- Current Mode: ");
    Serial.println(globalState.bytCurrentMode);
    Serial.print("- Next Mode: ");
    Serial.println(globalState.bytNextMode);
  }

  switch(globalState.bytCurrentMode)
  {
    default:
    case MODE_IDLE:
      // sleep and wait, change mode to Update after N loops
      handleIdle(&globalState);
    break;
    case MODE_INIT:
      // Initialise something? otherwise head right into Update mode.
      globalState.bytNextMode = MODE_UPDATE;
      break;
    case MODE_UPDATE:
      printWIFIStrength();
      handleUpdate(&globalState);
      break;
    case MODE_MOVIE:
      handleMovie(&globalState);
      break;
    case MODE_VOTE:
      handleVote(&globalState);
      break;
    case MODE_EASTEREGG:
      handleEasterEgg(&globalState);
    break;
  }
  
  Serial.println();
  Serial.println("Ending loop");
  delay(1000);

  // go to deepsleep for 10 seconds
  //ESP.deepSleep(1 * 10 * 1000000, WAKE_RF_DEFAULT);
  // delay(1000);

  Serial.print("Time used: ");
  Serial.println(millis() - globalState.ulLoopStartAt);
}

