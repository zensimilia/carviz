#pragma once

#include "screen.h"
#include "CEveryNTime.h"
#include "audio_analyzer.h"

class Spectrum : public Screen
{
private:
    LGFX_Sprite *spectrum;
    LGFX_Sprite *vu;
    CEveryNMillis *avgVUrefreshTime;

    uint8_t prevBands[BANDS] = {0};
    uint32_t *bandBins = getSpectrumBins();

    const uint16_t w = spectrum->width();
    const uint16_t h = spectrum->height();
    const uint16_t bw = (w / BANDS);

public:
    Spectrum()
    {
        spectrum = new LGFX_Sprite(canvas);
        vu = new LGFX_Sprite(canvas);
        avgVUrefreshTime = new CEveryNMillis(100);

        vu->setColorDepth(lgfx::rgb332_1Byte);
        vu->createSprite(208, 20);
        vu->fillScreen(TFT_BLACK);
        vu->setTextColor(TFT_WHITE, TFT_BLACK);

        spectrum->setColorDepth(lgfx::rgb332_1Byte);
        spectrum->createSprite(208, 100);
        spectrum->fillScreen(TFT_BLACK);
    };
    ~Spectrum()
    {
        delete spectrum;
        delete vu;
        delete avgVUrefreshTime;
    }

    void draw();
    void drawBar();
    void drawSpectrum();
};
