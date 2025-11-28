/**
 * @file linkerwx_wifi.ino
 * @brief Exemplo de uso da biblioteca LinkerWX v2.0 com WiFiClient
 * @details Este exemplo demonstra como usar a biblioteca LinkerWX v2.0
 * para comunicação TCP via WiFi usando ESP32. A biblioteca é usada apenas
 * para gerar comandos e interpretar respostas, enquanto o gerenciamento
 * da conexão TCP é feito manualmente.
 * 
 * @version 2.0.0
 * @author Labmaker
 */

 #include <WiFi.h>
 #include <WiFiClient.h>
 #include <LinkerWX.h>
 #include <string.h>
 #include <stdlib.h>
 
 // ============================================
 // STRUCT PACOTE
 // ============================================
 #pragma pack(push, 1)
 struct Pacote_t {
     uint8_t command;      ///< Comando do pacote
     uint8_t* value;       ///< Ponteiro para dados (alocado dinamicamente)
     uint8_t value_len;     ///< Tamanho dos dados
 };
 #pragma pack(pop)
 
 // ============================================
 // CONFIGURAÇÕES WIFI
 // ============================================
 const char* ssid = "Labmaker";        ///< Nome da rede WiFi
 const char* password = "Anglo2020pp";   ///< Senha da rede WiFi
 
 // ============================================
 // CONFIGURAÇÕES DO SERVIDOR
 // ============================================
 const char* serverIP = "168.231.99.137";  ///< IP do servidor TCP
 const uint16_t serverPort = 12345;         ///< Porta do servidor TCP
 
 // ============================================
 // CONFIGURAÇÕES LINKERWX
 // ============================================
 const String DEVICE_ID = "esp32_device";   ///< ID único do dispositivo
 const String GRUPO = "grupo1";            ///< Grupo do dispositivo
 const String TOKEN = "987654321";           ///< Token de autenticação
 
 // ============================================
 // OBJETOS
 // ============================================
 WiFiClient client;                         ///< Cliente WiFi para comunicação TCP
 LinkerWX linker;                           ///< Instância da classe LinkerWX
 
 // ============================================
 // VARIÁVEIS DE CONTROLE
 // ============================================
 bool deviceRegistered = false;            ///< Flag indicando se dispositivo está registrado
 uint32_t lastReconnectCheck = 0;          ///< Último tempo de verificação de reconexão
 const uint32_t RECONNECT_CHECK_INTERVAL = 30000; ///< Intervalo para verificar reconexão (30s)
 uint32_t lastRegisterTime = 0;            ///< Último tempo de registro (HI ou AGAIN)
 uint32_t lastPacoteSend = 0;              ///< Último tempo de envio de pacote
 const uint32_t PACOTE_SEND_INTERVAL = 55000; ///< Intervalo para enviar pacote (55s)
 
 // ============================================
 // CALLBACKS
 // ============================================
 
 /**
  * @brief Callback para mensagens SRV do servidor
  * @param response Estrutura com dados da resposta SRV
  */
 void onSRV(const SRVResponse& response) {
     Serial.print("[SRV] Tipo: ");
     
     switch (response.type) {
         case SRVType::HI:
             Serial.print("HI");
             Serial.print(" - Grupo: ");
             Serial.print(response.group);
             Serial.print(", ID: ");
             Serial.println(response.id);
             
             // Verifica se é confirmação do nosso registro
             if (response.group == GRUPO && response.id == DEVICE_ID) {
                 deviceRegistered = true;
                 lastRegisterTime = millis();
                 Serial.println("✅ Dispositivo registrado com sucesso!");
             }
             break;
             
         case SRVType::AGAIN:
             Serial.println("AGAIN - Dispositivo já estava registrado");
             deviceRegistered = true;
             lastRegisterTime = millis();
             break;
             
         case SRVType::WELCOME:
             Serial.println("WELCOME - Mensagem de boas-vindas");
             break;
             
         case SRVType::UNKNOWN:
         default:
             Serial.println("UNKNOWN - Tipo desconhecido");
             break;
     }
 }
 
 /**
  * @brief Serializa um Pacote_t para buffer binário
  * @param pacote Ponteiro para o pacote a serializar
  * @param buffer Buffer de saída
  * @param bufferSize Tamanho do buffer
  * @return Tamanho dos dados serializados (0 se erro)
  */
 size_t serializarPacote(const Pacote_t* pacote, uint8_t* buffer, size_t bufferSize) {
     if (pacote == nullptr || buffer == nullptr || bufferSize < 2) {
         return 0;
     }
     
     // Tamanho necessário: 1 byte (command) + 1 byte (value_len) + value_len bytes (value)
     size_t requiredSize = 2 + pacote->value_len;
     if (bufferSize < requiredSize) {
         return 0;
     }
     
     size_t pos = 0;
     
     // Serializar command
     buffer[pos++] = pacote->command;
     
     // Serializar value_len
     buffer[pos++] = pacote->value_len;
     
     // Serializar value (se houver)
     if (pacote->value_len > 0 && pacote->value != nullptr) {
         memcpy(&buffer[pos], pacote->value, pacote->value_len);
         pos += pacote->value_len;
     }
     
     return pos;
 }
 
 /**
  * @brief Deserializa um buffer binário para Pacote_t
  * @param buffer Buffer com dados serializados
  * @param bufferSize Tamanho do buffer
  * @param pacote Ponteiro para o pacote a preencher
  * @return true se deserializou com sucesso
  */
 bool deserializarPacote(const uint8_t* buffer, size_t bufferSize, Pacote_t* pacote) {
     if (buffer == nullptr || pacote == nullptr || bufferSize < 2) {
         return false;
     }
     
     size_t pos = 0;
     
     // Deserializar command
     pacote->command = buffer[pos++];
     
     // Deserializar value_len
     pacote->value_len = buffer[pos++];
     
     // Validar tamanho
     if (bufferSize < 2 + pacote->value_len) {
         return false;
     }
     
     // Alocar e copiar value
     if (pacote->value_len > 0) {
         pacote->value = (uint8_t*)malloc(pacote->value_len);
         if (pacote->value == nullptr) {
             return false;
         }
         memcpy(pacote->value, &buffer[pos], pacote->value_len);
     } else {
         pacote->value = nullptr;
     }
     
     return true;
 }
 
 /**
  * @brief Libera memória de um Pacote_t
  * @param pacote Ponteiro para o pacote
  */
 void liberarPacote(Pacote_t* pacote) {
     if (pacote != nullptr && pacote->value != nullptr) {
         free(pacote->value);
         pacote->value = nullptr;
         pacote->value_len = 0;
     }
 }
 
 /**
  * @brief Interpreta um Pacote_t recebido
  * @param pacote Ponteiro para o pacote
  */
 void interpretarPacote(const Pacote_t* pacote) {
     if (pacote == nullptr) {
         return;
     }
     
     Serial.print("[PACOTE] Command: 0x");
     Serial.print(pacote->command, HEX);
     Serial.print(", Value Len: ");
     Serial.print(pacote->value_len);
     
     if (pacote->value_len > 0 && pacote->value != nullptr) {
         Serial.print(", Value: ");
         for (uint8_t i = 0; i < pacote->value_len; i++) {
             if (pacote->value[i] < 0x10) {
                 Serial.print("0");
             }
             Serial.print(pacote->value[i], HEX);
             if (i < pacote->value_len - 1) {
                 Serial.print(" ");
             }
         }
     }
     Serial.println();
     
     // Aqui você pode adicionar lógica específica baseada no command
     switch (pacote->command) {
         case 0x01:
             Serial.println("  -> Comando 0x01 detectado");
             break;
         case 0x02:
             Serial.println("  -> Comando 0x02 detectado");
             break;
         default:
             Serial.print("  -> Comando desconhecido: 0x");
             Serial.println(pacote->command, HEX);
             break;
     }
 }
 
 /**
  * @brief Callback para mensagens RECV (dados recebidos - texto)
  * @param response Estrutura com dados recebidos
  */
 void onRECV(const RECVResponse& response) {
     Serial.print("[RECV] De ");
     Serial.print(response.sourceGroup);
     Serial.print(":");
     Serial.print(response.sourceId);
     Serial.print(" - ");
     Serial.println(response.data);
 }
 
 /**
  * @brief Callback para mensagens RECV_BINARY (dados binários recebidos)
  * @param response Estrutura com dados binários recebidos
  */
 void onRECVBinary(const RECVBinaryResponse& response) {
     Serial.print("[RECV_BINARY] De ");
     Serial.print(response.sourceGroup);
     Serial.print(":");
     Serial.print(response.sourceId);
     Serial.print(" - StructID: 0x");
     Serial.print(response.structId, HEX);
     Serial.print(", Tamanho: ");
     Serial.print(response.binaryDataLen);
     Serial.println(" bytes");
     
     // Verificar STRUCT_ID para determinar o tipo de dados
     const uint8_t STRUCT_ID_PACOTE = 0x01;
     
     if (response.structId == STRUCT_ID_PACOTE) {
         // Tentar deserializar como Pacote_t
         Pacote_t pacote;
         if (deserializarPacote(response.binaryData, response.binaryDataLen, &pacote)) {
             Serial.println("📦 Pacote binário recebido:");
             interpretarPacote(&pacote);
             liberarPacote(&pacote);
         } else {
             Serial.println("❌ Erro ao deserializar Pacote_t");
         }
     } else {
         // STRUCT_ID desconhecido - exibir dados brutos
         Serial.print("Dados binários (hex) - StructID desconhecido: ");
         for (size_t i = 0; i < response.binaryDataLen && i < 32; i++) {  // Limitar exibição
             if (response.binaryData[i] < 0x10) {
                 Serial.print("0");
             }
             Serial.print(response.binaryData[i], HEX);
             Serial.print(" ");
         }
         Serial.println();
     }
 }
 
 /**
  * @brief Callback para mensagens ERR (erros do servidor)
  * @param response Estrutura com informações do erro
  * 
  * @note Tratamento de erros específicos:
  * - Código 1 (ERR_NO_REG): Cliente não registrado
  * - Código 10 (ERR_FORCED_DISCONNECT): Desconexão forçada por novo dispositivo usando mesmo token+grupo+id
  * - Código 11 (ERR_INVALID_TOKEN): Tentativa de enviar mensagem para dispositivo com token diferente
  */
 void onERR(const ERRResponse& response) {
     Serial.print("[ERR] Código: ");
     Serial.print(response.code);
     Serial.print(" - ");
     Serial.println(response.message);
     
     // Se erro 1 (não registrado), marca como não registrado
     if (response.code == 1) {
         deviceRegistered = false;
     }
     
     // Se erro 10 (ERR_FORCED_DISCONNECT), reconecta automaticamente
     // Ocorre quando um novo dispositivo tenta registrar com o mesmo token+grupo+id
     if (response.code == 10) {
         Serial.println("⚠️ Desconexão forçada detectada! Novo dispositivo usando mesmo token+grupo+id.");
         Serial.println("   Reconectando...");
         deviceRegistered = false;
         client.stop();
         delay(1000);
         // A reconexão será feita automaticamente pelo checkReconnectServer()
     }
     
     // Se erro 11 (ERR_INVALID_TOKEN), informa sobre isolamento por token
     // Ocorre quando tenta enviar mensagem para dispositivo com token diferente
     if (response.code == 11) {
         Serial.println("⚠️ Erro: Tentativa de enviar mensagem para dispositivo com token diferente!");
         Serial.println("   Lembre-se: Mensagens só podem ser enviadas entre dispositivos com o mesmo token.");
         Serial.println("   Verifique se o token do destinatário corresponde ao seu token.");
     }
 }
 
 /**
  * @brief Callback para mensagens DEST (informações de destino)
  * @param response Estrutura com informações de destino
  */
 void onDEST(const DESTResponse& response) {
     Serial.print("[DEST] IP: ");
     Serial.print(response.ip);
     Serial.print(", Porta: ");
     Serial.println(response.port);
 }
 
 /**
  * @brief Callback genérico para erros de interpretação
  * @param errorCode Código do erro
  * @param command Comando que causou o erro
  */
 void onError(ErrorCode errorCode, const char* command) {
     Serial.print("[ERROR] Código: ");
     Serial.print((int)errorCode);
     Serial.print(" - Comando: ");
     Serial.println(command);
 }
 
 
 // ============================================
 // SETUP
 // ============================================
 void setup() {
     Serial.begin(115200);
     delay(1000);
     
     Serial.println("\n========================================");
     Serial.println("LinkerWX WiFi Example v2.0");
     Serial.println("========================================\n");
     
     // Configura LinkerWX
     Serial.println("Configurando LinkerWX...");
     if (!linker.setup(DEVICE_ID, GRUPO, TOKEN)) {
         Serial.println("❌ Erro ao configurar LinkerWX!");
         while(1) delay(1000);
     }
     Serial.println("✅ LinkerWX configurado");
     
     // Registra callbacks
     linker.registerSRVCallback(onSRV);
     linker.registerRECVCallback(onRECV);
     linker.registerRECVBinaryCallback(onRECVBinary);
     linker.registerERRCallback(onERR);
     linker.registerDESTCallback(onDEST);
     linker.registerErrorCallback(onError);
     Serial.println("✅ Callbacks registrados");
     
     Serial.println("\n✅ Setup completo! Aguardando respostas...\n");
 }
 
 bool conectarWiFi() {
     Serial.print("\nConectando ao WiFi: ");
     Serial.println(ssid);
     WiFi.begin(ssid, password);
     
     int tentativas = 0;
     while (WiFi.status() != WL_CONNECTED && tentativas < 3) {
         delay(1000);
         Serial.print(".");
         tentativas++;
     }
 
     if (WiFi.status() != WL_CONNECTED) {
         Serial.println("\n❌ Falha ao conectar WiFi!");
         return false;
     }
     
     Serial.println("\n✅ WiFi conectado!");
     Serial.print("IP: ");
     Serial.println(WiFi.localIP());
     return true;
 }
 
 bool isWiFiConnected() {
     return WiFi.status() == WL_CONNECTED;
 }
 
 bool conectarServidorTCP() {
     Serial.print("\nConectando ao servidor ");
     Serial.print(serverIP);
     Serial.print(":");
     Serial.println(serverPort);
     
     if (!client.connect(serverIP, serverPort)) {
         Serial.println("❌ Falha ao conectar ao servidor!");
         return false;
     }
     
     Serial.println("✅ Conectado ao servidor!");
     return true;
 }
 
 bool isServerConnected() {
     return client.connected();
 }
 
 void sendRegisterCommand() {
     
     if(!isServerConnected()) {
         Serial.println("❌ Servidor desconectado! Tentando reconectar...");
         return;
     }
     
     uint8_t buffer[256];
     size_t packetSize = linker.getCommandRegisterBytes(buffer, sizeof(buffer));
     if (packetSize > 0) {
         client.write(buffer, packetSize);
         Serial.print("Comando MYID enviado (");
         Serial.print(packetSize);
         Serial.println(" bytes)");
     } else {
         Serial.println("❌ Erro ao gerar comando de registro!");
     }
 }
 
 
 void checkReconnectServer() {
     if (!isServerConnected() && millis() - lastReconnectCheck >= RECONNECT_CHECK_INTERVAL) {
         lastReconnectCheck = millis();
         if(conectarServidorTCP()){
             delay(1000);
             sendRegisterCommand();
         }
     }
 }
 
 void sendKeepAliveCommand() {
     // Verifica se precisa enviar keep-alive
     if (linker.needSendKeepAlive() && (millis() - lastRegisterTime > 300)) {
         linker.updateKeepAliveTime();
         uint8_t buffer[16];
         size_t packetSize = linker.getCommandAliveBytes(buffer, sizeof(buffer));
         if (packetSize > 0) {
             client.write(buffer, packetSize);
             Serial.println("[Keep-Alive] Enviado");
         }
     }
 }
 
 void sendData(){
     // Exemplo: Envia dados a cada 10 segundos (apenas se registrado)
     static uint32_t lastSend = 0;
     if (deviceRegistered && millis() - lastSend >= 10000 && isServerConnected() && (millis() - lastRegisterTime > 300)) {
         lastSend = millis();
         
         // Exemplo: Envia dados simulados
         String dados = "temperatura=" + String(random(20, 30)) ;
         
         uint8_t buffer[256];
         size_t packetSize = linker.getCommandSendToBytes(GRUPO, "outro_device", dados, buffer, sizeof(buffer));
         if (packetSize > 0) {
             client.write(buffer, packetSize);
             Serial.print("[SEND] Enviado: ");
             Serial.println(dados);
         }
     }
 }
 
 void sendPacote() {
     // Envia Pacote_t a cada 55 segundos (apenas se registrado)
     if (deviceRegistered && millis() - lastPacoteSend >= PACOTE_SEND_INTERVAL && 
         isServerConnected() && (millis() - lastRegisterTime > 300)) {
         lastPacoteSend = millis();
         
         // Criar pacote de exemplo
         Pacote_t pacote;
         pacote.command = 0x01;  // Exemplo: comando 0x01
         pacote.value_len = 4;   // Exemplo: 4 bytes de dados
         
         // Alocar e preencher dados de exemplo
         pacote.value = (uint8_t*)malloc(pacote.value_len);
         if (pacote.value != nullptr) {
             // Exemplo: dados simulados (pode ser qualquer coisa)
             pacote.value[0] = 0xAA;
             pacote.value[1] = 0xBB;
             pacote.value[2] = 0xCC;
             pacote.value[3] = 0xDD;
             
             // Serializar pacote
             uint8_t serialBuffer[256];
             size_t serialSize = serializarPacote(&pacote, serialBuffer, sizeof(serialBuffer));
             
             if (serialSize > 0) {
                 // STRUCT_ID para identificar o tipo de pacote (0x01 = Pacote_t)
                 // IMPORTANTE: 0x1E é reservado e não deve ser usado
                 const uint8_t STRUCT_ID_PACOTE = 0x01;
                 
                 // Enviar usando SEND_BINARY (para dispositivo específico)
                 // ou BROADCAST_BINARY (para todo o grupo)
                 uint8_t packetBuffer[256];
                 size_t packetSize = linker.getCommandBroadcastBinaryBytes(GRUPO, STRUCT_ID_PACOTE, 
                                                                           serialBuffer, serialSize, 
                                                                           packetBuffer, sizeof(packetBuffer));
                 
                 if (packetSize > 0) {
                     client.write(packetBuffer, packetSize);
                     Serial.print("[PACOTE] Enviado (Broadcast) - Command: 0x");
                     Serial.print(pacote.command, HEX);
                     Serial.print(", StructID: 0x");
                     Serial.print(STRUCT_ID_PACOTE, HEX);
                     Serial.print(", Size: ");
                     Serial.println(serialSize);
                 } else {
                     Serial.println("❌ Erro ao gerar comando BROADCAST_BINARY!");
                 }
             } else {
                 Serial.println("❌ Erro ao serializar pacote!");
             }
             
             // Liberar memória
             liberarPacote(&pacote);
         } else {
             Serial.println("❌ Erro ao alocar memória para pacote!");
         }
     }
 }
 
 void readReceivedData(){
     // Lê dados do servidor (protocolo binário)
     if (client.available()) {
         uint8_t buffer[256];
         size_t bytesRead = client.readBytes(buffer, sizeof(buffer));
         
         if (bytesRead > 0) {
             // Processa dados recebidos (o método receiveData gerencia buffer interno e processa pacotes completos)
             linker.receiveData(buffer, bytesRead);
         }
     }
 }
 
 // ============================================
 // LOOP PRINCIPAL
 // ============================================
 void loop() {
     
     if (!isWiFiConnected()) {
         conectarWiFi();
     }
 
     checkReconnectServer();
 
     sendKeepAliveCommand();
 
     sendData();
     
     sendPacote();
     
     readReceivedData();
     // Pequeno delay para não sobrecarregar
     delay(10);
 }
 
 