#ifndef CONFIG_WIFI_MORE_DIALOG_H
#define CONFIG_WIFI_MORE_DIALOG_H

#include "mygui/dialog_base.h"
#include "mygui/dialog_button.h"
#include "mygui/dialog_keyboard.h"
#include "mygui/dialog_listbox.h"

#include "command_interface.h"
#include "mygui/config.h"
#include "WiFi.h"

class WIFI_MORE_DIALOG: public DIALOG{
  public:
    EVENT_RETURN Event_handle(EVENT_QUEUE e);

    DIALOG_BUTTON_CLOSE_DIALOG CLOSE{         0, 160,  70, 40, ILI9341_BLUE,      ILI9341_PINK, "Back", 2, 0xFFFF};
    
};

#endif