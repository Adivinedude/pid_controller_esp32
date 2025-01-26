#ifndef DIALOG_BASE_H
#define DIALOG_BASE_H

#include "dialog_object.h"

class mygui;

// base class for dialog
class DIALOG{
  public:
    DIALOG(){};
    virtual ~DIALOG();
    std::vector<DIALOG_OBJECT*> _static_array;
    std::vector<DIALOG_OBJECT_DYNAMIC*> _dynamic_array;
    std::vector<DIALOG_OBJECT_DYNAMIC*> _touch_array;
   
    void Create(uint16_t x, uint16_t y, uint16_t w, uint16_t l, uint16_t b_color=0x0000);

    void addStaticObject(DIALOG_OBJECT* o);
    void addDynamicObject(DIALOG_OBJECT_DYNAMIC* o);
    void addTouchObject(DIALOG_OBJECT_DYNAMIC* o);

    virtual EVENT_RETURN Event_handle(EVENT_QUEUE e);
    GFXcanvas16* static_image=0;
    DIALOG_OBJECT_DYNAMIC* last_obj_touched=0;
    uint16_t hold_counter;
    uint16_t orgin_x, orgin_y, width, length, bg_color;
    mygui* gui=0;
    DIALOG* parent_dialog  = 0;
    DIALOG* child_dialog   = 0;
    bool lock_draw_events  = 0;
    bool lock_touch_events = 0;
};


#endif
