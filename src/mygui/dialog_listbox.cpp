#include "dialog_listbox.h"

EVENT_RETURN DIALOG_LISTBOX::Event_handle(DIALOG* d, EVENT_QUEUE e){

  switch(e.e){
    case EVENT_TOUCH:
      uint16_t x, y;
      x = e.t.x - d->orgin_x - orgin_x;
      y = e.t.y - d->orgin_y - orgin_y;

      if( x < 0 || y < 0 ){
        Serial.println("ERROR shouldnt happen");
        break;
      }
      
      //scroll bar
      
      if( x >= length-length*.1){
        scroll = 0;
        if( content.size() > max_space ){
          float _h, _y;
          _h = height; _y = y;
          scroll = _y / (_h/ (content.size()-max_space+1));
        }
        //LOGF("\n\rScroll Bar Moved- Scroll: %d\n\ry: %d  r_h: %d  size: %d  max: %d\n\r", scroll, y, row_height, content.size(), max_space);
      //selection
      }else{
        selection = (y / row_height) + scroll;
        //LOGF("\n\r Selection - %d  y: %d  rh: %d  s: %d\n\r", selection, y, row_height, scroll);
      }
      Draw(0);
      break;
    case EVENT_TOUCH_NO:
    case EVENT_TOUCH_PRESSED:
      if( !content.empty() && selection >= 0 && selection < content.size() ){
        xSemaphoreTake( thread_safe_access, portMAX_DELAY);
        this->onClick(d);
        xSemaphoreGive( thread_safe_access );
      }
      break;
  }

  return EVENT_RETURN_OK;
}

void DIALOG_LISTBOX::Draw(GFXcanvas16* p){

  int16_t  x1, y1;
  uint16_t w, h;
  uint16_t w1, h1;
  if(img)
      delete img; //erase the old one
  img = new GFXcanvas16(length, height, true);//make a new one
  w1 = 0; h1 = 0;
  
  img->fillRect(w1, h1, length, height, color2);
  
  for(uint8_t a = 1; a < 4; a++)
    img->drawRoundRect(w1+a,  h1+a, length-a*2,  height-a*2,  3, color1);
  img->setTextSize(text_size);
  
  img->setCursor(0,0);
  img->getTextBounds("A", 0, 0, &x1,&y1,&w ,&h );
  row_height = h;
  max_space = height/row_height; //how many rows can be displayed at once
  
  //LOGF("\n\rselection:%d   row_height:%d   max_space:%d   \n\r", selection, row_height, max_space);
  xSemaphoreTake( thread_safe_access, portMAX_DELAY);
  //LOGF("\n\rt:%d   u:%d\n\r", t, u); 
  img->setTextWrap(false);
  for( uint8_t iter = scroll; iter < content.size() && iter < scroll+max_space; iter++){
    if( iter == selection ){
      //LOGF("selected-  X:%d   Y:%d   L:%d    H:%d\n", w1+4, h*(iter-scroll), length-8, h);
      img->fillRect(w1+4, row_height*(iter-scroll)+4, length-8, row_height, color1);
    }
    img->setCursor( w1+4, row_height*(iter-scroll)+4 );
    img->println(content[iter].c_str());
  }
  if( max_space < content.size() ){
    int X, Y, L, H;
    X = length-length*.1;
    Y = scroll*(height/(content.size()+1-max_space));
    L = length*.1;
    H = height/max_space;
    //LOGF("scroll bar-  X:%d   Y:%d   L:%d    H:%d\n\r", X, Y, L, H);
    img->fillRect(X, Y, L, H, color1);
  }
  xSemaphoreGive( thread_safe_access );

  
  if(p){  //if were using the objects image buffer.
    w1 = orgin_x, h1 = orgin_y;
    p->drawRGBBitmap(orgin_x, orgin_y, img->getBuffer(), length, height);
  }  
  update_flag = true;
}
