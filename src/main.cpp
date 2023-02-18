#include <Arduino.h>
#include <esp_wifi.h>
#include <esp_adc_cal.h>

#include "settings.h"
#include "aspect.h"
#include "lgfx.h"
#include "screens/spectrum.h"
#include "screens/rocket.h"

ASpect analyzer(SAMPLES, SAMPLING_FREQ);
LGFX cvbs(SCREEN_WIDTH, SCREEN_HEIGHT, DAC2); // NTSC, 240x160, 8-bit (RGB332) color

Screens::Spectrum sSpectrum;
Screens::Rocket sRocket;

esp_pm_lock_handle_t powerManagementLock;
esp_adc_cal_characteristics_t adc2_chars;

static uint64_t frames = 0;

/**
 * The function is called once when the MCU starts. It sets up the serial port, turns off
 * the WiFi and Bluetooth, and initializes the display
 */
void setup()
{
    // Setup Serial
    Serial.begin(115200);
    Serial.println();

    // Set highest CPU clockspeed
    esp_pm_lock_create(ESP_PM_CPU_FREQ_MAX, 0, "PowerManagementLock", &powerManagementLock);
    esp_pm_lock_acquire(powerManagementLock);
    setCpuFrequencyMhz(240);

    // Turn off the BT and Wi-Fi
    btStop();
    esp_wifi_stop();

    // ADC calibration
    esp_adc_cal_characterize(ADC_UNIT_2, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_11, 0, &adc2_chars);
    adc2_config_channel_atten(ADC2_CHANNEL_4, ADC_ATTEN_11db);

    // Setup audio spectrum analyzer
    analyzer.begin();

    // Setup CVBS display
    cvbs.begin();

    delay(500); // Wait for initialization to complete?

    // cvbs.drawColorTable();
    // delay(5000);
}

/**
 * The function is called repeatedly by the Arduino
 */
void loop()
{
    if (millis() - frames >= 1000 / FPS)
    {
        frames = millis();
        sSpectrum.draw();
        // sRocket.draw();
    }
}
