#include "rocket.h"

using namespace Screens;

void Rocket::draw()
{
    cvbs.canvas->clear();

    drawAsteroids();
    drawRocket();
    drawHeader();

    cvbs.push();
}

void Rocket::initAsteroids()
{
    asteroid_t *a;

    for (uint8_t i = 0; i < asteroids.size(); i++)
    {
        a = &asteroids[i];

        a->x = rand() % cvbs.width();
        a->y = rand() % cvbs.height();
        a->z = rand() % 3 + 1;
        a->r = rand() % 4;
        a->sprite = new LGFX_Sprite(rocket);
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
    ry += rdy;
    if (ry > 20)
        rdy = -1;
    if (ry < 5)
        rdy = 1;

    rocket->pushSprite(rocketPivotX, rocketPivotY - ry, TFT_BLACK);
}

void Rocket::drawAsteroids()
{
    asteroid_t *a;

    for (uint8_t i = 0; i < asteroids.size(); i++)
    {
        a = &asteroids[i];
        a->x -= a->z;
        a->r = 3.0 / 100 * bandBins[i];

        if (a->x < -10)
        {
            a->x = rand() % cvbs.width() + cvbs.width();
            a->y = rand() % cvbs.height();
        }

        if (a->x <= cvbs.width())
        {
            a->sprite->clear();
            a->sprite->fillCircle(5, 5, a->r, TFT_WHITE);
            a->sprite->pushSprite(cvbs.canvas, a->x, a->y, TFT_BLACK);
        }
    }
}

void Rocket::drawHeader(const char *text, float_t textSize)
{
    header->clear();
    header->setTextSize(textSize);
    header->setFont(&fonts::Orbitron_Light_24);
    header->drawString(text, headerPivotX, 0);

    if ((millis() / blinkDelayMs) % 2)
    {
        header->setTextSize(1);
        header->setFont(&fonts::Font8x8C64);
        header->drawString("PRESS START", headerPivotX, 24);
    }

    header->pushSprite(0, headerPivotY, TFT_BLACK);
}
