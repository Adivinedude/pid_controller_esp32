#ifndef DIALOG_OBJECT_H
#define DIALOG_OBJECT_H

#include "gui_typedef.h"
#include "DisplayController.h"

class DIALOG;

//base class for dialog objects
#define _DIALOG_OBJECT_PRAM uint16_t x, uint16_t y, uint16_t w, uint16_t h
#define _DIALOG_OBJECT_USE  x,y,w,h
#define _DIALOG_OBJECT_CALL DIALOG_OBJECT{_DIALOG_OBJECT_USE}
class DIALOG_OBJECT{
  public:
    // x, y, width, height
    DIALOG_OBJECT( _DIALOG_OBJECT_PRAM ):orgin_x{x}, orgin_y{y}, length{w}, height{h},update_flag{0}, img{0}{};
    virtual ~DIALOG_OBJECT();
    virtual void Draw(GFXcanvas16* p)=0;

    GFXcanvas16* GetImg();
    void Trash();
    uint16_t orgin_x, orgin_y, length, height;
    bool update_flag;
    GFXcanvas16* img;
};

//base class for interactive objects
#define _DIALOG_OBJECT_DYNAMIC_PRAM _DIALOG_OBJECT_PRAM
#define _DIALOG_OBJECT_DYNAMIC_USE  _DIALOG_OBJECT_USE
#define _DIALOG_OBJECT_DYNAMIC_CALL DIALOG_OBJECT_DYNAMIC{_DIALOG_OBJECT_DYNAMIC_USE}
class DIALOG_OBJECT_DYNAMIC: public DIALOG_OBJECT{
  public:
	DIALOG_OBJECT_DYNAMIC( _DIALOG_OBJECT_DYNAMIC_PRAM ):
     _DIALOG_OBJECT_CALL {};

    virtual EVENT_RETURN Event_handle(DIALOG* d, EVENT_QUEUE e)=0;
    bool TouchInBounds(TS_Point t);
};

//base class for drawable text
#define _DIALOG_OBJECT_TEXT_PRAM const char* s, uint8_t t_size, uint16_t color
#define _DIALOG_OBJECT_TEXT_USE s, t_size, color
#define _DIALOG_OBJECT_TEXT_CALL DIALOG_OBJECT_TEXT{_DIALOG_OBJECT_TEXT_USE}
class DIALOG_OBJECT_TEXT{
  public:
    DIALOG_OBJECT_TEXT(_DIALOG_OBJECT_TEXT_PRAM) {string = s; text_size = t_size; text_color = color;};

    const char* string;
    uint8_t text_size;
    uint16_t text_color;
};

#endif
