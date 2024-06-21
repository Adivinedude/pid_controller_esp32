#ifndef CONFIG_H
#define CONFIG_H
#include <string>

struct pid_settings{
  float P;
  float I;
  float D;
  float R;
  float MAXI;
  uint8_t mode;
  uint32_t cycle;
  float freq;
};

struct thermocouple_settings{
  float temp_offset;
  float temp_corrention;
};

struct settings_object{
  uint8_t screen_timeout;
  bool autoconnect_to_wifi; 
  char ssid[32];
  char pass[64];
  uint8_t units;
  pid_settings pid_s[3];
  thermocouple_settings tc_s[3];
  /*    
  bool use_dhcp;
  uint32_t ipaddress;
  uint32_t subnet;
  uint32_t dns;
  uint32_t gateway;
  */
};

void config_file_load();
void config_file_save();

extern settings_object config_file;

#endif
