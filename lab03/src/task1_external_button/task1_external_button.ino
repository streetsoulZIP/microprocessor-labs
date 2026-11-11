#include <Arduino.h>
#include <util/atomic.h>

const byte LED_PIN = 13;
const byte BUTTON_PIN = 2;  // INT0
// Wokwi's contact model can keep producing edges for several dozen
// milliseconds. Re-arm only after an unmistakably stable release.
const unsigned long RELEASE_STABLE_MS = 120UL;
const unsigned long MIN_PRESS_INTERVAL_US = 300000UL;

volatile bool fastMode = false;
volatile bool modeChanged = false;
volatile bool buttonArmed = true;
volatile bool resetReleaseTimer = false;
volatile unsigned long lastAcceptedPressUs = 0;

void onButtonChange() {
  if (digitalRead(BUTTON_PIN) == LOW && buttonArmed) {
    const unsigned long nowUs = micros();
    if (lastAcceptedPressUs != 0 &&
        (unsigned long)(nowUs - lastAcceptedPressUs) < MIN_PRESS_INTERVAL_US) {
      return;
    }
    lastAcceptedPressUs = nowUs;
    fastMode = !fastMode;
    modeChanged = true;
    buttonArmed = false;
    resetReleaseTimer = true;
  }
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(115200);

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), onButtonChange, CHANGE);
  Serial.println(F("T1:BOOT MODE=NORMAL PERIOD_MS=1000"));
}

void loop() {
  static bool ledState = false;
  static unsigned long lastEdgeMs = 0;
  static unsigned long releaseSinceMs = 0;

  bool localFast;
  bool reportChange;
  bool resetRelease;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    localFast = fastMode;
    reportChange = modeChanged;
    modeChanged = false;
    resetRelease = resetReleaseTimer;
    resetReleaseTimer = false;
  }

  if (resetRelease) releaseSinceMs = 0;

  if (reportChange) {
    Serial.print(F("T1:MODE="));
    Serial.print(localFast ? F("FAST") : F("NORMAL"));
    Serial.print(F(" PERIOD_MS="));
    Serial.println(localFast ? 200 : 1000);
  }

  if (digitalRead(BUTTON_PIN) == HIGH) {
    if (releaseSinceMs == 0) releaseSinceMs = millis();
    if ((unsigned long)(millis() - releaseSinceMs) >= RELEASE_STABLE_MS) {
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { buttonArmed = true; }
    }
  } else {
    releaseSinceMs = 0;
  }

  const unsigned long halfPeriodMs = localFast ? 100UL : 500UL;
  const unsigned long now = millis();
  if ((unsigned long)(now - lastEdgeMs) >= halfPeriodMs) {
    lastEdgeMs += halfPeriodMs;
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
    Serial.print(F("T1:EDGE MS="));
    Serial.print(now);
    Serial.print(F(" STATE="));
    Serial.print(ledState);
    Serial.print(F(" MODE="));
    Serial.println(localFast ? F("FAST") : F("NORMAL"));
  }
}
