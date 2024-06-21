#pragma once
#ifndef DISPLAYCONTROLLER_H
#define DISPLAYCONTROLLER_H

#include "custom_pins.h"
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <XPT2046_Touchscreen.h>

extern SPIClass display_spi;
extern SPIClass touch_spi;
extern Adafruit_ILI9341 tft;
extern XPT2046_Touchscreen touch;

void DisplaySetup();
void DisplayRestart();
void SetCalabration(uint16_t x_min, uint16_t x_max, uint16_t y_min, uint16_t y_max, uint8_t z_min, uint8_t z_max);
void GetTouchPoint(TS_Point* p);

extern uint16_t TS_X_MIN;
extern uint16_t TS_X_MAX;
extern uint16_t TS_Y_MIN;
extern uint16_t TS_Y_MAX;
extern uint16_t TS_Z_MIN;
extern uint16_t TS_Z_MAX;


#endif
