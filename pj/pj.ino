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
#include <ArduinoJson.h> // json parser
#include <EEPROM.h> // to store status/registration in

// ESP8266 SDK headers are in C, they need to be included externally or else 
// they will throw an error compiling/linking.
//
// all SDK .c files required can be added between the { }
//extern "C" {
//}

// change these to your setup
const char* SSID  = "hardac";
const char* PWD   = "hardac";

// where your hardac server is running
const char* SERVER  = "0.0.0.0";
const int PORT      = 8765;

// EEPROM addressing
const int EEPROM_REGKEY_ADDRESS = 0;
// value if registered
const byte REGISTERED = 0x5F;

// http reply struct
struct REPLY 
{
  int iStatusCd;
  String sReply;
};
typedef struct REPLY reply;

// device_id
String sDeviceId = "notinitialised";

// ------------------------------------------------------------------
// used to turn off GPIO Pi
void turnOff(int pin) 
{
  pinMode(pin, OUTPUT);
  digitalWrite(pin, 1);
}

// ------------------------------------------------------------------
// call URL, return statuscode and JSON
reply callUrl(String sUrl)
{
  reply r;

  r.iStatusCd = 0;
  r.sReply = "";
  
  Serial.print("Connecting to ");
  Serial.println(SERVER);
  
  // Use WiFiClient class to create TCP connections
  // todo: change to WiFiClientSecure when ready so as to make sniffing a bit more complex
  WiFiClient client;

  // todo: we should probably wait here or sleep here until we can connect
  // so we do not drain the battery too much
  if (!client.connect(SERVER, PORT)) {
    Serial.println("Connection failed, trying later...");
    return r;
  }
  
  // We now create a URI for the request
  Serial.print("Requesting URL: ");
  Serial.println(sUrl);
  
  // This will send the request to the server
  client.print(String("GET ") + sUrl + " HTTP/1.1\r\n" + "Host: " + SERVER + "\r\n" + "Connection: close\r\n\r\n");
               
  // just a short delay to give other components the chance to do something else
  delay(100);

  // handling of response
  Serial.println("");
  Serial.println("Response:");
  
  // Read line by line
  while(client.available())
  {
    String sLine = client.readStringUntil('\r');

    // make sure to not have whitespaces so that the very basic parser will not fail
    Serial.print(sLine);
    sLine.trim();

    if (sLine.startsWith("HARDAC-STATUS: ")) 
    {
        String sStatus = sLine.substring(14, 18);

        r.iStatusCd = (int)sStatus.toInt();
    } 
    // todo: how can we detect the JSON payload?
    else if (sLine.startsWith("{ \"colour_1\": ")) 
    {
      r.sReply = sLine;
    }
  }

  Serial.println("EOF.");
  if (!client.connected()) 
  {
    // free up memory
    client.stop();
  }
  
  return r;
}

// ------------------------------------------------------------------
bool registerPJ()
{
  // register with backend  
  String sUrl = "/" + sDeviceId + "/r/public_key/";
  reply r = callUrl(sUrl);

  if(r.iStatusCd != 201)
  {
    Serial.print("Registration of PJ failed with StatusCode: ");
    Serial.println(r.iStatusCd);
  }
  else
  {
    // registration successful, make sure, that device knows after reboot
    EEPROM.write(EEPROM_REGKEY_ADDRESS, REGISTERED);
    EEPROM.commit();
    Serial.println("Registration of PJ succeeded.");
  }
}

// todo: unregister when asked by backend
bool unregisterPJ()
{
}

// ------------------------------------------------------------------
// is called once after powering up, sets up the wireless network 
// checks a few setting, especially if already registered
void setup() {

  // start a serial console for debugging purposes
  // todo: remove when going into production?
  Serial.begin(115200);
  delay(500);

  // disable all unneeded output ports to save power
  turnOff(0);

  // get the wifi up and running
  WiFi.begin(SSID, PWD);

  Serial.print("Connecting to network: ");
  Serial.println(SSID);

  // todo: this loops until things get connected to the wireless network
  // should we cancel after a few attempts and sleep instead to not drain
  // the battery too much when out of reach?
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
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
  sDeviceId = String(ESP.getChipId()) + "_" + String(ESP.getFlashChipId());
  Serial.println("DeviceId: " + sDeviceId);

  delay(500);
  
  // check if registered already (check EEPROM), register with HARDAC otherwise.
  if(sDeviceId.length() <= 1)
  {
    Serial.println("sDeviceId is too small. " + sDeviceId);
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
      registerPJ();
    }
  }
}

// ------------------------------------------------------------------
// todo: this generic method will become a handle response to status message function
// parse json response, act accordingly
// checking on statuscode makes no sense like that
void handleReply(reply r)
{
  Serial.println("");
  Serial.println("Handling Reply");
      
  switch (r.iStatusCd) 
  {
    case 200:
      Serial.println("Request successfull.");
      break;
    case 201:
      Serial.println("Registration successfull.");
      break;
    case 202:
      Serial.println("Ballot sent successfully.");
      break;
    case 400:
      Serial.println("Not all parameters were given.");
      break;
    case 403:
      Serial.println("Function called without being registered.");
      break;
    case 420:
      Serial.println("Re-registration attempt, HARDAC tells us to calm down.");
      break;
    default:
      Serial.println("That status code is not known (yet): '" + String(r.iStatusCd) + "' with line: " + r.sReply);
    break;
  }

  // { "colour_1": "R", "colour_2": "B" }
  if(r.sReply.length() > 24)
  {
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(r.sReply);
    
    const char* colour1 = root["colour_1"];
    const char* colour2 = root["colour_2"];
  
    Serial.print("colour1: ");
    Serial.println(colour1);
    Serial.print("colour2: ");
    Serial.println(colour2);
  }
  else
  {
    Serial.println("JSON seems not to be valid.");
  }
}

// ------------------------------------------------------------------
// will be called right after setup and then in a loop after every deep sleep cycle
// since we (currently) deep sleep after every cycle...
void loop() {

  // todo: remove me when implementing deep sleep
  // delay(5000);

  // todo: check if button was pressed
  // if so, send ballot to server
  // otherwise just ask for current status
  // we also could just skip the status every other round

  // ask HARDAC for status and actions
  String sUrl = "/" + sDeviceId + "/s/";
  reply r = callUrl(sUrl);

  handleReply(r);
  
  Serial.println();
  Serial.println("Closing connection, going to deep sleep");
  delay(1000);

  // go to deepsleep for 10 seconds
  ESP.deepSleep(1 * 10 * 1000000, WAKE_RF_DEFAULT);
  delay(1000);
}

