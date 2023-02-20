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
    uint16_t bass = (spectrum[0] + spectrum[1]) / 2;
    uint16_t midd;
    uint16_t treb;

    for (uint8_t i = 2; i < 9; i++)
    {
        midd += spectrum[i];
    }
    midd /= 7;

    for (uint8_t i = 9; i < 16; i++)
    {
        treb += spectrum[i];
    }
    treb /= 7;

    indicators->clear();

    indicators->setTextColor(TFT_WHITE);
    indicators->setCursor(0, 0);
    indicators->printf("TIME %6d\n", millis() / 1000);
    indicators->printf("HEAP %6d\n", esp_get_free_heap_size());
    indicators->printf("PEAK %6d\n", analyzer.getAvgVU());
    indicators->print("BASS\nMIDD\nTREB");

    indicators->drawRoundRect(35, 24, 130, 8, 2, TFT_WHITE);
    indicators->drawRoundRect(35, 32, 130, 8, 2, TFT_WHITE);
    indicators->drawRoundRect(35, 40, 130, 8, 2, TFT_WHITE);

    indicators->fillRect(38, 26, bass / 100.0 * 124, 4, TFT_WHITE);
    indicators->fillRect(38, 34, midd / 100.0 * 124, 4, TFT_WHITE);
    indicators->fillRect(38, 42, treb / 100.0 * 124, 4, TFT_WHITE);

    indicators->pushSprite(cvbs.width() >> 1, 10);
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
