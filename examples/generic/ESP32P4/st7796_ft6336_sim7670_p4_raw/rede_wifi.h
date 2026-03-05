#ifndef REDE_WIFI_H
#define REDE_WIFI_H

#include <WiFi.h>
#include "variaveis.h"
#include "display.h"

bool precisaGravarNoFirebase = false; ///< Flag para acionar rotina de gravação no Firebase.
bool precisaAtualizarNoFirebase = false; ///< Flag para acionar rotina de atualização no Firebase.

WiFiClient client; ///< Cliente TCP utilizado para conexão com o servidor.

/* @brief Conecta ao ponto de acesso Wi-Fi definido em `variaveis.h`.
 * Exibe status no display e na Serial até obter `WL_CONNECTED`.
 */
void conectarWiFi() {
    showMessageStatus("Conectando ao Wi-Fi...");
  Serial.print("Conectando ao Wi-Fi ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  showMessageStatus("Conectado no WiFi!");
  Serial.println("\nWi-Fi conectado!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

/* @brief Conecta o socket TCP ao servidor `serverIP:serverPort`.
 * @return true se conectou com sucesso, false caso contrário.
 */
bool conectarSocket() {
  Serial.printf("Tentando conectar ao servidor %s:%u ... ", serverIP, serverPort);
  showMessageStatus("Conectando ao servidor...");

  if (client.connect(serverIP, serverPort)) {
    showMessageStatus("Conectado ao servidor!");
    return true;
  } else {
    showMessageStatus("Falha na conexao ao servidor!");
    return false;
  }
}

/* @brief Envia uma mensagem via socket TCP, tentando reconectar se necessário.
 * @param mensagem String a ser enviada ao servidor (terminada por \n via println).
 */
void enviarDados(const String& mensagem) {
  if (client.connected()) {
    client.println(mensagem);  // envia a string
    showMessageStatus("Enviando dados...");
    Serial.print("Enviado: ");
    Serial.println(mensagem);
  } else {
    Serial.print("Não conectado. Tentando reconectar... ");
    if (conectarSocket()) {
      client.println(mensagem);
    }
  }
}

/* @brief Lê dados disponíveis do socket TCP até '\n'.
 * @return String recebida (sem espaços extras nas extremidades) ou vazia.
 */
String lerDados() {
  String recebido = "";

  if (client.connected() && client.available()) {
    recebido = client.readStringUntil('\n');
    recebido.trim();
    Serial.print("Recebido: ");
    Serial.println(recebido);
  }

  return recebido;
}

/* @brief Interpreta um comando textual recebido e dispara ações.
 * @param comando Texto do comando (e.g., "gravar", "atualizar").
 */
void interpretarComando(const String& comando) {
  if (comando.length() == 0) return;

  if (comando.equalsIgnoreCase("gravar")) {
    Serial.println("Comando: gravar — executando gravação...");
    // Aqui você poderia salvar em memória, SD, ou Firebase
    precisaGravarNoFirebase = true;
    enviarDados("ESP32: dados gravados");
  }else if (comando.equalsIgnoreCase("atualizar")) {
    Serial.println("Comando: atualizar — executando atualizaçao...");
    // Aqui você poderia salvar em memória, SD, ou Firebase
    precisaAtualizarNoFirebase = true;
    enviarDados("ESP32: dados atualizados");
  }
  else {
    Serial.print("Comando desconhecido: ");
    Serial.println(comando);
    enviarDados("ESP32: comando desconhecido -> " + comando);
  }
}

#endif