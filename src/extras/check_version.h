#ifndef CHECK_VERSION_H
#define CHECK_VERSION_H

#if defined(ESP_ARDUINO_VERSION)

// Helpers para converter números em strings
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

// Monta a string completa da versão
#define ARDUINO_CORE_VERSION_STR \
  STR(ESP_ARDUINO_VERSION_MAJOR) "." STR(ESP_ARDUINO_VERSION_MINOR) "." STR(ESP_ARDUINO_VERSION_PATCH)

// Mostra uma única linha no log de compilação
#pragma message("Versão Arduino Core: " ARDUINO_CORE_VERSION_STR)

// Verificação de versão mínima
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3,3,0)
  #define ESP_ARDUINO_VERSION_OK 1
#else
  #pragma message("Versão Arduino Core incompatível. Requer versão 3.3.0 ou superior.")
#endif

#endif // ESP_ARDUINO_VERSION
#endif // CHECK_VERSION_H
