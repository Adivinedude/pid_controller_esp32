#ifndef MAX31855_H
#define MAX31855_H
 
#define MAX31855_SPI_FREQUENCY 1000 //4500000 //5mhz max
#define MAX31855_SPI_MODE SPI_MODE0

#define MAX31855_EXTERNAL_TEMP_FACTOR .25
#define MAX31855_INTERNAL_TEMP_FACTOR .0625

#define MAX31855_ERROR -1000

#define MAX31855_ERROR_NONE 0
#define MAX31855_ERROR_OPEN_CIRCUIT 1
#define MAX31855_ERROR_SHORT_GND 2
#define MAX31855_ERROR_SHORT_VCC 4

#define MAX31855_READ_DELAY 200 //100


#define DEFAULT_TEMP_UNIT 0
#define CELSIUS 1
#define FAHRENHEIT 2
#define KELVIN 3

#include <Arduino.h>
#include <SPI.h>
#include "custom_pins.h"

float convert_temp_units( float value, uint8_t to, uint8_t from);

class MAX31855 {
  private:
    uint8_t  unit_type = CELSIUS;
    uint8_t _cs;
    SPIClass* _spi;
    uint32_t read_timer = 0;
    volatile uint32_t last_reading = 0;
    float offset = 0;
 
  public:
    MAX31855(){};
    MAX31855( SPIClass* spi_buss, uint8_t cs);
    void    Begin(SPIClass* spi_buss, uint8_t cs);
    void    Update();  //Read the chip data
    float   Read(         uint8_t u = DEFAULT_TEMP_UNIT); //Get Temp in selected format
    float   ReadInternal( uint8_t u = DEFAULT_TEMP_UNIT); //Get Internal temp
    void    Format(       uint8_t u ); //Set Temp Format
    uint8_t error(); //Get Error Value
    const char* GetError(); //Get error value in readable format
    volatile uint32_t Raw(){return last_reading;}; //get raw ic data

    float   GetOffset(){return offset;};            //Returns the temp offset in celsius
    void    SetOffset(float value){offset = value;};//Set temptaure offset in celsius
};

#endif