#pragma once

#define FPS 30

// Analog to digital
#define ADC_PIN 36 // GPIO36
#define SAMPLING_FREQ 32000
#define SAMPLES 128

// FFT
#define BANDS 16
// LOW 20-200 | MID 200-1000-5000 | HIGH 5000-10000-20000
static int16_t freqTable[BANDS] = {
    200, 600, 1000,                                 // LOW
    1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000, // MID
    6000, 8000, 10000, 12000, 20000                 // HIGH
};
int32_t bandBins[BANDS] = {0};
double_t vReal[SAMPLES];
double_t vImag[SAMPLES];
volatile double_t avgVU = 0.0;
