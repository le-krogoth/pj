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

  // { "colour_1": "R", "colour_2": "B", "idle_time": "cycles?" }
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

    char* cMovie = "F0090F0900F9";
    
    int iMovieLength = strlen(cMovie);
    int iMovieElements = iMovieLength / 4;
    
    int pos = 0;
    short location = 0;

    for(int i = 0; i < iMovieElements; i++)
    {
        movieframe *temp;

        temp = addMFLeft(cMovie[pos], cMovie[pos + 1], cMovie[pos + 2], location);

        location += (cMovie[pos + 3] - '0');

        pos += 4;
    }

    delay(10);
    
    Serial.println("Movie file was parsed like that:");
    for(int i = 0; i < gs->iMovieLeftFrameCount; i++)
    {
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
    }

    Serial.print("iMovieLeftFrameCapacity: ");
    Serial.println(gs->iMovieLeftFrameCapacity);
    
    Serial.print("MovieLeftFrameCount: ");
    Serial.println(gs->iMovieLeftFrameCount);

    gs->lMovieLength = gs->iMovieLeftFrameCount > gs->iMovieRightFrameCount ? gs->iMovieLeftFrameCount : gs->iMovieRightFrameCount;

    gs->lMoviePosition = 0;
    gs->bytNextMode = MODE_MOVIE;

    Serial.println("Leaving handling function.");

  }
  else
  {
    Serial.println("JSON seems not to be valid.");
    gs->bytNextMode = MODE_IDLE;
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
    gs->bytNextMode = MODE_IDLE;
  }
  else
  {
    loadMovieFailed();    
    gs->bytNextMode = MODE_IDLE;
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
            break;
        }
        else
        {
            Serial.print(".");
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
    
    // if movie is over, go back to IDLE
    gs->bytNextMode = MODE_IDLE;      
  }
}

// ------------------------------------------------------------------
void handleUpdate()
{
  Serial.println("Handle Update");
  
  // ask HARDAC for status and actions
  String sUrl = "/" + sPJDeviceId + "/s/";
  reply r = callUrl(sUrl);

  handleUpdateReply(r);
}

// ------------------------------------------------------------------
void handleVote()
{
  Serial.println("Handle Vote");
  
  // send ballot to HARDAC
  // /{device_id}/v/{ballot}
  
  String sUrl = "/" + sPJDeviceId + "/v/" + gs->cVote + "/";
  reply r = callUrl(sUrl);

  handleVoteReply(r);
}
