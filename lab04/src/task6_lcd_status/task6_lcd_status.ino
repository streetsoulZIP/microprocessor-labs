#include <LiquidCrystal_I2C.h>
#include <Wire.h>

constexpr uint8_t POT_PIN = A0;
LiquidCrystal_I2C lcd(0x27, 16, 2);

const __FlashStringHelper *statusFor(uint16_t millivolts) {
  if (millivolts < 1700) {
    return F("LOW");
  }
  if (millivolts > 3400) {
    return F("HIGH");
  }
  return F("NORMAL");
}

void printPaddedStatus(const __FlashStringHelper *status) {
  lcd.setCursor(0, 0);
  lcd.print(F("STATUS: "));
  lcd.print(status);
  lcd.print(F("      "));
}

void printVoltage(uint16_t adc, uint16_t millivolts) {
  lcd.setCursor(0, 1);
  lcd.print(F("U="));
  lcd.print(millivolts / 1000);
  lcd.print('.');
  const uint16_t fraction = millivolts % 1000;
  if (fraction < 100) lcd.print('0');
  if (fraction < 10) lcd.print('0');
  lcd.print(fraction);
  lcd.print(F("V A="));
  lcd.print(adc);
  lcd.print(F("    "));
}

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  Serial.println(F("L04:LCD_READY ADDRESS=0x27"));
}

void loop() {
  static unsigned long nextUpdateMs = 0;
  const unsigned long now = millis();
  if (static_cast<long>(now - nextUpdateMs) < 0) {
    return;
  }

  nextUpdateMs = now + 250;
  const uint16_t adc = analogRead(POT_PIN);
  const uint16_t millivolts = (static_cast<uint32_t>(adc) * 5000 + 511) / 1023;
  const __FlashStringHelper *status = statusFor(millivolts);
  printPaddedStatus(status);
  printVoltage(adc, millivolts);

  Serial.print(F("L04:LCD ADC="));
  Serial.print(adc);
  Serial.print(F(" MV="));
  Serial.print(millivolts);
  Serial.print(F(" STATUS="));
  Serial.println(status);
}
