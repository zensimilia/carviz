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
        struct star_t
        {
            float_t x;
            float_t y;
            float_t z;
            uint8_t r;
        };

    private:
        uint8_t _numStars;
        float_t _speed;
        float_t _delta = 0.01f;
        float_t _spread = STARFIELD_SPREAD;
        float_t _idx, _idy;
        const uint16_t _hw = cvbs._width >> 1;
        const uint16_t _hh = cvbs._height >> 1;

        std::vector<star_t> _stars;

        LGFX_Sprite *canvas;

        void initStar(uint8_t i);

    public:
        Starfield(uint8_t numStars = 64, float_t speed = 10)
        {
            _numStars = numStars;
            _speed = speed;

            canvas = new LGFX_Sprite(&cvbs);
            canvas->setColorDepth(lgfx::palette_1bit);
            canvas->createSprite(cvbs._width, cvbs._height);
            canvas->fillScreen(TFT_BLACK);

            _stars.resize(_numStars);
            for (uint8_t i = 0; i < _numStars; i++)
            {
                initStar(i);
            }
        };
        Starfield(const Starfield &) = delete;
        ~Starfield()
        {
            delete canvas;
        };

        void draw();
    }; // class Starfield

} // namespace Screens
