#include "dialog_keyboard.h"
#include <algorithm>

char KEYBOARD_ABCD[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
char KEYBOARD_abcd[] = "abcdefghijklmnopqrstuvwxyz ";
char KEYBOARD_NUMBER[] = "1234567890.-";
char KEYBOARD_SYMBOLS[] = "`~!@#$%^&*()_-=+[]{}\\|,.<>/?";

char* full[] = {KEYBOARD_ABCD, KEYBOARD_abcd, KEYBOARD_NUMBER, KEYBOARD_SYMBOLS, 0};
char* num[] = {KEYBOARD_NUMBER, 0};

char** FULL_KEYBOARD = full;
char** NUMBER_KEYBOARD = num;

void KB_KEY::onClick(DIALOG* p){
	EVENT_QUEUE q;
  q.e = KEY_PRESSED;
  q.t.x = value;
  p->gui->addEvent(q);
}


void KB_BUTTON::onClick(DIALOG* p){
  EVENT_QUEUE q;
  q.e = (EVENT)value;
  p->gui->addEvent(q);
}

EVENT_RETURN KB_BACKSPACE_BUTTON::Event_handle(DIALOG* d, EVENT_QUEUE e){
  switch(e.e){
    case EVENT_TOUCH_HOLD:
      KB_BUTTON::onClick(d); 
      return EVENT_RETURN_OK;      
    break;
  }
  return DIALOG_BUTTON::Event_handle(d, e);
}

EVENT_RETURN DIALOG_KEYBOARD_SYMBOLS::Event_handle(EVENT_QUEUE e)
{
	PROFILE_START
  switch(e.e){
	case EVENT_CREATE:
	{
	  Create(0,39,320,200,0x0000); //make the dialog

	  //make the keyboard_buffer and initialize heap stuff
		  uint8_t keyboardsize;
		  keyboardsize = strlen(keyboard_string);
		  keyboard_buffer = new char[keyboardsize*2];
		  for(uint8_t a=0; a<keyboardsize; a++){
			keyboard_buffer[a*2] = keyboard_string[a];
			keyboard_buffer[a*2+1] = 0;
		  }

	  //calculate keyboard spacing

		  uint16_t rows, columns, x_interval, y_interval, a;
		  rows = 3;
		  columns = keyboardsize / rows;
		  columns += (keyboardsize%rows)?1:0;
		  x_interval = 320 / columns;
		  y_interval = 200 / rows;
		  #ifdef DEBUG_VERBOSE
		  	  //LOGF("\nKeyboard: rows %d, columns %d\nx_int %d y_int %d\n",
		  	  //  rows, columns, x_interval, y_interval);
		  #endif

	  //create keys
		  a = 0; KB_KEY* k;
		  for(uint16_t y = 0; y < rows; y++){
			  for(uint16_t x = 0; x < columns; x++){
				  if(keyboardsize > a){
					char c = keyboard_string[a];
#ifdef DEBUG_VERBOSE
					//  Serial.printf("%c: x:%d, y: %d, l:%d, w:%d\n",
					//		        c, x, y, x_interval, y_interval);
#endif
					k = new KB_KEY(x*x_interval, y*y_interval, x_interval, y_interval, 
					                ILI9341_DARKGREY, ILI9341_LIGHTGREY, 
					                &keyboard_buffer[a*2], 2, 0xFFFF, c);
					addTouchObject(k);
					a++;
				  }else
					  break;
			  }
		  }

	}
	  break;
	case EVENT_DISTROY:
	  //LOG("keyboard symbols distroy\n")
		delete keyboard_buffer;
		std::for_each(_dynamic_array.begin(), _dynamic_array.end(), [](DIALOG_OBJECT_DYNAMIC* it){delete it;});
	  break;
  }
	PROFILE_END
  return DIALOG::Event_handle(e);
}

EVENT_RETURN DIALOG_KEYBOARD_MAIN::Event_handle(EVENT_QUEUE e){
  //handle standard events
  switch(e.e){
	  case EVENT_CREATE:
			PROFILE_LOG("EVENT_CREATE START")
      Create(0,0,320,32,0x0000);
      lock_draw_events = true;
      
      addStaticObject(&input_display);
		addTouchObject(&CANCEL_BUTTON);
		addTouchObject(&BACKSPACE_BUTTON);

		addTouchObject(&OK_BUTTON);
		addTouchObject(&SHIFT_KEYS);
      
      child_dialog = new DIALOG_KEYBOARD_SYMBOLS(symbol_list[0]);
		  gui->addDialog(child_dialog);
			PROFILE_LOG("EVENT_CREATE END")
	  break;

	  case EVENT_DISTROY:
			PROFILE_LOG("EVENT_DISTROY START")
	    //LOG("keyboard main distroy\n")
		  gui->removeDialog(child_dialog);
		  delete child_dialog;
		  child_dialog = 0;
			PROFILE_LOG("EVENT_DISTROY END")
	  break;
    
    case KEY_BACKSPACE:
    	//LOG("Backspace\n")
    	{
	      uint8_t position = strlen(input_buffer);
	      if (position){
		      input_buffer[position-1] = 0;
		      input_display.Draw(0);
		    }
      }
	    return EVENT_RETURN_OK;

    case KEY_CANCEL:
    	PROFILE_LOG("KEY_CANCEL")
    	gui->addEvent(KEYBOARD_CANCEL);
			parent_button->close();
    	return EVENT_RETURN_DISTROY;

    case KEY_OK:
    	PROFILE_LOG("KEY_OK")
    	gui->addEvent(KEYBOARD_OK);
			parent_button->update();
			parent_button->close();
	    return EVENT_RETURN_OK;

    case KEY_SHIFT:
	    PROFILE_LOG("KEY_SHIFT")

      current_keyboard++;
      if( symbol_list[current_keyboard] == 0 )
      current_keyboard = 0;
      	    
	    gui->removeDialog(child_dialog);
	    delete child_dialog;
	    
      child_dialog = new DIALOG_KEYBOARD_SYMBOLS(symbol_list[current_keyboard]);
		      
	    gui->addDialog(child_dialog);
	    return EVENT_RETURN_OK;
	    
	  case KEY_PRESSED:
		  //LOG("Keypress\n")
		  handleKeys(e);
		  return EVENT_RETURN_OK;
  }
  return DIALOG::Event_handle(e);
}

void DIALOG_KEYBOARD_MAIN::handleKeys( EVENT_QUEUE e){
	PROFILE_START
	uint8_t position = strlen(input_buffer);
	//Serial.println(position);
	if( position > sizeof(input_buffer) )
		return;
	input_buffer[position] = e.t.x;
	//Serial.println(input_buffer);
	input_display.Draw(0);
	PROFILE_END
}

/////////////////////////////////////////////////////////////////////

void DIALOG_BUTTON_INPUT::init(){
	PROFILE_START
  char temp[70];
  value_to_alpha(temp);
  sprintf(buff, "%s%s", prefix, temp);
  string = (char*)&buff;
  Draw(0);
	PROFILE_END
}

void DIALOG_BUTTON_INPUT::onClick(DIALOG* d){
	PROFILE_START
  KEYBOARD = new DIALOG_KEYBOARD_MAIN( charset );
  KEYBOARD->parent_dialog = d;
	KEYBOARD->parent_button = this;
  value_to_alpha(KEYBOARD->input_buffer);
  d->gui->addDialog(KEYBOARD);
	PROFILE_END
}

void DIALOG_BUTTON_INPUT::update(){
	PROFILE_START
  if( !KEYBOARD )
    return;
  alpha_to_value(KEYBOARD->input_buffer);
	if( callback_function ){
		PROFILE_LOG("callback function start")
		callback_function(value);
		PROFILE_LOG("callback function end")
	}
  init();
  PROFILE_END
}

void DIALOG_BUTTON_INPUT::close(){
	PROFILE_START
	if( KEYBOARD ){
		KEYBOARD->gui->removeDialog(KEYBOARD);
		delete KEYBOARD;
		KEYBOARD = 0;
	}
	PROFILE_END
}

//////////
//child classes - BOOL
void DIALOG_BUTTON_INPUT_BOOL::onClick(DIALOG* d){
  *(bool*)value = !(*(bool*)value);
  init();
}

void DIALOG_BUTTON_INPUT_BOOL::value_to_alpha(char* buffer){
  sprintf(buffer, "%s", *(bool*)value?"on":"off");
}
void DIALOG_BUTTON_INPUT_BOOL::alpha_to_value(char* buffer){
  return;
}

//child class - INT32
void DIALOG_BUTTON_INPUT_INT32::value_to_alpha(char* buffer){
  sprintf(buffer, "%u", *(uint32_t*)value);
}
void DIALOG_BUTTON_INPUT_INT32::alpha_to_value(char* buffer){
  *(uint32_t*)value = atoi(buffer);
}

//child class - INT8
void DIALOG_BUTTON_INPUT_INT8::value_to_alpha(char* buffer){
  sprintf(buffer, "%u", *(uint8_t*)value);
}
void DIALOG_BUTTON_INPUT_INT8::alpha_to_value(char* buffer){
  *(uint8_t*)value = atoi(buffer);
}

//child class - FLOAT
extern char* morphNumericString (char *s, int n);
void DIALOG_BUTTON_INPUT_FLOAT::value_to_alpha(char* buffer){
  sprintf(buffer, "%.2f", *(float*)value);
  morphNumericString(buffer, 2);
}
void DIALOG_BUTTON_INPUT_FLOAT::alpha_to_value(char* buffer){
  *(float*)value = atof(buffer);
}

//child class - STRING
void DIALOG_BUTTON_INPUT_STRING::value_to_alpha(char* buffer){
  sprintf(buffer, "%s", ((std::string*)value)->c_str());
}
void DIALOG_BUTTON_INPUT_STRING::alpha_to_value(char* buffer){
  *(std::string*)value = buffer;
}

//child class - CHAR[]
void DIALOG_BUTTON_INPUT_CHAR::value_to_alpha(char* buffer){
  sprintf(buffer,  "%s", (char*)value);
}
void DIALOG_BUTTON_INPUT_CHAR::alpha_to_value(char* buffer){
  snprintf((char*)value, size,  "%s", buffer);
}

