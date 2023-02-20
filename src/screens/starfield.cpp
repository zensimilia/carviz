#include "starfield.h"

using namespace Screens;

void Starfield::draw()
{
    uint8_t ix, iy;
    uint32_t *spectrum = analyzer.getSpectrum();
    static uint64_t frames;

    canvas->clear();

    for (uint8_t i = 0; i < stars.size(); i++)
    {
        star_t *s = &stars[i];

        // Move the star towards the camera which is at 0 on Z.
        s->z -= delta * speed;

        // If a star is at or behind the camera, generate a new position for it
        if (s->z <= 0)
            initStar(i);

        // Multiplying the position by (size/2) and then adding (size/2)
        // remaps the positions from range (-1, 1) to (0, size)
        int16_t x = (s->x / s->z) * halfWidth + halfWidth;
        int16_t y = (s->y / s->z) * halfHeight + halfHeight;

        if (x < 0 || x >= canvas->width() || (y < 0 || y >= canvas->height()))
        {
            // If the star is not within range of the screen,
            // then generate a new position for it
            initStar(i);
        }
        else
        {
            // Otherwise, it is safe to draw this star to the screen
            s->r = max(0.0f, (5.0f - s->z) + (spectrum[i / BANDS] * 0.03f));
            canvas->fillCircle(x, y, s->r, TFT_WHITE);
        }
    }

    ix = (canvas->width() - 120) >> 1;
    iy = (canvas->height() - 95) >> 1;

    // Calculate delta for astronaut animation
    idx = 2.0f * sinf(frames / 15);
    idy = 2.0f * sinf(frames / 20 + 2 * PI);

    // Draw astronaut and push frame to the screen
    canvas->pushImage(ix + std::roundf(idx), iy + std::roundf(idy), 120, 95, astronaut_img, 0xe0);
    canvas->pushSprite(0, 0);

    frames++;
}

void Starfield::initStar(uint8_t index)
{
    star_t *s = &stars[index];

    s->x = 2 * ((float)((double)rand() / (double)RAND_MAX) - 0.5f) * spread;
    s->y = 2 * ((float)((double)rand() / (double)RAND_MAX) - 0.5f) * spread;

    // For Z, the random value is only adjusted by a small amount to stop
    // a star from being generated at 0 on Z.
    s->z = ((float)((double)rand() / (double)RAND_MAX) + 0.1f) * spread;
}
