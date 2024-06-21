#include "config_wifi_dialog.h"
#include <TelnetStream.h>

volatile bool KillScanTask;
void ListWifiNetworks(void *pvParameters);

WIFI_CONNECT_BUTTON* connect_button_pointer = 0;

void WiFiConnected( WiFiEvent_t event, WiFiEventInfo_t info){
    Serial.println("Connected to WiFi access point");
    if( connect_button_pointer ){
      connect_button_pointer->string = "Disconnect";
      connect_button_pointer->Draw(0);
    }
}

void WiFiDisconnected( WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("WiFiDisconnect");
  if( connect_button_pointer ){
    if( connect_button_pointer->connecting ){
      Serial.println("Could not connect to WiFi access point");
      Serial.print("Reason: ");
      Serial.println(info.disconnected.reason);
      try_to_connect_to_wifi = false;
      //ToDo: Properly see why the connection was not established.
      connect_button_pointer->string = "Bad Password";
      connect_button_pointer->Draw(0);
    }
  }
}

void WIFI_CONNECT_BUTTON::onClick(DIALOG* d){
  if( is_wifi_connected ){
    try_to_connect_to_wifi = false;
    connecting = false;
    WiFi.disconnect();
    string = "Connect";
    Draw(0);
  }else{
    try_to_connect_to_wifi = true;
    string = "Connecting";
    connecting = true;
    Draw(0);
  }       
}

EVENT_RETURN WIFI_DIALOG::Event_handle(EVENT_QUEUE e){
  switch(e.e){
           
    case EVENT_CREATE:
      lock_touch_events = true;
    
      this->Create(0,40,320,200,0x0000);                
      addTouchObject( &CLOSE);
      addTouchObject( &MORE);
      addTouchObject( &SSID);
      addTouchObject( &PASS);
      addTouchObject( &CONNECT);
      addTouchObject( &AUTOCONNECT);
      
      NETWORKLIST.content.push_back("Scanning....");
      addTouchObject( &NETWORKLIST );
      
      KillScanTask = false;
      xTaskCreateUniversal(ListWifiNetworks, "loopTask", 3000, &NETWORKLIST, 1, &ScanTaskHandle, 1);

      break;

    case EVENT_DRAW_ALL:
      AUTOCONNECT.init();
      break;
    
    case EVENT_DRAW_UPDATE:
      SSID.init();
      PASS.init();
      break;
      
    case KEYBOARD_OK:
      SSID.update();
      PASS.update();
      AUTOCONNECT.update();
            
    case KEYBOARD_CANCEL:
      SSID.close();
      PASS.close();
      AUTOCONNECT.close();      
      return EVENT_RETURN_OK;    
      
    case EVENT_DISTROY:   
      KillScanTask = true;
    break;  
  }
  return DIALOG::Event_handle(e);
} 

extern TaskHandle_t task_handles[];

int8_t NetworksFound = 0;
void ListWifiNetworks(void *pvParameters){
  DIALOG_LISTBOX* pram = (DIALOG_LISTBOX*) pvParameters;
  std::vector<std::string> networks;

  delay(2000);
  for(;;){ 
    networks.clear();
    if( KillScanTask )
      vTaskDelete( NULL );
      
    for(int a = 0; a <= 10; a++){
      if( task_handles[a] == 0)
        break;
      vTaskSuspend( task_handles[a] ); 
    }
    yield();
    NetworksFound = WiFi.scanNetworks();
    
    for(int a = 0; a <= 3; a++)  
      vTaskResume( task_handles[a] );
    
    LOGF("Network Count: %d\n\r", NetworksFound);
    if (NetworksFound <= 0) {
        networks.push_back("no networks found");
    } else {
      for (int8_t i = 0; i < NetworksFound; ++i) {
        String s = WiFi.SSID(i);
        if( s.length() )//&& WiFi.RSSI(i) > -70)
          networks.push_back( s.c_str() );
      }
    }
    
    if( KillScanTask )
      vTaskDelete( NULL );
    if( pram->content != networks ){
      xSemaphoreTake( pram->thread_safe_access, portMAX_DELAY);
      pram->content = networks;
      xSemaphoreGive( pram->thread_safe_access );
      pram->Draw(0);
    }
    
    delay(30000);
  }
}