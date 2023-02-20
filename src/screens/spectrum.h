#pragma once

#include "lgfx.h"
#include "CEveryNTime.h"
#include "aspect.h"

extern ASpect analyzer; // Global analyzer
extern LGFX cvbs;       // Global cvbs

#define SPECTRUM_VU_REFRESH_MS 100

namespace Screens
{
    class Spectrum
    {
        LGFX_Sprite *vu;
        LGFX_Sprite *spectrum;
        CEveryNMillis *avgVUrefreshTime;

        uint32_t prevBands[BANDS] = {0};
        uint32_t *bandBins;
        uint8_t bandHeight = 0;

        uint16_t w;
        uint16_t h;
        uint16_t bandWidth;

    public:
        Spectrum()
        {
            avgVUrefreshTime = new CEveryNMillis(SPECTRUM_VU_REFRESH_MS);
            bandBins = analyzer.getSpectrum();

            vu = new LGFX_Sprite(&cvbs);
            vu->setColorDepth(lgfx::palette_2bit);
            vu->createSprite(cvbs._width - 20, 10);
            vu->setPaletteColor(1, TFT_RED);
            vu->fillScreen(TFT_BLACK);
            vu->setTextColor(TFT_WHITE, TFT_BLACK);

            spectrum = new LGFX_Sprite(&cvbs);
            spectrum->setColorDepth(lgfx::palette_4bit);
            spectrum->createSprite(cvbs._width - 20, cvbs._height - 40);
            spectrum->setPaletteColor(1, TFT_GREENYELLOW);
            spectrum->setPaletteColor(2, TFT_YELLOW);
            spectrum->setPaletteColor(3, TFT_RED);
            spectrum->fillScreen(TFT_BLACK);

            w = spectrum->width();
            h = spectrum->height();
            bandWidth = (w / BANDS);
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
