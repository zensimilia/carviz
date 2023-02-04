#pragma once

#include <soc/adc_channel.h>

#define FPS 30
#define VOUT_PIN (DAC2)
#define USE_PSRAM 0
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 160

#define ASTEROIDS_QTY 16

// Analog to digital
#define ADC_CHANNEL (ADC1_GPIO34_CHANNEL) // ADC1 ONLY! Default: GPIO34
#define ADC_VREF_PIN (GPIO_NUM_27)        // GPIO 25,26 or 27
#define ADC_USE_VREF 0
#define ADC_THRESHOLD 1000
#define SAMPLING_FREQ 32000
#define SAMPLES 1024

// FFT
#define BANDS 16
#define AMPLITUDE 300
// static int16_t freqTable[BANDS] = {
//     80, 160, 320, 640,                   // LOW 20-200
//     800, 1000, 1500, 2000, 2500, 4000,   // MID 200-1000-5000
//     5000, 5500, 6000, 7500, 10000, 16000 // HIGH 5000-10000-20000
// };

const int16_t freqTable[BANDS] = {
    80, 160, 250, 400,
    650, 1000, 1300, 1600,
    2500, 3000, 4000, 5000,
    6250, 8000, 12000, 20000};
