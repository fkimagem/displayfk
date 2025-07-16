// Defines for font and files
#define FORMAT_SPIFFS_IF_FAILED false
#define DISPLAY_W 480
#define DISPLAY_H 320

// Prototypes for each screen
void screen0();
void screen1();

// Prototypes for callback functions
void btnleda_cb();
void btnledb_cb();
void btnledc_cb();
void btnmotor_cb();
void btncompressor_cb();
void rightpng_cb();
void homepng_cb();
void spinmaxtemp_cb();

#include <OneWire.h>
#include <DallasTemperature.h>

// Include for plugins of chip 2
// Include external libraries and files
#include <Arduino_GFX_Library.h>
#include <displayfk.h>
#include <SPI.h>


// Create global objects. Constructor is: xPos, yPos and indexScreen
#if defined(CONFIG_IDF_TARGET_ESP32S3)
SPIClass spi_shared(FSPI);
#else
SPIClass spi_shared(HSPI);
#endif
Arduino_DataBus *bus = nullptr;
Arduino_GFX *tft = nullptr;
uint8_t rotationScreen = 1; // This value can be changed depending of orientation of your screen
DisplayFK myDisplay;
SPIClass spi_sd(VSPI);
GaugeSuper gauge(230, 290, 1);
const uint8_t qtdGauge = 1;
GaugeSuper *arrayGauge[qtdGauge] = {&gauge};
const uint8_t qtdIntervalG0 = 4;
int range0[qtdIntervalG0] = {0,25,50,75};
uint16_t colors0[qtdIntervalG0] = {CFK_COLOR08,CFK_COLOR06,CFK_COLOR04,CFK_COLOR01};
Label linhaverde(365, 70, 0);
Label linhalaranja(365, 115, 0);
const uint8_t qtdLabel = 2;
Label *arrayLabel[qtdLabel] = {&linhaverde, &linhalaranja};
LineChart grafico(15, 40, 0);
const uint8_t qtdLineChart = 1;
LineChart *arrayLinechart[qtdLineChart] = {&grafico};
Label *seriesGrafico0[2] = {&linhaverde,&linhalaranja};
const uint8_t qtdLinesChart0 = 2;
uint16_t colorsChart0[qtdLinesChart0] = {CFK_COLOR10,CFK_COLOR04};
SpinBox spinmaxtemp(215, 85, 1);
const uint8_t qtdSpinbox = 1;
SpinBox *arraySpinbox[qtdSpinbox] = {&spinmaxtemp};
int maxTempAllowed = 50; // Global variable that stores the value of the widget spinmaxtemp
ToggleButton btnleda(10, 245, 0);
ToggleButton btnledb(95, 245, 0);
ToggleButton btnledc(180, 247, 0);
ToggleButton btnmotor(405, 220, 0);
ToggleButton btncompressor(405, 260, 0);
const uint8_t qtdToggleBtn = 5;
ToggleButton *arrayTogglebtn[qtdToggleBtn] = {&btnleda, &btnledb, &btnledc, &btnmotor, &btncompressor};
Image rightpng(440, 10, 0);
Image homepng(15, 275, 1);
const uint8_t qtdImagem = 2;
Image *arrayImagem[qtdImagem] = {&rightpng, &homepng};



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


void setup(){
    Serial.begin(115200);
    
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

    spi_sd.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);
    myDisplay.startSD(SD_CS, &spi_sd);
    spi_shared.begin(DISP_SCLK, DISP_MISO, DISP_MOSI);
    bus = new Arduino_HWSPI(DISP_DC, DISP_CS, DISP_SCLK, DISP_MOSI, DISP_MISO, &spi_shared);
    tft = new Arduino_ILI9488_18bit(bus, DISP_RST, rotationScreen, false);
    tft->begin(DISP_FREQUENCY);
    WidgetBase::objTFT = tft; // Reference to object to draw on screen
    myDisplay.startTouchXPT2046(DISPLAY_W, DISPLAY_H, rotationScreen, TC_CS, &spi_shared, tft);
    //myDisplay.recalibrate();
    myDisplay.checkCalibration();
    //myDisplay.enableTouchLog();

    sensors_1.begin(); // Inicializa o sensor de temperatura
    sensors_2.begin(); // Inicializa o sensor de temperatura


    loadWidgets(); // This function is used to setup with widget individualy
    WidgetBase::loadScreen = screen0; // Use this line to change between screens
    myDisplay.createTask(); // Initialize the task to read touch and draw

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
        grafico.push(0, temperatureC_1); // Insere o valor da temperatura do primeiro sensor na serie 0
        grafico.push(1, temperatureC_2); // Insere o valor da temperatura do segundo sensor na serie 1
        grafico.push(2, seno); // Insere o valor da onda senoidal na serie 2
        
        // Atualizo o ponteiro/agulha do gauge
        gauge.setValue(temperatureC_1); // Atualiza o valor do gauge
        
        // Finalizo a transacao para indicar para a task de desenho que pode desenhar
        myDisplay.finishTransaction();
  
        bool temperaturaAlta = (temperatureC_1 >= maxTempAllowed); // Verifica se a temperatura do primeiro sensor é maior ou igual ao valor maximo permitido
  
        // Se a temperatura do primeiro sensor for maior ou igual ao valor maximo permitido, desativa o relé 1
        if(temperaturaAlta){
          btnmotor.setStatus(false); // Desativa o toggle que controla o relé 1
          Serial.println("Quente! Quente! Quente!"); // Imprime uma mensagem no monitor serial
        }
  
        btnmotor.setEnabled(!temperaturaAlta); // Desativa/Ativa o controle manual (por touch) do toggle que controla o relé 1
      }
  
      delay(10); // Aguarda 10ms para evitar que o loop seja executado muito rapidamente
}

void screen0(){
    tft->fillScreen(CFK_GREY3);
    WidgetBase::backgroundColor = CFK_GREY3;
    myDisplay.printText("Tela de controle", 165, 7, TL_DATUM, CFK_COLOR07, CFK_GREY3, &Roboto_Regular10pt7b);
    tft->drawRoundRect(5, 200, 243, 110, 10, CFK_WHITE);
    myDisplay.printText("LED", 105, 212, TL_DATUM, CFK_WHITE, CFK_GREY3, &Roboto_Regular10pt7b);
    tft->drawRoundRect(260, 200, 205, 108, 10, CFK_WHITE);
    myDisplay.printText("Motor", 275, 222, TL_DATUM, CFK_WHITE, CFK_GREY3, &Roboto_Regular10pt7b);
    myDisplay.printText("Compres.", 270, 267, TL_DATUM, CFK_WHITE, CFK_GREY3, &Roboto_Regular10pt7b);
    myDisplay.drawWidgetsOnScreen(0);
}
void screen1(){
    tft->fillScreen(CFK_GREY3);
    WidgetBase::backgroundColor = CFK_GREY3;
    myDisplay.printText("Config do motor", 165, 17, TL_DATUM, CFK_COLOR06, CFK_GREY3, &Roboto_Regular10pt7b);
    myDisplay.printText("Limite de temp.", 50, 107, TL_DATUM, CFK_WHITE, CFK_GREY3, &Roboto_Regular10pt7b);
    myDisplay.drawWidgetsOnScreen(1);
}

// Configure each widgtes to be used
void loadWidgets(){
    GaugeConfig configGauge0 = {
            .width = 250,
            .title = "Temp.",
            .intervals = range0,
            .colors = colors0,
            .amountIntervals = qtdIntervalG0,
            .minValue = 0,
            .maxValue = 100,
            .borderColor = CFK_GREY1,
            .textColor = CFK_BLACK,
            .backgroundColor = CFK_WHITE,
            .titleColor = CFK_BLACK,
            .needleColor = CFK_RED,
            .markersColor = CFK_BLACK,
            .showLabels = true,
            .fontFamily = &Roboto_Bold10pt7b
        };
    gauge.setup(configGauge0);
    myDisplay.setGauge(arrayGauge,qtdGauge);
    LabelConfig configLabel0 = {
            .text = "Line 1",
            .fontFamily = &Roboto_Regular10pt7b,
            .datum = TL_DATUM,
            .fontColor = CFK_COLOR10,
            .backgroundColor = CFK_GREY3,
            .prefix = "pref",
            .suffix = "suf"
        };
    linhaverde.setup(configLabel0);
    LabelConfig configLabel1 = {
            .text = "Line 2",
            .fontFamily = &Roboto_Regular10pt7b,
            .datum = TL_DATUM,
            .fontColor = CFK_COLOR04,
            .backgroundColor = CFK_GREY3,
            .prefix = "pref",
            .suffix = "suf"
        };
    linhalaranja.setup(configLabel1);
    myDisplay.setLabel(arrayLabel,qtdLabel);
    LineChartConfig configLineChart0 = {
            .width = 310,
            .height = 118,
            .minValue = 0,
            .maxValue = 100,
            .amountSeries = qtdLinesChart0,
            .colorsSeries = colorsChart0,
            .gridColor = CFK_GREY6,
            .borderColor = CFK_BLACK,
            .backgroundColor = CFK_GREY4,
            .textColor = CFK_WHITE,
            .verticalDivision = 5,
            .workInBackground = false,
            .showZeroLine = false,
            .boldLine = true,
            .showDots = false,
            .maxPointsAmount = LineChart::SHOW_ALL,
            .font = &Roboto_Regular5pt7b,
            .subtitles = seriesGrafico0
        };
    grafico.setup(configLineChart0);
    myDisplay.setLineChart(arrayLinechart,qtdLineChart);
    SpinBoxConfig configSpinBox0 = {
            .width = 214,
            .height = 65,
            .step = 1,
            .minValue = 0,
            .maxValue = 100,
            .startValue = maxTempAllowed,
            .color = CFK_GREY14,
            .textColor = CFK_BLACK,
            .callback = spinmaxtemp_cb
        };
    spinmaxtemp.setup(configSpinBox0);
    myDisplay.setSpinbox(arraySpinbox,qtdSpinbox);
    ToggleButtonConfig configToggle0 = {
            .width = 56,
            .height = 28,
            .pressedColor = CFK_COLOR18,
            .callback = btnleda_cb
        };
    btnleda.setup(configToggle0);
    ToggleButtonConfig configToggle1 = {
            .width = 55,
            .height = 28,
            .pressedColor = CFK_COLOR18,
            .callback = btnledb_cb
        };
    btnledb.setup(configToggle1);
    ToggleButtonConfig configToggle2 = {
            .width = 53,
            .height = 27,
            .pressedColor = CFK_COLOR18,
            .callback = btnledc_cb
        };
    btnledc.setup(configToggle2);
    ToggleButtonConfig configToggle3 = {
            .width = 47,
            .height = 24,
            .pressedColor = CFK_COLOR09,
            .callback = btnmotor_cb
        };
    btnmotor.setup(configToggle3);
    ToggleButtonConfig configToggle4 = {
            .width = 47,
            .height = 24,
            .pressedColor = CFK_COLOR04,
            .callback = btncompressor_cb
        };
    btncompressor.setup(configToggle4);
    myDisplay.setToggle(arrayTogglebtn,qtdToggleBtn);
    ImageFromFileConfig configImage0 = {
            .source = SourceFile::SPIFFS,
            .path = "/Rightpng.fki",
            .cb = rightpng_cb,
            .angle = 0
        };
    rightpng.setup(configImage0);
    ImageFromFileConfig configImage1 = {
            .source = SourceFile::SPIFFS,
            .path = "/Homepng.fki",
            .cb = homepng_cb,
            .angle = 0
        };
    homepng.setup(configImage1);
    myDisplay.setImage(arrayImagem,qtdImagem);
}


// Quando clica no botão de seleção de temperatura máxima, atualiza o valor da temperatura máxima
void spinmaxtemp_cb(){
    maxTempAllowed = spinmaxtemp.getValue();
}

// Quando clica no botão leda, ativa/desativa o rele do leda
void btnleda_cb(){
    bool myValue = btnleda.getStatus();
    digitalWrite(pinLed_1, myValue); // Ativa/Desativa o LED 1
}

// Quando clica no botão ledb, ativa/desativa o rele do ledb
void btnledb_cb(){
    bool myValue = btnledb.getStatus();
    digitalWrite(pinLed_2, myValue); // Ativa/Desativa o LED 2
}

// Quando clica no botão ledc, ativa/desativa o rele do ledc
void btnledc_cb(){
    bool myValue = btnledc.getStatus();
    digitalWrite(pinLed_3, myValue); // Ativa/Desativa o LED 3
}


// Quando clica no botão motor, ativa/desativa o rele do motor
void btnmotor_cb(){
    bool myStatus = btnmotor.getStatus();

    bool statusRele = !myStatus;//O rele ativa com LOW, entao aqui inverto a logica para facilitar o entendimento das linhas abaixo.
    //statusRele = true -> rele ativado (passando energia)
    //statusRele = false -> rele desativado
    
    digitalWrite(pinMotor, statusRele); // Ativa/Desativa o relé 1

    // Se ligou togglebutton do rele do motor
    if(myStatus){
        // Força a bomba para ligar
        btncompressor.setStatus(true);//Ativa o togglebutton da bomba
        digitalWrite(pinBomba, statusRele);//Coloca o pin da bomba no mesmo nivel do pin do rele (digitalWrite acima desse 'if')
    }

    Serial.print("O motor esta em nivel: ");Serial.println(statusRele); // Imprime o valor do toggleButton no monitor serial

}


// Quando clica no botão compressor, ativa/desativa o rele da bomba
void btncompressor_cb(){
    bool myStatus = btncompressor.getStatus();
    bool statusRele = !myStatus;//O rele ativa com LOW, entao aqui inverto a logica para facilitar o entendimento das linhas abaixo.
    
    digitalWrite(pinBomba, statusRele); // Ativa/Desativa o relé 2

    // Se desligou o togglebutton do rele da bomba
    if(!myStatus){
        // Força o motor para desligar
        btnmotor.setStatus(false);//Desativa o togglebutton de controle do motor
        digitalWrite(pinMotor, statusRele);////Coloca o pin do motor no mesmo nivel do pin do rele (digitalWrite acima desse 'if')
    }

    Serial.print("A bomba esta em nivel: ");Serial.println(statusRele); // Imprime o valor do toggleButton no monitor serial
}

// Quando clica no botão right, carrega a tela de configuração
void rightpng_cb(){
    WidgetBase::loadScreen = screen1; // Carrega a tela de configuração
}

// Quando clica no botão home, carrega a tela de controle
void homepng_cb(){
    WidgetBase::loadScreen = screen0; // Carrega a tela de configuração
}
