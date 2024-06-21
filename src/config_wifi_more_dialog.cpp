#include "config_wifi_more_dialog.h"

EVENT_RETURN WIFI_MORE_DIALOG::Event_handle(EVENT_QUEUE e){
  switch(e.e){
           
    case EVENT_CREATE:
      lock_touch_events = true;
      lock_draw_events = true;
    
      this->Create(0,40,320,200,0x0000);                
      addTouchObject( &CLOSE);
      
      break;

    case EVENT_DRAW_ALL:
      
    case EVENT_DRAW_UPDATE:
      break;
      
    case KEYBOARD_OK:
            
    case KEYBOARD_CANCEL:
      return EVENT_RETURN_OK;    
      
    case EVENT_DISTROY:   
      break;  
  }
  return DIALOG::Event_handle(e);
} 