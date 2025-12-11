#ifndef CHECK_VERSION_H
#define CHECK_VERSION_H

#include <Arduino.h>

// Garante que a macro de construção exista
#ifndef ESP_ARDUINO_VERSION_VAL
#define ESP_ARDUINO_VERSION_VAL(major, minor, patch) \
  (((major) << 16) | ((minor) << 8) | (patch))
#endif

#if defined(ESP_ARDUINO_VERSION)

// Garante macros de versão caso o core não os declare
#ifndef ESP_ARDUINO_VERSION_MAJOR
#define ESP_ARDUINO_VERSION_MAJOR 0
#endif

#ifndef ESP_ARDUINO_VERSION_MINOR
#define ESP_ARDUINO_VERSION_MINOR 0
#endif

#ifndef ESP_ARDUINO_VERSION_PATCH
#define ESP_ARDUINO_VERSION_PATCH 0
#endif

// Helpers para converter números em strings
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

// String completa da versão
#define ARDUINO_CORE_VERSION_STR \
  STR(ESP_ARDUINO_VERSION_MAJOR) "." \
  STR(ESP_ARDUINO_VERSION_MINOR) "." \
  STR(ESP_ARDUINO_VERSION_PATCH)

#pragma message("Usando versão ArduinoCore: " ARDUINO_CORE_VERSION_STR)

// Verificação de versão mínima
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3,3,0)
  #define ESP_ARDUINO_VERSION_OK
  #pragma message("Versão ArduinoCore OK.")
#else
  #pragma message("Versão ArduinoCore incompatível com ESP32P4. Requer 3.3.0 ou superior.")
#endif

#endif // ESP_ARDUINO_VERSION
#endif // CHECK_VERSION_H
