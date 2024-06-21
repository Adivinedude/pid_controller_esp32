#ifndef COMMAND_TASK_H
#define COMMAND_TASK_H

    #define buffer_size 100
    #include "command_interface.h"
    #include <TelnetStream.h>

    //handles input from user
    void handle_input( Stream &input_stream , uint8_t &counter, char* input_buffer);

    //performs wifi connection and OTA
    void OTA_TASK_LOOP(void *pvParameters);

    //Gets input from telnet
    void TELNET_TASK_LOOP(void *pvParameters);

    //Gets input from serial monitor(USB)
    void SERIAL_TASK_LOOP(void *pvParameters);

    //ToDo: MODBUS

#endif