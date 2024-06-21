#ifndef DIALOG_LISTBOX_H
#define DIALOG_LISTBOX_H

#include "dialog_button.h"
#include <vector>
#include <string>

//simple push button
#define _DIALOG_LISTBOX_PRAM _DIALOG_OBJECT_DYNAMIC_PRAM, uint16_t c1, uint16_t c2, uint8_t s
#define _DIALOG_LISTBOX_USE  _DIALOG_OBJECT_DYNAMIC_USE, c1, c2, s
#define _DIALOG_LISTBOX_CALL DIALOG_LISTBOX{_DIALOG_LISTBOX_USE}
class DIALOG_LISTBOX: public DIALOG_OBJECT_DYNAMIC{
  public:
    DIALOG_LISTBOX( _DIALOG_LISTBOX_PRAM ):
    	_DIALOG_OBJECT_DYNAMIC_CALL {
    	  text_size = s, color1 = c1; color2 = c2;
    	  thread_safe_access = xSemaphoreCreateMutex();
    };
    ~DIALOG_LISTBOX(){
      xSemaphoreTake( thread_safe_access, portMAX_DELAY);
      vSemaphoreDelete( thread_safe_access );
    };
    
    
    int selection = -1;
    int scroll = 0;
    int max_space;
    int row_height;
    uint16_t color1, color2;
    uint8_t text_size;	
        
    std::vector<std::string> content;
    
    SemaphoreHandle_t thread_safe_access;
    EVENT_RETURN Event_handle(DIALOG* d, EVENT_QUEUE e);
    virtual void onClick(DIALOG* p)=0;
    void Draw(GFXcanvas16* p);
    
};

#endif
