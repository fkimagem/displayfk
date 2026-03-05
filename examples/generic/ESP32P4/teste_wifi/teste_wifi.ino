#include <WiFi.h>

const char* ssid = "Labmaker";
const char* password = "Anglo2020pp";

void conectarWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;

  Serial.print("Conectando ao WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  int timeout = 5000;
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < timeout) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConectado!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFalha na conexão.");
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  conectarWiFi();
}

void loop() {
  // Aqui, para reconectar só chame conectarWiFi() se desconectado,
  // sem repetir WiFi.mode() ou WiFi.disconnect()
  if (WiFi.status() != WL_CONNECTED) {
    conectarWiFi();
  }
}