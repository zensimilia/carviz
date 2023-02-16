#pragma once

#include "screen.h"
#include "CEveryNTime.h"
#include "audio_analyzer.h"

extern Screen cvbs; // Global screen

namespace Screens
{
    class Spectrum
    {
    private:
        LGFX_Sprite *spectrum;
        LGFX_Sprite *vu;
        CEveryNMillis *avgVUrefreshTime;

        uint32_t prevBands[BANDS] = {0};
        uint32_t *bandBins = getSpectrumBins();
        uint8_t bandHeight = 0;

        uint16_t w;
        uint16_t h;
        uint16_t bw;

    public:
        Spectrum()
        {
            avgVUrefreshTime = new CEveryNMillis(100);

            vu = new LGFX_Sprite(cvbs.canvas);
            vu->setColorDepth(lgfx::rgb332_1Byte);
            vu->createSprite(208, 10);
            vu->fillScreen(TFT_BLACK);
            vu->setTextColor(TFT_WHITE, TFT_BLACK);

            spectrum = new LGFX_Sprite(cvbs.canvas);
            spectrum->setColorDepth(lgfx::rgb332_1Byte);
            spectrum->createSprite(208, 100);
            spectrum->fillScreen(TFT_BLACK);

            w = spectrum->width();
            h = spectrum->height();
            bw = (w / BANDS);
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
    }; // class Spectrum
} // namespace Screens