#pragma once

#include <driver/i2s.h>
#include <driver/adc.h>
#include <settings.h>

const i2s_port_t I2S_PORT = I2S_NUM_0;

void i2sInit()
{
    // The I2S config
    const i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN),
        .sample_rate = SAMPLING_FREQ,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_MSB,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 4,
        .dma_buf_len = SAMPLES,
        .use_apll = false,
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0};

    // Configuring the I2S driver and pins
    i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
    i2s_set_adc_mode(ADC_UNIT_1, ADC_INPUT);
    adc1_config_channel_atten(ADC_INPUT, ADC_ATTEN_DB_11);
    i2s_adc_enable(I2S_PORT);
}

int adcRead(int16_t *samples, int count)
{
    // Read from i2s
    size_t bytes_read = 0;
    i2s_read(I2S_PORT, samples, sizeof(int16_t) * count, &bytes_read, portMAX_DELAY);
    uint16_t samples_read = bytes_read / sizeof(int16_t);
    for (uint16_t i = 0; i < samples_read; i++)
    {
        samples[i] = (2048 - (uint16_t(samples[i]) & 0xfff)) * 15;
    }
    return samples_read;
}
