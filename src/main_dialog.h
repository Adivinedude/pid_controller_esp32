#ifndef MAIN_DIALOG_H
#define MAIN_DIALOG_H

#include "src/gui.h"
#include "src/dialog_keyboard.h"
#include "src/pid.h"

#include "command_interface.h"

//add dialogs that will be loaded from the push buttons here
#include "pid_dialog.h"
#include "profile_dialog.h"
#include "config_dialog.h"


extern char status_buffer[];
///////////////////////////////////  
//STATUS BAR  
class MAIN_DIALOG:public DIALOG{
  public: 
    EVENT_RETURN Event_handle(EVENT_QUEUE e);  
    DIALOG_TEXT STATUS_BAR{ 0, 0, 320, 40, status_buffer, 2, 0xFFFF};
};

// BUTTONS

///////////////////////////////////
// pid button
class PID_MODE: public DIALOG_BUTTON_TEXT{
  public:
  PID_MODE ( _DIALOG_BUTTON_TEXT_PRAM ) : _DIALOG_BUTTON_TEXT_CALL {};
  void onClick(DIALOG* d){
    OpenChildDialog<PID_MODE_DIALOG> (this, d);
  }
};

///////////////////////////////////
// pid config
class PID_CONFIG: public DIALOG_BUTTON_TEXT{
  public:
  PID_CONFIG ( _DIALOG_BUTTON_TEXT_PRAM ) : _DIALOG_BUTTON_TEXT_CALL {};
  void onClick(DIALOG* d){
    OpenChildDialog<PID_CONFIG_DIALOG> (this, d);
  }
};

///////////////////////////////////
// profile mode
class PROFILE_MODE: public DIALOG_BUTTON_TEXT{
  public:
  PROFILE_MODE ( _DIALOG_BUTTON_TEXT_PRAM ) : _DIALOG_BUTTON_TEXT_CALL {};
  void onClick(DIALOG* d){
    OpenChildDialog<PROFILE_DIALOG> (this, d);
  }
};

///////////////////////////////////
// profile manager
class PROFILE_MANAGER: public DIALOG_BUTTON_TEXT{
  public:
  PROFILE_MANAGER ( _DIALOG_BUTTON_TEXT_PRAM ) : _DIALOG_BUTTON_TEXT_CALL {};
  void onClick(DIALOG* d){
    OpenChildDialog<PROFILE_MANAGER_DIALOG> (this, d);
  }
};

///////////////////////////////////
// config menu
class CONFIG_MENU: public DIALOG_BUTTON_TEXT{
  public:
  CONFIG_MENU ( _DIALOG_BUTTON_TEXT_PRAM ) : _DIALOG_BUTTON_TEXT_CALL {};
  void onClick(DIALOG* d){
    OpenChildDialog<CONFIG_DIALOG> (this, d);
  }
};

/* main menu setup
little math for the layout
 320x200  93x  X: 10, 113, 206
          85y  Y: 10, 95
10 + 93 + 10 + 93 + 10
*/

#define MM_COL_1 10
#define MM_COL_2 113
#define MM_COL_3 216
#define MM_ROW_1 10
#define MM_ROW_2 95

class MAIN_MENU:public DIALOG{
  public: 
    EVENT_RETURN Event_handle(EVENT_QUEUE e); 
    
    PID_MODE        pid_mode{       MM_COL_1,  MM_ROW_1, 93, 85, ILI9341_RED, ILI9341_DARKGREEN, "PID\nMode",        2, 0xFFFF}; 
    PID_CONFIG      pid_config{     MM_COL_1,  MM_ROW_2, 93, 85, ILI9341_RED, ILI9341_DARKGREEN, "PID\nConfig",      2, 0xFFFF};
    PROFILE_MODE    profile_mode{   MM_COL_2,  MM_ROW_1, 93, 85, ILI9341_RED, ILI9341_DARKGREEN, "Profile\nMode",    2, 0xFFFF};
    PROFILE_MANAGER profile_manager{MM_COL_2,  MM_ROW_2, 93, 85, ILI9341_RED, ILI9341_DARKGREEN, "Profile\nManager", 2, 0xFFFF};
    CONFIG_MENU     config_menu{    MM_COL_3,  MM_ROW_1, 93, 85, ILI9341_RED, ILI9341_DARKGREEN, "Config",           2, 0xFFFF};   
    
};

/////////////////////////////////////////
#endif
