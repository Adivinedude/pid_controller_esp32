#include "dialog_object.h"

DIALOG_OBJECT::~DIALOG_OBJECT(){
	Trash();
}

GFXcanvas16* DIALOG_OBJECT::GetImg(){
  if( img == 0 )
    this->Draw(0);
  return img;
}

void DIALOG_OBJECT::Trash(){
  if( img )
    delete img;
  img = 0;
}

bool DIALOG_OBJECT_DYNAMIC::TouchInBounds(TS_Point t){
  return t.x >= orgin_x && t.x <= orgin_x+length && t.y >= orgin_y && t.y <= orgin_y+height;
}
