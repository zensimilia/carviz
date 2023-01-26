#pragma once

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

class LGFX : public lgfx::LGFX_Device
{
public:
    lgfx::Panel_CVBS _panel_instance;

    LGFX(void)
    {
        {
            auto cfg = _panel_instance.config();

            cfg.memory_width = 480;
            cfg.memory_height = 272;

            cfg.panel_width = 480;
            cfg.panel_height = 270;

            cfg.offset_x = 0;
            cfg.offset_y = 1;

            _panel_instance.config(cfg);
        }

        {
            auto cfg = _panel_instance.config_detail();

            cfg.signal_type = cfg.signal_type_t::NTSC; // NTSC | NTSC_J | PAL | PAL_M | PAL_N
            cfg.pin_dac = 26;                          // 25 | 26
            cfg.use_psram = 0;                         // 0=!PSRAM | 1=PSRAM/SRAM | 2=PSRAM
            cfg.output_level = 128;                    // Brightness
            cfg.chroma_level = 128;                    // Saturation

            // Reading PSRAM task priority
            // cfg.task_priority = 25;

            // Core that executes the task of reading PSRAM
            // in the background (APP_CPU_NUM | PRO_CPU_NUM)
            // cfg.task_pinned_core = PRO_CPU_NUM;

            _panel_instance.config_detail(cfg);
        }

        setPanel(&_panel_instance);
    }
};