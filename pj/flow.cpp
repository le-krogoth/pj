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

void checkVote(short sVote)
{
  Serial.println("Checking Vote");
  
  // if we do not leave nor join the easter egg mode, this is a vote then
  if(gs->bytCurrentMode != MODE_EASTEREGG && gs->bytNextMode != MODE_EASTEREGG)
  {
    Serial.print("Let's vote: ");
    Serial.println(sVote);
  
    gs->bytNextMode = MODE_VOTE;
    gs->sVote = sVote;
  }
  else
  {
    Serial.print("No voting, still in easter egg mode.");
    Serial.println("");
  }
}
