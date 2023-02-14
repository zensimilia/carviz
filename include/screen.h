#pragma once

#include "settings.h"
#include "lgfx.h"

class Screen
{
private:
    bool _initAlready = false;
    uint16_t _sizeX;
    uint16_t _sizeY;

public:
    static LGFX *display;
    static LGFX_Sprite *canvas;

    Screen(uint16_t sizeX, uint16_t sizeY) : _sizeX(sizeX), _sizeY(sizeY){};
    ~Screen()
    {
        canvas->deleteSprite();
    };

    bool initDisplay(void);
    virtual void draw(void);
    virtual void clearScreen(void);

protected:
    void initCanvas();
};
