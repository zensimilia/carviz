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

#define ASTEROIDS 32

short int asteroidsX[ASTEROIDS];
short int asteroidsY[ASTEROIDS];
short int asteroidsZ[ASTEROIDS];
short int asteroidsS[ASTEROIDS];

unsigned char frames = 0;
unsigned char ry = 20;
signed char rdy = 1;
float seconds;

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
    int xCenter = gfx.width() >> 1;
    int yCenter = gfx.height() >> 1;

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
    for (int i = 0; i < ASTEROIDS; i++)
    {
        if (asteroidsX[i] <= 0)
        {
            asteroidsX[i] = random(480, 480 << 1);
            asteroidsY[i] = random(0, 270);
        }

        if (asteroidsX[i] <= 480 && asteroidsX[i] > 0)
            gfx.fillCircle(asteroidsX[i], asteroidsY[i], asteroidsS[i], WHITE);

        asteroidsX[i] -= asteroidsZ[i];
    }
}

void drawRocket()
{
    int x = (gfx.width() - 72) >> 1;
    int y = (gfx.height() - 40) >> 1;

    ry += rdy;
    if (ry > 20)
        rdy = -1;
    if (ry < 5)
        rdy = 1;

    gfx.drawXBitmap(x, y - ry, rocket_img, 72, 40, WHITE);
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

    for (int i = 0; i < ASTEROIDS; i++)
    {
        asteroidsX[i] = random(480, 480 << 1);
        asteroidsY[i] = random(0, 270);
        asteroidsZ[i] = random(1, 3);
        asteroidsS[i] = random(0, 3);
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
    seconds = (float)(millis() % 1000) / 1000.0;

    rocketScreen();
}
