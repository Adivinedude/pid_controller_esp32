#ifndef DIALOG_TEXT_H
#define DIALOG_TEXT_H

//#include "gui_typedef.h"
#include "DisplayController.h"
#include "dialog_object.h"

class DIALOG;

class DIALOG_TEXT: public DIALOG_OBJECT, public DIALOG_OBJECT_TEXT{
  public:
    DIALOG_TEXT(_DIALOG_OBJECT_PRAM, _DIALOG_OBJECT_TEXT_PRAM, uint16_t bg_color=0x0000):
      _DIALOG_OBJECT_CALL, _DIALOG_OBJECT_TEXT_CALL{ text_background_color = bg_color;};
    
    uint16_t text_background_color;
    void Draw(GFXcanvas16* p);
};

#endif
