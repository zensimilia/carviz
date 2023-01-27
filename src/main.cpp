#include <Arduino.h>
#include <arduinoFFT.h>
#include <esp_wifi.h>
#include <settings.h>
#include <lgfx.h>
#include <i2sadc.h>
#include <rocket_img.h>

LGFX display; // NTSC, 480x270 / 240x160, 8-bit (RGB332) color
arduinoFFT FFT = arduinoFFT();
TaskHandle_t adcTaskHandle;

// Sprites
static LGFX_Sprite rocket(&display);
static LGFX_Sprite header(&display);

#define ASTEROIDS_QTY 16
struct asteroid_t
{
    int16_t x;
    int16_t y;
    uint8_t z;
    uint8_t r;
    LGFX_Sprite sprite;
};
asteroid_t asteroids[ASTEROIDS_QTY];

uintmax_t frames = 0;
uint8_t ry = 20;
int8_t rdy = 1;
float_t seconds;
uint16_t screenW;
uint16_t screenH;

int bucketFreq(int i)
{
    if (i <= 1)
        return 0;
    return (i - 2) * (SAMPLING_FREQ >> 1) / REAL_SAMPLES;
}

void adcWriterTask(void *param)
{
    while (true)
    {
        double_t totalVU = 0.0;
        uint16_t maxBin = 0;

        adcRead((int16_t *)vReal, SAMPLES);
        memset(&vImag, 0, sizeof(double)); // Fill the vImag with zeroes (quick way)

        // Compute FFT
        FFT.DCRemoval();
        FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
        FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
        FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
        FFT.MajorPeak(vReal, SAMPLES, SAMPLING_FREQ);

        // Fill spectrum bins
        for (int i = 2; i < REAL_SAMPLES; i++)
        {
            totalVU += vReal[i];
            int freq = bucketFreq(i);
            int n = 0;
            while (n < BANDS)
            {
                if (freq < bandFreqTable[n])
                    break;
                n++;
            }
            if (n > BANDS)
                n = BANDS;
            bandBins[n] += (int)vReal[i];
            maxBin = max(bandBins[n], maxBin);
        }

        for (int i = 0; i < BANDS; i++)
        {
            bandBins[i] = map(bandBins[i], 0, maxBin, 0, 100);
        }

        avgVU = totalVU / REAL_SAMPLES;
    }
}

void drawHeader()
{
    uint8_t centerX = screenW >> 1;
    uint8_t centerY = screenH >> 1;

    header.clear();
    header.setTextSize(0.8);
    header.setFont(&fonts::Orbitron_Light_24);
    header.drawString("ASTRO BLACK", centerX, 0);

    if ((millis() / 1000) % 2)
    {
        header.setTextSize(1);
        header.setFont(&fonts::Font8x8C64);
        header.drawString("PRESS START", centerX, 24);
    }

    header.pushSprite(0, centerY, TFT_BLACK);
}

void drawAsteroids()
{
    for (uint8_t i = 0; i < ASTEROIDS_QTY; i++)
    {
        asteroids[i].sprite.clear();

        if (asteroids[i].x <= 0)
        {
            asteroids[i].x = random(screenW, screenW << 1);
            asteroids[i].y = random(0, screenH);
        }

        if (asteroids[i].x <= screenW && asteroids[i].x > 0)
            asteroids[i].sprite.pushSprite(&display, asteroids[i].x, asteroids[i].y, TFT_BLACK);

        asteroids[i].x -= asteroids[i].z;
    }
}

void drawRocket()
{
    uint8_t x = (screenW - 96) >> 1;
    uint8_t y = (screenH - 96) >> 1;

    ry += rdy;
    if (ry > 20)
        rdy = -1;
    if (ry < 5)
        rdy = 1;

    rocket.pushSprite(x, y - ry, TFT_BLACK);
}

void rocketScreen()
{
    rocket.setColorDepth(lgfx::palette_1bit);
    rocket.createSprite(96, 56);
    rocket.fillScreen(TFT_BLACK);
    rocket.drawBitmap(0, 1, rocket_img, 96, 54, TFT_WHITE);

    header.setColorDepth(lgfx::palette_1bit);
    header.createSprite(screenW, screenH >> 1);
    header.fillScreen(TFT_BLACK);
    header.setTextColor(TFT_WHITE);
    header.setTextDatum(textdatum_t::top_center);

    for (uint8_t i = 0; i < ASTEROIDS_QTY; i++)
    {
        asteroids[i].x = random(0, screenW << 1);
        asteroids[i].y = random(0, screenH);
        asteroids[i].z = random(1, 4);
        asteroids[i].r = random(0, 4);
        asteroids[i].sprite.setColorDepth(lgfx::palette_1bit);
        asteroids[i].sprite.createSprite(10, 10);
        asteroids[i].sprite.fillScreen(TFT_BLACK);
        asteroids[i].sprite.fillCircle(5, 5, asteroids[i].r, TFT_WHITE);
    }

    while (true)
    {

        frames++;

        if (frames > 5)
        {
            frames = 0;

            display.clear();
            display.startWrite();

            drawAsteroids();
            drawRocket();
            drawHeader();

            display.endWrite();
        }

        display.display();
    }

    frames = 0;
    rocket.deleteSprite();
    header.deleteSprite();
    for (uint8_t i = 0; i < ASTEROIDS_QTY; i++)
    {
        asteroids[i].sprite.deleteSprite();
    }
}

void setup()
{
    // Setup Serial
    Serial.begin(115200);
    Serial.println();

    esp_wifi_stop(); // Turn off the WiFi
    btStop();        // Turn off the BT
    // i2sInit(); // Setup I2S
    // xTaskCreatePinnedToCore(adcWriterTask, "ADC Writer Task", 4096, NULL, 1, &adcTaskHandle, 1);

    display.setColorDepth(lgfx::rgb332_1Byte);
    display.init();

    screenW = display.width();
    screenH = display.height();

    delay(1000);
}

void loop()
{
    rocketScreen();
}
