#include "command_task.h"
#include "src/gui.h"
#include "src/config.h"
#include "OTA.h"

extern mygui GUI;
extern bool  volatile is_wifi_connected;
extern bool  volatile try_to_connect_to_wifi;


//process commands comming from a stream
void handle_input( Stream &input_stream , uint8_t &counter, char* input_buffer){
  int input;
  std::string s;
  if(counter >= buffer_size){
    counter = 0;   
    input_buffer[0] = 0;
  } 
  
  while( input_stream.available() && counter < buffer_size){
    
    input = input_stream.read();
    //TelnetStream.print( input, HEX);
    switch( input ){
      case '\r':
        //break;
      case '\n':
      /*
        if(DEBUG_MODE){
          char* p = input_buffer;
          TelnetStream.print("Input: ");
          for( uint32_t a = strlen(input_buffer);a < 0; --a, ++p)
            TelnetStream.printf(" %x", *p);
          TelnetStream.println("");
        }
        */
        s = cmd.sendCommandLine(input_buffer);
                
        if( s == "OK" )
          GUI._brightness = 0xff;
        
        TelnetStream.println("");
        TelnetStream.print(input_buffer);
        TelnetStream.print(" ");
        TelnetStream.println(s.c_str());
        //TelnetStream.flush();
        
        Serial.println("");
        Serial.print(input_buffer);
        Serial.print(" ");
        Serial.println(s.c_str());
        counter = buffer_size;

        if( input_stream.available() == 0 )
          GUI.addEvent(KEY_OK);
        

        break;
    case 0x7F: //backspace
      if(counter > 0){
        counter--;
      }
      input_buffer[counter] = 0;
      break;
    default:
      input_buffer[counter] = (char)input;
      input_buffer[counter+1] = 0; 
      counter++;     
    } 
  }
}


//this task will manage OTA updates and network connection
void OTA_TASK_LOOP(void *pvParameters){
  uint8_t test_byte = 0xA5;
  bool first_run = true;

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(1000);
  while(1){
    if( is_wifi_connected == false ){
      if( try_to_connect_to_wifi ){
        int attempt_counter = 10;
        while( is_wifi_connected == false ){
          if( setupOTA("gui_test", config_file.ssid, config_file.pass) ){
            if( attempt_counter == 1 ){
             LOG("too many tries, give up connecting to a network.");
              try_to_connect_to_wifi = false;
              break;
            }
            Serial.printf("Trying again %d\n\r", attempt_counter);
            if( attempt_counter != 0 )
              attempt_counter--;
            delay(10000); 
          }else{
            LOG("Wifi connected\n");
            is_wifi_connected = true;
            if(first_run){
              TelnetStream.begin(23);
              first_run = false;
            }
            digitalWrite(19, HIGH);
          }        
        } // End of  while is wifi connected FALSE
      }else{ //if  try to connect to wifi FALSE
        delay(1000);
      }
    }else{// if is wifi connected TRUE

      ArduinoOTA.handle();
      delay(1000);
    }
    
    if( test_byte != 0xA5){
      Serial.print("OTA_TASK_LOOP - Stack Corrupt");
      delay(10000);
      ESP.restart();
    }
  }
}

//this task will manage network command interface
void TELNET_TASK_LOOP(void *pvParameters){
  uint8_t test_byte = 0xA5;
  uint8_t counter;
  uint8_t status_check_timer = 0;
  char* input_buffer = (char*)ps_malloc(buffer_size+1);

  counter = 0;   
  input_buffer[0] = 0;
  delay(3000);
  while(1){
    
    if( is_wifi_connected == true ){
      handle_input( TelnetStream , counter, input_buffer);

      if( !counter ){
        delay(500);
      }else{
        delay(10);
      }

      if( millis() - status_check_timer > 5000 ){
        status_check_timer = millis();
        if( WiFi.status() != WL_CONNECTED){
          Serial.println("WiFi Disconnected");
          WiFi.disconnect();
          is_wifi_connected = false;
          digitalWrite(19, LOW);
        }
      }
    }
    if( test_byte != 0xA5){
      Serial.print("OTA_TASK_LOOP - Stack Corrupt");
      delay(10000);
      ESP.restart();
    }
  }
  free(input_buffer);
}

//this task will manage serial command interface
void SERIAL_TASK_LOOP(void *pvParameters){
  uint8_t test_byte = 0xA5;
  uint8_t counter;
  //char input_buffer[buffer_size+1];
  char* input_buffer = (char*)ps_malloc(buffer_size+1);
  std::string s;
  
  //Serial.println("starting serial task");
  counter = 0;   
  input_buffer[0] = 0;
  
  while(1){
    handle_input( Serial , counter, input_buffer);
    if( !counter ){
      delay(500);
    }else{
      delay( 100 );
    }
    if( test_byte != 0xA5){
      Serial.print("SERIAL_TASK_LOOP - Stack Corrupt");
      ESP.restart();
    }
  }
  free(input_buffer);
}
