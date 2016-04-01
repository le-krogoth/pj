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
#include "moviehelper.h"
#include "comm.h"
#include "global.h"
#include <ESP8266WiFi.h>

// ------------------------------------------------------------------
void handleUpdateReply(reply r)
{
  Serial.println("");
  Serial.println("Handling Update Reply");
      
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

  // { "colour_1": "F0020F0200F20002100120013001400150016001700180019001A001B001C001D001E001F001", "colour_2": "0F0200020F0200020F020002", "cycle_length": "1000" }
  if(r.iStatusCd != 0 && r.sReply.length() > 24)
  {
    // TODO: evaluate if we should switch to dynamic json buffer...
    // Free heap:43648
    StaticJsonBuffer<4096> jsonBuffer;
    //DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(r.sReply);
    
    const char* colourLeft = root["colour_1"];
    const char* colourRight = root["colour_2"];

    // the possibility to define how long a cycle is
    gs->iCycleLength = root["cycle_length"];

    Serial.print("colour1: ");
    Serial.println(colourLeft);
    Serial.print("colour2: ");
    Serial.println(colourRight);
    Serial.print("cycle_length: ");
    Serial.println(gs->iCycleLength);

    loadMovie(colourLeft, colourRight);
  }
  else
  {
    Serial.println("JSON seems not to be valid.");
    loadMovieFailed();
  }
}

// ------------------------------------------------------------------
void handleVoteReply(reply r)
{
  Serial.println("");
  Serial.println("Handling Vote Reply");
      
  if(r.iStatusCd == 202)
  {
    // tell user that it worked
    // 
    loadMovieSucceeded();
  }
  else
  {
    loadMovieFailed();
  }
}

// ------------------------------------------------------------------
void handleEasterEgg()
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
void handleIdle()
{
  
  if(millis() - gs->ulLastModeChangeAt >= (IDLE_MODE_LOOP_TIME * SECONDS_TO_MILLIS) )
  {
    Serial.println("Changing from idle to update");
    // if enough time passed, go into UPDATE mode
    gs->bytNextMode = MODE_UPDATE;
  }
}

int getAnalogueValue(char colour)
{
  char cHex[2];
  memset(cHex, 0x00, sizeof(cHex));
    
  cHex[0] = colour;
    
  int iVal = strtol(cHex, NULL, 16);
  Serial.print("Converting colour from: ");
  Serial.print(colour);
    
  int iAnalogVal = (iVal * 17);
  Serial.print(" to ");
  Serial.println(iAnalogVal);

  return iAnalogVal;
}

// ------------------------------------------------------------------
void handleMovie()
{
  Serial.println("Handle Movie");

  // find out if we have to change colour
  // for that: run through movie array, check movie.position, change colour when movie.position matches lMoviePosition
  for(int i = 0; i < gs->iMovieLeftFrameCount; i++)
  {
      int iPos = gs->movieLeft[i].position;
      if(iPos > gs->lMoviePosition)
      {
          Serial.println("iPos > lMoviePosition, breaking");
          break;
      }
      else if(iPos == gs->lMoviePosition)
      {
          Serial.println("BLINK");
          Serial.print("Element ");
          Serial.print(i);
          Serial.print(" has values R:");
          Serial.print(gs->movieLeft[i].r);
          Serial.print(" G:");
          Serial.print(gs->movieLeft[i].g);
          Serial.print(" B:");
          Serial.print(gs->movieLeft[i].b);
          Serial.print(" at ");
          Serial.println(gs->movieLeft[i].position);

          analogWrite(LED_LEFT_R, getAnalogueValue(gs->movieLeft[i].r));
          analogWrite(LED_LEFT_G, getAnalogueValue(gs->movieLeft[i].g));
          analogWrite(LED_LEFT_B, getAnalogueValue(gs->movieLeft[i].b));
          
          break;
      }
      else
      {
          //Serial.print(".");
      }
  }

  Serial.println("");

  // continue position to the next
  gs->lMoviePosition++;

  // if the movie is over, clean up and leave this mode
  if(gs->lMoviePosition >= gs->lMovieLength)
  {
    // TODO: fix memory allocation here?
    gs->lMoviePosition = 0;
    gs->iMovieLeftFrameCount = 0;
    gs->iMovieRightFrameCount = 0;
    gs->lMovieLength = 0;

    // clean LED
    analogWrite(LED_LEFT_R, 0);
    analogWrite(LED_LEFT_G, 0);
    analogWrite(LED_LEFT_B, 0);
    
    // if movie is over, go back to IDLE
    gs->bytNextMode = MODE_IDLE;      
  }
}

// ------------------------------------------------------------------
void handleUpdate()
{
  Serial.print("Handle Update: ");
  
  // ask HARDAC for status and actions
  String sUrl = "/" + sPJDeviceId + "/s/";
  Serial.println(sUrl);

  reply r = callUrl(sUrl);

  handleUpdateReply(r);
}

// ------------------------------------------------------------------
void handleVote()
{
  Serial.print("Handle Vote: ");
  
  // send ballot to HARDAC
  // /{device_id}/v/{ballot}
  
  String sUrl = "/" + sPJDeviceId + "/v/" + gs->sVote + "/";
  Serial.println(sUrl);
  
  reply r = callUrl(sUrl);

  handleVoteReply(r);
}
