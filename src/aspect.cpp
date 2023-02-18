#include "aspect.h"

/**
 * It waits for the FFT to be ready, then collects samples and sends them to the Queue
 *
 * @param pvParameters A parameter to pass to the task.
 */
void ASpect::adcReadTask(void *pvParameters)
{
    _process = true;
    adc_power_acquire();

    while (_process)
    {
        int32_t val;
        uint64_t ts;

        // Waiting for the FFT to be ready
        xEventGroupWaitBits(_xEventGroup, FFT_READY, pdTRUE, pdTRUE, portMAX_DELAY);

        // Collect samples
        for (uint16_t i = 0; i < _sampleRate; i++)
        {
            ts = micros();
            adc2_get_raw(ADC2_CHANNEL_4, ADC_WIDTH_11Bit, &val);
            _vReal[i] = val;
            while ((micros() - ts) < _samplingPeriodUs)
            {
                taskYIELD();
            }
        }

        // Send collected samples to the Queue
        xQueueSend(_xSamplesQueue, (void *)&_vReal, (TickType_t)0);
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
    const uint16_t df = _samplingFreq / _sampleRate;
    _process = true;

    while (_process)
    {
        if (xQueueReceive(_xSamplesQueue, &_vReal, portMAX_DELAY) == pdPASS)
        {
            memset(_vImag, 0, sizeof(*_vImag) * _sampleRate);

            // Compute FFT
            _fft->Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
            _fft->Compute(FFT_FORWARD);

            // Fill spectrum bins
            for (uint16_t i = 2; i < (_sampleRate >> 1); i++)
            {
                if (_vReal[i] > ADC_THRESHOLD)
                {
                    uint8_t bin = 0;
                    uint16_t freq = (i - 2) * df;

                    while (bin < _bands)
                    {
                        if (freq < _freqTable[bin])
                            break;
                        bin++;
                    }

                    if (bin >= _bands)
                        break;

                    // Complex to magnitude
                    _bandBins[bin] += int_sqrt(_vReal[i] * _vReal[i] + _vImag[i] * _vImag[i]);
                }
            }

            // Normalize spectrum bins
            for (uint8_t i = 0; i < _bands; i++)
            {
                _avgVU += _bandBins[i];

                _bandBins[i] /= AMPLITUDE;
                _bandBins[i] = constrain(_bandBins[i], 0, 100);
            }

            uint16_t t = _avgVU / (_sampleRate >> 1);
            _avgVU = _oldVU = max(t, (uint16_t)((_oldVU * 3 + t) / 4));

            // Trigger event that FFT is ready for next computing
            xEventGroupSetBits(_xEventGroup, FFT_READY);
        }
    }

    delete _fft;
    delete[] _vReal;
    delete[] _vImag;

    _avgVU = 0;
    _oldVU = 0;
    memset(&_bandBins, 0, sizeof(_bandBins));

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
    if (!_process)
    {
        _process = true; // Flag

        _vReal = new double_t[_sampleRate]();
        _vImag = new double_t[_sampleRate]();

        _fft = new arduinoFFT(_vReal, _vImag, _sampleRate, _samplingFreq);

        // Create the Tasks
        xTaskCreatePinnedToCore(adcReadTaskWrapper, "ADC Read Task", 4096, this, 1, NULL, 0);
        xTaskCreatePinnedToCore(fftComputeTaskWrapper, "FFT Compute Task", 4096, this, 0, NULL, 0); // Proirity needs to be zero

        // Trigger event that FFT is ready for next computing
        xEventGroupSetBits(_xEventGroup, FFT_READY);
    }
}

/**
 * Alias for `init()` member function
 */
void ASpect::begin() { init(); }

/**
 * Stops analyzer and clean memory
 */
void ASpect::stop() { _process = false; }
