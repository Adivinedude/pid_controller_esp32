#ifndef GUI_H
#define GUI_H
//support libarys
#include <vector>
#include <queue>
#include <list>

#include "gui_typedef.h"
#include "custom_pins.h"
#include "esp32-hal-cpu.h"

//base libarys
#include "dialog_object.h"
//#include "dialog_text.h"
#include "dialog_base.h"
#include "gui_typedef.h"

/*  how to get Adafruit_GFX to use esp32 psram.
//use psram for the image buffers. its bigger (4-8megs).
// edited "Adafruit_GFX.h" line 368. added default paramater 'use_psram'
  GFXcanvas16(uint16_t w, uint16_t h, bool use_psram=false);
  
// edited "Adafruit_GFX.cpp" line 2385 added ps_malloc usage
GFXcanvas16::GFXcanvas16(uint16_t w, uint16_t h, bool use_psram) : Adafruit_GFX(w, h) {
  uint32_t bytes = w * h * 2;
  if( use_psram ){
    if ((buffer = (uint16_t *)ps_malloc(bytes))) { 
      //memset(buffer, 0, bytes);//unnessary. im about to color this. why doubledown the writes.
    }
  }else{
    if ((buffer = (uint16_t *)malloc(bytes))) { 
      memset(buffer, 0, bytes);
    }
  }
}
*/

void ReportResources();

class mygui{
  private:
    std::list<DIALOG*> _dialog_list;
    std::queue <EVENT_QUEUE> _event_queue;
    uint32_t _t_time, _d_time, _debounce, _screen_off_timer;
    TS_Point last_touch;
    bool     touch_flag = false;
    bool     draw_flag = false;
    
  public:
    void addDialog(DIALOG* d);
    void addChildDialog(DIALOG* parent, DIALOG* child);
    void removeDialog(DIALOG* d);
    void addEvent(EVENT e);
    void addEvent(EVENT_QUEUE t);
    void loop();
    void Begin();
    bool isEmpty();
    uint8_t _brightness, max_brightness;
};


template <class DIALOG_BUTTON_OPEN_CHILD_DIALOG_T>
void OpenChildDialog(DIALOG_OBJECT* obj, DIALOG* d){
  if( obj )
    obj->update_flag = false; //do not redraw the button.
    
  if( d->child_dialog ) //if there is an existing child dialog, delete it
    delete d->child_dialog;
    
  d->child_dialog = new DIALOG_BUTTON_OPEN_CHILD_DIALOG_T;
  d->gui->addChildDialog(d, d->child_dialog);
}

#endif
