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
#ifndef H_MOVIEHELPER
#define H_MOVIEHELPER

#include <stdlib.h>
#include "flow.h"

void loadMovieFailed();
void loadMovieSucceeded();
void loadMovie(const char* movieLeft, const char* movieRight);

int getAnalogueValue(char colour);

// internal use only
void serialPrintMovieParsed();

// internal use only
movieframe *addMFLeft(const char r, const char g, const char b, const int position);
movieframe *addMFRight(const char r, const char g, const char b, const int position);

#endif // H_MOVIEHELPER
