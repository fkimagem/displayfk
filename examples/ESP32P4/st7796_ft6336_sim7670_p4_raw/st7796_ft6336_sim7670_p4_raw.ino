#include "cores.h"
#include "variaveis.h"
#include "touch.h"
#include "modem.h"
#include "display.h"
#include "rede_wifi.h"


/* @brief Rotina de inicialização do Arduino: configura periféricos, modem, WiFi e UI. */
void setup()
{
    pinMode(POTENCIOMETRO, INPUT);
    Serial.begin(115200);
    

    startModemSerial();

    startDisplay();
    startTouch();
    
    createTaskTestDisplay();
    delay(100);
    showMessageStatus("Iniciando...");
    showStatusLed(LedStatus::BUSY);
    
    bool allReady = waitModemReady();
    if(!allReady){
        Serial.println("Modem not ready!");
        fullRestartModem();
    }

    setupModem();

    conectarWiFi();
    conectarSocket();

    showMessageStatus("Pronto!");
    showStatusLed(LedStatus::READY);
}
/* @brief Laço principal: trata eventos de UI, comunicação TCP e operações Firebase. */
void loop()
{
    serialModemMonitor();

    if(rectButtons[idButtonSend].pressed || precisaGravarNoFirebase){
        rectButtons[idButtonSend].pressed = false;
        precisaGravarNoFirebase = false;
        Serial.println("Gravar novo item...");
        String ip = "";
        
        if(WiFi.status() == WL_CONNECTED) ip = WiFi.localIP().toString();

        String json = "{\"ip\":\"" + ip + "\", \"millis\":" + String(millis()) + ", \"potenciometro\":" + String(analogRead(POTENCIOMETRO)) + "}";
        // {ip:"xxxxxx", millis:xxxxx, potenciometro:xxxxx}
        postDataFirebase("/leituras.json", json);
    }
    if(rectButtons[idButtonUpdate].pressed || precisaAtualizarNoFirebase){
        rectButtons[idButtonUpdate].pressed = false;
        precisaAtualizarNoFirebase = false;
        Serial.println("Update item...");
        int aleatorio = random(1000);
        String jsonIdade = "{\"aleatorio\":" + String(aleatorio) + "}";

        updateDataFirebase("/outrocampo.json", jsonIdade);
    }

    if(rectButtons[idButtonHello].pressed){
        rectButtons[idButtonHello].pressed = false;
        enviarDados("Hello Server!");
    }

    if(rectButtons[idButtonTeste].pressed){
        rectButtons[idButtonTeste].pressed = false;
        Serial.println("Teste MonitorSerial...");
    }



    if (!client.connected()) {
        conectarSocket();
    }

    String recebido = lerDados();
    if (recebido.length() > 0) {
        interpretarComando(recebido);
    }

    delay(10);
}
