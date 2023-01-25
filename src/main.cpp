#include <Arduino.h>
#include <ESP_8_BIT_GFX.h>
#include <arduinoFFT.h>
#include <esp_wifi.h>
#include <settings.h>
#include <i2sadc.h>

// PAL, 8-bit (RGB332) color
ESP_8_BIT_GFX videoOut(false, 8);
arduinoFFT FFT = arduinoFFT();
TaskHandle_t adcWriterTaskHandle;

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

void setup()
{
    Serial.begin(115200);
    esp_wifi_deinit(); // Turn off the WiFi
    btStop();          // Turn off the BT
    i2sInit();         // Setup I2S

    xTaskCreatePinnedToCore(adcWriterTask, "ADC Writer Task", 4096, NULL, 1, &adcWriterTaskHandle, 1);

    videoOut.begin();
}

void loop()
{
    // Get the current time and calculate a scaling factor
    unsigned long time = millis();
    float partial_second = (float)(time % 1000) / 1000.0;

    videoOut.waitForFrame();

    // Clear screen
    videoOut.fillScreen(0);
}
