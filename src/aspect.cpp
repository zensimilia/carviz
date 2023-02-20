#include "aspect.h"

/**
 * It waits for the FFT to be ready, then collects samples and sends them to the Queue
 *
 * @param pvParameters A parameter to pass to the task.
 */
void ASpect::adcReadTask(void *pvParameters)
{
    process = true;
    adc_power_acquire();

    while (process)
    {
        int32_t val;
        uint64_t ts;

        // Waiting for the FFT to be ready
        xEventGroupWaitBits(xEventGroup, FFT_READY, pdTRUE, pdTRUE, portMAX_DELAY);

        // Collect samples
        for (uint16_t i = 0; i < sampleRate; i++)
        {
            ts = micros();
            adc2_get_raw(ADC2_CHANNEL_4, ADC_WIDTH_11Bit, &val);
            vReal[i] = val;
            while ((micros() - ts) < samplingPeriodUs)
            {
                taskYIELD();
            }
        }

        // Send collected samples to the Queue
        xQueueSend(xSamplesQueue, (void *)&vReal, (TickType_t)0);
    }

    adc_power_release();

    vTaskDelete(NULL);
}

/**
 * The function takes the samples from the ADC, computes the FFT, and then computes the average VU
 * level
 *
 * @param pvParameters A pointer to the parameters passed to the task when the task was created.
 */
void ASpect::fftComputeTask(void *pvParameters)
{
    // Width of each frequency bin in Hz
    const uint16_t df = samplingFreq / sampleRate;
    process = true;

    while (process)
    {
        if (xQueueReceive(xSamplesQueue, &vReal, portMAX_DELAY) == pdPASS)
        {
            memset(vImag, 0, sizeof(*vImag) * sampleRate);

            // Compute FFT
            fft->Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
            fft->Compute(FFT_FORWARD);

            // Fill spectrum bins
            for (uint16_t i = 2; i < (sampleRate >> 1); i++)
            {
                if (vReal[i] > ADC_THRESHOLD)
                {
                    uint8_t bin = 0;
                    uint16_t freq = (i - 2) * df;

                    while (bin < BANDS)
                    {
                        if (freq < freqTable[bin])
                            break;
                        bin++;
                    }

                    if (bin >= BANDS)
                        break;

                    // Complex to magnitude
                    bandBins[bin] += int_sqrt(vReal[i] * vReal[i] + vImag[i] * vImag[i]);
                }
            }

            // Normalize spectrum bins
            for (uint8_t i = 0; i < BANDS; i++)
            {
                avgVU += bandBins[i];

                bandBins[i] /= AMPLITUDE;
                bandBins[i] = constrain(bandBins[i], 0, 100);
            }

            uint16_t t = avgVU / (sampleRate >> 1);
            avgVU = oldVU = max(t, (uint16_t)((oldVU * 3 + t) / 4)); // Smooth

            // Trigger event that FFT is ready for next computing
            xEventGroupSetBits(xEventGroup, FFT_READY);
        }
    }

    delete fft;
    delete[] vReal;
    delete[] vImag;

    avgVU = 0;
    oldVU = 0;
    memset(&bandBins, 0, sizeof(bandBins));

    vTaskDelete(NULL);
}

void ASpect::adcReadTaskWrapper(void *_this) { static_cast<ASpect *>(_this)->adcReadTask(NULL); }       // RTOS xTaskCreate issue
void ASpect::fftComputeTaskWrapper(void *_this) { static_cast<ASpect *>(_this)->fftComputeTask(NULL); } // RTOS xTaskCreate issue

/**
 * The function creates two tasks, one for reading the ADC and one for computing the FFT. The ADC task
 * is pinned to core 0 and the FFT task is pinned to core 1. The ADC task is triggered by the ADC
 * interrupt and the FFT task is triggered by the FFT_READY event
 */
void ASpect::init()
{
    if (!process)
    {
        process = true; // Flag

        vReal = new double_t[sampleRate]();
        vImag = new double_t[sampleRate]();

        fft = new arduinoFFT(vReal, vImag, sampleRate, samplingFreq);

        // Create the Tasks
        xTaskCreatePinnedToCore(adcReadTaskWrapper, "ADC Read Task", 4096, this, 1, NULL, 0);
        xTaskCreatePinnedToCore(fftComputeTaskWrapper, "FFT Compute Task", 4096, this, 0, NULL, 0); // Proirity needs to be zero

        // Trigger event that FFT is ready for next computing
        xEventGroupSetBits(xEventGroup, FFT_READY);
    }
}

/**
 * Alias for `init()` member function
 */
void ASpect::begin() { init(); }

/**
 * Stops analyzer and clean memory
 */
void ASpect::stop() { process = false; }
