#define FORMAT_SPIFFS_IF_FAILED false // false para não formatar o SPIFFS (se usar o SPIFFS)

// Definições para o display
#define DISPLAY_W 480 // Largura do display
#define DISPLAY_H 320 // Altura do display


// Protótipos das funções de callback
void tg10tgb_cb(); // Função para o primeiro toggleButton
void tg21tgb_cb(); // Função para o segundo toggleButton
void tg32tgb_cb(); // Função para o terceiro toggleButton
void releMotortgb_cb(); // Função para o primeiro relé
void releBombatgb_cb(); // Função para o segundo relé
void icoRight0_cb(); // Função para o primeiro ícone
void icoLeft1_cb(); // Função para o segundo ícone
void spinbox0sb_cb(); // Função para o primeiro spinbox

// Inclusão das bibliotecas para o sensor de temperatura
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Arduino.h>

// Inclusão das bibliotecas para o display
#include <Arduino_GFX_Library.h>
#include <displayfk.h>
#include <SPI.h>

// Protótipos das funções de tela
void screen0(); // Função para o primeiro screen
void screen1(); // Função para o segundo screen

// Criação dos objetos globais.
SPIClass spi_shared(FSPI); // Objeto para a comunicação SPI
Arduino_DataBus *bus = nullptr; // Objeto para a comunicação com o display (Arduino GFX)
Arduino_GFX *tft = nullptr; // Objeto para a comunicação com o display (Arduino GFX)
uint8_t rotationScreen = 1; // Valor de rotação do display
DisplayFK myDisplay; // Objeto para a comunicação com o display (DisplayFK)

// Sensor de temperatura
const int pinOneWire_1 = 17; // Pino digital ao qual o sensor de temperatura está conectado
OneWire oneWire_1(pinOneWire_1); // Objeto para a comunicação com o sensor de temperatura
DallasTemperature sensors_1(&oneWire_1); // Objeto para a comunicação com o sensor de temperatura
const int pinOneWire_2 = 18; // Pino digital ao qual o sensor de temperatura está conectado
OneWire oneWire_2(pinOneWire_2); // Objeto para a comunicação com o sensor de temperatura
DallasTemperature sensors_2(&oneWire_2); // Objeto para a comunicação com o sensor de temperatura

// Pinos de saída
const uint8_t pinLed_1 = 47;  // Pino digital ao qual o relé está conectado
const uint8_t pinLed_2 = 48;  // Pino digital ao qual o relé está conectado
const uint8_t pinLed_3 = 1;  // Pino digital ao qual o relé está conectado
const uint8_t pinMotor = 6;  // Pino digital ao qual o relé está conectado
const uint8_t pinBomba = 7;  // Pino digital ao qual o relé está conectado

int contador = 0; // Contador auxliar para a onda senoidal do grafico
TimerHandle_t xTimerLeitura; // Timer para a leitura do sensor
volatile bool podeLerSensor = false; // Variável auxiliar para a leitura do sensor

/*       ------------------- CRIACAO DE WIDGETS------------  */

// Gauge
const uint8_t qtdGauge = 1; // Quantidade de widgets gauges no projeto
GaugeSuper arrayGauge[qtdGauge] = {GaugeSuper(345, 160, 0)}; // Widget gauge
const uint8_t qtdIntervalG0 = 4; // Quantidade de intervalos no widget gauge
int range0[qtdIntervalG0] = {0,25,40,75}; // Valores onde cada cor do widget gauge é aplicada
uint16_t colors0[qtdIntervalG0] = {CFK_AQUA,CFK_YELLOW,CFK_ORANGE,CFK_RED}; // Cores do widget gauge

// Label (text mutavel por codigo)
const uint8_t qtdLabel = 1; // Quantidade de widgets label no projeto
Label arrayLabel[qtdLabel] = {Label(340, 60, 1)}; // Widget label

// LineChart (grafico de linhas)
const uint8_t qtdLineChart = 1; // Quantidade de widgets lineChart no projeto
LineChart arrayLinechart[qtdLineChart] = {LineChart(10, 45, 0)}; // Widget lineChart
const uint8_t qtdLinesChart0 = 3; // Quantidade de series no widget lineChart
uint16_t colorsChart0[qtdLinesChart0] = {CFK_LIME,CFK_FUCHSIA, CFK_RED}; // Cores das series no widget lineChart

// SpinBox (caixa de numero com sinal + e -)
const uint8_t qtdSpinbox = 1; // Quantidade de widgets spinbox no projeto
SpinBox arraySpinbox[qtdSpinbox] = {SpinBox(30, 120, 1)}; // Widget spinbox
int maxTempAllowed = 45; // Valor inicial do widget spinbox

// ToggleButton (botão de seleção true/false)
const uint8_t qtdToggleBtn = 5; // Quantidade de widgets toggleButton no projeto
ToggleButton arrayTogglebtn[qtdToggleBtn] = {ToggleButton(80, 205, 0), ToggleButton(175, 205, 0), ToggleButton(265, 205, 0), ToggleButton(80, 250, 0), ToggleButton(175, 250, 0)}; // Widget toggleButton

// Imagem
const uint8_t qtdImagem = 2; // Quantidade de widgets imagem no projeto
Image arrayImagem[qtdImagem] = {Image(435, 275, 0), Image(430, 270, 1)}; // Widget imagem

// CircularBar (barra circular)
const uint8_t qtdCircBar = 1; // Quantidade de widgets circularBar no projeto
CircularBar arrayCircularbar[qtdCircBar] = {CircularBar(355, 160, 1)}; // Widget circularBar


void cb_LerSensor(TimerHandle_t xTimer){ // Função de callback para a leitura do sensor
  podeLerSensor = true;
}


void setup(){ // Função de setup
    Serial.begin(115200); // Inicializa a comunicação serial
    
    const bool initialStatusPinsRele = HIGH; // Status inicial dos pinos de rele
    pinMode(pinLed_1, OUTPUT); // Configura o pino como saída
    digitalWrite(pinLed_1, LOW);  // Inicializa o pino como LOW
    pinMode(pinLed_2, OUTPUT); // Configura o pino como saída
    digitalWrite(pinLed_2, LOW);  // Inicializa o pino como LOW
    pinMode(pinLed_3, OUTPUT); // Configura o pino como saída
    digitalWrite(pinLed_3, LOW);  // Inicializa o pino como LOW
    pinMode(pinMotor, OUTPUT); // Configura o pino como saída
    digitalWrite(pinMotor, initialStatusPinsRele);  // Inicializa o pino como initialStatusPinsRele
    pinMode(pinBomba, OUTPUT); // Configura o pino como saída
    digitalWrite(pinBomba, initialStatusPinsRele);  // Inicializa o pino como initialStatusPinsRele

    spi_shared.begin(DISP_SCLK, DISP_MISO, DISP_MOSI); // Inicializa a comunicação SPI
    bus = new Arduino_HWSPI(DISP_DC, DISP_CS, DISP_SCLK, DISP_MOSI, DISP_MISO, &spi_shared); // Inicializa o objeto para a comunicação com o display
    tft = new Arduino_ILI9488_18bit(bus, DISP_RST, rotationScreen, false); // Inicializa o objeto para a comunicação com o display
    tft->begin(DISP_FREQUENCY); // Inicializa a comunicação com o display
    
    WidgetBase::objTFT = tft; // Referência para o objeto para desenhar na tela
    //myDisplay.startTouch(DISPLAY_W, DISPLAY_H, rotationScreen, &spi_shared); // Inicializa a comunicação com o touch controller
    myDisplay.startTouchXPT2046(DISPLAY_W, DISPLAY_H, rotationScreen, TC_CS, &spi_shared, tft); //Start comunication with touch controller
    //myDisplay.recalibrate(); // Verifica e aplica os valores de calibração
    myDisplay.checkCalibration(); // Verifica e aplica os valores de calibração
    myDisplay.disableTouchLog();
    loadWidgets(); // Carrega os widgets

    sensors_1.begin(); // Inicializa o sensor de temperatura
    sensors_2.begin(); // Inicializa o sensor de temperatura

    

    

    WidgetBase::loadScreen = screen0; // Defino qual tela deve ser carregada/desenhada
    myDisplay.createTask(); // Inicializa a task para ler o touch e desenhar

    // Cria o timer para a leitura do sensor
    xTimerLeitura = xTimerCreate("TIMER1", pdMS_TO_TICKS(1000), pdTRUE, 0, cb_LerSensor);

    // Inicia o timer
    xTimerStart(xTimerLeitura, 0);
}

void loop(){

    // Se puder ler o sensor, lê o sensor
    if(podeLerSensor){
      podeLerSensor = false; // Reseta a variável auxiliar para a leitura do sensor

      // Calcula valor do seno para a variavel 'contador' que servira apenas para ver o grafico funcionando quando a temperatura estiver estavel
      int seno = round(sin(contador * 0.0174533) * 50) + 50;

      // Incrementa o contador para a onda senoidal do grafico
      contador++;
  
      // Requisita a leitura de temperatura para os dois sensores
      sensors_1.requestTemperatures();
      sensors_2.requestTemperatures();
      delay(750); // Aguarda 750ms para a leitura das temperaturas
  
      // Faz a leitura da temperatura do primeiro sensor
      float temperatureC_1 = sensors_1.getTempCByIndex(0);

      // Faz a leitura da temperatura do segundo sensor
      float temperatureC_2 = sensors_2.getTempCByIndex(0);
  
      // Inicio a transação, como vamos fazer varias atualizações que afetam o display, primeiro vamos mudar todos os valores e depois desenhar tudo de uma vez
      myDisplay.startTransaction();
      
      // Em cada serie (linha) do grafico, insere um novo valor
      arrayLinechart[0].push(0, temperatureC_1); // Insere o valor da temperatura do primeiro sensor na serie 0
      arrayLinechart[0].push(1, temperatureC_2); // Insere o valor da temperatura do segundo sensor na serie 1
      arrayLinechart[0].push(2, seno); // Insere o valor da onda senoidal na serie 2
      
      // Atualizo o ponteiro/agulha do gauge
      arrayGauge[0].setValue(temperatureC_1); // Atualiza o valor do gauge
  
      // Atualiza o valor do label
      arrayLabel[0].setTextFloat(temperatureC_1, 2); // Atualiza o valor do label
      
      // Atualizo o valor e preenchimento do circular bar
      arrayCircularbar[0].setValue(temperatureC_1); // Atualiza o valor do circular bar
  
      // Finalizo a transacao para indicar para a task de desenho que pode desenhar
      myDisplay.finishTransaction();

      bool temperaturaAlta = (temperatureC_1 >= maxTempAllowed); // Verifica se a temperatura do primeiro sensor é maior ou igual ao valor maximo permitido

      // Se a temperatura do primeiro sensor for maior ou igual ao valor maximo permitido, desativa o relé 1
      if(temperaturaAlta){
        arrayTogglebtn[3].setStatus(false); // Desativa o toggle que controla o relé 1
        Serial.println("Quente! Quente! Quente!"); // Imprime uma mensagem no monitor serial
      }

      arrayTogglebtn[3].setEnabled(!temperaturaAlta); // Desativa/Ativa o controle manual (por touch) do toggle que controla o relé 1
    }

    delay(10); // Aguarda 10ms para evitar que o loop seja executado muito rapidamente
}

// Função de callback do widget spinbox
void spinbox0sb_cb(){
    maxTempAllowed = arraySpinbox[0].getValue(); // Atualiza o valor maximo permitido
}

// Função de callback do widget toggleButton
void tg10tgb_cb(){
    bool myValue = arrayTogglebtn[0].getStatus(); // Obtém o status do toggleButton
    digitalWrite(pinLed_1, myValue); // Ativa/Desativa o LED 1
    Serial.print("New value for toggle is: ");Serial.println(myValue); // Imprime o valor do toggleButton no monitor serial
}

// Função de callback do widget toggleButton
void tg21tgb_cb(){
    bool myValue = arrayTogglebtn[1].getStatus(); // Obtém o status do toggleButton
    digitalWrite(pinLed_2, myValue); // Ativa/Desativa o LED 2
    Serial.print("New value for toggle is: ");Serial.println(myValue); // Imprime o valor do toggleButton no monitor serial
}

// Função de callback do widget toggleButton
void tg32tgb_cb(){
    bool myValue = arrayTogglebtn[2].getStatus(); // Obtém o status do toggleButton
    digitalWrite(pinLed_3, myValue); // Ativa/Desativa o LED 3
    Serial.print("New value for toggle is: ");Serial.println(myValue); // Imprime o valor do toggleButton no monitor serial
}

// Função de callback do widget toggleButton
void releMotortgb_cb(){
    bool myStatus = arrayTogglebtn[3].getStatus(); // Obtém o status do toggleButton (ligado/desligado)
    
    bool statusRele = !myStatus;//O rele ativa com LOW, entao aqui inverto a logica para facilitar o entendimento das linhas abaixo.
    //statusRele = true -> rele ativado (passando energia)
    //statusRele = false -> rele desativado
    
    digitalWrite(pinMotor, statusRele); // Ativa/Desativa o relé 1

    // Se ligou togglebutton do rele do motor
    if(myStatus){
        // Força a bomba para ligar
        arrayTogglebtn[4].setStatus(true);//Ativa o togglebutton da bomba
        digitalWrite(pinBomba, statusRele);//Coloca o pin da bomba no mesmo nivel do pin do rele (digitalWrite acima desse 'if')
    }

    Serial.print("O motor esta em nivel: ");Serial.println(statusRele); // Imprime o valor do toggleButton no monitor serial
}

// Função de callback do widget toggleButton
void releBombatgb_cb(){
    bool myStatus = arrayTogglebtn[4].getStatus(); // Obtém o status do toggleButton (ligado/desligado)
    //statusRele = true -> rele ativado (passando energia)
    //statusRele = false -> rele desativado
    
    bool statusRele = !myStatus;//O rele ativa com LOW, entao aqui inverto a logica para facilitar o entendimento das linhas abaixo.
    
    digitalWrite(pinBomba, statusRele); // Ativa/Desativa o relé 2

    // Se desligou o togglebutton do rele da bomba
    if(!myStatus){
        // Força o motor para desligar
        arrayTogglebtn[3].setStatus(false);//Desativa o togglebutton de controle do motor
        digitalWrite(pinMotor, statusRele);////Coloca o pin do motor no mesmo nivel do pin do rele (digitalWrite acima desse 'if')
    }

    Serial.print("A bomba esta em nivel: ");Serial.println(statusRele); // Imprime o valor do toggleButton no monitor serial
}

// Função de callback do widget imagem
void icoRight0_cb(){
    // Image icoRight clicked
    Serial.println("Image icoRight clicked"); // Imprime uma mensagem no monitor serial
    WidgetBase::loadScreen = screen1; // Carrega a tela de configuração
}

// Função de callback do widget imagem
void icoLeft1_cb(){
    // Image icoLeft clicked
    Serial.println("Image icoLeft clicked"); // Imprime uma mensagem no monitor serial
    WidgetBase::loadScreen = screen0; // Carrega a tela inicial
}

/*       ------------------- CENAS ------------  */



// Função para a tela inicial
void screen0(){
    tft->fillScreen(CFK_GREY4); // Preenche a tela com a cor CFK_GREY4
    WidgetBase::backgroundColor = CFK_GREY4; // Define a cor de fundo da tela
    myDisplay.printText("Sensor temp", 170, 12, TL_DATUM, CFK_LIME, CFK_GREY4, &Roboto_Regular10pt7b); // Imprime o texto "Sensor temp" na tela
    myDisplay.printText("LEDs", 15, 212, TL_DATUM, CFK_LIME, CFK_GREY4, &Roboto_Regular10pt7b); // Imprime o texto "LEDs" na tela
    myDisplay.printText("Reles", 15, 262, TL_DATUM, CFK_LIME, CFK_GREY4, &Roboto_Regular10pt7b); // Imprime o texto "Reles" na tela
    myDisplay.printText("Motor", 90, 297, TL_DATUM, CFK_LIME, CFK_GREY4, &Roboto_Regular10pt7b); // Texto do motor
    myDisplay.printText("Compr.", 180, 297, TL_DATUM, CFK_LIME, CFK_GREY4, &Roboto_Regular10pt7b); // Texto da bomba dagua
    myDisplay.drawWidgetsOnScreen(0); // Desenha os widgets na tela
}

// Função para a tela de configuração
void screen1(){
    tft->fillScreen(CFK_GREY4); // Preenche a tela com a cor CFK_GREY4
    WidgetBase::backgroundColor = CFK_GREY4; // Define a cor de fundo da tela
    myDisplay.printText("Config", 195, 17, TL_DATUM, CFK_LIME, CFK_GREY4, &Roboto_Regular10pt7b); // Imprime o texto "Config" na tela
    myDisplay.printText("Max temp", 35, 92, TL_DATUM, CFK_SKYBLUE, CFK_GREY4, &Roboto_Regular10pt7b); // Imprime o texto "Max temp" na tela
    myDisplay.drawWidgetsOnScreen(1); // Desenha os widgets na tela
}




/* -------- CONFIG DOS WIDGETS ---------------   */

// Configura cada widget para ser usado
void loadWidgets(){

    // Configuração do gauge
    GaugeConfig configGauge0 = {
            .width = 250, // Largura do gauge
            .title = "Sensor 1", // Titulo do gauge
            .intervals = range0, // Valores onde cada cor do widget gauge é aplicada
            .colors = colors0, // Cores do widget gauge
            .amountIntervals = qtdIntervalG0, // Quantidade de intervalos no widget gauge
            .minValue = 0, // Valor minimo do gauge
            .maxValue = 100, // Valor maximo do gauge
            .borderColor = CFK_OLIVE, // Cor da borda do gauge
            .textColor = CFK_WHITE, // Cor do texto do gauge
            .backgroundColor = CFK_GREY3, // Cor de fundo do gauge
            .titleColor = CFK_WHITE, // Cor do titulo do gauge
            .needleColor = CFK_RED, // Cor da agulha do gauge
            .markersColor = CFK_BLACK, // Cor dos marcadores(risquinhos) do gauge
            .showLabels = true, // Mostra os labels do gauge
            .fontFamily = &Roboto_Bold10pt7b // Fonte do gauge
        };
    arrayGauge[0].setup(configGauge0); // Configura o widget gauge 0 com os valores da configuração
    myDisplay.setGauge(arrayGauge,qtdGauge); // Informa para a lib as configurações do widget gauge

    // Configuração do label
    LabelConfig configLabel0 = { // Configuração do label
            .text = "99", // Texto do label
            .fontFamily = &Roboto_Regular10pt7b, // Fonte do label
            .datum = TL_DATUM, // Posição do label
            .fontColor = CFK_FUCHSIA, // Cor do texto do label
            .backgroundColor = CFK_GREY4, // Cor de fundo do label
            .prefix = "Temp ", // Texto prefixo do label
            .suffix = " C" // Texto sufixo do label
        };
    arrayLabel[0].setup(configLabel0); // Configura o widget label 0 com os valores da configuração
    myDisplay.setLabel(arrayLabel,qtdLabel); // Informa para a lib as configurações do widget label

    // Configuração do grafico de linhas
    LineChartConfig configLineChart0 = { // Configuração do grafico de linhas
            .width = 205, // Largura do grafico de linhas
            .height = 117, // Altura do grafico de linhas
            .minValue = 0, // Valor minimo do grafico de linhas
            .maxValue = 100, // Valor maximo do grafico de linhas
            .amountSeries = qtdLinesChart0, // Quantidade de series no grafico de linhas
            .colorsSeries = colorsChart0, // Cores das series no grafico de linhas
            .gridColor = CFK_GREY3, // Cor da grade do grafico de linhas
            .borderColor = CFK_SADDLEBROWN, // Cor da borda do grafico de linhas
            .backgroundColor = CFK_BLACK, // Cor de fundo do grafico de linhas
            .textColor = CFK_WHITE, // Cor do texto do grafico de linhas
            .verticalDivision = 5, // Quantidade de divisões verticais do grafico de linhas
            .workInBackground = true, // Se o grafico de linhas deve ser desenhado em background
            .showZeroLine = true, // Se deve mostrar a linha zero do grafico de linhas
            .font = &Roboto_Regular5pt7b // Fonte do grafico de linhas
        };
    arrayLinechart[0].setup(configLineChart0); // Configura o widget grafico de linhas 0 com os valores da configuração
    myDisplay.setLineChart(arrayLinechart,qtdLineChart); // Informa para a lib as configurações do widget grafico de linhas

    // Configuração do spinbox
    SpinBoxConfig configSpinBox0 = { // Configuração do spinbox
            .width = 196, // Largura do spinbox
            .height = 52, // Altura do spinbox
            .step = 1, // Passo do spinbox
            .minValue = 0, // Valor minimo do spinbox
            .maxValue = 100, // Valor maximo do spinbox
            .startValue = maxTempAllowed, // Valor inicial do spinbox
            .color = CFK_SKYBLUE, // Cor do spinbox
            .textColor = CFK_BLACK, // Cor do texto do spinbox
            .callback = spinbox0sb_cb // Função de callback do spinbox
        };
    arraySpinbox[0].setup(configSpinBox0); // Configura o widget spinbox 0 com os valores da configuração
    myDisplay.setSpinbox(arraySpinbox,qtdSpinbox); // Informa para a lib as configurações do widget spinbox

    // Configuração do toggleButton
    ToggleButtonConfig configToggle0 = { // Configuração do toggleButton
            .width = 66, // Largura do toggleButton
            .height = 33, // Altura do toggleButton
            .pressedColor = CFK_OLIVE, // Cor do toggleButton pressionado
            .callback = tg10tgb_cb // Função de callback do toggleButton
        };
    arrayTogglebtn[0].setup(configToggle0); // Configura o widget toggleButton 0 com os valores da configuração
    ToggleButtonConfig configToggle1 = { // Configuração do toggleButton
            .width = 66, // Largura do toggleButton
            .height = 33, // Altura do toggleButton
            .pressedColor = CFK_OLIVE, // Cor do toggleButton pressionado
            .callback = tg21tgb_cb // Função de callback do toggleButton
        };
    arrayTogglebtn[1].setup(configToggle1); // Configura o widget toggleButton 1 com os valores da configuração
    ToggleButtonConfig configToggle2 = { // Configuração do toggleButton
            .width = 66, // Largura do toggleButton
            .height = 33, // Altura do toggleButton
            .pressedColor = CFK_OLIVE, // Cor do toggleButton pressionado
            .callback = tg32tgb_cb // Função de callback do toggleButton
        };
    arrayTogglebtn[2].setup(configToggle2); // Configura o widget toggleButton 2 com os valores da configuração
    ToggleButtonConfig configToggle3 = { // Configuração do toggleButton
            .width = 66, // Largura do toggleButton
            .height = 33, // Altura do toggleButton
            .pressedColor = CFK_CHOCOLATE, // Cor do toggleButton pressionado
            .callback = releMotortgb_cb // Função de callback do toggleButton
        };
    arrayTogglebtn[3].setup(configToggle3); // Configura o widget toggleButton 3 com os valores da configuração
    ToggleButtonConfig configToggle4 = { // Configuração do toggleButton
            .width = 66, // Largura do toggleButton
            .height = 33, // Altura do toggleButton
            .pressedColor = CFK_GOLD, // Cor do toggleButton pressionado
            .callback = releBombatgb_cb // Função de callback do toggleButton
        };
    arrayTogglebtn[4].setup(configToggle4); // Configura o widget toggleButton 4 com os valores da configuração
    myDisplay.setToggle(arrayTogglebtn,qtdToggleBtn); // Informa para a lib as configurações do widget toggleButton

    // Configuração da imagem
    ImageFromFileConfig configImage0 = { // Configuração da imagem
            .source = SourceFile::SPIFFS, // Fonte da imagem
            .path = "/icoRight.fki", // Caminho da imagem
            .cb = icoRight0_cb, // Função de callback da imagem
            .angle = 0 // Ângulo da imagem
        };
    arrayImagem[0].setup(configImage0); // Configura a imagem 0 com os valores da configuração
    ImageFromFileConfig configImage1 = { // Configuração da imagem
            .source = SourceFile::SPIFFS, // Fonte da imagem
            .path = "/icoLeft.fki", // Caminho da imagem
            .cb = icoLeft1_cb, // Função de callback da imagem
            .angle = 0 // Ângulo da imagem
        };
    arrayImagem[1].setup(configImage1); // Configura a imagem 1 com os valores da configuração
    myDisplay.setImage(arrayImagem,qtdImagem); // Informa para a lib as configurações da imagem

    // Configuração da barra circular
    CircularBarConfig configCirculaBar0 = { // Configuração da barra circular
            .radius = 55, // Raio da barra circular
            .minValue = 0, // Valor minimo da barra circular 
            .maxValue = 100, // Valor maximo da barra circular
            .startAngle = 0, // Ângulo inicial da barra circular
            .endAngle = 360, // Ângulo final da barra circular
            .thickness = 20, // Espessura da barra circular
            .color = CFK_SKYBLUE, // Cor da barra circular
            .backgroundColor = CFK_GREY4, // Cor de fundo da barra circular
            .textColor = CFK_WHITE, // Cor do texto da barra circular
            .showValue = true, // Se deve mostrar o valor da barra circular
            .inverted = false // Se a barra circular deve ser invertida
        };
    arrayCircularbar[0].setup(configCirculaBar0); // Configura a barra circular 0 com os valores da configuração
    myDisplay.setCircularBar(arrayCircularbar,qtdCircBar); // Informa para a lib as configurações da barra circular
}