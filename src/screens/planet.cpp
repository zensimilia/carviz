#include "planet.h"

using namespace Screens;

void Planet::draw()
{
    float_t r;
    float_t t = millis();
    float_t bass = ((spectrum[0] + spectrum[1]) >> 1) / 500.0f;
    float_t rad = PI / 180;
    float_t pivotX = (planet->width() >> 1);
    float_t pivotY = (planet->height() >> 1);

    planet->pushImage(0, 0, 360, 200, planet_img);

    for (uint16_t i = 0; i < 360; i++)
    {
        // float_t x = (planet->width() >> 1) + 10 * sin(q + t / 1000);
        // float_t y = (planet->height() >> 1) + 10 * cos(-q + t / 1000);
        float_t th = i * rad;
        float_t os = num_to_range(cos(th - TWO_PI * t), -1, 1, 0, 1);
        os = bass * pow(os, 2.75);
        r = 50 * (1 + os * cos(20 * th + 1.5 * TWO_PI * (t / 1000) + PI));

        planet->drawPixel(r * sin(th) + pivotX, -r * cos(th) + pivotY, TFT_WHITE);
    }

    planet->pushSprite(0, 0, TFT_BLACK);
}
