#include "planet.h"

using namespace Screens;

void Planet::draw()
{
    float_t r, rotate, th, os;
    float_t bass = ((spectrum[0] + spectrum[1]) >> 1) / 360.0f;

    planet->pushImage(0, 0, 360, 200, planet_img);

    for (uint16_t i = 0; i < 360; i++)
    {
        th = i * rad;
        os = bass * num_to_range(cos(th), -1, 1, 0, 1);
        r = 51 * (1 + os * sin(16 * th + millis() * 0.01f));
        rotate = th + rad * 105.0f;

        planet->drawPixel(r * sin(rotate) + pivotX, -r * cos(rotate) + pivotY, TFT_WHITE);
    }

    planet->pushSprite(0, 0);
}
