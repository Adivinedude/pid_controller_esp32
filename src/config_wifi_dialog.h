#ifndef CONFIG_WIFI_DIALOG_H
#define CONFIG_WIFI_DIALOG_H
#include "mygui/dialog_base.h"
#include "mygui/dialog_button.h"
#include "mygui/dialog_keyboard.h"
#include "mygui/dialog_listbox.h"

#include "command_interface.h"
#include "mygui/config.h"
#include "WiFi.h"

#include "config_wifi_more_dialog.h"

//wifi dialog
extern volatile bool is_wifi_connected;
extern volatile bool try_to_connect_to_wifi;
class network_listbox: public DIALOG_LISTBOX{
  public:
    network_listbox( _DIALOG_LISTBOX_PRAM ):_DIALOG_LISTBOX_CALL{}
    
    void onClick(DIALOG* p){
      strncpy( config_file.ssid, content[selection].c_str(), sizeof(config_file.ssid));
    }

};

//////
//nasty trash to change the text in the wifi_connect button
void WiFiDisconnected( WiFiEvent_t event, WiFiEventInfo_t info);
void WiFiConnected( WiFiEvent_t event, WiFiEventInfo_t info);

class WIFI_CONNECT_BUTTON;
extern WIFI_CONNECT_BUTTON* connect_button_pointer;
// Trash finished
////
class WIFI_CONNECT_BUTTON: public DIALOG_BUTTON_TEXT{
  public:
    WIFI_CONNECT_BUTTON ( _DIALOG_BUTTON_TEXT_PRAM ) : _DIALOG_BUTTON_TEXT_CALL {
      WiFi.onEvent(WiFiDisconnected, SYSTEM_EVENT_STA_DISCONNECTED);
      WiFi.onEvent(WiFiConnected,    SYSTEM_EVENT_STA_CONNECTED);
      connect_button_pointer = this;
    };
    ~WIFI_CONNECT_BUTTON (){
      WiFi.removeEvent(SYSTEM_EVENT_STA_DISCONNECTED);
      WiFi.removeEvent(SYSTEM_EVENT_STA_CONNECTED);
      connect_button_pointer = 0;
    }
    
    bool connecting = false;
    void onClick(DIALOG* d);
};

class WIFI_MORE_BUTTON: public DIALOG_BUTTON_TEXT{
  public:
  WIFI_MORE_BUTTON ( _DIALOG_BUTTON_TEXT_PRAM ) : _DIALOG_BUTTON_TEXT_CALL {};
  void onClick(DIALOG* d){
    OpenChildDialog<WIFI_MORE_DIALOG> (this, d);
  }
};

class WIFI_DIALOG: public DIALOG{
  public:
    EVENT_RETURN Event_handle(EVENT_QUEUE e);

    DIALOG_BUTTON_CLOSE_DIALOG CLOSE{        0, 160,  70,  40, ILI9341_BLUE     , ILI9341_PINK,     "Back"   , 2, 0xFFFF};
    network_listbox            NETWORKLIST{  0,   0, 150, 150, ILI9341_DARKGREEN, ILI9341_BLUE, 2};
    DIALOG_BUTTON_INPUT_CHAR   SSID{       155,   0, 160,  40, ILI9341_DARKGREEN, ILI9341_BLUE, "SSID:\n",     2, 0xFFFF, 0, config_file.ssid, sizeof(config_file.ssid)};
    DIALOG_BUTTON_INPUT_CHAR   PASS{       155,  50, 160,  40, ILI9341_DARKGREEN, ILI9341_BLUE, "Password:\n\n\n", 2, 0xFFFF, 0, config_file.pass, sizeof(config_file.pass)};
    WIFI_CONNECT_BUTTON        CONNECT{    155, 100, 160,  40, ILI9341_DARKGREEN, ILI9341_BLUE, is_wifi_connected?(char*)"Disconnect":try_to_connect_to_wifi?(char*)"Connecting":(char*)"Connect", 2, 0xFFFF};
    DIALOG_BUTTON_INPUT_BOOL   AUTOCONNECT{155, 150, 160,  40, ILI9341_DARKGREEN, ILI9341_BLUE, "Auto Connect\n", 2, 0xFFFF, 0, &config_file.autoconnect_to_wifi};
    WIFI_MORE_BUTTON           MORE{        75, 160,  70,  40, ILI9341_BLUE     , ILI9341_PINK, "More", 2, 0xFFFF};
    TaskHandle_t ScanTaskHandle = NULL;    
};


#endif