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
#ifndef H_FLOW
#define H_FLOW

#include <stdlib.h>

typedef unsigned char byte;

// States
static const byte MODE_NONE       = 0x00;
static const byte MODE_INIT       = 0x01;
static const byte MODE_IDLE       = 0x02;
static const byte MODE_UPDATE     = 0x03;
static const byte MODE_MOVIE      = 0x04;
static const byte MODE_VOTE       = 0x05;
static const byte MODE_EASTEREGG  = 0x06;

// one element of a movie
typedef struct movieframe
{
    char r;
    char g;
    char b;
    short position;
} movieframe;

// state structure for state machine
typedef struct state
{
  unsigned long ulLoopStartAt;
  unsigned long ulLastModeChangeAt;
  byte bytCurrentMode;
  byte bytNextMode;
  int iMovieLeftFrameCount;
  int iMovieLeftFrameCapacity;
  movieframe *movieLeft;
  int iMovieRightFrameCount;
  int iMovieRightFrameCapacity;
  movieframe *movieRight;
  long lMoviePosition;
  long lMovieLength;
  char cVote;
  char* cLastButtons;
  short shPosLeft;
  short shPosRight;
  bool bLeftButtonDown;
  bool bRightButtonDown;
} state;

extern state *gs;

movieframe *addMFLeft(const char r, const char g, const char b, const short position);
movieframe *addMFRight(const char r, const char g, const char b, const short position);
void checkEasterEggMode();
void checkVote(char cVote);

#endif // H_FLOW
