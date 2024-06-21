//upload OTA console command
//python /home/cad/.arduino15/packages/esp32/hardware/esp32/1.0.6/tools/espota.py -i 10.0.0.175 -p 3232 --auth= -f /tmp/arduino_build_312612/GUI_TEST.ino.bin 

#define ESP32_RTOS  // Uncomment this line if you want to use the code with freertos only on the ESP32
                    // Has to be done before including "OTA.h"

//#include <WiFi.h>

//#define CONFIG_ARDUINO_LOOP_STACK_SIZE

#include "mygui/config.h"
#include <TelnetStream.h>

#include <Arduino.h>

#include "main_dialog.h"
#include "command_interface.h"
#include "mygui/sd_card_reader.h"
//#include "mygui/thermocouple_array.h"
#include <algorithm>

#include "mygui/MAX31855.h"
#include "command_task.h"
#include "mygui/pid.h"

#include "math.h"

uint32_t serial_update = 0;
volatile uint32_t total_time = 0;
volatile float current_temp_kelvin;
MAX31855 tc;
uint32_t draw_time = 0;
uint32_t serial_update_time = 0;
//uint32_t screen_diangnostic_timer = 0;
volatile bool     send_update = true;
volatile bool     is_wifi_connected = false;
volatile bool     try_to_connect_to_wifi = false;
volatile bool     DEBUG_MODE = true;
volatile bool     DEBUG_MODE_FIRST = true;

extern TaskHandle_t loopTaskHandle;
#define task_handles_size 7
TaskHandle_t task_handles[task_handles_size];

SemaphoreHandle_t SPI_ACCESS = xSemaphoreCreateMutex();

//main visual interface
mygui GUI;
MAIN_DIALOG dialog1;
MAIN_MENU   dialog2;

//this task worker will manage 1 pid controller (can be used for multiple controllers)
void PID_TASK_LOOP(void *pvParameters){
  uint8_t test_byte = 0xA5;
  uint32_t PID_time = 0 ;
  //PID_CONTROLLER* p = &pid;//(PID_CONTROLLER*) pvParameters;
  uint32_t rt;  
  Serial.println("starting pid task");
  delay(5000);

  
#ifdef use_jtag
  float debug_temp = 273.15;
  pid.temp_current = &debug_temp;
#endif

  while(1){
    uint32_t e = millis();
    //TelnetStream.print(".");
    rt = pid.loop(e - PID_time);
    
    PID_time = e;
    
    total_time = pid.GetTime();
    //LOGF("PID rt: %d\n\r", rt);
    if( !pid.active || rt > 500)
      rt = 500;

    if( test_byte != 0xA5){
      Serial.print("PID_TASK_LOOP - Stack Corrupt");
      ESP.restart();
    }
    //LOG(rt)
    if(rt > 1)
      delay(rt);
  }
}

void TEMP_TASK_LOOP(void *pvParameters){
  //setup cs pin
  pinMode(VSPICS0, OUTPUT);
  digitalWrite(VSPICS0, HIGH);
  //start spi interface
  SPIClass vspi = SPIClass(VSPI);
  vspi.begin(VSPICLK, VSPI_MISO, VSPI_MOSI);
  //load  chip interface
  tc.Begin(&vspi, VSPICS0);
  //set unit
  tc.Format(config_file.units);
  //let the chip power up
  delay(1000);

  #define SAMPLE_ARRAY_SIZE 1

  float sample_array[SAMPLE_ARRAY_SIZE];
  float average = 0;
  bool first_run = true;

  for(;;){
    //stop all spi access during tc update
    xSemaphoreTake( SPI_ACCESS, portMAX_DELAY );

    //wait for the bad readings to by cycled out.
    delay(MAX31855_READ_DELAY);

    //take multiple samples from the chip
    for( int a = 0; a != SAMPLE_ARRAY_SIZE; ++a){
      delay(MAX31855_READ_DELAY); //minimum delay between chip readings
      tc.Update();                //read the chip
      if( tc.error() )
        break;
      sample_array[a] = tc.Read(KELVIN);  //store the results
    }
    //return spi access
    xSemaphoreGive( SPI_ACCESS );

    //average the samples to generate the result
    average = 0;
    for(uint8_t a = 0 ; a < SAMPLE_ARRAY_SIZE ; ++a){
      average += sample_array[a];
    }
    average /= SAMPLE_ARRAY_SIZE;
    average *= 4;                 
    average = round(average);     
    average /= 4;                 
    if(first_run){
      if(!tc.error()){
        first_run = false;
      }
    }

    //apply offset and correction
    //average = (average + config_file.tc_s[0].temp_offset) * config_file.tc_s[0].temp_corrention;
    //check for ic defined errors
    pid.error = tc.error(); 
    pid.error_code = tc.GetError();
    if( pid.error )
      TelnetStream.println( tc.GetError() );
    //store the value as kelven
    current_temp_kelvin = average;
    delay(2000);
  }
}

void setup() {

  pinMode(19, OUTPUT);     //led 
  digitalWrite(19, LOW);
  Serial.begin(115200);
  delay(3000);
  
  Serial.println("");
  Serial.print("Software Build Date: ");
  Serial.print(__DATE__);
  Serial.print(" ");
  Serial.println(__TIME__);
  Serial.println("loading EEPROM");
  config_file_load();
  
  //LOG("Start Application\nsd card start\n")
  GUI.Begin();
  sd_card_reader_setup();
  
  pid.temp_current = &current_temp_kelvin;

  pid.setup(&config_file.pid_s[0], OUT_0, 0);
  
  Serial.println("Starting all task");
  for( int a = 0; a != task_handles_size; a++)
    task_handles[a] = 0;
  try_to_connect_to_wifi = config_file.autoconnect_to_wifi;
  task_handles[0] = loopTaskHandle;
  xTaskCreatePinnedToCore(   OTA_TASK_LOOP, "OTA_TASK",    8000, NULL, 2, &task_handles[1], 1);
  xTaskCreatePinnedToCore(SERIAL_TASK_LOOP, "SERIAL_TASK", 8000, NULL, 2, &task_handles[2], 1);
  xTaskCreatePinnedToCore(   PID_TASK_LOOP, "PID_TASK",    8000, NULL, 2, &task_handles[3], 1);
  xTaskCreatePinnedToCore(  TEMP_TASK_LOOP, "TEMP_TASK",   8000, NULL, 2, &task_handles[4], 1);
  xTaskCreatePinnedToCore(TELNET_TASK_LOOP, "TELNET_TASK", 8000, NULL, 2, &task_handles[5], 1);
   
  Serial.println("All task started");
  
  draw_time = millis();
  tft.fillRect(0, 0, 320, 240, 0x0000);
  GUI.addDialog(&dialog1); // add status bar
  GUI.addDialog(&dialog2); // add main menu
  OpenChildDialog<PID_MODE_DIALOG> (&dialog2.pid_mode, &dialog2); //open default screen

  //ledcWrite(0, 255);
  digitalWrite(BACKLIGHT, HIGH);
  cmd.sendCommandLine("startup");
  Serial.println("Setup Complete");
}

void loop(){

  uint32_t _time = millis();
  
  if( _time - draw_time > 100){
    //Serial.print("Loop report ");
    //ReportResources();
    total_time += _time - draw_time;
      
    draw_time = _time;
    char buffer[10];
    snprintf(buffer, 10, "%.02f", convert_temp_units( pid.setpoint, config_file.units, KELVIN ));
    morphNumericString(buffer, 2);
    sprintf(status_buffer, "%s PV: %s\nSV: %s %.0f%%"
      , FormatTimeOutput( &pid ).c_str() 
      , cmd.sendCommand(CMD_TEMP).c_str()
      , buffer
      , pid.power);

    dialog1.STATUS_BAR.Draw(0);

    if(send_update && !(DEBUG_MODE && pid.active) ){
      std::string s = cmd.sendCommandLine("status");
      std::replace( s.begin(), s.end(), '\n', ' ');
      if(_time - serial_update_time > 1000){
        serial_update_time = _time;
        TelnetStream.print('\r');
        TelnetStream.print(s.c_str());
        TelnetStream.print("   ");
      }
      serial_update++;
      if(serial_update > 100){
        serial_update = 0;
        Serial.println(s.c_str());
      }
    }    
  }
  
  GUI.loop();
  
  if( GUI.isEmpty() ){
    LOG("\nNo Dialog Loaded !!!\nRestarting...\n")
    delay(5000);
    ESP.restart();
  }

  delay(10);
 }
