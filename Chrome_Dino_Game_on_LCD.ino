#include <LiquidCrystal.h>
#include <EEPROM.h>

#include "bitmaps.h"

LiquidCrystal lcd(11, 9, 5, 4, 3, 2);

int adc_key_in  = 0;

#define btnRIGHT  14
#define btnUP     15
#define btnDOWN   16
#define btnLEFT   17
#define btnSELECT 18
#define btnNONE   19

byte runnerArea[16] {32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32};
byte jump = 32;

int score = 0;
bool freeze_score = 0;

byte correct_code = 123;

unsigned long previousMillis = 0;
unsigned long previousMillisLED = 0;
unsigned long jumpTime = 0;
const int jumpLength = 500;

#define checkSafe runnerArea[1] == 32 || runnerArea[1] == 0

const byte chance_of_ob = 15;
int speedOfScroller = 300;

void setup() {
  lcd.begin(16, 2);
  Serial.begin(9600);

  lcd.createChar(0, dino);
  lcd.createChar(1, cacti);
  lcd.createChar(2, bird);
  lcd.createChar(3, block);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(13, OUTPUT);
  randomSeed(A1);
  lcd.clear();
  showSplashScreen(1000, true);
}

void loop() {
  unsigned long currentMillis = millis();
  unsigned long currentMillisOb = millis();
  if (currentMillisOb - previousMillis >= speedOfScroller) {
    previousMillis = currentMillisOb;
    if (random(chance_of_ob) == 0) {
      runnerArea[15] = 1;
    } else if (random(chance_of_ob) == 1) {
      runnerArea[15] = 2;
    } else {
      runnerArea[15] = 32;
    }
    for (int i = 0; i <= 15; i++) {
      runnerArea[i] = runnerArea[i + 1];
    }
    if (freeze_score == 0) {
      score++;
    }
  }
  drawBarrier();

  if (read_LCD_buttons() == btnSELECT) {
    // runnerArea[1] = 32;
    if (runnerArea[1] != 32 && (runnerArea[1] != 1 || runnerArea[1] != 2)) {
      runnerArea[1] = 32;
    }
    jump = 0;
    freeze_score = 1;
    jumpTime = millis();
  }
  if (millis() - jumpTime >= jumpLength) {
    if (checkSafe) {
      runnerArea[1] = 0;
      jump = 32;
      freeze_score = 0;
    } else {
      showCrashScreen();
    }
  }
  updateLcd();
  printScore();

  if (millis() - previousMillisLED >= 500) {
    previousMillisLED = currentMillis;
    digitalWrite(13, !digitalRead(13));
  }
}
