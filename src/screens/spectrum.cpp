#include "spectrum.h"

using namespace Screens;

void Spectrum::drawBar()
{
    if (*avgVUrefreshTime)
    {
        uint32_t avgVU = analyzer.getAvgVU();

        vu->clear();
        vu->setCursor(0, 0);
        vu->printf("VU:%3u", avgVU);
        vu->setCursor(vu->width() - 65, 0);
        vu->printf("RAM:%6d", esp_get_free_heap_size());
        vu->drawCenterString("20Hz-16kHz", vu->width() >> 1, 0);

        if (avgVU > 100)
            vu->fillCircle(44, 3, 3, 1);

        vu->pushSprite(10, cvbs._height - 20);
    }
}

void Spectrum::drawSpectrum()
{
    spectrum->clear(TFT_BLACK);

    for (uint8_t i = 0; i < BANDS; i++)
    {
        uint16_t x = i * bandWidth;

        bandHeight = prevBands[i] = h * ((prevBands[i] + bandBins[i]) >> 1) / 100; // Smooth

        spectrum->fillRect(x, h, bandWidth, constrain(-bandHeight, -(h >> 1), 0), 1);
        if (bandHeight > h >> 1)
            spectrum->fillRect(x, h >> 1, bandWidth, (h >> 1) - bandHeight, 2);
        if (bandHeight > h * 0.8)
            spectrum->fillRect(x, (h / 5), bandWidth, (h * 0.8) - bandHeight, 3);

        spectrum->drawFastVLine(x - 1, 0, h, TFT_BLACK);
    }

    for (uint8_t i = 0; i < 20; i++)
    {
        spectrum->drawFastHLine(0, i * (h / 20), w, TFT_BLACK);
    }

    spectrum->pushSprite(10, 10);
}

void Spectrum::draw()
{
    drawBar();
    drawSpectrum();
}
