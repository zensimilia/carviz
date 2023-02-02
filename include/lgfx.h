#pragma once

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

#include "settings.h"

class LGFX : public lgfx::LGFX_Device
{
public:
    lgfx::Panel_CVBS _panel_instance;

    LGFX(void)
    {
        {
            auto cfg = _panel_instance.config();

            cfg.memory_width = SCREEN_WIDTH;
            cfg.memory_height = SCREEN_HEIGHT;

            cfg.panel_width = SCREEN_WIDTH;
            cfg.panel_height = SCREEN_HEIGHT;

            cfg.offset_x = 0;
            cfg.offset_y = 0;

            _panel_instance.config(cfg);
        }

        {
            auto cfg = _panel_instance.config_detail();

            cfg.signal_type = cfg.signal_type_t::NTSC; // NTSC | NTSC_J | PAL | PAL_M | PAL_N
            cfg.pin_dac = VOUT_PIN;                    // 25 | 26
            cfg.output_level = 128;                    // Brightness
            cfg.chroma_level = 128;                    // Saturation

            if ((bool)USE_PSRAM)
            {
                cfg.use_psram = 0; // 0=!PSRAM | 1=PSRAM/SRAM | 2=PSRAM

                // Reading PSRAM task priority
                cfg.task_priority = 25;

                // Core that executes the task of reading PSRAM
                // in the background (APP_CPU_NUM | PRO_CPU_NUM)
                cfg.task_pinned_core = PRO_CPU_NUM;
            }

            _panel_instance.config_detail(cfg);
        }

        setPanel(&_panel_instance);
    }
};
