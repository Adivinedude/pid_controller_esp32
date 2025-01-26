#include "dialog_button.h"

EVENT_RETURN DIALOG_BUTTON::Event_handle(DIALOG* d, EVENT_QUEUE e){
  switch(e.e){
    case EVENT_TOUCH_YES:
      rollover_color = true;
      Draw(0);
      break;
    case EVENT_TOUCH_NO:
      rollover_color = false;
      Draw(0);
      break;
    case EVENT_TOUCH_PRESSED:
      rollover_color = false;
      Draw(0);
      this->onClick(d);
  }
  return EVENT_RETURN_OK;
}

void DIALOG_BUTTON_TEXT::Draw(GFXcanvas16* p){
  int16_t  x1, y1;
  uint16_t w, h;
  uint16_t w1, h1;
  char* work_string = 0;
  char* second_string = 0;
  {
    char* tmp_pointer =(char*) string;
    while(1){
      if( *tmp_pointer == 0 )
        break;
      if( *tmp_pointer == '\n' ){
        work_string = (char*) ps_malloc( strlen(string)+1 );
        strcpy(work_string, string);
        tmp_pointer = tmp_pointer - string + work_string;
        *tmp_pointer = 0;
        second_string = tmp_pointer+1;
        break;
      }
      tmp_pointer++;
    }
  }
  
  if(img)
      delete img; //erase the old one
  img = new GFXcanvas16(length, height, true);//make a new one
  w1 = 0; h1 = 0;
  
  img->fillRect(w1, h1, length, height, 0x0000);
  uint8_t a = 1;
  for(; a < 4; a++)
    img->drawRoundRect(w1+a,  h1+a, length-a*2,  height-a*2,  3, rollover_color?color1:color2);
  img->fillRect(w1+a, h1+a, length-a*2, height-a*2, !rollover_color?color1:color2);
  img->setTextSize(text_size);
  
  if( !work_string ){
    img->getTextBounds(string, 0, 0, &x1,&y1,&w,&h);
    img->setCursor(length/2-w/2,height/2-h/2);
    img->print(string);
  }else{
    uint16_t w2, h2; 
    img->getTextBounds(work_string  , 0, 0, &x1,&y1,&w ,&h );
    img->getTextBounds(second_string, 0, 0, &x1,&y1,&w2,&h2);
    
    img->setCursor( length/2-w/2  , height/2-h );
    img->print(work_string); 
    img->setCursor( length/2-w2/2 , height/2 );
    img->print(second_string);
    free(work_string);
  }
  
  if(p){  //if were using the objects image buffer.
    w1 = orgin_x, h1 = orgin_y;
    p->drawRGBBitmap(orgin_x, orgin_y, img->getBuffer(), length, height);
  } 
  update_flag = true; 
}

EVENT_RETURN DIALOG_BUTTON_ARROW::Event_handle(DIALOG* d, EVENT_QUEUE e){
  PROFILE_START
  switch(e.e){
    case EVENT_TOUCH_HOLD:
      onClick(d); 
      return EVENT_RETURN_OK;      
    break;
  }
  PROFILE_END
  return DIALOG_BUTTON::Event_handle(d, e);
}

void DIALOG_BUTTON_ARROW::Draw(GFXcanvas16* p){
  PROFILE_START
  //create image buffer
  if(img)
      delete img; //erase the old one
  img = new GFXcanvas16(length, height, true);//make a new one
  img->fillRect(0,0,length,height,!rollover_color?color1:color2);
  
  //fill image
  uint8_t a = 1;
  for(; a < 4; a++) //draw border
    img->drawRoundRect(a, a, length-a*2,  height-a*2,  3, rollover_color?color1:color2);
    
  //img->fillRect(a, a, length-a*2, height-a*2, !rollover_color?color1:color2);
  
  int16_t l,  h,
          x0, y0, 
          x1, y1, 
          x2, y2, 
          x3, y3, 
          x4, y4,
          x5, y5,
          x6, y6;
  int16_t angle;
  l = length/2; h = height/2;
/*
  0-----1
  |     |
  2-----3
4---------5
 \       /
   \   /
     6
*/
  //square 2 triangles
  x0 = l * -.2; y0 = h * -.8;
  x1 = l *  .2; y1 = h * -.8;
  x2 = l * -.2; y2 = 0;
  x3 = l *  .2; y3 = 0;
  //pointer 1 triangle
  x4 = l * -.6; y4 = 0;
  x5 = l *  .6; y5 = 0;
  x6 = 0      ; y6 = l * .8;

  
  angle = (direction%4) * 90;

  rotate_point( x0, y0, angle, l, h);
  rotate_point( x1, y1, angle, l, h);
  rotate_point( x2, y2, angle, l, h);
  rotate_point( x3, y3, angle, l, h);
  rotate_point( x4, y4, angle, l, h);
  rotate_point( x5, y5, angle, l, h);
  rotate_point( x6, y6, angle, l, h);
  
/*
  0-----1
  |     |
  2-----3
4---------5
 \       /
   \   /
     6
*/
    
  img->fillTriangle( x0, y0, x1, y1, x2, y2, rollover_color?color1:color2);
  img->fillTriangle( x1, y1, x3, y3, x2, y2, rollover_color?color1:color2);
  img->fillTriangle( x4, y4, x5, y5, x6, y6, rollover_color?color1:color2);
   
  if(p){  //if were using the objects image buffer.  
    p->drawRGBBitmap(orgin_x, orgin_y, img->getBuffer(), 0, 0);
  } 
  update_flag = true;
  PROFILE_END 
}

/*
void DIALOG_BUTTON_SHAPE::Draw(GFXcanvas16* p){
  uint16_t w1, h1;
  
  //create image buffer
  if(img)
      delete img; //erase the old one
  img = new GFXcanvas16(length, height, true);//make a new one
  img->fillRect(0,0,length,height,0x0000);
  
  //fill image
  uint8_t a = 1;
  for(; a < 4; a++) //draw border
    img->drawRoundRect(a, a, length-a*2,  height-a*2,  3, rollover_color?color1:color2);
    
  img->fillRect(a, a, length-a*2, height-a*2, !rollover_color?color1:color2);

  for( uint8_t a = 0; a <= _face_count; a+=3){
    int16_t b[6];    // array of points
    {
      int16_t *c = b;
      int16_t *d = &_faces[a*3];
      //load points from _face to b    
      *c = *d; c++;d++;
      *c = *d; c++;d++;
      *c = *d; c++;d++;
      *c = *d; c++;d++;
      *c = *d; c++;d++;
      *c = *d; c++;d++;
    }
    rotate_point( b[0], b[1], rotation, length/2, height/2);
    rotate_point( b[2], b[3], rotation, length/2, height/2);
    rotate_point( b[4], b[5], rotation, length/2, height/2); 
    img->fillTriangle( b[0],b[1],b[2],b[3],b[4],b[5], rollover_color?color1:color2);   
  }  
  update_flag = true;
}
*/
void rotate_point(int16_t &x, int16_t &y, int16_t angle, int16_t orgin_x, int16_t orgin_y){
  double _x, _y, x_, y_, c, s, a;
  a = angle * PI/180;
  //x_ = x - orgin_x;
  //y_ = y - orgin_y;
  x_ = x;
  y_ = y;
  
  c = cos( a );//1
  s = sin( a );//0
  
  _x = x_ * c - y_ * s;
  _y = x_ * s + y_ * c;
  
  x = orgin_x + _x;
  y = orgin_y + _y;  
}
