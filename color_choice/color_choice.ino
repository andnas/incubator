#include <Adafruit_NeoPixel.h>
#include "printf.h"

#define PIN            6
#define NUMPIXELS      10
#define COLOR_LEVELS   5
#define COLOR_COUNT    16
#define DELAY          500

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

byte scale(byte val, byte level) {
  return val >> (COLOR_LEVELS - level);
}

byte fixColorR = colors[0][0];
byte fixColorG = colors[0][1];
byte fixColorB = colors[0][2];
byte varColorR = fixColorR;
byte varColorG = fixColorG;
byte varColorB = fixColorB;
int varColorNumPixels = NUMPIXELS;
int level = 5;
int delayVal = DELAY;



Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_BRG);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  // Open serial communications and wait for port to open:
  Serial.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  printf_begin();

  Serial.println("Goodnight moon!");
  pixels.begin(); // This initializes the NeoPixel library.

  showStrip();
  varColorR = 0;
  varColorG = 0;
  varColorB = 0;
  delayVal = 0;
  showStrip();
  delayVal = DELAY;
}

void loop() { // run over and over
  if (Serial.available()) {
    int ch = readChar();
    switch (ch) {
      case 'f': {
        int colorNum = readHexCharAsInt();
        fixColorR = colors[colorNum][0];
        fixColorG = colors[colorNum][1];
        fixColorB = colors[colorNum][2];
        varColorR = colors[colorNum][0];
        varColorG = colors[colorNum][1];
        varColorB = colors[colorNum][2];
        varColorNumPixels = NUMPIXELS - COLOR_LEVELS;
        Serial.print(" colorNum: ");
        Serial.println(colorNum); 
        showStrip();
        break;
      }
      case 'r':
        varColorR = readColorPart();
        Serial.print(" varColorR: ");
        Serial.println(varColorR);
        showStrip();
        break;
      case 'g':
        varColorG = readColorPart();
        Serial.print(" varColorG: ");
        Serial.println(varColorG);
        showStrip();
        break;
      case 'b':
        varColorB = readColorPart();
        Serial.print(" varColorB: ");
        Serial.println(varColorB);
        showStrip();
        break;
      case 'n':
        varColorNumPixels = readHexCharAsInt();
        Serial.print(" varColorNumPixels: ");
        Serial.println(varColorNumPixels);
        showStrip();
        break;
      case 'l':
        level = readHexCharAsInt();
        Serial.print(" level: ");
        Serial.println(level);
        showStrip();
        break;
      case 'd':
        delayVal = readInt();
        Serial.print(" delayVal: ");
        Serial.println(delayVal);
        showStrip();
        break;
    }
  } else {
    health();
  }
}

int readChar() {
  int ch = Serial.read();
  Serial.write(ch);
  return ch;
}

int readHexCharAsInt() {
  int hex = readChar();
  if (isHexadecimalDigit(hex)) {
    if (isDigit(hex)) {
      hex -= '0';
    } else {
      hex = 10 + (hex >= 'a' && hex <= 'f' ? hex - 'a' : hex - 'A');
    }
    return hex;
  }
  return 0;
}

int readColorPart() {
  int highColorPart = readHexCharAsInt();
  int lowColorPart = readHexCharAsInt();
  return (highColorPart << 4) + lowColorPart;
}

int readInt() {
  String inString = "";    // string to hold input  
  do {
    int inChar = Serial.read();
    if (isDigit(inChar)) {
      // convert the incoming byte to a char and add it to the string:
      inString += (char)inChar;
    }
    delay(5);
  } while (Serial.available() > 0);
  return inString.toInt();
}

void showStrip() {
  for(int i = 0;i<NUMPIXELS;i++){
    pixels.setPixelColor(i, pixels.Color(0,0,0)); // Moderately bright green color.
  }
  pixels.show(); // This sends the updated pixel color to the hardware.
  delay(DELAY); // Delay for a period of time (in milliseconds).
  byte r, g, b;
  for(int i = 0;i<NUMPIXELS;i++){
    if (i < NUMPIXELS - varColorNumPixels) {
      if (i < COLOR_LEVELS) {
        r = scale(fixColorR, COLOR_LEVELS - i);
        g = scale(fixColorG, COLOR_LEVELS - i);
        b = scale(fixColorB, COLOR_LEVELS - i);
      } else {
        r = 0;
        g = 0;
        b = 0;
      }
    } else {
      r = scale(varColorR, level);
      g = scale(varColorG, level);
      b = scale(varColorB, level);
    }
    printf(" 0x%02X%02X%02X",r,g,b); 
    pixels.setPixelColor(i, pixels.Color(r,g,b)); // Moderately bright green color.
    if (delayVal > 0) {
      pixels.show(); // This sends the updated pixel color to the hardware.
      delay(delayVal); // Delay for a period of time (in milliseconds).
    }
  }
  if (delayVal == 0) {
    pixels.show(); // This sends the updated pixel color to the hardware.
  }
  Serial.println();
}

void health() {
    int state = digitalRead(LED_BUILTIN);
    if (state == HIGH) {
      state = LOW;
    } else {
      state = HIGH;
    }
    digitalWrite(LED_BUILTIN, state);
    delay(1000);                       // wait for a second
}

