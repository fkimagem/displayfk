#include "displayfk.h"



#if defined(DEBUG_DISPLAY)
#define DEBUG_D(format, ...) log_d(format, ##__VA_ARGS__)
#define DEBUG_E(format, ...) log_e(format, ##__VA_ARGS__)
#define DEBUG_W(format, ...) log_w(format, ##__VA_ARGS__)
#else
#define DEBUG_D(format, ...) 
#define DEBUG_E(format, ...) 
#define DEBUG_W(format, ...) 
#endif


DisplayFK *DisplayFK::instance = nullptr;
bool DisplayFK::sdcardOK = false;
QueueHandle_t DisplayFK::xFilaLog;
logMessage_t DisplayFK::bufferLog[LOG_LENGTH];
uint8_t DisplayFK::logIndex = 0;
uint16_t DisplayFK::logFileCount = 1;


#ifdef DFK_SD

#endif


/**
 * @brief Enables or disables the watchdog timer
 * @param enable true to enable, false to disable
 */
void DisplayFK::changeWTD(bool enable)
{
    if (enable)
    {
        Serial.println("Enabling watchdog");
        uint8_t timeout_seconds = 5;
        esp_err_t ret = esp_task_wdt_init(timeout_seconds, true);

        if (ret == ESP_OK)
        {
            Serial.printf("Successful to enable watchdog with %i seconds\n", timeout_seconds);
        }
        else
        {
            Serial.printf("Error to turn on wtd with %i seconds\n", timeout_seconds);
        }
    }
    else
    {
        Serial.println("Disabling watchdog");
        esp_task_wdt_deinit();
        esp_err_t ret = esp_task_wdt_delete(m_hndTaskEventoTouch);
        if (ret == ESP_OK)
        {
            Serial.println("Successful to disable watchdog");
        }
        else
        {
            Serial.println("Error to disable watchdog");
        }
    }
}

#ifdef DFK_SD

/**
 * @brief Generates a unique filename for logs
 * @return Pointer to the generated filename
 */
const char* DisplayFK::generateNameFile() {
    // Pre-calculate the total length needed: "/" + number + "-" + 32 chars + ".txt\0"
    const size_t numberLength = 10; // Max length for uint16_t
    const size_t totalLength = 1 + numberLength + 1 + 32 + 5; // "/" + number + "-" + random + ".txt\0"
    
    // Allocate memory for the filename
    char* filename = new char[totalLength];
    if (!filename) {
        DEBUG_E("Failed to allocate memory for filename");
        return nullptr;
    }

    // Generate the random string first
    char randomString[33]; // 32 chars + null terminator
    for (int i = 0; i < 32; ++i) {
        randomString[i] = randomHexChar();
    }
    randomString[32] = '\0';

    // Format the filename using snprintf for safety
    int written = snprintf(filename, totalLength, "/%u-%s.txt", logFileCount, randomString);
    if (written < 0 || written >= totalLength) {
        DEBUG_E("Failed to format filename");
        delete[] filename;
        return nullptr;
    }

    DEBUG_D("Generated filename: %s", filename);
    logFileCount++;
    
    return filename;
}

/**
 * @brief Returns the current log filename
 * @return Current log filename
 */
const char *DisplayFK::getLogFileName()
{
    return m_nameLogFile;
}

#endif

#ifdef DFK_CHECKBOX

/**
 * @brief Configures the checkbox array
 * @param array Pointer to the checkbox array
 * @param amount Number of checkboxes in the array
 */
void DisplayFK::setCheckbox(CheckBox *array[], uint8_t amount)
{
    if (m_checkboxConfigured)
    {
        DEBUG_W("Checkbox already configured");
        return;
    }
    m_checkboxConfigured = (amount > 0 && array != nullptr);
    arrayCheckbox = array;
    qtdCheckbox = amount;
}

#endif

#ifdef DFK_CIRCLEBTN

/**
 * @brief Configures the circle button array
 * @param array Pointer to the circle button array
 * @param amount Number of buttons in the array
 */
void DisplayFK::setCircleButton(CircleButton *array[], uint8_t amount)
{
    if (m_circleButtonConfigured)
    {
        DEBUG_W("CircleButton already conffigured");
        return;
    }
    m_circleButtonConfigured = (amount > 0 && array != nullptr);
    arrayCircleBtn = array;
    qtdCircleBtn = amount;
}

#endif

#ifdef DFK_GAUGE

/**
 * @brief Configures the gauge array
 * @param array Pointer to the gauge array
 * @param amount Number of gauges in the array
 */
void DisplayFK::setGauge(GaugeSuper *array[], uint8_t amount)
{
    if (m_gaugeConfigured)
    {
        DEBUG_W("Gauge already conffigured");
        return;
    }
    m_gaugeConfigured = (amount > 0 && array != nullptr);
    arrayGauge = array;
    qtdGauge = amount;
}

#endif

#ifdef DFK_CIRCULARBAR

/**
 * @brief Configures the circular bar array
 * @param array Pointer to the circular bar array
 * @param amount Number of bars in the array
 */
void DisplayFK::setCircularBar(CircularBar *array[], uint8_t amount)
{
    if (m_circularBarConfigured)
    {
        DEBUG_W("CircularBar already configured");
        return;
    }
    m_circularBarConfigured = (amount > 0 && array != nullptr);
    arrayCircularBar = array;
    qtdCircularBar = amount;
}

#endif

#ifdef DFK_HSLIDER

/**
 * @brief Configures the horizontal slider array
 * @param array Pointer to the slider array
 * @param amount Number of sliders in the array
 */
void DisplayFK::setHSlider(HSlider *array[], uint8_t amount)
{
    if (m_hSliderConfigured)
    {
        DEBUG_W("HSlider already conffigured");
        return;
    }
    m_hSliderConfigured = (amount > 0 && array != nullptr);
    arrayHSlider = array;
    qtdHSlider = amount;
}

#endif

#ifdef DFK_LABEL

/**
 * @brief Configures the label array
 * @param array Pointer to the label array
 * @param amount Number of labels in the array
 */
void DisplayFK::setLabel(Label *array[], uint8_t amount)
{
    if (m_labelConfigured)
    {
        DEBUG_W("Label already conffigured");
        return;
    }
    m_labelConfigured = (amount > 0 && array != nullptr);
    arrayLabel = array;
    qtdLabel = amount;
}
#endif

#ifdef DFK_LED

/**
 * @brief Configures the LED array
 * @param array Pointer to the LED array
 * @param amount Number of LEDs in the array
 */
void DisplayFK::setLed(Led *array[], uint8_t amount)
{
    if (m_ledConfigured)
    {
        DEBUG_W("Led already conffigured");
        return;
    }
    m_ledConfigured = (amount > 0 && array != nullptr);
    arrayLed = array;
    qtdLed = amount;
}

#endif

#ifdef DFK_LINECHART

/**
 * @brief Configures the line chart array
 * @param array Pointer to the line chart array
 * @param amount Number of charts in the array
 */
void DisplayFK::setLineChart(LineChart *array[], uint8_t amount)
{
    if (m_lineChartConfigured)
    {
        DEBUG_W("LineChart already conffigured");
        return;
    }
    m_lineChartConfigured = (amount > 0 && array != nullptr);
    arrayLineChart = array;
    qtdLineChart = amount;
}

#endif

#ifdef DFK_NUMBERBOX
/**
 * @brief Configures the number box array
 * @param array Pointer to the number box array
 * @param amount Number of boxes in the array
 */
void DisplayFK::setNumberbox(NumberBox *array[], uint8_t amount)
{
    if (m_numberboxConfigured)
    {
        DEBUG_W("Numberbox already conffigured");
        return;
    }
    m_numberboxConfigured = (amount > 0 && array != nullptr);
    arrayNumberbox = array;
    qtdNumberBox = amount;

    setupNumpad();
}

/**
 * @brief Sets up the numeric keypad
 */
void DisplayFK::setupNumpad()
{
    //m_pNumpad.setup();
    //DisplayFK::numpad = &m_pNumpad;
}

/**
 * @brief Inserts a character into the numeric keypad
 * @param c Character to be inserted
 */
void DisplayFK::insertCharNumpad(char c)
{
    if(DisplayFK::numpad){
        DisplayFK::numpad->insertChar(c);
    }else{
        DEBUG_E("Numpad not configured");
    }
    
}
#endif

#ifdef DFK_RADIO

/**
 * @brief Configures the radio button group array
 * @param array Pointer to the radio group array
 * @param amount Number of groups in the array
 */
void DisplayFK::setRadioGroup(RadioGroup *array[], uint8_t amount)
{
    if (m_radioGroupConfigured)
    {
        DEBUG_W("RadioGroup already conffigured");
        return;
    }
    m_radioGroupConfigured = (amount > 0 && array != nullptr);
    arrayRadioGroup = array;
    qtdRadioGroup = amount;
}
#endif

#ifdef DFK_RECTBTN

/**
 * @brief Configures the rectangular button array
 * @param array Pointer to the button array
 * @param amount Number of buttons in the array
 */
void DisplayFK::setRectButton(RectButton *array[], uint8_t amount)
{
    if (m_rectButtonConfigured)
    {
        DEBUG_W("RectButton already conffigured");
        return;
    }
    m_rectButtonConfigured = (amount > 0 && array != nullptr);
    arrayRectBtn = array;
    qtdRectBtn = amount;
}
#endif

#ifdef DFK_TEXTBUTTON

/**
 * @brief Configures the text button array
 * @param array Pointer to the button array
 * @param amount Number of buttons in the array
 */
void DisplayFK::setTextButton(TextButton *array[], uint8_t amount)
{
    if (m_textButtonConfigured)
    {
        DEBUG_W("TextButton already conffigured");
        return;
    }
    m_textButtonConfigured = (amount > 0 && array != nullptr);
    arrayTextButton = array;
    qtdTextButton = amount;
}
#endif

#ifdef DFK_SPINBOX

/**
 * @brief Configures the spin box array
 * @param array Pointer to the spin box array
 * @param amount Number of boxes in the array
 */
void DisplayFK::setSpinbox(SpinBox *array[], uint8_t amount)
{
    if (m_spinboxConfigured)
    {
        DEBUG_W("SpinBox already conffigured");
        return;
    }
    m_spinboxConfigured = (amount > 0 && array != nullptr);
    arraySpinbox = array;
    qtdSpinbox = amount;
}
#endif

#ifdef DFK_TEXTBOX

/**
 * @brief Configures the text box array
 * @param array Pointer to the text box array
 * @param amount Number of boxes in the array
 */
void DisplayFK::setTextbox(TextBox *array[], uint8_t amount)
{
    if (m_textboxConfigured)
    {
        DEBUG_W("Textbox already conffigured");
        return;
    }
    m_textboxConfigured = (amount > 0 && array != nullptr);
    arrayTextBox = array;
    qtdTextBox = amount;

    setupKeyboard();
}

/**
 * @brief Sets up the virtual keyboard
 */
void DisplayFK::setupKeyboard()
{
    //m_pKeyboard.setup();
    //DisplayFK::keyboard = &m_pKeyboard;
}

/**
 * @brief Inserts a character into the text box
 * @param c Character to be inserted
 */
void DisplayFK::insertCharTextbox(char c)
{
    if(DisplayFK::keyboard){
        keyboard->insertChar(c);
    }else{
        DEBUG_E("Keyboard not configured");
    }
    
}
#endif

#ifdef DFK_TOGGLE

/**
 * @brief Configures the toggle button array
 * @param array Pointer to the button array
 * @param amount Number of buttons in the array
 */
void DisplayFK::setToggle(ToggleButton *array[], uint8_t amount)
{
    if (m_toggleConfigured)
    {
        DEBUG_W("Toggle already conffigured");
        return;
    }
    m_toggleConfigured = (amount > 0 && array != nullptr);
    arrayToggleBtn = array;
    qtdToggle = amount;
}
#endif

#if defined(DFK_TOUCHAREA)

/**
 * @brief Configures the touch area array
 * @param array Pointer to the touch area array
 * @param amount Number of areas in the array
 */
void DisplayFK::setTouchArea(TouchArea *array[], uint8_t amount)
{
    if (m_touchAreaConfigured)
    {
        DEBUG_W("TouchArea already conffigured");
        return;
    }
    m_touchAreaConfigured = (amount > 0 && array != nullptr);
    arrayTouchArea = array;
    qtdTouchArea = amount;
}
#endif

#ifdef DFK_VANALOG

/**
 * @brief Configures the analog viewer array
 * @param array Pointer to the viewer array
 * @param amount Number of viewers in the array
 */
void DisplayFK::setVAnalog(VAnalog *array[], uint8_t amount)
{
    if (m_vAnalogConfigured)
    {
        DEBUG_W("VAnalog already conffigured");
        return;
    }
    m_vAnalogConfigured = (amount > 0 && array != nullptr);
    arrayVAnalog = array;
    qtdVAnalog = amount;
}
#endif

#ifdef DFK_VBAR

/**
 * @brief Configures the vertical bar array
 * @param array Pointer to the bar array
 * @param amount Number of bars in the array
 */
void DisplayFK::setVBar(VBar *array[], uint8_t amount)
{
    if (m_vBarConfigured)
    {
        DEBUG_W("VBar already conffigured");
        return;
    }
    m_vBarConfigured = (amount > 0 && array != nullptr);
    arrayVBar = array;
    qtdVBar = amount;
}
#endif

#ifdef DFK_IMAGE

/**
 * @brief Configures the image array
 * @param array Pointer to the image array
 * @param amount Number of images in the array
 */
void DisplayFK::setImage(Image *array[], uint8_t amount)
{
    if (m_imageConfigured)
    {
        DEBUG_W("Image already configured");
        return;
    }
    m_imageConfigured = (amount > 0 && array != nullptr);
    arrayImage = array;
    qtdImage = amount;
    DEBUG_D("Configuring image: %i", amount);
}
#endif

#ifdef DFK_SD

/**
 * @brief Initializes the SD card with default settings
 * @param pinCS CS pin of the SD card
 * @param spiShared Pointer to the shared SPI interface
 * @return true if initialization was successful, false otherwise
 */
bool DisplayFK::startSD(uint8_t pinCS, SPIClass *spiShared)
{
    return startSD(pinCS, spiShared, 0);
}

/**
 * @brief Initializes the SD card with custom settings
 * @param pinCS CS pin of the SD card
 * @param spiShared Pointer to the shared SPI interface
 * @param hz SPI interface frequency in Hz
 * @return true if initialization was successful, false otherwise
 */
bool DisplayFK::startSD(uint8_t pinCS, SPIClass *spiShared, int hz)
{
    if(!spiShared){
        DEBUG_E("SPI for SD not configured");
        return false;
    }
    if(hz <= 0){
        hz = 4000000;
        DEBUG_D("Setting Hz to default value: %i", hz);
    }

    DEBUG_D("Starting setup SD: CS: %i, velocidade: %i", pinCS, hz);

    m_spiSD = *spiShared;
    m_spiSD.setFrequency(hz);
    if (!SD.begin(pinCS, m_spiSD, hz , "/sd", 5, false))
    {
        DEBUG_E("Card Mount Failed");
        return false;
    }

    WidgetBase::mySD = &SD;

    uint8_t cardType = WidgetBase::mySD->cardType();

    if (cardType == CARD_NONE)
    {
        DEBUG_W("No SD card attached");
        SD.end();
        return false;
    }

    if (cardType == CARD_MMC)
    {
        DEBUG_D("SD Card Type: MMC");
    }
    else if (cardType == CARD_SD)
    {
        DEBUG_D("SD Card Type: SDSC");
    }
    else if (cardType == CARD_SDHC)
    {
        DEBUG_D("SD Card Type: SDHC");
    }
    else
    {
        DEBUG_D("SD Card Type: UNKNOWN");
    }

    uint64_t cardSize = WidgetBase::mySD->cardSize() / (1024 * 1024);
    DEBUG_D("SD Card Size: %lluMB\n", cardSize);

    DEBUG_D("SD Done");
    DisplayFK::sdcardOK = true;

    m_nameLogFile = generateNameFile();

    writeFile(WidgetBase::mySD, m_nameLogFile, "Start Log");

    listFiles(WidgetBase::mySD, "/", 1);
    SD.end();
    return true;
}

/**
 * @brief Lists files in a directory
 * @param fs Pointer to the file system
 * @param dirname Directory name
 * @param levels Listing depth level
 */
void DisplayFK::listFiles(fs::FS *fs, const char *dirname, uint8_t levels)
{
    if (!DisplayFK::sdcardOK)
    {
        DEBUG_E("SD not configured");
        return;
    }
    if(dirname[0] != '/'){
        DEBUG_E("Path must start with /");
    }
    DEBUG_D("Listing directory: %s\n", dirname);

    File root = fs->open(dirname);
    if (!root)
    {
        DEBUG_E("Failed to open directory");
        return;
    }
    if (!root.isDirectory())
    {
        DEBUG_E("Not a directory");
        return;
    }

    File file = root.openNextFile();
    uint16_t indiceFile = 1;
    while (file)
    {
        if (file.isDirectory())
        {
            DEBUG_D("\tDIR: %s\n", file.name());
            //Serial.println(file.name());
            if (levels)
            {
                const char *dirbase = "/";
                size_t len1 = strlen(dirbase);
                size_t len2 = strlen(file.name());

                char* resultado = (char*) malloc(len1 + len2 + 1); 
                if (!resultado) continue;  // falha na alocação

                strcpy(resultado, dirbase);  // copia str1
                strcat(resultado, file.name());  // concatena str2

                //String path = "/" + String(file.name());
                listFiles(fs, resultado, levels - 1);
                free(resultado);
            }
        }
        else
        {
            DEBUG_D("\t%i\tFILE: %s\tSIZE: %d\n", indiceFile, file.name(), file.size());
            indiceFile++;
            //Serial.print(file.name());
            //Serial.print("  SIZE: ");
            //Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

/**
 * @brief Appends content to the end of a file
 * @param fs Pointer to the file system
 * @param path File path
 * @param message Message to be appended
 */
void DisplayFK::appendFile(fs::FS *fs, const char *path, const char *message)
{
    if (!DisplayFK::sdcardOK)
    {
        DEBUG_E("SD not configured");
        return;
    }

    auto len = strlen(message);

    DEBUG_D("Appending %d bytes to file: %s", len, path);

    File file = fs->open(path, FILE_APPEND);
    if (!file)
    {
        DEBUG_E("Failed to open file for appending");
        if(SD.totalBytes() == 0){
            DEBUG_E("SD card empty");
            DisplayFK::sdcardOK = false;
        }
        return;
    }

    if (file.size() > LOG_MAX_SIZE)
    {
        DEBUG_E("Log too large");
        file.close();
        return;
    }

    if (len <= 1)
    {
        DEBUG_W("Empty message isnt appended");
        file.close();
        return;
    }

    if (file.println(message))
    {
        DEBUG_D("Message appended");
    }
    else
    {
        DEBUG_E("Append failed");
    }
    file.close();
}

/**
 * @brief Reads the content of a file
 * @param fs Pointer to the file system
 * @param path File path
 */
void DisplayFK::readFile(fs::FS *fs, const char *path)
{
    if (!DisplayFK::sdcardOK)
    {
        DEBUG_E("SD not configured");
        return;
    }
    DEBUG_D("Reading file: %s\n", path);

    File file = fs->open(path);
    if (!file)
    {
        DEBUG_E("Failed to open file for reading");
        return;
    }

    DEBUG_D("Read from file: ");
    while (file.available())
    {
        Serial.write(file.read());
    }
    file.close();
}

/**
 * @brief Creates a directory
 * @param fs Pointer to the file system
 * @param path Directory path
 */
void DisplayFK::createDir(fs::FS *fs, const char *path)
{
    if (!DisplayFK::sdcardOK)
    {
        DEBUG_E("SD not configured");
        return;
    }
    DEBUG_D("Creating Dir: %s\n", path);
    if (fs->mkdir(path))
    {
        DEBUG_D("Dir created");
    }
    else
    {
        DEBUG_E("mkdir failed");
    }
}

/**
 * @brief Writes content to a file
 * @param fs Pointer to the file system
 * @param path File path
 * @param message Message to be written
 */
void DisplayFK::writeFile(fs::FS *fs, const char * path, const char * message){
    if (!DisplayFK::sdcardOK)
    {
        DEBUG_E("SD not configured");
        return;
    }

    DEBUG_D("Writing file: %s\n", path);

    File file = fs->open(path, FILE_WRITE);
  if(!file){
    DEBUG_E("Failed to open file for writing");
    return;
  }
  if(file.print(message)){
    DEBUG_D("File written");
  } else {
    DEBUG_E("Write failed");
  }
  file.close();
}

/**
 * @brief Appends logs to the log file
 * @param fs Pointer to the file system
 * @param path Log file path
 * @param lines Array of log messages
 * @param amount Number of messages
 * @param createNewFile true to create new file if needed
 */
void DisplayFK::appendLog(fs::FS *fs, const char *path, const logMessage_t* lines, uint8_t amount, bool createNewFile){
if (!DisplayFK::sdcardOK)
    {
        DEBUG_E("SD not configured");
        return;
    }
    if(amount == 0){
        DEBUG_D("No lines to append");
        return;
    }

    File file = fs->open(path, FILE_APPEND);
    if (!file)
    {
        DEBUG_E("Failed to open file for appending");
        if(SD.totalBytes() == 0){
            DEBUG_E("SD card empty");
            DisplayFK::sdcardOK = false;
        }
        return;
    }

    if (file.size() > LOG_MAX_SIZE)
    {
        DEBUG_E("Log too large");
        file.close();

        if(createNewFile){
            DEBUG_D("Creating new one");
            m_nameLogFile = generateNameFile();
            writeFile(WidgetBase::mySD, m_nameLogFile, "Start Log");
            file = fs->open(getLogFileName(), FILE_APPEND);
            if (!file){
                DEBUG_E("Failed to open new file created for appending");
                return;
            }
        }else{
            return;
        }
        
    }

    uint8_t success = 0;
    DEBUG_D("Starting write log with %d lines", amount);
    for(int i = 0; i < amount; i++){
        if (file.println(lines[i].line))
        {
            DEBUG_D("Message appended");
            success++;
        }
        else
        {
            DEBUG_E("Append failed");
        }
    }
    file.close();
    DEBUG_D("%d/%d lines appended", success, amount);
}
#endif

/**
 * @brief Generates a random hexadecimal character
 * @return Random hexadecimal character
 */
char DisplayFK::randomHexChar()
{
    const char hexDigits[] = "0123456789abcdef";
    int indice = random(0, 16);
    return hexDigits[indice];
}

/**
 * @brief Adds a message to the log queue
 * @param data Message to be added
 */
void DisplayFK::addLog(const char *data)
{
    if(DisplayFK::xFilaLog != NULL){
        if(uxQueueSpacesAvailable(DisplayFK::xFilaLog) > 0){
            const size_t lengthLine = strlen(data) + 1;
            const size_t max_length = (lengthLine > MAX_LINE_LENGTH) ? MAX_LINE_LENGTH : lengthLine;
            logMessage_t message;
            for(int i = 0; i < max_length; ++i){
            message.line[i] = data[i];
            }
            message.line_length = max_length;
            message.line[message.line_length] = 0; // Add the terminating nul char

            int ret = xQueueSend(DisplayFK::xFilaLog, (void*) &message, 0);
            if(ret == pdTRUE){
                Serial.printf("Add message to log: %s\n", message.line);
            }else if(ret == errQUEUE_FULL){
                Serial.println("Unable to send log data into the Queue");
            }
        }else{
            DEBUG_E("Can't add log. Queue is full");
        }
    }
}

/**
 * @brief Constructor of the DisplayFK class
 */
DisplayFK::DisplayFK() : m_configs(), m_runningTransaction(false)
{
    DisplayFK::instance = this;
    WidgetBase::fontBold = const_cast<GFXfont *>(&Roboto_Bold5pt7b);
    WidgetBase::fontNormal = const_cast<GFXfont *>(&Roboto_Bold5pt7b);

    

    
}

void DisplayFK::startKeyboards(){
    #ifdef DFK_TEXTBOX
        keyboard = new WKeyboard();
        keyboard->setup();
    #endif
    #ifdef DFK_NUMBERBOX
        numpad = new Numpad();
        numpad->setup();
    #endif
}

/**
 * @brief Destructor of the DisplayFK class
 */
DisplayFK::~DisplayFK()
{
    // Free all static arrays
#if defined(DFK_TOUCHAREA)
    if (arrayTouchArea) {
        delete[] arrayTouchArea;
        arrayTouchArea = nullptr;
    }
#endif

#ifdef DFK_CHECKBOX
    if (arrayCheckbox) {
        delete[] arrayCheckbox;
        arrayCheckbox = nullptr;
    }
#endif

#ifdef DFK_CIRCLEBTN
    if (arrayCircleBtn) {
        delete[] arrayCircleBtn;
        arrayCircleBtn = nullptr;
    }
#endif

#ifdef DFK_GAUGE
    if (arrayGauge) {
        delete[] arrayGauge;
        arrayGauge = nullptr;
    }
#endif

#ifdef DFK_CIRCULARBAR
    if (arrayCircularBar) {
        delete[] arrayCircularBar;
        arrayCircularBar = nullptr;
    }
#endif

#ifdef DFK_HSLIDER
    if (arrayHSlider) {
        delete[] arrayHSlider;
        arrayHSlider = nullptr;
    }
#endif

#ifdef DFK_LABEL
    if (arrayLabel) {
        delete[] arrayLabel;
        arrayLabel = nullptr;
    }
#endif

#ifdef DFK_LED
    if (arrayLed) {
        delete[] arrayLed;
        arrayLed = nullptr;
    }
#endif

#ifdef DFK_LINECHART
    if (arrayLineChart) {
        delete[] arrayLineChart;
        arrayLineChart = nullptr;
    }
#endif

#ifdef DFK_RADIO
    if (arrayRadioGroup) {
        delete[] arrayRadioGroup;
        arrayRadioGroup = nullptr;
    }
#endif

#ifdef DFK_RECTBTN
    if (arrayRectBtn) {
        delete[] arrayRectBtn;
        arrayRectBtn = nullptr;
    }
#endif

#ifdef DFK_TOGGLE
    if (arrayToggleBtn) {
        delete[] arrayToggleBtn;
        arrayToggleBtn = nullptr;
    }
#endif

#ifdef DFK_VBAR
    if (arrayVBar) {
        delete[] arrayVBar;
        arrayVBar = nullptr;
    }
#endif

#ifdef DFK_VANALOG
    if (arrayVAnalog) {
        delete[] arrayVAnalog;
        arrayVAnalog = nullptr;
    }
#endif

#ifdef DFK_TEXTBOX
    if (arrayTextBox) {
        delete[] arrayTextBox;
        arrayTextBox = nullptr;
    }
    if (keyboard) {
        delete keyboard;
        keyboard = nullptr;
    }
#endif

#ifdef DFK_NUMBERBOX
    if (arrayNumberbox) {
        delete[] arrayNumberbox;
        arrayNumberbox = nullptr;
    }
    if (numpad) {
        delete numpad;
        numpad = nullptr;
    }
#endif

#ifdef DFK_IMAGE
    if (arrayImage) {
        delete[] arrayImage;
        arrayImage = nullptr;
    }
#endif

#ifdef DFK_TEXTBUTTON
    if (arrayTextButton) {
        delete[] arrayTextButton;
        arrayTextButton = nullptr;
    }
#endif

#ifdef DFK_SPINBOX
    if (arraySpinbox) {
        delete[] arraySpinbox;
        arraySpinbox = nullptr;
    }
#endif

#if (HAS_TOUCH)
    if (touchExterno) {
        delete touchExterno;
        touchExterno = nullptr;
    }
#endif

#ifdef DFK_SD
    if (m_nameLogFile) {
        delete[] m_nameLogFile;
        m_nameLogFile = nullptr;
    }
#endif

    // Free the log queue
    if (xFilaLog) {
        vQueueDelete(xFilaLog);
        xFilaLog = nullptr;
    }

    // Reset static variables
    //funcaoCB = nullptr;
    sdcardOK = false;
    logIndex = 0;
    logFileCount = 1;
}


/**
 * @brief Sets up the display
 */
void DisplayFK::setup(){
    DisplayFK::xFilaLog = xQueueCreate(LOG_LENGTH, sizeof(logMessage_t));
    WidgetBase::xFilaCallback = xQueueCreate(5, sizeof(functionCB_t));
    if (DisplayFK::xFilaLog == NULL)
    {
        DEBUG_E("Fail while creating log queue");
    }else{
        DEBUG_D("Log queue created");
    }
    if (WidgetBase::xFilaCallback == NULL)
    {
        DEBUG_E("Fail while creating callback queue");
    }else{
        DEBUG_D("Callback queue created");
    }
}

/**
 * @brief Starts a drawing transaction
 */
void DisplayFK::startTransaction(){
    if(m_runningTransaction){
        DEBUG_W("Transaction already running.");
        return;
    }

    m_runningTransaction = true;
}

/**
 * @brief Finishes a drawing transaction
 */
void DisplayFK::finishTransaction(){

    m_runningTransaction = false;
}

/**
 * @brief Sets the normal font for widgets
 * @param _font Pointer to the font
 */
void DisplayFK::setFontNormal(const GFXfont *_font)
{
    WidgetBase::fontNormal = _font;
}

/**
 * @brief Sets the bold font for widgets
 * @param _font Pointer to the font
 */
void DisplayFK::setFontBold(const GFXfont *_font)
{
    WidgetBase::fontBold = _font;
}

#if HAS_TOUCH && defined(DISP_DEFAULT)

/**
 * @brief Initializes the touch system
 * @param w Screen width
 * @param h Screen height
 * @param _rotation Screen rotation
 * @param _sharedSPI Pointer to the shared SPI interface
 */
void DisplayFK::startTouch(uint16_t w, uint16_t h, uint8_t _rotation, SPIClass *_sharedSPI = nullptr)
{
    DEBUG_E("startTouch not implemented.");
    /*touchExterno = new TouchScreen();

    if (touchExterno)
    {
        touchExterno->setDimension(w, h, _rotation);
    }*/
}

void DisplayFK::setTouchCorners(int x0, int x1, int y0, int y1){
    m_x0 = x0;
    m_x1 = x1;
    m_y0 = y0;
    m_y1 = y1;
}

void DisplayFK::setInvertAxis(bool invertX, bool invertY){
  m_invertXAxis = invertX;
  m_invertYAxis = invertY;
}

void DisplayFK::setSwapAxis(bool swap){
  m_swapAxis = swap;
}

#if defined(TOUCH_XPT2046)
void DisplayFK::startTouchXPT2046(uint16_t w, uint16_t h, uint8_t _rotation, int8_t pinCS, SPIClass *_sharedSPI, Arduino_GFX *_objTFT, int touchFrequency, int displayFrequency, int displayPinCS){
    if(!touchExterno){
        touchExterno = new TouchScreen();
        if (touchExterno)
        {
            touchExterno->setTouchCorners(m_x0, m_x1, m_y0, m_y1);
            touchExterno->setInvertAxis(m_invertXAxis, m_invertYAxis);
            touchExterno->setSwapAxis(m_swapAxis);
            touchExterno->startAsXPT2046(w, h, _rotation, -1, -1, -1, pinCS, _sharedSPI, _objTFT, touchFrequency, displayFrequency, displayPinCS);
        }
    }
}
void DisplayFK::startTouchXPT2046(uint16_t w, uint16_t h, uint8_t _rotation, int8_t pinSclk, int8_t pinMosi, int8_t pinMiso, int8_t pinCS, Arduino_GFX *_objTFT, int touchFrequency, int displayFrequency, int displayPinCS){
    if(!touchExterno){
        touchExterno = new TouchScreen();
        if (touchExterno)
        {
            touchExterno->setTouchCorners(m_x0, m_x1, m_y0, m_y1);
            touchExterno->setInvertAxis(m_invertXAxis, m_invertYAxis);
            touchExterno->setSwapAxis(m_swapAxis);
            touchExterno->startAsXPT2046(w, h, _rotation, pinSclk, pinMosi, pinMiso, pinCS, nullptr, _objTFT, touchFrequency, displayFrequency, displayPinCS);
        }
    }
}
#elif defined (TOUCH_FT6236U)
void DisplayFK::startTouchFT6236U(uint16_t w, uint16_t h, uint8_t _rotation, int8_t pinSDA, int8_t pinSCL, uint8_t pinINT, int8_t pinRST){
    if(!touchExterno){
        touchExterno = new TouchScreen();
        if (touchExterno)
        {
            touchExterno->setTouchCorners(m_x0, m_x1, m_y0, m_y1);
            touchExterno->setInvertAxis(m_invertXAxis, m_invertYAxis);
            touchExterno->setSwapAxis(m_swapAxis);
            touchExterno->startAsFT6236U(w, h, _rotation, pinSDA, pinSCL, pinINT, pinRST);
        }
    }
}
#elif defined (TOUCH_FT6336)
void DisplayFK::startTouchFT6336(uint16_t w, uint16_t h, uint8_t _rotation, int8_t pinSDA, int8_t pinSCL, int8_t pinINT, int8_t pinRST){
    if(!touchExterno){
        touchExterno = new TouchScreen();
        if (touchExterno)
        {
            touchExterno->setTouchCorners(m_x0, m_x1, m_y0, m_y1);
            touchExterno->setInvertAxis(m_invertXAxis, m_invertYAxis);
            touchExterno->setSwapAxis(m_swapAxis);
            touchExterno->startAsFT6336(w, h, _rotation, pinSDA, pinSCL, pinINT, pinRST);
        }
    }
}
#elif defined (TOUCH_CST816)
void DisplayFK::startTouchCST816(uint16_t w, uint16_t h, uint8_t _rotation, int8_t pinSDA, int8_t pinSCL, int8_t pinINT, int8_t pinRST){
    if(!touchExterno){
        touchExterno = new TouchScreen();
        if (touchExterno)
        {
            touchExterno->setTouchCorners(m_x0, m_x1, m_y0, m_y1);
            touchExterno->setInvertAxis(m_invertXAxis, m_invertYAxis);
            touchExterno->setSwapAxis(m_swapAxis);
            touchExterno->startAsCST816(w, h, _rotation, pinSDA, pinSCL, pinINT, pinRST);
        }
    }
}
#elif defined (TOUCH_GT911)
void DisplayFK::startTouchGT911(uint16_t w, uint16_t h, uint8_t _rotation, int8_t pinSDA, int8_t pinSCL, int8_t pinINT, int8_t pinRST){
    
    if(!touchExterno){
        touchExterno = new TouchScreen();
        if (touchExterno)
        {
            touchExterno->setTouchCorners(m_x0, m_x1, m_y0, m_y1);
            touchExterno->setInvertAxis(m_invertXAxis, m_invertYAxis);
            touchExterno->setSwapAxis(m_swapAxis);
            touchExterno->startAsGT911(w, h, _rotation, pinSDA, pinSCL, pinINT, pinRST);
        }
    }
}
#endif

#endif

/**
 * @brief Draws widgets on the current screen
 * @param currentScreenIndex Current screen index
 */
void DisplayFK::drawWidgetsOnScreen(const uint8_t currentScreenIndex)
{
    WidgetBase::currentScreen = currentScreenIndex;
    Serial.printf("Drawing widgets of screen:%i\n", WidgetBase::currentScreen);

#if defined(DFK_TOUCHAREA)
    if (m_touchAreaConfigured)
    {
        for (register uint32_t indice = 0; indice < (qtdTouchArea); indice++)
        {
            arrayTouchArea[indice]->redraw();
        }
    }
    else
    {
        DEBUG_W("Array of TouchArea not configured");
    }
#endif

#ifdef DFK_CHECKBOX
    if (m_checkboxConfigured)
    {
        for (register uint32_t indice = 0; indice < (qtdCheckbox); indice++)
        {
            arrayCheckbox[indice]->forceUpdate();
            arrayCheckbox[indice]->redraw();
        }
    }
    else
    {
        DEBUG_W("Array of Checkbox not configured");
    }
#endif
#ifdef DFK_CIRCLEBTN
    if (m_circleButtonConfigured)
    {
        for (register uint32_t indice = 0; indice < (qtdCircleBtn); indice++)
        {
            arrayCircleBtn[indice]->forceUpdate();
            arrayCircleBtn[indice]->redraw();
        }
    }
    else
    {
        DEBUG_W("Array of CircleButton not configured");
    }
#endif
#ifdef DFK_SPINBOX
    if (m_spinboxConfigured)
    {
        for (register uint32_t indice = 0; indice < (qtdSpinbox); indice++)
        {
            arraySpinbox[indice]->drawBackground();
            arraySpinbox[indice]->redraw();
        }
    }
    else
    {
        DEBUG_W("Array of Spinbox not configured");
    }
#endif
#ifdef DFK_GAUGE
    if (m_gaugeConfigured)
    {
        for (register uint32_t indice = 0; indice < (qtdGauge); indice++)
        {
            arrayGauge[indice]->forceUpdate();
            arrayGauge[indice]->drawBackground();
        }
    }
    else
    {
        DEBUG_W("Array of Gauge not configured");
    }

#endif

#ifdef DFK_CIRCULARBAR
    if (m_circularBarConfigured)
    {
        for (register uint32_t indice = 0; indice < (qtdCircularBar); indice++)
        {
            arrayCircularBar[indice]->forceUpdate();
            arrayCircularBar[indice]->drawBackground();
        }
    }
    else
    {
        DEBUG_W("Array of CircularBar not configured");
    }
#endif

#ifdef DFK_HSLIDER
    if (m_hSliderConfigured)
    {
        for (register uint32_t indice = 0; indice < (qtdHSlider); indice++)
        {
            arrayHSlider[indice]->forceUpdate();
            arrayHSlider[indice]->drawBackground();
        }
    }
    else
    {
        DEBUG_W("Array of HSlider not configured");
    }
#endif
#ifdef DFK_LABEL
    if (m_labelConfigured)
    {
        for (register uint32_t indice = 0; indice < (qtdLabel); indice++)
        {
            arrayLabel[indice]->forceUpdate();
            arrayLabel[indice]->redraw();
        }
    }
    else
    {
        DEBUG_W("Array of Label not configured");
    }
#endif
#ifdef DFK_LED
    if (m_ledConfigured)
    {
        for (register uint32_t indice = 0; indice < (qtdLed); indice++)
        {
            arrayLed[indice]->forceUpdate();
            arrayLed[indice]->drawBackground();
            arrayLed[indice]->redraw();
        }
    }
    else
    {
        DEBUG_W("Array of Led not configured");
    }
#endif
#ifdef DFK_LINECHART
    if (m_lineChartConfigured)
    {
        for (register uint32_t indice = 0; indice < (qtdLineChart); indice++)
        {
            arrayLineChart[indice]->forceUpdate();
            arrayLineChart[indice]->drawBackground();
        }
    }
    else
    {
        DEBUG_W("Array of LineChart not configured");
    }
#endif
#ifdef DFK_RADIO
    if (m_radioGroupConfigured)
    {
        for (register uint32_t indice = 0; indice < (qtdRadioGroup); indice++)
        {
            arrayRadioGroup[indice]->forceUpdate();
            arrayRadioGroup[indice]->redraw();
        }
    }
    else
    {
        DEBUG_W("Array of RadioGroup not configured");
    }
#endif
#ifdef DFK_RECTBTN
    if (m_rectButtonConfigured)
    {
        for (register uint32_t indice = 0; indice < (qtdRectBtn); indice++)
        {
            arrayRectBtn[indice]->forceUpdate();
            arrayRectBtn[indice]->redraw();
        }
    }
    else
    {
        DEBUG_W("Array of RectButton not configured");
    }
#endif
#ifdef DFK_TOGGLE
    if (m_toggleConfigured)
    {
        for (register uint32_t indice = 0; indice < (qtdToggle); indice++)
        {
            arrayToggleBtn[indice]->forceUpdate();
            arrayToggleBtn[indice]->redraw();
        }
    }
    else
    {
        DEBUG_W("Array of Toggle not configured");
    }
#endif

#ifdef DFK_TEXTBUTTON
    if (m_textButtonConfigured)
    {
        for (register uint32_t indice = 0; indice < (qtdTextButton); indice++)
        {
            arrayTextButton[indice]->redraw();
        }
    }
    else
    {
        DEBUG_W("Array of TextButton not configured");
    }
#endif

#ifdef DFK_VBAR
    if (m_vBarConfigured)
    {
        for (register uint32_t indice = 0; indice < (qtdVBar); indice++)
        {
            arrayVBar[indice]->forceUpdate();
            arrayVBar[indice]->drawBackground();
        }
    }
    else
    {
        DEBUG_W("Array of VBar not configured");
    }
#endif
#ifdef DFK_VANALOG
    if (m_vAnalogConfigured)
    {
        for (register uint32_t indice = 0; indice < (qtdVAnalog); indice++)
        {
            arrayVAnalog[indice]->forceUpdate();
            arrayVAnalog[indice]->drawBackground();
        }
    }
    else
    {
        DEBUG_W("Array of VAnalog not configured");
    }
#endif
#ifdef DFK_TEXTBOX
    if (m_textboxConfigured)
    {
        for (register uint32_t indice = 0; indice < (qtdTextBox); indice++)
        {
            arrayTextBox[indice]->forceUpdate();
            arrayTextBox[indice]->redraw();
        }
    }
    else
    {
        DEBUG_W("Array of Textbox not configured");
    }
#endif
#ifdef DFK_NUMBERBOX
    if (m_numberboxConfigured)
    {
        for (register uint32_t indice = 0; indice < (qtdNumberBox); indice++)
        {
            arrayNumberbox[indice]->forceUpdate();
            arrayNumberbox[indice]->redraw();
        }
    }
    else
    {
        DEBUG_W("Array of Numberbox not configured");
    }
#endif
#ifdef DFK_IMAGE
    if (m_imageConfigured)
    {
        for (register uint32_t indice = 0; indice < (qtdImage); indice++)
        {
            arrayImage[indice]->forceUpdate();
            arrayImage[indice]->draw();
            //arrayImage[indice].redraw();
        }
    }
    else
    {
        DEBUG_W("Array of Image not configured");
    }
#endif
}

/**
 * @brief Creates the event processing task
 */
void DisplayFK::createTask()
{
    this->setup();
    this->startKeyboards();

    BaseType_t xRetorno;
    xRetorno = xTaskCreatePinnedToCore(DisplayFK::TaskEventoTouch, "TaskEventoTouch", configMINIMAL_STACK_SIZE + 3048, this, 1, &m_hndTaskEventoTouch, 0);
    if (xRetorno == pdFAIL)
    {
        DEBUG_E("Cant create task to read touch or draw widgets");
    }
}

/**
 * @brief Draws a PNG image on the screen
 * @param _x Initial X position
 * @param _y Initial Y position
 * @param _colors Color array
 * @param _mask Image mask
 * @param _w Image width
 * @param _h Image height
 */
void DisplayFK::drawPng(uint16_t _x, uint16_t _y, const uint16_t _colors[], const uint8_t _mask[], uint16_t _w, uint16_t _h)
{
    for (auto i = 0; i < _w; ++i)
    {
        for (auto j = 0; j < _h; ++j)
        {
            auto posX = _x + i;
            auto posY = _y + j;
            uint8_t _bit = _mask[(i * _w) + j];
            uint16_t _cor = _colors[(i * _w) + j];
            if (_bit)
            {
                WidgetBase::objTFT->drawPixel(posX, posY, _cor);
            }
        }
    }
}

/**
 * @brief Processes the callback queue
 */
void DisplayFK::processCallback(){
    
    if(!WidgetBase::xFilaCallback) return;

    functionCB_t cb;
    int ret = xQueueReceive(WidgetBase::xFilaCallback, &cb, pdMS_TO_TICKS(1));
    if(ret == pdPASS && cb != nullptr){
        cb();
    }
}

/**
 * @brief Processes the log queue
 */
void DisplayFK::processLogQueue() {
    
    if (!xFilaLog) return;
    
    logMessage_t messageInLog;
    int retornoLog = xQueueReceive(xFilaLog, &messageInLog, 0);
    
    if (retornoLog == pdPASS) {
        Serial.printf("ADD LOG FILE\t-->\t%i\t%s\n", messageInLog.line_length, messageInLog.line);
        
        if (logIndex >= LOG_LENGTH) {
            DEBUG_E("Log buffer overflow");
            return;
        }
        
        memcpy(&bufferLog[logIndex], &messageInLog, sizeof(messageInLog));
        logIndex++;

        if (logIndex >= (LOG_LENGTH * 0.8)) {
            processLogBuffer();
        }
    }
}

/**
 * @brief Processes the log buffer
 */
void DisplayFK::processLogBuffer() {
    Serial.printf("Write log file %i lines\n", logIndex);
    
    if (!sdcardOK) {
        DEBUG_E("SD card not available for log writing");
        return;
    }
    
    appendLog(WidgetBase::mySD, getLogFileName(), static_cast<const logMessage_t *>(bufferLog), logIndex, true);
    
    for (size_t i = 0; i < logIndex; i++) {
        Serial.printf("WRITE LOG FILE LINE:%i\t-->\t%i\t%s\n", i, bufferLog[i].line_length, bufferLog[i].line);
    }
    
    logIndex = 0;
}

/**
 * @brief Processes touch events
 * @param xTouch Touch X position
 * @param yTouch Touch Y position
 * @param zPressure Touch pressure
 * @param gesture Detected gesture
 */
void DisplayFK::processTouchEvent(uint16_t xTouch, uint16_t yTouch, int zPressure, uint8_t gesture) {
    UNUSED(gesture);
    UNUSED(zPressure);
    
    if (WidgetBase::usingKeyboard) return;

    // Process other touchable widgets
    processTouchableWidgets(xTouch, yTouch);
}

/**
 * @brief Processes touchable widgets
 * @param xTouch Touch X position
 * @param yTouch Touch Y position
 */
void DisplayFK::processTouchableWidgets(uint16_t xTouch, uint16_t yTouch) {
    DEBUG_D("Processing touchable widgets");
    // Process each type of touchable widget
    processCheckboxTouch(xTouch, yTouch);
    processCircleButtonTouch(xTouch, yTouch);
    processHSliderTouch(xTouch, yTouch);
    processRadioGroupTouch(xTouch, yTouch);
    processRectButtonTouch(xTouch, yTouch);
    processImageTouch(xTouch, yTouch);
    processSpinboxTouch(xTouch, yTouch);
    processToggleTouch(xTouch, yTouch);
    processTextButtonTouch(xTouch, yTouch);
    processTextBoxTouch(xTouch, yTouch);
    processNumberBoxTouch(xTouch, yTouch);
    processEmptyAreaTouch(xTouch, yTouch);
}

/**
 * @brief Processes touch in empty area
 * @param xTouch Touch X position
 * @param yTouch Touch Y position
 */
void DisplayFK::processEmptyAreaTouch(uint16_t xTouch, uint16_t yTouch) {
#if defined(DFK_TOUCHAREA)
    if(!m_touchAreaConfigured || !arrayTouchArea){
        return;
    }
    
        for (uint32_t indice = 0; indice < qtdTouchArea; indice++) {
            if (arrayTouchArea[indice]->detectTouch(&xTouch, &yTouch)) {
                functionCB_t cb = arrayTouchArea[indice]->getCallbackFunc();
                WidgetBase::addCallback(cb, WidgetBase::CallbackOrigin::TOUCH);
                return;
            }
        }
#endif
}

/**
 * @brief Processes touch in checkbox
 * @param xTouch Touch X position
 * @param yTouch Touch Y position
 */
void DisplayFK::processCheckboxTouch(uint16_t xTouch, uint16_t yTouch) {
#ifdef DFK_CHECKBOX
    if(!m_checkboxConfigured || !arrayCheckbox){
        return;
    }
    
        for (uint32_t indice = 0; indice < qtdCheckbox; indice++) {
            if (arrayCheckbox[indice]->detectTouch(&xTouch, &yTouch)) {
                functionCB_t cb = arrayCheckbox[indice]->getCallbackFunc();
                WidgetBase::addCallback(cb, WidgetBase::CallbackOrigin::TOUCH);
                return;
            }
        }
    
#endif
}

/**
 * @brief Processes touch in circle button
 * @param xTouch Touch X position
 * @param yTouch Touch Y position
 */
void DisplayFK::processCircleButtonTouch(uint16_t xTouch, uint16_t yTouch) {
#ifdef DFK_CIRCLEBTN
    if(!m_circleButtonConfigured || !arrayCircleBtn){
        return;
    }
    
        for (uint32_t indice = 0; indice < qtdCircleBtn; indice++) {
            if (arrayCircleBtn[indice]->detectTouch(&xTouch, &yTouch)) {
                functionCB_t cb = arrayCircleBtn[indice]->getCallbackFunc();
                WidgetBase::addCallback(cb, WidgetBase::CallbackOrigin::TOUCH);
                return;
            }
        }
#endif
}

/**
 * @brief Processes touch in horizontal slider
 * @param xTouch Touch X position
 * @param yTouch Touch Y position
 */
void DisplayFK::processHSliderTouch(uint16_t xTouch, uint16_t yTouch) {
#ifdef DFK_HSLIDER
    if(!m_hSliderConfigured || !arrayHSlider){
        return;
    }
    
        for (uint32_t indice = 0; indice < qtdHSlider; indice++) {
            if (arrayHSlider[indice]->detectTouch(&xTouch, &yTouch)) {
                functionCB_t cb = arrayHSlider[indice]->getCallbackFunc();
                WidgetBase::addCallback(cb, WidgetBase::CallbackOrigin::TOUCH);
                return;
            }
        }
#endif
}

/**
 * @brief Processes touch in radio button group
 * @param xTouch Touch X position
 * @param yTouch Touch Y position
 */
void DisplayFK::processRadioGroupTouch(uint16_t xTouch, uint16_t yTouch) {
#ifdef DFK_RADIO
    if(!m_radioGroupConfigured || !arrayRadioGroup){
        return;
    }
    
        for (uint32_t indice = 0; indice < qtdRadioGroup; indice++) {
            if (arrayRadioGroup[indice]->detectTouch(&xTouch, &yTouch)) {
                functionCB_t cb = arrayRadioGroup[indice]->getCallbackFunc();
                WidgetBase::addCallback(cb, WidgetBase::CallbackOrigin::TOUCH);
                return;
            }
        }
#endif
}

/**
 * @brief Processes touch in rectangular button
 * @param xTouch Touch X position
 * @param yTouch Touch Y position
 */
void DisplayFK::processRectButtonTouch(uint16_t xTouch, uint16_t yTouch) {
#ifdef DFK_RECTBTN
    if(!m_rectButtonConfigured || !arrayRectBtn){
        return;
    }
    
        for (uint32_t indice = 0; indice < qtdRectBtn; indice++) {
            if (arrayRectBtn[indice]->detectTouch(&xTouch, &yTouch)) {
                functionCB_t cb = arrayRectBtn[indice]->getCallbackFunc();
                WidgetBase::addCallback(cb, WidgetBase::CallbackOrigin::TOUCH);
                return;
            }
        }
#endif
}

/**
 * @brief Processes touch in image
 * @param xTouch Touch X position
 * @param yTouch Touch Y position
 */
void DisplayFK::processImageTouch(uint16_t xTouch, uint16_t yTouch) {
#ifdef DFK_IMAGE
    if(!m_imageConfigured || !arrayImage){
        return;
    }
    
        for (uint32_t indice = 0; indice < qtdImage; indice++) {
            if (arrayImage[indice]->detectTouch(&xTouch, &yTouch)) {
                functionCB_t cb = arrayImage[indice]->getCallbackFunc();
                WidgetBase::addCallback(cb, WidgetBase::CallbackOrigin::TOUCH);
                return;
            }
        }
#endif
}

/**
 * @brief Processes touch in spin box
 * @param xTouch Touch X position
 * @param yTouch Touch Y position
 */
void DisplayFK::processSpinboxTouch(uint16_t xTouch, uint16_t yTouch) {
#ifdef DFK_SPINBOX
    if(!m_spinboxConfigured || !arraySpinbox){
        return;
    }
    
        for (uint32_t indice = 0; indice < qtdSpinbox; indice++) {
            if (arraySpinbox[indice]->detectTouch(&xTouch, &yTouch)) {
                functionCB_t cb = arraySpinbox[indice]->getCallbackFunc();
                WidgetBase::addCallback(cb, WidgetBase::CallbackOrigin::TOUCH);
                return;
            }
        }
    
#endif
}

/**
 * @brief Processes touch in toggle button
 * @param xTouch Touch X position
 * @param yTouch Touch Y position
 */
void DisplayFK::processToggleTouch(uint16_t xTouch, uint16_t yTouch) {
#ifdef DFK_TOGGLE
    if(!m_toggleConfigured || !arrayToggleBtn){
        return;
    }

        for (uint32_t indice = 0; indice < qtdToggle; indice++) {
            if (arrayToggleBtn[indice]->detectTouch(&xTouch, &yTouch)) {
                //funcaoCB = arrayToggleBtn[indice].getCallbackFunc();
                functionCB_t cb = arrayToggleBtn[indice]->getCallbackFunc();
                WidgetBase::addCallback(cb, WidgetBase::CallbackOrigin::TOUCH);
                return;
            }
        }
#endif
}

/**
 * @brief Processes touch in text button
 * @param xTouch Touch X position
 * @param yTouch Touch Y position
 */
void DisplayFK::processTextButtonTouch(uint16_t xTouch, uint16_t yTouch) {
#ifdef DFK_TEXTBUTTON
    if(!m_textButtonConfigured || !arrayTextButton){
        return;
    }
    
    for (uint32_t indice = 0; indice < qtdTextButton; indice++) {
        if (arrayTextButton[indice]->detectTouch(&xTouch, &yTouch)) {
            functionCB_t cb = arrayTextButton[indice]->getCallbackFunc();
            WidgetBase::addCallback(cb, WidgetBase::CallbackOrigin::TOUCH);
            return;
        }
    }
#endif
}

/**
 * @brief Processes touch in text box
 * @param xTouch Touch X position
 * @param yTouch Touch Y position
 */
void DisplayFK::processTextBoxTouch(uint16_t xTouch, uint16_t yTouch) {
#ifdef DFK_TEXTBOX
    if(!m_textboxConfigured || !arrayTextBox || !keyboard){
        return;
    }
        for (uint32_t indice = 0; indice < qtdTextBox; indice++) {
            if (arrayTextBox[indice] == nullptr) {
                DEBUG_E("TextBox pointer is null");
                continue;
            }

            if (arrayTextBox[indice]->detectTouch(&xTouch, &yTouch) && keyboard) {
                keyboard->open(arrayTextBox[indice]);
                WKeyboard::PressedKeyType pressedKey = WKeyboard::PressedKeyType::NONE;

                while (WidgetBase::usingKeyboard) {
                    uint16_t internal_xTouch = 0;
                    uint16_t internal_yTouch = 0;
                    int internal_zPressure = 0;
                    bool hasTouch = touchExterno && (touchExterno->getTouch(&internal_xTouch, &internal_yTouch, &internal_zPressure));
                    if (hasTouch && keyboard->detectTouch(&internal_xTouch, &internal_yTouch, &pressedKey)) {
                        if (pressedKey == WKeyboard::PressedKeyType::RETURN) {
                            WidgetBase::loadScreen = keyboard->m_field->parentScreen;
                            keyboard->close();
                            functionCB_t cb = arrayTextBox[indice]->getCallbackFunc();
                            WidgetBase::addCallback(cb, WidgetBase::CallbackOrigin::TOUCH);
                            return;
                        }
                    }
                    vTaskDelay(pdMS_TO_TICKS(10));
                }
            }
        }
#endif
}

/**
 * @brief Processes touch in number box
 * @param xTouch Touch X position
 * @param yTouch Touch Y position
 */
void DisplayFK::processNumberBoxTouch(uint16_t xTouch, uint16_t yTouch) {
#ifdef DFK_NUMBERBOX
        if(!m_numberboxConfigured || !arrayNumberbox || !numpad){
            return;
        }

        for (uint32_t indice = 0; indice < qtdNumberBox; indice++) {
            if (arrayNumberbox[indice] == nullptr) {
                DEBUG_E("NumberBox pointer is null");
                continue;
            }
            if (arrayNumberbox[indice]->detectTouch(&xTouch, &yTouch) && numpad) {
                numpad->open(arrayNumberbox[indice]);
                Numpad::PressedKeyType pressedKey = Numpad::PressedKeyType::NONE;

                while (WidgetBase::usingKeyboard) {
                    uint16_t internal_xTouch = 0;
                    uint16_t internal_yTouch = 0;
                    int internal_zPressure = 0;
                    bool hasTouch = touchExterno && (touchExterno->getTouch(&internal_xTouch, &internal_yTouch, &internal_zPressure));
                    if (hasTouch && numpad->detectTouch(&internal_xTouch, &internal_yTouch, &pressedKey)) {
                        if (pressedKey == Numpad::PressedKeyType::RETURN) {
                            if (numpad->m_field && numpad->m_field->parentScreen) {
                                WidgetBase::loadScreen = numpad->m_field->parentScreen;
                            }
                            numpad->close();
                            functionCB_t cb = arrayNumberbox[indice]->getCallbackFunc();
                            WidgetBase::addCallback(cb, WidgetBase::CallbackOrigin::TOUCH);
                            return;
                        }
                    }
                    vTaskDelay(pdMS_TO_TICKS(10));
                }
            }
        }
#endif
}

/**
 * @brief Updates screen widgets
 */
void DisplayFK::updateWidgets() {
    if (!m_runningTransaction) {
        updateCircularBar();
        updateGauge();
        updateLabel();
        updateLed();
        updateLineChart();
        updateVBar();
        updateVAnalog();
        updateCheckbox();
        updateCircleButton();
        updateHSlider();
        updateRadioGroup();
        updateRectButton();
        updateToggle();
        //updateImage();
        //updateTextButton();
        updateSpinbox();
        updateNumberBox();
        updateTextBox();
    }
}

/**
 * @brief Main task loop
 */
void DisplayFK::loopTask() {
    uint32_t startTime = micros();
    
    // Process screen loading
    if (WidgetBase::loadScreen) {
        log_d("Loading screen on taskloop");
        WidgetBase::loadScreen();
        WidgetBase::loadScreen = nullptr;
        vTaskDelay(pdMS_TO_TICKS(5));
    }

    // Process log queue
    processLogQueue();

    // Process callback queue
    processCallback();

    // Process touch events
#if defined(TOUCH_CS) || HAS_TOUCH
    uint16_t xTouch, yTouch;
    int zPressure;
    bool hasTouch = false;
    uint8_t gesture = 0;

#if defined(DISP_DEFAULT)
    hasTouch = touchExterno && (touchExterno->getTouch(&xTouch, &yTouch, &zPressure));
    gesture = hasTouch ? touchExterno->getGesture() : 0;
#else
    hasTouch = false;
    gesture = 0;
#endif

    if (hasTouch) {
        if(m_debugTouch){
            Serial.printf("Touch event: [%i, %i], %i, %i\n", xTouch, yTouch, zPressure, gesture);
            WidgetBase::objTFT->fillCircle(xTouch, yTouch, 2, CFK_FUCHSIA);
        }
        processTouchEvent(xTouch, yTouch, zPressure, gesture);
    }
#endif

    // Update widgets
    updateWidgets();

    // Calculate and log execution time
    startTime = micros() - startTime;
    vTaskDelay(pdMS_TO_TICKS(1));
}

/**
 * @brief Touch event task callback function
 * @param pvParameters Task parameters
 */
void DisplayFK::TaskEventoTouch(void *pvParameters)
{
    //(void)pvParameters;
    //DisplayFK *instance = static_cast<DisplayFK *>(pvParameters);

    DisplayFK::instance->changeWTD(false);

    DEBUG_D("TaskEventoTouch created");
    // const TickType_t xDelay = 10 / portTICK_PERIOD_MS;

    for (;;)
    {
        if(DisplayFK::instance){
            DisplayFK::instance->loopTask();
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

/**
 * @brief Returns the touch event task handle
 * @return Task handle
 */
TaskHandle_t DisplayFK::getTaskHandle()
{
    return m_hndTaskEventoTouch;
}

/** @brief Enables touch log */
void DisplayFK::enableTouchLog(){
    m_debugTouch = true;
}

/** @brief Disables touch log */
void DisplayFK::disableTouchLog(){
    m_debugTouch = false;
}

#if defined(TOUCH_XPT2046) && HAS_TOUCH

/**
 * @brief Checks and performs touch calibration
 */
void DisplayFK::checkCalibration()
{
#if HAS_TOUCH
    if (!WidgetBase::objTFT)
    {
        Serial.println("Define WidgetBase::objTFT before use this function");
        return;
    }
    else
    {
        Serial.println("Continuing calibration check");
    }
    if(!touchExterno){
        Serial.println("Touch externo not defined");
        return;
    }
    
    const uint8_t lengthCalibrationPoints = 4;
    const uint8_t raioDoMarcador = 20;
    const uint32_t corDoFundo = CFK_WHITE;
    const uint32_t corDoMarcador = CFK_RED;
    CalibrationPoint_t dados[lengthCalibrationPoints];
    memset(dados, 0, sizeof(dados));


    m_configs.begin("iniciais", false);                    // Abre o namespace 'iniciais' para leitura e gravação
    bool jaCalibrado = m_configs.getBool("jaCalibrado", false); // carrega a flag de calibração prévia
    uint8_t savedRotation = m_configs.getUChar("rotation", 255);
    /*if (savedRotation != touchExterno->getRotation())
    {
        jaCalibrado = false;
    }*/

    Serial.printf("saved rotation: %i\n", savedRotation);

    if (m_configs.isKey("CALIB"))
    {
        m_configs.getBytes("CALIB", dados, sizeof(dados));
    }

    // Define a mensagem
    WidgetBase::objTFT->fillScreen(corDoFundo);
    // WidgetBase::objTFT->setTextDatum(CC_DATUM);
    WidgetBase::objTFT->setTextColor(corDoMarcador, corDoFundo);
    if (!jaCalibrado)
    {
        WidgetBase::objTFT->setRotation(0);
        uint16_t widthScreen = WidgetBase::objTFT->width();
        uint16_t heightScreen = WidgetBase::objTFT->height();
        Serial.printf("Dimensao da tela: %i x %i\n", widthScreen, heightScreen);

        Serial.println("recalibrando");

        //uint16_t widthScreen = touchExterno ? touchExterno->getWidthScreen() : WidgetBase::objTFT->width();
        //uint16_t heightScreen = touchExterno ? touchExterno->getHeightScreen() : WidgetBase::objTFT->height();

        const char *msg = "Starting calibration";
        
        TextBound_t retorno;
        WidgetBase::objTFT->getTextBounds(msg, 0, 0, &retorno.x, &retorno.y, &retorno.width, &retorno.height);
        
        
        WidgetBase::objTFT->setCursor(widthScreen / 2 - retorno.width / 2, heightScreen / 4 - retorno.height / 2);
        WidgetBase::objTFT->print(msg);
        //Dimensao da tela
        Serial.printf("Dimensao da tela: %i x %i\n", widthScreen, heightScreen);

        Rect_t rectScreen = {0, 0, widthScreen, heightScreen};

        // Executar calibração e teste de tela, reportar valores de calibração na tela
        // touchExterno->calibrateTouch(dadosDeCalibracao, corDaLinha, corFundoDaCaixa, tamanhoDaCaixa);
        // touchExterno->calibrateTouch9Points(dadosDeCalibracao, corDaLinha, corFundoDaCaixa, tamanhoDaCaixa);

        touchExterno->calibrateTouchEstrutura(dados, lengthCalibrationPoints, &rectScreen, corDoMarcador, corDoFundo, raioDoMarcador);
        m_configs.putBool("jaCalibrado", true); // Salva a flag de calibração prévia
        // configs.putBytes("calib", dados, sizeof(dados));
        Serial.printf("Total size struct: %u bytes\n", sizeof(CalibrationPoint_t));
        Serial.printf("Total size calibration data: %u bytes\n", sizeof(dados));
        if (dados)
        {
            uint8_t bytesCalib[sizeof(dados)];
            memcpy(bytesCalib, dados, 4 * sizeof(CalibrationPoint_t));
            m_configs.putBytes("CALIB", bytesCalib, sizeof(dados));
            m_configs.putUChar("rotation", touchExterno->getRotation());
            for (uint8_t i = 0; i < 4; i++)
            {
                Serial.printf("Calibrado Tela: %i x %i = Touch: %i x %i\n", dados[i].xScreen, dados[i].yScreen, dados[i].xTouch, dados[i].yTouch);
            }
        }
        WidgetBase::objTFT->setCursor(widthScreen / 2 - retorno.width / 2, heightScreen / 4 - retorno.height / 2);
        WidgetBase::objTFT->print("Calibrated");
    }
    else
    {
        Serial.println("ja calibrado");
        for (uint8_t i = 0; i < 4; i++)
        {
            Serial.printf("Calibrado Tela: %i x %i = Touch: %i x %i\n", dados[i].xScreen, dados[i].yScreen, dados[i].xTouch, dados[i].yTouch);
        }
    }
    m_configs.end();
    // Ajustar os valores de calibração da tela
    touchExterno->setCalibration(dados);
    Serial.println("Calibration OK");
    // WidgetBase::objTFT->setTouch(dadosDeCalibracao);
    WidgetBase::objTFT->setCursor(WidgetBase::objTFT->width() / 2, WidgetBase::objTFT->height() / 4);
    WidgetBase::objTFT->print("Done");
    // WidgetBase::objTFT->setTextPadding(WidgetBase::objTFT->textWidth(msg));
    // WidgetBase::objTFT->drawString(msg, WidgetBase::objTFT->width() / 2, WidgetBase::objTFT->height() / 2, 4);
    vTaskDelay(pdMS_TO_TICKS(1000));
    WidgetBase::objTFT->fillScreen(CFK_BLACK);
    WidgetBase::objTFT->setRotation(touchExterno->getRotation());
#else
    DEBUG_W("TOUCH_CS wasnt defined on TFT_eSPI library.");
#endif
}

/**
 * @brief Forces a new touch calibration
 */
void DisplayFK::recalibrate()
{
    m_configs.begin("iniciais", false);
    // bool jaCalibrado = configs.getBool("jaCalibrado", false);
    m_configs.putBool("jaCalibrado", false);
    m_configs.end();
    // ESP.restart();
}
#endif


void DisplayFK::printText(const char* _texto, uint16_t _x, uint16_t _y, uint8_t _datum, uint16_t _colorText, uint16_t _colorPadding, const GFXfont* _font){
    if(_font){
        WidgetBase::objTFT->setFont(_font);
    }else{
        WidgetBase::objTFT->setFont(NULL);
    }
    
    WidgetBase::objTFT->setTextColor(_colorText, _colorPadding);
    WidgetBase::recalculateTextPosition(_texto, &_x, &_y, _datum);
    WidgetBase::objTFT->setCursor(_x, _y);
    WidgetBase::objTFT->print(_texto);
    WidgetBase::objTFT->setFont(NULL);
    
}

/**
 * @brief Updates circular bars
 */
void DisplayFK::updateCircularBar() {
#ifdef DFK_CIRCULARBAR

    if (m_circularBarConfigured) {
        for (uint32_t indice = 0; indice < qtdCircularBar; indice++) {
            arrayCircularBar[indice]->redraw();
        }
    }
#endif
}

/**
 * @brief Updates gauges
 */
void DisplayFK::updateGauge() {
#ifdef DFK_GAUGE
    if (m_gaugeConfigured) {
        for (uint32_t indice = 0; indice < qtdGauge; indice++) {
            arrayGauge[indice]->redraw();
        }
    }
#endif
}

/**
 * @brief Updates labels
 */
void DisplayFK::updateLabel() {
#ifdef DFK_LABEL
    if (m_labelConfigured) {
        for (uint32_t indice = 0; indice < qtdLabel; indice++) {
            arrayLabel[indice]->redraw();
        }
    }
#endif
}

/**
 * @brief Updates LEDs
 */
void DisplayFK::updateLed() {
#ifdef DFK_LED
    if (m_ledConfigured) {
        for (uint32_t indice = 0; indice < qtdLed; indice++) {
            arrayLed[indice]->redraw();
        }
    }
#endif
}

/**
 * @brief Updates line charts
 */
void DisplayFK::updateLineChart() {
#ifdef DFK_LINECHART
    if (m_lineChartConfigured) {
        for (uint32_t indice = 0; indice < qtdLineChart; indice++) {
            arrayLineChart[indice]->redraw();
        }
    }
#endif
}

/**
 * @brief Updates vertical bars
 */
void DisplayFK::updateVBar() {
#ifdef DFK_VBAR
    if (m_vBarConfigured) {
        for (uint32_t indice = 0; indice < qtdVBar; indice++) {
            arrayVBar[indice]->redraw();
        }
    }
#endif
}

/**
 * @brief Updates analog viewers
 */
void DisplayFK::updateVAnalog() {
#ifdef DFK_VANALOG
    if (m_vAnalogConfigured) {
        for (uint32_t indice = 0; indice < qtdVAnalog; indice++) {
            arrayVAnalog[indice]->redraw();
        }
    }
#endif
}

/**
 * @brief Updates checkboxes
 */
void DisplayFK::updateCheckbox() {
#ifdef DFK_CHECKBOX
    if (m_checkboxConfigured) {
        for (uint32_t indice = 0; indice < qtdCheckbox; indice++) {
            arrayCheckbox[indice]->redraw();
        }
    }
#endif
}

/**
 * @brief Updates circle buttons
 */
void DisplayFK::updateCircleButton() {
#ifdef DFK_CIRCLEBTN
    if (m_circleButtonConfigured) {
        for (uint32_t indice = 0; indice < qtdCircleBtn; indice++) {
            arrayCircleBtn[indice]->redraw();
        }
    }
#endif
}

/**
 * @brief Updates horizontal sliders
 */
void DisplayFK::updateHSlider() {
#ifdef DFK_HSLIDER
    if (m_hSliderConfigured) {
        for (uint32_t indice = 0; indice < qtdHSlider; indice++) {
            arrayHSlider[indice]->redraw();
        }
    }
#endif
}

/**
 * @brief Updates radio groups
 */
void DisplayFK::updateRadioGroup() {
#ifdef DFK_RADIO
    if (m_radioGroupConfigured) {
        for (uint32_t indice = 0; indice < qtdRadioGroup; indice++) {
            arrayRadioGroup[indice]->redraw();
        }
    }
#endif
}

/**
 * @brief Updates rect buttons
 */
void DisplayFK::updateRectButton() {
#ifdef DFK_RECTBTN
    if (m_rectButtonConfigured) {
        for (uint32_t indice = 0; indice < qtdRectBtn; indice++) {
            arrayRectBtn[indice]->redraw();
        }
    }
#endif
}

/**
 * @brief Updates toggles
 */
void DisplayFK::updateToggle() {
#ifdef DFK_TOGGLE
    if (m_toggleConfigured) {
        for (uint32_t indice = 0; indice < qtdToggle; indice++) {
            arrayToggleBtn[indice]->redraw();
        }
    }
#endif
}


/**
 * @brief Updates images
 */
void DisplayFK::updateImage() {
#ifdef DFK_IMAGE
    if (m_imageConfigured) {
        for (uint32_t indice = 0; indice < qtdImage; indice++) {
            arrayImage[indice]->redraw();
        }
    }
#endif
}

/**
 * @brief Updates text buttons
 */
void DisplayFK::updateTextButton() {
#ifdef DFK_TEXTBUTTON
    if (m_textButtonConfigured) {
        for (uint32_t indice = 0; indice < qtdTextButton; indice++) {
            arrayTextButton[indice]->redraw();
        }
    }
#endif
}

/**
 * @brief Updates spinboxes
 */
void DisplayFK::updateSpinbox() {
#ifdef DFK_SPINBOX
    if (m_spinboxConfigured) {
        for (uint32_t indice = 0; indice < qtdSpinbox; indice++) {
            arraySpinbox[indice]->redraw();
        }
    }
#endif
}

/**
 * @brief Updates text boxes
 */
void DisplayFK::updateTextBox() {
#ifdef DFK_TEXTBOX
    if (m_textboxConfigured) {
        for (uint32_t indice = 0; indice < qtdTextBox; indice++) {
            arrayTextBox[indice]->redraw();
        }
    }
#endif
}

void DisplayFK::updateNumberBox(){
    #ifdef DFK_NUMBERBOX
    if (m_numberboxConfigured) {
        for (uint32_t indice = 0; indice < qtdNumberBox; indice++) {
            arrayNumberbox[indice]->redraw();
        }
    }
#endif
}