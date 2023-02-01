#pragma once

#include <arduinoFFT.h>
#include <driver/adc_common.h>
#include <settings.h>

#define FFT_READY (1 << 0)

extern QueueHandle_t xSamplesQueue;
extern EventGroupHandle_t xEventGroup;

void adcReadTask(void *pvParameters);
void fftComputeTask(void *pvParameters);
void beginAnalyzerTasks();

uint16_t getAvgVU();
uint32_t *getSpectrumBins();
