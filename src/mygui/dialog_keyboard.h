#ifndef DIALOG_KEYBOARD_H
#define DIALOG_KEYBOARD_H

#include "dialog_base.h"
#include "dialog_button.h"
#include "dialog_text.h"
#include "gui.h"

extern char KEYBOARD_ABCD[];
extern char KEYBOARD_abcd[];
extern char KEYBOARD_NUMBER[];
extern char KEYBOARD_SYMBOLS[];

extern char** FULL_KEYBOARD;
extern char** NUMBER_KEYBOARD;

typedef void (*callback_function_set_value)(void* value);

class KB_KEY: public DIALOG_BUTTON_TEXT{
  public:
	KB_KEY (_DIALOG_BUTTON_TEXT_PRAM, char val=0) : _DIALOG_BUTTON_TEXT_CALL {value = val;};
  void onClick(DIALOG* p);
  uint16_t value;
};

#define _KB_BUTTON_PRAM _DIALOG_BUTTON_TEXT_PRAM, char val
#define _KB_BUTTON_USE  _DIALOG_BUTTON_TEXT_USE, val
#define _KB_BUTTON_CALL KB_BUTTON{_KB_BUTTON_USE}

class KB_BUTTON: public DIALOG_BUTTON_TEXT{
  public:
	KB_BUTTON( _KB_BUTTON_PRAM ) : _DIALOG_BUTTON_TEXT_CALL {value = val;};
	void onClick(DIALOG* p);
  uint16_t value;
};

//TODO: there is an easyier way to do this.
class KB_BACKSPACE_BUTTON: public KB_BUTTON{
  public:
  KB_BACKSPACE_BUTTON (_KB_BUTTON_PRAM)
  : _KB_BUTTON_CALL {};
  
  EVENT_RETURN Event_handle(DIALOG* d, EVENT_QUEUE e);
};

class DIALOG_KEYBOARD_SYMBOLS: public DIALOG
{
  public:
	DIALOG_KEYBOARD_SYMBOLS(char* s){keyboard_string = s;};
	EVENT_RETURN Event_handle(EVENT_QUEUE e);
  private:
	char* keyboard_string=0;
	char* keyboard_buffer=0;
};

class DIALOG_BUTTON_INPUT;
class DIALOG_KEYBOARD_MAIN: public DIALOG
{
  public:
	DIALOG_KEYBOARD_MAIN(char** l):symbol_list{l}{memset(input_buffer, 0x00, sizeof(input_buffer));};
	~DIALOG_KEYBOARD_MAIN(){}
	EVENT_RETURN Event_handle(EVENT_QUEUE e);

	char input_buffer[64];              //DIALOG_OBJECT     DIALOG_OBJECT_TEXT           
	char** symbol_list;                 //X   Y   L    H  STR   TEXT_SIZE     FOR_COLOR  
	DIALOG_TEXT input_display            { 66, 10, 157, 20, (char*)&input_buffer,  2, 0xFFFF};
	                                    //DIALOG_OBJECT     DIALOG_OBJECT_TEXT           KB_BUTTON
	                                    //X   Y   L    H  STR  TEXT_SIZE FOR_COLOR BG_COLOR  EVENT_VALUE
	KB_BUTTON CANCEL_BUTTON              {  0,  0,  32, 32, ILI9341_DARKGREY, ILI9341_LIGHTGREY, "X" , 2, 0xFFFF, KEY_CANCEL};
	KB_BACKSPACE_BUTTON BACKSPACE_BUTTON { 32,  0,  32, 32, ILI9341_DARKGREY, ILI9341_LIGHTGREY, "<" , 2, 0xFFFF, KEY_BACKSPACE};
	KB_BUTTON OK_BUTTON                  {223,  0,  64, 32, ILI9341_DARKGREY, ILI9341_LIGHTGREY, "Ok", 2, 0xFFFF, KEY_OK};
	KB_BUTTON SHIFT_KEYS                 {287,  0,  32, 32, ILI9341_DARKGREY, ILI9341_LIGHTGREY, "^" , 2, 0xFFFF, KEY_SHIFT};
	DIALOG* child_dialog=0;
	uint8_t current_keyboard=0;
  DIALOG_BUTTON_INPUT* parent_button;
  private:
	void handleKeys(EVENT_QUEUE e);
};


#define _DIALOG_BUTTON_INPUT_PRAM _DIALOG_BUTTON_TEXT_PRAM , callback_function_set_value callback
#define _DIALOG_BUTTON_INPUT_USE  _DIALOG_BUTTON_TEXT_USE, callback
#define _DIALOG_BUTTON_INPUT_CALL DIALOG_BUTTON_INPUT{_DIALOG_BUTTON_INPUT_USE}
class DIALOG_BUTTON_INPUT: public DIALOG_BUTTON_TEXT{
  public:
    DIALOG_BUTTON_INPUT( _DIALOG_BUTTON_INPUT_PRAM ): _DIALOG_BUTTON_TEXT_CALL 
      {prefix=s; callback_function = callback;};
    char buff[70];
    const char* prefix;
    char** charset = NUMBER_KEYBOARD;
    DIALOG_KEYBOARD_MAIN* KEYBOARD = 0;
    callback_function_set_value callback_function;
    void* value;
    virtual void alpha_to_value(char* buffer)=0;
    virtual void value_to_alpha(char* buffer)=0;
    
    void onClick(DIALOG* d);

    void init();
    void update();
    void close();
};

class DIALOG_BUTTON_INPUT_BOOL: public DIALOG_BUTTON_INPUT{
  public:
  DIALOG_BUTTON_INPUT_BOOL (_DIALOG_BUTTON_INPUT_PRAM, bool *val):_DIALOG_BUTTON_INPUT_CALL {value = val;};
  
  void onClick(DIALOG* d);
  
  void alpha_to_value(char* buffer);
  void value_to_alpha(char* buffer);
  
};

class DIALOG_BUTTON_INPUT_INT32: public DIALOG_BUTTON_INPUT{
  public:
  DIALOG_BUTTON_INPUT_INT32 (_DIALOG_BUTTON_INPUT_PRAM, uint32_t *val):_DIALOG_BUTTON_INPUT_CALL {value = val;};
  
  void alpha_to_value(char* buffer);
  void value_to_alpha(char* buffer);
  
};

class DIALOG_BUTTON_INPUT_INT8: public DIALOG_BUTTON_INPUT{
  public:
  DIALOG_BUTTON_INPUT_INT8 (_DIALOG_BUTTON_INPUT_PRAM, uint8_t *val):_DIALOG_BUTTON_INPUT_CALL {value = val;};
  
  void alpha_to_value(char* buffer);
  void value_to_alpha(char* buffer);
  
};

class DIALOG_BUTTON_INPUT_FLOAT: public DIALOG_BUTTON_INPUT{
  public:
  DIALOG_BUTTON_INPUT_FLOAT (_DIALOG_BUTTON_INPUT_PRAM, float* val):_DIALOG_BUTTON_INPUT_CALL {value = val;};
  
  void alpha_to_value(char* buffer);
  void value_to_alpha(char* buffer);
};

class DIALOG_BUTTON_INPUT_STRING: public DIALOG_BUTTON_INPUT{
  public:
  DIALOG_BUTTON_INPUT_STRING (_DIALOG_BUTTON_INPUT_PRAM, std::string* val):_DIALOG_BUTTON_INPUT_CALL {value = val;charset = FULL_KEYBOARD;};
  
  void alpha_to_value(char* buffer);
  void value_to_alpha(char* buffer);
};

class DIALOG_BUTTON_INPUT_CHAR: public DIALOG_BUTTON_INPUT{
  public:
  DIALOG_BUTTON_INPUT_CHAR (_DIALOG_BUTTON_INPUT_PRAM, char* val, int sz):_DIALOG_BUTTON_INPUT_CALL {value = val; size = sz; charset = FULL_KEYBOARD;};
  
  int size;
  void alpha_to_value(char* buffer);
  void value_to_alpha(char* buffer);
};

#endif
