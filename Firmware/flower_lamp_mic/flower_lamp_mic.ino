/*
  ESP32 microphone-reactive breathing lamp controller.

  The lamp brightness follows the music's volume envelope: louder passages
  make it brighter, quiet passages let it fade toward the breathing floor.
  This tracks loudness, not pitch/melody -- reliable pitch/beat tracking
  needs far more compute than a single-core Arduino loop can spare, while
  a smoothed volume envelope already reads as "breathing with the music".

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
*/

const int LAMP_PIN = 23;
const int MIC_PIN = 34;

const int PWM_FREQUENCY_HZ = 1000;
const int PWM_RESOLUTION_BITS = 8;
const int PWM_MAX_DUTY = (1 << PWM_RESOLUTION_BITS) - 1;
const int MAX_BRIGHTNESS_PERCENT = 55;
const byte BREATH_FLOOR_PERCENT = 3;

const unsigned long SAMPLE_WINDOW_MS = 30;
const unsigned long CALIBRATION_MS = 1500;

const float ENVELOPE_ATTACK_ALPHA = 0.6f;
const float ENVELOPE_RELEASE_ALPHA = 0.05f;
const float BREATH_SMOOTHING_ALPHA = 0.08f;
const float CEILING_DECAY_ALPHA = 0.0008f;
const float NOISE_GATE_MULTIPLIER = 1.3f;
const float CEILING_HEADROOM_MULTIPLIER = 6.0f;

float noiseFloor = 0;
float envelope = 0;
float loudnessCeiling = 0;
float smoothedBrightnessPercent = BREATH_FLOOR_PERCENT;
unsigned long windowStartedAt = 0;
int windowMinReading = 4095;
int windowMaxReading = 0;

void writeBrightnessPercent(float percent) {
  float limitedPercent = min(percent, (float)MAX_BRIGHTNESS_PERCENT);
  int duty = (int)(PWM_MAX_DUTY * limitedPercent / 100.0f);
  ledcWrite(LAMP_PIN, duty);
}

float readPeakToPeakOverWindow(unsigned long windowMs) {
  int minReading = 4095;
  int maxReading = 0;
  unsigned long windowStart = millis();

  while (millis() - windowStart < windowMs) {
    int reading = analogRead(MIC_PIN);
    minReading = min(minReading, reading);
    maxReading = max(maxReading, reading);
  }

  return (float)(maxReading - minReading);
}

float calibrateNoiseFloor() {
  float total = 0;
  int samples = 0;
  unsigned long calibrationStart = millis();

  while (millis() - calibrationStart < CALIBRATION_MS) {
    total += readPeakToPeakOverWindow(SAMPLE_WINDOW_MS);
    samples++;
  }

  return samples > 0 ? total / samples : 0;
}

float smoothstep(float x) {
  float clamped = max(0.0f, min(1.0f, x));
  return clamped * clamped * (3.0f - 2.0f * clamped);
}

void setup() {
  analogSetPinAttenuation(MIC_PIN, ADC_11db);
  ledcAttach(LAMP_PIN, PWM_FREQUENCY_HZ, PWM_RESOLUTION_BITS);
  writeBrightnessPercent(0);

  noiseFloor = calibrateNoiseFloor();
  loudnessCeiling = max(noiseFloor * CEILING_HEADROOM_MULTIPLIER, 40.0f);
  envelope = noiseFloor;
}

void loop() {
  float peakToPeak = readPeakToPeakOverWindow(SAMPLE_WINDOW_MS);

  float attackOrRelease = peakToPeak > envelope ? ENVELOPE_ATTACK_ALPHA : ENVELOPE_RELEASE_ALPHA;
  envelope += (peakToPeak - envelope) * attackOrRelease;

  if (envelope > loudnessCeiling) {
    loudnessCeiling = envelope;
  } else {
    loudnessCeiling -= loudnessCeiling * CEILING_DECAY_ALPHA;
    loudnessCeiling = max(loudnessCeiling, noiseFloor * CEILING_HEADROOM_MULTIPLIER);
  }

  float normalized = 0;
  if (envelope > noiseFloor * NOISE_GATE_MULTIPLIER) {
    normalized = (envelope - noiseFloor) / (loudnessCeiling - noiseFloor);
  }

  float eased = smoothstep(normalized);
  float targetPercent = BREATH_FLOOR_PERCENT + eased * (MAX_BRIGHTNESS_PERCENT - BREATH_FLOOR_PERCENT);

  smoothedBrightnessPercent += (targetPercent - smoothedBrightnessPercent) * BREATH_SMOOTHING_ALPHA;
  writeBrightnessPercent(smoothedBrightnessPercent);
}
