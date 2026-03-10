// Versao do core ESP32 em tempo de compilacao via #pragma message
#pragma message("========================================")

#ifdef ESP_ARDUINO_VERSION_MAJOR
  #pragma message("Core ESP32 detectado via ESP_ARDUINO_VERSION")
#else
  #pragma message("AVISO: Macros ESP_ARDUINO_VERSION nao disponiveis (core muito antigo?)")
#endif

// Macros nao podem ser expandidas diretamente no #pragma message de forma portavel,
// mas podemos usar o truque abaixo para stringificar o valor:
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#ifdef ESP_ARDUINO_VERSION_MAJOR
  #pragma message("Core Major : " TOSTRING(ESP_ARDUINO_VERSION_MAJOR))
  #pragma message("Core Minor : " TOSTRING(ESP_ARDUINO_VERSION_MINOR))
  #pragma message("Core Patch : " TOSTRING(ESP_ARDUINO_VERSION_PATCH))
#endif

#ifdef IDF_VER
  #pragma message("ESP-IDF Version : " IDF_VER)
#endif

#ifdef ARDUINO_BOARD
  #pragma message("Board : " TOSTRING(ARDUINO_BOARD))
#endif

#ifdef ARDUINO
  #pragma message("Arduino IDE Version : " TOSTRING(ARDUINO))
#endif

#pragma message("========================================")

// -------------------------------------------------------

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("========================================");
  Serial.println("   ESP32 Core Version Checker");
  Serial.println("========================================");

#ifdef ESP_ARDUINO_VERSION_MAJOR
  Serial.printf("Core Version      : %d.%d.%d\n",
    ESP_ARDUINO_VERSION_MAJOR,
    ESP_ARDUINO_VERSION_MINOR,
    ESP_ARDUINO_VERSION_PATCH);

  Serial.printf("Core Version (hex): 0x%06X\n", ESP_ARDUINO_VERSION);
#else
  Serial.println("Macros ESP_ARDUINO_VERSION nao disponiveis.");
  Serial.println("Seu core ESP32 pode ser muito antigo (< 2.0.0).");
#endif

#ifdef ARDUINO
  Serial.printf("Arduino IDE Version: %d\n", ARDUINO);
#endif

#ifdef ARDUINO_VARIANT
  Serial.printf("Board Variant     : %s\n", ARDUINO_VARIANT);
#endif

#ifdef ARDUINO_BOARD
  Serial.printf("Board             : %s\n", ARDUINO_BOARD);
#endif

#ifdef CONFIG_IDF_TARGET
  Serial.printf("IDF Target        : %s\n", CONFIG_IDF_TARGET);
#endif

#ifdef IDF_VER
  Serial.printf("ESP-IDF Version   : %s\n", IDF_VER);
#endif

  Serial.println("========================================");
}

void loop() {
  // Nada aqui
}