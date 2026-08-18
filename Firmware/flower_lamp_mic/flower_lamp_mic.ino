#include <Preferences.h>
#include "FlowerLampConfig.h"

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
int currentLoudnessLevel = 0;
float transientBaselinePressure = 0.0f;
float transientPulsePercent = BREATH_FLOOR_PERCENT;
unsigned long lastTransientAt = 0;
unsigned long soundReactiveSettlesAt = 0;
unsigned long soundReactiveCalibratesAt = 0;
bool transientWasAboveThreshold = false;
bool soundReactiveCalibrationPending = false;
unsigned long lastDebugSerialAt = 0;
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
  unsigned long sampleCount = max(1UL, ((windowMs * (unsigned long)SAMPLE_RATE_HZ) + 999UL) / 1000UL);
  unsigned long sampleStartUs = micros();

  for (unsigned long sampleIndex = 0; sampleIndex < sampleCount; sampleIndex++) {
    unsigned long targetSampleUs = sampleStartUs + (unsigned long)(((uint64_t)sampleIndex * 1000000ULL) / SAMPLE_RATE_HZ);
    while ((long)(micros() - targetSampleUs) < 0) {
      delayMicroseconds(1);
    }

    int reading = analogRead(MIC_PIN);
    micDcCenter += ((float)reading - micDcCenter) * DC_CENTER_ALPHA;

    float acPressure = (float)reading - micDcCenter;
    sumSquares += acPressure * acPressure;
  }

  return sqrtf(sumSquares / sampleCount);
}

float calibrateNoiseFloor() {
  float mean = 0.0f;
  float m2 = 0.0f;
  int samples = 0;
  unsigned long calibrationStart = millis();

  while (millis() - calibrationStart < CALIBRATION_MS) {
    float rms = readSoundPressureRms(SAMPLE_WINDOW_MS);
    samples++;

    float delta = rms - mean;
    mean += delta / samples;
    float deltaAfterMean = rms - mean;
    m2 += delta * deltaAfterMean;
  }

  if (samples <= 0) {
    return 0.0f;
  }

  float variance = samples > 1 ? m2 / (samples - 1) : 0.0f;
  float stddev = sqrtf(max(0.0f, variance));
  return mean + stddev * NOISE_FLOOR_STDDEV_MULTIPLIER + NOISE_FLOOR_MARGIN_RMS;
}

void resetSoundReactiveRuntime() {
  writeBrightnessPercent(BREATH_FLOOR_PERCENT);
  smoothedBrightnessPercent = BREATH_FLOOR_PERCENT;
  currentLoudnessLevel = 0;
  transientBaselinePressure = 0.0f;
  transientPulsePercent = BREATH_FLOOR_PERCENT;
  lastTransientAt = 0;
  transientWasAboveThreshold = false;
  pressureEnvelope = noiseFloor;
  pressureCeiling = max(noiseFloor * CEILING_HEADROOM_MULTIPLIER, noiseFloor + MIN_PRESSURE_RANGE);
}

void enterSoundReactiveMode() {
  micDcCenter = analogRead(MIC_PIN);
  resetSoundReactiveRuntime();
  soundReactiveCalibrationPending = true;
  soundReactiveCalibratesAt = millis() + SOUND_REACTIVE_PRE_CALIBRATION_SETTLE_MS;
  soundReactiveSettlesAt = millis() + SOUND_REACTIVE_SETTLE_MS;

  if (DEBUG_SERIAL_ENABLED) {
    Serial.println("sound reactive: waiting before noise calibration");
  }
}

void finishSoundReactiveCalibration() {
  micDcCenter = analogRead(MIC_PIN);
  noiseFloor = calibrateNoiseFloor();
  resetSoundReactiveRuntime();
  soundReactiveCalibrationPending = false;
  soundReactiveSettlesAt = millis() + SOUND_REACTIVE_SETTLE_MS;

  if (DEBUG_SERIAL_ENABLED) {
    Serial.printf("sound reactive: calibrated noise floor %.2f\n", noiseFloor);
  }
}

float smoothstep(float x) {
  float clamped = clampFloat(x, 0.0f, 1.0f);
  return clamped * clamped * (3.0f - 2.0f * clamped);
}

int quantizedLoudnessLevel(float effectivePressure, float pressureRange) {
  if (effectivePressure <= LOUDNESS_FIRST_LEVEL_RMS) {
    currentLoudnessLevel = 0;
    return currentLoudnessLevel;
  }

  float mappedPressure = effectivePressure - LOUDNESS_FIRST_LEVEL_RMS;
  float mappedRange = max(pressureRange - LOUDNESS_FIRST_LEVEL_RMS, 1.0f);
  float normalized = log1pf(mappedPressure) / log1pf(mappedRange);
  float scaledLevel = clampFloat(normalized, 0.0f, 1.0f) * (LOUDNESS_LEVEL_COUNT - 1);
  float hysteresis = max(0.0f, LOUDNESS_HYSTERESIS_LEVELS);

  while (currentLoudnessLevel < LOUDNESS_LEVEL_COUNT - 1 &&
         scaledLevel >= currentLoudnessLevel + 0.5f + hysteresis) {
    currentLoudnessLevel++;
  }

  while (currentLoudnessLevel > 0 &&
         scaledLevel <= currentLoudnessLevel - 0.5f - hysteresis) {
    currentLoudnessLevel--;
  }

  return currentLoudnessLevel;
}

float brightnessPercentForLevel(int loudnessLevel) {
  int brightnessLevel = (int)roundf((float)loudnessLevel * (BRIGHTNESS_LEVEL_COUNT - 1) / (LOUDNESS_LEVEL_COUNT - 1));
  brightnessLevel = constrain(brightnessLevel, 0, BRIGHTNESS_LEVEL_COUNT - 1);

  float normalized = (float)brightnessLevel / (float)(BRIGHTNESS_LEVEL_COUNT - 1);
  float shaped = powf(normalized, max(0.1f, BRIGHTNESS_LEVEL_GAMMA));
  return BREATH_FLOOR_PERCENT + shaped * (MAX_BRIGHTNESS_PERCENT - BREATH_FLOOR_PERCENT);
}

float transientPulseTargetPercent(float activePressure, unsigned long now) {
  if (!TRANSIENT_ENABLED) {
    transientPulsePercent = BREATH_FLOOR_PERCENT;
    transientBaselinePressure += (max(0.0f, activePressure) - transientBaselinePressure) * TRANSIENT_BASELINE_ALPHA;
    transientWasAboveThreshold = false;
    return transientPulsePercent;
  }

  float baselineBeforeUpdate = transientBaselinePressure;
  float requiredRise = max(TRANSIENT_MIN_RISE_RMS, baselineBeforeUpdate * (TRANSIENT_THRESHOLD_MULTIPLIER - 1.0f));
  bool isAboveThreshold = activePressure > baselineBeforeUpdate + requiredRise;
  bool isTransient = isAboveThreshold &&
                     !transientWasAboveThreshold &&
                     now - lastTransientAt >= TRANSIENT_COOLDOWN_MS;

  if (isTransient) {
    transientPulsePercent = max(transientPulsePercent, (float)TRANSIENT_PULSE_PERCENT);
    lastTransientAt = now;
  } else {
    transientPulsePercent += (BREATH_FLOOR_PERCENT - transientPulsePercent) * TRANSIENT_PULSE_DECAY_ALPHA;
  }

  transientBaselinePressure += (max(0.0f, activePressure) - transientBaselinePressure) * TRANSIENT_BASELINE_ALPHA;
  transientWasAboveThreshold = isAboveThreshold;
  return transientPulsePercent;
}

void adaptNoiseFloor(float soundPressure) {
  if (soundPressure > noiseFloor && soundPressure <= noiseFloor + NOISE_FLOOR_ADAPT_MAX_RISE_RMS) {
    noiseFloor += (soundPressure - noiseFloor) * NOISE_FLOOR_ADAPT_ALPHA;
  }
}

void debugSoundReactive(float soundPressure, float noiseGate, float activePressure, int loudnessLevel, float targetPercent, unsigned long now) {
  if (!DEBUG_SERIAL_ENABLED || now - lastDebugSerialAt < DEBUG_SERIAL_INTERVAL_MS) {
    return;
  }

  lastDebugSerialAt = now;
  Serial.printf(
      "rms=%.2f floor=%.2f gate=%.2f active=%.2f ceiling=%.2f level=%d basePulse=%.2f out=%.2f\n",
      soundPressure,
      noiseFloor,
      noiseGate,
      activePressure,
      pressureCeiling,
      loudnessLevel,
      transientPulsePercent,
      targetPercent);
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
    enterSoundReactiveMode();
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

  return BREATH_FLOOR_PERCENT + eased * (BREATH_PEAK_PERCENT - BREATH_FLOOR_PERCENT);
}

void runSoundReactiveMode() {
  unsigned long now = millis();

  if (soundReactiveCalibrationPending) {
    writeBrightnessPercent(BREATH_FLOOR_PERCENT);
    if ((long)(now - soundReactiveCalibratesAt) < 0) {
      return;
    }
    finishSoundReactiveCalibration();
    return;
  }

  float soundPressure = readSoundPressureRms(SAMPLE_WINDOW_MS);
  adaptNoiseFloor(soundPressure);

  float pressureAlpha = soundPressure > pressureEnvelope ? PRESSURE_ATTACK_ALPHA : PRESSURE_RELEASE_ALPHA;
  pressureEnvelope += (soundPressure - pressureEnvelope) * pressureAlpha;

  if (pressureEnvelope > pressureCeiling) {
    pressureCeiling = pressureEnvelope;
  } else {
    float ceilingFloor = max(noiseFloor * CEILING_HEADROOM_MULTIPLIER, noiseFloor + MIN_PRESSURE_RANGE);
    pressureCeiling += (ceilingFloor - pressureCeiling) * CEILING_DECAY_ALPHA;
  }

  float noiseGate = max(noiseFloor * NOISE_GATE_MULTIPLIER, noiseFloor + NOISE_GATE_OFFSET);
  float activePressure = pressureEnvelope - noiseGate - LOUDNESS_ACTIVATION_RMS;
  float pressureRange = max(MIN_PRESSURE_RANGE, pressureCeiling - noiseGate - LOUDNESS_ACTIVATION_RMS);
  now = millis();

  if ((long)(now - soundReactiveSettlesAt) < 0) {
    currentLoudnessLevel = 0;
    transientPulsePercent = BREATH_FLOOR_PERCENT;
    transientBaselinePressure = max(transientBaselinePressure, max(0.0f, activePressure));
    transientWasAboveThreshold = false;
    lastTransientAt = now;
    smoothedBrightnessPercent += (BREATH_FLOOR_PERCENT - smoothedBrightnessPercent) * BRIGHTNESS_RELEASE_ALPHA;
    writeBrightnessPercent(smoothedBrightnessPercent);
    return;
  }
  int loudnessLevel = quantizedLoudnessLevel(activePressure, pressureRange);
  float basePercent = brightnessPercentForLevel(loudnessLevel);
  float pulsePercent = transientPulseTargetPercent(activePressure, now);
  float targetPercent = max(basePercent, pulsePercent);
  float brightnessAlpha = targetPercent > smoothedBrightnessPercent ? BRIGHTNESS_ATTACK_ALPHA : BRIGHTNESS_RELEASE_ALPHA;

  smoothedBrightnessPercent += (targetPercent - smoothedBrightnessPercent) * brightnessAlpha;
  debugSoundReactive(soundPressure, noiseGate, activePressure, loudnessLevel, targetPercent, now);
  writeBrightnessPercent(smoothedBrightnessPercent);
}

void setup() {
  if (DEBUG_SERIAL_ENABLED) {
    Serial.begin(115200);
  }

  pinMode(MODE_BUTTON_PIN, INPUT_PULLUP);
  analogSetPinAttenuation(MIC_PIN, ADC_11db);
  ledcAttach(LAMP_PIN, PWM_FREQUENCY_HZ, PWM_RESOLUTION_BITS);
  writeBrightnessPercent(0);

  preferences.begin("flower", false);
  loadMode();

  if (activeMode == MODE_SOUND_REACTIVE) {
    enterSoundReactiveMode();
  } else {
    resetSoundReactiveRuntime();
  }
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
