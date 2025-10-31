#ifndef DISPLAY_LIB_H
#define DISPLAY_LIB_H

#include <Arduino.h>

#include <SPI.h>
#include <Arduino_GFX_Library.h>
#include "variaveis.h"
#include "touch.h"

Arduino_DataBus *bus = nullptr; ///< Barramento de dados compartilhado para o display.
Arduino_GFX *tft = nullptr; ///< Ponteiro para instância do driver gráfico (ST7796).
TaskHandle_t taskDisplay = NULL; ///< Handle da task responsável por atualizar o display.

SPIClass spi_shared(HSPI);

#define POINT_IN_RECT(px, py, x, y, w, h) (((px) >= (x)) && ((px) < ((x) + (w))) && ((py) >= (y)) && ((py) < ((y) + (h)))) ///< Teste de ponto em retângulo.
#define POINT_IN_CIRCLE(px, py, cx, cy, r) (((px) - (cx)) * ((px) - (cx)) + ((py) - (cy)) * ((py) - (cy)) <= (r) * (r)) ///< Teste de ponto em círculo.

bool touchedCircle = false; ///< Estado de toque sobre o botão circular (se usado).
bool touchedRestart = false; ///< Estado de toque para reinicialização (se usado).

bool drawingScreen = false; ///< Verdadeiro quando a tela está sendo redesenhada.

/** @brief Botão circular renderizável no display. */
typedef struct{
    int x;
    int y;
    int r;
    uint16_t color;
    bool pressed;
    uint32_t lastClick;
    int intervalClick;
}CircleButton_t;

/** @brief Botão retangular renderizável no display. */
typedef struct{
    int x;
    int y;
    int w;
    int h;
    const char* text;
    uint16_t outColor;
    uint16_t inColor;
    bool pressed;
    uint32_t lastClick;
    int intervalClick;
}RectButton_t;

/** @brief Estados para o LED de status renderizado na tela. */
enum class LedStatus : uint8_t{OFF = 0, BUSY, READY};

const int timoutMessage = 5000; ///< Tempo de exibição da mensagem de status (ms).
unsigned long eraseMessageAt = 0; ///< Timestamp para apagar a mensagem de status.
bool showingMessage = false; ///< Indica se há mensagem de status visível.


const int idButtonSend = 0; ///< Índice do botão "Enviar".
const int idButtonUpdate = 1; ///< Índice do botão "Atual.".
const int idButtonHello = 2; ///< Índice do botão "Hello".
const int idButtonTeste = 3; ///< Índice do botão "Teste".


const int amountButtons = 4; ///< Quantidade total de botões retangulares.

RectButton_t rectButtons[amountButtons] = { ///< Lista de botões retangulares exibidos na tela.
    {.x = 190, .y = 10, .w = 120, .h = 50, .text = "Enviar", .outColor = 0x013c, .inColor = 0x055c,.pressed = false, .lastClick = 0, .intervalClick = 2000},
    {.x = 190, .y = 70, .w = 120, .h = 50, .text = "Atual.",.outColor = 0xe0a0, .inColor = 0xe540,.pressed = false, .lastClick = 0, .intervalClick = 2000},
    {.x = 190, .y = 130, .w = 120, .h = 50, .text = "Hello",.outColor = 0xf81e, .inColor = 0x6812,.pressed = false, .lastClick = 0, .intervalClick = 2000},
    {.x = 190, .y = 190, .w = 120, .h = 50, .text = "Teste",.outColor = 0xd7e0, .inColor = 0x1340,.pressed = false, .lastClick = 0, .intervalClick = 2000},
};



/* @brief Inicializa SPI compartilhado e o driver ST7796, configurando o display. */
void startDisplay(){
  spi_shared.begin(DISP_SCLK, DISP_MISO, DISP_MOSI);
    bus = new Arduino_HWSPI(DISP_DC, DISP_CS, DISP_SCLK, DISP_MOSI, DISP_MISO, &spi_shared);
    tft = new Arduino_ST7796(bus, DISP_RST, rotationScreen, true);
    tft->begin(DISP_FREQUENCY);
}

/* @brief Desenha um botão retangular com bordas em degradê e texto centralizado.
 * @param button Referência ao botão a ser desenhado.
 */
void drawRectButton(RectButton_t& button){
    int numTons = 10;
    uint16_t* tons = blendColors(button.outColor, button.inColor, numTons);

    tft->fillRect(button.x, button.y, button.w, button.h, button.inColor);

    for(int i = numTons; i > 0; i--){
        int offset = numTons - i;
        uint16_t tom = tons[offset];

        tft->drawRect(button.x + offset, button.y + offset, button.w - (offset * 2), button.h - (offset * 2), tom);
    }
    
    tft->setTextColor(COLOR_WHITE);
    tft->setTextSize(2);
    int16_t x, y;
    uint16_t w, h;
    int buttonCenterW = button.w / 2 + button.x;
    int buttonCenterH = button.h / 2 + button.y;
    
    tft->getTextBounds(button.text, 0, 0, &x, &y, &w, &h);
    
    tft->setCursor(buttonCenterW - (w / 2), buttonCenterH - (h / 2));
    tft->println(button.text);

    delete[] tons; // libera a memória alocada
}

/* @brief Redesenha toda a tela: limpa e desenha todos os botões. */
void drawScreen(){
    drawingScreen = true;
    tft->fillScreen(COLOR_BLACK);
    /*for(auto i = 0; i < amountButtons; i++){
        CircleButton_t button = circleButtons[i];
        tft->fillCircle(button.x,   button.y, button.r, button.color);    
    }*/
    

    for(auto i = 0; i < amountButtons; i++){
        RectButton_t button = rectButtons[i];
        drawRectButton(button);
    }

    drawingScreen = false;
}

/* @brief Desenha um LED de status (OFF/BUSY/READY) no canto superior esquerdo. */
void showStatusLed(LedStatus status){
    if(!tft){
        return;
    }

    int x = 20;
    int y = 20;
    int radius = 10;

    switch (status)
    {
    case LedStatus::OFF:
        {
            tft->fillCircle(x, y, radius, COLOR_BLACK);
            tft->drawCircle(x, y, radius, COLOR_WHITE);
        }
        break;
    case LedStatus::BUSY:
        {
            tft->fillCircle(x, y, radius, COLOR_YELLOW);
            tft->drawCircle(x, y, radius, COLOR_WHITE);
        }
        break;
    case LedStatus::READY:
        {
            tft->fillCircle(x, y, radius, COLOR_GREEN);
            tft->drawCircle(x, y, radius, COLOR_WHITE);
        }
        break;
    default:
        break;
    }
}

/* @brief Limpa a área reservada à mensagem de status no rodapé. */
void clearMessageStatus(){
    tft->fillRect(0, tft->height() - 30, tft->width(), 30, COLOR_WHITE);
}
/* @brief Exibe uma mensagem de status no rodapé por tempo limitado.
 * @param message Texto a ser exibido.
 */
void showMessageStatus(const String& message){
    if(drawingScreen) return;

    

    const int margin = 3;

    clearMessageStatus();
    tft->setTextColor(COLOR_BLACK);
    tft->setTextSize(2);
    tft->setCursor(margin, tft->height() - 30 + margin);
    tft->println(message);
    Serial.print("Status: ");
    Serial.println(message);

    unsigned long agora = millis();
    eraseMessageAt = agora + timoutMessage;
    showingMessage = true;
}


/* @brief Task RTOS principal de UI: lê touch, atualiza destaques e trata cliques.
 * @param parameter Não utilizado.
 */
void runTaskDisplay(void *parameter)
{
    drawScreen();

    int lastX = 0;
    int lastY = 0;
    while (1)
    {
        int x, y;
        bool hasTouch = check_touch(&x, &y);
        if (hasTouch)
        {
            tft->drawCircle(lastX, lastY, 5, COLOR_BLACK);
            tft->drawCircle(x, y, 5, COLOR_WHITE);

            for(auto i = 0; i < amountButtons; i++){
                RectButton_t *button = &rectButtons[i];
                bool isInRect = POINT_IN_RECT(x, y, button->x, button->y, button->w, button->h);
                if(isInRect && !button->pressed && millis() - button->lastClick > button->intervalClick){
                    button->pressed = true;
                    button->lastClick = millis();
                }
            }
            

            lastX = x;
            lastY = y;
        }

        if(showingMessage && millis() > eraseMessageAt){
            clearMessageStatus();
            showingMessage = false;
        }

        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

/* @brief Cria a task do display fixada no core 0.
 * Inicializa `taskDisplay` e reporta erro via Serial se falhar.
 */
void createTaskTestDisplay(){
    auto retorno = xTaskCreatePinnedToCore(runTaskDisplay, "taskDisplay", 10000, NULL, 2, &taskDisplay, 0);
    if (retorno != pdPASS)
    {
        Serial.println("Erro ao criar task para display");
    }
}


#endif
