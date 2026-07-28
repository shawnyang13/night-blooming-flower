/*
  ESP32 timed breathing melody lamp controller.

  Wiring:
  - Do not connect the lamp directly between VIN and GND if you need software control.
  - Use a constant-current LED driver such as the CN5711 module.
  - GPIO23 drives the CN5711 CE/PWM pin through a 100-220 ohm resistor.
*/

const int LAMP_PIN = 23;

const int PWM_FREQUENCY_HZ = 1000;
const int PWM_RESOLUTION_BITS = 8;
const int PWM_MAX_DUTY = (1 << PWM_RESOLUTION_BITS) - 1;
const int MAX_BRIGHTNESS_PERCENT = 55;

const unsigned long PLAY_DURATION_MS = 60UL * 1000UL;
const unsigned long REST_DURATION_MS = 60UL * 1000UL;
const unsigned long FADE_OUT_DURATION_MS = 8UL * 1000UL;
const byte BREATH_FLOOR_PERCENT = 3;

struct BreathStep {
  unsigned int durationMs;
  byte peakPercent;
  unsigned int gapMs;
};

const BreathStep BREATH_MELODY[] = {
  // Lullaby-like phrases: soft-soft-long, then a higher swell, then a quiet ending.
  {1500, 24, 180},
  {1500, 28, 240},
  {3600, 42, 700},

  {1500, 24, 180},
  {1500, 30, 240},
  {3600, 46, 900},

  {1800, 32, 160},
  {1800, 42, 160},
  {2600, 55, 240},
  {2600, 48, 600},

  {2200, 32, 220},
  {2200, 26, 220},
  {4200, 18, 1200},
};

const int BREATH_STEP_COUNT = sizeof(BREATH_MELODY) / sizeof(BREATH_MELODY[0]);

bool sequenceActive = true;
unsigned long sequenceStartedAt = 0;
unsigned long nextSequenceAt = 0;
byte outputScalePercent = 100;

void writeBrightnessPercent(byte percent) {
  byte limitedPercent = min(percent, (byte)MAX_BRIGHTNESS_PERCENT);
  int duty = PWM_MAX_DUTY * limitedPercent * outputScalePercent / 10000;
  ledcWrite(LAMP_PIN, duty);
}

byte easeBreathPercent(byte peakPercent, unsigned long elapsed, unsigned int durationMs) {
  unsigned long halfDuration = max(1U, durationMs / 2);
  unsigned long phase = elapsed < halfDuration ? elapsed : durationMs - elapsed;
  byte x = min(255UL, phase * 255UL / halfDuration);

  // Integer smoothstep curve: slow at both ends, soft in the middle.
  unsigned long eased = (unsigned long)x * x * (765UL - 2UL * x) / 65025UL;
  byte limitedPeak = min(peakPercent, (byte)MAX_BRIGHTNESS_PERCENT);
  return BREATH_FLOOR_PERCENT + (limitedPeak - BREATH_FLOOR_PERCENT) * eased / 255;
}

void startSequence() {
  sequenceActive = true;
  sequenceStartedAt = millis();
  outputScalePercent = 100;
  writeBrightnessPercent(0);
}

void stopSequence() {
  sequenceActive = false;
  outputScalePercent = 100;
  writeBrightnessPercent(0);
  nextSequenceAt = millis() + REST_DURATION_MS;
}

unsigned long melodyDurationMs() {
  unsigned long duration = 0;

  for (int i = 0; i < BREATH_STEP_COUNT; i++) {
    duration += BREATH_MELODY[i].durationMs + BREATH_MELODY[i].gapMs;
  }

  return duration;
}

void playMelodyAt(unsigned long sequenceElapsed) {
  unsigned long melodyElapsed = sequenceElapsed % melodyDurationMs();
  unsigned long cursor = 0;

  for (int i = 0; i < BREATH_STEP_COUNT; i++) {
    unsigned long breathEnd = cursor + BREATH_MELODY[i].durationMs;
    unsigned long gapEnd = breathEnd + BREATH_MELODY[i].gapMs;

    if (melodyElapsed < breathEnd) {
      unsigned long breathElapsed = melodyElapsed - cursor;
      writeBrightnessPercent(easeBreathPercent(
        BREATH_MELODY[i].peakPercent,
        breathElapsed,
        BREATH_MELODY[i].durationMs
      ));
      return;
    }

    if (melodyElapsed < gapEnd) {
      writeBrightnessPercent(0);
      return;
    }

    cursor = gapEnd;
  }

  writeBrightnessPercent(0);
}

void setup() {
  ledcAttach(LAMP_PIN, PWM_FREQUENCY_HZ, PWM_RESOLUTION_BITS);
  startSequence();
}

void loop() {
  unsigned long now = millis();

  if (sequenceActive) {
    unsigned long elapsed = now - sequenceStartedAt;

    if (elapsed >= PLAY_DURATION_MS) {
      stopSequence();
      return;
    }

    unsigned long remaining = PLAY_DURATION_MS - elapsed;
    if (remaining < FADE_OUT_DURATION_MS) {
      outputScalePercent = remaining * 100UL / FADE_OUT_DURATION_MS;
    } else {
      outputScalePercent = 100;
    }

    playMelodyAt(elapsed);
    return;
  }

  if ((long)(now - nextSequenceAt) >= 0) {
    startSequence();
  }
}
