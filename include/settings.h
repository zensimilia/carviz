#pragma once

#define FPS 30

// Quick colors
#define BLACK 0x00
#define WHITE 0xFF
#define GREY 0xB6
#define RED 0xE0
#define GREEN 0x1C
#define BLUE 0x03

// Invert color macros
#define INVERT(color) (0xFF - color)

// Analog to digital
#define SAMPLING_FREQ 44100
#define SAMPLES 1024
#define REAL_SAMPLES (SAMPLES >> 1)
#define ADC_INPUT ADC1_CHANNEL_0 // GPIO36

// FFT
#define BANDS 16
static int bandFreqTable[BANDS] = {50, 80, 100, 150, 250, 400, 650, 800, 1250, 2500, 4000, 6250, 12000, 14000, 16000, 20000};
unsigned int bandBins[BANDS] = {0};
double vReal[SAMPLES];
double vImag[SAMPLES];
volatile double avgVU = 0.0;
