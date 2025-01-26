#include "dialog_base.h"
#include "gui.h"

DIALOG::~DIALOG(){
	if(static_image)
		delete static_image;
}

//dialog methods
void DIALOG::Create(uint16_t x, uint16_t y, uint16_t w, uint16_t l, uint16_t b_color){
  orgin_x = x; orgin_y = y; width = w; length = l; bg_color = b_color;
  if( static_image )
    delete static_image;
  static_image = new GFXcanvas16(w, l, true);
}

void DIALOG::addStaticObject(DIALOG_OBJECT* d){
  _static_array.push_back(d);
}

void DIALOG::addDynamicObject(DIALOG_OBJECT_DYNAMIC* d){
   _dynamic_array.push_back(d);
}


void DIALOG::addTouchObject(DIALOG_OBJECT_DYNAMIC* d){
  _dynamic_array.push_back(d);
  _touch_array.push_back(d);
  EVENT_QUEUE e;
  e.e = EVENT_TOUCH_NO;
  d->Event_handle(this, e);
}

EVENT_RETURN DIALOG::Event_handle(EVENT_QUEUE e){
  PROFILE_START
  DIALOG_OBJECT_DYNAMIC* p;
  DIALOG_OBJECT* o;
  bool found = false;
  switch(e.e){
    case EVENT_CHILD_CLOSED:
    
    case EVENT_DRAW_ALL:
      PROFILE_LOG("EVENT_DRAW_ALL - START")
      static_image->fillRect(0, 0, width, length, bg_color);
      for(std::vector<DIALOG_OBJECT*>::iterator it = _static_array.begin(); it != _static_array.end(); it++)
        (*it)->Draw(static_image);

      for(std::vector<DIALOG_OBJECT_DYNAMIC*>::iterator it = _dynamic_array.begin(); it != _dynamic_array.end(); it++)
      	(*it)->Draw(static_image);
      
      tft.drawRGBBitmap(orgin_x, orgin_y, static_image->getBuffer(), width, length);
      PROFILE_LOG("EVENT_DRAW_ALL - END")
      break;

    case EVENT_DRAW_UPDATE:
      PROFILE_LOG("EVENT_DRAW_UPDATE - START")
      for(std::vector<DIALOG_OBJECT*>::iterator it = _static_array.begin(); it != _static_array.end(); it++){
        o = *it;
        if(o->update_flag == true){
          o->update_flag = false;
          tft.drawRGBBitmap(orgin_x+o->orgin_x, orgin_y+o->orgin_y, o->GetImg()->getBuffer(), o->length, o->height);
        }
      }

      for(std::vector<DIALOG_OBJECT_DYNAMIC*>::iterator it = _dynamic_array.begin(); it != _dynamic_array.end(); it++){
        p = *it;
        if(p->update_flag){
          p->update_flag = false;
          tft.drawRGBBitmap(orgin_x+p->orgin_x, orgin_y+p->orgin_y, p->GetImg()->getBuffer(), p->length, p->height);
        }
      } 
      PROFILE_LOG("EVENT_DRAW_UPDATE - END")
      break;
      //end of DRAW_ALL

    case EVENT_TOUCH:
    	//LOG("EVENT_TOUCH\n")
      for(std::vector<DIALOG_OBJECT_DYNAMIC*>::iterator it = _touch_array.begin(); it != _touch_array.end() && !found; it++){
        p = *it;
        TS_Point t = e.t;
        
        //is the touch within the dialog?
        if( lock_touch_events ){
          if( ! (t.x > orgin_x && t.x < orgin_x+width && t.y > orgin_y && t.y < orgin_y+length) )
            return EVENT_RETURN_PASS;
        }
        //correct for dialog position
        t.x -= orgin_x;
        t.y -= orgin_y;
        if(p->TouchInBounds(t)){ //if an object is being touched.
        
        	e.e = EVENT_TOUCH;
        	p->Event_handle(this, e); //send it the touch event
        	
          if(last_obj_touched == 0){ // if this is the first object being touched.
            //LOG("First Touch\n")
            last_obj_touched = p;
            e.e = EVENT_TOUCH_YES;
            p->Event_handle(this, e);
            hold_counter = 0;
            return EVENT_RETURN_OK;
          }
          else  // if a object was being touched on the last iteration
          {
            if(last_obj_touched == p){  //if this is the same object as last iteration.
              hold_counter++;
              
              EVENT_RETURN rt = EVENT_RETURN_OK;
              if( e.t.z == 0){ //has it been pressed then released
            	  //LOG("Button Presse\n")
                e.e = EVENT_TOUCH_PRESSED;//signal button press
                rt = last_obj_touched->Event_handle(this, e);
                last_obj_touched = 0;
                hold_counter = 0;
              }else if( hold_counter > 24 ){//if the object has been held for a long time send the hold message.
                uint8_t hold_speed_counter = 0;
                //LOG("Button Hold\n")
                if( hold_counter < 100 ){
                  hold_speed_counter = hold_counter % 15;
                }else if( hold_counter < 300 ){
                  hold_speed_counter = 0;
                  hold_counter = 400;
                }
                if( hold_speed_counter == 0 ){
                  e.e = EVENT_TOUCH_HOLD;
                  rt = last_obj_touched->Event_handle(this, e);
                }
              }
              return rt;
            }
            else
            { //else transfer touch to the new object.
              hold_counter = 0;
              e.e = EVENT_TOUCH_NO;
              last_obj_touched->Event_handle(this, e);
              last_obj_touched = p;
              e.e = EVENT_TOUCH_YES;
              p->Event_handle(this, e);
              return EVENT_RETURN_OK;
            }
          }
        }
      }//end of for loop

      //no object was being touched.
      //if there was a touch on the last iteration. clear it.
      if(last_obj_touched){
        //LOG("touch no\n")
        e.e = EVENT_TOUCH_NO;
        last_obj_touched->Event_handle(this, e);
        last_obj_touched = 0;
      }
      
      if( lock_touch_events ){
        return EVENT_RETURN_OK;
      }else{
        return EVENT_RETURN_PASS;
      }
        
      break;
      //end of EVENT_TOUCH
      
    case EVENT_TOUCH_HOLD:
      return EVENT_RETURN_OK;
    break;

    case EVENT_DISTROY:
    	_static_array.clear();
    	_dynamic_array.clear();
    	_touch_array.clear();
      break;
    default:
      break;
  }//end of switch statement
  PROFILE_END
  return EVENT_RETURN_PASS;
}
