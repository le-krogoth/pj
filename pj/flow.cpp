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

void checkEasterEggMode(state *globalState)
{
  // if both buttons are down, we either leave or enter the easter egg mode
  if((*globalState).bLeftButtonDown && (*globalState).bRightButtonDown)  
  {
    (*globalState).bytNextMode = (*globalState).bytCurrentMode != MODE_EASTEREGG ? MODE_EASTEREGG : MODE_IDLE;
  }
}

void checkVote(char cVote, state *globalState)
{
  // if we do not leave nor join the easter egg mode, this is a vote then
  if((*globalState).bytCurrentMode != MODE_EASTEREGG && (*globalState).bytNextMode != MODE_EASTEREGG)
  {
    (*globalState).bytNextMode = MODE_VOTE;
    (*globalState).cVote = cVote;
  }
}
