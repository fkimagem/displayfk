#include "widgetbase.h"

#define DEBUG_WIDGETBASE

#ifdef DEBUG_WIDGETBASE
#define DEBUG_D(fmt, ...) Serial.printf(fmt, ##__VA_ARGS__)
#define DEBUG_E(fmt, ...) Serial.printf(fmt, ##__VA_ARGS__)
#define DEBUG_W(fmt, ...) Serial.printf(fmt, ##__VA_ARGS__)
#else
#define DEBUG_D(fmt, ...)
#define DEBUG_E(fmt, ...)
#define DEBUG_W(fmt, ...)
#endif

bool WidgetBase::usingKeyboard = false;
uint8_t WidgetBase::currentScreen = 0;
bool WidgetBase::showingLog = false;
bool WidgetBase::lightMode = true;
uint16_t WidgetBase::backgroundColor = 0x0;
const GFXfont *WidgetBase::fontNormal = nullptr;
const GFXfont *WidgetBase::fontBold = nullptr;
QueueHandle_t WidgetBase::xFilaCallback;

#if defined(DISP_DEFAULT)
Arduino_GFX *WidgetBase::objTFT = nullptr;
#else
#error "You need to define the display type in user_setup.h"
#endif

uint16_t WidgetBase::screenWidth = 480;
uint16_t WidgetBase::screenHeight = 240;
// unsigned long WidgetBase::sharedTime = 0;
functionLoadScreen_t WidgetBase::loadScreen = nullptr;
#if defined(DFK_SD)
fs::SDFS *WidgetBase::mySD = nullptr;
#endif

/**
 * @brief Lightens a color by a given factor.
 * @param color The color to lighten.
 * @param factor The factor to lighten the color by. (0.0 to 5.0)
 * @return The lightened color.
 */
uint16_t WidgetBase::lightenColor565(unsigned short color, float factor)
{
    factor = constrain(factor, 0.0, 5.0);
    factor /= 20.0;

    // Extrair os componentes de cor
    unsigned short red   = (color >> 11) & 0x1F; // 5 bits para vermelho
    unsigned short green = (color >> 5) & 0x3F;  // 6 bits para verde
    unsigned short blue  = color & 0x1F;         // 5 bits para azul

    // Clarear os componentes de cor
    red   = _min(static_cast<unsigned short>(red * (1 + factor)), 0x1F);
    green = _min(static_cast<unsigned short>(green * (1 + factor)), 0x3F);
    blue  = _min(static_cast<unsigned short>(blue * (1 + factor)), 0x1F);

    // Recombinar os componentes em uma cor 565
    return (red << 11) | (green << 5) | blue;
}

uint16_t WidgetBase::lightenToWhite565(uint16_t color, float factor)
{
    // limita factor entre 0.0 (original) e 1.0 (branco puro)
    if (factor < 0.0f) factor = 0.0f;
    if (factor > 1.0f) factor = 1.0f;

    // extrair componentes
    uint8_t r = (color >> 11) & 0x1F; // 5 bits
    uint8_t g = (color >> 5) & 0x3F;  // 6 bits
    uint8_t b = color & 0x1F;         // 5 bits

    // máximos
    const uint8_t Rmax = 0x1F;
    const uint8_t Gmax = 0x3F;
    const uint8_t Bmax = 0x1F;

    // para clarear, aproximamos cada componente do máximo
    r = r + (uint8_t)((Rmax - r) * factor);
    g = g + (uint8_t)((Gmax - g) * factor);
    b = b + (uint8_t)((Bmax - b) * factor);

    // recombinar
    return (r << 11) | (g << 5) | b;
}

/**
 * @brief Blends two colors with a given factor.
 * @param color1 The first color to blend.
 * @param color2 The second color to blend.
 * @param factor The factor to blend the colors by.
 * @return The blended color.
 */
uint16_t WidgetBase::blendColors(uint16_t color1, uint16_t color2, float factor) {
    uint8_t r1, g1, b1;
    uint8_t r2, g2, b2;
    
    // Extrai as cores RGB de ambas as cores no formato RGB565
    WidgetBase::extract565toRGB(color1, r1, g1, b1);
    WidgetBase::extract565toRGB(color2, r2, g2, b2);
    
    // Calcula a cor intermediária com base no fator
    uint8_t r = r1 * (1.0 - factor) + r2 * factor;
    uint8_t g = g1 * (1.0 - factor) + g2 * factor;
    uint8_t b = b1 * (1.0 - factor) + b2 * factor;
    
    // Converte a cor intermediária para o formato RGB565 e retorna
    return convertToRGB565(r, g, b);
}

/**
 * @brief Extracts the RGB components from a color in RGB565 format.
 * @param color565 The color in RGB565 format.
 * @param r Reference to store the red component.
 * @param g Reference to store the green component.
 * @param b Reference to store the blue component.
 */
void WidgetBase::extract565toRGB(uint16_t color565, uint8_t &r, uint8_t &g, uint8_t &b) {
    r = (color565 >> 11) & 0x1F; // Extrai o componente vermelho (5 bits)
    g = (color565 >> 5) & 0x3F;  // Extrai o componente verde (6 bits)
    b = color565 & 0x1F;         // Extrai o componente azul (5 bits)
    
    // Normaliza para 8 bits
    r = (r << 3) | (r >> 2);
    g = (g << 2) | (g >> 4);
    b = (b << 3) | (b >> 2);
}

/**
 * @brief Converts RGB components to RGB565 format.
 * @param r The red component.
 * @param g The green component.
 * @param b The blue component.
 * @return The color in RGB565 format.
 */
uint16_t WidgetBase::convertToRGB565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}


/**
 * @brief Darkens a color by a given factor.
 * @param color The color to darken.
 * @param factor The factor to darken the color by.
 * @return The darkened color.
 */
uint16_t WidgetBase::darkenColor565(unsigned short color, float factor)
{
    factor = constrain(factor, 0.0, 5.0);
    factor /= 20.0;
    // Extrair os componentes de cor
    unsigned short red   = (color >> 11) & 0x1F; // 5 bits para vermelho
    unsigned short green = (color >> 5) & 0x3F;  // 6 bits para verde
    unsigned short blue  = color & 0x1F;         // 5 bits para azul

    // Escurecer os componentes de cor
    red   = _max(static_cast<unsigned short>(red * (1 - factor)), 0x0);
    green = _max(static_cast<unsigned short>(green * (1 - factor)), 0x0);
    blue  = _max(static_cast<unsigned short>(blue * (1 - factor)), 0x0);

    // Recombinar os componentes em uma cor 565
    return (red << 11) | (green << 5) | blue;
}

/**
 * @brief Constructor for the WidgetBase class.
 * @param _x X position of the widget.
 * @param _y Y position of the widget.
 * @param _screen Screen number.
 */
WidgetBase::WidgetBase(uint16_t _x, uint16_t _y, uint8_t _screen) : xPos(_x), yPos(_y), screen(_screen), loaded(false), cb(nullptr)
{
}

/**
 * @brief Destructor for the WidgetBase class.
 */
WidgetBase::~WidgetBase()
{
    // Limpa o callback se existir
    if (cb != nullptr) {
        cb = nullptr;
    }
    
    // Reseta o estado do widget
    loaded = false;
    
    // Não é necessário limpar xPos, yPos e screen pois são valores primitivos
    // Não é necessário limpar fontNormal e fontBold pois são ponteiros estáticos compartilhados
}

/**
 * @brief Detects a touch on the widget.
 * @param _xTouch Pointer to the X-coordinate of the touch.
 * @param _yTouch Pointer to the Y-coordinate of the touch.
 * @return True if the touch is detected, false otherwise.
 */
bool WidgetBase::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch)
{
    return false;
}

/**
 * @brief Retrieves the callback function for the widget.
 * @return Pointer to the callback function.
 */
functionCB_t WidgetBase::getCallbackFunc()
{
    return nullptr;
}

/**
 * @brief Adds a callback to the callback queue
 * @param callback Callback to be added
 * @param origin Origin of the callback
 */
void WidgetBase::addCallback(functionCB_t callback, CallbackOrigin origin){
    if(WidgetBase::xFilaCallback != NULL){

        if(callback == nullptr){
            DEBUG_E("Callback is nullptr. Not adding to queue callback.\n");
            return;
        }
        
        if(uxQueueSpacesAvailable(WidgetBase::xFilaCallback) > 0){
            if(xQueueSend(WidgetBase::xFilaCallback, &callback, pdMS_TO_TICKS(50)) == pdPASS){
                DEBUG_D("Callback added to queue. Origin: %d\n", origin);
            }else{
                DEBUG_E("Can't add callback. Queue is full\n");
            }
        }else{
            DEBUG_E("Can't add callback. Queue is full\n");
        }
    }else{
        DEBUG_E("Can't add callback. Queue is not created\n");
    }
}

/**
 * @brief Retrieves the best fitting Roboto Bold font for a given area.
 * @param availableWidth Available width for text.
 * @param availableHeight Available height for text.
 * @param texto Text string to be displayed.
 * @return Pointer to the best fitting font.
 */
const GFXfont *WidgetBase::GetBestRobotoBold(uint16_t availableWidth, uint16_t availableHeight, const char* texto)
{
    // Array de fontes disponíveis, ordenadas do maior para o menor
    static const GFXfont* const fontes[] = {
        &Roboto_Bold50pt7b,
        &Roboto_Bold40pt7b,
        &Roboto_Bold30pt7b,
        &Roboto_Bold20pt7b,
        &Roboto_Bold10pt7b
    };
    static const int numFontes = sizeof(fontes) / sizeof(fontes[0]);

    // Função para calcular dimensões do texto
    auto calcularDimensoes = [](const GFXfont* font, const char* texto, uint16_t* width, uint16_t* height) {
        TextBound_t area;
        objTFT->setFont(font);
        objTFT->getTextBounds(texto, 0, 0, &area.x, &area.y, &area.width, &area.height);
        *width = area.width;
        *height = area.height;
    };

    uint16_t textWidth, textHeight;
    
    // Testa cada fonte até encontrar uma que se encaixe
    for (int i = 0; i < numFontes; i++) {
        calcularDimensoes(fontes[i], texto, &textWidth, &textHeight);
        if (textWidth <= availableWidth && textHeight <= availableHeight) {
            return fontes[i];
        }
    }

    // Se nenhuma fonte se encaixou, retorna a menor fonte disponível
    return fontes[numFontes - 1];
}

/**
 * @brief Updates the font according to the specified font type.
 * @param _f Font type to set (NORMAL, BOLD).
 */
void WidgetBase::updateFont(FontType _f)
{
    WidgetBase::objTFT->setTextSize(1);
    switch (_f)
    {
    case FontType::NORMAL:
    {
#if defined(DISP_DEFAULT)
        WidgetBase::objTFT->setFont(WidgetBase::fontNormal);
#endif
    }
    break;
    case FontType::BOLD:
    {
#if defined(DISP_DEFAULT)
        WidgetBase::objTFT->setFont(WidgetBase::fontBold);
#endif
    }
    break;

    default:
    {
#if defined(DISP_DEFAULT)
        WidgetBase::objTFT->setFont(nullptr);
#endif
    }
    break;
    }
}

#if defined(DISP_DEFAULT)
/**
 * @brief Prints text on the screen.
 * @param _texto Text to print.
 * @param _x X position of the text.
 * @param _y Y position of the text.
 * @param _datum Datum of the text.
 */
void WidgetBase::printText(const char* _texto, uint16_t _x, uint16_t _y, uint8_t _datum){
    //int ox = _x, oy = _y;
    WidgetBase::recalculateTextPosition(_texto, &_x, &_y, _datum);

    TextBound_t areaAux;
    objTFT->getTextBounds(_texto, _x, _y, &areaAux.x, &areaAux.y, &areaAux.width, &areaAux.height);

    objTFT->setCursor(_x, _y);
    objTFT->print(_texto);

    #ifdef DEBUG_TEXT_BOUND
        objTFT->drawRect(areaAux.x, areaAux.y, areaAux.width, areaAux.height, CFK_FUCHSIA);
        objTFT->fillCircle(_x, _y, 3, CFK_RED);
    #endif

}

/**
 * @brief Prints text on the screen.
 * @param _texto Text to print.
 * @param _x X position of the text.
 * @param _y Y position of the text.
 * @param _datum Datum of the text.
 * @param lastTextBoud Reference to store the last text bounds.
 * @param _colorPadding Color of the padding.
 */
void WidgetBase::printText(const char* _texto, uint16_t _x, uint16_t _y, uint8_t _datum, TextBound_t &lastTextBoud, uint16_t _colorPadding){
    objTFT->fillRect(lastTextBoud.x, lastTextBoud.y, lastTextBoud.width, lastTextBoud.height, _colorPadding);
    //int ox = _x, oy = _y;
    WidgetBase::recalculateTextPosition(_texto, &_x, &_y, _datum);

    TextBound_t areaAux;
    objTFT->getTextBounds(_texto, _x, _y, &areaAux.x, &areaAux.y, &areaAux.width, &areaAux.height);

    lastTextBoud.x = areaAux.x;
    lastTextBoud.y = areaAux.y;
    lastTextBoud.width = areaAux.width;
    lastTextBoud.height = areaAux.height;

    objTFT->setCursor(_x, _y);
    objTFT->print(_texto);

    #ifdef DEBUG_TEXT_BOUND
        objTFT->drawRect(lastTextBoud.x, lastTextBoud.y, lastTextBoud.width, lastTextBoud.height, CFK_MAGENTA);
        objTFT->fillCircle(_x, _y, 3, CFK_RED);
        //log_d("Wrote on screen: %s at %d, %d", _texto, _x, _y);
    #endif

}

/**
 * @brief Recalculates the position of text on the screen.
 * @param _texto Text to recalculate position for.
 * @param _x Pointer to the X position of the text.
 * @param _y Pointer to the Y position of the text.
 * @param _datum Datum of the text.
 */
void WidgetBase::recalculateTextPosition(const char *_texto, uint16_t *_x, uint16_t *_y, uint8_t _datum)
{
    static const int16_t OFFSET_PADDING_Y = 6;
    
    // Obtém as dimensões do texto
    TextBound_t area;
    objTFT->getTextBounds(_texto, 0, 0, &area.x, &area.y, &area.width, &area.height);

    // Calcula os offsets baseados no datum
    int16_t xOffset = 0;
    int16_t yOffset = 0;

    // Calcula o offset horizontal
    switch (_datum) {
        case TC_DATUM:
        case MC_DATUM:
        case BC_DATUM:
            xOffset = -(area.width / 2);
            break;
        case TR_DATUM:
        case MR_DATUM:
        case BR_DATUM:
            xOffset = -area.width;
            break;
        default: // TL_DATUM, ML_DATUM, BL_DATUM
            xOffset = 0;
            break;
    }

    // Calcula o offset vertical
    switch (_datum) {
        case TL_DATUM:
        case TC_DATUM:
        case TR_DATUM:
            yOffset = area.height;
            break;
        case ML_DATUM:
        case MC_DATUM:
        case MR_DATUM:
            yOffset = area.height / 2;
            break;
        case BL_DATUM:
        case BC_DATUM:
        case BR_DATUM:
            yOffset = -OFFSET_PADDING_Y;
            break;
        default:
            yOffset = 0;
            break;
    }

    // Aplica os offsets
    *_x += xOffset;
    *_y += yOffset;
}

/**
 * @brief Gets the text bounds for a given string.
 * @param str The string to get the bounds for.
 * @param x The X position of the text.
 * @param y The Y position of the text.
 * @return The text bounds.
 */
TextBound_t WidgetBase::getTextBounds(const char *str, int16_t x, int16_t y)
{
    TextBound_t retorno;
    WidgetBase::objTFT->getTextBounds(str, x, y, &retorno.x, &retorno.y, &retorno.width, &retorno.height);
    return retorno;
}

/**
 * @brief Draws a rotated image on the screen.
 * @param image The image to draw.
 * @param width The width of the image.
 * @param height The height of the image.
 * @param angle The angle to rotate the image.
 * @param pivotX The X position of the pivot.
 * @param pivotY The Y position of the pivot.
 * @param drawX The X position to draw the image.
 * @param drawY The Y position to draw the image.
 */
void WidgetBase::drawRotatedImageOptimized(uint16_t *image, int16_t width, int16_t height, float angle, int16_t pivotX, int16_t pivotY, int16_t drawX, int16_t drawY)
{
    log_d("Drawing image with  %i x %i and angle %f at pos %i x %i", width, height, angle, drawX, drawY);
    // Converte o ângulo para radianos
    float radians = angle * DEG_TO_RAD;

    // Pré-calcula o seno e o cosseno do ângulo
    float cosA = cos(radians);
    float sinA = sin(radians);

    // Pré-calcula limites de varredura
    int16_t minX = -pivotX;
    int16_t maxX = width - pivotX;
    int16_t minY = -pivotY;
    int16_t maxY = height - pivotY;

    // Desenha a imagem rotacionada
    objTFT->startWrite();  // Começa a transação de escrita para otimizar a comunicação com o display
    for (int16_t y = minY; y < maxY; y++) {
        for (int16_t x = minX; x < maxX; x++) {
            // Calcula as coordenadas rotacionadas
            int16_t rotatedX = pivotX + (x * cosA - y * sinA);
            int16_t rotatedY = pivotY + (x * sinA + y * cosA);

            // Verifica se o pixel rotacionado está dentro dos limites da tela
            if (rotatedX >= 0 && rotatedX < objTFT->width() && rotatedY >= 0 && rotatedY < objTFT->height()) {
                // Desenha o pixel rotacionado no display
                uint16_t color = image[(y + pivotY) * width + (x + pivotX)]; // Obtém a cor original no formato RGB565
                objTFT->writePixel(drawX + rotatedX, drawY + rotatedY, color); // Desenha na nova posição
            }
        }
    }
    objTFT->endWrite();  // Termina a transação de escrita
}

/**
 * @brief Shows the origin of the widget.
 * @param color The color of the origin.
 */
void WidgetBase::showOrigin(uint16_t color){
#if defined(HELPERS)
    uint8_t tam = 5;
    objTFT->drawFastVLine(xPos, yPos-tam, 2*tam, color);
    objTFT->drawFastHLine(xPos-tam, yPos, 2*tam, color);
    objTFT->fillCircle(xPos, yPos, 2, color);
#endif
}
#endif
