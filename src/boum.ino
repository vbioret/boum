#include <TM1637Display.h>
#include "pitches.h"
#include "themes.h"

// Module connection pins (Digital Pins)
#define CLK 2
#define DIO 3

// The amount of time (in milliseconds) between tests
#define TEST_DELAY 70

TM1637Display display(CLK, DIO);
const int resetValue = 100;
int currentDelay = resetValue;
int currentD1State = false;
int currentD3State = true;
int count = currentDelay;
bool started = false;
uint8_t data[] = {0, 0, 0, 0};
const int pinSw1 = 4; // Push Button 1
const int pinSw2 = 5; // Push Button 2
const int pinSw3 = 6; // Push Button 3
const int pinSw4 = 8; // Push Button 4
const int pinBz1 = 7; // Buzzer 1
const int pinD1 = 9;  // LED 1 (Green)
const int pinD3 = 10; // LED 2 (Red)
int resetAndRestartBtState = 0;
int increase1BtState = 0;
int increase2BtState = 0;
int abortBtState = 0;

// notes in the failMelody:
int failMelody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

void setup() {
    pinMode(pinSw1, INPUT);
    pinMode(pinSw2, INPUT);
    pinMode(pinSw3, INPUT);
    pinMode(pinSw4, INPUT);
    pinMode(pinBz1, OUTPUT);
    pinMode(pinD1, OUTPUT);
    pinMode(pinD3, OUTPUT);
}

void loop() {
  display.setBrightness(0x0f);
  if (started) {
    currentD1State = !currentD1State;
    if (currentD1State) {
      digitalWrite(pinD3, HIGH);
    } else {
      digitalWrite(pinD3, LOW);
    }
  }
 
  increase1BtState = digitalRead(pinSw1);
  increase2BtState = digitalRead(pinSw2);
  resetAndRestartBtState = digitalRead(pinSw3);
  abortBtState = digitalRead(pinSw4);

  if (resetAndRestartBtState == HIGH) {
    count = currentDelay;
    digitalWrite(pinD1, LOW);
    started = true;
    delay(500);
  }
  if (abortBtState == HIGH && started) {
    started = false;
    noTone(pinBz1);
    digitalWrite(pinD1, HIGH);
    digitalWrite(pinD3, LOW);
    tone(pinBz1,1500);
    delay(1000);
    noTone(pinBz1);
  }
  if (increase1BtState == HIGH) {
    currentDelay +=1000;
    count = currentDelay;
    started = false;
    noTone(pinBz1);
    digitalWrite(pinD1, LOW);
    delay(500);
  }
  if (increase2BtState == HIGH) {
    currentDelay +=100;
    count = currentDelay;
    started = false;
    noTone(pinBz1);
    digitalWrite(pinD1, LOW);
    delay(500);
  }

  if (currentDelay > 10000) {
    currentDelay -=10000;
  }
  if (currentDelay == 10000) {
    currentDelay = resetValue;
  }
  if (currentDelay <= 0) {
    currentDelay = 0;
  }
  data[3] = display.encodeDigit(count/1 % 10);
  data[2] = display.encodeDigit(count/10 % 10);
  data[1] = display.encodeDigit(count/100 % 10);
  data[0] = display.encodeDigit(count/1000 % 10);
  display.setSegments(data);
  if (count <= 0 && started) {
    digitalWrite(pinD1, LOW);
    digitalWrite(pinD3, HIGH);
    for (int i = 0;i < 5;i++) {
      for (int thisNote = 0; thisNote < 8; thisNote++) {
        int noteDuration = 1000 / MarioUW_duration[thisNote];
        tone(pinBz1, MarioUW_note[thisNote], noteDuration);

        int pauseBetweenNotes = noteDuration * 1.30;
        delay(pauseBetweenNotes);
        noTone(pinBz1);
      }
      delay(300);
    }
  started = false;
  currentD1State = false;
  count = currentDelay;
  }
  if(started){
    count--;
  }
  
  delay(TEST_DELAY);
}
