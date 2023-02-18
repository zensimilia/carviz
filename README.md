# Carviz

A real-time audio spectrum analyzer with visual demo scenes and composite video out (8bit) based on ESP32 mocrocontroller.

## Refs and useful links

- https://github.com/espressif/arduino-esp32/tree/master/libraries/ESP32/examples/I2S/HiFreq_ADC
- https://github.com/atomic14/esp32_audio/tree/master/i2s_sampling/src
- https://github.com/donnersm/FFT_ESP32_Analyzer
- https://github.com/Roger-random/ESP_8_BIT_composite
- https://github.com/kosme/arduinoFFT
- https://github.com/adafruit/Adafruit-GFX-Library
- https://github.com/lovyan03/LovyanGFX/blob/master/doc/Panel_CVBS.md
- https://lovyangfx.readthedocs.io/en/latest/index.html
- https://github.com/AlexGyver/FHTSpectrumAnalyzer
- https://github.com/s-marley/ESP32_FFT_VU
- https://github.com/G6EJD/ESP32-8-Octave-Audio-Spectrum-Display
- https://www.toptal.com/embedded/esp32-audio-sampling
- https://forum.arduino.cc/t/how-to-get-esp32-rtos-to-do-fast-analogreads-as-well-as-blink-the-led/877199/32
- https://github.com/cskarai/ledstriporgan

## Utils

Create a hex dump of a given file in C format include file style:

```
xxd -i [in_file] > [out_file.cpp]
```

Convert images for use with TFT: https://lvgl.io/tools/imageconverter
