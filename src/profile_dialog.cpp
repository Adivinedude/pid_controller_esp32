#include "profile_dialog.h"


EVENT_RETURN PROFILE_DIALOG::Event_handle(EVENT_QUEUE e){
  switch(e.e){
           
    case EVENT_CREATE:
      lock_touch_events = true;
    
      this->Create(0,0,320,240,0x0000);          
      addTouchObject( &CLOSE);
      break;

    case EVENT_DRAW_ALL:

      break;
      
    case KEYBOARD_OK:

  
    case KEYBOARD_CANCEL:

      return EVENT_RETURN_OK;         
  }
  return DIALOG::Event_handle(e);
} 

EVENT_RETURN PROFILE_MANAGER_DIALOG::Event_handle(EVENT_QUEUE e){
  switch(e.e){
           
    case EVENT_CREATE:
      lock_touch_events = true;
      
      this->Create(0,0,320,240,0x0000);          
      addTouchObject( &CLOSE);
     break;

    case EVENT_DRAW_ALL:
      break;
      
    case EVENT_DRAW_UPDATE:
      break;
    
    case KEYBOARD_OK:
  
    case KEYBOARD_CANCEL:
 
      return EVENT_RETURN_OK;         
  }
  return DIALOG::Event_handle(e);
}
