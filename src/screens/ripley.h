#pragma once

#include "lgfx.h"
#include "aspect.h"
#include "CEveryNTime.h"
#include "assets/ripley_img.h"

#define RIPLEY_WRITE_DELAY_MS 500

extern ASpect analyzer; // Global analyzer
extern LGFX cvbs;       // Global cvbs

namespace Screens
{
    class Ripley
    {
        LGFX_Sprite *indicators;
        LGFX_Sprite *textarea;

        const char *title = "体字 サイズ 12 ボルド";
        const char *description =
            "Lorem ipsum dolor sit amet, consectetur adipiscing elit.\n\n"
            "Etiam vel sem arcu. Etiam pulvinar nec urna ac suscipit. "
            "Vestibulum congue bibendum nisi, vel egestas eros varius vel.\n\n"
            "Nunc lobortis ipsum tortor, vitae tincidunt justo pretium ac. "
            "Vivamus dapibus purus vel urna imperdiet, in consequat orci vulputate.\n\n";

        uint32_t *spectrum = analyzer.getSpectrum();
        uint32_t descriptionIndex = 0;

        bool firstDraw = true;

    public:
        Ripley()
        {
            indicators = new LGFX_Sprite(&cvbs);
            indicators->setColorDepth(lgfx::palette_1bit);
            indicators->createSprite(170, 60);
            indicators->fillScreen(TFT_BLACK);

            textarea = new LGFX_Sprite(&cvbs);
            textarea->setColorDepth(lgfx::palette_1bit);
            textarea->createSprite(170, 80);
            textarea->fillScreen(TFT_BLACK);
            textarea->setClipRect(0, 0, textarea->width(), textarea->height());
            textarea->setTextWrap(true);
            textarea->setTextScroll(true);
        };
        Ripley(const Ripley &) = delete;
        ~Ripley()
        {
            delete textarea;
            delete indicators;
        };

        void draw();
        void drawDescription(const char *text);
        void drawIndicators();
        void drawBackground();
        void clearDescription(bool reset = true);
    }; // class Ripley
} // namespace Screens
