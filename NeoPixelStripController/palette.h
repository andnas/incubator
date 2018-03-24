/*
  palette.h contains the color set for algorithmes.

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

#ifndef palette_h
#define palette_h

#define COLOR_COUNT    16

byte colors[COLOR_COUNT][3] = {
  {0x00,0x00,0xFF}, // синий
  {0xFF,0x00,0x00}, // красный
  {0x00,0xFF,0x00}, // зеленый
  {0xFF,0x00,0xFF}, // малиновый
  {0x00,0xFF,0xFF}, // голубой
  {0xFF,0xFF,0x00}, // желтый
  {0xFF,0xFF,0xFF}, // белый
  {0x30,0x04,0xC0}, // фиолетовый
  {0x00,0x30,0xC0}, // светло-синий
  {0x04,0xC0,0x30}, // светло-зеленый
  {0x30,0xC0,0x00}, // желто-зеленый
  {0xC0,0x00,0x30}, // ярко-малиновый
  {0xFF,0x40,0x00}, // оранжевый
  {0x40,0x40,0xFF}, // индиго
  {0xFF,0x40,0x40}, // розовый
  {0x40,0xFF,0x40}  // светло-светло-зеленый
};

#endif
