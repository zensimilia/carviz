#include "screen.h"

LGFX *Screen::display(new LGFX);
LGFX_Sprite *Screen::canvas(new LGFX_Sprite(display));

bool Screen::initDisplay()
{
    display->_panel_instance.setResolution(_sizeX, _sizeY);

    if (display->init())
    {
        display->startWrite();
        initCanvas();
        _initAlready = true;

        return true;
    }
    return false;
}

void Screen::draw()
{
    if (_initAlready)
    {
        canvas->clear(TFT_WHITE);
        canvas->setTextColor(TFT_BLACK);
        canvas->setCursor(10, 10);
        canvas->printf("CVBS / RAM:%6d", esp_get_free_heap_size());
        canvas->pushSprite(0, 0);
    }
};

void Screen::initCanvas()
{
    canvas->setColorDepth(lgfx::rgb332_1Byte);
    canvas->createSprite(_sizeX, _sizeY);
    canvas->fillScreen(TFT_BLACK);
    canvas->pushSprite(0, 0);
}

void Screen::pushCanvas(uint16_t x, uint16_t y)
{
    canvas->pushSprite(x, y);
}

void Screen::clearScreen()
{
    canvas->clear();
    canvas->pushSprite(0, 0);
}
