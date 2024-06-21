#ifndef PID_DIALOG_H
#define PID_DIALOG_H
#include "mygui/dialog_base.h"
#include "mygui/dialog_button.h"
#include "mygui/dialog_keyboard.h"

#include "command_interface.h"

#include "mygui/pid.h"
//#include "mygui/config.h"
//#include "mygui/MAX31855.h"

extern PID_CONTROLLER pid;
//extern MAX31855 tc;

extern float settemp_holder;
extern float i_holder;
void SetTempCallback(void* value);
void SetICallback(void * value);

class MODE_BUTTON:public DIALOG_BUTTON_TEXT {
public:
  char buffer[20];
  MODE_BUTTON( _DIALOG_BUTTON_TEXT_PRAM ) : _DIALOG_BUTTON_TEXT_CALL { 
    init(); 
    string = buffer;
  }

  void init(){
    std::string str;
    str = "Mode: " + cmd.sendCommand(CMD_MODE);
    strncpy( buffer, str.c_str(), 20);
    //string = pid.mode==PID?"Mode: PID":"Mode: Manual";
  };

  void onClick(DIALOG* p){
    PROFILE_START
    std::string s;
    s = "Mode: " + cmd.sendCommand(CMD_MODENEXT);
    strncpy( buffer, s.c_str(), 20);
    Draw(0);
    PROFILE_END
  }
};

class UNIT_BUTTON: public DIALOG_BUTTON_TEXT {
  public:
    UNIT_BUTTON( _DIALOG_BUTTON_TEXT_PRAM) : _DIALOG_BUTTON_TEXT_CALL { string = buffer; init();}
    char buffer[15];
    void init(){
      PROFILE_START
      std::string s;
      s = cmd.sendCommand(CMD_UNIT);
      strncpy( buffer, s.c_str(), 15);
      Draw(0);
      PROFILE_END
    }

    void onClick(DIALOG* p){
      PROFILE_START
      cmd.sendCommand(CMD_UNITNEXT);
      init();
      EVENT_QUEUE e;
      e.e = KEY_OK;
      p->Event_handle(e);
      PROFILE_END
    }
};

class PID_CONFIG_DIALOG:public DIALOG{
  public: 
    PID_CONFIG_DIALOG(){}
    EVENT_RETURN Event_handle(EVENT_QUEUE e); 

    MODE_BUTTON                MODE{          0,   0, 150, 40, ILI9341_DARKGREEN, ILI9341_BLUE, ""  , 2, ILI9341_RED};  
    DIALOG_BUTTON_INPUT_BOOL   ONOFFBUTTON{   0,  50, 150, 40, ILI9341_GREEN,      ILI9341_RED, "Heater: " , 2, 0xFFFF, 0, &pid.active};
    DIALOG_BUTTON_INPUT_FLOAT  SETTEMPBUTTON{ 0, 100, 150, 40, ILI9341_GREEN, ILI9341_RED,      "T: "      , 2, 0xFFFF, &SetTempCallback, &settemp_holder};
    DIALOG_BUTTON_CLOSE_DIALOG CLOSE{         0, 160,  70, 40, ILI9341_BLUE,  ILI9341_PINK, "Back", 2, 0xFFFF};
    UNIT_BUTTON                UNITS{        80, 160,  70, 40, ILI9341_BLUE,  ILI9341_PINK, "", 1, 0xFFFF};

    DIALOG_BUTTON_INPUT_FLOAT buttonr{     170,   0, 150, 40, ILI9341_GREEN, ILI9341_BLUE,     "R: "      , 2, 0xFFFF, 0, &pid.max_rate_of_change};
    DIALOG_BUTTON_INPUT_FLOAT buttonp{     170,  50, 150, 40, ILI9341_GREEN, ILI9341_RED,      "P: "      , 2, 0xFFFF, 0, &pid.pid.P};
    DIALOG_BUTTON_INPUT_FLOAT buttoni{     170, 100, 150, 40, ILI9341_GREEN, ILI9341_RED,      "I: "      , 2, 0xFFFF, &SetICallback, &pid.pid.I};
    DIALOG_BUTTON_INPUT_FLOAT buttond{     170, 150, 150, 40, ILI9341_GREEN, ILI9341_RED,      "D: "      , 2, 0xFFFF, 0, &pid.pid.D};
  
};

class temp_up_arrow: public DIALOG_BUTTON_ARROW{
  public:
    temp_up_arrow( _DIALOG_BUTTON_ARROW_PRAM ): _DIALOG_BUTTON_ARROW_CALL {}
    void onClick(DIALOG* p){
      PROFILE_START
      pid.set_temp( pid.GetTarget() + 1);
      EVENT_QUEUE e;
      e.e = KEY_OK;
      p->Event_handle(e);
      PROFILE_END
    }
};

class temp_down_arrow: public DIALOG_BUTTON_ARROW{
  public:
    temp_down_arrow( _DIALOG_BUTTON_ARROW_PRAM ): _DIALOG_BUTTON_ARROW_CALL {}
    void onClick(DIALOG* p){
      PROFILE_START
      pid.set_temp( pid.GetTarget() - 1);
      EVENT_QUEUE e;
      e.e = KEY_OK;
      p->Event_handle(e);
      PROFILE_END
    }
};

class PID_MODE_DIALOG: public DIALOG{
  public:
    PID_MODE_DIALOG(){  
      settemp_holder = pid.GetSetpoint();
    }
    EVENT_RETURN Event_handle(EVENT_QUEUE e);
    char current_temp_string[10];   
    DIALOG_BUTTON_INPUT_FLOAT  SETTEMPBUTTON{ 0,   0, 160, 60, ILI9341_BLACK,     ILI9341_DARKGREY, "Temp: "   , 2, 0xFFFF, &SetTempCallback, &settemp_holder};
    DIALOG_BUTTON_INPUT_BOOL   ONOFFBUTTON{   0,  70, 160, 60, ILI9341_BLACK,     ILI9341_DARKGREY, "Heater: " , 2, 0xFFFF, 0, &pid.active};
    temp_up_arrow              temp_up{     200,   0,  90, 90, ILI9341_DARKGREEN, ILI9341_BLUE, 2};
    temp_down_arrow            temp_down{   200, 110,  90, 90, ILI9341_DARKGREEN, ILI9341_BLUE, 0};    
    DIALOG_BUTTON_CLOSE_DIALOG CLOSE{         0, 160,  70, 40, ILI9341_BLUE,      ILI9341_PINK, "Back", 2, 0xFFFF};
    UNIT_BUTTON                UNITS{        80, 160,  70, 40, ILI9341_BLUE,  ILI9341_PINK, "", 1, 0xFFFF};
};

#endif
