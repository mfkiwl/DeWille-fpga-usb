/*==============================================================================
   DeWille ESP32 firmware

   A simple SPI bus arbiter. Deliberately uses Arduino SPI driver to allow 
   easier switch to a different module if required later on.

   Copyright 2020 Ivan Vasilev, Zmei Research Ltd.

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
  ============================================================================*/

//==============================================================================
//  Includes
//==============================================================================
#include <Arduino.h>
#include <SPI.h>

#include "globals.h"
#include "logger.h"

#include "spi.h"

//==============================================================================
//  Defines
//==============================================================================#
#define   INIT_DO_HIGH(x)       { pinMode((x), OUTPUT); digitalWrite((x), HIGH); }
#define   INIT_DO_LOW(x)        { pinMode((x), OUTPUT); digitalWrite((x), LOW); }

//==============================================================================
//  Local data
//==============================================================================#
static SPIClass *spi = NULL;
static bool initialized = false;

static const uint8_t csPins[eSpiDevNum] = { 
    DO_nCS_CLK, 
    DO_nCS_DACR, 
    DO_nCS_DACL, 
    DO_nCS_DUMMY }; // TODO

//==============================================================================
//  Local functions
//==============================================================================#

//==============================================================================
//  Exported functions
//==============================================================================#
eStatus SpiTransfer(eSpiDevice device, uint8_t * buffer, const uint8_t size)
{
    eStatus retVal = eOK;

    if ((device >= eSpiDevNum) || (size == 0) || (buffer == NULL))
    {
        retVal = eINVALIDARG;
    }
    else
    {   
        delay(1);
        spi->beginTransaction(SPISettings(SPI_CLK_FREQ, MSBFIRST, SPI_MODE0));
        digitalWrite(csPins[device], LOW);
        for (int i = 0; i < size; i++)
        {
            uint8_t tmp = spi->transfer(*buffer);
            *buffer++ = tmp;
        }
        digitalWrite(csPins[device], HIGH);
        spi->endTransaction();
        delay(1);
    }


    return retVal;
}

eStatus SpiInit()
{
    eStatus retVal = eOK;

    INIT_DO_HIGH(DO_nCS_CLK);
    INIT_DO_HIGH(DO_nCS_DACR);
    INIT_DO_HIGH(DO_nCS_DACL);

    //pinMode(SPI_MOSI, INPUT);

    spi = new SPIClass(VSPI);
    spi->begin(SPI_SCLK, SPI_MISO, SPI_MOSI, DO_nCS_DUMMY);

    initialized = true;

    return retVal;
}

