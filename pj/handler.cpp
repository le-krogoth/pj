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
#include "handler.h"
#include "flow.h"
#include "comm.h"
#include "global.h"
#include <ESP8266WiFi.h>

// ------------------------------------------------------------------
void handleReply(reply r, state *globalState)
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
  if(r.iStatusCd != 0 && r.sReply.length() > 24)
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

  // be sure to move back to IDLE mode after handling the reply
  // TODO: this line is in here because I am unsure if we only
  // should go back to IDLE mode if everything worked...
  (*globalState).bytNextMode = MODE_IDLE;
}

// ------------------------------------------------------------------
void handleEasterEgg(state *globalState)
{
  Serial.println("Handle Easter Egg");
  
  unsigned long lTestLoop = millis();
  bool bOn = true;
  while (lTestLoop + 20000 > millis()) 
  {
    if(bOn)
    {    
      digitalWrite(LED_LEFT_R, 20);
      digitalWrite(LED_LEFT_B, 200);
    }
    else
    {
      digitalWrite(LED_LEFT_R, 0);
      digitalWrite(LED_LEFT_B, 0);
    }

    bOn = !bOn;
    delay(500);
    Serial.print(bOn);
  }  
}

// ------------------------------------------------------------------
void handleIdle(state *globalState)
{
  
  if(millis() - (*globalState).ulLastModeChangeAt >= (IDLE_MODE_LOOP_TIME * SECONDS_TO_MILLIS) )
  {
    Serial.println("Changing from idle to update");
    // if enough time passed, go into UPDATE mode
    (*globalState).bytNextMode = MODE_UPDATE;
  }
}

// ------------------------------------------------------------------
void handleUpdate(state *globalState)
{
  Serial.println("Handle Update");
  
  // ask HARDAC for status and actions
  String sUrl = "/" + sPJDeviceId + "/s/";
  reply r = callUrl(sUrl);

  handleReply(r, globalState);
}

// ------------------------------------------------------------------
void handleMovie(state *globalState)
{
  Serial.println("Handle Movie");

  // if movie is over, go back to IDLE
  (*globalState).bytNextMode = MODE_IDLE;
}

// ------------------------------------------------------------------
void handleVote(state *globalState)
{
  Serial.println("Handle Vote");
  
  // ask HARDAC for status and actions
  //String sUrl = "/" + sPJDeviceId + "/v/" + cVote + "/";
  //reply r = callUrl(sUrl);

  //handleReply(r);
  (*globalState).bytNextMode = MODE_IDLE;
}
