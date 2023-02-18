#pragma once

#include <vector>

#include "lgfx.h"
#include "aspect.h"
#include "assets/rocket_img.h"

extern ASpect analyzer; // Global analyzer
extern LGFX cvbs;       // Global cvbs

namespace Screens
{
    class Rocket
    {
    public:
        Rocket()
        {
            // TODO: try `sprite.setBuffer` or `pushImage`
            // https://github.com/lovyan03/LovyanGFX/blob/master/examples/Sprite/FlashMemSprite/FlashMemSprite.ino
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
        void drawHeader(const char *text = "ASTRO BLACK", float_t textSize = 0.7f);

    private:
        // Struct for asteroids: position, speed, radius and sprite
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

        const uint16_t rocketPivotX = (cvbs._width - 96) >> 1;
        const uint16_t rocketPivotY = (cvbs._height - 96) >> 1;
        const uint16_t headerPivotX = cvbs._width >> 1;
        const uint16_t headerPivotY = (cvbs._height >> 1) + 10;
        const uint16_t blinkDelayMs = 600;

        uint8_t ry = 20;
        int8_t rdy = 1;

        uint32_t *bandBins = analyzer.getSpectrum();
    }; // class Rocket
} // namespace Screens
