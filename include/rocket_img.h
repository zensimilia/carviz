#pragma once

#include <pgmspace.h>

// https://javl.github.io/image2cpp/
// Horizontal
// 96x54px
const unsigned char rocket_img[] PROGMEM = {
    0x00, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xc0, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x33, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33, 0xff, 0xff, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xcc, 0xcf, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xcc, 0xcf, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x33, 0x3f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33, 0x3f, 0xf0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0xcf, 0xfc, 0x03, 0xff, 0xff, 0xff,
    0xff, 0x00, 0x00, 0x00, 0x00, 0x0c, 0xcf, 0xfc, 0x03, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00,
    0x00, 0x03, 0x33, 0xf0, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x03, 0x33, 0xf0,
    0x3f, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0xcc, 0x0f, 0x3f, 0x3f, 0xff, 0xff,
    0xf3, 0xf3, 0xc0, 0x00, 0x00, 0x00, 0xcc, 0x0f, 0x3f, 0x3f, 0xff, 0xff, 0xf3, 0xf3, 0xc0, 0x00,
    0x00, 0x00, 0x30, 0xff, 0x3c, 0xcf, 0xff, 0xff, 0xcc, 0xf3, 0xf0, 0x00, 0x00, 0x00, 0x30, 0xff,
    0x3c, 0xcf, 0xff, 0xff, 0xcc, 0xf3, 0xf0, 0x00, 0x00, 0x00, 0x0f, 0xff, 0x3f, 0x3f, 0xc0, 0x0f,
    0xf3, 0xf3, 0xfc, 0x00, 0x00, 0x00, 0x0f, 0xff, 0x3f, 0x3f, 0xc0, 0x0f, 0xf3, 0xf3, 0xfc, 0x00,
    0x00, 0x00, 0xff, 0xff, 0x3f, 0xff, 0x00, 0x03, 0xff, 0xf3, 0xff, 0x00, 0x00, 0x00, 0xff, 0xff,
    0x3f, 0xff, 0x00, 0x03, 0xff, 0xf3, 0xff, 0x00, 0x00, 0x3f, 0xff, 0xff, 0x3f, 0xfc, 0x00, 0x30,
    0xff, 0xf3, 0xff, 0xc0, 0x00, 0x3f, 0xff, 0xff, 0x3f, 0xfc, 0x00, 0x30, 0xff, 0xf3, 0xff, 0xc0,
    0x00, 0x00, 0x00, 0x0f, 0x3f, 0xfc, 0x00, 0xc0, 0xff, 0xf3, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x0f,
    0x3f, 0xfc, 0x00, 0xc0, 0xff, 0xf3, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0x3f, 0xfc, 0x03, 0x0c,
    0xff, 0xf3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0xfc, 0x03, 0x0c, 0xff, 0xf3, 0xff, 0xff,
    0x0f, 0xff, 0xff, 0xff, 0x3f, 0xfc, 0x0c, 0x30, 0xff, 0xf3, 0xff, 0x30, 0x0f, 0xff, 0xff, 0xff,
    0x3f, 0xfc, 0x0c, 0x30, 0xff, 0xf3, 0xff, 0x30, 0x00, 0xfc, 0xff, 0xff, 0x3f, 0xfc, 0x00, 0xc0,
    0xff, 0xf3, 0xfc, 0xc0, 0x00, 0xfc, 0xff, 0xff, 0x3f, 0xfc, 0x00, 0xc0, 0xff, 0xf3, 0xfc, 0xc0,
    0x00, 0x0f, 0x33, 0xff, 0x3f, 0xff, 0x00, 0x03, 0xff, 0xf3, 0x33, 0x00, 0x00, 0x0f, 0x33, 0xff,
    0x3f, 0xff, 0x00, 0x03, 0xff, 0xf3, 0x33, 0x00, 0x00, 0x00, 0xcc, 0xcf, 0x3f, 0x3f, 0xc0, 0x0f,
    0xf3, 0xf3, 0xcc, 0x00, 0x00, 0x00, 0xcc, 0xcf, 0x3f, 0x3f, 0xc0, 0x0f, 0xf3, 0xf3, 0xcc, 0x00,
    0x00, 0x00, 0x33, 0x33, 0x3c, 0x0f, 0xff, 0xff, 0xc0, 0xf0, 0x30, 0x00, 0x00, 0x00, 0x33, 0x33,
    0x3c, 0x0f, 0xff, 0xff, 0xc0, 0xf0, 0x30, 0x00, 0x00, 0x00, 0xcc, 0xcc, 0x0f, 0x3f, 0xff, 0xff,
    0xf3, 0xc3, 0xc0, 0x00, 0x00, 0x00, 0xcc, 0xcc, 0x0f, 0x3f, 0xff, 0xff, 0xf3, 0xc3, 0xc0, 0x00,
    0x00, 0x03, 0x33, 0x33, 0x33, 0xf3, 0x33, 0x33, 0x3f, 0x3c, 0x00, 0x00, 0x00, 0x03, 0x33, 0x33,
    0x33, 0xf3, 0x33, 0x33, 0x3f, 0x3c, 0x00, 0x00, 0x00, 0x0c, 0xcc, 0xcc, 0xfc, 0xcc, 0xcc, 0xcc,
    0xcc, 0xc0, 0x00, 0x00, 0x00, 0x0c, 0xcc, 0xcc, 0xfc, 0xcc, 0xcc, 0xcc, 0xcc, 0xc0, 0x00, 0x00,
    0x00, 0x33, 0x33, 0x30, 0x03, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x33, 0x33, 0x30,
    0x03, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0xcc, 0xcc, 0xcc, 0xf0, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xcc, 0xcc, 0xcc, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x3f, 0x33, 0x33, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x33, 0x33, 0xc0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xcc, 0xcf, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x03, 0xcc, 0xcf, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x3f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xf0, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
