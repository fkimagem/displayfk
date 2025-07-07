#include "wcircularbar.h"

#define DEBUG_CIRCULARBAR

#if defined(DEBUG_CIRCULARBAR)
#define DEBUG_D(format, ...) log_d(format, ##__VA_ARGS__)
#define DEBUG_E(format, ...) log_e(format, ##__VA_ARGS__)
#define DEBUG_W(format, ...) log_w(format, ##__VA_ARGS__)
#else
#define DEBUG_D(format, ...) 
#define DEBUG_E(format, ...) 
#define DEBUG_W(format, ...) 
#endif

/**
 * @brief Constructor for the CircularBar class.
 * @param _x X position of the circular bar.
 * @param _y Y position of the circular bar.
 * @param _screen Screen number.
 */
CircularBar::CircularBar(uint16_t _x, uint16_t _y, uint8_t _screen) : WidgetBase(_x, _y, _screen)
{
}

/**
 * @brief Destructor for the CircularBar class.
 */
CircularBar::~CircularBar()
{
}

/**
 * @brief Detects a touch on the circular bar.
 * @param _xTouch Pointer to the X-coordinate of the touch.
 * @param _yTouch Pointer to the Y-coordinate of the touch.
 * @return True if the touch is detected, false otherwise.
 */
bool CircularBar::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch)
{
    return false;
}

/**
 * @brief Gets the callback function for the circular bar.
 * @return Pointer to the callback function.
 */
functionCB_t CircularBar::getCallbackFunc()
{
    return cb;
}

/**
 * @brief Draws the background of the circular bar.
 */
void CircularBar::drawBackground()
{

    if (WidgetBase::currentScreen != screen || WidgetBase::usingKeyboard == true || !loaded)
    {
        return;
    }

    uint8_t offsetBorda = 1;
    WidgetBase::objTFT->fillArc(xPos, yPos, m_radius + offsetBorda, (m_radius - m_lineWeight - offsetBorda), m_startAngle, m_endAngle, m_bkColor);

    /*
    if(!m_invertedFill){
        //WidgetBase::objTFT->fillArc(xPos, yPos, m_radius + offsetBorda, (m_radius - m_lineWeight - offsetBorda), m_startAngle, m_endAngle, m_middleColor);
        WidgetBase::objTFT->fillArc(xPos, yPos, m_radius + offsetBorda, (m_radius - m_lineWeight - offsetBorda), m_startAngle, m_endAngle, m_bkColor);
    }else{
        WidgetBase::objTFT->fillArc(xPos, yPos, m_radius + offsetBorda, (m_radius - m_lineWeight - offsetBorda), m_endAngle, m_startAngle, m_lineColor);
    }*/
    
    //int xEnd = (cos(_startAngle * DEG_TO_RAD) * (_radius - _lineWeight / 2)) + xPos;
    //int yEnd = (sin(_startAngle * DEG_TO_RAD) * (_radius - _lineWeight / 2)) + yPos;
    //WidgetBase::objTFT->fillCircle(xEnd, yEnd, _lineWeight / 2, _middleColor);

    //xEnd = (cos(_endAngle * DEG_TO_RAD) * (_radius - _lineWeight / 2)) + xPos;
    //yEnd = (sin(_endAngle * DEG_TO_RAD) * (_radius - _lineWeight / 2)) + yPos;
    //WidgetBase::objTFT->fillCircle(xEnd, yEnd, _lineWeight / 2, _middleColor);
    m_lastValue = m_vmin;
    redraw();
}

/**
 * @brief Sets the value of the circular bar.
 * @param newValue New value to set.
 */
void CircularBar::setValue(int newValue)
{
    m_lastValue = m_value;
    m_value = newValue;
    m_update = true;
}

/**
 * @brief Redraws the circular bar.
 */
void CircularBar::redraw()
{

    if (WidgetBase::currentScreen != screen || WidgetBase::usingKeyboard == true || !m_update || !loaded)
    {
        return;
    }

    if (millis() - m_myTime < 50)
    {
        return;
    }

    m_update = false;

    
    int angleValue = map(m_value, m_vmin, m_vmax, m_startAngle, m_endAngle);
    int lastAngleValue = map(m_lastValue, m_vmin, m_vmax, m_startAngle, m_endAngle);


#if defined(DISP_DEFAULT)
    int xEnd = 0, xStart = 0, xCursor = 0, xLastCursor = 0;
    int yEnd = 0, yStart = 0, yCursor = 0, yLastCursor = 0;

    xStart = (cos(m_startAngle * DEG_TO_RAD) * (m_radius - m_lineWeight / 2)) + xPos;
    yStart = (sin(m_startAngle * DEG_TO_RAD) * (m_radius - m_lineWeight / 2)) + yPos;

    xEnd = (cos(m_endAngle * DEG_TO_RAD) * (m_radius - m_lineWeight / 2)) + xPos;
    yEnd = (sin(m_endAngle * DEG_TO_RAD) * (m_radius - m_lineWeight / 2)) + yPos;

    xCursor = (cos(angleValue * DEG_TO_RAD) * (m_radius - m_lineWeight / 2)) + xPos;
    yCursor = (sin(angleValue * DEG_TO_RAD) * (m_radius - m_lineWeight / 2)) + yPos;

    xLastCursor = (cos(lastAngleValue * DEG_TO_RAD) * (m_radius - m_lineWeight / 2)) + xPos;
    yLastCursor = (sin(lastAngleValue * DEG_TO_RAD) * (m_radius - m_lineWeight / 2)) + yPos;


    uint16_t lastCursoColor  = 0;

    

    // Se esta 'voltando' o angulo
    if (angleValue < lastAngleValue){
        DEBUG_D("Diminuindo -> x: %d, y: %d, r1: %d, r2: %d, start: %d, end: %d", xPos, yPos, m_radius, (m_radius - m_lineWeight), angleValue, lastAngleValue);
        //WidgetBase::objTFT->fillArc(xPos, yPos, m_radius, (m_radius - m_lineWeight), angleValue, lastAngleValue, m_middleColor);// Pinta a diferença
        WidgetBase::objTFT->fillArc(xPos, yPos, m_radius, (m_radius - m_lineWeight), angleValue, lastAngleValue, m_bkColor);// Pinta a diferença
        //lastCursoColor = m_middleColor;
        lastCursoColor = m_bkColor;
    }else if(angleValue > lastAngleValue){
        DEBUG_D("Aumentando -> x: %d, y: %d, r1: %d, r2: %d, start: %d, end: %d", xPos, yPos, m_radius, (m_radius - m_lineWeight), lastAngleValue, angleValue);
        WidgetBase::objTFT->fillArc(xPos, yPos, m_radius, (m_radius - m_lineWeight), lastAngleValue, angleValue, m_lineColor);// Pinta a diferença
        lastCursoColor = m_lineColor;
    }


    if(m_lineWeight >= 10){
        //WidgetBase::objTFT->fillCircle(xEnd, yEnd, m_lineWeight / 2 - 1, !m_invertedFill ? m_middleColor : m_lineColor);
        // WidgetBase::objTFT->fillCircle(xEnd, yEnd, m_lineWeight / 2 - 1, m_lineColor);
        //WidgetBase::objTFT->fillCircle(xLastCursor, yLastCursor, m_lineWeight / 2 - 1, lastCursoColor);
        //WidgetBase::objTFT->fillCircle(xLastCursor, yLastCursor, m_lineWeight / 2 - 1, lastCursoColor);

        // WidgetBase::objTFT->fillCircle(xStart, yStart, m_lineWeight / 2 - 1, m_lineColor);
        //WidgetBase::objTFT->fillCircle(xCursor, yCursor, m_lineWeight / 2 - 1, !m_invertedFill ? m_lineColor : m_middleColor);
    }

#endif

    

    if (m_showValue)
    {
        WidgetBase::objTFT->fillCircle(xPos, yPos, (m_radius - m_lineWeight) - 5, m_bkColor);

        char char_arr[100];
        sprintf(char_arr, "%d", m_value);

        // String str = String(_value);
        WidgetBase::objTFT->setTextColor(m_textColor);
        WidgetBase::objTFT->setFont(&Roboto_Bold10pt7b);
        // updateFont(FontType::BOLD);

#if defined(DISP_DEFAULT)
        printText(char_arr, xPos, yPos, MC_DATUM);
#endif

        updateFont(FontType::UNLOAD);
    }
}

/**
 * @brief Forces the circular bar to update.
 */
void CircularBar::forceUpdate()
{
    m_update = true;
}

/**
 * @brief Sets up the circular bar.         
 * @param radius Radius of the circular bar.
 * @param vmin Minimum value of the circular bar.
 * @param vmax Maximum value of the circular bar.
 * @param startAngle Starting angle of the circular bar.
 * @param endAngle Ending angle of the circular bar.
 * @param weight Weight of the circular bar.
 * @param color Color of the circular bar.
 * @param bkColor Background color of the circular bar.
 * @param textColor Text color of the circular bar.
 * @param showLabel True if the label is shown, false otherwise.
 * @param inverted True if the circular bar is inverted, false otherwise.
 */
void CircularBar::setup(uint16_t radius, int vmin, int vmax, uint16_t startAngle, uint16_t endAngle, uint8_t weight, uint16_t color, uint16_t bkColor, uint16_t textColor, bool showLabel, bool inverted)
{
    m_radius = radius;
    m_lineWeight = weight;
    m_lineColor = color;
    m_bkColor = bkColor;
    m_textColor = textColor;


    //m_middleColor = WidgetBase::blendColors(m_bkColor, m_lineColor, 0.25);

    m_startAngle = startAngle;
    m_endAngle = endAngle;

    //m_invertedFill = inverted;

    m_vmin = vmin;
    m_vmax = vmax;

    if (m_vmax < m_vmin)
    {
        int aux = m_vmin;
        m_vmin = m_vmax;
        m_vmax = aux;
    }

    //Se for preenchimento anti-horario, inverte os angulos
    /*if(m_invertedFill){
        int aux = m_startAngle;
        m_startAngle = m_endAngle;
        m_endAngle = aux;

        uint16_t auxColor = m_lineColor;
        //m_lineColor = m_middleColor;
        //m_middleColor = auxColor;
    }*/



    m_value = m_vmin;
    m_lastValue = m_vmin;

    if ((m_radius - m_lineWeight) < 20 || !m_showValue)
    {
        m_showValue = false;
    }

    m_update = true;
    loaded = true;
}

/**
 * @brief Configures the CircularBar with parameters defined in a configuration structure.
 * @param config Structure containing the CircularBar configuration parameters.
 */
void CircularBar::setup(const CircularBarConfig& config)
{
    setup(config.radius, config.minValue, config.maxValue, config.startAngle, config.endAngle, 
          config.thickness, config.color, config.backgroundColor, config.textColor, config.showValue, config.inverted);
}
