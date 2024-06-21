 #include "DisplayController.h"


SPIClass display_spi(HSPI);
SPIClass touch_spi(HSPI);
Adafruit_ILI9341 tft(&display_spi, DC, HSPICS1, RST);
XPT2046_Touchscreen touch(&touch_spi, HSPICS2, -1);


void DisplaySetup(){
  pinMode(BACKLIGHT, OUTPUT);
  digitalWrite(BACKLIGHT, HIGH); //turn off the display backlight
  // ledcSetup(0, 100, 8); // 100 Hz PWM, 8-bit resolution
  // ledcAttachPin(BACKLIGHT, 0); // assign a led pins to a channel
  // ledcWrite(0, 0);
  
//start up the spi buss
  display_spi.begin(HSPICLK, HSPI_MISO, HSPI_MOSI, HSPICS1); //SCLK, MISO, MOSI, SS
  touch_spi.begin(HSPICLK, HSPI_MISO, HSPI_MOSI, HSPICS2);
  tft.begin(HSPI_FREQUENCY);

// read diagnostics (optional but can help debug problems)
  uint8_t x = tft.readcommand8(ILI9341_RDMODE);
  Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDMADCTL);
  Serial.print("MADCTL Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDPIXFMT);
  Serial.print("Pixel Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDIMGFMT);
  Serial.print("Image Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDSELFDIAG);
  Serial.print("Self Diagnostic: 0x"); Serial.println(x, HEX); 


//startup the touchscreen
  touch.begin();
}

void DisplayRestart(){tft.begin(HSPI_FREQUENCY);}

uint16_t TS_X_MIN = 340;
uint16_t TS_X_MAX = 3930;
uint16_t TS_Y_MIN = 190;
uint16_t TS_Y_MAX = 3880;
uint16_t TS_Z_MIN = 0;
uint16_t TS_Z_MAX = 3200;

void GetTouchPoint(TS_Point* p){
/*
  if(p->x > X_MAX)
    X_MAX = p->x;
  if(p->x < X_MIN)
    X_MIN = p->x;
    
  if(p->y > Y_MAX)
    Y_MAX = p->y;
  if(p->y < Y_MIN)
    Y_MIN = p->y;
    
  if(p->z > Z_MAX){
    Z_MAX = p->z;
    Serial.print("+");Serial.println(p->z);
    }
  if(p->z < Z_MIN){
    Z_MIN = p->z;
    Serial.print("-");Serial.println(p->z);
    }
 */
  //calerbrate touch screen here.
  p->x = map(p->x, TS_X_MAX, TS_X_MIN, 0, 320);
  p->y = map(p->y, TS_Y_MAX, TS_Y_MIN, 0, 240);
  if(p->z > TS_Z_MAX)
    p->z = TS_Z_MIN;
  p->z = map(p->z, TS_Z_MIN, TS_Z_MAX, 0, 255);
}
