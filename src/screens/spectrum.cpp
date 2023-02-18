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
        vu->setCursor(148, 0);
        vu->printf("RAM:%6d", esp_get_free_heap_size());
        vu->drawCenterString("20Hz-16kHz", vu->width() >> 1, 0);

        if (avgVU > 100)
            vu->fillCircle(44, 3, 3, 1);

        vu->pushSprite(16, 140);
    }
}

void Spectrum::drawSpectrum()
{
    spectrum->clear(TFT_BLACK);

    for (uint8_t i = 0; i < BANDS; i++)
    {
        uint16_t x = i * bw;

        bandHeight = prevBands[i] = (prevBands[i] + bandBins[i]) >> 1; // Smooth
        spectrum->fillRect(x, 100, bw, constrain(-bandHeight, -50, 0), 1);

        if (bandHeight > 50)
            spectrum->fillRect(x, 50, bw, 50 - bandHeight, 2);

        if (bandHeight > 80)
            spectrum->fillRect(x, 20, bw, 80 - bandHeight, 3);

        spectrum->drawFastVLine(x - 1, 0, h, TFT_BLACK);
    }

    for (uint8_t i = 0; i < 20; i++)
    {
        spectrum->drawFastHLine(0, i * 5, w, TFT_BLACK);
    }

    spectrum->pushSprite(16, 30);
}

void Spectrum::draw()
{
    drawBar();
    drawSpectrum();
}
