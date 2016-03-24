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
#include "flow.h"
#include "global.h"
#include <ESP8266WiFi.h>

movieframe *addMFLeft(const char r, const char g, const char b, const short position)
{
    if (gs->iMovieLeftFrameCount == gs->iMovieLeftFrameCapacity)
    {
        // TODO: define growth size
        gs->iMovieLeftFrameCapacity += 2;

        gs->movieLeft = (struct movieframe *)realloc(gs->movieLeft, sizeof(state) * gs->iMovieLeftFrameCapacity);
    }

    movieframe *mf = (gs->movieLeft + gs->iMovieLeftFrameCount);
    mf->r = r;
    mf->g = g;
    mf->b = b;
    mf->position = position;

    gs->iMovieLeftFrameCount++;

    return mf;
}

movieframe *addMFRight(const char r, const char g, const char b, const short position)
{
    if (gs->iMovieRightFrameCount == gs->iMovieRightFrameCapacity)
    {
        // TODO: define growth size
        gs->iMovieRightFrameCapacity += 2;

        gs->movieRight = (struct movieframe *)realloc(gs->movieRight, (sizeof(state) * gs->iMovieRightFrameCapacity));
    }

    movieframe *mf = (gs->movieRight + gs->iMovieRightFrameCount);
    mf->r = r;
    mf->g = g;
    mf->b = b;
    mf->position = position;

    gs->iMovieRightFrameCount++;

    return mf;
}

void checkEasterEggMode()
{
  Serial.println("Checking Easter Egg Mode");
  Serial.print("Left button: ");
  Serial.print(gs->bLeftButtonDown);
  Serial.print("Right button: ");
  Serial.println(gs->bRightButtonDown);
    
  // if both buttons are down, we either leave or enter the easter egg mode
  if(gs->bLeftButtonDown && gs->bRightButtonDown)  
  {
    Serial.println("Both buttons down");
    Serial.println(gs->bytCurrentMode != MODE_EASTEREGG ? MODE_EASTEREGG : MODE_IDLE);

    gs->bytNextMode = gs->bytCurrentMode != MODE_EASTEREGG ? MODE_EASTEREGG : MODE_IDLE;
  }
}

void checkVote(char cVote)
{
  Serial.println("Checking Vote");
  
  // if we do not leave nor join the easter egg mode, this is a vote then
  if(gs->bytCurrentMode != MODE_EASTEREGG && gs->bytNextMode != MODE_EASTEREGG)
  {
    Serial.print("Let's vote: ");
    Serial.println(cVote);
  
    gs->bytNextMode = MODE_VOTE;
    gs->cVote = cVote;
  }
  else
  {
    Serial.print("No voting, still in easter egg mode.");
    Serial.println("");
  }
}
