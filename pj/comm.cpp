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
#include "comm.h"
#include "global.h"
#include "moviehelper.h"
#include <ESP8266WiFi.h>
#include <EEPROM.h> // to store status/registration in

// ------------------------------------------------------------------
bool registerPJ(String sDeviceId)
{
  // register with backend  
  String sUrl = "/" + sDeviceId + "/r/public_key/";
  reply r = callUrl(sUrl);

  switch (r.iStatusCd)
    {
      case 420:
        // calm down, reset the eeprom in case of failed sync
        // TODO: Find out why this can happen
        EEPROM.write(EEPROM_REGKEY_ADDRESS, REGISTERED);
        EEPROM.commit();
        Serial.println("HARDAC says this device was registered before");
      break;
      case 201:
        // registration successful, make sure, that device knows after reboot
        EEPROM.write(EEPROM_REGKEY_ADDRESS, REGISTERED);
        EEPROM.commit();
        Serial.println("Registration of PJ succeeded.");
      break;
      default:
        Serial.print("Registration of PJ failed with StatusCode: ");
        Serial.println(r.iStatusCd);
      break;
    }
}

// todo: unregister when asked by backend
bool unregisterPJ(String sDeviceId)
{
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
  delay(50);

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
    else if (sLine.startsWith("{ \"m1\": "))
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

byte readWifiBootEeprom()
{
  Serial.print("readWifiModeEeprom() ");

  byte byMagicNumber = 0;

  EEPROM.get(EEPROM_WIFITOGGLE_ADDRESS, byMagicNumber);

  Serial.print("Magic Number: ");
  Serial.println(byMagicNumber, HEX);

  if(byMagicNumber == BOOTWIFI_OFF)
  {
    return BOOTWIFI_OFF;
  }
  else
  {
    return BOOTWIFI_ON;
  }
}


void writeWifiBootEeprom(const byte byMagicNumber)
{
  //toggle if we should deepsleep when wifi is not found
  Serial.print("writeWifiModeEeprom() ");

  EEPROM.put(EEPROM_WIFITOGGLE_ADDRESS, byMagicNumber);
  EEPROM.commit();

  Serial.print("Magic Number: ");
  Serial.println(byMagicNumber,HEX);
}

//EOF