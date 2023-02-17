#pragma once

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

#define LGFX_BRIGHTNESS 128
#define LGFX_SATURATION 128
#define LGFX_USE_PSRAM 0

/**
 * CVBS
 */
class LGFX : public lgfx::LGFX_Device
{
public:
    lgfx::Panel_CVBS _panel_instance;
    lgfx::LGFX_Sprite *canvas;

    LGFX(uint16_t width = 240, uint16_t height = 160, uint8_t pin = DAC1)
    {
        {
            auto cfg = _panel_instance.config();

            cfg.memory_width = width;
            cfg.memory_height = height;

            cfg.panel_width = width;
            cfg.panel_height = height;

            cfg.offset_x = 0;
            cfg.offset_y = 0;

            _panel_instance.config(cfg);
        }

        {
            auto cfg = _panel_instance.config_detail();

            cfg.signal_type = cfg.signal_type_t::NTSC; // NTSC | NTSC_J | PAL | PAL_M | PAL_N
            cfg.pin_dac = pin;                         // 25 | 26
            cfg.output_level = LGFX_BRIGHTNESS;        // Brightness
            cfg.chroma_level = LGFX_SATURATION;        // Saturation

            if ((bool)LGFX_USE_PSRAM)
            {
                // 0=SRAM | 1=PSRAM/SRAM | 2=PSRAM
                cfg.use_psram = 1;

                // Reading PSRAM task priority
                cfg.task_priority = 25;

                // Core that executes the task of reading PSRAM
                // in the background (APP_CPU_NUM | PRO_CPU_NUM)
                cfg.task_pinned_core = PRO_CPU_NUM;
            }

            _panel_instance.config_detail(cfg);
        }

        setPanel(&_panel_instance);
        this->setColorDepth(lgfx::palette_1bit);

        canvas = new lgfx::LGFX_Sprite(this);
        canvas->setColorDepth(lgfx::palette_1bit);
        canvas->createSprite(width, height);
        canvas->createPalette();
        canvas->fillScreen(TFT_BLACK);
        canvas->pushSprite(0, 0);
    };
    LGFX(const LGFX &) = delete;
    ~LGFX() { delete canvas; };

    inline void push() { canvas->pushSprite(0, 0); };

    inline void drawColorTable()
    {
        this->clear();

        for (int x = 0; x < this->width(); ++x)
        {
            int v = x * 256 / this->width();
            this->fillRect(x, 0 * this->height() >> 3, 7, this->height() >> 3, this->color888(v, v, v));
            this->fillRect(x, 1 * this->height() >> 3, 7, this->height() >> 3, this->color888(v, 0, 0));
            this->fillRect(x, 2 * this->height() >> 3, 7, this->height() >> 3, this->color888(0, v, 0));
            this->fillRect(x, 3 * this->height() >> 3, 7, this->height() >> 3, this->color888(0, 0, v));
        }
    };
};
