#include <Arduino.h>
#include <arduinoFFT.h>
#include <esp_wifi.h>
#include <settings.h>
#include <lgfx.h>
#include <i2sadc.h>
#include "rocket_img.h"

LGFX gfx; // NTSC, 480x270, 8-bit (RGB332) color
arduinoFFT FFT = arduinoFFT();
TaskHandle_t adcWriterTaskHandle;

#define ASTEROIDS 32

short int asteroidsX[ASTEROIDS];
short int asteroidsY[ASTEROIDS];
short int asteroidsZ[ASTEROIDS];
short int asteroidsS[ASTEROIDS];

unsigned char frames = 0;
unsigned char ry = 20;
signed char rdy = 1;

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
        double totalVU = 0.0;
        unsigned int maxBin = 0;

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
    gfx.setFont(&fonts::FreeMonoBold24pt7b);
    gfx.setTextDatum(textdatum_t::middle_center);

    gfx.drawString("ASTRO BLACK", gfx.width() >> 1, gfx.height() >> 1);
    if (millis() / 1000 % 2)
    {
        gfx.setTextSize(3);
        gfx.drawString("PRESS START", 50, 190);
    }
}

void drawAsteroids()
{
    for (int i = 0; i < ASTEROIDS; i++)
    {
        if (asteroidsX[i] <= 0)
        {
            asteroidsX[i] = random(240, 340);
            asteroidsY[i] = random(1, 255);
            asteroidsZ[i] = random(1, 4);
        }

        if (asteroidsX[i] <= 240 && asteroidsX[i] > 0)
            gfx.fillCircle(asteroidsX[i], asteroidsY[i], asteroidsS[i], WHITE);

        asteroidsX[i] -= asteroidsZ[i];
    }
}

void drawRocket()
{
    unsigned char rx = (gfx.width() - 72) >> 1;
    unsigned char y = (gfx.height() - 40) >> 1;
    ry += rdy;
    if (ry > 20)
        rdy = -1;
    if (ry < 5)
        rdy = 1;
    gfx.drawBitmap(rx, y - ry, rocket_img, 72, 40, WHITE);
}

void rocketScreen()
{
    frames++;

    // if (frames > 2)
    // {
    gfx.clearDisplay();
    frames = 0;
    drawRocket();
    drawText();
    // }

    drawAsteroids();

    // gfx.waitDisplay();
}

void setup()
{
    Serial.begin(115200);
    esp_wifi_stop(); // Turn off the WiFi
    btStop();        // Turn off the BT
    // i2sInit(); // Setup I2S
    // xTaskCreatePinnedToCore(adcWriterTask, "ADC Writer Task", 4096, NULL, 1, &adcWriterTaskHandle, 1);

    for (int i = 0; i < ASTEROIDS; i++)
    {
        asteroidsX[i] = random(1, 239);
        asteroidsY[i] = random(1, 95);
        asteroidsZ[i] = random(1, 3);
        asteroidsS[i] = random(0, 2);
        ;
    }

    gfx.setColorDepth(lgfx::color_depth_t::rgb332_1Byte);
    gfx.init();
    gfx.setTextSize((std::max(gfx.width(), gfx.height()) + 255) >> 8);
    gfx.setTextColor(WHITE);

    delay(1000);
}

void loop()
{
    // Get the current time and calculate a scaling factor
    unsigned long time = millis();
    float partial_second = (float)(time % 1000) / 1000.0;

    rocketScreen();
}
