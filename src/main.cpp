#include <Arduino.h>
#include <CEveryNTime.h>
#include <esp_wifi.h>
#include <esp_adc_cal.h>

#include "settings.h"
#include "audio_analyzer.h"
#include "lgfx.h"
#include "rocket_img.h"
#include "screen.h"

Screen cvbs(SCREEN_WIDTH, SCREEN_HEIGHT);
LGFX_Sprite *canvas = cvbs.canvas;

// LGFX display; // NTSC, 240x160, 8-bit (RGB332) color
esp_pm_lock_handle_t powerManagementLock;
esp_adc_cal_characteristics_t adc2_chars;

// Sprites
// static LGFX_Sprite canvas(&display);
LGFX_Sprite rocket(canvas);
LGFX_Sprite header(canvas);

// Struct for asteroids params: position, speed, radius and Sprite
struct asteroid_t
{
    int16_t x;
    int16_t y;
    uint8_t z;
    uint8_t r;
    LGFX_Sprite *sprite;
};

asteroid_t asteroids[ASTEROIDS_QTY];
uint8_t ry = 20;
int8_t rdy = 1;

static uint64_t frames = 0;

/**
 * It draws a text header on the screen
 */
void drawHeader()
{
    uint8_t px = SCREEN_WIDTH >> 1;
    uint8_t py = (SCREEN_HEIGHT >> 1) + 10;
    uint16_t blinkDelayMs = 600;

    header.clear();
    header.setTextSize(0.7);
    header.setFont(&fonts::Orbitron_Light_24);
    header.drawString("ASTRO BLACK", px, 0);

    if ((millis() / blinkDelayMs) % 2)
    {
        header.setTextSize(1);
        header.setFont(&fonts::Font8x8C64);
        header.drawString("PRESS START", px, 24);
    }

    header.pushSprite(0, py, TFT_BLACK);
}

/**
 * For each asteroid, if it's off the screen, put it back on the screen at a random location. If it's
 * on the screen, draw it. Then move it to the left
 */
void drawAsteroids()
{
    asteroid_t *a;
    uint32_t *bandBins = getSpectrumBins();

    for (uint8_t i = 0; i < ASTEROIDS_QTY; i++)
    {
        a = &asteroids[i];
        a->x -= a->z;
        a->r = 3.0 / 100 * bandBins[i];

        if (a->x < -10)
        {
            a->x = rand() % SCREEN_WIDTH + SCREEN_WIDTH;
            a->y = rand() % SCREEN_HEIGHT;
        }

        if (a->x <= SCREEN_WIDTH)
        {
            a->sprite->clear();
            a->sprite->fillCircle(5, 5, a->r, TFT_WHITE);
            a->sprite->pushSprite(canvas, a->x, a->y, TFT_BLACK);
        }
    }
}

/**
 * The rocket sprite is moved up and down by a small amount, and then drawn to the screen
 */
void drawRocket()
{
    uint8_t x = (SCREEN_WIDTH - 96) >> 1;
    uint8_t y = (SCREEN_HEIGHT - 96) >> 1;

    ry += rdy;
    if (ry > 20)
        rdy = -1;
    if (ry < 5)
        rdy = 1;

    rocket.pushSprite(x, y - ry, TFT_BLACK);
}

/**
 * It draws a rocket on the screen, and then draws a bunch of asteroids around it, then header
 * text next to rocket
 */
void rocketScreen()
{
    asteroid_t *a;

    rocket.setColorDepth(lgfx::palette_1bit);
    rocket.createSprite(96, 56);
    rocket.fillScreen(TFT_BLACK);
    rocket.drawBitmap(0, 1, rocket_img, 96, 54, TFT_WHITE);

    header.setColorDepth(lgfx::palette_1bit);
    header.createSprite(SCREEN_WIDTH, SCREEN_HEIGHT >> 1);
    header.fillScreen(TFT_BLACK);
    header.setTextColor(TFT_WHITE);
    header.setTextDatum(textdatum_t::top_center);

    for (uint8_t i = 0; i < ASTEROIDS_QTY; i++)
    {
        a = &asteroids[i];

        a->x = rand() % SCREEN_WIDTH;
        a->y = rand() % SCREEN_HEIGHT;
        a->z = rand() % 3 + 1;
        a->r = rand() % 4;
        a->sprite = new LGFX_Sprite(&rocket);
        a->sprite->setColorDepth(lgfx::palette_1bit);
        a->sprite->createSprite(10, 10);
        a->sprite->fillScreen(TFT_BLACK);
        a->sprite->fillCircle(5, 5, a->r, TFT_WHITE);
    }

    while (true)
    {
        if (millis() - frames >= 1000 / FPS)
        {
            frames = millis();

            canvas->clear();

            drawAsteroids();
            drawRocket();
            drawHeader();

            canvas->pushSprite(0, 0);
        }
    }

    rocket.deleteSprite();
    header.deleteSprite();
    for (uint8_t i = 0; i < ASTEROIDS_QTY; i++)
    {
        a = &asteroids[i];
        a->sprite->deleteSprite();
    }
}

/**
 * It creates two sprites, one for the spectrum and one for the VU meter. It then loops through the
 * array of band bins and draws a rectangle for each band bin. It then draws a line between each band
 * bin. It then draws the VU meter and pushes the sprites to the canvas
 */
void spectrumScreen()
{
    static LGFX_Sprite spectrum(canvas);
    static LGFX_Sprite vu(canvas);
    static CEveryNMillis avgVUrefreshTime(100);

    vu.setColorDepth(lgfx::rgb332_1Byte);
    vu.createSprite(208, 20);
    vu.fillScreen(TFT_BLACK);
    vu.setTextColor(TFT_WHITE, TFT_BLACK);

    spectrum.setColorDepth(lgfx::rgb332_1Byte);
    spectrum.createSprite(208, 100);
    spectrum.fillScreen(TFT_BLACK);

    uint8_t prevBands[BANDS] = {0};
    uint16_t w = spectrum.width();
    uint16_t h = spectrum.height();
    uint16_t bw = (w / BANDS);
    uint32_t *bandBins = getSpectrumBins();
    uint16_t *avgVU = getAvgVU();
    uint8_t bandHeight = 0;

    while (true)
    {

        if (millis() - frames >= 1000 / FPS)
        {
            frames = millis();
            spectrum.clear(TFT_BLACK);

            if (avgVUrefreshTime)
            {
                vu.clear();
                vu.setCursor(0, 0);
                vu.printf("VU:%3u", *avgVU);
                vu.drawCenterString("F:20Hz-16kHz", vu.width() >> 1, 0);

                if (*avgVU > 100)
                {
                    vu.fillCircle(44, 3, 3, TFT_RED);
                }
            }

            for (uint8_t i = 0; i < BANDS; i++)
            {
                uint16_t x = i * bw;

                bandHeight = (prevBands[i] + bandBins[i]) / 2;
                spectrum.fillRect(x, 100, bw, constrain(-bandHeight, -50, 0), TFT_GREENYELLOW);
                prevBands[i] = bandHeight;

                if (bandHeight > 50)
                {
                    spectrum.fillRect(x, 50, bw, 50 - bandHeight, TFT_YELLOW);
                }

                if (bandHeight > 80)
                {
                    spectrum.fillRect(x, 20, bw, 80 - bandHeight, TFT_RED);
                }

                spectrum.drawFastVLine(x - 1, 0, h, TFT_BLACK);
            }

            for (uint8_t i = 0; i < 20; i++)
            {
                spectrum.drawFastHLine(0, i * 5, w, TFT_BLACK);
            }

            canvas->pushSprite(0, 0);
            vu.pushSprite(16, 140);
            spectrum.pushSprite(16, 30);
        }
    }

    frames = 0;
    spectrum.deleteSprite();
    vu.deleteSprite();
}

/**
 * The function is called once when the MCU starts. It sets up the serial port, turns off
 * the WiFi and Bluetooth, and initializes the display
 */
void setup()
{
    // Set highest CPU clockspeed
    esp_pm_lock_create(ESP_PM_CPU_FREQ_MAX, 0, "PowerManagementLock", &powerManagementLock);
    esp_pm_lock_acquire(powerManagementLock);
    setCpuFrequencyMhz(240);

    // Setup Serial
    Serial.begin(115200);
    Serial.println();

    // Turn off the BT and Wi-Fi
    btStop();
    esp_wifi_stop();

    // ADC calibration
    esp_adc_cal_characterize(ADC_UNIT_2, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_11, 0, &adc2_chars);
    adc2_config_channel_atten(ADC2_CHANNEL_4, ADC_ATTEN_11db);

    beginAnalyzerTasks();

    // Setup CVBS display
    cvbs.initDisplay();
    // display.init();
    // display.startWrite();

    // Create main canvas
    // canvas.setColorDepth(lgfx::rgb332_1Byte);
    // canvas.createSprite(SCREEN_WIDTH, SCREEN_HEIGHT);
    // canvas.fillScreen(TFT_BLACK);
    // canvas.pushSprite(0, 0);

    delay(500); // Wait for initialization to complete?
}

/**
 * The function is called repeatedly by the Arduino
 */
void loop()
{
    cvbs.draw();
    // spectrumScreen();
    // rocketScreen();
}
