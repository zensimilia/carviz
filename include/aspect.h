#pragma once

#include <driver/adc_common.h>
#include <arduinoFFT.h>

#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
#include "utils.h"

#define FFT_READY (1 << 0) // Event bits
#define AMPLITUDE 256
#define ADC_THRESHOLD 2048
#define BANDS 16
#define BANDS_FREQ_TABLE                                                                           \
    {                                                                                              \
        80, 160, 320, 640, 800, 1000, 1500, 2000, 2500, 4000, 5000, 5500, 6000, 7500, 10000, 20000 \
    } // LOW 20-200, MID 200-1000-5000, HIGH 5000-10000-20000

/**
 * A class that performs a Fast Fourier Transform (FFT) of an audio signal
 * from an analog input and returns its spectrum in 16 bands. It can also
 * return the average VU of the signal. Uses two processor cores for
 * the necessary tasks.
 */
class ASpect
{
    arduinoFFT *fft;

    uint16_t sampleRate;
    uint16_t samplingFreq;

    double_t *vReal;
    double_t *vImag;

    bool process = false;
    const uint8_t samplingPeriodUs = round(1.0 / samplingFreq * 1000000);

    uint32_t avgVU = 0;
    uint32_t oldVU = 0;
    uint32_t bandBins[BANDS] = {0};
    uint16_t freqTable[BANDS] = BANDS_FREQ_TABLE;

    QueueHandle_t xSamplesQueue = xQueueCreate(sampleRate, sizeof(double_t *));
    EventGroupHandle_t xEventGroup = xEventGroupCreate();

    void adcReadTask(void *pvParameters);
    void fftComputeTask(void *pvParameters);

    static void adcReadTaskWrapper(void *_this);
    static void fftComputeTaskWrapper(void *_this);

public:
    ASpect(uint16_t samples = 1024, uint16_t freq = 44100) : sampleRate(samples), samplingFreq(freq){};
    ASpect(const ASpect &) = delete;
    ~ASpect() { stop(); };

    void init();
    void begin();
    void stop();

    uint32_t getAvgVU() const { return avgVU / 500.0 * 100.0; }; // TODO: make true VU
    uint32_t *getSpectrum() { return bandBins; };
}; // class ASpect
