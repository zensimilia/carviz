#include "planet.h"

using namespace Screens;

void Planet::draw()
{
    float_t r;
    float_t t = millis();
    uint8_t q = PI;
    float_t bass = spectrum[0] / 500.0f;

    planet->clear();

    for (uint16_t i = 0; i < 360; i++)
    {
        // float_t x = (planet->width() >> 1) + 10 * sin(q + t / 1000);
        // float_t y = (planet->height() >> 1) + 10 * cos(-q + t / 1000);
        float_t x = (planet->width() >> 1);
        float_t y = (planet->height() >> 1);
        float_t th = i * TWO_PI / 360;
        float_t os = num_to_range(cos(th - TWO_PI * t), -1, 1, 0, 1);
        os = bass * pow(os, 2.75);
        r = 70 * (1 + os * cos(20 * th + 1.5 * TWO_PI * (t / 1000) + q));

        planet->drawPixel(r * sin(th) + x, -r * cos(th) + y, TFT_WHITE);
    }

    planet->pushSprite((cvbs.width() - planet->width() >> 1), 0);
}
