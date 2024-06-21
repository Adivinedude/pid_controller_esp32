#ifndef GUI_TYPEDEF_H
#define GUI_TYPEDEF_H

#include <XPT2046_Touchscreen.h>
#include "my_debug.h"

#define res_x 320
#define res_y 240

#define TOUCH_UPDATE_INTERVAL 40
#define DRAW_UPDATE_INTERVAL  200
#define DEBOUNCE_COUNT 4
#define SCREEN_OFF_TIME 0 //minutes 0 - screen never turns off

// #define debug_mygui_loop
//#define  debug_mygui_display_this_touch

enum EVENT{
	//DIALOG EVENTS
  EVENT_CREATE = 0,     //Run item create routine
  EVENT_DISTROY,        //Run item destroy routine.
  EVENT_DRAW_ALL,       //redraw all objects.
  EVENT_DRAW_UPDATE,    //redraw dynamic objects
  EVENT_CHILD_CLOSED,   //child object has closed.

	//OBJECT EVENTS
  EVENT_TOUCH,          //touch event has occurred.
  EVENT_TOUCH_YES,      //mouse is over object
  EVENT_TOUCH_NO,       //mouse is no longer over object
  EVENT_TOUCH_PRESSED,  //item has been clicked on.
  EVENT_TOUCH_HOLD,     //item is being held down.

	//KEYBOARD OBJECT EVENTS
  KEY_OK,
	KEY_CANCEL,
	KEY_BACKSPACE,
	KEY_SHIFT,
	KEY_PRESSED, //EVENT_QUEUE.p.x = key value;
	
	//KEYBOARD COMPLETE EVENTs
	KEYBOARD_OK,
	KEYBOARD_CANCEL,
	

	EVENT_CUSTOM
	};

enum EVENT_RETURN{
  EVENT_RETURN_OK,            //Default return status. do nothing
  EVENT_RETURN_PASS,          //Event was not handled, let another dialog try.
  EVENT_RETURN_DISTROY,       //Object request destruction
	
	EVENT_RETURN_CUSTOM
};

struct EVENT_QUEUE{
  uint8_t e;
  TS_Point t;
};
#endif
