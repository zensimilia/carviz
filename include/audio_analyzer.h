#pragma once

#include <arduinoFFT.h>
#include <driver/adc_common.h>
#include <settings.h>

#define FFT_READY (1 << 0)

void adcReadTask(void *pvParameters);
void fftComputeTask(void *pvParameters);
void beginAnalyzerTasks();

uint16_t getAvgVU();
uint32_t *getSpectrumBins();
