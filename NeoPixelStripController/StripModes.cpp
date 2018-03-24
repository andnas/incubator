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

#include <EEPROM.h>
#include <Adafruit_NeoPixel.h>
#include "StripModes.h"
#include "palette.h"
#include "SimpleTimer.h"

StripModes stripModes = StripModes();
Adafruit_NeoPixel pixels;
StripPersistentParameters commonStripParams;
StripMode* choisedMode;
byte choisedSettingsMode;

boolean needSaveCommonStripParams = 0;
boolean needSaveChoisedModeParams = 0;

void saveStripParams(void) {
  if (needSaveCommonStripParams) {
    needSaveCommonStripParams = false;
    #ifdef DEBUG_SERIAL
    Serial.print("stripModes.saveCommonStripParams()... ");
    #endif
    stripModes.saveCommonStripParams();
    #ifdef DEBUG_SERIAL
    Serial.println("OK");
    #endif
  }
  if (needSaveChoisedModeParams) {
    needSaveChoisedModeParams = false;
    StripModePersistent* persistentMode = (StripModePersistent*) choisedMode;
    if (persistentMode != 0) {
      #ifdef DEBUG_SERIAL
      Serial.print("stripModes.saveCommonStripParams()... ");
      #endif
      persistentMode->save();
      #ifdef DEBUG_SERIAL
      Serial.println("OK");
      #endif
    }
  }
}

void reshowChoisedMode(void) {
  choisedMode->reshow();
}

SimpleTimer stripParamsSavingTimer = SimpleTimer(&saveStripParams, 5000, false);
SimpleTimer choisedModeReshowTimer = SimpleTimer(&reshowChoisedMode, 1000, false);

void setNeedSaveCommonStripParams(void) {
  needSaveCommonStripParams = 1;
  stripParamsSavingTimer.start();
  #ifdef DEBUG_SERIAL
  Serial.print("CommonStripParams");
  Serial.print(": pixelCount="); Serial.print(commonStripParams.pixelCount);
  Serial.print(", choisedModeIndex="); Serial.print(commonStripParams.choisedModeIndex);
  Serial.print(", choisedPalette="); Serial.print(commonStripParams.choisedPalette);
  Serial.print(", choisedColor="); Serial.print(commonStripParams.choisedColor);
  Serial.print(", darknessLevel="); Serial.print(commonStripParams.darknessLevel);
  Serial.print(", speed="); Serial.print(commonStripParams.speed);
  Serial.println();
  #endif
}

void setNeedSaveChoisedModeParams(void) {
  needSaveChoisedModeParams = 1;
  stripParamsSavingTimer.start();
}

void updatePixelsLength(void) {
  if (commonStripParams.pixelCount > pixels.numPixels()) {
    pixels.updateLength(pixels.numPixels() + 10);
  }
}

uint32_t getColor(uint8_t r, uint8_t g, uint8_t b, byte darknessLevel) {
  if (darknessLevel) {
    pixels.Color(r >> darknessLevel, g >> darknessLevel, b >> darknessLevel);
  } else {
    pixels.Color(r, g, b);
  }
}

/***** StripModes *****/
void StripModes::init(int eepromAddress, uint16_t initialPixelCount, uint8_t neoPixelPin) {
  this->eepromAddress = eepromAddress;
  uint16_t pixelCount;
  EEPROM.get(eepromAddress, pixelCount);
  if (pixelCount > 0 && pixelCount <= MAX_PIXEL_COUNT) {
    EEPROM.get(eepromAddress, commonStripParams);
  } else {
    commonStripParams.pixelCount = initialPixelCount;
    // save fabric initial parameters
    setNeedSaveCommonStripParams();
  }
  pixels = Adafruit_NeoPixel(commonStripParams.pixelCount, neoPixelPin, NEO_BRG);
};

byte StripModes::getChoisedModeIndex(void) {
  return commonStripParams.choisedModeIndex;
}

void StripModes::initMode(StripMode* mode) {
  choisedSettingsMode = 0;
  mode->init();
  choisedMode = mode;
}

void StripModes::saveCommonStripParams(void) {
  EEPROM.put(eepromAddress, commonStripParams);
}

void StripModes::forceSavingAllStripParams(void) {
  stripParamsSavingTimer.force();
}

void StripModes::setChoisedMode(StripMode* mode, byte choisedModeIndex) {
  if (mode != choisedMode) {
    initMode(mode);
    commonStripParams.choisedModeIndex = choisedModeIndex;
    saveStripParams();
    mode->start();
  } else {
    // todo switch mode
  }
}

void StripModes::choiseSettingsMode() {
  choisedMode->choiseSettingsMode();
}

void StripModes::changeSettingsUp() {
  choisedMode->changeSettingsUp();
}

void StripModes::changeSettingsDown() {
  choisedMode->changeSettingsDown();
}

void StripModes::show() {
  stripParamsSavingTimer.checkTimeout();
  choisedModeReshowTimer.checkTimeout();
}


/***** StripMode *****/
void StripMode::init(void) {
};

void StripMode::reshowSettingsMode() {
  fillStripSettingsMode();
  pixels.show();
}

void StripMode::fillStripSettingsMode(void) {
  for(uint16_t i = 0; i < commonStripParams.pixelCount; i++) {
    pixels.setPixelColor(i, pixels.Color(0xFF, 0xFF, 0xFF));
  }
};

void StripMode::choiseSettingsMode() {
  if (choisedSettingsMode) {
    choisedSettingsMode = 0;
  } else {
    choisedSettingsMode = 1;
  }
}

/***** StripModeStatic *****/
void StripModeStatic::start(void) {
  pixels.clear();
  fillStrip();
  pixels.show(); // This sends the updated pixel color to the hardware.
};

void StripModeStatic::reshow(void) {
};

void StripModeStatic::changeSettingsUp() {
  if (choisedSettingsMode) {
    commonStripParams.pixelCount++;
    updatePixelsLength();
    setNeedSaveCommonStripParams();
    reshowSettingsMode();
  } else {
    if (commonStripParams.darknessLevel) {
      commonStripParams.darknessLevel--;
      setNeedSaveCommonStripParams();
      reshow();
    }
  }
}

void StripModeStatic::changeSettingsDown() {
  if (choisedSettingsMode) {
    commonStripParams.pixelCount--;
    setNeedSaveCommonStripParams();
    reshowSettingsMode();
  } else {
    if (commonStripParams.darknessLevel <= 4) {
      commonStripParams.darknessLevel++;
      setNeedSaveCommonStripParams();
      reshow();
    }
  }
}

/***** StripModeDinamic *****/
void StripModeDinamic::start(void) {
  //todo set timer
  pixels.clear();
  fillStrip();
  pixels.show(); // This sends the updated pixel color to the hardware.
};

void StripModeDinamic::reshow(void) {
  //todo next step
  pixels.clear();
  fillStrip();
  pixels.show(); // This sends the updated pixel color to the hardware.
};

/***** StripModeWhite *****/
StripModeWhite::StripModeWhite(void) {
  this->mode = SHORT;
  this->pixelCountShort = 0;
  this->pixelCountMiddle = 0;
}

void StripModeWhite::init(void) {
  int _eepromAddress = eepromAddress();
  EEPROM.get(_eepromAddress, mode);
  EEPROM.get(_eepromAddress + sizeof(StripModeWhiteMode), pixelCountShort);
  EEPROM.get(_eepromAddress + sizeof(StripModeWhiteMode) +  + sizeof(byte), pixelCountMiddle);
  if (mode < SHORT || mode > LONG) {
    mode = LONG;
  }
  if (pixelCountShort == 0 || pixelCountShort > commonStripParams.pixelCount) {
    pixelCountShort = commonStripParams.pixelCount;
  }
  if (pixelCountMiddle == 0 || pixelCountMiddle > commonStripParams.pixelCount) {
    pixelCountMiddle = commonStripParams.pixelCount;
  }
};

void StripModeWhite::save(void) {
  int _eepromAddress = eepromAddress();
  EEPROM.put(_eepromAddress, mode);
  EEPROM.put(_eepromAddress + sizeof(StripModeWhiteMode), pixelCountShort);
  EEPROM.put(_eepromAddress + sizeof(StripModeWhiteMode) +  + sizeof(byte), pixelCountMiddle);
};

void StripModeWhite::fillStrip(void) {
  #ifdef DEBUG_SERIAL
  debugSerial();
  #endif
  int pixelCount;
  switch (mode) {
    case SHORT:
      pixelCount = pixelCountShort;
      break;
    case MIDDLE:
      pixelCount = pixelCountMiddle;
      break;
    default:
      pixelCount = commonStripParams.pixelCount;
      break;
  }
  for(uint16_t i = 0; i < pixelCount; i++) {
    pixels.setPixelColor(i, getColor(0xFF, 0xFF, 0xFF, commonStripParams.darknessLevel));
  }
};

void StripModeWhite::fillStripSettingsMode(void) {
  #ifdef DEBUG_SERIAL
  debugSerial();
  #endif
  int pixelCount = 0;
  uint32_t color = pixels.Color(0xFF, 0xFF, 0xFF);
  byte darknessLevel = 2;
  switch (mode) {
    case SHORT:
      for(; pixelCount < pixelCountShort; pixelCount++) {
        pixels.setPixelColor(pixelCount, color);
      }
      color = getColor(0xFF, 0xFF, 0xFF, darknessLevel);
      darknessLevel += 2;
    case MIDDLE:
      for(; pixelCount < pixelCountMiddle; pixelCount++) {
        pixels.setPixelColor(pixelCount, color);
      }
      color = getColor(0xFF, 0xFF, 0xFF, darknessLevel);
    default:
      for(; pixelCount < commonStripParams.pixelCount; pixelCount++) {
        pixels.setPixelColor(pixelCount, color);
      }
  }
};

void StripModeWhite::changeSettingsUp() {
  if (choisedSettingsMode) {
    switch (mode) {
      case SHORT:
        pixelCountShort++;
        setNeedSaveChoisedModeParams();
        if (pixelCountMiddle < pixelCountShort) {
          pixelCountMiddle = pixelCountShort;
        }
        if (commonStripParams.pixelCount < pixelCountShort) {
          commonStripParams.pixelCount = pixelCountShort;
          updatePixelsLength();
          setNeedSaveCommonStripParams();
        }
        break;
      case MIDDLE:
        pixelCountMiddle++;
        setNeedSaveChoisedModeParams();
        if (commonStripParams.pixelCount < pixelCountMiddle) {
          commonStripParams.pixelCount = pixelCountMiddle;
          updatePixelsLength();
          setNeedSaveCommonStripParams();
        }
        break;
      default:
        commonStripParams.pixelCount++;
        updatePixelsLength();
        setNeedSaveCommonStripParams();
        break;
    }
    reshowSettingsMode();
  } else {
    StripModeStatic::changeSettingsUp();
  }
}

void StripModeWhite::changeSettingsDown() {
  if (choisedSettingsMode) {
    switch (mode) {
      case SHORT:
        if (pixelCountShort > 1) {
          pixelCountShort--;
          setNeedSaveChoisedModeParams();
        }
        break;
      case MIDDLE:
        if (pixelCountMiddle > 1) {
          pixelCountMiddle--;
          setNeedSaveChoisedModeParams();
        }
        if (pixelCountMiddle < pixelCountShort) {
          pixelCountShort = pixelCountMiddle;
        }
        break;
      default:
        if (commonStripParams.pixelCount > 1) {
          commonStripParams.pixelCount--;
          setNeedSaveCommonStripParams();
        }
        if (commonStripParams.pixelCount < pixelCountMiddle) {
          pixelCountMiddle = commonStripParams.pixelCount;
          setNeedSaveChoisedModeParams();
        }
        if (commonStripParams.pixelCount < pixelCountShort) {
          pixelCountShort = commonStripParams.pixelCount;
          setNeedSaveChoisedModeParams();
        }
        break;
    }
    reshowSettingsMode();
  } else {
    StripModeStatic::changeSettingsDown();
  }
}

#ifdef DEBUG_SERIAL
void StripModeWhite::debugSerial(void) {
  Serial.print("StripModeWhite");
  Serial.print(": mode="); Serial.print(mode);
  Serial.print(", pixelCountShort="); Serial.print(pixelCountShort);
  Serial.print(", pixelCountMiddle="); Serial.print(pixelCountMiddle);
  Serial.println();
}
#endif

