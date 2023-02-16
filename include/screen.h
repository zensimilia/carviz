#pragma once

#include "lgfx.h"

class Screen
{
private:
    bool _initAlready = false;
    uint16_t _sizeX;
    uint16_t _sizeY;

public:
    static LGFX *cvbs;
    static LGFX_Sprite *canvas;

    Screen(uint16_t width = 240, uint16_t height = 160) : _sizeX(width), _sizeY(height){};
    Screen(Screen const &) = delete;
    ~Screen() = default;

    bool init();
    bool begin();
    uint16_t width() const;
    uint16_t height() const;

    template <typename T>
    void clearScreen(const T &color);
    void clearScreen();

    void draw();
    void push();

protected:
    void initCanvas();
}; // class Screen
