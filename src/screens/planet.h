#pragma once

#include "lgfx.h"
#include "aspect.h"
#include "settings.h"
#include "utils.h"
#include "assets/planet_img.h"

extern ASpect analyzer; // Global analyzer
extern LGFX cvbs;       // Global cvbs

namespace Screens
{
    class Planet
    {
        LGFX_Sprite *planet;
        uint32_t *spectrum = analyzer.getSpectrum();

    public:
        Planet()
        {
            planet = new LGFX_Sprite(&cvbs);
            planet->setColorDepth(lgfx::palette_1bit);
            planet->createSprite(360, 200);
            planet->clear(TFT_BLACK);
        };
        Planet(const Planet &) = delete;
        ~Planet() = default;

        void draw();
    }; // class Planet
} // namespace Screens
