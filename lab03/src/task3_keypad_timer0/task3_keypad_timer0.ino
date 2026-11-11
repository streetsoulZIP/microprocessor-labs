#include <Arduino.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

const byte ROW_PINS[4] = {4, 5, 6, 7};
const byte COL_PINS[4] = {8, 9, 10, 11};
const char KEYS[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

volatile byte currentRow = 0;
volatile byte lowSamples[16] = {0};
volatile bool keyLatched[16] = {false};
volatile bool keyReady = false;
volatile char pendingKey = 0;
volatile uint16_t scanInterrupts = 0;

ISR(TIMER0_COMPA_vect) {
  digitalWrite(ROW_PINS[currentRow], HIGH);
  currentRow = (currentRow + 1) & 0x03;
  digitalWrite(ROW_PINS[currentRow], LOW);

  for (byte col = 0; col < 4; col++) {
    const byte index = currentRow * 4 + col;
    if (digitalRead(COL_PINS[col]) == LOW) {
      if (lowSamples[index] < 3) lowSamples[index]++;
      if (lowSamples[index] >= 3 && !keyLatched[index] && !keyReady) {
        keyLatched[index] = true;
        pendingKey = KEYS[currentRow][col];
        keyReady = true;
      }
    } else {
      lowSamples[index] = 0;
      keyLatched[index] = false;
    }
  }
  scanInterrupts++;
}

void setupTimer0CompareA() {
  // Arduino already runs Timer0 at 16 MHz / 64 in fast-PWM mode.
  // Compare A at 249 occurs once per 256 counts: every 1.024 ms.
  cli();
  OCR0A = 249;
  TIFR0 = _BV(OCF0A);
  TIMSK0 |= _BV(OCIE0A);
  sei();
}

void setup() {
  Serial.begin(115200);
  for (byte row = 0; row < 4; row++) {
    pinMode(ROW_PINS[row], OUTPUT);
    digitalWrite(ROW_PINS[row], HIGH);
  }
  for (byte col = 0; col < 4; col++) pinMode(COL_PINS[col], INPUT_PULLUP);
  digitalWrite(ROW_PINS[currentRow], LOW);
  setupTimer0CompareA();
  Serial.println(F("T3:BOOT TIMER0_COMPA_PERIOD_US=1024 ROW_STEP_ISR=1"));
}

void loop() {
  char key = 0;
  uint16_t scans = 0;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    if (keyReady) {
      key = pendingKey;
      keyReady = false;
    }
    scans = scanInterrupts;
  }

  if (key) {
    Serial.print(F("T3:KEY="));
    Serial.print(key);
    Serial.print(F(" SCAN_ISR_COUNT="));
    Serial.println(scans);
  }
}
