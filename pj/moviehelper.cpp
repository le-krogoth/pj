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
#include <EEPROM.h> // to store status/registration in

void playWarning()
{
  for(int j=0; j < ERROR_SUCCESS_BLINKCOUNT; j++ )
  {
    digitalWrite(LED_RIGHT_B, LOW);
    delay(50);
    digitalWrite(LED_RIGHT_B, HIGH);
    delay(50);
  }
}

void playSuccess()
{
  for(int j=0; j < ERROR_SUCCESS_BLINKCOUNT; j++ )
  {
    digitalWrite(LED_RIGHT_G, LOW);
    delay(50);
    digitalWrite(LED_RIGHT_G, HIGH);
    delay(50);
  }
}

void playError()
{
  for (int j=0; j < ERROR_SUCCESS_BLINKCOUNT; j++ )
  {
    digitalWrite(LED_RIGHT_R, LOW);
    delay(50);
    digitalWrite(LED_RIGHT_R, HIGH);
    delay(50);
  }
}

void playBootError()
{
  for (int j=0; j < ERROR_SUCCESS_BLINKCOUNT; j++ )
  {
    digitalWrite(LED_LEFT_R, LOW);
    digitalWrite(LED_RIGHT_R, LOW);
    delay(50);
    digitalWrite(LED_LEFT_R, HIGH);
    digitalWrite(LED_RIGHT_R, HIGH);
    delay(50);
    digitalWrite(LED_LEFT_B, LOW);
    digitalWrite(LED_RIGHT_B, LOW);
    delay(50);
    digitalWrite(LED_RIGHT_B, HIGH);
    digitalWrite(LED_LEFT_B, HIGH);
    delay(50);
  }
}

void playStockMovie(const unsigned short sStockMovieIndex)
{
  if(sStockMovieIndex < STOCK_MOVIE_ARRAY_SIZE)
  {
    storeActiveMovieToEEPROM(sStockMovieIndex);
    loadMovie(STOCK_MOVIES_LEFT[sStockMovieIndex].c_str(), STOCK_MOVIES_RIGHT[sStockMovieIndex].c_str());
  }
  else
  {
    Serial.println("Movie could not be loaded because Index was bigger than StockMovie Array.");
  }
}

void loadMovie(const char* movieLeft, const char* movieRight)
{
  Serial.println("loadingMovies:");
  Serial.println(movieLeft);
  Serial.println(movieRight);

  // reset movie counters
  gs->iMovieLeftFrameCount = 0;
  gs->iMovieRightFrameCount = 0;
  gs->lMovieLength = 0;
    
  int iMovieLeftLength = strlen(movieLeft);
  int iMovieLeftElements = iMovieLeftLength / 4;
    
    int pos = 0;
    int iLocationLeft = 0;

    for(int i = 0; i < iMovieLeftElements; i++)
    {
        movieframe *temp;

        temp = addMFLeft(movieLeft[pos], movieLeft[pos + 1], movieLeft[pos + 2], iLocationLeft);

        iLocationLeft += hex2dec(movieLeft[pos + 3]);

        pos += 4;
    }

    delay(10);

    int iMovieRightLength = strlen(movieRight);
    int iMovieRightElements = iMovieRightLength / 4;
    
    pos = 0;
    int iLocationRight = 0;

    for(int i = 0; i < iMovieRightElements; i++)
    {
        movieframe *temp;

        temp = addMFRight(movieRight[pos], movieRight[pos + 1], movieRight[pos + 2], iLocationRight);

        iLocationRight += hex2dec(movieRight[pos + 3]);

        pos += 4;
    }

    delay(10);

    // have parsed movie printed to the serial console
    serialPrintMovieParsed();

    delay(10);

    Serial.print("iLocationLeft:");
    Serial.println(iLocationLeft);
    Serial.print("iLocationRight:");
    Serial.println(iLocationRight);

    // calculate length of whole movie, take bigger number
    gs->lMovieLength = iLocationLeft > iLocationRight ? iLocationLeft : iLocationRight;

    Serial.print("Movielength: ");
    Serial.println(gs->lMovieLength);

    // reset movie to start position
    gs->lMoviePosition = 0;
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


movieframe *addMFLeft(const char r, const char g, const char b, const int position)
{
    if (gs->iMovieLeftFrameCount == gs->iMovieLeftFrameCapacity)
    {
        // TODO: define growth size
        gs->iMovieLeftFrameCapacity += MOVIE_MEMORY_GROWTH_SIZE;

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

movieframe *addMFRight(const char r, const char g, const char b, const int position)
{
    if (gs->iMovieRightFrameCount == gs->iMovieRightFrameCapacity)
    {
        // TODO: define growth size
        gs->iMovieRightFrameCapacity += MOVIE_MEMORY_GROWTH_SIZE;

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

int hex2dec(char cHexVal)
{
  char cHex[2];
  memset(cHex, 0x00, sizeof(cHex));

  cHex[0] = cHexVal;

  int iVal = strtol(cHex, NULL, 16);
  Serial.print("Converting hex from: ");
  Serial.print(cHexVal);

  Serial.print(" to dec: ");
  Serial.println(iVal);

  return iVal;
}

int convert2AnalogueValue(char colour)
{
  int iVal = hex2dec(colour);
  Serial.print("Converting colour from: ");
  Serial.print(colour);
    
  int iAnalogVal = (iVal * 17);
  Serial.print(" to ");
  Serial.println(iAnalogVal);

  return iAnalogVal;
}

void storeActiveMovieToEEPROM(const unsigned short sStockMovieIndex)
{
  // store ID, movie index
  Serial.println("storeActiveMovie()");

  //  EEPROM.length() not required as we dont save dynamic content
  int eeAddress = EEPROM_MOVIE_ADDRESS;
  EEPROM.put(eeAddress, MAGICMOVIE);
  eeAddress +=sizeof(MAGICMOVIE);
  EEPROM.put(eeAddress, sStockMovieIndex);
  eeAddress +=sizeof(sStockMovieIndex);

  EEPROM.commit();
  Serial.print("Written to EEPROM: ");
  Serial.print(eeAddress - EEPROM_MOVIE_ADDRESS);
  Serial.println(" bytes");

  //save new movie as soft default as well
  Serial.println("updated ActiveMovies");
  gs->sStockMovieActiveIndex = sStockMovieIndex;
}

void loadActiveMovieFromEEPROM()
{
  Serial.println("loadActiveMovie()");

  byte byMagicNumber = 0;
  int iEEAddress = EEPROM_MOVIE_ADDRESS;

  EEPROM.get(iEEAddress, byMagicNumber);
  iEEAddress +=sizeof(byMagicNumber);

  unsigned short sStockMovieIndex;

  // memory seems correct, lets load the stored movie
  if(byMagicNumber == MAGICMOVIE)
  {
    EEPROM.get(iEEAddress, sStockMovieIndex);

    Serial.print("Loaded the following movie index:");
    Serial.println(sStockMovieIndex);
  }
  else
  {
    Serial.print("Error reading movie from EEPROM:");
    Serial.println(byMagicNumber, HEX);
    // load default movie instead
    sStockMovieIndex = 0;
  }

  if(sStockMovieIndex < STOCK_MOVIE_ARRAY_SIZE)
  {
    playStockMovie[sStockMovieIndex];
  }
  else
  {
    Serial.println("Movie could not be loaded because Index was bigger than StockMovie Array.");
  }
}

// EOF
