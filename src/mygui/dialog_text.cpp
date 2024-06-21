#include "dialog_text.h"

void DIALOG_TEXT::Draw(GFXcanvas16* p){
  int16_t  x1, y1;
  uint16_t w, h;
  //ToDo: rewrite using image buffer, then copy to *p
  if(p==0){  //if were using the objects image buffer.
    if(img)
      delete img; //erase the old one
    img = new GFXcanvas16(length, height, true);//make a new one
    p = img;
    p->fillRect(0, 0, length, height, text_background_color);
    p->setCursor(0,0);
  }else{// if were using the callers image buffer
    p->setTextSize(text_size);
    p->getTextBounds(string, 0, 0, &x1, &y1, &w, &h);
    if( length == 0)
      length = w; 
    if( height == 0)
      height = h;
    p->fillRect(orgin_x, orgin_y, length, height, text_background_color);
    p->setCursor(orgin_x, orgin_y);
  }  
  p->setTextSize(text_size);
  p->print(string);
  update_flag = true;
}

