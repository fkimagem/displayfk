/**
 * @mainpage DisplayFK Library Documentation
 *
 * @brief A C++ library for creating graphical user interfaces on microcontrollers
 * based on the Arduino_GFX library.
 *
 * @details
 * This library provides a comprehensive framework for developing graphical user interfaces
 * on microcontrollers. It offers a wide range of widgets and controls that can be easily
 * integrated into your projects.
 *
 * ## Features
 * - Touch screen support
 * - Multiple widget types (buttons, sliders, gauges, etc.)
 * - SD card integration
 * - Logging system
 * - Event handling
 * - Screen management
 *
 * ## Getting Started
 * To use this library in your project:
 * 1. Include the necessary header files
 * 2. Initialize the DisplayFK object
 * 3. Configure your widgets
 * 4. Start the main loop
 *
 * ## Widget Types
 * The library supports various widget types:
 * - Buttons (Text, Circle, Rectangle)
 * - Sliders (Horizontal, Vertical)
 * - Gauges and Charts
 * - Text Input (TextBox, NumberBox)
 * - Indicators (LED, Label)
 * - And more...
 *
 * @note This library requires Arduino_GFX as a dependency
 * @see Arduino_GFX documentation for display driver requirements
 *
 * @version 1.0.0
 * @author Your Name
 * @copyright Copyright (c) 2024
 */

#ifndef DISPLAYFK_H
#define DISPLAYFK_H

//#define DEBUG_TOUCH
#define DEBUG_DISPLAY

#include "widgets/widgetsetup.h"
#include "../user_setup.h"
#include "check_touch.h"


#include <Arduino.h>
#include "SPIFFS.h"
#include <FS.h>
#include <Preferences.h>
#include <SPI.h>
#if defined(HAS_TOUCH)
#include "touch/touch.h"
#endif
//#include "soc/timer_group_struct.h" //watchdog
//#include "soc/timer_group_reg.h"    //watchdog
#include "freertos/queue.h"
#include "freertos/timers.h"
#if defined(PLATFORMIO)
#include "esp_task_wdt.h"
#elif defined(ARDUINO)
#include <esp_task_wdt.h>
#endif


#include "widgets/widgetbase.h"

#include "touch_widgets.h"
#include "output_widgets.h"

#include "freertos/queue.h"
#include <freertos/semphr.h>


/**
 * @brief Maximum length for a single log line
 */
#define MAX_LINE_LENGTH (64)

/**
 * @brief Length of the log queue buffer
 */
#define LOG_LENGTH (10)

/**
 * @brief Maximum size for the log file in bytes
 */
#define LOG_MAX_SIZE (1000)

/**
 * @brief Structure to hold a single log message
 */
typedef struct {
    char line[MAX_LINE_LENGTH];     ///< The actual log message text
    uint8_t line_length;            ///< Length of the message
} logMessage_t;

/// @brief Represents the main display framework class, managing various widget types, SD card functionality, and touch input.
class DisplayFK
{
private:
    static QueueHandle_t xFilaLog; ///< Queue handle for log data.
    TaskHandle_t m_hndTaskEventoTouch; ///< Handle for the touch event task.
    TimerHandle_t m_timer;
    uint32_t m_intervalMs;
    uint16_t m_xAutoClick = 0;
    uint16_t m_yAutoClick = 0;
    bool m_simulateAutoClick = false;
    static void timerCallback(TimerHandle_t xTimer);
    Preferences m_configs;             ///< Preferences for storing and accessing configuration data.
    char randomHexChar();
    static logMessage_t bufferLog[LOG_LENGTH]; ///< Array of log messages.
    static uint8_t logIndex; ///< Index of the next log message to be written.
    static uint16_t logFileCount; ///< Number of log files created
    bool m_debugTouch = false;
    int m_x0 = 0, m_y0 = 0, m_x1 = 0, m_y1 = 0;
    bool m_invertXAxis = false, m_invertYAxis = false, m_swapAxis = false;
    int m_widthScreen = 0;
    int m_heightScreen = 0;
    int m_rotationScreen = 0;
	uint16_t m_timeoutWTD = 0;
	bool m_enableWTD = false;
    bool m_watchdogInitialized = false;
    SemaphoreHandle_t m_loopSemaphore;

    


#if defined(DFK_TOUCHAREA)
    uint8_t qtdTouchArea = 0;      ///< Number of TouchArea widgets.
    TouchArea **arrayTouchArea = nullptr; ///< Array of TouchArea widgets.
    bool m_touchAreaConfigured = false; ///< Flag indicating if TouchArea is configured.
#endif

#ifdef DFK_CHECKBOX
    uint8_t qtdCheckbox = 0;      ///< Number of CheckBox widgets.
    CheckBox **arrayCheckbox = nullptr;  ///< Array of CheckBox widgets.
    bool m_checkboxConfigured = false; ///< Flag indicating if CheckBox is configured.
#endif

#ifdef DFK_CIRCLEBTN
    uint8_t qtdCircleBtn = 0;         ///< Number of CircleButton widgets.
    CircleButton **arrayCircleBtn = nullptr; ///< Array of CircleButton widgets.
    bool m_circleButtonConfigured = false; ///< Flag indicating if CircleButton is configured.
#endif

#ifdef DFK_GAUGE
    uint8_t qtdGauge = 0;       ///< Number of GaugeSuper widgets.
    GaugeSuper **arrayGauge = nullptr; ///< Array of GaugeSuper widgets.
    bool m_gaugeConfigured = false;  ///< Flag indicating if GaugeSuper is configured.
#endif

#ifdef DFK_CIRCULARBAR
    uint8_t qtdCircularBar = 0;        ///< Number of CircularBar widgets.
    CircularBar **arrayCircularBar = nullptr; ///< Array of CircularBar widgets.
    bool m_circularBarConfigured = false;   ///< Flag indicating if CircularBar is configured.
#endif

#ifdef DFK_HSLIDER
    uint8_t qtdHSlider = 0;      ///< Number of HSlider widgets.
    HSlider **arrayHSlider = nullptr;   ///< Array of HSlider widgets.
    bool m_hSliderConfigured = false; ///< Flag indicating if HSlider is configured.
#endif

#ifdef DFK_LABEL
    uint8_t qtdLabel = 0;      ///< Number of Label widgets.
    Label **arrayLabel = nullptr;     ///< Array of Label widgets.
    bool m_labelConfigured = false; ///< Flag indicating if Label is configured.
#endif

#ifdef DFK_LED
    uint8_t qtdLed = 0;      ///< Number of Led widgets.
    Led **arrayLed = nullptr;       ///< Array of Led widgets.
    bool m_ledConfigured = false; ///< Flag indicating if Led is configured.
#endif

#ifdef DFK_LINECHART
    uint8_t qtdLineChart = 0;      ///< Number of LineChart widgets.
    LineChart **arrayLineChart = nullptr; ///< Array of LineChart widgets.
    bool m_lineChartConfigured = false; ///< Flag indicating if LineChart is configured.
#endif

#ifdef DFK_RADIO
    uint8_t qtdRadioGroup = 0;       ///< Number of RadioGroup widgets.
    RadioGroup **arrayRadioGroup = nullptr; ///< Array of RadioGroup widgets.
    bool m_radioGroupConfigured = false;  ///< Flag indicating if RadioGroup is configured.
#endif

#ifdef DFK_RECTBTN
    uint8_t qtdRectBtn = 0;         ///< Number of RectButton widgets.
    RectButton **arrayRectBtn = nullptr;   ///< Array of RectButton widgets.
    bool m_rectButtonConfigured = false; ///< Flag indicating if RectButton is configured.
#endif

#ifdef DFK_TOGGLE
    uint8_t qtdToggle = 0;            ///< Number of ToggleButton widgets.
    ToggleButton **arrayToggleBtn = nullptr; ///< Array of ToggleButton widgets.
    bool m_toggleConfigured = false;       ///< Flag indicating if ToggleButton is configured.
#endif

#ifdef DFK_VBAR
    uint8_t qtdVBar = 0;      ///< Number of VBar widgets.
    VBar **arrayVBar = nullptr;      ///< Array of VBar widgets.
    bool m_vBarConfigured = false; ///< Flag indicating if VBar is configured.
#endif

#ifdef DFK_VANALOG
    uint8_t qtdVAnalog = 0;      ///< Number of VAnalog widgets.
    VAnalog **arrayVAnalog = nullptr;   ///< Array of VAnalog widgets.
    bool m_vAnalogConfigured = false; ///< Flag indicating if VAnalog is configured.
#endif

#ifdef DFK_TEXTBOX
    uint8_t qtdTextBox = 0;      ///< Number of TextBox widgets.
    TextBox **arrayTextBox = nullptr;   ///< Array of TextBox widgets.
    bool m_textboxConfigured = false; ///< Flag indicating if TextBox is configured.
    WKeyboard *keyboard = nullptr;     ///< Pointer to the WKeyboard instance for text input.
    //WKeyboard m_pKeyboard;            ///< Internal keyboard instance for TextBox.
#endif

#ifdef DFK_NUMBERBOX
    uint8_t qtdNumberBox = 0;      ///< Number of NumberBox widgets.
    NumberBox **arrayNumberbox = nullptr; ///< Array of NumberBox widgets.
    bool m_numberboxConfigured = false; ///< Flag indicating if NumberBox is configured.
    Numpad *numpad = nullptr;            ///< Pointer to the Numpad instance for number input.
    //Numpad m_pNumpad;                   ///< Internal numpad instance for NumberBox.
#endif

#ifdef DFK_IMAGE
    uint8_t qtdImage = 0;      ///< Number of Image widgets.
    Image **arrayImage = nullptr;     ///< Array of Image widgets.
    bool m_imageConfigured = false; ///< Flag indicating if Image is configured.
#endif

#ifdef DFK_TEXTBUTTON
    uint8_t qtdTextButton = 0;       ///< Number of TextButton widgets.
    TextButton **arrayTextButton = nullptr; ///< Array of TextButton widgets.
    bool m_textButtonConfigured = false;  ///< Flag indicating if TextButton is configured.
#endif

#ifdef DFK_SD
    SPIClass m_spiSD;                ///< SPI instance for SD card communication.
    const char *m_nameLogFile;       ///< Name of the log file.
#endif

#ifdef DFK_SPINBOX
    uint8_t qtdSpinbox = 0;      ///< Number of SpinBox widgets.
    SpinBox **arraySpinbox = nullptr;   ///< Array of SpinBox widgets.
    bool m_spinboxConfigured = false; ///< Flag indicating if SpinBox is configured.
#endif

#ifdef DFK_THERMOMETER
    uint8_t qtdThermometer = 0;      ///< Number of Thermometer widgets.
    Thermometer **arrayThermometer = nullptr;   ///< Array of Thermometer widgets.
    bool m_thermometerConfigured = false; ///< Flag indicating if Thermometer is configured.
#endif

#ifdef DFK_EXTERNALINPUT
    static uint8_t qtdExternalInput;      ///< Number of ExternalInput widgets.
    static InputExternal *arrayInputExternal;   ///< Array of ExternalInput widgets.
    bool m_inputExternalConfigured = false; ///< Flag indicating if ExternalInput is configured.
    static ExternalKeyboard *externalKeyboard;            ///< Pointer to the Numpad instance for number input.
    ExternalKeyboard m_pExternalKeyboard;                   ///< Internal numpad instance for NumberBox.
#endif

    // Helper functions for loopTask
    void processLogQueue();
    void processLogBuffer();
    void processTouchEvent(uint16_t xTouch, uint16_t yTouch, int zPressure, uint8_t gesture);
    void processTouchableWidgets(uint16_t xTouch, uint16_t yTouch);
    void updateWidgets();
    void processCallback();

    // Touch processing functions
    void processCheckboxTouch(uint16_t xTouch, uint16_t yTouch);
    void processCircleButtonTouch(uint16_t xTouch, uint16_t yTouch);
    void processHSliderTouch(uint16_t xTouch, uint16_t yTouch);
    void processRadioGroupTouch(uint16_t xTouch, uint16_t yTouch);
    void processRectButtonTouch(uint16_t xTouch, uint16_t yTouch);
    void processImageTouch(uint16_t xTouch, uint16_t yTouch);
    void processSpinboxTouch(uint16_t xTouch, uint16_t yTouch);
    void processToggleTouch(uint16_t xTouch, uint16_t yTouch);
    void processTextButtonTouch(uint16_t xTouch, uint16_t yTouch);
    void processTextBoxTouch(uint16_t xTouch, uint16_t yTouch);
    void processNumberBoxTouch(uint16_t xTouch, uint16_t yTouch);
    void processEmptyAreaTouch(uint16_t xTouch, uint16_t yTouch);

    // Widget update functions
    void updateCircularBar();
    void updateGauge();
    void updateLabel();
    void updateLed();
    void updateLineChart();
    void updateVBar();
    void updateVAnalog();
    void updateCheckbox();
    void updateCircleButton();
    void updateHSlider();
    void updateRadioGroup();
    void updateRectButton();
    void updateToggle();
    void updateImage();
    void updateTextButton();
    void updateSpinbox();
    void updateTextBox();
    void updateNumberBox();
    void updateThermometer();

public:
    static DisplayFK *instance;
    static bool sdcardOK; ///< Indicates if the SD card is successfully initialized.
    static void TaskEventoTouch(void *pvParamenters);

    bool m_runningTransaction;

    DisplayFK();
    ~DisplayFK();

    void setup();
    void startKeyboards();

    void startTransaction();
    void finishTransaction();
    void drawWidgetsOnScreen(const uint8_t currentScreenIndex);
    #if defined(USING_GRAPHIC_LIB)
    void setFontNormal(const GFXfont *_font);
    void setFontBold(const GFXfont *_font);
    void printText(const char* _texto, uint16_t _x, uint16_t _y, uint8_t _datum, uint16_t _colorText, uint16_t _colorPadding, const GFXfont* _font);
    #endif
    void createTask(bool enableWatchdog, uint16_t timeout_s = 3);
    void drawPng(uint16_t _x, uint16_t _y, const uint16_t _colors[], const uint8_t _mask[], uint16_t _w, uint16_t _h);
    void enableTouchLog();
    void disableTouchLog();
    void addLog(const char *data);
    void loopTask();
    TaskHandle_t getTaskHandle();
    void setupAutoClick(uint32_t intervalMs, uint16_t x, uint16_t y);
    void startAutoClick();
    void stopAutoClick();
    bool isRunningAutoClick();
    void blockLoopTask();
    void freeLoopTask();

#if defined(HAS_TOUCH)
#if defined(TOUCH_XPT2046)
    void checkCalibration();
    void recalibrate();
#endif

    void startTouch(uint16_t w, uint16_t h, uint8_t _rotation, SPIClass *_sharedSPI);
    void setTouchCorners(int x0, int x1, int y0, int y1);
    void setInvertAxis(bool invert_x, bool invert_y);
    void setSwapAxis(bool swap);

    #if defined(TOUCH_XPT2046)
    void startTouchXPT2046(uint16_t w, uint16_t h, uint8_t _rotation, int8_t pinCS, SPIClass *_sharedSPI, Arduino_GFX *_objTFT, int touchFrequency, int displayFrequency, int displayPinCS);
    void startTouchXPT2046(uint16_t w, uint16_t h, uint8_t _rotation, int8_t pinSclk, int8_t pinMosi, int8_t pinMiso, int8_t pinCS, Arduino_GFX *_objTFT, int touchFrequency, int displayFrequency, int displayPinCS);
    #elif defined(TOUCH_FT6236U)
    void startTouchFT6236U(uint16_t w, uint16_t h, uint8_t _rotation, int8_t pinSDA, int8_t pinSCL, uint8_t pinINT, int8_t pinRST);
    #elif defined(TOUCH_FT6336)
    void startTouchFT6336(uint16_t w, uint16_t h, uint8_t _rotation, int8_t pinSDA, int8_t pinSCL, int8_t pinINT, int8_t pinRST);
    #elif defined(TOUCH_CST816)
    void startTouchCST816(uint16_t w, uint16_t h, uint8_t _rotation, uintint8_t8_t pinSDA, int8_t pinSCL, int8_t pinINT, int8_t pinRST);
    #elif defined(TOUCH_GT911)
    void startTouchGT911(uint16_t w, uint16_t h, uint8_t _rotation, int8_t pinSDA, int8_t pinSCL, int8_t pinINT, int8_t pinRST);
    #endif
    TouchScreen *touchExterno = nullptr; ///< External touch screen object.
#endif

    void changeWTD();

#ifdef DFK_CHECKBOX
    void setCheckbox(CheckBox *array[], uint8_t amount);
#endif

#ifdef DFK_CIRCLEBTN
    void setCircleButton(CircleButton *array[], uint8_t amount);
#endif

#ifdef DFK_GAUGE
    void setGauge(GaugeSuper *array[], uint8_t amount);
#endif

#ifdef DFK_CIRCULARBAR
    void setCircularBar(CircularBar *array[], uint8_t amount);
#endif

#ifdef DFK_HSLIDER
    void setHSlider(HSlider *array[], uint8_t amount);
#endif

#ifdef DFK_LABEL
    void setLabel(Label *array[], uint8_t amount);
#endif

#ifdef DFK_LED
    void setLed(Led *array[], uint8_t amount);
#endif

#ifdef DFK_LINECHART
    void setLineChart(LineChart *array[], uint8_t amount);
#endif

#ifdef DFK_NUMBERBOX
    void setNumberbox(NumberBox *array[], uint8_t amount);
    void setupNumpad();
    void insertCharNumpad(char c);
#endif

#ifdef DFK_RADIO
    void setRadioGroup(RadioGroup *array[], uint8_t amount);
#endif

#ifdef DFK_RECTBTN
    void setRectButton(RectButton *array[], uint8_t amount);
#endif

#ifdef DFK_TEXTBOX
    void setTextbox(TextBox *array[], uint8_t amount);
    void setupKeyboard();
    void insertCharTextbox(char c);
#endif

#ifdef DFK_TOGGLE
    void setToggle(ToggleButton *array[], uint8_t amount);
#endif

#if defined(DFK_TOUCHAREA)
    void setTouchArea(TouchArea *array[], uint8_t amount);
#endif

#ifdef DFK_VANALOG
    void setVAnalog(VAnalog *array[], uint8_t amount);
#endif

#ifdef DFK_VBAR
    void setVBar(VBar *array[], uint8_t amount);
#endif

#ifdef DFK_IMAGE
    void setImage(Image *array[], uint8_t amount);
#endif

#ifdef DFK_SPINBOX
    void setSpinbox(SpinBox *array[], uint8_t amount);
#endif

#if defined(DFK_TEXTBUTTON)
    void setTextButton(TextButton *array[], uint8_t amount);
#endif

#ifdef DFK_THERMOMETER
    void setThermometer(Thermometer *array[], uint8_t amount);
#endif

#ifdef DFK_SD
    bool startSD(uint8_t pinCS, SPIClass *spiShared);
    bool startSD(uint8_t pinCS, SPIClass *spiShared, int hz);
    void listFiles(fs::FS *fs, const char *dirname, uint8_t levels);
    void createDir(fs::FS *fs, const char *path);
    void readFile(fs::FS *fs, const char *path);
    void appendFile(fs::FS *fs, const char *path, const char *message);
    void appendLog(fs::FS *fs, const char *path, const logMessage_t* lines, uint8_t amount, bool createNewFile);
    void writeFile(fs::FS *fs, const char * path, const char * message);
    const char *getLogFileName();
    const char *generateNameFile();
#endif

    
};

#endif