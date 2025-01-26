#ifndef DIALOG_BUTTON_H
#define DIALOG_BUTTON_H

#include "dialog_object.h"
#include "dialog_base.h"
#include "gui.h"

//simple push button
#define _DIALOG_BUTTON_PRAM _DIALOG_OBJECT_DYNAMIC_PRAM, uint16_t c1, uint16_t c2
#define _DIALOG_BUTTON_USE  _DIALOG_OBJECT_DYNAMIC_USE, c1, c2
#define _DIALOG_BUTTON_CALL DIALOG_BUTTON{_DIALOG_BUTTON_USE}
class DIALOG_BUTTON: public DIALOG_OBJECT_DYNAMIC{
  public:
    DIALOG_BUTTON( _DIALOG_BUTTON_PRAM ):
    	_DIALOG_OBJECT_DYNAMIC_CALL {rollover_color = false; color1 = c1; color2 = c2;};
    	
    bool rollover_color;
    uint16_t color1, color2;	

    EVENT_RETURN Event_handle(DIALOG* d, EVENT_QUEUE e);
    virtual void onClick(DIALOG* p)=0;
    
};

#define _DIALOG_BUTTON_TEXT_PRAM _DIALOG_BUTTON_PRAM, _DIALOG_OBJECT_TEXT_PRAM
#define _DIALOG_BUTTON_TEXT_USE  _DIALOG_BUTTON_USE, _DIALOG_OBJECT_TEXT_USE
#define _DIALOG_BUTTON_TEXT_CALL DIALOG_BUTTON_TEXT{_DIALOG_BUTTON_TEXT_USE}
class DIALOG_BUTTON_TEXT: public DIALOG_BUTTON, public DIALOG_OBJECT_TEXT{
  public:
    DIALOG_BUTTON_TEXT( _DIALOG_BUTTON_TEXT_PRAM ) :
      _DIALOG_BUTTON_CALL, _DIALOG_OBJECT_TEXT_CALL {};

    void Draw(GFXcanvas16* p);
    virtual void onClick(DIALOG* p)=0;
};


class DIALOG_BUTTON_CLOSE_DIALOG: public DIALOG_BUTTON_TEXT{
  public:
  DIALOG_BUTTON_CLOSE_DIALOG (_DIALOG_BUTTON_TEXT_PRAM) : _DIALOG_BUTTON_TEXT_CALL{};
  void onClick(DIALOG* d){
    PROFILE_START
    d->gui->removeDialog(d);
    PROFILE_END
  }
};

#define _DIALOG_BUTTON_ARROW_PRAM _DIALOG_BUTTON_PRAM, uint8_t dir
#define _DIALOG_BUTTON_ARROW_USE  _DIALOG_BUTTON_USE, dir
#define _DIALOG_BUTTON_ARROW_CALL DIALOG_BUTTON_ARROW{_DIALOG_BUTTON_ARROW_USE}
class DIALOG_BUTTON_ARROW: public DIALOG_BUTTON{
  public:
  DIALOG_BUTTON_ARROW (_DIALOG_BUTTON_ARROW_PRAM) : _DIALOG_BUTTON_CALL {direction = dir;};
  EVENT_RETURN Event_handle(DIALOG* d, EVENT_QUEUE e);
  uint8_t direction;
  void Draw(GFXcanvas16* p);
};

/*
#define _DIALOG_BUTTON_SHAPE_PRAM _DIALOG_BUTTON_PRAM, uint8_t angle, int16_t *faces, uint8_t face_count
#define _DIALOG_BUTTON_SHAPE_USE  _DIALOG_BUTTON_USE, rotation, faces, face_count
#define _DIALOG_BUTTON_SHAPE_CALL DIALOG_BUTTON_ARROW{_DIALOG_BUTTON_ARROW_USE}

class DIALOG_BUTTON_SHAPE: public DIALOG_BUTTON{
  public:
  DIALOG_BUTTON_SHAPE (_DIALOG_BUTTON_SHAPE_PRAM) : _DIALOG_BUTTON_CALL 
    {rotation = angle; _faces = faces; _face_count = face_count;};
  EVENT_RETURN Event_handle(DIALOG* d, EVENT_QUEUE e);
  uint16_t rotation;
  int16_t *_faces;
  uint8_t  _face_count;
  
  void Draw(GFXcanvas16* p);
};
*/

void rotate_point(int16_t &x, int16_t &y, int16_t angle, int16_t orgin_x, int16_t orgin_y);
#endif
