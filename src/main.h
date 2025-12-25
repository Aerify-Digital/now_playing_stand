#include <Arduino.h>
#include <FastLED.h>
#include <IRremote.h>
#include <ir_codes.h>
#include <FreeRTOS.h>
#include <task.h>
#include <stdint.h>
#include <vector>
#include "debug.h"
#include "pindefs.h"
#include "flash.h"

#define samples (256)

#define CHIPSET WS2812B
#define COLOR_ORDER RGB

int audioInRaw;
int audioInRectified;

enum AnimationType
{
    ANIMATION_NONE,
    ANIMATION_COLOR_WIPE,
    ANIMATION_FIRE,
    ANIMATION_METEOR,
    ANIMATION_RAINBOW,
    ANIMATION_STARS,
    ANIMATION_MUSIC,
    ANIMATION_SHUF_ALL,
    ANIMATION_SHUF_FAV,
    ANIMATION_SHUF_MUSIC,
};

enum MusicMode
{
    VISUALIZER,
    BEAT_FLASH,
    BEAT_PULSE,
    BEAT_WAVE,
    BEAT_SPARKLE
};

struct AnimationState
{
    AnimationType type = ANIMATION_NONE;
    CRGB color = CRGB::Black;
    int wait = 50;
    int length = 4;
    int step = 0;
    unsigned long lastUpdate = 0;
};

extern Flash flash;

std::vector<AnimationState> favoriteAnimations;

void loadFavorites();

void saveFavorites();

void addFavorite(const AnimationState &state);

void removeFavorite(const AnimationState &state);

bool isFavorite(const AnimationState &state);

void setColor(const CRGB &color);

void changeColor(const CRGB &newColor);

void wipe(AnimationState &state);

void rainbow(AnimationState &state);

void meteor(AnimationState &state);

void stars(AnimationState &state);

void fire(AnimationState &state);

void visualizer(const AnimationState &state);

void beatFlash(const AnimationState &state);

void beatPulse(const AnimationState &state);

void beatWave(const AnimationState &state);

void beatSparkle(const AnimationState &state);

void shuffleAll(AnimationState &state);

void shuffleFavorites(AnimationState &state);

void shuffleMusic(AnimationState &state);

AnimationState getRandomAnimation();

AnimationState getRandomFavorite();

MusicMode getRandomMusicMode();