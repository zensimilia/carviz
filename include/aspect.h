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
private:
    uint16_t _sampleRate;
    uint16_t _samplingFreq;

    double_t *_vReal;
    double_t *_vImag;

    bool _process = false;
    const uint8_t _bands = BANDS;
    const uint8_t _samplingPeriodUs = round(1.0 / _samplingFreq * 1000000);

    uint32_t _avgVU = 0;
    uint32_t _oldVU = 0;
    uint32_t _bandBins[BANDS];
    uint16_t _freqTable[BANDS] = BANDS_FREQ_TABLE;

    QueueHandle_t _xSamplesQueue = xQueueCreate(_sampleRate, sizeof(double_t *));
    EventGroupHandle_t _xEventGroup = xEventGroupCreate();

    void adcReadTask(void *pvParameters);
    void fftComputeTask(void *pvParameters);

    static void adcReadTaskWrapper(void *_this);
    static void fftComputeTaskWrapper(void *_this);

public:
    arduinoFFT *fft;

    ASpect(uint16_t samples = 1024, uint16_t freq = 44100) : _sampleRate(samples), _samplingFreq(freq)
    {
        _vReal = new double_t[_sampleRate]();
        _vImag = new double_t[_sampleRate]();

        fft = new arduinoFFT(_vReal, _vImag, _sampleRate, _samplingFreq);
    };
    ASpect(const ASpect &) = delete;
    ~ASpect()
    {
        delete[] _vReal;
        delete[] _vImag;

        delete fft;
    };

    void init();
    void begin();
    void stop();

    uint32_t getAvgVU() const { return _avgVU / 500.0 * 100.0; }; // TODO: make true VU
    uint32_t *getSpectrum() { return _bandBins; };

}; // class ASpect
