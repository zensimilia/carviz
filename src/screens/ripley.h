#pragma once

#include "lgfx.h"
#include "aspect.h"
#include "assets/ripley_img.h"

extern ASpect analyzer; // Global analyzer
extern LGFX cvbs;       // Global cvbs

namespace Screens
{
    class Ripley
    {
        std::string description = "Lorem ipsum dolor sit amet, consectetur adipiscing elit.\n\nEtiam vel sem arcu. Etiam pulvinar nec urna ac suscipit. Vestibulum congue bibendum nisi, vel egestas eros varius vel.\n\nNunc lobortis ipsum tortor, vitae tincidunt justo pretium ac. Vivamus dapibus purus vel urna imperdiet, in consequat orci vulputate.\n\n";

    private:
        uint32_t *_spectrum = analyzer.getSpectrum();
        LGFX_Sprite *_canvas;
        LGFX_Sprite *_textarea;

        uint32_t _di = 0;

    public:
        Ripley()
        {
            _canvas = new LGFX_Sprite(&cvbs);
            _canvas->setColorDepth(lgfx::palette_1bit);
            _canvas->createSprite(cvbs._width, cvbs._height);
            _canvas->fillScreen(TFT_BLACK);

            _textarea = new LGFX_Sprite(_canvas);
            _textarea->setColorDepth(lgfx::palette_1bit);
            _textarea->createSprite(170, 80);
            _textarea->setClipRect(0, 0, _textarea->width(), _textarea->height());
            _textarea->setTextWrap(true);
            _textarea->setTextScroll(true);
        };
        ~Ripley()
        {
            delete _textarea;
            delete _canvas;
        };

        void draw();
        void drawText();
    }; // class Ripley
} // namespace Screens
