#ifndef CONFIG_DIALOG_H
#define CONFIG_DIALOG_H
#include "mygui/dialog_base.h"
#include "mygui/dialog_button.h"
#include "mygui/dialog_keyboard.h"

#include "command_interface.h"
#include "mygui/config.h"
#include "WiFi.h"

#include "config_wifi_dialog.h"

extern mygui GUI;

class WIFI_BUTTON: public DIALOG_BUTTON_TEXT{
  public:
  WIFI_BUTTON ( _DIALOG_BUTTON_TEXT_PRAM ) : _DIALOG_BUTTON_TEXT_CALL {};
  void onClick(DIALOG* d){
    OpenChildDialog<WIFI_DIALOG> (this, d);
  }
};

class SAVE_CONFIG_BUTTON: public DIALOG_BUTTON_TEXT{
  public:
    SAVE_CONFIG_BUTTON ( _DIALOG_BUTTON_TEXT_PRAM ) : _DIALOG_BUTTON_TEXT_CALL {};
    void onClick(DIALOG* d){
      config_file_save();
    }
};

class CONFIG_DIALOG:public DIALOG{
  public: 
    EVENT_RETURN Event_handle(EVENT_QUEUE e); 

    DIALOG_BUTTON_INPUT_INT8 DISPLAY_TIMEOUT{  0,   0, 150, 40, ILI9341_DARKGREEN, ILI9341_BLUE, "Time Out: "  , 2, ILI9341_RED, 0, &config_file.screen_timeout};
    WIFI_BUTTON                  WIFIBUTTON{   0,  50, 150, 40, ILI9341_DARKGREEN, ILI9341_BLUE, "Wifi",         2, 0xFFFF}; 
    SAVE_CONFIG_BUTTON           SAVE{         0, 100, 150, 40, ILI9341_DARKGREEN, ILI9341_BLUE, "Save\nSettings",2, 0xFFFF};
    DIALOG_BUTTON_CLOSE_DIALOG   CLOSE{        0, 150,  70, 40, ILI9341_BLUE,      ILI9341_PINK, "Back", 2, 0xFFFF};
    
};

#endif
