#include "ripley.h"

using namespace Screens;

void Ripley::draw()
{
    if (firstDraw)
        drawBackground();

    drawIndicators();

    EVERY_N_MILLISECONDS(RIPLEY_WRITE_DELAY_MS)
    {
        drawDescription(description);
    }
}

void Ripley::drawDescription(const char *text)
{
    const size_t descriptionLength = strlen(text);

    if (descriptionIndex < descriptionLength)
        textarea->print(text[descriptionIndex++]);

    if (descriptionIndex >= descriptionLength)
        descriptionIndex = 0;

    textarea->pushSprite((cvbs.width() >> 1), (cvbs.height() >> 1) + 5);
}

void Ripley::drawIndicators()
{
    indicators->clear();

    indicators->setTextColor(TFT_WHITE);
    indicators->setCursor(0, 0);
    indicators->printf("RAM:%6d", esp_get_free_heap_size());

    indicators->pushSprite(indicators->width() >> 1, 20);
}

void Ripley::drawBackground()
{
    firstDraw = false;

    cvbs.clear(TFT_BLACK);

    cvbs.drawFastHLine(0, 70, cvbs.width(), TFT_WHITE);
    cvbs.fillRect(0, 72, cvbs.width(), 16, TFT_WHITE);
    cvbs.drawFastHLine(0, 89, cvbs.width(), TFT_WHITE);

    cvbs.setTextColor(TFT_BLACK);
    cvbs.drawString(title, cvbs.width() >> 1, 74, &fonts::efontCN_12_b);

    cvbs.pushImage(30, cvbs.height() - 183, 141, 183, ripley_img, 0xe3);
}

void Ripley::clearDescription(bool reset)
{
    if (reset)
        descriptionIndex = 0;

    textarea->clear(TFT_BLACK);
    textarea->setCursor(0, 0);
}
