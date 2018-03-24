/*

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

#include "SimpleTimer.h"

SimpleTimer::SimpleTimer(void (*callback)(void), unsigned long timeout, boolean periodic) {
  this->callback = callback;
  this->timeout = timeout;
  this->periodic = periodic;
  this->nextTime = 0;
}

void SimpleTimer::start(void) {
  unsigned long currentTime = millis();
  nextTime = currentTime + timeout;
}

void SimpleTimer::checkTimeout(void) {
  if (nextTime > 0) {
    unsigned long currentTime = millis();
    if (currentTime >= nextTime) {
      if (periodic) {
        nextTime += timeout;
      } else {
        nextTime = 0;
      }
      callback();
    }
  }
}

void SimpleTimer::force(void) {
  if (nextTime > 0) {
    nextTime = 0;
    callback();
  }
}

void SimpleTimer::setTimeout(unsigned long timeout) {
  this->timeout = timeout;
  start();
}

