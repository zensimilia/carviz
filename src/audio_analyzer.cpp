#include "audio_analyzer.h"
#include "utils.h"

static double_t vReal[SAMPLES];
static double_t vImag[SAMPLES];
static uint32_t bandBins[BANDS];
static uint16_t avgVU = 0;

// Create arduinoFFT instance
arduinoFFT FFT = arduinoFFT(vReal, vImag, SAMPLES, SAMPLING_FREQ);

// Create Queue and EventGroup
QueueHandle_t xSamplesQueue = xQueueCreate(SAMPLES, sizeof(double_t *));
EventGroupHandle_t xEventGroup = xEventGroupCreate();

/**
 * It waits for the FFT_READY event, then collects the samples and sends them to the Queue
 *
 * @param pvParameters A parameter passed into the task.
 */
void adcReadTask(void *pvParameters)
{
    for (;;)
    {
        // Waiting for the FFT to be ready
        xEventGroupWaitBits(xEventGroup, FFT_READY, pdTRUE, pdTRUE, portMAX_DELAY);

        // Collect samples
        for (uint16_t i = 0; i < SAMPLES; i++)
        {
            vReal[i] = adc1_get_raw(ADC_CHANNEL);
        }

        // Send collected samples to the Queue
        xQueueSend(xSamplesQueue, (void *)&vReal, (TickType_t)0);
    }

    vTaskDelete(NULL);
}

/**
 * It takes the samples from the queue, computes the FFT,
 * and then puts the results into the bandBins array
 *
 * @param pvParameters A parameter passed into the task.
 */
void fftComputeTask(void *pvParameters)
{
    // Width of each frequency bin in Hz
    const uint16_t df = SAMPLING_FREQ / SAMPLES;

    for (;;)
    {
        if (xQueueReceive(xSamplesQueue, &vReal, portMAX_DELAY) == pdTRUE)
        {
            memset((void *)&vImag, 0, sizeof(vImag));

            // Compute FFT
            FFT.DCRemoval();
            FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
            FFT.Compute(FFT_FORWARD);
            FFT.ComplexToMagnitude();

            // Fill spectrum bins
            for (uint16_t i = 1; i < (SAMPLES >> 1); i++)
            {
                if (vReal[i] > ADC_THRESHOLD)
                {
                    uint8_t bin = 0;
                    uint16_t freq = (i - 1) * df;

                    while (bin < BANDS)
                    {
                        if (freq < freqTable[bin])
                            break;
                        bin++;
                    }

                    if (bin >= BANDS)
                        break;

                    bandBins[bin] += isqrt(vReal[i] * vReal[i] + vImag[i] * vImag[i]);
                }
            }

            // Normalize spectrum bins
            for (uint8_t i = 0; i < BANDS; i++)
            {
                bandBins[i] = map(bandBins[i], 0, 200000, 0, 100); // TODO: fix that
                bandBins[i] = constrain(bandBins[i], 0, 100);
                avgVU += bandBins[i];
            }

            avgVU /= BANDS;

            // Trigger event that FFT is ready for next computing
            xEventGroupSetBits(xEventGroup, FFT_READY);
        }
    }

    vTaskDelete(NULL);
}

/**
 * It returns the average VU value
 *
 * @return The average VU value.
 */
uint16_t getAvgVU() { return avgVU; };

/**
 * This function returns a pointer to the array of spectrum data
 *
 * @return The address of the first element.
 */
uint32_t *getSpectrumBins() { return bandBins; }

/**
 * Create Tasks and trigger Event
 */
void beginAnalyzerTasks()
{
    // Create a Tasks
    xTaskCreatePinnedToCore(adcReadTask, "ADC Read Task", 4096, NULL, 25, NULL, 0);
    xTaskCreatePinnedToCore(fftComputeTask, "FFT Compute Task", 4096, NULL, 25, NULL, 1);

    // Trigger event that FFT is ready for next computing
    xEventGroupSetBits(xEventGroup, FFT_READY);
}
