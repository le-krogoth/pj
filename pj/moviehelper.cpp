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
  loadMovie("F0020002F0020002F0020002"); 
}

void loadMovieSucceeded()
{
  loadMovie("0F0200020F0200020F020002");
}

void loadMovie(char* movie)
{
  
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
