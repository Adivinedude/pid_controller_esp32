#include "main_dialog.h"

char status_buffer[] = "00:00:00:00  Temp: 0000.00\nHeater is off      \0\0\0\0\0\0\0\0\0\0";

EVENT_RETURN MAIN_DIALOG::Event_handle(EVENT_QUEUE e){
  PROFILE_START
  switch(e.e){
    case EVENT_DRAW_ALL:
    case EVENT_DRAW_UPDATE:
      STATUS_BAR.Draw(0);
      break;
    
    case EVENT_CREATE:
      this->Create(0,0,320,40,0x0000);
      addStaticObject( &STATUS_BAR);
      break;
  }
  PROFILE_END
  return DIALOG::Event_handle(e);
}

EVENT_RETURN MAIN_MENU::Event_handle(EVENT_QUEUE e){
  PROFILE_START
  switch(e.e){
    case EVENT_CREATE:
      this->Create(0,40,320,200,0x0000);
      
      addTouchObject(&pid_mode);
      addTouchObject(&pid_config);
      addTouchObject(&profile_mode);
      addTouchObject(&profile_manager);
      addTouchObject(&config_menu); 
      
      break;

  }
  PROFILE_END
  return DIALOG::Event_handle(e);
}
