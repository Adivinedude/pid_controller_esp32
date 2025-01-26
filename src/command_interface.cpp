#include <Arduino.h>

#include "src/pid.h"
#include "src/gui.h"
#include "src/sd_card_reader.h"

#include "command_interface.h"
#include "src/config.h"
#include <sstream>
#include <TelnetStream.h>

#include "src/MAX31855.h"

command_interface cmd;

//extern OVEN_SIMULATOR oven;
extern PID_CONTROLLER pid;
extern uint32_t total_time;
extern char status_buffer[];
extern bool volatile send_update;
extern mygui GUI;
extern MAX31855 tc;
extern bool volatile DEBUG_MODE;
extern bool volatile DEBUG_MODE_FIRST;

char* morphNumericString (char *s, int n) {
    char *p;
    int count;
    p = strchr (s,'.');         // Find decimal point, if any.
    if (p != NULL) {
        count = n;              // Adjust for more or less decimals.
        while (count >= 0) {    // Maximum decimals allowed.
             count--;
             if (*p == '\0')    // If there's less than desired.
                 break;
             p++;               // Next character.
        }

        *p-- = '\0';            // Truncate string.
        while (*p == '0')       // Remove trailing zeros.
            *p-- = '\0';

        if (*p == '.') {        // If all decimals were zeros, remove ".".
            *p = '\0';
        }
    }
    p = s;
    if( *p == '-' ){ // check for a negitave sign.
      for( ++p; *p != '\0' && *p == '0'; ++p){} //search the string for non '0' characters
      if( *p == '\0' ){ //string only contains '0'. 
        s[0] = '0';
        s[1] = '\0';
      }
    }
    return s;
}

void command_interface::add_to_list(const char* s, COMMAND_LIST value){
  CMD_PROFILE_START
  uint32_t key = text_to_int(s);
  if(DEBUG_MODE){
    TelnetStream.printf("%s %x", s, key);
  }
  bool success = command_list_map.insert( std::pair<uint32_t, COMMAND_LIST>( key, value) ).second;
  if( success == false ){
    while(1){
      LOGF("ERROR: not all command hashs are unique \"%s\" : %d", s, value)
      delay(5000); 
    }
  }
  CMD_PROFILE_END
}
  
std::string handle_command_uint8_t( std::stringstream & s, uint8_t &i, std::stringstream &rt){
  CMD_PROFILE_START
  uint32_t t;
  uint8_t  u;
  t = i;
  s >> t;
  if(s.fail()){
    rt << t;
    CMD_PROFILE_END
    return rt.str();
  }
  u = t;
  i = u;
  CMD_PROFILE_END
  return "OK";
}

std::string handle_command_uint32_t( std::stringstream & s, uint32_t &i, std::stringstream &rt){
  CMD_PROFILE_START
  uint32_t t;
  s >> t;
  if(s.fail()){
    rt << i;
    CMD_PROFILE_END
    return rt.str();
  }
  i = t;
  CMD_PROFILE_END
  return "OK";
}

std::string handle_command_bool( std::stringstream & s, bool &b, std::stringstream &rt){
  CMD_PROFILE_START
  std::string value;
  s >> value;
  if(s.fail()){
    CMD_PROFILE_END
    return b?"on":"off";;
  }else{
    if(value == "on" || value == "1"){
      b = true;
    }else if( value == "off" || value == "0"){
      b = false;
    }else{
      CMD_PROFILE_END
      return "Error: invalad option";
    }
    CMD_PROFILE_END
    return "OK";
  }
}

std::string handle_command_float( std::stringstream & s, float &f, std::stringstream &rt){
  CMD_PROFILE_START
  float t;
  s >> t;   
  if(s.fail()){
    char buffer[10];
    snprintf(buffer, 10, "%.4f", f);
    rt << morphNumericString(buffer, 4);
    CMD_PROFILE_END
    return rt.str();
  }
  f = t;
  CMD_PROFILE_END
  return "OK";
}

uint32_t command_interface::text_to_int(const char* s){
  CMD_PROFILE_START
  
  unsigned int h;
  unsigned char *p;

  h = 0;
  for (p = (unsigned char*)s; *p != '\0'; p++)
    h = 37 * h + *p;
  
  CMD_PROFILE_LOG(h)
  CMD_PROFILE_END
  return h;
}

std::string command_interface::sendCommandLine( const char* str ){
  CMD_PROFILE_START
  CMD_PROFILE_LOG(str)
  std::string rt;
  std::stringstream s(str);
  while( s ){  
    rt = sendCommand(s);
    if( rt == "")
      break;
    if( rt != "OK"){
      CMD_PROFILE_END
      return rt;
    }
  }
  CMD_PROFILE_END
  return "OK";  
}

std::string command_interface::sendCommand(std::stringstream &s){
  CMD_PROFILE_START
  std::string command;
  s >> command;
  if(s.fail()){
    CMD_PROFILE_END
    return ""; 
  }
  
  COMMAND_LIST a = Text_to_command( command.c_str() );
  CMD_PROFILE_LOG(a)
  command = sendCommand(a, s);
  CMD_PROFILE_LOG(command.c_str())

  CMD_PROFILE_END
  return command;
}

COMMAND_LIST command_interface::Text_to_command( const char* s ){
  CMD_PROFILE_START
  std::map<uint32_t, COMMAND_LIST>::iterator iter;
  iter = command_list_map.find( text_to_int(s) );
  if( iter == command_list_map.end() ){
    CMD_PROFILE_END
    return CMD_UNKNOWN;
  }
  CMD_PROFILE_END
  return iter->second;
}

std::string command_interface::sendCommand(COMMAND_LIST action){
  std::stringstream s;
  return sendCommand(action, s);
}

std::string command_interface::sendCommand(COMMAND_LIST action, std::stringstream &s){
  CMD_PROFILE_START
  std::string       value;
  std::stringstream rt;
  float f;
  bool b = false;
  char c;
  uint32_t i;
  std::string str;

  EXECUTE_ACTION:
  CMD_PROFILE_LOG(initalized)
  if( initalized ){
    CMD_PROFILE_LOG(action)
    goto *(lable_list[action]);
  }
  goto LABLE_INITALIZE;

  LABLE_FIX:
    CMD_PROFILE_LOG("fix")
    GUI.Begin();
    GUI.addEvent(EVENT_DRAW_ALL);
    sendCommandLine("display");
    TelnetStream.println( tc.GetError() );
    return "OK";
  
  LABLE_CONFIG_LOAD:
    CMD_PROFILE_LOG("cfgload")
    config_file_load();
    pid.load_settings( &config_file.pid_s[0] );
    return "OK";
    
  LABLE_CONFIG_SAVE:
    CMD_PROFILE_LOG("cfgsave")
    config_file_save();
    return "OK";

  LABLE_UPDATE:
    CMD_PROFILE_LOG("update")
    b = send_update;
    str = handle_command_bool(s, b, rt);
    if( str == "OK" )
      send_update = b;
    return str;

  LABLE_REPORT:
    CMD_PROFILE_LOG("report")
    ReportResources();
    return "OK";

  LABLE_DISPLAY:
    CMD_PROFILE_LOG("display")
    value = handle_command_uint8_t(s, GUI._brightness, rt);
    //digitalWrite(BACKLIGHT, GUI._brightness > 0?HIGH:LOW);
    //ledcWrite(0, GUI._brightness);
    return value;

  LABLE_CYCLE:
    CMD_PROFILE_LOG("cycle")
    return handle_command_uint32_t(s, pid.time_cycle_length, rt);

  LABLE_FREQ:
    CMD_PROFILE_LOG("freq")
    return handle_command_float(s, pid._output_frequency, rt);
    
  LABLE_ACTIVE:
    CMD_PROFILE_LOG("active")
    return handle_command_bool(s, pid.active, rt);
  
  LABLE_MODE:
    CMD_PROFILE_LOG("mode")
    s >> value;
    if(s.fail()){
      switch(pid.mode){
        case PID:
          rt << "pid";
          break;
        case MANUAL:
          rt << "manual";
          break;
        default:
          rt << pid.mode;
      }
      return rt.str();
    }else{
      if(value == "0" || value == "pid"){
        pid.mode = PID;
      }
      else if(value == "1" || value == "manual"){
        pid.mode = MANUAL;
      }
      else{
        return "Error: invalad option";
      }
      return "OK";
    }
      
  LABLE_MODENEXT:
    CMD_PROFILE_LOG("modeNext")
    if(pid.mode == 0){
      pid.mode = 1;
    }else{
      pid.mode = 0;
    }
    goto LABLE_MODE;
      
  LABLE_SETPOINT:
    CMD_PROFILE_LOG("setpoint")
    f = pid.GetTarget();
    value = handle_command_float(s, f, rt);    
    if(value == "OK")
      pid.set_temp(f);
    return value;
    
  LABLE_TEMP:
    CMD_PROFILE_LOG("temp")
    f = pid.GetPV();
    return handle_command_float(s, f, rt);
  
  LABLE_RESET:
    CMD_PROFILE_LOG("reset")
    f = pid.GetSetpoint();
    b = pid.active;
    //oven.setup();
    pid.setup(0, OUT_0, 0);
    total_time = 0;
    pid.cycle_counter = 0;
    pid.set_temp(f);
    pid.active = b;
    return "OK";
      
  LABLE_RATE:
    CMD_PROFILE_LOG("rate")
    return handle_command_float(s, pid.max_rate_of_change, rt);
  
  LABLE_P:
    CMD_PROFILE_LOG("p")
    f = pid.pid.P;
    str = handle_command_float( s, f, rt);
    if( str == "OK" )
      pid.pid.setP(f);
    return str;
    
  LABLE_I:
    CMD_PROFILE_LOG("i")
    f = pid.pid.I;
    str = handle_command_float( s, f, rt);
    if( str == "OK" )
      pid.pid.setI(f);
    return str;
  LABLE_MAXI:
    CMD_PROFILE_LOG("maxi")
    f = pid.pid.maxIOutput;
    str = handle_command_float( s, f, rt);
    if( str == "OK" )
      pid.pid.setMaxIOutput(f);
    return str; 
  LABLE_D:
    CMD_PROFILE_LOG("d")
    f = pid.pid.D;
    str = handle_command_float( s, f, rt);
    if( str == "OK" )
      pid.pid.setD(f);
    return str;
    
  LABLE_START:
    CMD_PROFILE_LOG("start")
    sendCommandLine("next 5s");
    s >> value;
    if( s.fail() )  
      return "OK";
    str = "load " + value + " next 5s";
    return sendCommandLine( str.c_str() );     
    
  LABLE_NEXT:
    CMD_PROFILE_LOG("next")
    s >> f;
    if(s.fail()){
      s.clear();
      s >> value;
      if(value == "target"){
        pid.time_to_next_command = SETPOINT;
        pid.setpoint_direction = *pid.temp_current > pid.temp_target?false:true;
        return "OK";
      }else if(value == "peak"){
        pid.time_to_next_command = PEAK;
        return "OK";
      }
      pid.time_to_next_command = 1000+pid.time_current;
      return "OK";
    }
    s >> c;
    if(s.fail())
      return "Invalid input";
      //ToDo: use timer properly
    switch(c){
      case 'h':
        pid.time_to_next_command = f*3600000+pid.time_current;
        break;
      case 'm':
        pid.time_to_next_command = f*60000+pid.time_current;
        break;
      case 's':
        pid.time_to_next_command = f*1000+pid.time_current;
        break;
      default:
        return "invalad input";
    }
    return "OK"; 

  LABLE_LOAD:
    CMD_PROFILE_LOG("load")
    s >> value;
    if( s.fail() )
      return "Invalid Profile Name";
    pid.profile = "";
    str = "/profile/" + value + ".txt";
    return readFile(SD, str.c_str(), pid.profile);  
    
  LABLE_SAVE:
    CMD_PROFILE_LOG("save")
    s >> value;
    if( s.fail() )
      return "Invalid Profile Name";
      
    if( !DoesDirExist( SD, "/profile" ) ){
      if( createDir(SD, "/profile") )
        return "Error creating folder for profiles";
    }  
    str = "/profile/" + value + ".txt";
    if(writeFile(SD, str.c_str(), pid.profile.c_str()))
      return "Error writing file";
    return "OK";

  LABLE_DELETE: 
    CMD_PROFILE_LOG("delete")  
    s >> value;
    if( s.fail() )
      return "Invalid Profile Name";
        
    str = "/profile/" + value + ".txt";
    if(deleteFile(SD, str.c_str()))
      return "Error Deleteing file";
    return "OK";

  LABLE_LIST:  
    CMD_PROFILE_LOG("list") 
    return listDirStr(SD, "/profile");   
    
  LABLE_PROFILE:
    CMD_PROFILE_LOG("profile")
    while(!s.fail()){
      pid.profile += value;
      pid.profile += " ";
      s >> value;
    }
    pid.profile += "\n\r";
    return "OK";
    
  LABLE_SHOW:
    CMD_PROFILE_LOG("show")
    return pid.profile;

  LABLE_CLEAR:
    CMD_PROFILE_LOG("clear")
    pid.profile = "";
    pid.time_to_next_command = 0;
    return "OK";
    
  LABLE_RESTART:
    CMD_PROFILE_LOG("restart")
    ESP.restart();

  LABLE_STATUS:
    CMD_PROFILE_LOG("status")
    return status_buffer;
  
  LABLE_UNITNEXT:
    ++config_file.units;
    if( config_file.units > KELVIN)
      config_file.units = CELSIUS;
    tc.Format( config_file.units );
    return "OK";
    
  LABLE_UNIT:
    CMD_PROFILE_LOG("unit")
    s >> c;
    if(s.fail() ){
      switch(config_file.units){
        case CELSIUS: return "Celsius";
        case FAHRENHEIT:return "Fahrenheit";
        case KELVIN:return "Kelvin";
      }
      return "INVALID OPTION";
    }
    switch(c){
      case 'c':
        config_file.units = CELSIUS;
        break;
      case 'f':
        config_file.units = FAHRENHEIT;
        break;
      case 'k':
        config_file.units = KELVIN;
        break;
      default:
        return "invalid option";
    }
    tc.Format( config_file.units );
    return "OK";

  LABLE_VALUE:
    CMD_PROFILE_LOG("value")
    s >> value;
    if(s.fail()){
      for(i = 0; i < 4; ++i)
        rt << "value[" << i << "] = " << saved_values[i] << "\n\r";
      rt << "peak " << pid.peak_wait_value << "\r\n";
      return rt.str();
    }
    str = "OK";
    if(value == "start"){
      saved_values[0] = *pid.temp_current;
    }else if( value == "peak"){
      saved_values[1] = pid.peak_wait_value;
    }else if( value == "cycle"){
      saved_values[2] = pid.cycle_counter;
    }else{
      str = "Invalid Option";
    }
    return str;

  LABLE_FIND_P:
    pid.pid.setP( 100 / abs(saved_values[1] - saved_values[0]) * 2);
    return "OK";

  LABLE_FIND_D:
    pid.pid.setD( pid.pid.errorSum / pid.pid.Ioutput );
    return "OK";

  LABLE_FIND_I:
    if( pid.temp_target > *pid.temp_current )
      pid.pid.setI( abs((pid.temp_target - *pid.temp_current)) / (pid.cycle_counter - saved_values[2]) );
    return "OK";
  
  LABLE_FINE_TUNE:
    if( pid.temp_target > *pid.temp_current){ //not reaching target. increase I
      pid.pid.setI( pid.pid.I * 1.1 + .005 );
      LOG("I+")
    }else if( abs(pid.temp_target - *pid.temp_current) > abs(*pid.temp_current* .01) ){ //overshooting target decrease I
      f = (pid.temp_target - *pid.temp_current) / *pid.temp_current; //percentage of overshoot
      f = abs(f);
      pid.pid.setI( pid.pid.I - pid.pid.I* f); //subtract overshoot from I
      LOG("I-")
    }else if( pid.cycle_counter - saved_values[2] < pid.pid.D ){ // slow to reach target reset D
      pid.pid.setD( pid.cycle_counter - saved_values[2] );
      LOG("D-")
    }

    return "OK";

  LABLE_PLUS:
    CMD_PROFILE_LOG("plus")
    f = 0;
    str = handle_command_float( s, f, rt);
    if( str == "OK" ){
      pid.temp_target = (*pid.temp_current)+f;
    }
    return str;

  LABLE_COUNT:
    CMD_PROFILE_LOG("count")
    rt << pid.cycle_counter;
    return rt.str();

  LABLE_TCOFFSET:
    CMD_PROFILE_LOG("tcoffset")
    f = config_file.tc_s[0].temp_offset;
    str = handle_command_float( s, f, rt);
    if( str == "OK" ){
      config_file.tc_s[0].temp_offset = f;
    }
    return str;

  LABLE_TCCORRECTION:
    CMD_PROFILE_LOG("tccorrection")
    f = config_file.tc_s[0].temp_corrention;
    str = handle_command_float( s, f, rt);
    if( str == "OK" ){
      config_file.tc_s[0].temp_corrention = f;
    }
    return str;    

  LABLE_DEBUG:
    DEBUG_MODE_FIRST = true;
    b = DEBUG_MODE;
    str = handle_command_bool(s, b, rt);
    if( str == "OK" )
      DEBUG_MODE = b;
    return str;

  LABLE_UNKNOWN:
    CMD_PROFILE_LOG("unknown")
    return "unknown command";


  LABLE_INITALIZE:
    CMD_PROFILE_LOG("init of command interface started")
    Begin();
    lable_list[CMD_FIX] = &&LABLE_FIX;
    lable_list[CMD_CONFIG_LOAD] = &&LABLE_CONFIG_LOAD;
    lable_list[CMD_CONFIG_SAVE] = &&LABLE_CONFIG_SAVE;
    lable_list[CMD_UPDATE] = &&LABLE_UPDATE;
    lable_list[CMD_REPORT] = &&LABLE_REPORT;
    lable_list[CMD_DISPLAY] = &&LABLE_DISPLAY;
    lable_list[CMD_CYCLE] = &&LABLE_CYCLE;
    lable_list[CMD_FREQ] = &&LABLE_FREQ;
    lable_list[CMD_ACTIVE] = &&LABLE_ACTIVE;
    lable_list[CMD_MODE] = &&LABLE_MODE;
    lable_list[CMD_MODENEXT] = &&LABLE_MODENEXT;
    lable_list[CMD_SETPOINT] = &&LABLE_SETPOINT;
    lable_list[CMD_TEMP] = &&LABLE_TEMP;
    lable_list[CMD_RESET] = &&LABLE_RESET;
    lable_list[CMD_RATE] = &&LABLE_RATE;
    lable_list[CMD_P] = &&LABLE_P;
    lable_list[CMD_I] = &&LABLE_I;
    lable_list[CMD_MAXI] = &&LABLE_MAXI;
    lable_list[CMD_D] = &&LABLE_D;
    lable_list[CMD_START] = &&LABLE_START;
    lable_list[CMD_NEXT] = &&LABLE_NEXT;
    lable_list[CMD_LOAD] = &&LABLE_LOAD;
    lable_list[CMD_SAVE] = &&LABLE_SAVE;
    lable_list[CMD_DELETE] = &&LABLE_DELETE;
    lable_list[CMD_LIST] = &&LABLE_LIST;
    lable_list[CMD_PROFILE] = &&LABLE_PROFILE;
    lable_list[CMD_SHOW] = &&LABLE_SHOW;
    lable_list[CMD_CLEAR] = &&LABLE_CLEAR;
    lable_list[CMD_RESTART] = &&LABLE_RESTART;
    lable_list[CMD_STATUS] = &&LABLE_STATUS;
    lable_list[CMD_UNIT] = &&LABLE_UNIT;
    lable_list[CMD_UNITNEXT] = &&LABLE_UNITNEXT;
    lable_list[CMD_VALUE] = &&LABLE_VALUE;
    lable_list[CMD_COUNT] = &&LABLE_COUNT;
    lable_list[CMD_FINDP] = &&LABLE_FIND_P;
    lable_list[CMD_FINDI] = &&LABLE_FIND_I;
    lable_list[CMD_FINE_TUNE] = &&LABLE_FINE_TUNE;
    lable_list[CMD_FINDD] = &&LABLE_FIND_D;
    lable_list[CMD_PLUS] = &&LABLE_PLUS;
    lable_list[CMD_TCOFFSET] = &&LABLE_TCOFFSET;
    lable_list[CMD_TCCORRECTION] = &&LABLE_TCCORRECTION;

    lable_list[CMD_DEBUG] = &&LABLE_DEBUG;
    lable_list[CMD_UNKNOWN] = &&LABLE_UNKNOWN;

    initalized = true;

    CMD_PROFILE_LOG("init of command interface finished")

    goto EXECUTE_ACTION;

  return "";
}

void command_interface::Begin(){
  CMD_PROFILE_START
  command_list_map.clear();
  add_to_list( "fix",     CMD_FIX);
  add_to_list( "cfgload", CMD_CONFIG_LOAD);
  add_to_list( "cfgsave", CMD_CONFIG_SAVE);
  add_to_list( "update",  CMD_UPDATE);
  add_to_list( "report",  CMD_REPORT);
  add_to_list( "display", CMD_DISPLAY);
  add_to_list( "cycle",   CMD_CYCLE);
  add_to_list( "freq",    CMD_FREQ);

  add_to_list( "active",  CMD_ACTIVE);
  add_to_list( "a",       CMD_ACTIVE);

  add_to_list( "mode",    CMD_MODE);

  add_to_list( "setpoint",CMD_SETPOINT);
  add_to_list( "t",       CMD_SETPOINT);
  
  add_to_list( "temp",    CMD_TEMP);
  add_to_list( "reset",   CMD_RESET);

  add_to_list( "rate",    CMD_RATE);
  add_to_list( "r",       CMD_RATE);

  add_to_list( "p",       CMD_P);
  add_to_list( "i",       CMD_I);
  add_to_list( "maxi",    CMD_MAXI);
  add_to_list( "d",       CMD_D);
  add_to_list( "start",   CMD_START);
  add_to_list( "next",    CMD_NEXT);
  add_to_list( "load",    CMD_LOAD);
  add_to_list( "save",    CMD_SAVE);
  add_to_list( "delete",  CMD_DELETE);
  add_to_list( "list",    CMD_LIST);
  add_to_list( "profile", CMD_PROFILE);
  add_to_list( "show",    CMD_SHOW);
  add_to_list( "clear",   CMD_CLEAR);
  add_to_list( "restart", CMD_RESTART);
  add_to_list( "status",  CMD_STATUS);
  add_to_list( "unit",    CMD_UNIT);
  add_to_list( "debug",   CMD_DEBUG);
  add_to_list( "value",   CMD_VALUE);
  add_to_list( "count",   CMD_COUNT);
  add_to_list( "findp",   CMD_FINDP);
  add_to_list( "findi",   CMD_FINDI);
  add_to_list( "finetune", CMD_FINE_TUNE);
  add_to_list( "findd",   CMD_FINDD);
  add_to_list( "tempplus",    CMD_PLUS);
  add_to_list( "tcoffset", CMD_TCOFFSET);
  add_to_list( "tccorrect", CMD_TCCORRECTION);

  ReportResources();
  CMD_PROFILE_END
}