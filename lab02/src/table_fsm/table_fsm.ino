#include <Arduino.h>

const byte RED = 13;
const byte YELLOW = 12;
const byte GREEN = 11;
const byte PED = 2;
const byte EMERGENCY = 3;
const byte NIGHT = 4;
const unsigned long DEBOUNCE = 50;

enum State { GREEN_STATE, YELLOW_STATE, RED_STATE, STATE_COUNT };
enum Event {
  TIMER_EVENT,
  PED_EVENT,
  EMERGENCY_ON_EVENT,
  EMERGENCY_OFF_EVENT,
  NIGHT_TOGGLE_EVENT,
  EVENT_COUNT
};

State state = GREEN_STATE;
State savedState = GREEN_STATE;
bool pedPending = false;
bool nightMode = false;
bool emergencyMode = false;
bool savedValid = false;
unsigned long started = 0;
unsigned long duration = 10000;
unsigned long savedRemaining = 0;
unsigned long greenMs = 10000;
unsigned long yellowMs = 3000;
unsigned long redMs = 10000;
unsigned long pedExtra = 5000;

struct Button {
  byte pin;
  bool raw = HIGH;
  bool stable = HIGH;
  unsigned long changed = 0;

  Button(byte p) : pin(p) {}

  bool fell() {
    bool reading = digitalRead(pin);
    if (reading != raw) {
      raw = reading;
      changed = millis();
    }
    if (raw != stable && millis() - changed >= DEBOUNCE) {
      stable = raw;
      return stable == LOW;
    }
    return false;
  }
};

Button ped{PED};
Button night{NIGHT};

const char* name(State value) {
  return value == GREEN_STATE ? "GREEN" : value == YELLOW_STATE ? "YELLOW" : "RED";
}

unsigned long normalDuration(State value) {
  if (value == GREEN_STATE) return greenMs;
  if (value == YELLOW_STATE) return yellowMs;
  return redMs + (pedPending ? pedExtra : 0);
}

void lamps(bool red, bool yellow, bool green) {
  digitalWrite(RED, red);
  digitalWrite(YELLOW, yellow);
  digitalWrite(GREEN, green);
}

void logTransition(const char* from, const char* to, const char* why, unsigned long time) {
  Serial.print(F("L02:MS="));
  Serial.print(millis());
  Serial.print(F(" FROM="));
  Serial.print(from);
  Serial.print(F(" TO="));
  Serial.print(to);
  Serial.print(F(" WHY="));
  Serial.print(why);
  Serial.print(F(" DUR="));
  Serial.println(time);
}

void enterNormal(State next, const char* why, unsigned long time = 0) {
  State old = state;
  state = next;
  duration = time ? time : normalDuration(next);
  started = millis();
  lamps(next == RED_STATE, next == YELLOW_STATE, next == GREEN_STATE);
  logTransition(name(old), name(next), why, duration);
}

void freezeNormal() {
  savedState = state;
  savedRemaining = millis() - started >= duration ? 0 : duration - (millis() - started);
  savedValid = true;
}

void restoreNormal(const char* why) {
  if (!savedValid) {
    enterNormal(GREEN_STATE, why);
    return;
  }
  State resume = savedState;
  unsigned long remaining = savedRemaining;
  savedValid = false;
  state = resume;
  duration = remaining ? remaining : normalDuration(resume);
  started = millis();
  lamps(resume == RED_STATE, resume == YELLOW_STATE, resume == GREEN_STATE);
  logTransition("SPECIAL", name(resume), why, duration);
}

void onPed() {
  pedPending = true;
  Serial.println(F("L02:PED_QUEUED"));
}

void onGreenTimer() { enterNormal(YELLOW_STATE, "TIMER"); }

void onYellowTimer() {
  bool extra = pedPending;
  pedPending = false;
  enterNormal(RED_STATE, extra ? "TIMER_PED" : "TIMER", redMs + (extra ? pedExtra : 0));
}

void onRedTimer() { enterNormal(GREEN_STATE, "TIMER"); }

void enterEmergency();
void leaveEmergency();
void toggleNight();

typedef void (*Handler)();
Handler table[STATE_COUNT][EVENT_COUNT] = {
  {onGreenTimer, onPed, enterEmergency, leaveEmergency, toggleNight},
  {onYellowTimer, onPed, enterEmergency, leaveEmergency, toggleNight},
  {onRedTimer, onPed, enterEmergency, leaveEmergency, toggleNight},
};

void enterEmergency() {
  if (emergencyMode) return;
  freezeNormal();
  emergencyMode = true;
  lamps(0, 1, 0);
  logTransition(name(state), "EMERGENCY", "EMERGENCY_ON", 0);
}

void leaveEmergency() {
  emergencyMode = false;
  if (nightMode) {
    lamps(0, 1, 0);
    logTransition("EMERGENCY", "NIGHT", "EMERGENCY_OFF_NIGHT", 0);
  } else {
    restoreNormal("EMERGENCY_OFF_RESTORE");
  }
}

void toggleNight() {
  nightMode = !nightMode;
  if (emergencyMode) {
    logTransition("EMERGENCY", "EMERGENCY", nightMode ? "NIGHT_QUEUED_ON" : "NIGHT_QUEUED_OFF", 0);
    return;
  }
  if (nightMode) {
    freezeNormal();
    lamps(0, 1, 0);
    logTransition(name(state), "NIGHT", "NIGHT_ON", 0);
  } else {
    restoreNormal("NIGHT_OFF_RESTORE");
  }
}

void fastMode() {
  greenMs = 1000;
  yellowMs = 300;
  redMs = 1000;
  pedExtra = 500;
  enterNormal(GREEN_STATE, "TEST_FAST", greenMs);
}

void setup() {
  pinMode(RED, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(PED, INPUT_PULLUP);
  pinMode(EMERGENCY, INPUT_PULLUP);
  pinMode(NIGHT, INPUT_PULLUP);
  Serial.begin(9600);
  enterNormal(GREEN_STATE, "BOOT", greenMs);
}

void loop() {
  if (Serial.available() && Serial.read() == 'F') fastMode();

  if (state >= STATE_COUNT) {
    enterEmergency();
    return;
  }

  bool emergencyNow = digitalRead(EMERGENCY) == LOW;
  if (emergencyNow && !emergencyMode) table[state][EMERGENCY_ON_EVENT]();
  if (!emergencyNow && emergencyMode) table[state][EMERGENCY_OFF_EVENT]();

  if (ped.fell()) {
    if (emergencyMode) {
      Serial.println(F("L02:PED_IGNORED_EMERGENCY"));
    } else {
      table[state][PED_EVENT]();
    }
  }
  if (night.fell()) table[state][NIGHT_TOGGLE_EVENT]();

  if (emergencyMode || nightMode) {
    static unsigned long blink = 0;
    if (millis() - blink >= 500) {
      blink = millis();
      digitalWrite(YELLOW, !digitalRead(YELLOW));
    }
    return;
  }
  if (millis() - started >= duration) table[state][TIMER_EVENT]();
}
