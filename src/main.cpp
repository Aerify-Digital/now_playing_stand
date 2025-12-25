
#include "main.h"

SemaphoreHandle_t stateMutex = NULL;

TaskHandle_t irTaskHandle = NULL;
TaskHandle_t argbTaskHandle = NULL;
TaskHandle_t micTaskHandle = NULL;

CRGB xleds[LED_COUNT];

AnimationState animationState = {
    ANIMATION_COLOR_WIPE,
    0,  // color
    25, // wait
    4,  // length
    0,  // step
    0   // lastUpdate
};

AnimationState shuffleAnimationState = {
    ANIMATION_COLOR_WIPE,
    0,  // color
    25, // wait
    4,  // length
    0,  // step
    0   // lastUpdate
};

MusicMode musicMode = VISUALIZER;

volatile bool on = true;
volatile bool changingColor = false;
unsigned long lastColorChangeTime = 0;
int favoriteIndex = -1;
volatile bool beatDetected = false;
unsigned long lastBeatTime = 0;
unsigned long lastShuffleTime = 0;

void setInitialAnimation()
{
    xSemaphoreTake(stateMutex, portMAX_DELAY);
    animationState.type = ANIMATION_COLOR_WIPE;
    animationState.color = xleds->setRGB(0xff, 0xff, 0xff);
    animationState.step = 0;
    animationState.wait = 25;
    animationState.length = 4;
    animationState.lastUpdate = millis();
    xSemaphoreGive(stateMutex);
}

void IRTask(void *pvParameters)
{
    IrReceiver.begin(IR_PIN, false);
    for (;;)
    {
        if (millis() - lastColorChangeTime >= 3000)
        {
            xSemaphoreTake(stateMutex, portMAX_DELAY);
            changingColor = false;
            lastColorChangeTime = 0;
            xSemaphoreGive(stateMutex);
        }

        if (IrReceiver.decode())
        {
            if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT)
            {
                IrReceiver.resume();
                continue;
            }
            DEBUG_IR(IrReceiver.printIRResultShort(&Serial);)
            switch (IrReceiver.decodedIRData.protocol)
            {
            case NEC:
                DEBUG_PRINTLN(" - NEC protocol detected");
                if (!on && IrReceiver.decodedIRData.command != IR_ON)
                {
                    DEBUG_PRINTLN("   - Ignoring command since device is OFF");
                    IrReceiver.resume();
                    continue;
                }
                switch (IrReceiver.decodedIRData.command)
                {
                case IR_ON:
                    DEBUG_PRINTLN("   - Power ON command received");
                    if (!on)
                    {
                        on = true;
                        setInitialAnimation();
                        xSemaphoreTake(stateMutex, portMAX_DELAY);
                        animationState.color = xleds->setRGB(0xff, 0xff, 0xff);
                        animationState.type = ANIMATION_COLOR_WIPE;
                        animationState.step = 0;
                        animationState.wait = 25;
                        xSemaphoreGive(stateMutex);
                        FastLED.setBrightness(127);
                    }
                    if (changingColor)
                    {
                        xSemaphoreTake(stateMutex, portMAX_DELAY);
                        changingColor = false;
                        lastColorChangeTime = 0;
                        xSemaphoreGive(stateMutex);
                    }
                    break;
                case IR_OFF:
                    DEBUG_PRINTLN("   - Power OFF command received");
                    if (on)
                    {

                        setInitialAnimation();
                        xSemaphoreTake(stateMutex, portMAX_DELAY);
                        on = false;
                        animationState.color = xleds->setRGB(0x00, 0x00, 0x00);
                        animationState.type = ANIMATION_COLOR_WIPE;
                        animationState.step = 0;
                        animationState.wait = 25;
                        xSemaphoreGive(stateMutex);
                    }
                    if (changingColor)
                    {
                        xSemaphoreTake(stateMutex, portMAX_DELAY);
                        changingColor = false;
                        lastColorChangeTime = 0;
                        xSemaphoreGive(stateMutex);
                    }
                    break;
                case IR_R1:
                    DEBUG_PRINTLN("   - R1 command received");
                    if (changingColor)
                    {
                        DEBUG_PRINTLN("   - Alternate ");
                        changeColor(xleds->setRGB(0xff, 0x00, 0x00));
                    }
                    else
                    {
                        setColor(xleds->setRGB(0xff, 0x00, 0x00));
                    }
                    break;
                case IR_R2:
                    DEBUG_PRINTLN("   - R2 command received");
                    if (changingColor)
                    {
                        DEBUG_PRINTLN("   - Alternate ");
                        changeColor(xleds->setRGB(0xff, 0x0b, 0x00));
                    }
                    else
                    {
                        setColor(xleds->setRGB(0xff, 0x0b, 0x00));
                    }
                    break;
                case IR_R3:
                    DEBUG_PRINTLN("   - R3 command received");
                    if (changingColor)
                    {
                        DEBUG_PRINTLN("   - Alternate ");
                        changeColor(xleds->setRGB(0xff, 0x1f, 0x00));
                    }
                    else
                    {
                        setColor(xleds->setRGB(0xff, 0x1f, 0x00));
                    }
                    break;
                case IR_R4:
                    DEBUG_PRINTLN("   - R4 command received");
                    if (changingColor)
                    {
                        DEBUG_PRINTLN("   - Alternate ");
                        changeColor(xleds->setRGB(0xff, 0x39, 0x00));
                    }
                    else
                    {
                        setColor(xleds->setRGB(0xff, 0x39, 0x00));
                    }
                    break;
                case IR_R5:
                    DEBUG_PRINTLN("   - R5 command received");
                    if (changingColor)
                    {
                        DEBUG_PRINTLN("   - Alternate ");
                        changeColor(xleds->setRGB(0xff, 0x7f, 0x00));
                    }
                    else
                    {
                        setColor(xleds->setRGB(0xff, 0x7f, 0x00));
                    }
                    break;
                case IR_G1:
                    DEBUG_PRINTLN("   - G1 command received");
                    if (changingColor)
                    {
                        DEBUG_PRINTLN("   - Alternate ");
                        changeColor(xleds->setRGB(0x00, 0xff, 0x00));
                    }
                    else
                    {
                        setColor(xleds->setRGB(0x00, 0xff, 0x00));
                    }
                    break;
                case IR_G2:
                    DEBUG_PRINTLN("   - G2 command received");
                    if (changingColor)
                    {
                        DEBUG_PRINTLN("   - Alternate ");
                        changeColor(xleds->setRGB(0x00, 0xff, 0x1f));
                    }
                    else
                    {
                        setColor(xleds->setRGB(0x00, 0xff, 0x1f));
                    }
                    break;
                case IR_G3:
                    DEBUG_PRINTLN("   - G3 command received");
                    if (changingColor)
                    {
                        DEBUG_PRINTLN("   - Alternate ");
                        changeColor(xleds->setRGB(0x00, 0xff, 0x3f));
                    }
                    else
                    {
                        setColor(xleds->setRGB(0x00, 0xff, 0x3f));
                    }
                    break;
                case IR_G4:
                    DEBUG_PRINTLN("   - G4 command received");
                    if (changingColor)
                    {
                        DEBUG_PRINTLN("   - Alternate ");
                        changeColor(xleds->setRGB(0x00, 0xff, 0x7f));
                    }
                    else
                    {
                        setColor(xleds->setRGB(0x00, 0xff, 0x7f));
                    }
                    break;
                case IR_G5:
                    DEBUG_PRINTLN("   - G5 command received");
                    if (changingColor)
                    {
                        DEBUG_PRINTLN("   - Alternate ");
                        changeColor(xleds->setRGB(0x00, 0xff, 0xff));
                    }
                    else
                    {
                        setColor(xleds->setRGB(0x00, 0xff, 0xff));
                    }
                    break;
                case IR_B1:
                    DEBUG_PRINTLN("   - B1 command received");
                    if (changingColor)
                    {
                        DEBUG_PRINTLN("   - Alternate ");
                        changeColor(xleds->setRGB(0x00, 0x00, 0xff));
                    }
                    else
                    {
                        setColor(xleds->setRGB(0x00, 0x00, 0xff));
                    }
                    break;
                case IR_B2:
                    DEBUG_PRINTLN("   - B2 command received");
                    if (changingColor)
                    {
                        DEBUG_PRINTLN("   - Alternate ");
                        changeColor(xleds->setRGB(0x1f, 0x00, 0xff));
                    }
                    else
                    {
                        setColor(xleds->setRGB(0x1f, 0x00, 0xff));
                    }
                    break;
                case IR_B3:
                    DEBUG_PRINTLN("   - B3 command received");
                    if (changingColor)
                    {
                        DEBUG_PRINTLN("   - Alternate ");
                        changeColor(xleds->setRGB(0x3f, 0x00, 0xff));
                    }
                    else
                    {
                        setColor(xleds->setRGB(0x3f, 0x00, 0xff));
                    }
                    break;
                case IR_B4:
                    DEBUG_PRINTLN("   - B4 command received");
                    if (changingColor)
                    {
                        DEBUG_PRINTLN("   - Alternate ");
                        changeColor(xleds->setRGB(0x7f, 0x00, 0xff));
                    }
                    else
                    {
                        setColor(xleds->setRGB(0x7f, 0x00, 0xff));
                    }
                    break;
                case IR_B5:
                    DEBUG_PRINTLN("   - B5 command received");
                    if (changingColor)
                    {
                        DEBUG_PRINTLN("   - Alternate ");
                        changeColor(xleds->setRGB(0xff, 0x00, 0xff));
                    }
                    else
                    {
                        setColor(xleds->setRGB(0xff, 0x00, 0xff));
                    }
                    break;
                case IR_W:
                    DEBUG_PRINTLN("   - W command received");
                    if (changingColor)
                    {
                        DEBUG_PRINTLN("   - Alternate ");
                        changeColor(xleds->setRGB(0xff, 0xff, 0xff));
                    }
                    else
                    {
                        setColor(xleds->setRGB(0xff, 0xff, 0xff));
                    }
                    break;
                case IR_RAINBOW:
                    DEBUG_PRINTLN("   - RAINBOW command received");
                    xSemaphoreTake(stateMutex, portMAX_DELAY);
                    animationState.type = ANIMATION_RAINBOW;
                    animationState.step = 0;
                    animationState.wait = 50;
                    animationState.length = (LED_COUNT / 2);
                    if (changingColor)
                    {
                        changingColor = false;
                        lastColorChangeTime = 0;
                    }
                    xSemaphoreGive(stateMutex);
                    break;
                case IR_METEOR:
                    DEBUG_PRINTLN("   - METEOR command received");
                    xSemaphoreTake(stateMutex, portMAX_DELAY);
                    animationState.type = ANIMATION_METEOR;
                    animationState.color = xleds->setRGB(0xff, 0xff, 0xff);
                    animationState.step = (LED_COUNT / 2) - 1;
                    animationState.wait = 100;
                    animationState.length = (LED_COUNT / 2);
                    if (changingColor)
                    {
                        changingColor = false;
                        lastColorChangeTime = 0;
                    }
                    xSemaphoreGive(stateMutex);
                    break;
                case IR_STARS:
                    DEBUG_PRINTLN("   - STARS command received");
                    xSemaphoreTake(stateMutex, portMAX_DELAY);
                    animationState.type = ANIMATION_STARS;
                    animationState.step = 0;
                    animationState.wait = 175;
                    animationState.length = 8;
                    animationState.color = xleds->setRGB(0xff, 0xff, 0xff);
                    if (changingColor)
                    {
                        changingColor = false;
                        lastColorChangeTime = 0;
                    }
                    xSemaphoreGive(stateMutex);
                    break;
                case IR_FIRE:
                    DEBUG_PRINTLN("   - FIRE command received");
                    xSemaphoreTake(stateMutex, portMAX_DELAY);
                    animationState.type = ANIMATION_FIRE;
                    animationState.step = 0;
                    animationState.wait = 125;
                    animationState.length = 4;
                    animationState.color = xleds->setRGB(0xff, 0x1f, 0x00);
                    if (changingColor)
                    {
                        changingColor = false;
                        lastColorChangeTime = 0;
                    }
                    xSemaphoreGive(stateMutex);
                    break;
                case IR_MODE_UP:
                    DEBUG_PRINTLN("   - MODE_UP command received");
                    xSemaphoreTake(stateMutex, portMAX_DELAY);
                    if (animationState.type < ANIMATION_COLOR_WIPE || animationState.type >= ANIMATION_MUSIC)
                    {
                        animationState.type = ANIMATION_COLOR_WIPE;
                    }
                    else
                    {
                        animationState.type = static_cast<AnimationType>(animationState.type + 1);
                        if (animationState.type > ANIMATION_MUSIC)
                        {
                            animationState.type = ANIMATION_COLOR_WIPE;
                        }
                    }
                    if (changingColor)
                    {
                        changingColor = false;
                        lastColorChangeTime = 0;
                    }
                    animationState.step = 0;
                    animationState.lastUpdate = millis();
                    DEBUG_PRINT("   - Animation type set to ");
                    DEBUG_PRINTLN(animationState.type);
                    xSemaphoreGive(stateMutex);
                    break;
                case IR_MODE_DOWN:
                    DEBUG_PRINTLN("   - MODE_DOWN command received");
                    xSemaphoreTake(stateMutex, portMAX_DELAY);
                    if (animationState.type < ANIMATION_COLOR_WIPE || animationState.type > ANIMATION_MUSIC)
                    {
                        animationState.type = ANIMATION_MUSIC;
                    }
                    else
                    {
                        animationState.type = static_cast<AnimationType>(animationState.type - 1);
                        if (animationState.type < ANIMATION_COLOR_WIPE)
                        {
                            animationState.type = ANIMATION_MUSIC;
                        }
                    }
                    if (changingColor)
                    {
                        changingColor = false;
                        lastColorChangeTime = 0;
                    }
                    animationState.step = 0;
                    animationState.lastUpdate = millis();
                    DEBUG_PRINT("   - Animation type set to ");
                    DEBUG_PRINTLN(animationState.type);
                    xSemaphoreGive(stateMutex);
                    break;
                case IR_FAV_UP:
                    DEBUG_PRINTLN("   - FAV_UP command received");
                    if (changingColor)
                    {
                        if (animationState.type == ANIMATION_SHUF_ALL ||
                            animationState.type == ANIMATION_SHUF_FAV ||
                            animationState.type == ANIMATION_SHUF_MUSIC)
                        {
                            DEBUG_PRINTLN("   - Cannot add shuffle mode to favorites.");
                            break;
                        }
                        addFavorite(animationState);
                        xSemaphoreTake(stateMutex, portMAX_DELAY);
                        changingColor = false;
                        lastColorChangeTime = 0;
                        xSemaphoreGive(stateMutex);
                    }
                    else
                    {
                        if (favoriteAnimations.size() == 0)
                        {
                            DEBUG_PRINTLN("   - No favorites saved.");
                            break;
                        }
                        xSemaphoreTake(stateMutex, portMAX_DELAY);
                        favoriteIndex = (favoriteIndex + 1) % favoriteAnimations.size();
                        animationState = favoriteAnimations[favoriteIndex];
                        DEBUG_PRINT("   - Favorite index set to ");
                        DEBUG_PRINTLN(favoriteIndex);
                        xSemaphoreGive(stateMutex);
                    }
                    break;
                case IR_FAV_DOWN:
                    DEBUG_PRINTLN("   - FAV_DOWN command received");

                    if (changingColor)
                    {
                        removeFavorite(animationState);
                        xSemaphoreTake(stateMutex, portMAX_DELAY);
                        changingColor = false;
                        lastColorChangeTime = 0;
                        xSemaphoreGive(stateMutex);
                    }
                    else
                    {
                        if (favoriteAnimations.size() == 0)
                        {
                            DEBUG_PRINTLN("   - No favorites saved.");
                            break;
                        }
                        xSemaphoreTake(stateMutex, portMAX_DELAY);
                        favoriteIndex--;
                        if (favoriteIndex < 0)
                        {
                            favoriteIndex = favoriteAnimations.size() - 1;
                        }
                        animationState = favoriteAnimations[favoriteIndex];
                        DEBUG_PRINT("   - Favorite index set to ");
                        DEBUG_PRINTLN(favoriteIndex);
                        xSemaphoreGive(stateMutex);
                    }
                    break;
                case IR_RAND_SHUF:
                    DEBUG_PRINTLN("   - RAND_SHUF command received");
                    xSemaphoreTake(stateMutex, portMAX_DELAY);
                    if (changingColor)
                    {
                        changingColor = false;
                        lastColorChangeTime = 0;
                    }
                    animationState.type = ANIMATION_SHUF_ALL;
                    lastShuffleTime = 0;
                    xSemaphoreGive(stateMutex);
                    break;
                case IR_FAV_SHUF:
                    DEBUG_PRINTLN("   - FAV_SHUF command received");
                    xSemaphoreTake(stateMutex, portMAX_DELAY);
                    if (changingColor)
                    {
                        changingColor = false;
                        lastColorChangeTime = 0;
                    }
                    if (favoriteAnimations.size() == 0)
                    {
                        DEBUG_PRINTLN("   - No favorites saved.");
                        xSemaphoreGive(stateMutex);
                        break;
                    }
                    animationState.type = ANIMATION_SHUF_FAV;
                    lastShuffleTime = 0;
                    xSemaphoreGive(stateMutex);
                    break;
                case IR_SPEED_UP:
                    DEBUG_PRINTLN("   - SPEED_UP command received");
                    xSemaphoreTake(stateMutex, portMAX_DELAY);
                    animationState.wait = max(0, animationState.wait - 5);
                    DEBUG_PRINT("new wait time: ");
                    DEBUG_PRINTLN(animationState.wait);
                    if (changingColor)
                    {
                        changingColor = false;
                        lastColorChangeTime = 0;
                    }
                    xSemaphoreGive(stateMutex);
                    break;
                case IR_SPEED_DOWN:
                    DEBUG_PRINTLN("   - SPEED_DOWN command received");
                    xSemaphoreTake(stateMutex, portMAX_DELAY);
                    animationState.wait = min(500, animationState.wait + 5);
                    DEBUG_PRINT("new wait time: ");
                    DEBUG_PRINTLN(animationState.wait);
                    if (changingColor)
                    {
                        changingColor = false;
                        lastColorChangeTime = 0;
                    }
                    xSemaphoreGive(stateMutex);
                    break;
                case IR_BRIGHT_UP:
                    DEBUG_PRINTLN("   - BRIGHT_UP command received");
                    FastLED.setBrightness(min(255, FastLED.getBrightness() + 16));
                    FastLED.show();
                    DEBUG_PRINT("new brightness: ");
                    DEBUG_PRINTLN(FastLED.getBrightness());

                    if (changingColor)
                    {
                        xSemaphoreTake(stateMutex, portMAX_DELAY);
                        changingColor = false;
                        lastColorChangeTime = 0;
                        xSemaphoreGive(stateMutex);
                    }
                    break;
                case IR_BRIGHT_DOWN:
                    DEBUG_PRINTLN("   - BRIGHT_DOWN command received");
                    FastLED.setBrightness(max(0, FastLED.getBrightness() - 16));
                    FastLED.show();
                    DEBUG_PRINT("new brightness: ");
                    DEBUG_PRINTLN(FastLED.getBrightness());
                    if (changingColor)
                    {
                        xSemaphoreTake(stateMutex, portMAX_DELAY);
                        changingColor = false;
                        lastColorChangeTime = 0;
                        xSemaphoreGive(stateMutex);
                    }
                    break;
                case IR_LENGTH_UP:
                    DEBUG_PRINTLN("   - LENGTH_UP command received");
                    xSemaphoreTake(stateMutex, portMAX_DELAY);
                    animationState.length = min(LED_COUNT - 1, animationState.length + 1);
                    DEBUG_PRINT("new length: ");
                    DEBUG_PRINTLN(animationState.length);
                    if (changingColor)
                    {
                        changingColor = false;
                        lastColorChangeTime = 0;
                    }
                    xSemaphoreGive(stateMutex);
                    break;
                case IR_LENGTH_DOWN:
                    DEBUG_PRINTLN("   - LENGTH_DOWN command received");
                    xSemaphoreTake(stateMutex, portMAX_DELAY);
                    animationState.length = max(1, animationState.length - 1);
                    DEBUG_PRINT("new length: ");
                    DEBUG_PRINTLN(animationState.length);
                    if (changingColor)
                    {
                        changingColor = false;
                        lastColorChangeTime = 0;
                    }
                    xSemaphoreGive(stateMutex);
                    break;
                case IR_MUSIC_UP:
                    DEBUG_PRINTLN("   - MUSIC_UP command received");
                    xSemaphoreTake(stateMutex, portMAX_DELAY);
                    if (animationState.type != ANIMATION_MUSIC)
                    {

                        animationState.type = ANIMATION_MUSIC;
                        animationState.step = 0;
                        animationState.lastUpdate = millis();
                    }
                    if (musicMode == VISUALIZER)
                    {
                        musicMode = BEAT_FLASH;
                        DEBUG_PRINTLN("   - Music mode set to BEAT_FLASH");
                    }
                    else if (musicMode == BEAT_FLASH)
                    {
                        musicMode = BEAT_PULSE;
                        DEBUG_PRINTLN("   - Music mode set to BEAT_PULSE");
                    }
                    else if (musicMode == BEAT_PULSE)
                    {
                        musicMode = BEAT_WAVE;
                        DEBUG_PRINTLN("   - Music mode set to BEAT_WAVE");
                    }
                    else if (musicMode == BEAT_WAVE)
                    {
                        musicMode = BEAT_SPARKLE;
                        DEBUG_PRINTLN("   - Music mode set to BEAT_SPARKLE");
                    }
                    else
                    {
                        musicMode = VISUALIZER;
                        DEBUG_PRINTLN("   - Music mode set to VISUALIZER");
                    }
                    if (changingColor)
                    {
                        changingColor = false;
                        lastColorChangeTime = 0;
                    }
                    xSemaphoreGive(stateMutex);
                    break;
                case IR_MUSIC_DOWN:
                    DEBUG_PRINTLN("   - MUSIC_DOWN command received");
                    xSemaphoreTake(stateMutex, portMAX_DELAY);
                    if (animationState.type != ANIMATION_MUSIC)
                    {
                        animationState.type = ANIMATION_MUSIC;
                        animationState.step = 0;
                        animationState.lastUpdate = millis();
                    }
                    if (musicMode == VISUALIZER)
                    {
                        musicMode = BEAT_SPARKLE;
                        DEBUG_PRINTLN("   - Music mode set to BEAT_SPARKLE");
                    }
                    else if (musicMode == BEAT_SPARKLE)
                    {
                        musicMode = BEAT_WAVE;
                        DEBUG_PRINTLN("   - Music mode set to BEAT_WAVE");
                    }
                    else if (musicMode == BEAT_WAVE)
                    {
                        musicMode = BEAT_PULSE;
                        DEBUG_PRINTLN("   - Music mode set to BEAT_PULSE");
                    }
                    else if (musicMode == BEAT_PULSE)
                    {
                        musicMode = BEAT_FLASH;
                        DEBUG_PRINTLN("   - Music mode set to BEAT_FLASH");
                    }
                    else
                    {
                        musicMode = VISUALIZER;
                        DEBUG_PRINTLN("   - Music mode set to VISUALIZER");
                    }
                    if (changingColor)
                    {
                        changingColor = false;
                        lastColorChangeTime = 0;
                    }
                    xSemaphoreGive(stateMutex);
                    break;
                case IR_MUSIC_SHUF:
                    DEBUG_PRINTLN("   - MUSIC_SHUF command received");
                    xSemaphoreTake(stateMutex, portMAX_DELAY);
                    if (changingColor)
                    {
                        changingColor = false;
                        lastColorChangeTime = 0;
                    }
                    animationState.type = ANIMATION_SHUF_MUSIC;
                    lastShuffleTime = 0;
                    xSemaphoreGive(stateMutex);
                    break;
                case IR_CHANGE_COLOR:
                    DEBUG_PRINTLN("   - CHANGE_COLOR command received");
                    xSemaphoreTake(stateMutex, portMAX_DELAY);
                    if (changingColor == false)
                    {
                        changingColor = true;
                        lastColorChangeTime = millis();
                    }
                    else
                    {
                        changingColor = false;
                        lastColorChangeTime = 0;
                    }
                    DEBUG_PRINT("   - changingColor set to ");
                    DEBUG_PRINTLN(changingColor ? "true" : "false");
                    xSemaphoreGive(stateMutex);
                    break;
                default:
                    DEBUG_PRINT("   - Command received: 0x");
                    DEBUG_PRINT(IrReceiver.decodedIRData.command, HEX);
                    DEBUG_PRINT(" ");
                    DEBUG_PRINTLN(IRCodeToString(static_cast<IRCode>(IrReceiver.decodedIRData.command)));
                    break;
                }
                break;
            default:
                DEBUG_PRINTLN(" - Other protocol detected");
                break;
            }
            IrReceiver.resume();
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

void ARGBTask(void *pvParameters)
{
    setInitialAnimation();
    FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(xleds, LED_COUNT);
    FastLED.setBrightness(127);
    FastLED.show();
    for (;;)
    {

        switch (animationState.type)
        {
        case ANIMATION_COLOR_WIPE:
            wipe(animationState);
            break;
        case ANIMATION_RAINBOW:
            rainbow(animationState);
            break;
        case ANIMATION_METEOR:
            meteor(animationState);
            break;
        case ANIMATION_STARS:
            stars(animationState);
            break;
        case ANIMATION_FIRE:
            fire(animationState);
            break;
        case ANIMATION_MUSIC:
            switch (musicMode)
            {
            case VISUALIZER:
                visualizer(animationState);
                break;
            case BEAT_FLASH:
                beatFlash(animationState);
                break;
            case BEAT_PULSE:
                beatPulse(animationState);
                break;
            case BEAT_WAVE:
                beatWave(animationState);
                break;
            case BEAT_SPARKLE:
                beatSparkle(animationState);
                break;
            default:
                break;
            }
            break;
        case ANIMATION_NONE:
            setColor(animationState.color);
            break;
        case ANIMATION_SHUF_ALL:
            shuffleAll(shuffleAnimationState);
            break;
        case ANIMATION_SHUF_FAV:
            shuffleFavorites(shuffleAnimationState);
            break;
        case ANIMATION_SHUF_MUSIC:
            shuffleMusic(shuffleAnimationState);
            break;
        default:
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

void MicTask(void *pvParameters)
{
    for (;;)
    {
        long audioAverage = 0, audioMax = 0, audioMin = 1024;

        for (int i = 0; i < samples; i++)
        {
            int audioInRaw = analogRead(MIC_PIN);

            audioInRectified = abs(audioInRaw - 337);
            audioMin = min(audioMin, audioInRaw);
            audioMax = max(audioMax, audioInRaw);
            audioAverage += audioInRaw;
            delayMicroseconds(22); // Approx 22us for 45kHz sampling rate
        }

        audioAverage /= samples;

        DEBUG_PRINT(">audioInRectified:");
        DEBUG_PRINT(audioInRectified);
        DEBUG_PRINT(",");
        DEBUG_PRINT("audioMin:");
        DEBUG_PRINT(audioMin);
        DEBUG_PRINT(",");
        DEBUG_PRINT("audioMax:");
        DEBUG_PRINT(audioMax);
        DEBUG_PRINT(",");
        DEBUG_PRINT("audioAverage:");
        DEBUG_PRINT(audioAverage);
        DEBUG_PRINT(",");
        DEBUG_PRINT("audioPeakToPeak:");
        DEBUG_PRINTLN(audioMax - audioMin);

        static float smoothedVolume = 0;
        const float smoothing = 0.5;
        const int beatThreshold = 55;

        smoothedVolume = smoothing * smoothedVolume + (1.0 - smoothing) * (audioMax - audioMin);

        if ((audioMax - audioMin) >= smoothedVolume + beatThreshold && millis() - lastBeatTime > 16)
        {
            beatDetected = true;
            lastBeatTime = millis();
        }
        else
        {
            beatDetected = false;
        }

        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

void setup()
{
    stateMutex = xSemaphoreCreateMutex();
    Serial.begin(115200);
    xTaskCreate(ARGBTask, "ARGB Task", 1024, NULL, 1, &argbTaskHandle);
    xTaskCreate(MicTask, "Mic Task", 1024, NULL, 1, &micTaskHandle);
}

void loop()
{
    // Not used when using FreeRTOS, but required by Arduino
    vTaskDelay(pdMS_TO_TICKS(1));
}

void setup1()
{
    delay(3000);
    flash.init(FLASH_CS_PIN);
    loadFavorites();
    xTaskCreate(IRTask, "IR Task", 1024, NULL, 1, &irTaskHandle);
}

void loop1()
{
    // Not used when using FreeRTOS, but required by Arduino
    vTaskDelay(pdMS_TO_TICKS(1));
}

void loadFavorites()
{
    favoriteAnimations = std::vector<AnimationState>();
    if (!SerialFlash.exists("favorites.dat"))
    {
        DEBUG_PRINTLN("No favorites file found, creating new one.");
        SerialFlash.createErasable("favorites.dat", 16 * sizeof(AnimationState));
        return;
    }
    DEBUG_PRINTLN("Loading favorites from flash.");
    SerialFlashFile favFile = SerialFlash.open("favorites.dat");
    size_t fileSize = favFile.size();
    size_t numFavorites = fileSize / sizeof(AnimationState);
    DEBUG_PRINT("Number of favorites in file: ");
    DEBUG_PRINTLN(numFavorites);
    AnimationState state;
    for (size_t i = 0; i < numFavorites; i++)
    {
        favFile.read(&state, sizeof(AnimationState));
        if (state.type < ANIMATION_NONE || state.type > ANIMATION_MUSIC)
        {
            DEBUG_PRINT("Invalid favorite at index ");
            DEBUG_PRINT(i);
            DEBUG_PRINT(": Type=");
            DEBUG_PRINTLN(state.type);
            break;
        }
        favoriteAnimations.push_back(state);
        DEBUG_PRINT("Loaded favorite ");
        DEBUG_PRINT(i);
        DEBUG_PRINT(": Type=");
        DEBUG_PRINTLN(state.type);
    }
    favFile.close();
}

void saveFavorites()
{
    DEBUG_PRINTLN("Saving favorites to flash.");
    SerialFlash.remove("favorites.dat");
    SerialFlash.createErasable("favorites.dat", favoriteAnimations.size() * sizeof(AnimationState));
    SerialFlashFile favFile = SerialFlash.open("favorites.dat");
    for (size_t i = 0; i < favoriteAnimations.size(); i++)
    {
        favFile.write(&favoriteAnimations[i], sizeof(AnimationState));
        DEBUG_PRINT("Saved favorite ");
        DEBUG_PRINT(i);
        DEBUG_PRINT(": Type=");
        DEBUG_PRINTLN(favoriteAnimations[i].type);
    }
    favFile.close();
}

void addFavorite(const AnimationState &state)
{
    if (favoriteAnimations.size() >= 16)
    {
        DEBUG_PRINTLN("Cannot add favorite: maximum number of favorites reached.");
        return;
    }
    favoriteAnimations.push_back(state);
    saveFavorites();
    DEBUG_PRINTLN("Favorite added.");
}

void removeFavorite(const AnimationState &state)
{
    if (favoriteAnimations.empty())
    {
        DEBUG_PRINTLN("Cannot remove favorite: no favorites stored.");
        return;
    }
    for (auto it = favoriteAnimations.begin(); it != favoriteAnimations.end(); ++it)
    {
        if (it->type == state.type &&
            it->color == state.color &&
            it->wait == state.wait &&
            it->length == state.length)
        {
            favoriteAnimations.erase(it);
            saveFavorites();
            DEBUG_PRINTLN("Favorite removed.");
            return;
        }
    }
    DEBUG_PRINTLN("Favorite not found.");
}

bool isFavorite(const AnimationState &state)
{
    for (const auto &fav : favoriteAnimations)
    {
        if (fav.type == state.type &&
            fav.color == state.color &&
            fav.wait == state.wait &&
            fav.length == state.length)
        {
            return true;
        }
    }
    return false;
}

void setColor(const CRGB &color)
{
    xSemaphoreTake(stateMutex, portMAX_DELAY);
    animationState.color = color;
    animationState.type = ANIMATION_NONE;
    animationState.step = 0;
    animationState.lastUpdate = millis();
    xSemaphoreGive(stateMutex);
    fill_solid(xleds, LED_COUNT, color);
    FastLED.show();
}

void changeColor(const CRGB &newColor)
{
    xSemaphoreTake(stateMutex, portMAX_DELAY);
    animationState.color = newColor;
    changingColor = false;
    lastColorChangeTime = 0;
    xSemaphoreGive(stateMutex);
}

void wipe(AnimationState &state)
{
    if (state.step >= LED_COUNT / 2)
    {
        xSemaphoreTake(stateMutex, portMAX_DELAY);
        state.type = ANIMATION_NONE;
        state.step = 0;
        state.lastUpdate = millis();
        xSemaphoreGive(stateMutex);
        return;
    }
    unsigned long now = millis();
    if (now - state.lastUpdate >= (unsigned long)state.wait)
    {
        xSemaphoreTake(stateMutex, portMAX_DELAY);
        xleds[state.step] = state.color;
        xleds[LED_COUNT - 1 - state.step] = state.color;
        FastLED.show();
        state.step++;
        state.lastUpdate = now;
        xSemaphoreGive(stateMutex);
    }
}

void rainbow(AnimationState &state)
{
    if (state.step >= 256)
    {
        xSemaphoreTake(stateMutex, portMAX_DELAY);
        state.step = 0;
        state.lastUpdate = millis();
        xSemaphoreGive(stateMutex);
        return;
    }
    unsigned long now = millis();
    if (now - state.lastUpdate >= (unsigned long)state.wait)
    {
        int numMirrored = LED_COUNT / 2;
        int cycles = max(1, state.length);
        for (int i = 0; i < numMirrored; i++)
        {
            uint8_t hue = ((uint32_t)i * 256 * cycles / numMirrored + state.step) & 0xFF;
            xleds[i] = CHSV(hue, 255, 255);
            xleds[LED_COUNT - 1 - i] = CHSV(hue, 255, 255);
        }
        FastLED.show();
        xSemaphoreTake(stateMutex, portMAX_DELAY);
        state.step++;
        state.lastUpdate = now;
        xSemaphoreGive(stateMutex);
    }
}

void meteor(AnimationState &state)
{
    const int meteorSize = max(1, min(LED_COUNT - 1, state.length));
    const float fadeAmount = 0.5;
    unsigned long now = millis();
    if (now - state.lastUpdate >= (unsigned long)state.wait)
    {
        for (int i = 0; i < LED_COUNT; i++)
        {
            xleds[i].nscale8_video((uint8_t)(255 * fadeAmount));
        }

        for (int i = 0; i < meteorSize; i++)
        {
            int pos = (state.step - i + LED_COUNT) % LED_COUNT;
            xleds[pos] = state.color;
        }

        FastLED.show();
        xSemaphoreTake(stateMutex, portMAX_DELAY);
        state.step = (state.step - 1 + LED_COUNT) % LED_COUNT;
        state.lastUpdate = now;
        xSemaphoreGive(stateMutex);
    }
}

void stars(AnimationState &state)
{
    const float fadeAmount = 0.66;
    const int numStars = max(1, min(LED_COUNT - 1, state.length));
    unsigned long now = millis();
    if (now - state.lastUpdate >= (unsigned long)state.wait)
    {
        for (int i = 0; i < LED_COUNT; i++)
        {
            xleds[i].nscale8_video((uint8_t)(255 * fadeAmount));
        }

        for (int i = 0; i < numStars; i++)
        {
            int pos = random(LED_COUNT);
            xleds[pos] = state.color;
        }

        FastLED.show();
        xSemaphoreTake(stateMutex, portMAX_DELAY);
        state.lastUpdate = now;
        xSemaphoreGive(stateMutex);
    }
}

void fire(AnimationState &state)
{
    const float fadeAmount = 0.85;
    unsigned long now = millis();
    if (now - state.lastUpdate >= (unsigned long)state.wait)
    {
        for (int i = 0; i < LED_COUNT; i++)
        {
            xleds[i].nscale8_video((uint8_t)(255 * fadeAmount));
        }

        int numFlames = max(1, min(LED_COUNT - 1, state.length));
        for (int i = 0; i < numFlames; i++)
        {
            int pos = random(LED_COUNT);
            xleds[pos] = state.color;
        }

        FastLED.show();
        xSemaphoreTake(stateMutex, portMAX_DELAY);
        state.lastUpdate = now;
        xSemaphoreGive(stateMutex);
    }
}

void visualizer(const AnimationState &state)
{
    int ledsPerRow = LED_COUNT / 2;
    int noiseFloor = 255;
    int adjustedAudio = max(0, audioInRectified - noiseFloor);
    int maxAudio = 319;
    int volumeLevel = map(adjustedAudio, 0, maxAudio, 0, ledsPerRow);
    volumeLevel = constrain(volumeLevel, 0, ledsPerRow);
    if (volumeLevel < 2)
        volumeLevel = 0;
    if (volumeLevel % 2 != 0)
        volumeLevel--;

    fill_solid(xleds, LED_COUNT, CRGB::Black);

    int center1_left = (ledsPerRow / 2) - 1;
    int center1_right = (ledsPerRow / 2);
    for (int i = 0; i < volumeLevel / 2; i++)
    {
        int left = center1_left - i;
        int right = center1_right + i;
        if (left >= 0)
            xleds[left] = state.color;
        if (right < ledsPerRow)
            xleds[right] = state.color;
    }

    int center2_left = ledsPerRow + (ledsPerRow / 2) - 1;
    int center2_right = ledsPerRow + (ledsPerRow / 2);
    for (int i = 0; i < volumeLevel / 2; i++)
    {
        int left = center2_left - i;
        int right = center2_right + i;
        if (left >= ledsPerRow)
            xleds[left] = state.color;
        if (right < LED_COUNT)
            xleds[right] = state.color;
    }

    FastLED.show();
}

void beatFlash(const AnimationState &state)
{
    static unsigned long beatFlashEnd = 0;
    const int flashDuration = 80;

    if (beatDetected)
    {
        fill_solid(xleds, LED_COUNT, state.color);
        FastLED.show();
        beatFlashEnd = millis() + flashDuration;
    }
    else if (millis() < beatFlashEnd)
    {

        fill_solid(xleds, LED_COUNT, state.color);
        FastLED.show();
    }
    else
    {
        fill_solid(xleds, LED_COUNT, CRGB::Black);
        FastLED.show();
    }
}

void beatPulse(const AnimationState &state)
{
    static unsigned long pulseStart = 0;
    static bool pulsing = false;
    const int pulseDuration = 300;

    if (beatDetected)
    {
        pulseStart = millis();
        pulsing = true;
    }

    if (pulsing)
    {
        unsigned long now = millis();
        unsigned long elapsed = now - pulseStart;
        if (elapsed < pulseDuration)
        {
            float phase = (float)elapsed / pulseDuration * PI;
            uint8_t brightness = 128 + 127 * sin(phase);
            FastLED.setBrightness(brightness);
            fill_solid(xleds, LED_COUNT, state.color);
            FastLED.show();
        }
    }
    else
    {
        fill_solid(xleds, LED_COUNT, CRGB::Black);
        FastLED.show();
    }
}

void beatWave(const AnimationState &state)
{
    static unsigned long waveStart = 0;
    static bool waving = false;
    const int waveDuration = 300;
    int ledsPerRow = LED_COUNT / 2;

    if (beatDetected)
    {
        waveStart = millis();
        waving = true;
    }

    if (waving)
    {
        unsigned long now = millis();
        unsigned long elapsed = now - waveStart;
        float progress = (float)elapsed / waveDuration;
        int maxSpread = ledsPerRow / 2;
        int spread = min(maxSpread, (int)(progress * maxSpread));

        fill_solid(xleds, LED_COUNT, CRGB::Black);

        int center1_left = (ledsPerRow / 2) - 1;
        int center1_right = (ledsPerRow / 2);
        for (int i = 0; i <= spread; i++)
        {
            int left = center1_left - i;
            int right = center1_right + i;
            if (left >= 0)
                xleds[left] = state.color;
            if (right < ledsPerRow)
                xleds[right] = state.color;
        }

        int center2_left = ledsPerRow + (ledsPerRow / 2) - 1;
        int center2_right = ledsPerRow + (ledsPerRow / 2);
        for (int i = 0; i <= spread; i++)
        {
            int left = center2_left - i;
            int right = center2_right + i;
            if (left >= ledsPerRow)
                xleds[left] = state.color;
            if (right < LED_COUNT)
                xleds[right] = state.color;
        }

        FastLED.show();

        if (elapsed >= waveDuration)
            waving = false;
    }
    else
    {
        fill_solid(xleds, LED_COUNT, CRGB::Black);
        FastLED.show();
    }
}

void beatSparkle(const AnimationState &state)
{
    static unsigned long sparkleEnd = 0;
    const int sparkleDuration = 300;
    const int numSparkles = max(1, min(LED_COUNT / 4, state.length));

    if (beatDetected)
    {
        sparkleEnd = millis() + sparkleDuration;
    }

    if (millis() < sparkleEnd)
    {
        fill_solid(xleds, LED_COUNT, CRGB::Black);
        for (int i = 0; i < numSparkles; i++)
        {
            int pos = random(LED_COUNT);
            xleds[pos] = state.color;
        }
        FastLED.show();
    }
    else
    {
        fill_solid(xleds, LED_COUNT, CRGB::Black);
        FastLED.show();
    }
}

void shuffleAll(AnimationState &state)
{
    if (lastShuffleTime == 0 || millis() - lastShuffleTime >= 30000)
    {
        xSemaphoreTake(stateMutex, portMAX_DELAY);
        state = getRandomAnimation();
        if ((state.type == ANIMATION_COLOR_WIPE || state.type == ANIMATION_NONE) && state.color == CRGB::Black)
            state.color = CHSV(random(0, 256), 255, 255);
        lastShuffleTime = millis();
        DEBUG_PRINT("Shuffled to new animation: Type=");
        DEBUG_PRINTLN(state.type);
        xSemaphoreGive(stateMutex);
    }
    switch (state.type)
    {
    case ANIMATION_NONE:
        fill_solid(xleds, LED_COUNT, state.color);
        FastLED.show();
        break;
    case ANIMATION_COLOR_WIPE:
        wipe(state);
        break;
    case ANIMATION_RAINBOW:
        rainbow(state);
        break;
    case ANIMATION_METEOR:
        meteor(state);
        break;
    case ANIMATION_STARS:
        stars(state);
        break;
    case ANIMATION_FIRE:
        fire(state);
        break;
    case ANIMATION_MUSIC:
        switch (musicMode)
        {
        case VISUALIZER:
            visualizer(state);
            break;
        case BEAT_FLASH:
            beatFlash(state);
            break;
        case BEAT_PULSE:
            beatPulse(state);
            break;
        case BEAT_WAVE:
            beatWave(state);
            break;
        case BEAT_SPARKLE:
            beatSparkle(state);
            break;
        default:
            DEBUG_PRINTLN("Invalid music mode in shuffleAll.");
            break;
        }
        break;
    default:
        DEBUG_PRINTLN("Invalid animation type in shuffleAll.");
        break;
    }
}

void shuffleFavorites(AnimationState &state)
{
    if (favoriteAnimations.size() == 0)
    {
        DEBUG_PRINTLN("No favorite animations to shuffle.");
        return;
    }
    if (lastShuffleTime == 0 || millis() - lastShuffleTime >= 30000)
    {
        xSemaphoreTake(stateMutex, portMAX_DELAY);
        state = getRandomFavorite();
        if ((state.type == ANIMATION_COLOR_WIPE || state.type == ANIMATION_NONE) && state.color == CRGB::Black)
            state.color = CHSV(random(0, 256), 255, 255);
        lastShuffleTime = millis();
        DEBUG_PRINT("Shuffled to new favorite animation: Type=");
        DEBUG_PRINTLN(state.type);
        xSemaphoreGive(stateMutex);
    }
    switch (state.type)
    {
    case ANIMATION_NONE:
        fill_solid(xleds, LED_COUNT, state.color);
        FastLED.show();
        break;
    case ANIMATION_COLOR_WIPE:
        wipe(state);
        break;
    case ANIMATION_RAINBOW:
        rainbow(state);
        break;
    case ANIMATION_METEOR:
        meteor(state);
        break;
    case ANIMATION_STARS:
        stars(state);
        break;
    case ANIMATION_FIRE:
        fire(state);
        break;
    case ANIMATION_MUSIC:
        switch (musicMode)
        {
        case VISUALIZER:
            visualizer(state);
            break;
        case BEAT_FLASH:
            beatFlash(state);
            break;
        case BEAT_PULSE:
            beatPulse(state);
            break;
        case BEAT_WAVE:
            beatWave(state);
            break;
        case BEAT_SPARKLE:
            beatSparkle(state);
            break;
        default:
            DEBUG_PRINTLN("Invalid music mode in shuffleFavorites.");
            break;
        }
        break;
    default:
        DEBUG_PRINTLN("Invalid animation type in shuffleFavorites.");
        break;
    }
}

void shuffleMusic(AnimationState &state)
{

    if (lastShuffleTime == 0 || millis() - lastShuffleTime >= 30000)
    {
        xSemaphoreTake(stateMutex, portMAX_DELAY);
        state.type = ANIMATION_MUSIC;
        state.color = CHSV(random(0, 256), 255, 255);
        state.wait = random(25, 201);
        state.length = random(1, (LED_COUNT / 2) - 1);
        state.lastUpdate = millis();
        lastShuffleTime = millis();
        musicMode = getRandomMusicMode();
        DEBUG_PRINT("Shuffled to new music animation: Mode=");
        DEBUG_PRINTLN(musicMode);
        xSemaphoreGive(stateMutex);
    }
    switch (musicMode)
    {
    case VISUALIZER:
        visualizer(state);
        break;
    case BEAT_FLASH:
        beatFlash(state);
        break;
    case BEAT_PULSE:
        beatPulse(state);
        break;
    case BEAT_WAVE:
        beatWave(state);
        break;
    case BEAT_SPARKLE:
        beatSparkle(state);
        break;
    default:
        DEBUG_PRINTLN("Invalid music mode in shuffleMusic.");
        break;
    }
}

AnimationState getRandomAnimation()
{
    AnimationState state;
    int animType = random(ANIMATION_NONE, ANIMATION_MUSIC + 1);
    state.type = static_cast<AnimationType>(animType);
    state.color = CHSV(random(0, 256), 255, 255);
    if (state.type == ANIMATION_NONE && state.color == CRGB::Black)
        state.color = CHSV(random(0, 256), 255, 255);
    state.wait = random(25, 201);
    state.length = random(1, (LED_COUNT / 2) - 1);
    state.lastUpdate = millis();
    return state;
}

AnimationState getRandomFavorite()
{
    if (favoriteAnimations.size() == 0)
    {
        DEBUG_PRINTLN("No favorite animations available.");
        return getRandomAnimation();
    }
    int favIndex = random(0, favoriteAnimations.size());
    return favoriteAnimations[favIndex];
}

MusicMode getRandomMusicMode()
{
    int mode = random(0, 5);
    return static_cast<MusicMode>(mode);
}