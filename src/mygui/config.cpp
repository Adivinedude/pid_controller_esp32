#include "config.h"
#include <EEPROM.h>
#include "pid.h"
settings_object config_file;

void config_file_load(){
  int address = 0;
 
  if( !EEPROM.begin(sizeof(config_file)+1) ) {
    Serial.println("Failed to initialise EEPROM");
    Serial.println("Restarting...");
    delay(5000);
    ESP.restart();
  }
  memset(&config_file, 0x00, sizeof(config_file));
  
  //read test byte
  if( EEPROM.readByte(address) != 0x01 ){
    Serial.println("No configs in EEPROM");
    return; //no configs present.
  }
  address += 1;
  
  unsigned char* p = (unsigned char*)&config_file;
  for( int a = sizeof(config_file); a; a--, address++, p++)
    *p = EEPROM.readByte(address);  
};

void config_file_save(){

  //load current pid values into config_file
  for(int a = 0; a < 3; a++){
    config_file.pid_s[a].P = pid.pid.P;
    config_file.pid_s[a].I = pid.pid.I;
    config_file.pid_s[a].D = pid.pid.D;
    config_file.pid_s[a].R = pid.max_rate_of_change;
    config_file.pid_s[a].cycle = pid.time_cycle_length;
    config_file.pid_s[a].freq = pid._output_frequency;
    //config_file.pid_s[a].MAXI = pid.pid.maxIOutput;
  }

  int address = 0;
  //write test byte
  if( EEPROM.readByte(address) != 0x01 )
    EEPROM.writeByte(address, 0x01);
  
  address += 1;
  
  unsigned char* p = (unsigned char*)&config_file;
  for( int a = sizeof(config_file); a; a--, address++, p++){
    if( EEPROM.readByte(address) != *p)
      EEPROM.writeByte(address, *p);
  }
  EEPROM.commit();
};
