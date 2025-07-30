/*
Esse codigo foi gerado automatico pelo edge. Para a versão final, criei apenas algumas novas funções, variáveis e movi algun comandos
(controles de leds, etc). Do loop (lugar original que os comandos são escritos) para as funções de callback que eu queria que executase a ação.
*/

#define FORMAT_SPIFFS_IF_FAILED false
#define DISPLAY_W 480 // Largura do display
#define DISPLAY_H 320 // Altura do display
#define DISP_CS	10 // Pino CS do display
#define DISP_MISO	13 // Pino MISO do display
#define DISP_MOSI	11 // Pino MOSI do display
#define DISP_SCLK	12 // Pino SCL do display
#define DISP_RST	9 // Pino RST do display
#define DISP_DC	8 // Pino DC do display
#define DISP_FREQUENCY	27000000 // Frequencia do SPI do display
#define TC_CS	21 // Pino CS do touch
#define TC_SCL	12 // Pino SCL do touch
#define TC_MOSI	11 // Pino MOSI do touch
#define TC_MISO	13 // Pino MISO do touch
#define TOUCH_FREQUENCY	2500000 // Frequencia SPI do touch
#define TOUCH_INVERT_X	false // Flag que indica se o eixo X está invertido no touch
#define TOUCH_INVERT_Y	false // Flag que indica se o eixo Y está invertido no touch

// Os quatro parametros abaixo indicam o map do touch (ex: eixo vai de 0 a 480 e y de 0 a 320)
#define TOUCH_MAP_X0	0 // Pontos x0
#define TOUCH_MAP_X1	480
#define TOUCH_MAP_Y0	0
#define TOUCH_MAP_Y1	320
#define TOUCH_SWAP_XY	false // Flag que indica se os eixos estao trocados

// prototipo da função que configura os widgets
void loadWidgets();

// Prototipos das telas
void screen0();
void screen1();

// Prototipos das funções de callback
void btnleda_cb();//Quando clica no botao do led A
void btnledb_cb();//Quando clica no botao do led B
void btnledc_cb();//Quando clica no botao do led C
void btnmotor_cb();//Quando clica no botao do motor
void btncompressor_cb();//Quando clica no botao do compressor
void rightpng_cb();//Quando clica na imagem 'seta para direita'
void homepng_cb();//Quando clica na imagem 'casa'
void spinmaxmotor_cb();//Quando modifica o valor do spinbox do motor
void spinmaxcomp_cb();//Quando modifica o valor do spinbox do compressor

// Bibliotec para comunicação com o sensor de temepratura
#include <OneWire.h>

// Biblioteca para interpretar os dados do sensor de temepratura
#include <DallasTemperature.h>

#include <Arduino_GFX_Library.h>
#include <displayfk.h>
#include <SPI.h>


#if defined(CONFIG_IDF_TARGET_ESP32S3)
SPIClass spi_shared(FSPI);              // Se estiver usando um ESP32-S3, inicializa o barramento SPI no canal FSPI (mais rápido e dedicado)
#else
SPIClass spi_shared(HSPI);              // Caso contrário (ESP32 convencional), usa o canal HSPI para comunicação SPI compartilhada
#endif

Arduino_DataBus *bus = nullptr;         // Ponteiro para o barramento de dados do display (será instanciado posteriormente com as configurações corretas)
Arduino_GFX *tft = nullptr;             // Ponteiro para o objeto gráfico principal do display (gerencia desenho, cores, texto, etc.)
uint8_t rotationScreen = 1;             // Define a rotação da tela (0 a 3); valor 1 corresponde a 90° (paisagem padrão para 480x320)

DisplayFK myDisplay;                    // Instancia do objeto principal da biblioteca `displayfk`, que gerencia widgets, eventos e o desenho na tela

GaugeSuper gaugecontrole(350, 160, 0);  // Cria um gauge (medidor circular) em (x=350, y=160), na tela 0
const uint8_t qtdGauge = 1;             // Número total de gauges usados
GaugeSuper *arrayGauge[qtdGauge] = {&gaugecontrole}; // Array de ponteiros para gauges, usado pelo sistema de renderização

const uint8_t qtdIntervalG1 = 4;        // Número de intervalos de cor no gauge
int range1[qtdIntervalG1] = {0, 25, 50, 75}; // Intervalos de valor que mudam a cor do gauge
uint16_t colors1[qtdIntervalG1] = {CFK_COLOR08, CFK_COLOR06, CFK_COLOR04, CFK_COLOR01}; // Cores correspondentes a cada faixa de valor

Label linhaverde(10, 170, 0);           // Label de texto (linha verde) na posição (10, 170), tela 0
Label linhalaranja(130, 170, 0);        // Outro label, com a linha laranja, mais à direita
const uint8_t qtdLabel = 2;             // Número total de labels
Label *arrayLabel[qtdLabel] = {&linhaverde, &linhalaranja}; // Lista de ponteiros para gerenciamento automático

LineChart grafico(15, 40, 0);           // Cria um gráfico de linha na posição (15, 40), na tela 0
const uint8_t qtdLineChart = 1;         // Só há um gráfico na interface
LineChart *arrayLinechart[qtdLineChart] = {&grafico}; // Array de ponteiros para os gráficos

Label *seriesGrafico0[2] = {&linhaverde, &linhalaranja}; // Labels usados como fontes de dados para as linhas do gráfico
const uint8_t qtdLinesChart0 = 2;       // O gráfico terá duas linhas
uint16_t colorsChart0[qtdLinesChart0] = {CFK_COLOR10, CFK_COLOR04}; // Cores de cada linha do gráfico

SpinBox spinmaxmotor(210, 80, 1);       // SpinBox para definir o valor máximo do motor, na tela 1
SpinBox spinmaxcomp(210, 175, 1);       // SpinBox para valor máximo do compressor, na tela 1
const uint8_t qtdSpinbox = 2;           // Número de SpinBoxes
SpinBox *arraySpinbox[qtdSpinbox] = {&spinmaxmotor, &spinmaxcomp}; // Lista de SpinBoxes para renderização e controle

int maxTempMotor = 50;                  // Valor máximo da temperatura do motor (padrão)
int maxTempCompressor = 50;            // Valor máximo da temperatura do compressor (padrão)

ToggleButton btnleda(10, 245, 0);       // Botão para LED A na posição (10, 245), tela 0
ToggleButton btnledb(95, 245, 0);       // Botão para LED B
ToggleButton btnledc(180, 247, 0);      // Botão para LED C
ToggleButton btnmotor(405, 220, 0);     // Botão para controlar o motor
ToggleButton btncompressor(405, 260, 0); // Botão para controlar o compressor
const uint8_t qtdToggleBtn = 5;         // Total de botões toggle
ToggleButton *arrayTogglebtn[qtdToggleBtn] = {&btnleda, &btnledb, &btnledc, &btnmotor, &btncompressor}; // Lista de botões para atualização e interação

Image rightpng(440, 10, 0);             // Imagem com ícone (ex: seta para direita) na posição (440, 10), tela 0
Image homepng(15, 275, 1);              // Imagem com ícone de "casa" na tela 1, posição inferior esquerda
const uint8_t qtdImagem = 2;            // Total de imagens usadas
Image *arrayImagem[qtdImagem] = {&rightpng, &homepng}; // Lista de imagens para exibição e eventos

const int pinOneWire_1 = 17;             // Pino digital conectado ao primeiro sensor de temperatura (OneWire)
OneWire oneWire_1(pinOneWire_1);         // Cria o barramento OneWire no pino 17
DallasTemperature sensors_1(&oneWire_1); // Instancia o objeto da biblioteca Dallas para ler dados do sensor via OneWire


const int pinOneWire_2 = 18;             // Pino digital conectado ao segundo sensor de temperatura
OneWire oneWire_2(pinOneWire_2);         // Cria outro barramento OneWire no pino 18
DallasTemperature sensors_2(&oneWire_2); // Segundo objeto da biblioteca Dallas, associado ao segundo sensor


// Pinos de saída
const uint8_t pinLed_1 = 47;             // Pino conectado ao LED 1 (pode ser LED indicador ou parte de um circuito)
const uint8_t pinLed_2 = 48;             // Pino conectado ao LED 2
const uint8_t pinLed_3 = 1;              // Pino conectado ao LED 3
const uint8_t pinMotor = 6;              // Pino de controle para o motor (saída digital)
const uint8_t pinBomba = 7;              // Pino de controle para a bomba (ou compressor)


void setup()
{
    Serial.begin(115200); // Inicializa a comunicação serial para debug, com baudrate 115200

        const bool initialStatusPinsRele = HIGH;       // Define o estado inicial (em HIGH) para os pinos de relé (motor e bomba)

    pinMode(pinLed_1, OUTPUT);                     // Define o pino do LED 1 como saída
    digitalWrite(pinLed_1, LOW);                   // Inicia o LED 1 desligado

    pinMode(pinLed_2, OUTPUT);                     // Define o pino do LED 2 como saída
    digitalWrite(pinLed_2, LOW);                   // Inicia o LED 2 desligado

    pinMode(pinLed_3, OUTPUT);                     // Define o pino do LED 3 como saída
    digitalWrite(pinLed_3, LOW);                   // Inicia o LED 3 desligado

    pinMode(pinMotor, OUTPUT);                     // Define o pino do motor como saída
    digitalWrite(pinMotor, initialStatusPinsRele); // Inicia o motor no estado HIGH (desligado ou ligado, conforme lógica inversa)

    pinMode(pinBomba, OUTPUT);                     // Define o pino da bomba/compressor como saída
    digitalWrite(pinBomba, initialStatusPinsRele); // Inicia a bomba no estado HIGH


    // Inicializa o barramento SPI compartilhado com os pinos definidos para o display
    spi_shared.begin(DISP_SCLK, DISP_MISO, DISP_MOSI);

    // Cria um objeto de barramento SPI de hardware com os pinos do display
    bus = new Arduino_HWSPI(DISP_DC, DISP_CS, DISP_SCLK, DISP_MOSI, DISP_MISO, &spi_shared);

    // Cria o objeto da tela com controlador ILI9488
    tft = new Arduino_ILI9488_18bit(bus, DISP_RST, rotationScreen, false);

    // Inicializa o display com a frequência de SPI especificada
    tft->begin(DISP_FREQUENCY);

    // Define o objeto global do display dentro da classe base dos widgets (para que todos possam desenhar)
    WidgetBase::objTFT = tft;

    // Inicializa o controle de toque com a biblioteca personalizada `displayfk`
    // Passa as dimensões do display, rotação, pino CS do touch, SPI compartilhado, ponteiro do display, frequência do touch e do display
    myDisplay.startTouchXPT2046(DISPLAY_W, DISPLAY_H, rotationScreen, TC_CS, &spi_shared, tft, TOUCH_FREQUENCY, DISP_FREQUENCY, DISP_CS);

    // myDisplay.recalibrate(); // (opcional) re-calibração do touch se necessário
    myDisplay.checkCalibration(); // Verifica se o touch está calibrado, pode carregar dados salvos
    // myDisplay.enableTouchLog(); // (opcional) ativa log de toque na serial para debug


    sensors_1.begin(); // Inicializa o primeiro sensor de temperatura (OneWire)
    sensors_2.begin(); // Inicializa o segundo sensor


    loadWidgets(); // Função que configura todos os widgets (botões, gráficos, spinboxes, etc.)
    WidgetBase::loadScreen = screen0; // Define qual tela será carregada inicialmente ao iniciar a interface gráfica
    myDisplay.createTask(); // Cria a tarefa principal da interface gráfica (usa FreeRTOS para lidar com toque e atualização de tela)

}

void loop()
{

    widget.setValue(random(0, 100)); //Use this command to change widget value.
    gauge.setValue(random(0, 100)); //Use this command to change widget value.
    linhaverde.setText("Line 1"); //Use this command to change widget value.
    linhalaranja.setText("Line 2"); //Use this command to change widget value.
    grafico.push(0, random(0, 100)); //Use this line to add value in serie 0.
    grafico.push(1, random(0, 100)); //Use this line to add value in serie 1.
    // Use the variable spin0_val to check value/status of widget spinmaxtemp
    delay(2000);
    // Commands for plugin Sensor DS18b20
    sensors_1.requestTemperatures();
    delay(750);
    float temperatureC_1 = sensors_1.getTempCByIndex(0);
    Serial.print("Temperature: ");
    Serial.print(temperatureC_1);
    Serial.println("°C");
    // Commands for plugin Sensor DS18b20
    sensors_2.requestTemperatures();
    delay(750);
    float temperatureC_2 = sensors_2.getTempCByIndex(0);
    Serial.print("Temperature: ");
    Serial.print(temperatureC_2);
    Serial.println("°C");
    // Commands for plugin Digital Write
    digitalWrite(pinLed_1, HIGH);  // Liga o rele(true);
    delay(5000);
    // Commands for plugin Digital Write
    digitalWrite(pinLed_2, HIGH);  // Liga o rele(true);
    delay(5000);
    // Commands for plugin Digital Write
    digitalWrite(pinLed_3, HIGH);  // Liga o rele(true);
    delay(5000);
    // Commands for plugin Digital Write
    digitalWrite(pinMotor, HIGH);  // Liga o rele(true);
    delay(5000);
    // Commands for plugin Digital Write
    digitalWrite(pinBomba, HIGH);  // Liga o rele(true);
    delay(5000);
}

void screen0()
{

    tft->fillScreen(CFK_GREY3); // Preenche toda a tela com a cor cinza escuro (CFK_GREY3) como fundo
    WidgetBase::backgroundColor = CFK_GREY3; // Define a cor de fundo padrão para os widgets desenhados nesta tela

    // Escreve o texto "Tela de controle" na posição (165, 7), centralizado à esquerda (TL_DATUM)
    // Usa cor de texto CFK_COLOR07 cor de fundo CFK_GREY3 e fonte Roboto tamanho 10pt
    myDisplay.printText("Tela de controle", 165, 7, TL_DATUM, CFK_COLOR07, CFK_GREY3, &Roboto_Regular10pt7b);

    // Desenha um retângulo arredondado com borda branca, representando a área dos LEDs
    // Posição: canto superior em (5,200), largura: 243, altura: 110, raio das bordas: 10
    tft->drawRoundRect(5, 200, 243, 110, 10, CFK_WHITE);

    // Escreve a palavra "LED" no topo da seção, centralizado horizontalmente
    myDisplay.printText("LED", 105, 212, TL_DATUM, CFK_WHITE, CFK_GREY3, &Roboto_Regular10pt7b);

    // Desenha o retângulo da área onde ficam os botões de Motor e Compressor
    // Posição em (260, 200), largura 205, altura 108
    tft->drawRoundRect(260, 200, 205, 108, 10, CFK_WHITE);

    // Escreve "Motor" na parte superior da área correspondente ao botão do motor
    myDisplay.printText("Motor", 275, 222, TL_DATUM, CFK_WHITE, CFK_GREY3, &Roboto_Regular10pt7b);

    // Escreve "Compres." (abreviação de Compressor) na parte inferior da área
    myDisplay.printText("Compres.", 270, 267, TL_DATUM, CFK_WHITE, CFK_GREY3, &Roboto_Regular10pt7b);

    // Desenha todos os widgets que estão associados à tela de índice 0
    // Isso inclui botões, gauge, gráficos, etc., registrados via `arrayTogglebtn`, `arrayGauge`, etc.
    myDisplay.drawWidgetsOnScreen(0);
}
void screen1()
{
    tft->fillScreen(CFK_GREY3);// Preenche toda a tela com a cor de fundo cinza escuro (CFK_GREY3)
    WidgetBase::backgroundColor = CFK_GREY3;// Define essa mesma cor como cor de fundo padrão dos widgets que serão desenhados nesta tela

    // Exibe o título "Config do motor" no topo da tela, centralizado à esquerda da coordenada (165, 17)
    // Usa a cor de texto CFK_COLOR06, fundo cinza, e fonte Roboto 10pt
    myDisplay.printText("Config do motor", 165, 17, TL_DATUM, CFK_COLOR06, CFK_GREY3, &Roboto_Regular10pt7b);

    // Escreve a legenda "Limite motor" próxima ao campo onde o usuário define a temperatura máxima do motor
    myDisplay.printText("Limite motor", 50, 107, TL_DATUM, CFK_WHITE, CFK_GREY3, &Roboto_Regular10pt7b);

    // Escreve a legenda "Limite compr." (limite do compressor) próximo ao respectivo campo
    myDisplay.printText("Limite compr.", 50, 200, TL_DATUM, CFK_WHITE, CFK_GREY3, &Roboto_Regular10pt7b);

    // Desenha todos os widgets associados à tela de índice 1
    // Aqui devem ser desenhados os dois SpinBox (spinmaxmotor e spinmaxcomp), conforme configurados no array arraySpinbox
    myDisplay.drawWidgetsOnScreen(1);
}

// Configure each widgtes to be used
void loadWidgets()
{

    GaugeConfig configGauge1 = {
        .width = 250,                    // Largura do gauge em pixels
        .title = "Motor",                // Título exibido no topo ou dentro do gauge
        .intervals = range1,             // Ponteiro para o array com os valores de intervalo (faixas de temperatura)
        .colors = colors1,               // Ponteiro para o array com as cores correspondentes aos intervalos
        .amountIntervals = qtdIntervalG1, // Quantidade de faixas (deve ser igual ao tamanho dos arrays 'range1' e 'colors1')
        .minValue = 0,                   // Valor mínimo representado no gauge
        .maxValue = 100,                 // Valor máximo representado no gauge
        .borderColor = CFK_BLACK,        // Cor da borda externa do gauge
        .textColor = CFK_BLACK,          // Cor dos textos numéricos (valores da escala)
        .backgroundColor = CFK_WHITE,    // Cor de fundo da área interna do gauge
        .titleColor = CFK_BLACK,         // Cor do título do gauge
        .needleColor = CFK_RED,          // Cor da agulha (ponteiro) do gauge
        .markersColor = CFK_BLACK,       // Cor dos marcadores da escala (linhas divisórias)
        .showLabels = true,              // Define se os rótulos numéricos devem ser exibidos (true = sim)
        .fontFamily = &Roboto_Bold10pt7b // Fonte utilizada para os textos dentro do gauge
    };
    gaugecontrole.setup(configGauge1);//Associa a configuração ao gauge
    
    myDisplay.setGauge(arrayGauge, qtdGauge);//Envia para a biblioteca displayFK quais e quantos são os gauges no projetos.

    LabelConfig configLabel0 = {
        .text = "Line 1",                          // Texto principal do label
        .fontFamily = &Roboto_Regular10pt7b,     // Fonte usada para o texto
        .datum = TL_DATUM,                        // Ponto de referência para posicionamento do texto (alinhamento)
        .fontColor = CFK_COLOR10,                 // Cor da fonte do texto principal
        .backgroundColor = CFK_GREY3,             // Cor de fundo do label
        .prefix = "Motor ",                        // Texto fixo exibido antes do valor dinâmico
        .suffix = " C"                            // Texto fixo exibido após o valor dinâmico (unidade °C)
    };
    linhaverde.setup(configLabel0); //Associa a configuração ao label
    
    LabelConfig configLabel1 = {
        .text = "Line 2",                          // Texto principal do label
        .fontFamily = &Roboto_Regular10pt7b,     // Fonte usada para o texto
        .datum = TL_DATUM,                        // Ponto de referência para posicionamento do texto (alinhamento)
        .fontColor = CFK_COLOR04,                 // Cor da fonte do texto principal
        .backgroundColor = CFK_GREY3,             // Cor de fundo do label
        .prefix = "Compr. ",                       // Texto fixo exibido antes do valor dinâmico
        .suffix = " C"                            // Texto fixo exibido após o valor dinâmico (unidade °C)
    };
    linhalaranja.setup(configLabel1);//Associa a configuração ao label
    
    myDisplay.setLabel(arrayLabel, qtdLabel);//Envia para a biblioteca displayFK quais e quantos são os labels no projetos.

    LineChartConfig configLineChart0 = {
        .width = 200,                          // Largura do gráfico em pixels
        .height = 118,                         // Altura do gráfico em pixels
        .minValue = 0,                        // Valor mínimo no eixo Y do gráfico
        .maxValue = 100,                      // Valor máximo no eixo Y do gráfico
        .amountSeries = qtdLinesChart0,       // Quantidade de séries de dados (linhas) no gráfico
        .colorsSeries = colorsChart0,          // Array com as cores usadas para cada série
        .gridColor = CFK_GREY6,                // Cor da grade de fundo do gráfico
        .borderColor = CFK_BLACK,              // Cor da borda do gráfico
        .backgroundColor = CFK_GREY4,          // Cor de fundo do gráfico
        .textColor = CFK_WHITE,                // Cor do texto (e.g. números da escala)
        .verticalDivision = 5,                 // Número de divisões verticais na grade
        .workInBackground = false,             // Indica se o gráfico atualiza em background (false = no loop principal)
        .showZeroLine = false,                 // Indica se a linha do valor zero deve ser exibida
        .boldLine = true,                      // Exibe linhas da grade com espessura maior
        .showDots = false,                     // Exibe pontos nos valores das séries (false = linhas lisas)
        .maxPointsAmount = LineChart::SHOW_ALL, // Número máximo de pontos mostrados (SHOW_ALL = exibe todos)
        .font = &Roboto_Regular5pt7b,          // Fonte usada para legendas e textos no gráfico
        .subtitles = seriesGrafico0            // Array de labels que descreve cada série do gráfico
    };
    grafico.setup(configLineChart0);//Associa a configuração ao grafico
    
    myDisplay.setLineChart(arrayLinechart, qtdLineChart);//Envia para a biblioteca displayFK quais e quantos são os graficos no projetos.
    
    SpinBoxConfig configSpinBox0 = {
        .width = 214,                     // Largura do SpinBox em pixels
        .height = 65,                    // Altura do SpinBox em pixels
        .step = 1,                      // Incremento/decremento do valor ao ajustar o SpinBox
        .minValue = 0,                  // Valor mínimo permitido no SpinBox
        .maxValue = 100,                // Valor máximo permitido no SpinBox
        .startValue = 50,               // Valor inicial do SpinBox
        .color = CFK_GREY14,            // Cor de fundo do SpinBox
        .textColor = CFK_BLACK,         // Cor do texto do valor exibido
        .callback = spinmaxmotor_cb     // Função chamada quando o valor do SpinBox é modificado
    };
    spinmaxmotor.setup(configSpinBox0);

    SpinBoxConfig configSpinBox1 = {
        .width = 214,                     // Largura do SpinBox em pixels
        .height = 63,                    // Altura do SpinBox em pixels
        .step = 1,                      // Incremento/decremento do valor ao ajustar o SpinBox
        .minValue = 0,                  // Valor mínimo permitido no SpinBox
        .maxValue = 100,                // Valor máximo permitido no SpinBox
        .startValue = 50,               // Valor inicial do SpinBox
        .color = CFK_GREY14,            // Cor de fundo do SpinBox
        .textColor = CFK_BLACK,         // Cor do texto do valor exibido
        .callback = spinmaxcomp_cb      // Função chamada quando o valor do SpinBox é modificado
    };
    spinmaxcomp.setup(configSpinBox1);
    myDisplay.setSpinbox(arraySpinbox, qtdSpinbox);
    
    ToggleButtonConfig configToggle0 = {
        .width = 56,                    // Largura do botão toggle em pixels
        .height = 28,                   // Altura do botão toggle em pixels
        .pressedColor = CFK_COLOR18,   // Cor exibida quando o botão está pressionado
        .callback = btnleda_cb          // Função chamada ao clicar no botão
    };
    btnleda.setup(configToggle0);
    
    ToggleButtonConfig configToggle1 = {
        .width = 55,                    // Largura do botão toggle em pixels
        .height = 28,                   // Altura do botão toggle em pixels
        .pressedColor = CFK_COLOR18,   // Cor exibida quando o botão está pressionado
        .callback = btnledb_cb          // Função chamada ao clicar no botão
    };
    btnledb.setup(configToggle1);
    
    ToggleButtonConfig configToggle2 = {
        .width = 53,                    // Largura do botão toggle em pixels
        .height = 27,                   // Altura do botão toggle em pixels
        .pressedColor = CFK_COLOR18,   // Cor exibida quando o botão está pressionado
        .callback = btnledc_cb          // Função chamada ao clicar no botão
    };
    btnledc.setup(configToggle2);
    
    ToggleButtonConfig configToggle3 = {
        .width = 47,                    // Largura do botão toggle em pixels
        .height = 24,                   // Altura do botão toggle em pixels
        .pressedColor = CFK_COLOR09,   // Cor exibida quando o botão está pressionado
        .callback = btnmotor_cb          // Função chamada ao clicar no botão
    };
    btnmotor.setup(configToggle3);
    
    ToggleButtonConfig configToggle4 = {
        .width = 47,                    // Largura do botão toggle em pixels
        .height = 24,                   // Altura do botão toggle em pixels
        .pressedColor = CFK_COLOR04,   // Cor exibida quando o botão está pressionado
        .callback = btncompressor_cb     // Função chamada ao clicar no botão
    };
    myDisplay.setToggle(arrayTogglebtn, qtdToggleBtn);
    
    ImageFromFileConfig configImage0 = {
        .source = SourceFile::SPIFFS,    // Fonte da imagem, neste caso arquivo armazenado no SPIFFS
        .path = "/Rightpng.fki",         // Caminho do arquivo da imagem no SPIFFS
        .cb = rightpng_cb,               // Função callback chamada ao clicar na imagem
        .angle = 0                      // Ângulo de rotação da imagem em graus (0 = sem rotação)
    };
    rightpng.setup(configImage0);
    ImageFromFileConfig configImage1 = {
        .source = SourceFile::SPIFFS,    // Fonte da imagem, neste caso arquivo armazenado no SPIFFS
        .path = "/Homepng.fki",          // Caminho do arquivo da imagem no SPIFFS
        .cb = homepng_cb,                // Função callback chamada ao clicar na imagem
        .angle = 0                      // Ângulo de rotação da imagem em graus (0 = sem rotação)
    };
    homepng.setup(configImage1);
    myDisplay.setImage(arrayImagem, qtdImagem);
}

// Quando clica no botão de seleção de temperatura máxima, atualiza o valor da variavel de temperatura máxima
void spinmaxmotor_cb()
{
    maxTempMotor = spinmaxmotor.getValue();
}

// Quando clica no botão de seleção de temperatura máxima, atualiza o valor da variavel de temperatura máxima
void spinmaxcomp_cb()
{
    maxTempCompressor = spinmaxcomp.getValue();
}

// Quando clica no botão leda, ativa/desativa o rele do ledA
void btnleda_cb()
{
    bool myValue = btnleda.getStatus();
}

// Quando clica no botão ledb, ativa/desativa o rele do ledB
void btnledb_cb()
{
    bool myValue = btnledb.getStatus();
}

// Quando clica no botão ledc, ativa/desativa o rele do ledC
void btnledc_cb()
{
    bool myValue = btnledc.getStatus();
}

// Quando clica no botão motor, ativa/desativa o rele do motor
void btnmotor_cb()
{
    bool myStatus = btnmotor.getStatus();
}

// Quando clica no botão compressor, ativa/desativa o rele da bomba
void btncompressor_cb()
{
    bool myStatus = btncompressor.getStatus();

    
}

// Quando clica no botão right, carrega a tela de configuração
void rightpng_cb()
{
   
}

// Quando clica no botão home, carrega a tela de controle
void homepng_cb()
{
    
}