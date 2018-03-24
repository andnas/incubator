/*
  StripModes.h, StripModes.cpp realizes various algorithmes of static and dinamic effects of the strip lighting

  NOTES
    * Uses the Adafruit Neopixel library. Get it here:
      https://github.com/adafruit/Adafruit_NeoPixel 
  
  LICENSE
  The MIT License (MIT)
  Copyright (c) 2018  Andrei Nasonov
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
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE. 
*/

#ifndef Strip_Modes_h
#define Strip_Modes_h

#define DEBUG_SERIAL            1

#include "Arduino.h"

#ifndef MAX_PIXEL_COUNT
#define MAX_PIXEL_COUNT 100
#endif
#ifndef INITIAL_PIXEL_COUNT
#define INITIAL_PIXEL_COUNT 50
#endif

struct StripPersistentParameters {
  uint16_t pixelCount = INITIAL_PIXEL_COUNT;
  byte choisedModeIndex = 0;
  int8_t choisedPalette = 7; // 3, 7, 16 - количество выбранных цветов палитры
  int8_t choisedColor = 0;   // выбраный цвет в палитре (<0 - белый, 0..choisedPalette-1)
  int8_t darknessLevel = 0; // уровень затемнения (0 - brightest, 4 - darkest)
  int8_t speed = 0; // скорость переключения (0 - slowest, 4 - fastest)
};

class StripMode {
  public:
    virtual void init(void);
    virtual void start(void);
    virtual void fillStrip(void);
    virtual void fillStripSettingsMode(void);
    virtual void reshow(void);
    virtual void reshowSettingsMode(void);
    virtual void choiseSettingsMode();
    virtual void changeSettingsUp();
    virtual void changeSettingsDown();
};

class StripModes {
  private:
    int eepromAddress;
  public:
    void init(int eepromAddress, uint16_t pixelCount, uint8_t neoPixelPin);
    byte getChoisedModeIndex(void);
    void initMode(StripMode* mode);
    void setChoisedMode(StripMode* mode, byte choisedModeIndex);
    void choiseSettingsMode();
    void changeSettingsUp();
    void changeSettingsDown();
    void show(void);
    void saveCommonStripParams(void);
    void forceSavingAllStripParams(void);
};

class StripModeStatic : public StripMode {
  public:
    virtual void start(void);
    virtual void reshow(void);
    virtual void changeSettingsUp();
    virtual void changeSettingsDown();
};

class StripModeDinamic : public StripMode {
  public:
    virtual void start(void);
    virtual void reshow(void);
    virtual void changeSettingsUp();
    virtual void changeSettingsDown();
};

class StripModePersistent {
  public:
    virtual void save(void);
    //static byte eepromSize(void);
    virtual int eepromAddress(void);
};

enum StripModeWhiteMode {SHORT, MIDDLE, LONG};

class StripModeWhite : public StripModeStatic, public StripModePersistent {
  private:
    StripModeWhiteMode mode;
    byte pixelCountShort;
    byte pixelCountMiddle;
  public:
    StripModeWhite(void);
    virtual void init(void);
    virtual void save(void);
    virtual void fillStrip(void);
    virtual void fillStripSettingsMode(void);
    virtual void changeSettingsUp();
    virtual void changeSettingsDown();
    virtual int eepromAddress(void);
    //static byte eepromSize(void);
    #ifdef DEBUG_SERIAL
    virtual void debugSerial(void);
    #endif
};

#define STRIPMODE_EEPROMSIZE_WHITE sizeof(StripModeWhiteMode) + sizeof(byte)*2

#endif
