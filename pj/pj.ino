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

// ESP8266 SDK headers are in C, they need to be included externally or else 
// they will throw an error compiling/linking.
//
// all SDK .c files required can be added between the { }
extern "C" {
#include "user_interface.h"
}

// change these to your setup
const char* ssid    = "hardac";
const char* pwd     = "hardac";

// where your hardac server is running
const char* server  = "10.10.10.10";
const int port      = 8765;

// ------------------------------------------------------------------
// used to turn off GPIO Pi
void turnOff(int pin) 
{
  pinMode(pin, OUTPUT);
  digitalWrite(pin, 1);
}

// ------------------------------------------------------------------
// is called once after powering up, sets up the wireless network 
// and reports status on serial console (for now)
void setup() {

  // start a serial console for debugging purposes
  // todo: remove when going into production?
  Serial.begin(115200);
  delay(500);

  // disable all unneeded output ports to save power
  turnOff(0);

  // get the wifi up and running
  WiFi.begin(ssid, pwd);

  Serial.print("Connecting to network: ");
  Serial.println(ssid);

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

  // todo: check if registered already (check EEPROM), register with HARDAC otherwise.
}

// ------------------------------------------------------------------
// will be called right after setup and then in a loop after every deep sleep cycle
void loop() {

  // todo: remove me when implementing deep sleep
  //delay(5000);

  // todo: check if button was pressed
  // if so, send ballot to server
  // otherwise just ask for current status

  Serial.print("Connecting to ");
  Serial.println(server);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  if (!client.connect(server, port)) {
    Serial.println("Connection failed, trying later...");
    return;
  }
  
  // We now create a URI for the request
  String url = "/device_id/s/";
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + server + "\r\n" + "Connection: close\r\n\r\n");
               
  // just a short delay to give other components the chance to do something else
  delay(10);
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available())
  {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("Closing connection, going to deep sleep");
  delay(1000);

  // go to deepsleep for one minutes
  system_deep_sleep_set_option(0);
  //system_deep_sleep(1 * 60 * 1000000);
  system_deep_sleep(1 * 10 * 1000000);
  delay(1000);

}

