// Lab01: ATmega328P/Arduino Uno. Buttons PD2..PD4, LEDs PB3..PB5.
#include <Arduino.h>

const uint8_t BUTTON_MASK = _BV(PD2) | _BV(PD3) | _BV(PD4);
const uint8_t LED_MASK = _BV(PB3) | _BV(PB4) | _BV(PB5);
const unsigned long DEBOUNCE_MS = 40;
const unsigned long SAMPLE_MS = 200; // exactly 5 Hz

uint8_t candidate = 0, stable = 0;
unsigned long candidateSince = 0, lastSample = 0;
bool useBitSetMethod = false;

uint8_t logicalPressedFromPind(uint8_t pind) {
  // INPUT_PULLUP makes a pressed button LOW. Result bit 0..2 maps to D2..D4.
  return (uint8_t)((~pind >> PD2) & 0x07);
}

int firstSetLoop(uint8_t v) {
  for (uint8_t i = 0; i < 8; ++i) if (v & _BV(i)) return i;
  return -1;
}

int firstSetCtz(uint8_t v) {
  return v ? __builtin_ctz((unsigned int)v) : -1; // ctz(0) is undefined
}

void writeLedsByMask(uint8_t v) {
  uint8_t out = (uint8_t)(v << PB3);
  PORTB = (uint8_t)((PORTB & ~LED_MASK) | (out & LED_MASK));
}

void writeLedsByBitSet(uint8_t v) {
  // Second method: bitSet/bitClear changes only LED bits, preserving PB0..PB2.
  for (uint8_t i = 0; i < 3; ++i) {
    if (v & _BV(i)) bitSet(PORTB, PB3 + i); else bitClear(PORTB, PB3 + i);
  }
}

void printBits(uint8_t v) { for (int8_t i = 7; i >= 0; --i) Serial.print((v & _BV(i)) ? '1' : '0'); }

void printSetPositions(uint8_t v) {
  Serial.print(F(" POS=["));
  bool first = true;
  for (uint8_t i = 0; i < 3; ++i) {
    if (v & _BV(i)) { if (!first) Serial.print(','); Serial.print(i); first = false; }
  }
  if (first) Serial.print(F("none"));
  Serial.print(']');
}

void setup() {
  DDRD &= ~BUTTON_MASK;       // PD2..PD4 inputs
  PORTD |= BUTTON_MASK;       // enable internal pull-ups
  DDRB |= LED_MASK;           // PB3..PB5 outputs: D11..D13
  PORTB &= ~LED_MASK;
  Serial.begin(9600);
  Serial.println(F("L01:BOOT PB3=D11 PB4=D12 PB5=D13"));
}

void loop() {
  const unsigned long now = millis();
  const uint8_t rawPressed = logicalPressedFromPind(PIND); // whole input port read
  if (rawPressed != candidate) { candidate = rawPressed; candidateSince = now; }
  if (candidate != stable && now - candidateSince >= DEBOUNCE_MS) stable = candidate;

  if (now - lastSample < SAMPLE_MS) return;
  lastSample = now;
  if (useBitSetMethod) writeLedsByBitSet(stable); else writeLedsByMask(stable);

  Serial.print(F("L01:MS=")); Serial.print(now);
  Serial.print(F(" RAW=")); printBits(PIND);
  Serial.print(F(" PRESS=")); printBits(stable);
  printSetPositions(stable);
  Serial.print(F(" LED=")); printBits((PORTB & LED_MASK) >> PB3);
  Serial.print(F(" FIRST_LOOP=")); Serial.print(firstSetLoop(stable));
  Serial.print(F(" FIRST_CTZ=")); Serial.print(firstSetCtz(stable));
  Serial.print(F(" METHOD=")); Serial.println(useBitSetMethod ? F("BITSET") : F("MASK"));
  useBitSetMethod = !useBitSetMethod;
}
