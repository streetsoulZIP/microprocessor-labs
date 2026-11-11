#include <Arduino.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

const byte LED_PIN = 13;
volatile uint16_t interruptCount = 0;

ISR(TIMER1_COMPA_vect) {
  PORTB ^= _BV(PB5);  // D13
  interruptCount++;
}

void setupTimer1Ctc100Hz() {
  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = 2499;                       // 16 MHz / (64 * 100 Hz) - 1
  TCCR1B |= _BV(WGM12);               // CTC, TOP = OCR1A
  TCCR1B |= _BV(CS11) | _BV(CS10);    // prescaler 64
  TIMSK1 |= _BV(OCIE1A);              // compare A interrupt
  sei();
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
  setupTimer1Ctc100Hz();
  Serial.println(F("T2:BOOT OCR1A=2499 PRESCALER=64 ISR_HZ=100 LED_CYCLE_HZ=50"));
}

void loop() {
  static uint16_t lastReported = 0;
  uint16_t snapshot;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { snapshot = interruptCount; }

  if ((uint16_t)(snapshot - lastReported) >= 100) {
    lastReported += 100;
    Serial.print(F("T2:CHECK TICKS="));
    Serial.print(lastReported);
    Serial.print(F(" ELAPSED_MS="));
    Serial.print(millis());
    Serial.println(F(" ISR_HZ=100 LED_CYCLE_HZ=50"));
  }
}
