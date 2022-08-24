/*!
  @file   wxbeacon2_log.hpp
  @brief  logging for WxBeacon2
  @note depends on esp32-hal-log
*/
#ifndef WXBEACON2_LOG_HPP
#define WXBEACON2_LOG_HPP

namespace wxbeacon2
{
constexpr char tag[] = "WB2";
}

#ifndef NDEBUG

  #if defined(ARDUINO_ARCH_ESP32)

    #include <esp32-hal-log.h>

    #ifndef WB2_LOG_LEVEL
      #if defined(CORE_DEBUG_LEVEL)
        #pragma message "[WB2] Using CORE_DEBUG_LEVEL"
        #define WB2_LOG_LEVEL (CORE_DEBUG_LEVEL)
      #elif  defined(LOG_LOCAL_LEVEL)
        #pragma message "[WB2] Using LOG_LOCAL_LEVEL"
        #define WB2_LOG_LEVEL (LOG_LOCAL_LEVEL)
      #endif
    #else
      #pragma message "[WB2] Using defined log level"
    #endif
  #endif

  #define WB2_LOGE(format, ...) do { if(WB2_LOG_LEVEL >= ESP_LOG_ERROR)   { log_printf(ARDUHAL_LOG_FORMAT(E, format), ##__VA_ARGS__); } } while(0)
  #define WB2_LOGW(format, ...) do { if(WB2_LOG_LEVEL >= ESP_LOG_WARN)    { log_printf(ARDUHAL_LOG_FORMAT(W, format), ##__VA_ARGS__); } } while(0)
  #define WB2_LOGI(format, ...) do { if(WB2_LOG_LEVEL >= ESP_LOG_INFO)    { log_printf(ARDUHAL_LOG_FORMAT(I, format), ##__VA_ARGS__); } } while(0)
  #define WB2_LOGD(format, ...) do { if(WB2_LOG_LEVEL >= ESP_LOG_DEBUG)   { log_printf(ARDUHAL_LOG_FORMAT(D, format), ##__VA_ARGS__); } } while(0)
  #define WB2_LOGV(format, ...) do { if(WB2_LOG_LEVEL >= ESP_LOG_VERBOSE) { log_printf(ARDUHAL_LOG_FORMAT(V, format), ##__VA_ARGS__); } } while(0)

// defined(NDEBUG)
#else

  #define WB2_LOGE(format, ...) 
  #define WB2_LOGW(format, ...) 
  #define WB2_LOGI(format, ...) 
  #define WB2_LOGD(format, ...) 
  #define WB2_LOGV(format, ...)

#endif

//
#endif
