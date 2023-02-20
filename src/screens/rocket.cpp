#include "rocket.h"

using namespace Screens;

void Rocket::draw()
{
    canvas->clear();

    drawAsteroids();
    drawRocket();
    drawHeader();

    canvas->pushSprite(0, 0);
}

void Rocket::initAsteroids()
{
    asteroid_t *a;

    for (uint8_t i = 0; i < asteroids.size(); i++)
    {
        a = &asteroids[i];

        a->x = rand() % canvas->width();
        a->y = rand() % canvas->height();
        a->z = rand() % 3 + 1;
        a->r = 1;
        a->sprite = new LGFX_Sprite(canvas);
        a->sprite->setColorDepth(lgfx::palette_1bit);
        a->sprite->createSprite(10, 10);
        a->sprite->fillScreen(TFT_BLACK);
        a->sprite->fillCircle(5, 5, a->r, TFT_WHITE);
    }
}

void Rocket::setAsteroidsQty(uint8_t qty)
{
    asteroid_t *a;

    for (uint8_t i = 0; i < asteroids.size(); i++)
    {
        a = &asteroids[i];
        delete a->sprite;
    }

    asteroids.resize(qty);
    initAsteroids();
};

void Rocket::drawRocket()
{
    static const uint16_t rocketX = (canvas->width() - 96) >> 1;
    static const uint16_t rocketY = (canvas->height() - 96) >> 1;

    ry += rdy;
    if (ry > 20)
        rdy = -1;
    if (ry < 5)
        rdy = 1;

    canvas->drawBitmap(rocketX, rocketY - ry, rocket_img, 96, 54, TFT_WHITE);
}

void Rocket::drawAsteroids()
{
    asteroid_t *a;

    for (uint8_t i = 0; i < asteroids.size(); i++)
    {
        a = &asteroids[i];
        a->x -= a->z;
        a->r = 3.0 / 100 * spectrum[i];

        if (a->x < -10)
        {
            a->x = rand() % canvas->width() + canvas->width();
            a->y = rand() % canvas->height();
            a->z = rand() % 3 + 1;
        }

        if (a->x <= canvas->width())
        {
            a->sprite->clear();
            a->sprite->fillCircle(5, 5, a->r, TFT_WHITE);
            a->sprite->pushSprite(a->x, a->y, TFT_BLACK);
        }
    }
}

void Rocket::drawHeader(const char *text, float_t textSize)
{
    static const uint16_t halfWidth = canvas->width() >> 1;
    static const uint16_t halfHeight = canvas->height() >> 1;

    uint8_t fontOffset = canvas->fontHeight(&fonts::Orbitron_Light_24) * textSize + 2;

    canvas->setTextSize(textSize);
    canvas->setFont(&fonts::Orbitron_Light_24);
    canvas->drawCenterString(text, halfWidth, halfHeight + 10);

    if ((millis() / ROCKET_BLINK_DELAY_MS) % 2)
    {
        canvas->setTextSize(1);
        canvas->drawCenterString("PRESS START", halfWidth, halfHeight + fontOffset, &fonts::Font8x8C64);
    }
}
