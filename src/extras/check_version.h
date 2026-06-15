#ifndef CHECK_VERSION_H
#define CHECK_VERSION_H

#include <Arduino.h>

// Ensure version builder macro exists
#ifndef ESP_ARDUINO_VERSION_VAL
#define ESP_ARDUINO_VERSION_VAL(major, minor, patch) \
  (((major) << 16) | ((minor) << 8) | (patch))
#endif

#if defined(ESP_ARDUINO_VERSION)

// Ensure version component macros exist if the core does not declare them
#ifndef ESP_ARDUINO_VERSION_MAJOR
#define ESP_ARDUINO_VERSION_MAJOR 0
#endif

#ifndef ESP_ARDUINO_VERSION_MINOR
#define ESP_ARDUINO_VERSION_MINOR 0
#endif

#ifndef ESP_ARDUINO_VERSION_PATCH
#define ESP_ARDUINO_VERSION_PATCH 0
#endif

// Required ESP32 Arduino Core version
#define ESP32_CORE_REQUIRED ESP_ARDUINO_VERSION_VAL(3, 3, 6)

// Helpers to convert numbers to strings
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

// Full Arduino Core version string
#define ARDUINO_CORE_VERSION_STR \
  STR(ESP_ARDUINO_VERSION_MAJOR) "." \
  STR(ESP_ARDUINO_VERSION_MINOR) "." \
  STR(ESP_ARDUINO_VERSION_PATCH)

#if defined(DEBUG_DISPLAY_FK)
#pragma message("Using Arduino Core version: " ARDUINO_CORE_VERSION_STR)
#endif

#if ESP_ARDUINO_VERSION < ESP32_CORE_REQUIRED
  #error "ESP32 Arduino Core version is too old. Please update to version 3.3.6."
#endif

#if ESP_ARDUINO_VERSION > ESP32_CORE_REQUIRED
  #error "ESP32 Arduino Core version is too new. Please downgrade to version 3.3.6."
#endif

#if ESP_ARDUINO_VERSION == ESP32_CORE_REQUIRED
  #define ESP_ARDUINO_VERSION_OK
  #if defined(DEBUG_DISPLAY_FK)
    #pragma message("ESP32 Arduino Core version 3.3.6 detected. Version OK.")
  #endif
#endif

#else

#error "ESP32 Arduino Core version could not be detected. Please install ESP32 Arduino Core version 3.3.6."

#endif // ESP_ARDUINO_VERSION

#endif // CHECK_VERSION_H