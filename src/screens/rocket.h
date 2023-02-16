#pragma once

#include <vector>

#include "screen.h"
#include "aspect.h"
#include "assets/rocket_img.h"

extern ASpect analyzer; // Global analyzer
extern Screen cvbs;     // Global screen

namespace Screens
{
    class Rocket
    {
    public:
        Rocket()
        {
            rocket = new LGFX_Sprite(cvbs.canvas);
            rocket->setColorDepth(lgfx::palette_1bit);
            rocket->createSprite(96, 56);
            rocket->fillScreen(TFT_BLACK);
            rocket->drawBitmap(0, 1, rocket_img, 96, 54, TFT_WHITE);

            header = new LGFX_Sprite(cvbs.canvas);
            header->setColorDepth(lgfx::palette_1bit);
            header->createSprite(cvbs.width(), cvbs.height() >> 1);
            header->fillScreen(TFT_BLACK);
            header->setTextColor(TFT_WHITE);
            header->setTextDatum(textdatum_t::top_center);

            setAsteroidsQty(16);
        };
        ~Rocket()
        {
            asteroid_t *a;

            delete rocket;
            delete header;

            for (uint8_t i = 0; i < asteroids.size(); i++)
            {
                a = &asteroids[i];
                delete a->sprite;
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

        LGFX_Sprite *rocket;
        LGFX_Sprite *header;

        const uint16_t rocketPivotX = (cvbs.width() - 96) >> 1;
        const uint16_t rocketPivotY = (cvbs.height() - 96) >> 1;
        const uint16_t headerPivotX = cvbs.width() >> 1;
        const uint16_t headerPivotY = (cvbs.height() >> 1) + 10;
        const uint16_t blinkDelayMs = 600;

        uint8_t ry = 20;
        int8_t rdy = 1;

        uint32_t *bandBins = analyzer.getSpectrum();
    }; // class Rocket
} // namespace Screens
