#pragma once

#include <vector>

#include "lgfx.h"
#include "aspect.h"
#include "assets/rocket_img.h"

#define ROCKET_BLINK_DELAY_MS 600

extern ASpect analyzer; // Global analyzer
extern LGFX cvbs;       // Global cvbs

namespace Screens
{
    class Rocket
    {
        // Struct for asteroids: position (x,y), speed (z), radius (r) and (sprite)
        struct asteroid_t
        {
            int16_t x;
            int16_t y;
            uint8_t z;
            uint8_t r;
            LGFX_Sprite *sprite;
        };
        std::vector<asteroid_t> asteroids;

        LGFX_Sprite *canvas;

        uint8_t ry = 20;
        int8_t rdy = 1;

        uint32_t *spectrum = analyzer.getSpectrum();

    public:
        Rocket()
        {
            canvas = new LGFX_Sprite(&cvbs);
            canvas->setColorDepth(lgfx::palette_1bit);
            canvas->createSprite(cvbs._width, cvbs._height);
            canvas->fillScreen(TFT_BLACK);
            canvas->setTextColor(TFT_WHITE);

            setAsteroidsQty(16);
        };
        ~Rocket()
        {
            delete canvas;

            {
                asteroid_t *a;

                for (uint8_t i = 0; i < asteroids.size(); i++)
                {
                    a = &asteroids[i];
                    delete a->sprite;
                }
            }
        };

        void draw();
        void setAsteroidsQty(uint8_t qty);
        void initAsteroids();
        void drawAsteroids();
        void drawRocket();
        void drawHeader(const char *text = "ASTRO BLACK", float_t textSize = 0.8f);

    }; // class Rocket
} // namespace Screens
