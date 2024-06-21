#include "pid_dialog.h"
#include "mygui/pid.h"

extern OVEN_SIMULATOR oven; //oven simulator
extern PID_CONTROLLER pid;  //heat controler object
extern volatile uint32_t total_time;

float settemp_holder;
float i_holder;

void SetTempCallback(void* value){
  PROFILE_START
  pid.set_temp( *(float*)value );
  PROFILE_END
}

void SetICallback(void* value){
  PROFILE_START
  pid.pid.setI( *(float*)value );
  PROFILE_END
}

EVENT_RETURN PID_CONFIG_DIALOG::Event_handle(EVENT_QUEUE e){
  PROFILE_START
  switch(e.e){
           
    case EVENT_CREATE:
      PROFILE_LOG("EVENT_CREATE")
      lock_touch_events = true;
    
      this->Create(0,40,320,200,0x0000);          
      addTouchObject( &MODE);
      addTouchObject( &ONOFFBUTTON); ONOFFBUTTON.init();
      addTouchObject( &SETTEMPBUTTON); SETTEMPBUTTON.init();
      addTouchObject( &CLOSE);
      addTouchObject( &UNITS); 
      addTouchObject( &buttonr); buttonr.init();
      addTouchObject( &buttonp); buttonp.init();
      addTouchObject( &buttoni); buttoni.init();
      addTouchObject( &buttond); buttond.init();
      break;

    case KEY_OK:
    case EVENT_DRAW_ALL:
      MODE.init();
      ONOFFBUTTON.init();

      settemp_holder = pid.GetTarget();
      SETTEMPBUTTON.init();
      UNITS.init();
      buttonr.init();
      buttonp.init();
      buttoni.init();
      buttond.init();
      break;   
  
  }
  PROFILE_END
  return DIALOG::Event_handle(e);
} 

EVENT_RETURN PID_MODE_DIALOG::Event_handle(EVENT_QUEUE e){
  PROFILE_START
  switch(e.e){
           
    case EVENT_CREATE:
      PROFILE_LOG("EVENT_CREATE")
      lock_touch_events = true;
      
      this->Create(0,40,320,200,0x0000);          
      addTouchObject( &CLOSE);
      addTouchObject( &UNITS);
      addTouchObject( &ONOFFBUTTON); ONOFFBUTTON.init();
      addTouchObject( &SETTEMPBUTTON); SETTEMPBUTTON.init();
      addTouchObject( &temp_up);
      addTouchObject( &temp_down);
      break;

    case KEY_OK:
    case EVENT_DRAW_ALL:
      settemp_holder = pid.GetTarget();
      SETTEMPBUTTON.init();
      ONOFFBUTTON.init();
      break;
      
  }
  PROFILE_END
  return DIALOG::Event_handle(e);
}
