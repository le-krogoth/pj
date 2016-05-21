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
void printWIFIStrength()
{
  Serial.print("My IP is: ");
  Serial.print(WiFi.localIP());
  Serial.print("Signal strength (RSSI):");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
}

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

  // { "m1": "F0020002F0020002F0020002", "m2": "0F0200020F0200020F020002", "mrc": "0", "cl": "1000" }
  // m1: movie 1, m2: movie 2, mrc: movie replay count, cl: cycle length
  if(r.iStatusCd != 0 && r.sReply.length() > 24)
  {
    DynamicJsonBuffer jsonBuffer;
    Serial.println("JSON Buffer allocated");
    
    JsonObject& root = jsonBuffer.parseObject(r.sReply);
    Serial.println("JSON Object parsed");

    const char* colourLeft = root["m1"];
    Serial.println("m1 read");

    const char* colourRight = root["m2"];
    Serial.println("m2 read");

    // Movie replay count. 0 for endless
    const short mrc = root["mrc"];
    gs->sMovieReplayCount = mrc;
    Serial.println("mrc read");

    // the possibility to define how long a cycle is
    gs->iCycleLength = root["cl"];
    Serial.println("cl read");

    Serial.print("m1: ");
    Serial.println(colourLeft);
    Serial.print("m2: ");
    Serial.println(colourRight);
    Serial.print("mrc: ");
    Serial.println(gs->sMovieReplayCount);
    Serial.print("cl: ");
    Serial.println(gs->iCycleLength);

    playSuccess();
    loadMovie(colourLeft, colourRight);
  }
  else
  {
    playError();
    Serial.println("JSON seems not to be valid.");
  }
}

// ------------------------------------------------------------------
void handleVoteReply(reply r)
{
  Serial.println("");
  Serial.println("Handling Vote Reply");
      
  if(r.iStatusCd == 202)
  {
    playSuccess();
    Serial.println(" Vote sent successfully");
    gs->sVote = 0;
  }
  else
  {
    playError();
    Serial.println(" Vote send failed");
  }
}

// ------------------------------------------------------------------
void handleInput()
{
  Serial.print(" - handleEasterEgg() -> ");
  Serial.println(gs->ulEgg);
  
  // check buttons for easteregg mode - enter if both buttons are down
  // Interrupts are a bit fluky sometimes, so we clear buttons in here instead of raising interrupt

  if (gs->bLeftButtonDown && gs->bRightButtonDown)
  {
    if (gs->bytCurrentMode != MODE_EASTEREGG)
    {
      // go directly into easter egg mode
      gs->bytCurrentMode = MODE_EASTEREGG;
      gs->ulEgg = 0;

    } 
    else
    {     
      // leave easter egg mode as both buttons are pressed
      gs->bytCurrentMode = MODE_IDLE;
      gs->sVote = 0;
      playWarning();
    }
    
    // clear buttons
    gs->bLeftButtonDown = false;
    gs->bRightButtonDown = false;
  }

// check left button
  if (gs->bLeftButtonDown)
  {
    if (gs->bytCurrentMode == MODE_EASTEREGG)
    {
      //add left button to easter egg hunt
      gs->ulEgg = gs->ulEgg * 10 + 2; //left shift +2
    }
    
    // clear button as it has been handled
    gs->bLeftButtonDown = false;
  }

  // check right button
  if (gs->bRightButtonDown)
  {
    if (gs->bytCurrentMode == MODE_EASTEREGG)
    {
      //add right button to easer egg hunt
      gs->ulEgg = gs->ulEgg * 10 + 3; //left shift +3
    }
    
    // clear button as it has been handled
    gs->bRightButtonDown = false;
  }

  // check for winning code
  Serial.print(" the current egg is:");
  Serial.println(gs->ulEgg);

  bool foundsomething = false;

  //set new default movie & blink
  switch(gs->ulEgg)
  {
    case EGG_CODE_1:
      foundsomething = true;
      playStockMovie(1);
    break;
    case EGG_CODE_2:
      foundsomething = true;
      playStockMovie(2);
    break;
    case EGG_CODE_3:
      foundsomething = true;
      playStockMovie(3);
    break;
  }

  if(foundsomething == true)
  {
      Serial.println(" ### WIN ###");
      playSuccess();
      gs->sMovieReplayCount = 0;
      gs->ulEgg = 0;
      gs->bytCurrentMode = MODE_IDLE;

      // cheat, else go go directly into an update and overwrite the movie
      gs->ulLastModeUpdateAt = millis();
  }

  // reset if it got too big (4.294.967.295)
  if(gs->ulEgg > 99999999)
  {
    Serial.println("resetting easter egg hunter string");
    gs->ulEgg = 0;

    //blink for reset
    playWarning();
   }
}

// ------------------------------------------------------------------
void handleIdle()
{
  if(millis() - gs->ulLastModeUpdateAt >= (IDLE_MODE_LOOP_TIME * SECONDS_TO_MILLIS) )
  {
    Serial.println("Changing from IDLE -> UPDATE due to time");
    // if enough time passed, go into UPDATE mode
    gs->bytCurrentMode = MODE_UPDATE;
  }
}

// ------------------------------------------------------------------
void handleMovie()
{
  Serial.print("Handle Movie at position: ");
  Serial.println(gs->lMoviePosition);

  // find out if we have to change colour
  // for that: run through movie array, check movie.position, change colour when movie.position matches lMoviePosition
  for(int i = 0; i < gs->iMovieLeftFrameCount; i++)
  {
    int iPos = gs->movieLeft[i].position;
    if(iPos > gs->lMoviePosition)
    {
      Serial.println("iPos_Left > lMoviePosition, breaking");
      break; // for
    }
    else if(iPos == gs->lMoviePosition)
    {
      Serial.println("* BLINK LEFT *");
      Serial.print("  Element ");
      Serial.print(i);
      Serial.print(" has values R:");
      Serial.print(gs->movieLeft[i].r);
      Serial.print(" G:");
      Serial.print(gs->movieLeft[i].g);
      Serial.print(" B:");
      Serial.print(gs->movieLeft[i].b);
      Serial.print(" at position ");
      Serial.println(gs->movieLeft[i].position);

      analogWrite(LED_LEFT_R, 255 - convert2AnalogueValue(gs->movieLeft[i].r));
      analogWrite(LED_LEFT_G, 255 - convert2AnalogueValue(gs->movieLeft[i].g));
      analogWrite(LED_LEFT_B, 255 - convert2AnalogueValue(gs->movieLeft[i].b));

      break;
    }
    else
    {
      //Serial.print(".");
    }
  }

  delay(10);
  //right
    for(int i = 0; i < gs->iMovieRightFrameCount; i++)
    {
        int iPos = gs->movieRight[i].position;
        if(iPos > gs->lMoviePosition)
        {
            Serial.println("iPos_Right > lMoviePosition, breaking");
            break;
        }
        else if(iPos == gs->lMoviePosition)
        {
            Serial.println("* BLINK RIGHT *");
            Serial.print("  Element ");
            Serial.print(i);
            Serial.print(" has values R:");
            Serial.print(gs->movieRight[i].r);
            Serial.print(" G:");
            Serial.print(gs->movieRight[i].g);
            Serial.print(" B:");
            Serial.print(gs->movieRight[i].b);
            Serial.print(" at position ");
            Serial.println(gs->movieRight[i].position);

            //inverted LED intensity 255-
            analogWrite(LED_RIGHT_R, 255 - convert2AnalogueValue(gs->movieRight[i].r));
            analogWrite(LED_RIGHT_G, 255 - convert2AnalogueValue(gs->movieRight[i].g));
            analogWrite(LED_RIGHT_B, 255 - convert2AnalogueValue(gs->movieRight[i].b));

            break;
        }

    }

    delay(10);
    Serial.println("");

  // continue position to the next
  gs->lMoviePosition++;

  // if the movie is over, clean up and reset to beginning of movie (endless loop)
  if(gs->lMoviePosition >= gs->lMovieLength)
  {
    // TODO: fix memory allocation here?
    gs->lMoviePosition = 0;

    if (gs->sMovieReplayCount == 1)
    {
      // we played the movie as long as requested, lets revert to the default one
      Serial.println("Switching movie because replay count done");
      playStockMovie(gs->sStockMovieActiveIndex);
    }

    if (gs->sMovieReplayCount > 0)
    {
        gs->sMovieReplayCount--;
    }
  }
}

// ------------------------------------------------------------------
void goOnline()
{
  Serial.print("Handle Update: ");

  // this is the only function that needs Wifi
  // Wakeup Wifi, WiFi.forceSleepWake(); didnt bring too much saving
  if(WiFi.status() != WL_CONNECTED)
  {
    Serial.println(" #-# starting wifi #-#");
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, PWD);
    delay(10);
   }

  // wait till connection is up
  unsigned long lStart = millis();
  while (WiFi.status() != WL_CONNECTED && lStart + (10 * SECONDS_TO_MILLIS) > millis())
  {
      delay(500);
      Serial.print(".");
  }
  // now it should be up, print status of wifi
  printWIFIStrength();

  handleUpdate();

  delay(10);
  //also handleVote now, if there is one
  if (gs->sVote != EMPTY_VOTE)
  {
    handleVote();
  }

    // shutdown wifi here, disconnect client to save power
  Serial.println(" #-# shutting down Wifi #-# ");
  WiFi.disconnect();
  delay(100);
  WiFi.mode(WIFI_OFF);
  delay(100);
}

void handleUpdate()
{
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
