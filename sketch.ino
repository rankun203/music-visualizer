#include "FastLED.h"

#define NUM_LEDS 64
#define NUM_X 8
#define NUM_Y 8
#define NUM_FPS 90

#define PIN_SOUND 2
#define PIN_LED 3

#define NUM_SOUND_MIN 1
#define NUM_SOUND_MAX 105
#define NUM_BRIGHTNESS 3

CRGB leds[NUM_LEDS];
CRGB COLORS[5]={CRGB::Red, CRGB::Orange, CRGB::Yellow, CRGB::Green, CRGB::Blue};

void setup() {
  FastLED.addLeds<NEOPIXEL, PIN_LED>(leds, NUM_LEDS);
  FastLED.setBrightness(NUM_BRIGHTNESS);
  pinMode(PIN_SOUND, INPUT);
  Serial.begin(9600);
}

/** 
 * Fps controller
 * @return true: render, false: not render
 */
unsigned long LAST_MS=millis();
bool shouldRenderAnim(int fps) {
  unsigned long nowMs = millis();
  unsigned long safeBufMs = 3;

  // Check if nosMs has passed this frame time
  bool shouldRender = floor(1000 / fps) < (nowMs - LAST_MS + safeBufMs);
  if (shouldRender) {
    LAST_MS = nowMs;
  }
  return shouldRender;
}

int LAST_READ_TOTAL=0;
int LAST_READ_TIMES=0;
void recordAnalog() {
  // FIX ME!: to avoid flashing
  // TODO: It should collect avg for last 16ms(60fps)
  int soundVal = analogRead(PIN_SOUND);
  LAST_READ_TOTAL += soundVal;
  LAST_READ_TIMES++;
}

// Normal refresh delay is 3ms
void loop() {
  recordAnalog();
  if(!shouldRenderAnim(NUM_FPS)) {
    return;
  }

  Serial.println(LAST_READ_TOTAL / LAST_READ_TIMES);
  int soundValAvg = floor(LAST_READ_TOTAL / LAST_READ_TIMES);
  LAST_READ_TOTAL=0; LAST_READ_TIMES=0;
  int height = map(soundValAvg, NUM_SOUND_MIN, NUM_SOUND_MAX, 0, NUM_Y);

  FastLED.clear();
  int yMid = NUM_Y/2;

  for (int x=0; x < height; x++) {
    int base = x * NUM_X;
    int horiDist = 0;
    for (int dot=yMid; dot>0; dot--) {
      leds[base + dot-1] = CRGB::Red;

      // Calculate right position, + horizontal distance
      int right = 1 + horiDist + dot -1;
      horiDist += 2;

      leds[base + right] = CRGB::Red;
    }
  }

  FastLED.show();
}
