#include "config_dialog.h"
#include <TelnetStream.h>


EVENT_RETURN CONFIG_DIALOG::Event_handle(EVENT_QUEUE e){
  switch(e.e){
           
    case EVENT_CREATE:
      lock_touch_events = true;
    
      this->Create(0,40,320,200,0x0000);                
      addTouchObject( &CLOSE);
      addTouchObject( &DISPLAY_TIMEOUT);
      addTouchObject( &WIFIBUTTON);
      addTouchObject( &SAVE);

      break;

    case EVENT_DRAW_ALL:
      break;        
  }
  return DIALOG::Event_handle(e);
} 

