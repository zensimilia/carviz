#pragma once

#define FPS 30
#define VOUT_PIN (DAC2)
#define USE_PSRAM 0
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 160

#define ASTEROIDS_QTY 16

// Analog to digital
#define ADC_PIN (A6)       // GPIO36
#define ADC_VREF_PIN (A17) // GPIO 25,26 or 27
#define ADC_USE_VREF 0
#define ADC_THRESHOLD 10000
#define SAMPLING_FREQ 32000
#define SAMPLES 512

// FFT
#define BANDS 16
#define AMPLITUDE 1
static int16_t freqTable[BANDS] = {
    80, 200, 500, 1000,                             // LOW 20-200
    1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000, // MID 200-1000-5000
    5500, 6000, 10000, 16000                        // HIGH 5000-10000-20000
};
uint32_t bandBins[BANDS];
double_t vReal[SAMPLES];
double_t vImag[SAMPLES];
