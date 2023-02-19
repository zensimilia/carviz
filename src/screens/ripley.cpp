#include "ripley.h"

using namespace Screens;

void Ripley::draw()
{
    _canvas->clear();

    _canvas->drawFastHLine(0, 70, _canvas->width(), TFT_WHITE);
    _canvas->fillRect(0, 72, _canvas->width(), 16, TFT_WHITE);
    _canvas->drawFastHLine(0, 89, _canvas->width(), TFT_WHITE);

    _canvas->setFont(&fonts::efontCN_12_b);
    _canvas->setCursor(_canvas->width() >> 1, 74);
    _canvas->setTextColor(TFT_BLACK);
    _canvas->print("体字 サイズ 12 ボルド");

    _canvas->pushImage(30, _canvas->height() - 183, 141, 183, ripley_img, 0xe3);

    _canvas->setFont(&fonts::Font0);
    _canvas->setCursor(_canvas->width() >> 1, 50);
    _canvas->setTextColor(TFT_WHITE);
    _canvas->printf("RAM:%6d", esp_get_free_heap_size());

    drawText();

    _canvas->pushSprite(0, 0);
}

void Ripley::drawText()
{

    if (_di < description.length())
    {
        _textarea->print(description.at(_di));
        _di++;
    }
    if (_di >= description.length())
        _di = 0;

    _textarea->pushSprite((_canvas->width() >> 1), (_canvas->height() >> 1) + 5);
}
