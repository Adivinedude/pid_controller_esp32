#ifndef COMMAND_INTERFACE_H
#define COMMAND_INTERFACE_H
#include <string>
#include <map>

enum COMMAND_LIST {
  CMD_FIX = 0,
  CMD_CONFIG_LOAD,
  CMD_CONFIG_SAVE,
  CMD_UPDATE,
  CMD_REPORT,
  CMD_DISPLAY,
  CMD_CYCLE,
  CMD_FREQ,
  CMD_ACTIVE,
  CMD_MODE,
  CMD_MODENEXT,
  CMD_SETPOINT,
  CMD_TEMP,
  CMD_RESET,
  CMD_RATE,
  CMD_P,
  CMD_I,
  CMD_MAXI,
  CMD_D,
  CMD_START,
  CMD_NEXT,
  CMD_LOAD,
  CMD_SAVE,
  CMD_DELETE,
  CMD_LIST,
  CMD_PROFILE,
  CMD_SHOW,
  CMD_CLEAR,
  CMD_RESTART,
  CMD_STATUS,
  CMD_UNIT,
  CMD_UNITNEXT,
  CMD_VALUE,
  CMD_COUNT,
  CMD_FINDP,
  CMD_FINDI,
  CMD_FINE_TUNE,
  CMD_FINDD,
  CMD_PLUS,
  CMD_TCOFFSET,
  CMD_TCCORRECTION,
  
  CMD_DEBUG,
  CMD_UNKNOWN
};

char* morphNumericString (char *s, int n);

class command_interface {
  std::string profile;
  uint32_t time_next_command;
  std::map<uint32_t, COMMAND_LIST> command_list_map;
  bool initalized = false;
  void* lable_list[CMD_UNKNOWN+1];
  float saved_values[4];

  
  uint32_t text_to_int(const char* s);
  void add_to_list(const char* s, COMMAND_LIST value);
  
  public:
  std::string sendCommand(std::stringstream &s);
  std::string sendCommand(COMMAND_LIST action, std::stringstream &s);
  std::string sendCommand(COMMAND_LIST action);
  std::string sendCommandLine( const char* str );

  void Begin();
  COMMAND_LIST Text_to_command(const char* s);
};

extern command_interface cmd;
#endif
