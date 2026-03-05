#ifndef MODEM_H
#define MODEM_H

#include <Arduino.h>
#include "variaveis.h"
#include "display.h"

#define CRLF String("\r\n") ///< Sequência de quebra de linha usada em protocolos de texto (CRLF).
#define END_OF_TRANSMISSION String("\x1A") ///< Sequência CTRL+Z para finalizar transmissões (ex.: envio de dados em modems).


/** @brief Configuração de APN (Access Point Name) para contexto de dados. */
struct APN_t
{
    String url;
    String user;
    String password;
};

/** @brief Representa um cabeçalho HTTP para uso com comandos CCH*. */
struct CCHHeader
{
    String name;
    String value;

    /* @brief Converte o cabeçalho para o formato "Nome: Valor". */
    String toString() const
    {
        return name + ": " + value;
    }
};

/** @brief Estrutura de resposta de um comando AT. */
struct ResponseAT
{
    bool success;
    String response;

    /* @brief Converte a resposta para texto legível para depuração. */
    String toString()
    {
        return "Success: " + String(success ? "true" : "false") +
               "\nResp: " + String(response);
    }
};

const APN_t apnVivo = {.url = "zap.vivo.com.br", .user = "", .password = ""}; ///< APN da operadora Vivo.
const APN_t apnClaro = {.url = "claro.com.br", .user = "claro", .password = "claro"}; ///< APN da operadora Claro.
const APN_t apnTim = {.url = "timbrasil.br", .user = "tim", .password = "tim"}; ///< APN da operadora TIM.

const APN_t currentApn = apnTim; ///< APN atualmente selecionada.

const uint8_t pdpContextId = 1; ///< Identificador do contexto PDP.
const uint8_t connectionFirebaseId = 0; ///< Identificador do contexto SSL para Firebase.


/* @brief Inicializa a porta Serial1 para comunicação com o modem. */
void startModemSerial(){
    Serial1.begin(115200, SERIAL_8N1, RX1_PIN, TX1_PIN);
}


/* @brief Separa um URL completo em domínio e caminho.
 * @param completeUrl URL completo (pode conter protocolo).
 * @param domain Saída com o domínio extraído.
 * @param path Saída com o caminho (path) extraído.
 * @return true se domínio e caminho foram extraídos, false caso contrário.
 */
bool separetUrl(const String &completeUrl, String &domain, String &path)
{
    String originalUrl = completeUrl;

    // Remove protocol from originalUrl
    int indexOfDoubleSlash = originalUrl.indexOf("//");
    if (indexOfDoubleSlash != -1)
    {
        originalUrl = originalUrl.substring(indexOfDoubleSlash + 2);
    }

    // Separent domain from path
    int indexOfSlash = originalUrl.indexOf("/");
    if (indexOfSlash != -1)
    {
        domain = originalUrl.substring(0, indexOfSlash);
        path = originalUrl.substring(indexOfSlash);
    }

    return domain != "" && path != "";
}


/* @brief Encaminha dados entre Serial (USB) e Serial1 (modem) para monitoramento. */
void serialModemMonitor()
{
    // Se houver dados disponíveis no Serial (USB -> ESP)
    while (Serial.available())
    {
        char c = Serial.read();
        Serial1.write(c); // Envia para Serial1
    }

    // Se houver dados disponíveis no Serial1 (ESP -> USB)
    while (Serial1.available())
    {
        char c = Serial1.read();
        Serial.write(c); // Envia para o Serial Monitor
    }
}

/* @brief Monta uma requisição HTTP/1.1 em texto com cabeçalhos e corpo opcionais.
 * @param method Método HTTP (ex.: "POST", "PATCH").
 * @param domain Domínio (Host) do servidor.
 * @param path Caminho da requisição.
 * @param headers Array de cabeçalhos.
 * @param headersSize Tamanho do array de cabeçalhos.
 * @param data Corpo de dados (opcional).
 * @return Texto da requisição HTTP finalizada.
 */
String createHTTPRequest(const String &method, const String &domain, const String &path, CCHHeader headers[], uint8_t headersSize, const String &data){
    String retorno = method + " " + path + " HTTP/1.1" + CRLF;
    retorno += "Host: " + domain + CRLF;
    for(uint8_t i = 0; i < headersSize; i++){
        retorno += headers[i].toString() + CRLF;
    }
    retorno += CRLF + (headersSize == 0 ? CRLF : "");
    if(data.length() > 0){
        retorno += data + CRLF + CRLF;
    }
    return retorno;
}


/* @brief Escreve bytes diretamente no Serial1 do modem (sem acrescentar \n). */
void onlyWriteOnModem(const String& command){
        Serial.print("Write on serial:[[");
        Serial.print(command.c_str());
        Serial.println("]]");
        Serial1.write(command.c_str());
}

/* @brief Envia um comando (com ou sem prefixo AT) e aguarda resposta.
 * @param command Comando a enviar (sem CR).
 * @param expectedResponse Trecho esperado na resposta para considerar sucesso.
 * @param timeout Tempo máximo de espera (ms).
 * @return Estrutura ResponseAT com status e resposta completa.
 */
ResponseAT sendCommand(const String &command, const String &expectedResponse, unsigned long timeout){
    ResponseAT retorno{.success = false, .response = ""};
    if (expectedResponse == "")
    {
        Serial.print("NAO ENVIADO (sem expectedResponse): ");
        Serial.println(command);
        return retorno;
    }

    Serial.print("Enviando:[[");
    Serial.print(command.c_str());
    Serial.println("]]");
    
    Serial1.println(command);
    delay(100);

    

    String resposta = "";
    resposta.reserve(1024);


    // Ler tudo que chega em Serial1 até ficar vazio ou timeout ou resposta esperada encontrada && vazio
    bool responseFound = false;
    bool errorStringFound = false;
    unsigned long startTime = millis();
    while (millis() - startTime < timeout && !responseFound && !errorStringFound)
    {
        if(Serial1.available()){
            resposta += (char)Serial1.read();

            responseFound = resposta.indexOf(expectedResponse) != -1;
            errorStringFound = resposta.indexOf("ERROR") != -1;
        }
    }

    // Continu lendo Serial1 até acabar tudo
    while (Serial1.available())
    {
        resposta += (char)Serial1.read();
    }

    retorno.success = responseFound;
    retorno.response = resposta;

    Serial.println(retorno.toString());
    return retorno;
}

/* @brief Envia um comando AT, adicionando prefixo "AT"/"AT+" se necessário.
 * @param command Comando AT (pode iniciar com '+').
 * @param expectedResponse Resposta esperada.
 * @param timeout Timeout em ms.
 * @return Estrutura ResponseAT com sucesso e resposta.
 */
ResponseAT sendAT(const String &command, const String &expectedResponse = "", unsigned long timeout = 10000)
{
    String prefix = command.startsWith("+") ? "AT" : "AT+";
    return sendCommand(prefix + command, expectedResponse, timeout);
    
}

/* @brief Fecha a conexão SSL identificada pelo session_id. */
ResponseAT closeSSLConnection(uint8_t session_id)
{
    Serial.println("Close SSL connection: " + String(session_id));
    showMessageStatus("Close SSL connection...");

    String command = "+CCHCLOSE=" + String(session_id);
    ResponseAT response = sendAT(command, "+CCHCLOSE:", 120000);
    return response;
}

/* @brief Inicializa o serviço SSL no modem. */
ResponseAT startSSLService()
{
    ResponseAT response = sendAT("+CCHSTART", "OK", 120000);
    return response;
}


/* @brief Abre conexão SSL para host/porta usando sessão informada. */
ResponseAT openSSLConnection(uint8_t session_id, const String &server_addr, uint16_t server_port)
{
    startSSLService();

    showMessageStatus("Open SSL connection...");
    Serial.println("Open SSL connection: " + server_addr + ":" + String(server_port));
    // AT+CCHOPEN=<session_id>,<host>,<port>[,<client_type>,[<bind_port>]]
    String command = "+CCHOPEN=" + String(session_id) + ",\"" + String(server_addr.c_str()) + "\"," + String(server_port) + ",2";
    ResponseAT response = sendAT(command, "+CCHOPEN:", 120000);
    return response;
}




/* @brief Configura modo de relatório de eventos SSL no modem. */
ResponseAT setSSLReportMode(uint8_t report_mode, uint8_t receive_mode)
{
    //__unused(receive_mode);
    String command = "+CCHSET=" + String(report_mode);// + "," + String(receive_mode);
    ResponseAT response = sendAT(command, "OK", 120000);
    return response;
}

/* @brief Configura parâmetro do contexto SSL (ex.: versão TLS, authmode). */
ResponseAT configureSSLContextNumber(uint8_t ssl_ctx_index, const String &ssl_para, uint8_t value)
{
    // AT+CSSLCFG="sslversion",<ssl_ctx_index>,<sslversion>
    String command = "+CSSLCFG=\"" + String(ssl_para.c_str()) + "\"," + String(ssl_ctx_index) + "," + String(value);
    ResponseAT response = sendAT(command, "OK", 120000);
    return response;
}

/* @brief Configura autenticação do contexto PDP (APN).
 * @param cid Context ID.
 * @param auth_prot 0=sem auth, 1=PAP, 2=CHAP.
 * @param userid Usuário do APN.
 * @param password Senha do APN.
 */
ResponseAT setPDPAuthentication(uint8_t cid, uint8_t auth_prot, const String &userid, const String &password)
{
    
    String command = "+CGAUTH=" + String(cid) + "," + String(auth_prot);
    if (userid.length() > 0)
    {
        command += ",\"" + String(userid.c_str()) + "\"";
    }
    if (password.length() > 0)
    {
        command += ",\"" + String(password.c_str()) + "\"";
    }
    
    ResponseAT response = sendAT(command, "OK");
    return response;
}

/* @brief Define contexto PDP (tipo, APN e parâmetros opcionais).
 * @param cid Context ID.
 * @param pdp_type Tipo (ex.: "IP").
 * @param apn APN.
 * @param pdp_addr Endereço alocado (opcional).
 * @param d_comp Compressão de dados.
 * @param h_comp Compressão de cabeçalho.
 * @param ipv4_addr_alloc Alocação IPv4.
 * @param request_type Tipo de requisição.
 */
ResponseAT setPDPContext(uint8_t cid, const String &pdp_type, const String &apn,
                             const String &pdp_addr, uint8_t d_comp, uint8_t h_comp,
                             uint8_t ipv4_addr_alloc, uint8_t request_type)
{

    String command = "+CGDCONT=" + String(cid) + ",\"" + String(pdp_type.c_str()) + "\",\"" + String(apn.c_str()) + "\"";
    if (pdp_addr.length() > 0)
    {
        command += ",\"" + String(pdp_addr.c_str()) + "\"";
    }
    if (d_comp > 0)
        command += "," + String(d_comp);
    if (h_comp > 0)
        command += "," + String(h_comp);
    if (ipv4_addr_alloc > 0)
        command += "," + String(ipv4_addr_alloc);
    if (request_type > 0)
        command += "," + String(request_type);
    
    ResponseAT response = sendAT(command, "OK");
    return response;
}

/* @brief Aguarda aparecer um trecho específico na Serial1 até o timeout. */
void waitForResponde(const String& waitFor, unsigned long timeout = 10000){
    Serial.println("Wait for responde: " + waitFor);
    String response = "";
    unsigned long startTime = millis();
    while (millis() - startTime < timeout && response.indexOf(waitFor) == -1)
    {
        if(Serial1.available()){
            response += (char)Serial1.read();
        }
    }
}

/* @brief Aguarda um evento "+CCHRECV:" e lê a quantidade indicada de bytes.
 * @return ResponseAT com sucesso e payload recebido.
 */
ResponseAT waitForSSLResponse(unsigned long timeout = 10000){
    ResponseAT retorno = {false, ""};
    // Need to keep reading Serial1 until buffer has line like '+CCHRECV: DATA,0,323\n' or '+CCHRECV: DATA,0,323\r\n'
    // Separate by ',', the last value is amount of bytes to read
    // Read all bytes then return content of read bytes
    Serial.println("Wait for SSL response...");
    unsigned long startTime = millis();
    String line = "";
    String response = "";

    while (millis() - startTime < timeout) {
        // Lê dados recebidos do modem (Serial1)
        while (Serial1.available()) {
            char c = Serial1.read();
            line += c;

            // Quando encontrar uma linha completa
            if (c == '\n') {
                line.trim();

                // Exemplo de linha esperada: +CCHRECV: DATA,0,323
                if (line.startsWith("+CCHRECV:")) {
                    // Divide a linha pelos separadores ','
                    int lastComma = line.lastIndexOf(',');
                    if (lastComma != -1) {
                        int dataLength = line.substring(lastComma + 1).toInt();

                        Serial.print("Bytes esperados: ");
                        Serial.println(dataLength);

                        // Lê o número de bytes indicado
                        unsigned long readStart = millis();
                        while ((millis() - readStart < 2000) && (response.length() < dataLength)) {
                            if (Serial1.available()) {
                                response += (char)Serial1.read();
                            }
                        }

                        Serial.println("Recebido (hex):");
                        for (size_t i = 0; i < response.length(); i++) {
                            Serial.print(response[i], HEX);
                            Serial.print(' ');
                        }
                        Serial.println();
                        retorno.response = response;
                        retorno.success = true;
                        Serial.println("Retorno: " + retorno.toString());
                        return retorno;
                    }
                }

                // Reseta buffer da linha para próxima leitura
                line = "";
            }
        }
    }

    Serial.println("Timeout aguardando resposta SSL");
    return retorno;
}


/* @brief Envia dados via SSL após receber o prompt '>' do modem. */
void sendSSLData(uint8_t session_id, uint16_t send_length, const String &data)
{
    showMessageStatus("Send data SSL...");
    Serial.println("Send data SSL: " + data);
    String command = "+CCHSEND=" + String(session_id) + "," + String(send_length);
    ResponseAT resp = sendAT(command, ">", 120000);
    // espera '>'
    if(resp.response.indexOf(">") >= 0){
        Serial.println("Modem allowed to send data");
        onlyWriteOnModem(data);
        waitForResponde("+CCHSEND:", 120000);
    }
}




/* @brief Realiza uma requisição HTTP POST via canal SSL do modem.
 * @param url URL completa (https://...).
 * @param port Porta do servidor (tipicamente 443).
 * @param headers Cabeçalhos HTTP.
 * @param headersSize Tamanho dos cabeçalhos.
 * @param closeAfterSend Fecha a conexão após envio.
 * @param numberOfConnection ID da sessão SSL.
 * @param data Corpo da requisição.
 * @return true em caso de fluxo concluído sem erro crítico.
 */
bool requestSSLPost(const String& url, uint16_t port, CCHHeader* headers, uint8_t headersSize, bool closeAfterSend, uint8_t numberOfConnection, String data){
    showMessageStatus("Request SSL POST...");
    Serial.println("Request SSL POST: " + url);
    String originalUrl = url;
    String domain = "";
    String path = "";

    bool validUrl = separetUrl(url, domain, path);
    if(!validUrl){
        Serial.println("Error separating domain and path from url:[[" + originalUrl + "]]");
        return false;
    }

    // Check if Content-Length is already in headers
    bool hasContentLength = false;
    for(uint8_t i = 0; i < headersSize; i++){
        if(headers[i].name.equalsIgnoreCase("Content-Length")){
            hasContentLength = true;
            break;
        }
    }

    // If Content-Length is not in headers, we need to add it
    // Note: This requires creating a new array with one more element
    CCHHeader* finalHeaders = headers;
    uint8_t finalHeadersSize = headersSize;
    
    if(!hasContentLength){
        // Create new array with Content-Length header
        CCHHeader* newHeaders = new CCHHeader[headersSize + 1];
        for(uint8_t i = 0; i < headersSize; i++){
            newHeaders[i] = headers[i];
        }
        newHeaders[headersSize] = {"Content-Length", String(data.length())};
        finalHeaders = newHeaders;
        finalHeadersSize = headersSize + 1;
    }

    ResponseAT responseOpen = openSSLConnection(numberOfConnection, domain, port);
    if(!responseOpen.success){
        Serial.println("Error opening SSL connection: " + responseOpen.response);
        return false;
    }

    String requisicao = createHTTPRequest("POST", domain, path, finalHeaders, finalHeadersSize, data);

    // Clean up if we created a new array
    if(!hasContentLength && finalHeaders){
        delete[] finalHeaders;
    }

    int tamanho = requisicao.length();
    sendSSLData(numberOfConnection, tamanho, requisicao);
    delay(500);
    ResponseAT resposta = waitForSSLResponse(120000);
    if(!resposta.success){
        Serial.println("Error waiting for SSL response: " + resposta.response);
    }

    if(closeAfterSend){
        ResponseAT responseClose = closeSSLConnection(numberOfConnection);
        if(!responseClose.success){
            Serial.println("Error closing SSL connection: " + responseClose.response);
            return false;
        }
    }

    return true;
}


/* @brief Realiza uma requisição HTTP PATCH via canal SSL do modem. */
bool requestSSLPatch(const String& url, uint16_t port, CCHHeader* headers, uint8_t headersSize, bool closeAfterSend, uint8_t numberOfConnection, String data){
    showMessageStatus("Request SSL PATCH...");
    Serial.println("Request SSL PATCH: " + url);
    String originalUrl = url;
    String domain = "";
    String path = "";

    bool validUrl = separetUrl(url, domain, path);
    if(!validUrl){
        Serial.println("Error separating domain and path from url:[[" + originalUrl + "]]");
        return false;
    }

    // Check if Content-Length is already in headers
    bool hasContentLength = false;
    for(uint8_t i = 0; i < headersSize; i++){
        if(headers[i].name.equalsIgnoreCase("Content-Length")){
            hasContentLength = true;
            break;
        }
    }

    // If Content-Length is not in headers, we need to add it
    // Note: This requires creating a new array with one more element
    CCHHeader* finalHeaders = headers;
    uint8_t finalHeadersSize = headersSize;
    
    if(!hasContentLength){
        // Create new array with Content-Length header
        CCHHeader* newHeaders = new CCHHeader[headersSize + 1];
        for(uint8_t i = 0; i < headersSize; i++){
            newHeaders[i] = headers[i];
        }
        newHeaders[headersSize] = {"Content-Length", String(data.length())};
        finalHeaders = newHeaders;
        finalHeadersSize = headersSize + 1;
    }

    ResponseAT responseOpen = openSSLConnection(numberOfConnection, domain, port);
    if(!responseOpen.success){
        Serial.println("Error opening SSL connection: " + responseOpen.response);
        return false;
    }

    String requisicao = createHTTPRequest("PATCH", domain, path, finalHeaders, finalHeadersSize, data);

    // Clean up if we created a new array
    if(!hasContentLength && finalHeaders){
        delete[] finalHeaders;
    }

    int tamanho = requisicao.length();
    sendSSLData(numberOfConnection, tamanho, requisicao);
    delay(500);
    ResponseAT resposta = waitForSSLResponse(120000);
    if(!resposta.success){
        Serial.println("Error waiting for SSL response: " + resposta.response);
    }

    if(closeAfterSend){
        ResponseAT responseClose = closeSSLConnection(numberOfConnection);
        if(!responseClose.success){
            Serial.println("Error closing SSL connection: " + responseClose.response);
            return false;
        }
    }

    return true;
}



/* @brief Aguarda modem reportar eventos de prontidão (CPIN, ATREADY, SMS DONE, PB DONE). */
bool waitModemReady(){
    showMessageStatus("Waiting for modem to be ready...");
    bool allReady = false;
    unsigned long start = millis();
    bool cpinReady = false, atReady = false, smsDone = false, pbDone = false;
    String buffer = "";
    int timeout_ms = 30000;

    while (millis() - start < timeout_ms && !allReady)
        { // timeout 15s
            while (Serial1.available())
            {
                char c = Serial1.read();
                if (c == '\n' || c == '\r')
                {
                    if (buffer.length() > 0)
                    {
                        // Verifica cada mensagem
                        if (buffer.indexOf("+CPIN: READY") >= 0 && !cpinReady){
                            Serial.println("Achou CPIN READY");
                            cpinReady = true;
                        }
                        else if (buffer.indexOf("*ATREADY: 1") >= 0 && !atReady){
                            Serial.println("Achou AT READY");
                            atReady = true;
                        }
                        else if (buffer.indexOf("SMS DONE") >= 0 && !smsDone){
                            Serial.println("Achou SMS DONE");
                            smsDone = true;
                        }
                        else if (buffer.indexOf("PB DONE") >= 0 && !pbDone){
                            Serial.println("Achou PB DONE");
                            pbDone = true;
                        }else{
                            Serial.println("Outro: " + buffer);
                        }

                        buffer = "";
                    }
                }
                else
                {
                    buffer += c;
                }
            }

            // Se todas condições atendidas, modem está pronto
            if (cpinReady && atReady && smsDone && pbDone)
            {
                allReady = true;
            }
        }
        Serial.println("Modem ready: " + String(allReady));
        Serial.println("CPIN READY: " + String(cpinReady));
        Serial.println("AT READY: " + String(atReady));
        Serial.println("SMS DONE: " + String(smsDone));
        Serial.println("PB DONE: " + String(pbDone));

        return allReady;
}

/* @brief Reinicialização lógica do modem via comandos AT+CFUN sequenciais. */
ResponseAT restartModem(){
    showMessageStatus("Software restar modem...");
    /*
    setPhoneFunctionality(Functionality::FUN_MINIMUM_FUNCTIONALITY);
    setPhoneFunctionality(Functionality::FUN_OFFLINE_MODE);
    setPhoneFunctionality(Functionality::FUN_RESET);
    resp = setPhoneFunctionality(Functionality::FUN_FULL_FUNCTIONALITY, 1);

    FUN_MINIMUM_FUNCTIONALITY = 0,
    FUN_FULL_FUNCTIONALITY = 1,
    FUN_DISABLE_PHONE_RF = 4,
    FUN_FACTORY_TEST_MODE = 5,
    FUN_RESET = 6,
    FUN_OFFLINE_MODE = 7,
    FUN_UNKNOWN = 8

    AT+CFUN=0
    AT+CFUN=7
    AT+CFUN=6
    AT+CFUN=1

    */

    sendAT("+CFUN=0", "OK", 30000);
    sendAT("+CFUN=7", "OK", 30000);
    sendAT("+CFUN=6", "OK", 30000);
    showMessageStatus("Ligar modem novamente");
    ResponseAT resp = sendAT("+CFUN=1,1", "OK", 1000);
    return resp;
}

/* @brief Efetua reinício completo: reinicia e aguarda modem ficar pronto. */
void fullRestartModem(){
    showMessageStatus("Full restart modem...");
    restartModem();
    waitModemReady();
}

/* @brief Executa POST de dados JSON para o Firebase Realtime Database. */
void postDataFirebase(const String &path, const String &data)
{
    showMessageStatus("Post data Firebase...");
    Serial.println("Post data Firebase: " + data);
    String urlBase = "https://appmensagem-d03df-default-rtdb.firebaseio.com" + path;

    // Define o tamanho do array de headers
    uint8_t headerSize = 3;
    CCHHeader headers[headerSize];

    // Preenche os headers
    headers[0] = {"Content-Type", "application/json"};
    headers[1] = {"Connection", "keep-alive"};
    headers[2] = {"Print", "silent"};
    // Content-Length será adicionado automaticamente pelo método

    requestSSLPost(urlBase, 443, headers, headerSize, true, connectionFirebaseId, data);
    //Serial.println("Result POST Firebase: " + String(result));
}

/* @brief Executa PATCH de dados JSON no Firebase Realtime Database. */
void updateDataFirebase(const String &path, const String &data)
{
    showMessageStatus("Patch data Firebase...");
    Serial.println("Patch data Firebase: " + data);
    String urlBase = "https://appmensagem-d03df-default-rtdb.firebaseio.com" + path;

    // Define o tamanho do array de headers
    uint8_t headerSize = 3;
    CCHHeader headers[headerSize];

    // Preenche os headers
    headers[0] = {"Content-Type", "application/json"};
    headers[1] = {"Connection", "keep-alive"};
    headers[2] = {"Print", "silent"};
    // Content-Length será adicionado automaticamente pelo método

    requestSSLPatch(urlBase, 443, headers, headerSize, true, connectionFirebaseId, data);
    //Serial.println("Result POST Firebase: " + String(result));
}

/* @brief Realiza configuração inicial do modem: AT básico, PDP/APN e SSL. */
void setupModem(){
    showMessageStatus("Setup Modem...");

    sendCommand("AT", "OK", 1000);
    sendCommand("ATE1", "OK", 1000);
    sendAT("+CMEE=2", "OK", 1000);
    sendAT("+CMGF=1", "OK", 1000);
    sendAT("+CREG?", "OK", 1000);
    
    setPDPContext(pdpContextId, "IP", currentApn.url, "0.0.0.0", 0, 0, 0, 0);

    uint8_t auth_prot = (currentApn.user != "" && currentApn.password != "") ? 2 : 0;
    setPDPAuthentication(pdpContextId, auth_prot, currentApn.user, currentApn.password);

    configureSSLContextNumber(connectionFirebaseId, "sslversion", 4);
    configureSSLContextNumber(connectionFirebaseId, "authmode", 0);
    setSSLReportMode(1, 1);
    startSSLService();

}


#endif
