#ifndef MY_DEBUG_H
#define MY_DEBUG_H
  
  //General message settings - uncomment to enable
  #define DEBUG_VERBOSE //report all LOG entrys
  //#define DEBUG_PROFILE //report all PROFILE entrys
  //#define DEBUG_CMD_PROFILE

  //Message output settings
  #define DEBUG_TO_SERIAL //send messages to Serial
  #define DEBUG_TO_TELNET //send messages to TelnetStream

  //short source code file name
  #define __FILENAME__ __FILE__//(strrchr(__FILE__, '/') ? strrchr(__FILE__,'/') + 1 : __FILE__)
  
  #include <string>
  inline std::string methodName(const std::string& prettyFunction)
  {
      size_t colons = prettyFunction.find("::");
      size_t begin = prettyFunction.substr(0,colons).rfind(" ") + 1;
      size_t end = prettyFunction.rfind("(") - begin;

      return prettyFunction.substr(begin,end) + "()";
  }
  #define __METHOD_NAME__ methodName(__PRETTY_FUNCTION__).c_str()
  #define DEBUG_LOG_STRING "%s:%d %s ", __FILENAME__, __LINE__, __METHOD_NAME__
  #define DEBUG_LOG_SERIAL(x) Serial.printf(DEBUG_LOG_STRING); Serial.println(x);
  #define DEBUG_LOG_TELNET(x) TelnetStream.printf(DEBUG_LOG_STRING); TelnetStream.println(x);

  //void MY_DEBUG_LOG()

  #ifdef DEBUG_TO_TELNET
    #include <TelnetStream.h> 
  #endif

  //DEBUG LOG(println) method  LOGF (printf);
  #ifndef DEBUG_VERBOSE
    #define LOG(x)
    #define LOGF(...)
    #define DEBUG_PAUSE
  #else
    #ifdef DEBUG_TO_SERIAL
      #ifdef DEBUG_TO_TELNET
        #define LOG(x){ DEBUG_LOG_SERIAL(x) DEBUG_LOG_TELNET(x) }
      #else
        #define LOG(x){ DEBUG_LOG_SERIAL(x) }
      #endif
    #else
      #ifdef DEBUG_TO_TELNET
        #define LOG(x){ DEBUG_LOG_TELNET(X) }
      #endif
    #endif    
    #define DEBUG_PAUSE LOG("") while(1){Serial.print('.');delay(2000);}
    #define LOGF(...) { char PROFILE_BUFFER[100]; snprintf(PROFILE_BUFFER, 99, __VA_ARGS__); LOG(PROFILE_BUFFER) }
  #endif

  #ifdef DEBUG_PROFILE   
    #define PROFILE_START LOG( "- ENTER")
    #define PROFILE_END   LOG( "- EXIT")
    #define PROFILE_LOG(x)  LOG(x)
    #define PROFILE_LOGF(...) LOGF(__VA_ARGS__)
  #else
    #define PROFILE_START 
    #define PROFILE_END 
    #define PROFILE_LOG(x)
    #define PROFILE_LOGF(...) 
  #endif

  #ifdef DEBUG_CMD_PROFILE   
    #define CMD_PROFILE_START LOG( "- ENTER")
    #define CMD_PROFILE_END   LOG( "- EXIT")
    #define CMD_PROFILE_LOG(x)  LOG(x)
    #define CMD_PROFILE_LOGF(...) LOGF(__VA_ARGS__)
  #else
    #define CMD_PROFILE_START 
    #define CMD_PROFILE_END 
    #define CMD_PROFILE_LOG(x)
    #define CMD_PROFILE_LOGF(...) 
  #endif
#endif