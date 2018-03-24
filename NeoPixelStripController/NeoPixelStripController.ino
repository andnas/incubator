#define DEBUG_SERIAL            1

#include <IRremote.h>

#define INITIAL_PIXEL_COUNT     10
#define NEOPIXEL_PIN            6
#define IR_RECEIVER_PIN         2

#define EEPROM_STRIP_COMMON     0
#define EEPROM_MODE_WHITE       EEPROM_STRIP_COMMON + sizeof(StripPersistentParameters)

#include "StripModes.h"

int StripModeWhite::eepromAddress(void) { return EEPROM_MODE_WHITE; };

class SingleMode {
  public:
    unsigned long irValue;
    StripMode* mode;
    SingleMode(unsigned long irValue, StripMode* mode) {
      this->irValue = irValue;
      this->mode = mode;
    }
};

struct MultipleMode {
  unsigned long irValue;
  byte modeCount;
  StripMode* modes[];
};

IRrecv irReceiver(IR_RECEIVER_PIN); // пин, к котрому подключен ИК-приемник
decode_results irReceiverResults;
unsigned long lastIrReceivedTime;

StripModeWhite stripModeWhite = StripModeWhite();

SingleMode singleModes[] = {
  SingleMode(0xFF609F, &stripModeWhite)   // W/WW
};

extern StripModes stripModes;

void handleIrCommand() {
    unsigned long newIrReceivedTime = millis();

    switch (irReceiverResults.value) {
      case 0x926DC837:
        digitalWrite(LED_BUILTIN, HIGH);
        break;
      case 0x926D48B7:
        digitalWrite(LED_BUILTIN, LOW);
        break;
      case 0xFFE01F: // ON/OFF (0/1)
        if (digitalRead(LED_BUILTIN) == HIGH ) {
          stripModes.forceSavingAllStripParams();
        }
        digitalWrite(LED_BUILTIN, digitalRead(LED_BUILTIN) == LOW ? HIGH : LOW);
        break;
      case 0xFF906F: // IC Set
        stripModes.choiseSettingsMode();
        break;
      case 0xFFA05F: // Arrow Up
        stripModes.changeSettingsUp();
        break;
      case 0xFF20DF: // Arrow Down
        stripModes.changeSettingsDown();
        break;
      default:
        StripMode* stripMode = NULL;
        byte i = 0;
        for (; i < sizeof(singleModes)/sizeof(SingleMode); i++) {
          SingleMode mode = singleModes[i];
          if (mode.irValue == irReceiverResults.value) {
            stripMode = mode.mode;
            break;
          }
        }
        if (stripMode) {
          stripModes.setChoisedMode(stripMode, i);
digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
delay(500);                       // wait for a second
digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
          break;
        }
        
        #ifdef DEBUG_SERIAL
        // выводим в консольку, что получили, число в 16-ричном виде
        Serial.print(irReceiverResults.value, HEX);
        Serial.print(" ");
        Serial.println(lastIrReceivedTime - newIrReceivedTime);
        #endif
    }

    irReceiver.resume(); // готовы принимать следующий сигнал
    lastIrReceivedTime = newIrReceivedTime;
}
// handleIrCommand

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  #ifdef DEBUG_SERIAL
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Started.");
  #endif
  
  irReceiver.enableIRIn(); // запуск приемника
  stripModes.init(EEPROM_STRIP_COMMON, INITIAL_PIXEL_COUNT, NEOPIXEL_PIN);
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  lastIrReceivedTime = millis();
}

void loop() {
  if (irReceiver.decode(&irReceiverResults)) {
    handleIrCommand();
  }
  stripModes.show();
}
