#include "gui.h"
#include "config.h"

extern SemaphoreHandle_t SPI_ACCESS;
extern TaskHandle_t task_handles[];
void ReportResources(){
PROFILE_START
  Serial.print("Time: ");       Serial.println((float)millis()/1000);
  Serial.print("CPU Speed: ");  Serial.println( getCpuFrequencyMhz());
  Serial.print("Total heap: "); Serial.println( ESP.getHeapSize());
  Serial.print("Free heap: ");  Serial.println( ESP.getFreeHeap());
  Serial.print("Total PSRAM: ");Serial.println( ESP.getPsramSize());
  Serial.print("Free PSRAM: "); Serial.println( ESP.getFreePsram());

  Serial.printf("Total heap used: %.2f%%\tTotal psram used: %.2f%%\tStack: %u\n", 
    ((float)ESP.getHeapSize()  - ESP.getFreeHeap())  / ESP.getHeapSize() *100,
    ((float)ESP.getPsramSize() - ESP.getFreePsram()) / ESP.getPsramSize() *100, 
    uxTaskGetStackHighWaterMark(NULL) );

  for( int a = 0; a !=7; ++a){
    if( task_handles[a] == 0)
      break;
    UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(task_handles[a]);
    Serial.printf("%d: %d\n\r", a, uxHighWaterMark);
  }
PROFILE_END
}

void mygui::Begin(){
PROFILE_START
  DisplaySetup();
  tft.setRotation(3);
  touch.setRotation(3);
  _t_time = _d_time = millis();
  _debounce = 0;
  _screen_off_timer = config_file.screen_timeout*60000;
  max_brightness = 255;
  _brightness = max_brightness;
PROFILE_END
}

void mygui::addDialog(DIALOG* d){
PROFILE_START
	d->gui = this;
  _dialog_list.push_back(d);
  EVENT_QUEUE z;
  
  z.e = EVENT_CREATE;
  d->Event_handle(z);  
  
  z.e = EVENT_DRAW_ALL;
  d->Event_handle(z);
PROFILE_END  
}

void mygui::addChildDialog(DIALOG* parent, DIALOG* child){
PROFILE_START
  child->parent_dialog = parent;
  addDialog(child);
PROFILE_END
}

void mygui::removeDialog(DIALOG* d){
PROFILE_START
	EVENT_QUEUE e;
  //notify parent
	if( d->parent_dialog ){
    PROFILE_LOG("EVENT_CHILD_CLOSED")
	  e.e = EVENT_CHILD_CLOSED;
	  d->parent_dialog->Event_handle(e);
	}
  //send dialog the event
	e.e = EVENT_DISTROY;
  PROFILE_LOG("REMOVE")
	d->Event_handle(e);
  //remove dialog from the list
  PROFILE_LOG("REMOVE FROM LIST")
	_dialog_list.remove(d);
	//addEvent(EVENT_DRAW_ALL);
PROFILE_END
}

void mygui::addEvent(EVENT e){ 
  EVENT_QUEUE t;
  t.e = e;
  t.t = last_touch;
  addEvent(t);
}

void mygui::addEvent(EVENT_QUEUE t){
PROFILE_START
PROFILE_LOG(t.e)
	if(t.e==EVENT_DRAW_UPDATE){
		if(draw_flag){
      PROFILE_END
			return;
    }
		draw_flag = true;
	}
	_event_queue.push(t);
  PROFILE_END
}

bool mygui::isEmpty(){
	return _dialog_list.empty();
}


#ifdef  debug_mygui_loop
 #define log_this(s) {LOG(s)}
#else
 #define log_this(s)
#endif

void mygui::loop(){
PROFILE_START
  uint32_t t = millis();
  //read touch events
  
  if( t-_t_time > TOUCH_UPDATE_INTERVAL ){
    PROFILE_LOG("\tTOUCH_UPDATE - START")
    _t_time = t;
    TS_Point this_touch = touch.getPoint();
    #ifdef debug_mygui_display_this_touch
      tft.setCursor(0,220);
      tft.setTextSize(1);
      tft.setTextColor(0xffff,0x0000);
      char TS_DEBUG_buff[50];
      sprintf(TS_DEBUG_buff, "x:%4d y:%4d z:%4d -- ", this_touch.x, this_touch.y, this_touch.z);
      tft.print(TS_DEBUG_buff);
    #endif
    
    GetTouchPoint(&this_touch);
      
    #ifdef debug_mygui_display_this_touch
      sprintf(TS_DEBUG_buff, "x:%3d y:%3d z:%3d ", this_touch.x, this_touch.y, this_touch.z);
      tft.print(TS_DEBUG_buff);
    #endif
     
    if( this_touch.z > 50 ){
      log_this(this_touch.z)
      if(_debounce < DEBOUNCE_COUNT){
        _debounce++;
        log_this("Touch active debounce\n")
      }else{      	
        log_this("Touch active\n")
				last_touch = this_touch; //read raw touchscreen data
				touch_flag = true;
				addEvent(EVENT_TOUCH);
				_screen_off_timer = t+config_file.screen_timeout*60000; //reset screen off timer
      }
    }else{
      //if the screen was touched last read but not this one add that event for button press detection
      if( _debounce > 3 ){
      	_debounce = 3;
      }else if(_debounce != 0){
      	_debounce--;
      	log_this("Touch deactive debounce\n")
      }else if( touch_flag ){  
        log_this("Touch deactive\n")         
        touch_flag=false;         
        last_touch.z = 0;               
        addEvent(EVENT_TOUCH);
      }
    }
    PROFILE_LOG("\tTOUCH_UPDATE - END")
  }
  
  //send out draw updates.
  if(t-_d_time > DRAW_UPDATE_INTERVAL){
    PROFILE_LOG("\tDRAW_UPDATE - START")
    _d_time = t;
    if( _brightness)
      addEvent(EVENT_DRAW_UPDATE);  
    //adjust screen brightness
    if( t > _screen_off_timer && _brightness > 0){
      //Serial.println("b-");
      if( config_file.screen_timeout != 0 ){
        _brightness--;
       //ledcWrite(0, _brightness);
       //digitalWrite(BACKLIGHT, _brightness>0?HIGH:LOW);
      }
    }else if( t < _screen_off_timer && _brightness == 0){
      //Serial.println("b+");
      _brightness = max_brightness;
      //addEvent(EVENT_DRAW_ALL);
      //ledcWrite(0, _brightness);
      //digitalWrite(BACKLIGHT, _brightness>0?HIGH:LOW);
    }
    PROFILE_LOG("\tDRAW_UPDATE - END")
  }

  if( _event_queue.empty() ){
    PROFILE_LOG("EXIT - empty q")
    return;
  }
    
  EVENT_QUEUE e = _event_queue.front();
  _event_queue.pop();

#ifdef debug_mygui_loop
  //print the queue to serial

  Serial.println("");
  for( std::queue <EVENT_QUEUE> temp_queue = _event_queue; !temp_queue.empty(); ){
    EVENT_QUEUE z = temp_queue.front();
    temp_queue.pop();
    Serial.print(z.e); Serial.print(" ");
  }
  Serial.println("");
  /////
  if( e.e == EVENT_TOUCH )
    log_this("EVENT_TOUCH")
#endif

  PROFILE_LOGF("\tDIALOG LOOP - START %d", _dialog_list.size())
  for( std::list<DIALOG*>::reverse_iterator iter = _dialog_list.rbegin(); iter != _dialog_list.rend(); ++iter){

		EVENT_RETURN rt = EVENT_RETURN_PASS;
    if( e.e == EVENT_DRAW_UPDATE || e.e == EVENT_DRAW_ALL){
      if( (*iter)->child_dialog == 0){
        if( xSemaphoreTake( SPI_ACCESS, 0 ) == pdTRUE ){
          PROFILE_LOG("\tEVENT_DRAW_UPDATE - START")
          rt = (*iter)->Event_handle(e);
          PROFILE_LOG("\tEVENT_DRAW_UPDATE - END")
          xSemaphoreGive( SPI_ACCESS );
        }
      }

      
    }else{
      PROFILE_LOG("\tEvent_handle - START")
      rt = (*iter)->Event_handle(e);
      PROFILE_LOG("\tEvent_handle - END")
    }
    
    //rt = (*iter)->Event_handle(e);

		switch( rt ){
			case EVENT_RETURN_OK: //dialog handled the event. Remove it.
        PROFILE_LOG("END - EVENT_RETURN_OK")
				return;

			case EVENT_RETURN_DISTROY: //dialog is requesting to close.
        PROFILE_LOG("\tEVENT_RETURN_DISTROY")
				removeDialog(*iter);
        PROFILE_END
				return;

      default:
        break;
		}

		if( (*iter)->lock_draw_events && e.e == EVENT_DRAW_UPDATE )
      break;	
    
  }
  PROFILE_LOG("\tDIALOG_LOOP - END")
  if(e.e == EVENT_DRAW_UPDATE){
  	draw_flag = false;
  }else if( e.e == EVENT_DRAW_ALL ){
    _brightness = 255;
  }
  
  //if(e.e > EVENT_TOUCH)
  //	LOGF("un-handled event: %d\n", e.e);
  PROFILE_END
 }
#undef log_this
