#pragma once

#include <LovyanGFX.hpp>

#define LGFX_BRIGHTNESS 128
#define LGFX_SATURATION 128
#define LGFX_USE_PSRAM 0

/**
 * CVBS
 */
class LGFX : public lgfx::LGFX_Device
{
    lgfx::Panel_CVBS _panel_instance;

public:
    uint16_t _width;
    uint16_t _height;

    LGFX(uint16_t width = 240, uint16_t height = 160, uint8_t pin = DAC1) : _width(width), _height(height)
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
            cfg.use_psram = 0;

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
    };
    LGFX(const LGFX &) = delete;
    ~LGFX() = default;

    inline void drawColorTable()
    {
        const uint16_t height = this->height() >> 3;
        const uint16_t width = this->width();

        this->clear();

        for (uint16_t x = 0; x < width; ++x)
        {
            uint16_t v = x * 256 / width;

            this->fillRect(x, 0 * height, 7, height, this->color888(v, v, v));
            this->fillRect(x, 1 * height, 7, height, this->color888(v, 0, 0));
            this->fillRect(x, 2 * height, 7, height, this->color888(0, v, 0));
            this->fillRect(x, 3 * height, 7, height, this->color888(0, 0, v));
        }
    };
};
