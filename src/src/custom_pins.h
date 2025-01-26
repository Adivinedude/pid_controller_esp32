#pragma once
#ifndef CUSTOM_PINS_H
#define CUSTOM_PINS_H

  #include "my_debug.h"

  //#define use_jtag

  //esp32 wrover custom pinout
  //High Speed SPI buss
  #define HSPICS0 15 //SD Card      3-9       //jtag
  #define HSPICS1 27 //LCD Display  4-2
  #define HSPICS2 26 //Touch Screen 2-3
  #define HSPICLK 14             // 1-7 TMS   //jtag 
  #define HSPI_MOSI 13  //D_IN      2-9 TCK   //jtag
  #define HSPI_MISO 34  //D_Out    1-6 
  #define HSPI_FREQUENCY 40000000

  #define VSPICS0 22 //Temp Sensor 0
  #define VSPICS1 4  //Temp Sensor 1
  #define VSPICS2 21 //Temp Sensor 2
  #define VSPICLK 18
  #define VSPI_MISO 39
  #define VSPI_MOSI 35

  //General Usage
  #define OUT_0 12 //Output Channel 0         //jtag
  #define OUT_1 19 //Output Channel 1
  #define OUT_2 23 //Output Channel 2
  #define BACKLIGHT 25 //LED Display Backlight  //4-3
  #define RST 32   //LED Display Reset/Initalize  4-4
  #define DC 33    //LED Display D/C pin          1-5
  #define T_IRQ 36 //Touch Screen IRQ        VP   2-2
  #define GPI02 2
  #define LED 2    //LED
  #define GPIO0 0
  #define GPIO5 5
  #define RX0 3
  #define TX0 1

  #ifdef use_jtag
    #undef HSPICS0
    #define HSPICS0 19//15 //SD Card      3-9       //jtag
    #undef HSPICLK
    #define HSPICLK 32//14             // 1-7 TMS   //jtag 
    #undef HSPI_MOSI
    #define HSPI_MOSI 25//13  //D_IN      2-9 TCK   //jtag
    #undef OUT_0
    #define OUT_0 32//12 //Output Channel 0         //jtag
  #endif
#endif
