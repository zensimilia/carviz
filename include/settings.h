#pragma once

#define FPS 30

// Analog to digital
#define ADC_PIN (A6)       // GPIO36
#define ADC_VREF_PIN (A17) // GPIO 25,26 or 27
#define ADC_USE_VREF 0
#define SAMPLING_FREQ 25600
#define SAMPLES 512

// FFT
#define BANDS 16
static int16_t freqTable[BANDS] = {
    80, 200, 600, 1000,                             // LOW 20-200
    1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000, // MID 200-1000-5000
    6000, 8000, 10000, 16000                        // HIGH 5000-10000-20000
};
int32_t bandBins[BANDS] = {0};
double_t vReal[SAMPLES];
double_t vImag[SAMPLES];
double_t avgVU = 0.0;
