#pragma once

#include "settings.h"
#include "lgfx.h"

class Screen
{
private:
    bool _initAlready = false;

public:
    static LGFX *display;
    static LGFX_Sprite *canvas;

    Screen() = default;
    Screen(uint16_t sizeX, uint16_t sizeY) : _sizeX(sizeX), _sizeY(sizeY){};
    ~Screen()
    {
        delete canvas;
        delete display;
    };

    bool initDisplay();
    virtual void draw();
    virtual void clearScreen();

protected:
    uint16_t _sizeX = 240;
    uint16_t _sizeY = 160;

    void initCanvas();
    void pushCanvas(uint16_t x = 0, uint16_t y = 0);
};
