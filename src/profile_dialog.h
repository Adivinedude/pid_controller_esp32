#ifndef PROFILE_DIALOG_H
#define PROFILE_DIALOG_H
#include "src/dialog_base.h"
#include "src/dialog_button.h"
#include "src/dialog_keyboard.h"
#include "src/dialog_listbox.h"

class select_up_arrow: public DIALOG_BUTTON_ARROW{
  public:
    select_up_arrow( _DIALOG_BUTTON_ARROW_PRAM ): _DIALOG_BUTTON_ARROW_CALL {}
    void onClick(DIALOG* p){
      PROFILE_START

      PROFILE_END
    }
};

class select_down_arrow: public DIALOG_BUTTON_ARROW{
  public:
    select_down_arrow( _DIALOG_BUTTON_ARROW_PRAM ): _DIALOG_BUTTON_ARROW_CALL {}
    void onClick(DIALOG* p){
      PROFILE_START

      PROFILE_END
    }
};

class editor_listbox: public DIALOG_LISTBOX{ 
  public:
    editor_listbox( _DIALOG_LISTBOX_PRAM ):_DIALOG_LISTBOX_CALL{}
    
    void onClick(DIALOG* p){
      //strncpy( config_file.ssid, content[selection].c_str(), sizeof(config_file.ssid));
    }

};

class PROFILE_DIALOG:public DIALOG{
  public: 
    EVENT_RETURN Event_handle(EVENT_QUEUE e); 
    select_up_arrow              temp_up{     200,   0,  90, 90, ILI9341_DARKGREEN, ILI9341_BLUE, 2};
    select_down_arrow            temp_down{   200, 110,  90, 90, ILI9341_DARKGREEN, ILI9341_BLUE, 0};    
    editor_listbox               NETWORKLIST{  0,   0, 150, 150, ILI9341_DARKGREEN, ILI9341_BLUE, 2};
    DIALOG_BUTTON_CLOSE_DIALOG CLOSE{         0, 160,  70, 40, ILI9341_BLUE,      ILI9341_PINK, "Back", 2, 0xFFFF};
};


class PROFILE_MANAGER_DIALOG: public DIALOG{
  public:
    EVENT_RETURN Event_handle(EVENT_QUEUE e);

    DIALOG_BUTTON_CLOSE_DIALOG CLOSE{         0, 160,  70, 40, ILI9341_BLUE,      ILI9341_PINK, "Back", 2, 0xFFFF};
};

#endif
