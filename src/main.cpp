#include <Arduino.h>
#include <arduinoFFT.h>
#include <esp_wifi.h>
#include <settings.h>
#include <lgfx.h>
#include <i2sadc.h>
#include <rocket_img.h>

LGFX gfx; // NTSC, 480x270, 8-bit (RGB332) color
arduinoFFT FFT = arduinoFFT();
TaskHandle_t adcWriterTaskHandle;

#define ASTEROIDS_QTY 32
struct asteroid_t
{
    int16_t x;
    int16_t y;
    uint8_t z;
    uint8_t r;
};
asteroid_t asteroids[ASTEROIDS_QTY];

uint8_t frames = 0;
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

void drawText()
{
    int16_t xCenter = screenW >> 1;
    int16_t yCenter = screenH >> 1;

    gfx.setFont(&fonts::Orbitron_Light_32);
    gfx.setTextDatum(textdatum_t::middle_center);
    gfx.drawString("ASTRO BLACK", xCenter, yCenter);

    if ((long)seconds % 2)
    {
        gfx.setFont(&fonts::FreeMonoBold18pt7b);
        gfx.drawString("PRESS START", xCenter, yCenter + 24);
    }
}

void drawAsteroids()
{
    for (uint8_t i = 0; i < ASTEROIDS_QTY; i++)
    {
        if (asteroids[i].x <= 0)
        {
            asteroids[i].x = random(screenW, screenW << 1);
            asteroids[i].y = random(0, screenH);
        }

        if (asteroids[i].x <= screenW && asteroids[i].x > 0)
            gfx.fillCircle(asteroids[i].x, asteroids[i].y, asteroids[i].r, GREEN);

        asteroids[i].x -= asteroids[i].z;
    }
}

void drawRocket()
{
    uint16_t x = (screenW - 96) >> 1;
    uint16_t y = (screenH - 96) >> 1;

    ry += rdy;
    if (ry > 20)
        rdy = -1;
    if (ry < 5)
        rdy = 1;

    gfx.drawXBitmap(x, y - ry, rocket_img, 96, 54, WHITE);
}

void rocketScreen()
{
    frames++;

    if (frames > 1)
    {
        frames = 0;
        gfx.clearDisplay();
        drawRocket();
        drawText();
    }

    drawAsteroids();
}

void setup()
{
    // Setup Serial
    Serial.begin(115200);
    Serial.println();

    esp_wifi_stop(); // Turn off the WiFi
    btStop();        // Turn off the BT
    // i2sInit(); // Setup I2S
    // xTaskCreatePinnedToCore(adcWriterTask, "ADC Writer Task", 4096, NULL, 1, &adcWriterTaskHandle, 1);

    gfx.setColorDepth(lgfx::color_depth_t::rgb332_1Byte);
    gfx.init();

    screenW = gfx.width();
    screenH = gfx.height();

    gfx.setTextSize((std::max(screenW, screenH) + 255) >> 8);
    gfx.setTextColor(WHITE);

    delay(1000);

    for (uint8_t i = 0; i < ASTEROIDS_QTY; i++)
    {
        asteroids[i].x = random(screenW, screenW << 1);
        asteroids[i].y = random(0, screenH);
        asteroids[i].z = random(1, 3);
        asteroids[i].r = random(0, 3);
    }
}

void loop()
{
    // Get the current time and calculate a scaling factor
    seconds = (float_t)(millis() % 1000) / 1000.0;

    rocketScreen();
}
