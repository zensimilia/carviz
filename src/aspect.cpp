#include "aspect.h"
#include "utils.h"

void ASpect::adcReadTask(void *pvParameters)
{
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
            }
        }

        // Send collected samples to the Queue
        xQueueSend(_xSamplesQueue, (void *)&_vReal, (TickType_t)0);
    }

    adc_power_release();

    vTaskDelete(NULL);
}

void ASpect::fftComputeTask(void *pvParameters)
{
    // Width of each frequency bin in Hz
    const uint16_t df = _samplingFreq / _sampleRate;

    while (_process)
    {
        if (xQueueReceive(_xSamplesQueue, &_vReal, portMAX_DELAY) == pdTRUE)
        {

            memset(_vImag, 0, sizeof(*_vImag) * _sampleRate);

            // Compute FFT
            fft->Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
            fft->Compute(FFT_FORWARD);

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

    vTaskDelete(NULL);
}

// Issue https://stackoverflow.com/questions/45831114/c-freertos-task-invalid-use-of-non-static-member-function
void ASpect::adcReadTaskWrapper(void *_this) { static_cast<ASpect *>(_this)->adcReadTask(NULL); }
void ASpect::fftComputeTaskWrapper(void *_this) { static_cast<ASpect *>(_this)->fftComputeTask(NULL); }

void ASpect::init()
{
    _process = true; // Flag

    // Create the Tasks
    xTaskCreatePinnedToCore(adcReadTaskWrapper, "ADC Read Task", 4096, this, 25, NULL, 0);
    xTaskCreatePinnedToCore(fftComputeTaskWrapper, "FFT Compute Task", 4096, this, 25, NULL, 1);

    // Trigger event that FFT is ready for next computing
    xEventGroupSetBits(_xEventGroup, FFT_READY);
}

void ASpect::begin() { init(); }

// TODO: fix memory leak and freezes
void ASpect::stop() { _process = false; }
