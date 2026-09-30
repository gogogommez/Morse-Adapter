// Morse Adapter
// Raspberry Pi Pico / RP2040
//
// Developed by Roland Schaal
//
// Arduino Core:
// "Raspberry Pi Pico/RP2040" by Earle F. Philhower
//
// Important:
// Tools -> USB Stack -> Adafruit TinyUSB
//
// Wiring:
// Left paddle  -> GP10 -> GND
// Right paddle -> GP2  -> GND
//
// Modes:
// 1 = Morse Mania
//     Left  -> a
//     Right -> s
//
// 2 = VBand / Morse Invaders
//     Left  -> [
//     Right -> ]
//
// Squeezing:
// Both paddles can be pressed simultaneously.
//
// Mode switch:
// Hold both paddles simultaneously for 15 seconds.
//
// The selected mode is stored in EEPROM.

#include <Keyboard.h>
#include <EEPROM.h>

constexpr uint8_t LEFT_PIN  = 10;
constexpr uint8_t RIGHT_PIN = 2;

constexpr uint32_t DEBOUNCE_MS = 8;
constexpr uint32_t MODE_SWITCH_MS = 15000;

constexpr uint8_t EEPROM_SIZE = 16;
constexpr uint8_t EEPROM_ADDR = 0;

enum PaddleMode {
  MORSE_MANIA = 0,
  VBAND = 1
};

PaddleMode currentMode = MORSE_MANIA;

struct PaddleInput {
  uint8_t pin;
  bool rawState;
  bool stableState;
  uint32_t lastChange;
};

PaddleInput leftPaddle;
PaddleInput rightPaddle;

bool bothPressedActive = false;
bool modeAlreadySwitched = false;
uint32_t bothPressedSince = 0;

#ifndef LED_BUILTIN
#define LED_BUILTIN 25
#endif

char getLeftKey() {
  if (currentMode == MORSE_MANIA) return 'a';
  return '[';
}

char getRightKey() {
  if (currentMode == MORSE_MANIA) return 's';
  return ']';
}

void blinkLED(uint8_t count) {
  for (uint8_t i = 0; i < count; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(150);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
  }
}

void showCurrentMode() {
  if (currentMode == MORSE_MANIA) blinkLED(1);
  else blinkLED(2);
}

void saveMode() {
  EEPROM.write(EEPROM_ADDR, static_cast<uint8_t>(currentMode));
  EEPROM.commit();
}

void loadMode() {
  uint8_t saved = EEPROM.read(EEPROM_ADDR);
  if (saved == MORSE_MANIA || saved == VBAND) {
    currentMode = static_cast<PaddleMode>(saved);
  } else {
    currentMode = MORSE_MANIA;
    saveMode();
  }
}

void initPaddle(PaddleInput &paddle, uint8_t pin) {
  paddle.pin = pin;
  pinMode(pin, INPUT_PULLUP);
  paddle.rawState = digitalRead(pin);
  paddle.stableState = paddle.rawState;
  paddle.lastChange = millis();
}

bool updatePaddle(PaddleInput &paddle) {
  bool raw = digitalRead(paddle.pin);

  if (raw != paddle.rawState) {
    paddle.rawState = raw;
    paddle.lastChange = millis();
  }

  if (millis() - paddle.lastChange >= DEBOUNCE_MS) {
    if (paddle.stableState != paddle.rawState) {
      paddle.stableState = paddle.rawState;
      return true;
    }
  }
  return false;
}

void releaseAllPaddleKeys() {
  Keyboard.release('a');
  Keyboard.release('s');
  Keyboard.release('[');
  Keyboard.release(']');
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  EEPROM.begin(EEPROM_SIZE);
  loadMode();

  initPaddle(leftPaddle, LEFT_PIN);
  initPaddle(rightPaddle, RIGHT_PIN);

  delay(700);
  Keyboard.begin();
  delay(300);
  Keyboard.releaseAll();

  showCurrentMode();
}

void loop() {
  bool leftChanged = updatePaddle(leftPaddle);
  bool rightChanged = updatePaddle(rightPaddle);

  bool leftPressed = leftPaddle.stableState == LOW;
  bool rightPressed = rightPaddle.stableState == LOW;

  if (modeAlreadySwitched) {
    releaseAllPaddleKeys();

    if (!leftPressed && !rightPressed) {
      modeAlreadySwitched = false;
      bothPressedActive = false;
    }
    return;
  }

  // Normal paddle output. Both keys may be held simultaneously for squeezing.
  if (leftChanged) {
    if (leftPressed) Keyboard.press(getLeftKey());
    else Keyboard.release(getLeftKey());
  }

  if (rightChanged) {
    if (rightPressed) Keyboard.press(getRightKey());
    else Keyboard.release(getRightKey());
  }

  if (leftPressed && rightPressed) {
    if (!bothPressedActive) {
      bothPressedActive = true;
      bothPressedSince = millis();
    }

    if (millis() - bothPressedSince >= MODE_SWITCH_MS) {
      releaseAllPaddleKeys();

      if (currentMode == MORSE_MANIA) currentMode = VBAND;
      else currentMode = MORSE_MANIA;

      saveMode();
      showCurrentMode();

      modeAlreadySwitched = true;
      return;
    }
  } else {
    bothPressedActive = false;
  }
}
