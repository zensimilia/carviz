#include <Arduino.h>
#include <ESP_8_BIT_GFX.h>
#include <arduinoFFT.h>
#include <esp_wifi.h>
#include <settings.h>
#include <i2sadc.h>
#include <Fonts/FreeSerif9pt7b.h>
#include "rocket_img.h"

// NTSC, 240x256, 8-bit (RGB332) color
ESP_8_BIT_GFX videoOut(true, 8);
arduinoFFT FFT = arduinoFFT();
TaskHandle_t adcWriterTaskHandle;

#define ASTEROIDS 32

short int asteroidsX[ASTEROIDS];
short int asteroidsY[ASTEROIDS];
short int asteroidsZ[ASTEROIDS];
short int asteroidsS[ASTEROIDS];

unsigned char frames = 0;
unsigned char rx = 60;
unsigned char ry = 20;
signed char rdy = 1;

int bucketFreq(int i)
{
    if (i <= 1)
        return 0;
    return (i - 2) * (SAMPLING_FREQ / 2) / REAL_SAMPLES;
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
    videoOut.setFont(&FreeSerif9pt7b);
    // TV.print(20, 60, "ASTRO BLACK");
    videoOut.setCursor(50, 160);
    videoOut.setTextColor(WHITE);
    videoOut.println("ASTRO BLACK");
    // if (millis() / 1000 % 2)
    // {
    // TV.select_font(font4x6);
    // TV.print(42, 70, "PRESS START");
    // }
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
            videoOut.fillCircle(asteroidsX[i], asteroidsY[i], asteroidsS[i], WHITE);

        asteroidsX[i] -= asteroidsZ[i];
    }
}

void drawRocket()
{
    ry += rdy;
    if (ry > 20)
        rdy = -1;
    if (ry < 5)
        rdy = 1;
    videoOut.drawBitmap(rx, ry, rocket_img, 72, 40, WHITE);
}

void rocketScreen()
{
    frames++;

    // if (frames > 2)
    // {
    videoOut.fillScreen(BLACK);
    frames = 0;
    drawRocket();
    drawText();
    // }

    drawAsteroids();

    videoOut.waitForFrame();
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

    videoOut.begin();
    Serial.println(3);
}

void loop()
{
    // Get the current time and calculate a scaling factor
    unsigned long time = millis();
    float partial_second = (float)(time % 1000) / 1000.0;

    rocketScreen();
}
