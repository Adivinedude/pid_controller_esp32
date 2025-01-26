#ifndef CONFIG_WIFI_MORE_DIALOG_H
#define CONFIG_WIFI_MORE_DIALOG_H

#include "src/dialog_base.h"
#include "src/dialog_button.h"
#include "src/dialog_keyboard.h"
#include "src/dialog_listbox.h"

#include "command_interface.h"
#include "src/config.h"
#include "WiFi.h"

class WIFI_MORE_DIALOG: public DIALOG{
  public:
    EVENT_RETURN Event_handle(EVENT_QUEUE e);

    DIALOG_BUTTON_CLOSE_DIALOG CLOSE{         0, 160,  70, 40, ILI9341_BLUE,      ILI9341_PINK, "Back", 2, 0xFFFF};
    
};

#endif