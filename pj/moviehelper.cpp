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
#include "moviehelper.h"
#include "global.h"
//#include <ESP8266WiFi.h>

void loadMovieFailed()
{
  loadMovie("F0020002F0020002F0020002", "F0020002F0020002F0020002"); 
}

void loadMovieSucceeded()
{
  loadMovie("0F0200020F0200020F020002", "0F0200020F0200020F020002");
}

void loadMovie(const char* movieLeft, const char* movieRight)
{
    int iMovieLeftLength = strlen(movieLeft);
    int iMovieLeftElements = iMovieLeftLength / 4;
    
    int pos = 0;
    short location = 0;

    for(int i = 0; i < iMovieLeftElements; i++)
    {
        movieframe *temp;

        temp = addMFLeft(movieLeft[pos], movieLeft[pos + 1], movieLeft[pos + 2], location);

        location += (movieLeft[pos + 3] - '0');

        pos += 4;
    }

    delay(10);

    int iMovieRightLength = strlen(movieRight);
    int iMovieRightElements = iMovieRightLength / 4;
    
    pos = 0;
    location = 0;

    for(int i = 0; i < iMovieRightElements; i++)
    {
        movieframe *temp;

        temp = addMFRight(movieRight[pos], movieRight[pos + 1], movieRight[pos + 2], location);

        location += (movieRight[pos + 3] - '0');

        pos += 4;
    }

    delay(10);

    // have parsed movie printed to the serial console
    serialPrintMovieParsed();

    delay(10);

    // calculate length of whole movie, take bigger number
    gs->lMovieLength = gs->iMovieLeftFrameCount > gs->iMovieRightFrameCount ? gs->iMovieLeftFrameCount : gs->iMovieRightFrameCount;

    // reset movie to start position
    gs->lMoviePosition = 0;
    gs->bytNextMode = MODE_MOVIE;
}

void serialPrintMovieParsed()
{
  Serial.println("Left Movie file was parsed like that:");
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

  Serial.println("Right Movie file was parsed like that:");
  for(int i = 0; i < gs->iMovieRightFrameCount; i++)
  {
      Serial.print("Element ");
      Serial.print(i);
      Serial.print(" has values R:");
      Serial.print(gs->movieRight[i].r);
      Serial.print(" G:");
      Serial.print(gs->movieRight[i].g);
      Serial.print(" B:");
      Serial.print(gs->movieRight[i].b);
      Serial.print(" at ");
      Serial.println(gs->movieRight[i].position);
  }

  Serial.print("iMovieRightFrameCapacity: ");
  Serial.println(gs->iMovieRightFrameCapacity);
  
  Serial.print("MovieRightFrameCount: ");
  Serial.println(gs->iMovieRightFrameCount);
}


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
