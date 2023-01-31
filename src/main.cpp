#include <Arduino.h>
#include <settings.h>
#include <timers.h>
#include <arduinoFFT.h>
#include <esp_wifi.h>
#include <lgfx.h>
#include <rocket_img.h>

LGFX display; // NTSC, 240x160, 8-bit (RGB332) color
arduinoFFT FFT = arduinoFFT(vReal, vImag, SAMPLES, SAMPLING_FREQ);
TaskHandle_t adcTaskHandle;
esp_pm_lock_handle_t powerManagementLock;

// Sprites
static LGFX_Sprite canvas(&display);
LGFX_Sprite rocket(&canvas);
LGFX_Sprite header(&canvas);

// Struct for asteroids params: position, speed, radius and Sprite
struct asteroid_t
{
    int16_t x;
    int16_t y;
    uint8_t z;
    uint8_t r;
    LGFX_Sprite sprite;
};

asteroid_t asteroids[ASTEROIDS_QTY];
uint8_t ry = 20;
int8_t rdy = 1;

static uint16_t avgVU = 0;
static uint32_t samplingPeriodUs;
static uint64_t frames = 0;

/**
 * It reads the ADC, computes the FFT, and then computes the VU meter and spectrum analyzer values
 *
 * @param param The Task params, can be NULL.
 */
void adcReadTask(void *param)
{
    static uint16_t barHeight = 0;
    static uint16_t freqStep = SAMPLING_FREQ / SAMPLES;
    static uint64_t startUs;

    for (;;)
    {
        memset(&vImag, 0, sizeof(vImag));
        memset(&bandBins, 0, sizeof(bandBins));

        // Collect samples
        for (uint16_t i = 0; i < SAMPLES; i++)
        {
            startUs = micros();
            vReal[i] = analogRead(A6);
            while ((micros() - startUs) < samplingPeriodUs)
            {
            }
        }

        // Compute FFT
        FFT.DCRemoval();
        FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
        FFT.Compute(FFT_FORWARD);
        FFT.ComplexToMagnitude();

        // Fill spectrum bins
        for (uint16_t i = 2; i < (SAMPLES >> 1); i++)
        {
            uint32_t freq = (i - 1) * freqStep;

            if (vReal[i] > ADC_THRESHOLD)
            {
                uint8_t b = 0;
                while (b < BANDS)
                {
                    if (freq < freqTable[b])
                        break;
                    b++;
                }
                if (b > BANDS)
                    b = BANDS;
                bandBins[b] += (int)vReal[i];
            }
        }

        // Normalize spectrum bins
        for (uint8_t i = 0; i < BANDS; i++)
        {
            avgVU += bandBins[i];
            bandBins[i] = map(bandBins[i] / AMPLITUDE, 0, avgVU / BANDS / AMPLITUDE, 0, 100); // TODO: fix that
        }

        avgVU /= (SAMPLES >> 1);
    }
}

/**
 * It draws a text header on the screen
 */
void drawHeader()
{
    uint8_t px = SCREEN_WIDTH >> 1;
    uint8_t py = (SCREEN_HEIGHT >> 1) + 10;

    header.clear();
    header.setTextSize(0.7);
    header.setFont(&fonts::Orbitron_Light_24);
    header.drawString("ASTRO BLACK", px, 0);

    if ((millis() / 1000) % 2)
    {
        header.setTextSize(1);
        header.setFont(&fonts::Font8x8C64);
        header.drawString("PRESS START", px, 24);
    }

    header.pushSprite(0, py, TFT_BLACK);
}

/**
 * For each asteroid, if it's off the screen, put it back on the screen at a random location. If it's
 * on the screen, draw it. Then move it to the left
 */
void drawAsteroids()
{
    asteroid_t *a;

    for (uint8_t i = 0; i < ASTEROIDS_QTY; i++)
    {
        a = &asteroids[i];
        a->x -= a->z;

        if (a->x < -10)
        {
            a->x = rand() % SCREEN_WIDTH + SCREEN_WIDTH;
            a->y = rand() % SCREEN_HEIGHT;
        }

        if (a->x <= SCREEN_WIDTH)
            a->sprite.pushSprite(&canvas, a->x, a->y, TFT_BLACK);
    }
}

/**
 * The rocket sprite is moved up and down by a small amount, and then drawn to the screen
 */
void drawRocket()
{
    uint8_t x = (SCREEN_WIDTH - 96) >> 1;
    uint8_t y = (SCREEN_HEIGHT - 96) >> 1;

    ry += rdy;
    if (ry > 20)
        rdy = -1;
    if (ry < 5)
        rdy = 1;

    rocket.pushSprite(x, y - ry, TFT_BLACK);
}

/**
 * It draws a rocket on the screen, and then draws a bunch of asteroids around it, then header
 * text next to rocket
 */
void rocketScreen()
{
    asteroid_t *a;

    rocket.setColorDepth(lgfx::palette_1bit);
    rocket.createSprite(96, 56);
    rocket.fillScreen(TFT_BLACK);
    rocket.drawBitmap(0, 1, rocket_img, 96, 54, TFT_WHITE);

    header.setColorDepth(lgfx::palette_1bit);
    header.createSprite(SCREEN_WIDTH, SCREEN_HEIGHT >> 1);
    header.fillScreen(TFT_BLACK);
    header.setTextColor(TFT_WHITE);
    header.setTextDatum(textdatum_t::top_center);

    for (uint8_t i = 0; i < ASTEROIDS_QTY; i++)
    {
        a = &asteroids[i];

        a->x = rand() % SCREEN_WIDTH;
        a->y = rand() % SCREEN_HEIGHT;
        a->z = rand() % 3 + 1;
        a->r = rand() % 4;
        a->sprite.setColorDepth(lgfx::palette_1bit);
        a->sprite.createSprite(10, 10);
        a->sprite.fillScreen(TFT_BLACK);
        a->sprite.fillCircle(5, 5, a->r, TFT_WHITE);
    }

    while (true)
    {
        EVERY_N_MILLISECONDS(33) // Test <timers.h>
        {
            canvas.clear();

            drawAsteroids();
            drawRocket();
            drawHeader();

            canvas.pushSprite(0, 0);
        }
    }

    rocket.deleteSprite();
    header.deleteSprite();
    for (uint8_t i = 0; i < ASTEROIDS_QTY; i++)
    {
        a = &asteroids[i];
        a->sprite.deleteSprite();
    }
}

void spectrumScreen()
{
    LGFX_Sprite spectrum(&canvas);
    LGFX_Sprite vu(&canvas);

    int32_t prevBands[BANDS] = {0};

    vu.setColorDepth(lgfx::palette_1bit);
    vu.createSprite(208, 40);
    vu.fillScreen(TFT_BLACK);

    spectrum.setColorDepth(lgfx::rgb332_1Byte);
    spectrum.createSprite(208, 100);
    spectrum.fillScreen(TFT_RED);

    uint16_t w = spectrum.width();
    uint16_t h = spectrum.height();
    uint16_t bw = w / BANDS;

    while (true)
    {

        if (millis() - frames >= 1000 / FPS)
        {

            canvas.clear();
            spectrum.clear(TFT_RED);
            vu.clear();
            vu.setCursor(0, 0);
            vu.printf("VU: %d", avgVU);

            for (uint8_t i = 0; i < BANDS; i++)
            {
                if (prevBands[i] < 0)
                    prevBands[i] = 0;
                int32_t bandHeight = (bandBins[i] < prevBands[i]) ? prevBands[i] - 1 : bandBins[i];
                uint16_t x = i * bw;

                spectrum.fillRect(x, 100, bw, -bandHeight, TFT_WHITE);
                spectrum.drawLine(x - 1, 0, x - 1, 100, TFT_RED);
                prevBands[i] = bandBins[i];
            }

            vu.pushSprite(16, 140);
            spectrum.pushSprite(16, 30);
            canvas.pushSprite(0, 0);

            frames = millis();
        }
    }

    frames = 0;
    spectrum.deleteSprite();
    vu.deleteSprite();
}

/**
 * The function is called once when the MCU starts. It sets up the serial port, turns off
 * the WiFi and Bluetooth, and initializes the display
 */
void setup()
{
    // Set highest CPU clockspeed
    esp_pm_lock_create(ESP_PM_CPU_FREQ_MAX, 0, "PowerManagementLock", &powerManagementLock);
    esp_pm_lock_acquire(powerManagementLock);

    // Setup Serial
    Serial.begin(115200);
    Serial.println();

    // Turn off the BT and Wi-Fi
    esp_wifi_stop();
    btStop();

    pinMode(ADC_PIN, ANALOG);
    analogSetPinAttenuation(ADC_PIN, ADC_0db);
    analogReadResolution(12);

    // VRef needs 3V3 divider to 1V1: 15K/7.5K resistors
    if ((bool)ADC_USE_VREF)
        analogSetVRefPin(ADC_VREF_PIN);

    samplingPeriodUs = round(1000000 * (1.0 / SAMPLING_FREQ));
    xTaskCreatePinnedToCore(adcReadTask, "ADC Read Task", 4096, NULL, 0, &adcTaskHandle, 0);

    // Setup CVBS display
    display.init();
    display.startWrite();

    // Create main canvas
    canvas.setColorDepth(lgfx::rgb332_1Byte);
    canvas.createSprite(SCREEN_WIDTH, SCREEN_HEIGHT);
    canvas.fillScreen(TFT_BLACK);
    canvas.pushSprite(0, 0);

    delay(500); // ?
}

/**
 * The function is called repeatedly by the Arduino
 */
void loop()
{
    spectrumScreen();
}
