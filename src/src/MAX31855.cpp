#include "MAX31855.h"

float convert_temp_units( float value, uint8_t to, uint8_t from){
	switch(from){
		case CELSIUS:
			break;
		case FAHRENHEIT:
			value = (value-32) / 1.8;
			break;
		case KELVIN:
		  value -= 273.15;
      break;
    default:
      return NAN;
	}
  switch(to){
    case CELSIUS:
      return value;
    case FAHRENHEIT:
      return (value * 1.8) + 32;
    case KELVIN:
      return value + 273.15;
    default:
      return NAN;
  }
}

MAX31855::MAX31855( SPIClass* spi_buss, uint8_t cs){
  Begin( spi_buss, cs);
}

void MAX31855::Begin(SPIClass* spi_buss, uint8_t cs){
  _spi = spi_buss;
  _cs = cs;
  last_reading = 0;
  pinMode( _cs, OUTPUT);
  digitalWrite( _cs, HIGH);
  unit_type = CELSIUS;  
}

void MAX31855::Update(){
  //static portMUX_TYPE my_mutex;
  //ToDo: Lock spi buss for thread saftey
  uint32_t t = millis();
  if( t - read_timer < MAX31855_READ_DELAY )
    return;
  read_timer = t;
  //portENTER_CRITICAL(&my_mutex);
  _spi->beginTransaction(SPISettings(MAX31855_SPI_FREQUENCY, MSBFIRST, MAX31855_SPI_MODE));
  digitalWrite(_cs, LOW);
  last_reading = _spi->transfer32(0);
  digitalWrite(_cs, HIGH);
  _spi->endTransaction();
  //portEXIT_CRITICAL(&my_mutex);

}

float MAX31855::Read(uint8_t u){
  /*
  1098 7654 3210 9876 5432 1098 7654 3210
  -xxx xxxx xxxx xx-- ---- ---- ---- ----
  0111 1111 1111 1100 
    7    F    F   C     0    0    0    0 
  1000
    8
  */
  float t;

  //check error bit
  if( last_reading & 0x07 )
    return MAX31855_ERROR;

  if( last_reading & 0x80000000){
    t = (int32_t) ( (last_reading >> 18) & 0x00003fff ) | 0xffffc0000;
  }else{
    t = last_reading >> 18;
  }
  t *= MAX31855_EXTERNAL_TEMP_FACTOR;
  t += offset;
  int f;
  if(u){
    f = u;
  }else{
    f = unit_type;
  }
  switch( f ){
    case FAHRENHEIT:
      return t * 1.8 + 32;
    case KELVIN:
      return t + 273.15;
  }
  return t;
}

float MAX31855::ReadInternal(uint8_t u){
  /*
  1098 7654 3210 9876 5432 1098 7654 3210
  ---- ---- ---- ---- -xxx xxxx xxxx ----
    0    0    0    0    7    F    F   0
                      x--- ---- ---- ----
                        8    0    0   0
  */
  float t;
  //if data is signed
  if( last_reading & 0x00008000){
    t = (int16_t) 0xf800 | ( (last_reading >> 4) & 0x07FF );
  }else{
    t = (last_reading >> 4) & 0x000007FF;
  }

  t *= MAX31855_INTERNAL_TEMP_FACTOR;
    
  int f;
  if(u){
    f = u;
  }else{
    f = unit_type;
  }
  switch( f ){
    case FAHRENHEIT:
      return t * 1.8 + 32;
    case KELVIN:
      return t + 273.15;
  }
  return t;
}

void MAX31855::Format(uint8_t f){
  if(f > KELVIN){
    LOG("unit is invalad.")
    unit_type = CELSIUS;   
    return;
  }
  unit_type = f; 
}

uint8_t MAX31855::error(){
/*
  1098 7654 3210 9876 5432 1098 7654 3210
  ---- ---- ---- ---- ---- ---- ---- -xxx
    0    0    0    0    0    0    0    7
  */
  return last_reading & 0x00000007;
}

const char* MAX31855::GetError(){
  switch( error() ){
    case MAX31855_ERROR_NONE:
      return "";
    case MAX31855_ERROR_OPEN_CIRCUIT:
      return "Open Circuit";
    case MAX31855_ERROR_SHORT_GND:
      return "Ground Short";
    case MAX31855_ERROR_SHORT_VCC:
      return "Vcc Short";
  }
  return "Unknown Error";
}