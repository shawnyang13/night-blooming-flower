#include <Preferences.h>

/*
  ESP32 microphone-reactive breathing lamp controller.

  The lamp brightness follows the microphone's sound-pressure envelope:
  louder sound raises the UV lamp brightness, quiet sound lets it fade back
  to the breathing floor. The MAX9814 is not factory SPL-calibrated here, so
  this maps relative sound pressure from the local room to lamp brightness.

  Wiring:
  - Microphone: MAX9814 electret mic module (has onboard AGC, so it adapts
    to quiet vs loud songs without manual gain tuning).
    - VCC  -> ESP32 3V3
    - GND  -> ESP32 GND
    - OUT  -> ESP32 GPIO34 (ADC1_CH6, input-only pin)
    - GAIN -> leave floating (60dB); tie to VCC for less gain (40dB) if the
      output clips on loud music.
    - A/R  -> leave floating (default attack/release timing).
    IMPORTANT: use an ADC1 pin (GPIO32-39) for the mic, not ADC2 -- ADC2 is
    unusable whenever Wi-Fi is active.
  - Lamp: same as flower_lamp_timer -- GPIO23 through a 100-220 ohm resistor
    into a constant-current LED driver such as the CN5711 module.
  - Mode button: momentary push button between GPIO27 and GND.
*/

const int LAMP_PIN = 23;
const int MIC_PIN = 34;
const int MODE_BUTTON_PIN = 27;

const int PWM_FREQUENCY_HZ = 1000;
const int PWM_RESOLUTION_BITS = 8;
const int PWM_MAX_DUTY = (1 << PWM_RESOLUTION_BITS) - 1;
const int MAX_BRIGHTNESS_PERCENT = 80;
const byte BREATH_FLOOR_PERCENT = 1;
const byte STEADY_BRIGHTNESS_PERCENT = 18;

const unsigned long SAMPLE_WINDOW_MS = 25;
const unsigned long CALIBRATION_MS = 2000;
const unsigned int SAMPLE_DELAY_US = 120;

const float DC_CENTER_ALPHA = 0.004f;
const float PRESSURE_ATTACK_ALPHA = 0.65f;
const float PRESSURE_RELEASE_ALPHA = 0.14f;
const float BRIGHTNESS_ATTACK_ALPHA = 0.32f;
const float BRIGHTNESS_RELEASE_ALPHA = 0.09f;
const float CEILING_DECAY_ALPHA = 0.0012f;
const float NOISE_GATE_MULTIPLIER = 0.6f;
const float NOISE_GATE_OFFSET = 3.0f;
const float CEILING_HEADROOM_MULTIPLIER = 5.5f;
const float MIN_PRESSURE_RANGE = 24.0f;

const unsigned long BUTTON_DEBOUNCE_MS = 35;
const unsigned long BREATH_PERIOD_MS = 3200;

enum LampMode {
  MODE_SOUND_REACTIVE = 0,
  MODE_SOFT_BREATH = 1,
  MODE_STEADY_LOW = 2,
  MODE_OFF = 3,
};

const byte MODE_COUNT = 4;

Preferences preferences;
LampMode activeMode = MODE_SOUND_REACTIVE;
float micDcCenter = 2048.0f;
float noiseFloor = 0.0f;
float pressureEnvelope = 0.0f;
float pressureCeiling = 0.0f;
float smoothedBrightnessPercent = BREATH_FLOOR_PERCENT;
int lastButtonReading = HIGH;
int stableButtonState = HIGH;
unsigned long lastButtonChangeAt = 0;

float clampFloat(float value, float low, float high) {
  return max(low, min(high, value));
}

void writeBrightnessPercent(float percent) {
  float limitedPercent = clampFloat(percent, 0.0f, (float)MAX_BRIGHTNESS_PERCENT);
  int duty = (int)(PWM_MAX_DUTY * limitedPercent / 100.0f);
  ledcWrite(LAMP_PIN, duty);
}

float readSoundPressureRms(unsigned long windowMs) {
  float sumSquares = 0.0f;
  int sampleCount = 0;
  unsigned long windowStart = millis();

  while (millis() - windowStart < windowMs) {
    int reading = analogRead(MIC_PIN);
    micDcCenter += ((float)reading - micDcCenter) * DC_CENTER_ALPHA;

    float acPressure = (float)reading - micDcCenter;
    sumSquares += acPressure * acPressure;
    sampleCount++;

    delayMicroseconds(SAMPLE_DELAY_US);
  }

  return sampleCount > 0 ? sqrtf(sumSquares / sampleCount) : 0.0f;
}

float calibrateNoiseFloor() {
  float total = 0.0f;
  int samples = 0;
  unsigned long calibrationStart = millis();

  while (millis() - calibrationStart < CALIBRATION_MS) {
    total += readSoundPressureRms(SAMPLE_WINDOW_MS);
    samples++;
  }

  return samples > 0 ? total / samples : 0.0f;
}

float smoothstep(float x) {
  float clamped = clampFloat(x, 0.0f, 1.0f);
  return clamped * clamped * (3.0f - 2.0f * clamped);
}

void saveMode() {
  preferences.putUChar("mode", (byte)activeMode);
}

void loadMode() {
  byte savedMode = preferences.getUChar("mode", MODE_SOUND_REACTIVE);
  activeMode = (LampMode)(savedMode % MODE_COUNT);
}

void advanceMode() {
  activeMode = (LampMode)(((byte)activeMode + 1) % MODE_COUNT);
  saveMode();

  if (activeMode == MODE_SOUND_REACTIVE) {
    pressureEnvelope = noiseFloor;
    pressureCeiling = max(noiseFloor * CEILING_HEADROOM_MULTIPLIER, noiseFloor + MIN_PRESSURE_RANGE);
  }
}

void handleModeButton() {
  int reading = digitalRead(MODE_BUTTON_PIN);
  unsigned long now = millis();

  if (reading != lastButtonReading) {
    lastButtonChangeAt = now;
    lastButtonReading = reading;
  }

  if (now - lastButtonChangeAt < BUTTON_DEBOUNCE_MS || reading == stableButtonState) {
    return;
  }

  stableButtonState = reading;
  if (stableButtonState == LOW) {
    advanceMode();
  }
}

float breathingBrightnessPercent(unsigned long now) {
  unsigned long phase = now % BREATH_PERIOD_MS;
  unsigned long halfPeriod = BREATH_PERIOD_MS / 2;
  unsigned long risingOrFalling = phase < halfPeriod ? phase : BREATH_PERIOD_MS - phase;
  float normalized = (float)risingOrFalling / (float)halfPeriod;
  float eased = smoothstep(normalized);

  return BREATH_FLOOR_PERCENT + eased * (MAX_BRIGHTNESS_PERCENT - BREATH_FLOOR_PERCENT);
}

void runSoundReactiveMode() {
  float soundPressure = readSoundPressureRms(SAMPLE_WINDOW_MS);

  float pressureAlpha = soundPressure > pressureEnvelope ? PRESSURE_ATTACK_ALPHA : PRESSURE_RELEASE_ALPHA;
  pressureEnvelope += (soundPressure - pressureEnvelope) * pressureAlpha;

  if (pressureEnvelope > pressureCeiling) {
    pressureCeiling = pressureEnvelope;
  } else {
    float ceilingFloor = max(noiseFloor * CEILING_HEADROOM_MULTIPLIER, noiseFloor + MIN_PRESSURE_RANGE);
    pressureCeiling += (ceilingFloor - pressureCeiling) * CEILING_DECAY_ALPHA;
  }

  float noiseGate = max(noiseFloor * NOISE_GATE_MULTIPLIER, noiseFloor + NOISE_GATE_OFFSET);
  float normalized = 0.0f;
  if (pressureEnvelope > noiseGate) {
    normalized = (pressureEnvelope - noiseGate) / max(MIN_PRESSURE_RANGE, pressureCeiling - noiseGate);
  }

  float eased = smoothstep(normalized);
  float targetPercent = BREATH_FLOOR_PERCENT + eased * (MAX_BRIGHTNESS_PERCENT - BREATH_FLOOR_PERCENT);
  float brightnessAlpha = targetPercent > smoothedBrightnessPercent ? BRIGHTNESS_ATTACK_ALPHA : BRIGHTNESS_RELEASE_ALPHA;

  smoothedBrightnessPercent += (targetPercent - smoothedBrightnessPercent) * brightnessAlpha;
  writeBrightnessPercent(smoothedBrightnessPercent);
}

void setup() {
  pinMode(MODE_BUTTON_PIN, INPUT_PULLUP);
  analogSetPinAttenuation(MIC_PIN, ADC_11db);
  ledcAttach(LAMP_PIN, PWM_FREQUENCY_HZ, PWM_RESOLUTION_BITS);
  writeBrightnessPercent(0);

  preferences.begin("flower", false);
  loadMode();

  micDcCenter = analogRead(MIC_PIN);
  noiseFloor = calibrateNoiseFloor();
  pressureEnvelope = noiseFloor;
  pressureCeiling = max(noiseFloor * CEILING_HEADROOM_MULTIPLIER, noiseFloor + MIN_PRESSURE_RANGE);
}

void loop() {
  handleModeButton();

  switch (activeMode) {
    case MODE_SOUND_REACTIVE:
      runSoundReactiveMode();
      break;
    case MODE_SOFT_BREATH:
      writeBrightnessPercent(breathingBrightnessPercent(millis()));
      break;
    case MODE_STEADY_LOW:
      writeBrightnessPercent(STEADY_BRIGHTNESS_PERCENT);
      break;
    case MODE_OFF:
      writeBrightnessPercent(0);
      break;
  }
}
