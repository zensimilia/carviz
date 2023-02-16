#include "screen.h"

LGFX *Screen::cvbs(new LGFX);
LGFX_Sprite *Screen::canvas(new LGFX_Sprite(cvbs));

bool Screen::init()
{
    _initAlready = false;
    cvbs->_panel_instance.setResolution(_sizeX, _sizeY);

    if (cvbs->init())
    {
        _initAlready = true;
        cvbs->startWrite();
        initCanvas();

        return true;
    }
    return false;
}

bool Screen::begin() { return init(); }

void Screen::draw()
{
    if (_initAlready)
    {
        canvas->clear(TFT_WHITE);
        canvas->setTextColor(TFT_BLACK);
        canvas->setCursor(10, 10);
        canvas->printf("CVBS\nRAM:%6d\n", esp_get_free_heap_size());
        canvas->pushSprite(0, 0);
    }
}

void Screen::initCanvas()
{
    if (_initAlready)
    {
        canvas->deleteSprite();
        canvas->deletePalette();
        canvas->setColorDepth(lgfx::rgb332_1Byte);
        canvas->createSprite(_sizeX, _sizeY);
        canvas->fillScreen(TFT_BLACK);
        canvas->pushSprite(0, 0);
    }
}

void Screen::clearScreen() { clearScreen(TFT_BLACK); }

template <typename T>
void Screen::clearScreen(const T &color)
{
    canvas->clear(color);
    canvas->pushSprite(0, 0);
}

uint16_t Screen::width() const { return _sizeX; }

uint16_t Screen::height() const { return _sizeY; }

void Screen::push() { canvas->pushSprite(0, 0); }
