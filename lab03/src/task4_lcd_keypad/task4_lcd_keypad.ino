#include <Arduino.h>
#include <Wire.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

const byte ROW_PINS[4] = {4, 5, 6, 7};
const byte COL_PINS[4] = {8, 9, 10, 11};
const byte KEY_INT_PIN = 2;  // INT0
const char KEYS[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

const byte LCD_ADDR = 0x27;
const byte LCD_RS = 0x01;
const byte LCD_ENABLE = 0x04;
const byte LCD_BACKLIGHT = 0x08;

volatile uint16_t displayTicks = 0;
volatile bool displayDue = false;
volatile bool scanRequested = false;

char lastKey = '-';
bool waitingKeyRelease = false;
unsigned long releaseStableSince = 0;

void lcdExpanderWrite(byte value) {
  Wire.beginTransmission(LCD_ADDR);
  Wire.write(value | LCD_BACKLIGHT);
  Wire.endTransmission();
}

void lcdPulse(byte value) {
  lcdExpanderWrite(value | LCD_ENABLE);
  delayMicroseconds(1);
  lcdExpanderWrite(value & ~LCD_ENABLE);
  delayMicroseconds(50);
}

void lcdWrite4(byte value) {
  lcdExpanderWrite(value);
  lcdPulse(value);
}

void lcdSend(byte value, bool dataMode) {
  const byte mode = dataMode ? LCD_RS : 0;
  lcdWrite4((value & 0xF0) | mode);
  lcdWrite4(((value << 4) & 0xF0) | mode);
}

void lcdCommand(byte command) { lcdSend(command, false); }
void lcdData(char value) { lcdSend((byte)value, true); }

void lcdInit() {
  delay(50);
  lcdWrite4(0x30);
  delay(5);
  lcdWrite4(0x30);
  delayMicroseconds(150);
  lcdWrite4(0x30);
  lcdWrite4(0x20);
  lcdCommand(0x28);  // 4-bit, 2 lines
  lcdCommand(0x0C);  // display on, cursor off
  lcdCommand(0x06);  // entry mode
  lcdCommand(0x01);  // clear
  delay(2);
}

void lcdSetCursor(byte col, byte row) {
  static const byte offsets[] = {0x00, 0x40};
  lcdCommand(0x80 | (offsets[row] + col));
}

void lcdPrintLine(byte row, const char *text) {
  lcdSetCursor(0, row);
  byte count = 0;
  while (*text && count < 16) {
    lcdData(*text++);
    count++;
  }
  while (count++ < 16) lcdData(' ');
}

ISR(TIMER1_COMPA_vect) {
  displayTicks++;
  displayDue = true;
}

void onKeyboardActivity() { scanRequested = true; }

void setupTimer1For500ms() {
  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = 7812;                      // 16 MHz / (1024 * 2 Hz) - 1 ~= 7812
  TIFR1 = _BV(OCF1A);                // discard a pending compare flag
  TCCR1B |= _BV(WGM12);
  TCCR1B |= _BV(CS12) | _BV(CS10);  // prescaler 1024
  TIMSK1 |= _BV(OCIE1A);
  sei();
}

void setAllRows(byte level) {
  for (byte row = 0; row < 4; row++) digitalWrite(ROW_PINS[row], level);
}

char scanKeypad() {
  setAllRows(HIGH);
  for (byte row = 0; row < 4; row++) {
    digitalWrite(ROW_PINS[row], LOW);
    delayMicroseconds(10);
    for (byte col = 0; col < 4; col++) {
      if (digitalRead(COL_PINS[col]) == LOW) {
        digitalWrite(ROW_PINS[row], HIGH);
        setAllRows(LOW);  // idle state re-arms the diode OR after release
        return KEYS[row][col];
      }
    }
    digitalWrite(ROW_PINS[row], HIGH);
  }
  setAllRows(LOW);
  return 0;
}

void refreshDisplay(uint16_t ticks) {
  char first[17];
  char second[17];
  snprintf(first, sizeof(first), "Count: %u", ticks);
  snprintf(second, sizeof(second), "Last key: %c", lastKey);
  lcdPrintLine(0, first);
  lcdPrintLine(1, second);
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  lcdInit();

  for (byte row = 0; row < 4; row++) pinMode(ROW_PINS[row], OUTPUT);
  for (byte col = 0; col < 4; col++) pinMode(COL_PINS[col], INPUT_PULLUP);
  setAllRows(LOW);

  pinMode(KEY_INT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(KEY_INT_PIN), onKeyboardActivity, FALLING);
  setupTimer1For500ms();

  refreshDisplay(0);
  Serial.println(F("T4:BOOT OCR1A=7812 TIMER_PERIOD_MS=500 EXT_INT=INT0"));
}

void loop() {
  bool doScan = false;
  bool doDisplay = false;
  uint16_t ticks = 0;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    doScan = scanRequested;
    scanRequested = false;
    doDisplay = displayDue;
    displayDue = false;
    ticks = displayTicks;
  }

  if (doScan && !waitingKeyRelease) {
    detachInterrupt(digitalPinToInterrupt(KEY_INT_PIN));
    const char key = scanKeypad();
    if (key) {
      lastKey = key;
      Serial.print(F("T4:KEY="));
      Serial.println(key);
      refreshDisplay(ticks);
    }
    waitingKeyRelease = true;
    releaseStableSince = 0;
  }

  if (waitingKeyRelease) {
    if (digitalRead(KEY_INT_PIN) == HIGH) {
      if (releaseStableSince == 0) releaseStableSince = millis();
      if ((unsigned long)(millis() - releaseStableSince) >= 30) {
        waitingKeyRelease = false;
        attachInterrupt(digitalPinToInterrupt(KEY_INT_PIN), onKeyboardActivity, FALLING);
        Serial.println(F("T4:KEYBOARD_REARMED"));
      }
    } else {
      releaseStableSince = 0;
    }
  }

  if (doDisplay) {
    refreshDisplay(ticks);
    Serial.print(F("T4:LCD COUNT="));
    Serial.print(ticks);
    Serial.print(F(" MS="));
    Serial.println(millis());
  }
}
