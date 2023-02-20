#pragma once

#include <vector>

#include "lgfx.h"
#include "aspect.h"
#include "assets/astronaut_img.h"

#define STARFIELD_SPREAD 10.0f

extern ASpect analyzer; // Global analyzer
extern LGFX cvbs;       // Global cvbs

namespace Screens
{
    class Starfield
    {
        // Struct for stars: position (x,y,z) and radius (r)
        struct star_t
        {
            float_t x;
            float_t y;
            float_t z;
            uint8_t r;
        };
        std::vector<star_t> stars;

        uint8_t numStars;
        float_t speed;
        float_t delta = 0.01f;
        float_t spread = STARFIELD_SPREAD;
        float_t idx, idy;
        const uint16_t halfWidth = cvbs._width >> 1;
        const uint16_t halfHeight = cvbs._height >> 1;

        LGFX_Sprite *canvas;

        void initStar(uint8_t index);

    public:
        Starfield(uint8_t _stars = 64, float_t _speed = 10)
        {
            numStars = _stars;
            speed = _speed;

            canvas = new LGFX_Sprite(&cvbs);
            canvas->setColorDepth(lgfx::palette_1bit);
            canvas->createSprite(cvbs._width, cvbs._height);
            canvas->fillScreen(TFT_BLACK);

            stars.resize(_stars);
            for (uint8_t i = 0; i < _stars; i++)
            {
                initStar(i);
            }
        };
        Starfield(const Starfield &) = delete;
        ~Starfield() { delete canvas; };

        void draw();
    }; // class Starfield
} // namespace Screens
