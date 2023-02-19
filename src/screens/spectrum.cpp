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
    const uint16_t sh = spectrum->height();
    spectrum->clear(TFT_BLACK);

    for (uint8_t i = 0; i < BANDS; i++)
    {
        uint16_t x = i * bw;

        bandHeight = prevBands[i] = sh * ((prevBands[i] + bandBins[i]) >> 1) / 100; // Smooth
        spectrum->fillRect(x, sh, bw, constrain(-bandHeight, -(sh >> 1), 0), 1);

        if (bandHeight > sh >> 1)
            spectrum->fillRect(x, sh >> 1, bw, (sh >> 1) - bandHeight, 2);

        if (bandHeight > sh * 0.8)
            spectrum->fillRect(x, (sh / 5), bw, (sh * 0.8) - bandHeight, 3);

        spectrum->drawFastVLine(x - 1, 0, h, TFT_BLACK);
    }

    for (uint8_t i = 0; i < 20; i++)
    {
        spectrum->drawFastHLine(0, i * (spectrum->height() / 20), w, TFT_BLACK);
    }

    spectrum->pushSprite(10, 10);
}

void Spectrum::draw()
{
    drawBar();
    drawSpectrum();
}
