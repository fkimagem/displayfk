#include "displayfk.h"




#define RESET_WDT {if(m_enableWTD){esp_task_wdt_reset();}};


DisplayFK *DisplayFK::instance = nullptr;
bool DisplayFK::sdcardOK = false;
QueueHandle_t DisplayFK::xFilaLog;
logMessage_t DisplayFK::bufferLog[LOG_LENGTH];
uint8_t DisplayFK::logIndex = 0;
uint16_t DisplayFK::logFileCount = 1;
StringPool DisplayFK::stringPool;
const char *DisplayFK::TAG = "DisplayFK";
int DisplayFK::offsetSwipe = 50;


#ifdef DFK_SD

#endif


/**
 * @brief Enables or disables the watchdog timer
 */
void DisplayFK::changeWTD()
{
    if (m_enableWTD && m_watchdogInitialized){
		esp_err_t adicionou = esp_task_wdt_add(m_hndTaskEventoTouch);                    // Adiciona a tarefa atual (loop) ao watchdog
        if (adicionou == ESP_OK)
        {
            ESP_LOGI(TAG, "Task of Display added to watchdog with %i seconds", m_timeoutWTD);
        }
        else
        {
            ESP_LOGE(TAG, "Error to add on watchdog with %i seconds to task Display", m_timeoutWTD);
        }
    }
    else
    {
        ESP_LOGI(TAG, "Disabling watchdog");
        esp_err_t removeu = esp_task_wdt_delete(m_hndTaskEventoTouch);
        if (removeu == ESP_OK)
        {
            ESP_LOGI(TAG, "Successful to disable watchdog");
        }
        else
        {
            ESP_LOGE(TAG, "Error to disable watchdog");
        }
    }
}

#ifdef DFK_SD

/**
 * @brief Generates a unique filename for logs using string pool
 * @return Pointer to the generated filename (managed by string pool)
 */
const char* DisplayFK::generateNameFile() const {
    // Try to use string pool first with exception safety
    char* filename = const_cast<DisplayFK*>(this)->allocateStringSafe("generateNameFile");
    if (!filename) {
        ESP_LOGE(TAG, "String pool exhausted, falling back to dynamic allocation");
        // Fallback to dynamic allocation if pool is exhausted
        const size_t totalLength = 50; // "/" + number + "-" + 32 chars + ".txt\0"
        filename = new(std::nothrow) char[totalLength];
        if (!filename) {
            ESP_LOGE(TAG, "Failed to allocate memory for filename");
            return nullptr;
        }
    }

    // Generate the random string first
    char randomString[33]; // 32 chars + null terminator
    for (int i = 0; i < 32; ++i) {
        uint8_t letter = random(65, 90);
        randomString[i] = (char)letter;
    }
    randomString[32] = '\0';

    // Format the filename using snprintf for safety
    int written = snprintf(filename, STRING_POOL_SIZE, "/%u-%s.txt", logFileCount, randomString);
    if (written < 0 || written >= STRING_POOL_SIZE) {
        ESP_LOGE(TAG, "Failed to format filename");
        // If using pool, deallocate it
        stringPool.deallocate(filename);
        return nullptr;
    }

    ESP_LOGD(TAG, "Generated filename: %s", filename);
    logFileCount++;
    
    return filename;
}

/**
 * @brief Returns the current log filename
 * @return Current log filename
 */
const char *DisplayFK::getLogFileName() const
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
    if (m_checkboxConfigured) {
        ESP_LOGW(TAG, "Checkbox already configured");
        return;
    }
    
    // Robust input validation
    if (!validateArray(array, amount)) {
        ESP_LOGE(TAG, "Invalid checkbox array configuration");
        return;
    }
    
    
    
    m_checkboxConfigured = true;
    arrayCheckbox = array;
    qtdCheckbox = amount;
    ESP_LOGD(TAG, "Checkbox array configured with %d elements", amount);
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
    if (m_circleButtonConfigured) {
        ESP_LOGW(TAG, "CircleButton already configured");
        return;
    }
    
    // Robust input validation
    if (!validateArray(array, amount)) {
        ESP_LOGE(TAG, "Invalid circle button array configuration");
        return;
    }
    
    m_circleButtonConfigured = true;
    arrayCircleBtn = array;
    qtdCircleBtn = amount;
    ESP_LOGD(TAG, "Circle button array configured with %d elements", amount);
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
        ESP_LOGW(TAG, "Gauge already configured");
        return;
    }

    // Robust input validation
    if (!validateArray(array, amount)) {
        ESP_LOGE(TAG, "Invalid gauge array configuration");
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
        ESP_LOGW(TAG, "CircularBar already configured");
        return;
    }

    // Robust input validation
    if (!validateArray(array, amount)) {
        ESP_LOGE(TAG, "Invalid circular bar array configuration");
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
        ESP_LOGW(TAG, "HSlider already configured");
        return;
    }

    // Robust input validation
    if (!validateArray(array, amount)) {
        ESP_LOGE(TAG, "Invalid horizontal slider array configuration");
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
        ESP_LOGW(TAG, "Label already configured");
        return;
    }
    // Robust input validation
    if (!validateArray(array, amount)) {
        ESP_LOGE(TAG, "Invalid label array configuration");
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
        ESP_LOGW(TAG, "Led already configured");
        return;
    }

    // Robust input validation
    if (!validateArray(array, amount)) {
        ESP_LOGE(TAG, "Invalid LED array configuration");
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
        ESP_LOGW(TAG, "LineChart already configured");
        return;
    }
    // Robust input validation
    if (!validateArray(array, amount)) {
        ESP_LOGE(TAG, "Invalid line chart array configuration");
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
        ESP_LOGW(TAG, "Numberbox already configured");
        return;
    }
    // Robust input validation
    if (!validateArray(array, amount)) {
        ESP_LOGE(TAG, "Invalid number box array configuration");
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
        ESP_LOGE(TAG, "Numpad not configured");
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
        ESP_LOGW(TAG, "RadioGroup already configured");
        return;
    }
    // Robust input validation
    if (!validateArray(array, amount)) {
        ESP_LOGE(TAG, "Invalid radio group array configuration");
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
        ESP_LOGW(TAG, "RectButton already configured");
        return;
    }
    // Robust input validation
    if (!validateArray(array, amount)) {
        ESP_LOGE(TAG, "Invalid rect button array configuration");
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
        ESP_LOGW(TAG, "TextButton already configured");
        return;
    }
    // Robust input validation
    if (!validateArray(array, amount)) {
        ESP_LOGE(TAG, "Invalid text button array configuration");
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
        ESP_LOGW(TAG, "SpinBox already configured");
        return;
    }
    // Robust input validation
    if (!validateArray(array, amount)) {
        ESP_LOGE(TAG, "Invalid spin box array configuration");
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
        ESP_LOGW(TAG, "Textbox already configured");
        return;
    }
    // Robust input validation
    if (!validateArray(array, amount)) {
        ESP_LOGE(TAG, "Invalid text box array configuration");
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
        ESP_LOGE(TAG, "Keyboard not configured");
    }
}
#endif

#ifdef DFK_THERMOMETER

/**
 * @brief Configures the thermometer array
 * @param array Pointer to the thermometer array
 * @param amount Number of thermometers in the array
 */
void DisplayFK::setThermometer(Thermometer *array[], uint8_t amount)
{
    if (m_thermometerConfigured)
    {
        ESP_LOGW(TAG, "Thermometer already configured");
        return;
    }
    // Robust input validation
    if (!validateArray(array, amount)) {
        ESP_LOGE(TAG, "Invalid thermometer array configuration");
        return;
    }
    m_thermometerConfigured = (amount > 0 && array != nullptr);
    arrayThermometer = array;
    qtdThermometer = amount;
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
        ESP_LOGW(TAG, "Toggle already configured");
        return;
    }
    // Robust input validation
    if (!validateArray(array, amount)) {
        ESP_LOGE(TAG, "Invalid toggle button array configuration");
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
        ESP_LOGW(TAG, "TouchArea already configured");
        return;
    }
    // Robust input validation
    if (!validateArray(array, amount)) {
        ESP_LOGE(TAG, "Invalid touch area array configuration");
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
        ESP_LOGW(TAG, "VAnalog already configured");
        return;
    }
    // Robust input validation
    if (!validateArray(array, amount)) {
        ESP_LOGE(TAG, "Invalid analog viewer array configuration");
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
        ESP_LOGW(TAG, "VBar already configured");
        return;
    }
    // Robust input validation
    if (!validateArray(array, amount)) {
        ESP_LOGE(TAG, "Invalid vertical bar array configuration");
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
        ESP_LOGW(TAG, "Image already configured");
        return;
    }
    // Robust input validation
    if (!validateArray(array, amount)) {
        ESP_LOGE(TAG, "Invalid image array configuration");
        return;
    }
    m_imageConfigured = (amount > 0 && array != nullptr);
    arrayImage = array;
    qtdImage = amount;
    ESP_LOGD(TAG, "Configuring image: %i", amount);
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
    // Use exception-safe method
    return startSDSafe(pinCS, spiShared, hz);
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
        ESP_LOGE(TAG, "SD not configured");
        return;
    }
    if(dirname[0] != '/'){
        ESP_LOGE(TAG, "Path must start with /");
    }
    ESP_LOGD(TAG, "Listing directory: %s", dirname);

    File root = fs->open(dirname);
    if (!root)
    {
        ESP_LOGE(TAG, "Failed to open directory");
        return;
    }
    if (!root.isDirectory())
    {
        ESP_LOGE(TAG, "Not a directory");
        return;
    }

    File file = root.openNextFile();
    uint16_t indiceFile = 1;
    while (file)
    {
		RESET_WDT
        if (file.isDirectory())
        {
            ESP_LOGD(TAG, "\tDIR: %s", file.name());
            //Serial.println(file.name());
            if (levels)
            {
                const char *dirbase = "/";
                size_t len1 = strlen(dirbase);
                size_t len2 = strlen(file.name());

                char* resultado = new(std::nothrow) char[len1 + len2 + 1]; 
                if (!resultado) continue;  // falha na alocação

                strcpy(resultado, dirbase);  // copia str1
                strcat(resultado, file.name());  // concatena str2

                //String path = "/" + String(file.name());
                listFiles(fs, resultado, levels - 1);
                delete[] resultado;
            }
        }
        else
        {
            ESP_LOGD(TAG, "\t%i\tFILE: %s\tSIZE: %d", indiceFile, file.name(), file.size());
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
        ESP_LOGE(TAG, "SD not configured");
        return;
    }

    auto len = strlen(message);

    ESP_LOGD(TAG, "Appending %d bytes to file: %s", len, path);

    File file = fs->open(path, FILE_APPEND);
    if (!file)
    {
        ESP_LOGE(TAG, "Failed to open file for appending");
        if(SD.totalBytes() == 0){
            ESP_LOGE(TAG, "SD card empty");
            DisplayFK::sdcardOK = false;
        }
        return;
    }

    if (file.size() > LOG_MAX_SIZE)
    {
        ESP_LOGE(TAG, "Log too large");
        file.close();
        return;
    }

    if (len <= 1)
    {
        ESP_LOGW(TAG, "Empty message isnt appended");
        file.close();
        return;
    }

    if (file.println(message))
    {
        ESP_LOGD(TAG, "Message appended");
    }
    else
    {
        ESP_LOGE(TAG, "Append failed");
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
        ESP_LOGE(TAG, "SD not configured");
        return;
    }
    ESP_LOGD(TAG, "Reading file: %s", path);

    File file = fs->open(path);
    if (!file)
    {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return;
    }

    ESP_LOGD(TAG, "Read from file: ");
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
        ESP_LOGE(TAG, "SD not configured");
        return;
    }
    ESP_LOGD(TAG, "Creating Dir: %s", path);
    if (fs->mkdir(path))
    {
        ESP_LOGD(TAG, "Dir created");
    }
    else
    {
        ESP_LOGE(TAG, "mkdir failed");
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
        ESP_LOGE(TAG, "SD not configured");
        return;
    }

    ESP_LOGD(TAG, "Writing file: %s", path);

    File file = fs->open(path, FILE_WRITE);
  if(!file){
    ESP_LOGE(TAG, "Failed to open file for writing");
    return;
  }
  if(file.print(message)){
    ESP_LOGD(TAG, "File written");
  } else {
    ESP_LOGE(TAG, "Write failed");
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
        ESP_LOGE(TAG, "SD not configured");
        return;
    }
    if(amount == 0){
        ESP_LOGD(TAG, "No lines to append");
        return;
    }

    File file = fs->open(path, FILE_APPEND);
    if (!file)
    {
        ESP_LOGE(TAG, "Failed to open file for appending");
        if(SD.totalBytes() == 0){
            ESP_LOGE(TAG, "SD card empty");
            DisplayFK::sdcardOK = false;
        }
        return;
    }

    if (file.size() > LOG_MAX_SIZE)
    {
        ESP_LOGE(TAG, "Log too large");
        file.close();

        if(createNewFile){
            ESP_LOGD(TAG, "Creating new one");
            m_nameLogFile = generateNameFile();
            writeFile(WidgetBase::mySD, m_nameLogFile, "Start Log");
            file = fs->open(getLogFileName(), FILE_APPEND);
            if (!file){
                ESP_LOGE(TAG, "Failed to open new file created for appending");
                return;
            }
        }else{
            return;
        }
        
    }

    uint8_t success = 0;
    ESP_LOGD(TAG, "Starting write log with %d lines", amount);
    for(int i = 0; i < amount; i++){
        if (file.println(lines[i].line))
        {
            ESP_LOGD(TAG, "Message appended");
            success++;
        }
        else
        {
            ESP_LOGE(TAG, "Append failed");
        }
    }
    file.close();
    ESP_LOGD(TAG, "%d/%d lines appended", success, amount);
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
    // Robust input validation
    if (!data) {
        ESP_LOGE(TAG, "Null data pointer");
        return;
    }
    
    if (!validateInput(data, MAX_LINE_LENGTH - 1)) {
        ESP_LOGE(TAG, "Invalid log data");
        return;
    }
    
    if (DisplayFK::xFilaLog == NULL) {
        ESP_LOGE(TAG, "Log queue not initialized");
        return;
    }
    
    if (uxQueueSpacesAvailable(DisplayFK::xFilaLog) <= 0) {
        ESP_LOGE(TAG, "Log queue is full");
        return;
    }
    
    // Process log with safety
    const size_t lengthLine = strlen(data) + 1;
    const size_t max_length = std::min(lengthLine, static_cast<size_t>(MAX_LINE_LENGTH));
    
    logMessage_t message;
    strncpy(message.line, data, max_length - 1);
    message.line[max_length - 1] = '\0';  // Ensure null termination
    message.line_length = max_length;

    BaseType_t ret = xQueueSend(DisplayFK::xFilaLog, &message, 0);
    if (ret == pdTRUE) {
        ESP_LOGD(TAG, "Log message queued: %s", message.line);
    } else if (ret == errQUEUE_FULL) {
        ESP_LOGE(TAG, "Unable to send log data into the Queue");
    } else {
        ESP_LOGE(TAG, "Failed to queue log message: %d", ret);
    }
}

/**
 * @brief Constructor of the DisplayFK class
 */
DisplayFK::DisplayFK() 
    : m_hndTaskEventoTouch(nullptr)
    , m_runningTransaction(false)
    , m_configs()
    , m_timer(nullptr)
    , m_intervalMs(0)
    , m_xAutoClick(0)
    , m_yAutoClick(0)
    , m_simulateAutoClick(false)
    , m_debugTouch(false)
    , m_x0(0), m_y0(0), m_x1(0), m_y1(0)
    , m_invertXAxis(false), m_invertYAxis(false), m_swapAxis(false)
    , m_widthScreen(0), m_heightScreen(0), m_rotationScreen(0)
    , m_timeoutWTD(0)
    , m_enableWTD(false)
    , m_watchdogInitialized(false)
    , m_loopSemaphore(nullptr)
    , m_transactionSemaphore(nullptr)
    , m_lastTouchState(TouchEventType::NONE)
    , m_lastTouchSwipeDirection(TouchSwipeDirection::NONE)
{
    // Set instance pointer (must be done first)
    DisplayFK::instance = this;

    pressPoint.x = 0;
    pressPoint.y = 0;
    releasePoint.x = 0;
    releasePoint.y = 0;
    
    // Initialize fonts (static members)
    #if defined(USING_GRAPHIC_LIB)
    WidgetBase::fontBold = const_cast<GFXfont *>(&RobotoBold5pt7b);
    WidgetBase::fontNormal = const_cast<GFXfont *>(&RobotoBold5pt7b);
    #endif
    
    // Create critical resources with exception safety
    if (!createSemaphoreSafe()) {
        ESP_LOGE(TAG, "Failed to create loop semaphore safely");
        // Continue anyway, but mark as failed
    }
    
    // Initialize all widget configuration flags to false
    initializeWidgetFlags();
    
    // Validate initialization
    if (!validateInitialization()) {
        ESP_LOGE(TAG, "Constructor initialization failed");
    }
    
    ESP_LOGD(TAG, "DisplayFK constructor completed");
}

/**
 * @brief Initialize all widget configuration flags to false
 */
void DisplayFK::initializeWidgetFlags()
{
    // Initialize widget configuration flags based on compile-time defines
    #if defined(DFK_TOUCHAREA)
    m_touchAreaConfigured = false;
    arrayTouchArea = nullptr;
    qtdTouchArea = 0;
    #endif

    #ifdef DFK_CHECKBOX
    m_checkboxConfigured = false;
    arrayCheckbox = nullptr;
    qtdCheckbox = 0;
    #endif

    #ifdef DFK_CIRCLEBTN
    m_circleButtonConfigured = false;
    arrayCircleBtn = nullptr;
    qtdCircleBtn = 0;
    #endif

    #ifdef DFK_GAUGE
    m_gaugeConfigured = false;
    arrayGauge = nullptr;
    qtdGauge = 0;
    #endif

    #ifdef DFK_CIRCULARBAR
    m_circularBarConfigured = false;
    arrayCircularBar = nullptr;
    qtdCircularBar = 0;
    #endif

    #ifdef DFK_HSLIDER
    m_hSliderConfigured = false;
    arrayHSlider = nullptr;
    qtdHSlider = 0;
    #endif

    #ifdef DFK_LABEL
    m_labelConfigured = false;
    arrayLabel = nullptr;
    qtdLabel = 0;
    #endif

    #ifdef DFK_LED
    m_ledConfigured = false;
    arrayLed = nullptr;
    qtdLed = 0;
    #endif

    #ifdef DFK_LINECHART
    m_lineChartConfigured = false;
    arrayLineChart = nullptr;
    qtdLineChart = 0;
    #endif

    #ifdef DFK_RADIO
    m_radioGroupConfigured = false;
    arrayRadioGroup = nullptr;
    qtdRadioGroup = 0;
    #endif

    #ifdef DFK_RECTBTN
    m_rectButtonConfigured = false;
    arrayRectBtn = nullptr;
    qtdRectBtn = 0;
    #endif

    #ifdef DFK_TOGGLE
    m_toggleConfigured = false;
    arrayToggleBtn = nullptr;
    qtdToggle = 0;
    #endif

    #ifdef DFK_VBAR
    m_vBarConfigured = false;
    arrayVBar = nullptr;
    qtdVBar = 0;
    #endif

    #ifdef DFK_VANALOG
    m_vAnalogConfigured = false;
    arrayVAnalog = nullptr;
    qtdVAnalog = 0;
    #endif

    #ifdef DFK_TEXTBOX
    m_textboxConfigured = false;
    arrayTextBox = nullptr;
    qtdTextBox = 0;
    #endif

    #ifdef DFK_NUMBERBOX
    m_numberboxConfigured = false;
    arrayNumberbox = nullptr;
    qtdNumberBox = 0;
    #endif

    #ifdef DFK_IMAGE
    m_imageConfigured = false;
    arrayImage = nullptr;
    qtdImage = 0;
    #endif

    #ifdef DFK_TEXTBUTTON
    m_textButtonConfigured = false;
    arrayTextButton = nullptr;
    qtdTextButton = 0;
    #endif

    #ifdef DFK_SPINBOX
    m_spinboxConfigured = false;
    arraySpinbox = nullptr;
    qtdSpinbox = 0;
    #endif

    #ifdef DFK_THERMOMETER
    m_thermometerConfigured = false;
    arrayThermometer = nullptr;
    qtdThermometer = 0;
    #endif

    #ifdef DFK_EXTERNALINPUT
    m_inputExternalConfigured = false;
    #endif
    
    ESP_LOGD(TAG, "Widget flags initialized");
}

/**
 * @brief Validate that the constructor completed successfully
 * @return true if initialization was successful, false otherwise
 */
bool DisplayFK::validateInitialization() const
{
    // Check critical resources
    if (!m_loopSemaphore) {
        ESP_LOGE(TAG, "Loop semaphore not initialized");
        return false;
    }
    
    // Check instance pointer
    if (DisplayFK::instance != this) {
        ESP_LOGE(TAG, "Instance pointer not set correctly");
        return false;
    }
    
    ESP_LOGD(TAG, "Initialization validation passed");
    return true;
}

/**
 * @brief Clean up FreeRTOS resources
 */
void DisplayFK::cleanupFreeRTOSResources()
{
    ESP_LOGD(TAG, "Cleaning up FreeRTOS resources");
    
    // Stop and delete auto-click timer
    if (m_timer != nullptr) {
        if (xTimerIsTimerActive(m_timer)) {
            xTimerStop(m_timer, 0);
            ESP_LOGD(TAG, "Auto-click timer stopped");
        }
        xTimerDelete(m_timer, 0);
        m_timer = nullptr;
        ESP_LOGD(TAG, "Auto-click timer deleted");
    }
    
    // Delete loop semaphore
    if (m_loopSemaphore != nullptr) {
        vSemaphoreDelete(m_loopSemaphore);
        m_loopSemaphore = nullptr;
        ESP_LOGD(TAG, "Loop semaphore deleted");
    }
    
    // Delete transaction semaphore
    if (m_transactionSemaphore != nullptr) {
        vSemaphoreDelete(m_transactionSemaphore);
        m_transactionSemaphore = nullptr;
        ESP_LOGD(TAG, "Transaction semaphore deleted");
    }
    
    // Remove task from watchdog if initialized
    if (m_watchdogInitialized && m_hndTaskEventoTouch != nullptr) {
        esp_err_t result = esp_task_wdt_delete(m_hndTaskEventoTouch);
        if (result == ESP_OK) {
            ESP_LOGD(TAG, "Task removed from watchdog");
        } else {
            ESP_LOGE(TAG, "Failed to remove task from watchdog: %s", esp_err_to_name(result));
        }
        m_watchdogInitialized = false;
    }
    
    // Delete log queue
    if (xFilaLog != nullptr) {
        vQueueDelete(xFilaLog);
        xFilaLog = nullptr;
        ESP_LOGD(TAG, "Log queue deleted");
    }
    
    ESP_LOGD(TAG, "FreeRTOS resources cleanup completed");
}

/**
 * @brief Clean up dynamic memory allocations
 */
void DisplayFK::cleanupDynamicMemory()
{
    ESP_LOGD(TAG, "Cleaning up dynamic memory");
    
    #ifdef DFK_SD
    // Clean up log filename (owned by this class)
    if (m_nameLogFile != nullptr) {
        delete[] m_nameLogFile;
        m_nameLogFile = nullptr;
        ESP_LOGD(TAG, "Log filename memory freed");
    }
    #endif
    
    #ifdef DFK_EXTERNALINPUT
    // Clean up external keyboard (owned by this class)
    if (externalKeyboard != nullptr) {
        delete externalKeyboard;
        externalKeyboard = nullptr;
        ESP_LOGD(TAG, "External keyboard memory freed");
    }
    #endif
    
    ESP_LOGD(TAG, "Dynamic memory cleanup completed");
}

/**
 * @brief Clean up widget resources
 */
void DisplayFK::cleanupWidgets()
{
    ESP_LOGD(TAG, "Cleaning up widget resources");
    
    // Clean up smart pointers (automatic cleanup)
    #if defined(HAS_TOUCH)
    if (touchExterno) {
        touchExterno.reset();
        ESP_LOGD(TAG, "Touch screen smart pointer reset");
    }
    #endif
    
    #ifdef DFK_TEXTBOX
    if (keyboard) {
        keyboard.reset();
        ESP_LOGD(TAG, "Keyboard smart pointer reset");
    }
    #endif
    
    #ifdef DFK_NUMBERBOX
    if (numpad) {
        numpad.reset();
        ESP_LOGD(TAG, "Numpad smart pointer reset");
    }
    #endif
    
    // Reset all widget arrays and flags
    initializeWidgetFlags();
    
    ESP_LOGD(TAG, "Widget resources cleanup completed");
}

/**
 * @brief Validate that cleanup was successful
 * @return true if cleanup was successful, false otherwise
 */
bool DisplayFK::validateCleanup() const
{
    ESP_LOGD(TAG, "Validating cleanup");
    
    // Check FreeRTOS resources
    if (m_timer != nullptr) {
        ESP_LOGE(TAG, "Timer not properly cleaned up");
        return false;
    }
    
    if (m_loopSemaphore != nullptr) {
        ESP_LOGE(TAG, "Loop semaphore not properly cleaned up");
        return false;
    }
    
    if (xFilaLog != nullptr) {
        ESP_LOGE(TAG, "Log queue not properly cleaned up");
        return false;
    }
    
    // Check dynamic memory
    #ifdef DFK_SD
    if (m_nameLogFile != nullptr) {
        ESP_LOGE(TAG, "Log filename not properly cleaned up");
        return false;
    }
    #endif
    
    #ifdef DFK_EXTERNALINPUT
    if (externalKeyboard != nullptr) {
        ESP_LOGE(TAG, "External keyboard not properly cleaned up");
        return false;
    }
    #endif
    
    // Check smart pointers
    #if defined(HAS_TOUCH)
    if (touchExterno) {
        ESP_LOGE(TAG, "Touch screen smart pointer not properly cleaned up");
        return false;
    }
    #endif
    
    #ifdef DFK_TEXTBOX
    if (keyboard) {
        ESP_LOGE(TAG, "Keyboard smart pointer not properly cleaned up");
        return false;
    }
    #endif
    
    #ifdef DFK_NUMBERBOX
    if (numpad) {
        ESP_LOGE(TAG, "Numpad smart pointer not properly cleaned up");
        return false;
    }
    #endif
    
    // Check StringPool allocations
    uint32_t activeAllocations, totalAllocations, totalDeallocations;
    stringPool.getStats(activeAllocations, totalAllocations, totalDeallocations);
    
    if (activeAllocations > 0) {
        ESP_LOGE(TAG, "StringPool has %u active allocations not cleaned up", activeAllocations);
        return false;
    }
    
    ESP_LOGD(TAG, "Cleanup validation passed");
    return true;
}

/**
 * @brief Manual cleanup function for external use
 */
void DisplayFK::cleanup()
{
    ESP_LOGD(TAG, "Manual cleanup started");
    
    // Clean up resources in proper order
    cleanupFreeRTOSResources();
    cleanupDynamicMemory();
    cleanupWidgets();
    cleanupStringPool();
    
    // Reset static variables
    sdcardOK = false;
    logIndex = 0;
    logFileCount = 1;
    
    // Validate cleanup
    if (!validateCleanup()) {
        ESP_LOGE(TAG, "Manual cleanup validation failed");
    } else {
        ESP_LOGD(TAG, "Manual cleanup completed successfully");
    }
}

/**
 * @brief Blocks the main loop task
 */
void DisplayFK::blockLoopTask()
{
    // Tenta pegar o semáforo (sem timeout, bloqueia até conseguir)
    if (m_loopSemaphore != nullptr && WidgetBase::usingKeyboard == false)
        xSemaphoreTake(m_loopSemaphore, portMAX_DELAY);
}

/**
 * @brief Unblocks the main loop task
 */
void DisplayFK::freeLoopTask()
{
    // Libera o semáforo para permitir que a Task continue
    if (m_loopSemaphore != nullptr && WidgetBase::usingKeyboard == false)
        xSemaphoreGive(m_loopSemaphore);
}

void DisplayFK::startKeyboards(){
    #ifdef DFK_TEXTBOX
        if (!keyboard) {
            keyboard = std::make_unique<WKeyboard>();
            if (keyboard) {
                keyboard->setup();
            } else {
                ESP_LOGE(TAG, "Failed to allocate memory for keyboard");
            }
        }
    #endif
    #ifdef DFK_NUMBERBOX
        if (!numpad) {
            numpad = std::make_unique<Numpad>();
            if (numpad) {
                numpad->setup();
            } else {
                ESP_LOGE(TAG, "Failed to allocate memory for numpad");
            }
        }
    #endif
}

/**
 * @brief Destructor of the DisplayFK class
 */
DisplayFK::~DisplayFK()
{
    ESP_LOGD(TAG, "DisplayFK destructor started");
    
    // Clean up resources in proper order
    cleanupFreeRTOSResources();
    cleanupDynamicMemory();
    cleanupWidgets();
    cleanupStringPool();
    
    // Reset static variables
    sdcardOK = false;
    logIndex = 0;
    logFileCount = 1;
    
    // Validate cleanup
    if (!validateCleanup()) {
        ESP_LOGE(TAG, "Cleanup validation failed");
    }
    
    ESP_LOGD(TAG, "DisplayFK destructor completed");
}


/**
 * @brief Sets up the display
 */
void DisplayFK::setup(){
    ESP_LOGD(TAG, "Initializing queues");
    DisplayFK::xFilaLog = xQueueCreate(LOG_LENGTH, sizeof(logMessage_t));
    WidgetBase::xFilaCallback = xQueueCreate(5, sizeof(functionCB_t));
    if (DisplayFK::xFilaLog == NULL)
    {
        ESP_LOGE(TAG, "Fail while creating log queue");
    }else{
        ESP_LOGD(TAG, "Log queue created");
    }
    if (WidgetBase::xFilaCallback == NULL)
    {
        ESP_LOGE(TAG, "Fail while creating callback queue");
    }else{
        ESP_LOGD(TAG, "Callback queue created");
    }
}

/**
 * @brief Starts a drawing transaction
 */
void DisplayFK::startCustomDraw(){
    if (!m_transactionSemaphore) {
        ESP_LOGE(TAG, "Transaction semaphore not initialized");
        return;
    }
    
    // Wait until semaphore is available (may take time if loopTask is using it)
    if (xSemaphoreTake(m_transactionSemaphore, portMAX_DELAY) == pdTRUE) {
        m_runningTransaction = true;
        // Release semaphore immediately - we only needed it to safely set the flag
        xSemaphoreGive(m_transactionSemaphore);
    } else {
        ESP_LOGE(TAG, "Failed to acquire transaction semaphore");
    }
}

/**
 * @brief Finishes a drawing transaction
 */
void DisplayFK::finishCustomDraw(){
    if (!m_transactionSemaphore) {
        ESP_LOGE(TAG, "Transaction semaphore not initialized");
        return;
    }
    
    // Wait until semaphore is available (may take time if loopTask is using it)
    if (xSemaphoreTake(m_transactionSemaphore, portMAX_DELAY) == pdTRUE) {
        m_runningTransaction = false;
        // Release semaphore immediately - we only needed it to safely set the flag
        xSemaphoreGive(m_transactionSemaphore);
    } else {
        ESP_LOGE(TAG, "Failed to acquire transaction semaphore");
    }
}


#if defined(USING_GRAPHIC_LIB)
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
#endif

#if defined(HAS_TOUCH) && defined(DISP_DEFAULT)

/**
 * @brief Initializes the touch system
 * @param w Screen width
 * @param h Screen height
 * @param _rotation Screen rotation
 * @param _sharedSPI Pointer to the shared SPI interface
 */
void DisplayFK::startTouch(uint16_t w, uint16_t h, uint8_t _rotation, SPIClass *_sharedSPI = nullptr)
{
    ESP_LOGE(TAG, "startTouch not implemented.");
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

    //Serial.print("X0: "); Serial.print(m_x0); Serial.print(" X1: "); Serial.println(m_x1);
    //Serial.print("Y0: "); Serial.print(m_y0); Serial.print(" Y1: "); Serial.println(m_y1);
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
    // Robust input validation
    if (!validateDimensions(w, h)) {
        ESP_LOGE(TAG, "Invalid touch screen dimensions: %dx%d", w, h);
        return;
    }
    
    if (!validatePinNumber(pinCS)) {
        ESP_LOGE(TAG, "Invalid CS pin: %d", pinCS);
        return;
    }
    
    if (!validateFrequency(touchFrequency)) {
        ESP_LOGE(TAG, "Invalid touch frequency: %d Hz", touchFrequency);
        return;
    }
    
    if (!validateFrequency(displayFrequency)) {
        ESP_LOGE(TAG, "Invalid display frequency: %d Hz", displayFrequency);
        return;
    }
    
    if (!_sharedSPI) {
        ESP_LOGE(TAG, "SPI interface not provided");
        return;
    }
    
    if (!_objTFT) {
        ESP_LOGE(TAG, "TFT object not provided");
        return;
    }
    
    m_widthScreen = w;
    m_rotationScreen = _rotation;
    m_heightScreen = h;
    if(!touchExterno){
        touchExterno = std::make_unique<TouchScreen>();
        if (touchExterno)
        {
            touchExterno->setTouchCorners(m_x0, m_x1, m_y0, m_y1);
            touchExterno->setInvertAxis(m_invertXAxis, m_invertYAxis);
            touchExterno->setSwapAxis(m_swapAxis);
            touchExterno->startAsXPT2046(w, h, _rotation, -1, -1, -1, pinCS, _sharedSPI, _objTFT, touchFrequency, displayFrequency, displayPinCS);
        } else {
            ESP_LOGE(TAG, "Failed to allocate memory for TouchScreen");
        }
    }
}
void DisplayFK::startTouchXPT2046(uint16_t w, uint16_t h, uint8_t _rotation, int8_t pinSclk, int8_t pinMosi, int8_t pinMiso, int8_t pinCS, Arduino_GFX *_objTFT, int touchFrequency, int displayFrequency, int displayPinCS){
    m_widthScreen = w;
    m_rotationScreen = _rotation;
    m_heightScreen = h;
    if(!touchExterno){
        touchExterno = std::make_unique<TouchScreen>();
        if (touchExterno)
        {
            touchExterno->setTouchCorners(m_x0, m_x1, m_y0, m_y1);
            touchExterno->setInvertAxis(m_invertXAxis, m_invertYAxis);
            touchExterno->setSwapAxis(m_swapAxis);
            touchExterno->startAsXPT2046(w, h, _rotation, pinSclk, pinMosi, pinMiso, pinCS, nullptr, _objTFT, touchFrequency, displayFrequency, displayPinCS);
        } else {
            ESP_LOGE(TAG, "Failed to allocate memory for TouchScreen");
        }
    }
}
#elif defined (TOUCH_FT6236U)
void DisplayFK::startTouchFT6236U(uint16_t w, uint16_t h, uint8_t _rotation, int8_t pinSDA, int8_t pinSCL, uint8_t pinINT, int8_t pinRST){
    m_widthScreen = w;
    m_rotationScreen = _rotation;
    m_heightScreen = h;
    if(!touchExterno){
        touchExterno = std::make_unique<TouchScreen>();
        if (touchExterno)
        {
            touchExterno->setTouchCorners(m_x0, m_x1, m_y0, m_y1);
            touchExterno->setInvertAxis(m_invertXAxis, m_invertYAxis);
            touchExterno->setSwapAxis(m_swapAxis);
            touchExterno->startAsFT6236U(w, h, _rotation, pinSDA, pinSCL, pinINT, pinRST);
        } else {
            ESP_LOGE(TAG, "Failed to allocate memory for TouchScreen");
        }
    }
}
#elif defined (TOUCH_FT6336)
void DisplayFK::startTouchFT6336(uint16_t w, uint16_t h, uint8_t _rotation, int8_t pinSDA, int8_t pinSCL, int8_t pinINT, int8_t pinRST){
    m_widthScreen = w;
    m_rotationScreen = _rotation;
    m_heightScreen = h;
    if(!touchExterno){
        touchExterno = std::make_unique<TouchScreen>();
        if (touchExterno)
        {
            touchExterno->setTouchCorners(m_x0, m_x1, m_y0, m_y1);
            touchExterno->setInvertAxis(m_invertXAxis, m_invertYAxis);
            touchExterno->setSwapAxis(m_swapAxis);
            touchExterno->startAsFT6336(w, h, _rotation, pinSDA, pinSCL, pinINT, pinRST);
        } else {
            ESP_LOGE(TAG, "Failed to allocate memory for TouchScreen");
        }
    }
}
#elif defined (TOUCH_CST816)
void DisplayFK::startTouchCST816(uint16_t w, uint16_t h, uint8_t _rotation, int8_t pinSDA, int8_t pinSCL, int8_t pinINT, int8_t pinRST){
    m_widthScreen = w;
    m_rotationScreen = _rotation;
    m_heightScreen = h;
    if(!touchExterno){
        touchExterno = std::make_unique<TouchScreen>();
        if (touchExterno)
        {
            touchExterno->setTouchCorners(m_x0, m_x1, m_y0, m_y1);
            touchExterno->setInvertAxis(m_invertXAxis, m_invertYAxis);
            touchExterno->setSwapAxis(m_swapAxis);
            touchExterno->startAsCST816(w, h, _rotation, pinSDA, pinSCL, pinINT, pinRST);
        } else {
            ESP_LOGE(TAG, "Failed to allocate memory for TouchScreen");
        }
    }
}
#elif defined (TOUCH_GT911)
void DisplayFK::startTouchGT911(uint16_t w, uint16_t h, uint8_t _rotation, int8_t pinSDA, int8_t pinSCL, int8_t pinINT, int8_t pinRST){
    m_widthScreen = w;
    m_rotationScreen = _rotation;
    m_heightScreen = h;
    if(!touchExterno){
        touchExterno = std::make_unique<TouchScreen>();
        if (touchExterno)
        {
            touchExterno->setTouchCorners(m_x0, m_x1, m_y0, m_y1);
            touchExterno->setInvertAxis(m_invertXAxis, m_invertYAxis);
            touchExterno->setSwapAxis(m_swapAxis);
            touchExterno->startAsGT911(w, h, _rotation, pinSDA, pinSCL, pinINT, pinRST);
        } else {
            ESP_LOGE(TAG, "Failed to allocate memory for TouchScreen");
        }
    }
}
#elif defined (TOUCH_GSL3680)
void DisplayFK::startTouchGSL3680(uint16_t w, uint16_t h, uint8_t _rotation, int8_t pinSDA, int8_t pinSCL, int8_t pinINT, int8_t pinRST){
    m_widthScreen = w;
    m_rotationScreen = _rotation;
    m_heightScreen = h;
    if(!touchExterno){
        touchExterno = std::make_unique<TouchScreen>();
        if (touchExterno)
        {
            touchExterno->setTouchCorners(m_x0, m_x1, m_y0, m_y1);
            touchExterno->setInvertAxis(m_invertXAxis, m_invertYAxis);
            touchExterno->setSwapAxis(m_swapAxis);
            touchExterno->startAsGSL3680(w, h, _rotation, pinSDA, pinSCL, pinINT, pinRST);
        } else {
            ESP_LOGE(TAG, "Failed to allocate memory for TouchScreen");
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
    CHECK_TFT_VOID
    uint32_t startMillis = millis();

    WidgetBase::currentScreen = currentScreenIndex;
    ESP_LOGD(TAG, "Drawing widgets of screen:%i", WidgetBase::currentScreen);

#if defined(DFK_TOUCHAREA)
    if (m_touchAreaConfigured)
    {
        for ( uint32_t indice = 0; indice < (qtdTouchArea); indice++)
        {
            arrayTouchArea[indice]->redraw();
        }
    }
    else
    {
        ESP_LOGW(TAG, "Array of TouchArea not configured");
    }
#endif

#ifdef DFK_CHECKBOX
    if (m_checkboxConfigured)
    {
        for ( uint32_t indice = 0; indice < (qtdCheckbox); indice++)
        {
            arrayCheckbox[indice]->forceUpdate();
            arrayCheckbox[indice]->redraw();
        }
    }
    else
    {
        ESP_LOGW(TAG, "Array of Checkbox not configured");
    }
#endif
#ifdef DFK_CIRCLEBTN
    if (m_circleButtonConfigured)
    {
        for ( uint32_t indice = 0; indice < (qtdCircleBtn); indice++)
        {
            arrayCircleBtn[indice]->forceUpdate();
            arrayCircleBtn[indice]->redraw();
        }
    }
    else
    {
        ESP_LOGW(TAG, "Array of CircleButton not configured");
    }
#endif
#ifdef DFK_SPINBOX
    if (m_spinboxConfigured)
    {
        for ( uint32_t indice = 0; indice < (qtdSpinbox); indice++)
        {
            arraySpinbox[indice]->drawBackground();
            arraySpinbox[indice]->redraw();
        }
    }
    else
    {
        ESP_LOGW(TAG, "Array of Spinbox not configured");
    }
#endif
#ifdef DFK_GAUGE
    if (m_gaugeConfigured)
    {
        for ( uint32_t indice = 0; indice < (qtdGauge); indice++)
        {
            arrayGauge[indice]->forceUpdate();
            arrayGauge[indice]->drawBackground();
        }
    }
    else
    {
        ESP_LOGW(TAG, "Array of Gauge not configured");
    }

#endif

#ifdef DFK_CIRCULARBAR
    if (m_circularBarConfigured)
    {
        for ( uint32_t indice = 0; indice < (qtdCircularBar); indice++)
        {
            arrayCircularBar[indice]->forceUpdate();
            arrayCircularBar[indice]->drawBackground();
        }
    }
    else
    {
        ESP_LOGW(TAG, "Array of CircularBar not configured");
    }
#endif

#ifdef DFK_HSLIDER
    if (m_hSliderConfigured)
    {
        for ( uint32_t indice = 0; indice < (qtdHSlider); indice++)
        {
            arrayHSlider[indice]->forceUpdate();
            arrayHSlider[indice]->drawBackground();
        }
    }
    else
    {
        ESP_LOGW(TAG, "Array of HSlider not configured");
    }
#endif
#ifdef DFK_LABEL
    if (m_labelConfigured)
    {
        for ( uint32_t indice = 0; indice < (qtdLabel); indice++)
        {
            arrayLabel[indice]->forceUpdate();
            arrayLabel[indice]->redraw();
        }
    }
    else
    {
        ESP_LOGW(TAG, "Array of Label not configured");
    }
#endif
#ifdef DFK_LED
    if (m_ledConfigured)
    {
        for ( uint32_t indice = 0; indice < (qtdLed); indice++)
        {
            arrayLed[indice]->forceUpdate();
            arrayLed[indice]->drawBackground();
            arrayLed[indice]->redraw();
        }
    }
    else
    {
        ESP_LOGW(TAG, "Array of Led not configured");
    }
#endif
#ifdef DFK_LINECHART
    if (m_lineChartConfigured)
    {
        for ( uint32_t indice = 0; indice < (qtdLineChart); indice++)
        {
            arrayLineChart[indice]->forceUpdate();
            arrayLineChart[indice]->drawBackground();
        }
    }
    else
    {
        ESP_LOGW(TAG, "Array of LineChart not configured");
    }
#endif
#ifdef DFK_RADIO
    if (m_radioGroupConfigured)
    {
        for ( uint32_t indice = 0; indice < (qtdRadioGroup); indice++)
        {
            arrayRadioGroup[indice]->forceUpdate();
            arrayRadioGroup[indice]->redraw();
        }
    }
    else
    {
        ESP_LOGW(TAG, "Array of RadioGroup not configured");
    }
#endif
#ifdef DFK_RECTBTN
    if (m_rectButtonConfigured)
    {
        for ( uint32_t indice = 0; indice < (qtdRectBtn); indice++)
        {
            arrayRectBtn[indice]->forceUpdate();
            arrayRectBtn[indice]->redraw();
        }
    }
    else
    {
        ESP_LOGW(TAG, "Array of RectButton not configured");
    }
#endif
#ifdef DFK_TOGGLE
    if (m_toggleConfigured)
    {
        for ( uint32_t indice = 0; indice < (qtdToggle); indice++)
        {
            arrayToggleBtn[indice]->forceUpdate();
            arrayToggleBtn[indice]->redraw();
        }
    }
    else
    {
        ESP_LOGW(TAG, "Array of Toggle not configured");
    }
#endif

#ifdef DFK_TEXTBUTTON
    if (m_textButtonConfigured)
    {
        for ( uint32_t indice = 0; indice < (qtdTextButton); indice++)
        {
            arrayTextButton[indice]->redraw();
        }
    }
    else
    {
        ESP_LOGW(TAG, "Array of TextButton not configured");
    }
#endif

#ifdef DFK_VBAR
    if (m_vBarConfigured)
    {
        for ( uint32_t indice = 0; indice < (qtdVBar); indice++)
        {
            arrayVBar[indice]->forceUpdate();
            arrayVBar[indice]->drawBackground();
        }
    }
    else
    {
        ESP_LOGW(TAG, "Array of VBar not configured");
    }
#endif

#ifdef DFK_THERMOMETER
    if (m_thermometerConfigured)
    {
        for ( uint32_t indice = 0; indice < (qtdThermometer); indice++)
        {
            arrayThermometer[indice]->forceUpdate();
            arrayThermometer[indice]->drawBackground();
        }
    }
    else
    {
        ESP_LOGW(TAG, "Array of Thermometer not configured");
    }
#endif


#ifdef DFK_VANALOG
    if (m_vAnalogConfigured)
    {
        for ( uint32_t indice = 0; indice < (qtdVAnalog); indice++)
        {
            arrayVAnalog[indice]->forceUpdate();
            arrayVAnalog[indice]->drawBackground();
        }
    }
    else
    {
        ESP_LOGW(TAG, "Array of VAnalog not configured");
    }
#endif
#ifdef DFK_TEXTBOX
    if (m_textboxConfigured)
    {
        for ( uint32_t indice = 0; indice < (qtdTextBox); indice++)
        {
            arrayTextBox[indice]->forceUpdate();
            arrayTextBox[indice]->redraw();
        }
    }
    else
    {
        ESP_LOGW(TAG, "Array of Textbox not configured");
    }
#endif
#ifdef DFK_NUMBERBOX
    if (m_numberboxConfigured)
    {
        for ( uint32_t indice = 0; indice < (qtdNumberBox); indice++)
        {
            arrayNumberbox[indice]->forceUpdate();
            arrayNumberbox[indice]->redraw();
        }
    }
    else
    {
        ESP_LOGW(TAG, "Array of Numberbox not configured");
    }
#endif
#ifdef DFK_IMAGE
    if (m_imageConfigured)
    {
        for ( uint32_t indice = 0; indice < (qtdImage); indice++)
        {
            arrayImage[indice]->forceUpdate();
			arrayImage[indice]->drawBackground();
            arrayImage[indice]->draw();
            //arrayImage[indice].redraw();
        }
    }
    else
    {
        ESP_LOGW(TAG, "Array of Image not configured");
    }
#endif

 startMillis = millis() - startMillis;
 Serial.printf("drawWidgetsOnScreen: %lu ms\n", startMillis);
}

/**
 * @brief Creates the event processing task
 */
void DisplayFK::createTask(bool enableWatchdog, uint16_t timeout_s)
{
    ESP_LOGD(TAG, "Creating task");

	m_timeoutWTD = timeout_s;
	m_enableWTD = enableWatchdog;
	
    this->setup();
    this->startKeyboards();

    /*
    ESP_ARDUINO_VERSION;

    ESP_ARDUINO_VERSION_MAJOR;

    ESP_ARDUINO_VERSION_MINOR;

    ESP_ARDUINO_VERSION_PATCH;

    ESP_ARDUINO_VERSION_VAL(1,2,3);
    */

    /*#if defined(ESP_ARDUINO_VERSION)
    Serial.printf("ESP_ARDUINO_VERSION: %i\n", ESP_ARDUINO_VERSION);
    #endif
    #if defined(ESP_ARDUINO_VERSION_MAJOR)
    Serial.printf("ESP_ARDUINO_VERSION_MAJOR: %i\n", ESP_ARDUINO_VERSION_MAJOR);
    #if ESP_ARDUINO_VERSION_MAJOR == 3
    #pragma message "ESP_ARDUINO_VERSION_MAJOR is 3.x"
    #elif ESP_ARDUINO_VERSION_MAJOR == 2
    #pragma message "ESP_ARDUINO_VERSION_MAJOR is 2.x"
    #else
    #pragma message "ESP_ARDUINO_VERSION_MAJOR is unknown"
    #error "ESP_ARDUINO_VERSION_MAJOR is unknown"
    #endif

    #endif
    #if defined(ESP_ARDUINO_VERSION_MINOR)
    Serial.printf("ESP_ARDUINO_VERSION_MINOR: %i\n", ESP_ARDUINO_VERSION_MINOR);
    #endif
    #if defined(ESP_ARDUINO_VERSION_PATCH)
    Serial.printf("ESP_ARDUINO_VERSION_PATCH: %i\n", ESP_ARDUINO_VERSION_PATCH);
    #endif*/

    if(m_enableWTD){
		esp_task_wdt_deinit();
        #if ESP_ARDUINO_VERSION_MAJOR == 2
            esp_err_t iniciou = esp_task_wdt_init(m_timeoutWTD * 1000, true);
        #elif ESP_ARDUINO_VERSION_MAJOR == 3
            esp_task_wdt_config_t twdt_config = {
                .timeout_ms = static_cast<uint32_t>(m_timeoutWTD * 1000), // timeout em ms m_timeoutWTD * 1000, // timeout em ms
                .idle_core_mask = (1 << portNUM_PROCESSORS) - 1, // aplica a todos os cores
                .trigger_panic = true,
            };

            esp_err_t iniciou = esp_task_wdt_init(&twdt_config);  // timeout em segundos, trigger_panic = true
        #endif


        #if (ESP_ARDUINO_VERSION_MAJOR == 2) || (ESP_ARDUINO_VERSION_MAJOR == 3)
        m_watchdogInitialized = (iniciou == ESP_OK);
        if(iniciou == ESP_OK){
            ESP_LOGD(TAG, "WDT initialized with %i seconds timeout", m_timeoutWTD);
            esp_task_wdt_add(NULL);
        }else{
            ESP_LOGE(TAG, "WDT initialization failed");
        }
        #endif
		
    }else{
        esp_task_wdt_delete(NULL);
        esp_err_t deinit = esp_task_wdt_deinit();
        if(deinit == ESP_OK){
            ESP_LOGD(TAG, "WDT deinitialized");
        }else{
            ESP_LOGE(TAG, "WDT deinitialization failed");
        }
    }


    BaseType_t xRetorno = pdFAIL;
    xRetorno = xTaskCreatePinnedToCore(DisplayFK::TaskEventoTouch, "TaskEventoTouch", configMINIMAL_STACK_SIZE + 3048, this, 1, &m_hndTaskEventoTouch, 0);
    if (xRetorno == pdFAIL)
    {
        ESP_LOGE(TAG, "Cant create task to read touch or draw widgets");
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
                #if defined(USING_GRAPHIC_LIB)
                WidgetBase::objTFT->drawPixel(posX, posY, _cor);
                #elif defined(DISP_U8G2)
                WidgetBase::objTFT->drawPixel(posX, posY);
                #endif
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
            ESP_LOGE(TAG, "Log buffer overflow");
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
        ESP_LOGE(TAG, "SD card not available for log writing");
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

    if(m_debugTouch){
        Serial.printf("Touch pressed at [%i, %i]\n", xTouch, yTouch);
        CHECK_TFT_VOID
        WidgetBase::objTFT->fillCircle(xTouch, yTouch, 2, CFK_FUCHSIA);
    }
    
    if (WidgetBase::usingKeyboard) return;

    // Process other touchable widgets
    processTouchableWidgets(xTouch, yTouch);
}

void DisplayFK::processTouchStatus(bool hasTouch, uint16_t xTouch, uint16_t yTouch){
    if(hasTouch && m_lastTouchState == TouchEventType::NONE){
        m_lastTouchState = TouchEventType::TOUCH_DOWN;
    }else if(hasTouch && m_lastTouchState == TouchEventType::TOUCH_DOWN){
        m_lastTouchState = TouchEventType::TOUCH_HOLD;
    }else if(!hasTouch && (m_lastTouchState == TouchEventType::TOUCH_DOWN || m_lastTouchState == TouchEventType::TOUCH_HOLD)){
        m_lastTouchState = TouchEventType::TOUCH_UP;
    }else if(!hasTouch && m_lastTouchState == TouchEventType::TOUCH_UP){
        m_lastTouchState = TouchEventType::NONE;
    }

    if(m_lastTouchState == TouchEventType::TOUCH_DOWN){
        Serial.println("---------------------- APERTEI");
        pressPoint.x = xTouch;
        pressPoint.y = yTouch;
        releasePoint.x = xTouch;
        releasePoint.y = yTouch;

    }else if(m_lastTouchState == TouchEventType::TOUCH_UP){
        Serial.println("---------------------- SOLTEI");
        // calculate diff between pressPoint and releasePoint for two coords, calculate abs value for this diff, check if is greatter than offsetSwipe,f
        int absX = abs((int)releasePoint.x - (int)pressPoint.x);
        int absY = abs((int)releasePoint.y - (int)pressPoint.y);

        if(absX > offsetSwipe){
            if(releasePoint.x > pressPoint.x){
                m_lastTouchSwipeDirection = TouchSwipeDirection::RIGHT;
            }else{
                m_lastTouchSwipeDirection = TouchSwipeDirection::LEFT;
            }
        }else if(absY > offsetSwipe){
            if(releasePoint.y > pressPoint.y){
                m_lastTouchSwipeDirection = TouchSwipeDirection::DOWN;
            }else{
                m_lastTouchSwipeDirection = TouchSwipeDirection::UP;
            }
        }else{
            m_lastTouchSwipeDirection = TouchSwipeDirection::NONE;
        }
        Serial.printf("Swipe direction: %i\n", (int)m_lastTouchSwipeDirection);

    }else if(m_lastTouchState == TouchEventType::TOUCH_HOLD){
        releasePoint.x = xTouch;
        releasePoint.y = yTouch;
        //Serial.println("---------------------- APERTANDO");
    }
}

/**
 * @brief Processes touchable widgets
 * @param xTouch Touch X position
 * @param yTouch Touch Y position
 */
void DisplayFK::processTouchableWidgets(uint16_t xTouch, uint16_t yTouch) {
    ESP_LOGD(TAG, "Processing touchable widgets");
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
 * @brief Processes touch in checkbox (optimized for current screen)
 * @param xTouch Touch X position
 * @param yTouch Touch Y position
 */
void DisplayFK::processCheckboxTouch(uint16_t xTouch, uint16_t yTouch) {
#ifdef DFK_CHECKBOX
    if(!m_checkboxConfigured || !arrayCheckbox){
        return;
    }
    
    for (uint32_t indice = 0; indice < qtdCheckbox; indice++) {
        // Skip widgets not on current screen
        if (!arrayCheckbox[indice]->showingMyScreen()) continue;
        
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
            if (!arrayCircleBtn[indice]->showingMyScreen()) continue;
            
            if (arrayCircleBtn[indice]->detectTouch(&xTouch, &yTouch)) {
                functionCB_t cb = arrayCircleBtn[indice]->getCallbackFunc();
                WidgetBase::addCallback(cb, WidgetBase::CallbackOrigin::TOUCH);
                return;
            }
        }
#endif
}

/**
 * @brief Processes touch in horizontal slider (optimized for current screen)
 * @param xTouch Touch X position
 * @param yTouch Touch Y position
 */
void DisplayFK::processHSliderTouch(uint16_t xTouch, uint16_t yTouch) {
#ifdef DFK_HSLIDER
    if(!m_hSliderConfigured || !arrayHSlider){
        return;
    }
    
    for (uint32_t indice = 0; indice < qtdHSlider; indice++) {
        // Skip widgets not on current screen
        if (!arrayHSlider[indice]->showingMyScreen()) continue;
        
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
            if (!arrayRectBtn[indice]->showingMyScreen()) continue;
            
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
            if (!arrayImage[indice]->showingMyScreen()) continue;

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
            if (!arraySpinbox[indice]->showingMyScreen()) continue;

            if (arraySpinbox[indice]->detectTouch(&xTouch, &yTouch)) {
                functionCB_t cb = arraySpinbox[indice]->getCallbackFunc();
                WidgetBase::addCallback(cb, WidgetBase::CallbackOrigin::TOUCH);
                return;
            }
        }
    
#endif
}

/**
 * @brief Processes touch in toggle button (optimized for current screen)
 * @param xTouch Touch X position
 * @param yTouch Touch Y position
 */
void DisplayFK::processToggleTouch(uint16_t xTouch, uint16_t yTouch) {
#ifdef DFK_TOGGLE
    if(!m_toggleConfigured || !arrayToggleBtn){
        return;
    }

    for (uint32_t indice = 0; indice < qtdToggle; indice++) {
        // Skip widgets not on current screen
        if (!arrayToggleBtn[indice]->showingMyScreen()) continue;
        
        if (arrayToggleBtn[indice]->detectTouch(&xTouch, &yTouch)) {
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
        if (!arrayTextButton[indice]->showingMyScreen()) continue;

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
    if(!touchExterno){
        return;
    }
        for (uint32_t indice = 0; indice < qtdTextBox; indice++) {
            if (arrayTextBox[indice] == nullptr) {
                ESP_LOGE(TAG, "TextBox pointer is null");
                continue;
            }

            if (!arrayTextBox[indice]->showingMyScreen()) continue;

            if (arrayTextBox[indice]->detectTouch(&xTouch, &yTouch)) {
                keyboard->open(arrayTextBox[indice]);
                PressedKeyType pressedKey = PressedKeyType::NONE;

                while (WidgetBase::usingKeyboard) {
                    uint16_t internal_xTouch = 0;
                    uint16_t internal_yTouch = 0;
                    int internal_zPressure = 0;
                    
                    bool hasTouch = touchExterno && (touchExterno->getTouch(&internal_xTouch, &internal_yTouch, &internal_zPressure));
                    if (hasTouch && keyboard->detectTouch(&internal_xTouch, &internal_yTouch, &pressedKey)) {
                        if (pressedKey == PressedKeyType::RETURN) {
                            WidgetBase::loadScreen = keyboard->m_field->parentScreen;
                            keyboard->close();
                            functionCB_t cb = arrayTextBox[indice]->getCallbackFunc();
                            WidgetBase::addCallback(cb, WidgetBase::CallbackOrigin::TOUCH);
                            return;
                        }
                    }
					RESET_WDT
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
        if(!touchExterno){
            return;
        }

        for (uint32_t indice = 0; indice < qtdNumberBox; indice++) {
            if (arrayNumberbox[indice] == nullptr) {
                ESP_LOGE(TAG, "NumberBox pointer is null");
                continue;
            }

            if (!arrayNumberbox[indice]->showingMyScreen()) continue;

            if (arrayNumberbox[indice]->detectTouch(&xTouch, &yTouch)) {
                numpad->open(arrayNumberbox[indice]);
                PressedKeyType pressedKey = PressedKeyType::NONE;

                while (WidgetBase::usingKeyboard) {
                    uint16_t internal_xTouch = 0;
                    uint16_t internal_yTouch = 0;
                    int internal_zPressure = 0;
                    bool hasTouch = touchExterno && (touchExterno->getTouch(&internal_xTouch, &internal_yTouch, &internal_zPressure));
                    if (hasTouch && numpad->detectTouch(&internal_xTouch, &internal_yTouch, &pressedKey)) {
                        if (pressedKey == PressedKeyType::RETURN) {
                            if (numpad->m_field && numpad->m_field->parentScreen) {
                                WidgetBase::loadScreen = numpad->m_field->parentScreen;
                            }
                            numpad->close();
                            functionCB_t cb = arrayNumberbox[indice]->getCallbackFunc();
                            WidgetBase::addCallback(cb, WidgetBase::CallbackOrigin::TOUCH);
                            return;
                        }
                    }
					RESET_WDT
                    vTaskDelay(pdMS_TO_TICKS(10));
                }
            }
        }
#endif
}

/**
 * @brief Updates screen widgets (optimized for current screen only)
 */
void DisplayFK::updateWidgets() {
    if (m_runningTransaction) return;
    
    // Only process widgets from current screen
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
    updateImage();
    //updateTextButton();
    updateSpinbox();
    updateNumberBox();
    updateTextBox();
    updateThermometer();
}


void DisplayFK::refreshScreen() {
    if(m_lastScreen) {
        Serial.println("Reloading screen");
        WidgetBase::loadScreen = m_lastScreen;
    }
}

void DisplayFK::loadScreen(functionLoadScreen_t screen) {
    WidgetBase::loadScreen = screen;
}

#if defined(DISP_DEFAULT)
void DisplayFK::setDrawObject(Arduino_GFX *objTFT){
    WidgetBase::objTFT = objTFT;
}
#elif defined(DISP_PCD8544)
void DisplayFK::setDrawObject(Adafruit_PCD8544 *objTFT){
    WidgetBase::objTFT = objTFT;
}
#elif defined(DISP_SSD1306)
void DisplayFK::setDrawObject(Adafruit_SSD1306 *objTFT){
    WidgetBase::objTFT = objTFT;
}
#elif defined(DISP_U8G2)
void DisplayFK::setDrawObject(U8G2 *objTFT){
    WidgetBase::objTFT = objTFT;
}
#endif


/**
 * @brief Main task loop
 */
void DisplayFK::loopTask() {

    uint32_t startTime = millis();
    bool semaphoreAcquired = false;

    // Try to acquire the transaction semaphore with 10ms timeout
    if (!m_transactionSemaphore) {
        ESP_LOGE(TAG, "Transaction semaphore not initialized in loopTask");
        vTaskDelay(pdMS_TO_TICKS(1));
        return;
    }
    
    if (xSemaphoreTake(m_transactionSemaphore, pdMS_TO_TICKS(1)) != pdTRUE) {
        // Could not acquire semaphore within 10ms, return early
        ESP_LOGE(TAG, "Could not acquire semaphore within 10ms, return early");
        return;
    }
    
    semaphoreAcquired = true;

    if(m_runningTransaction){
        xSemaphoreGive(m_transactionSemaphore);
        vTaskDelay(pdMS_TO_TICKS(1));
        return;
    }
    
    // Release the semaphore after checking - don't hold it during screen loading
    xSemaphoreGive(m_transactionSemaphore);
    semaphoreAcquired = false;
    
    
    // Process screen loading
    if (WidgetBase::loadScreen) {
        m_lastScreen = WidgetBase::loadScreen;
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
#if defined(HAS_TOUCH)
    uint16_t xTouch, yTouch;
    int zPressure;
    bool hasTouch = false;
    uint8_t gesture = 0;

#if defined(DISP_DEFAULT)
if(touchExterno){
    hasTouch = touchExterno && (touchExterno->getTouch(&xTouch, &yTouch, &zPressure));
    gesture = hasTouch ? touchExterno->getGesture() : 0;
}
#else
    hasTouch = false;
    gesture = 0;
#endif

    if(m_simulateAutoClick){
        m_simulateAutoClick = false;
        hasTouch = true;
        xTouch = m_xAutoClick;
        yTouch = m_yAutoClick;
        gesture = 0;
        Serial.printf("Simulating click at (%i, %i)\n", xTouch, yTouch);
    }

    processTouchStatus(hasTouch, xTouch, yTouch);

    if (hasTouch) {
        
        processTouchEvent(xTouch, yTouch, zPressure, gesture);
    }
#endif

    // Update widgets
    updateWidgets();

    #if defined(DISP_PCD8544)
    CHECK_TFT_VOID
    WidgetBase::objTFT->display();
    #endif

    // Calculate and log execution time
    startTime = millis() - startTime;
    
    if(startTime > 20){
        Serial.printf("Time to loopTask: %lu ms\n", startTime);
    }

    //vTaskDelay(pdMS_TO_TICKS(1));
}

/**
 * @brief Touch event task callback function
 * @param pvParameters Task parameters
 */
void DisplayFK::TaskEventoTouch(void *pvParameters)
{
    //(void)pvParameters;
    DisplayFK::instance->changeWTD();

    //vTaskDelay(pdMS_TO_TICKS(3000));
    ESP_LOGD(TAG, "TaskEventoTouch created");
    // const TickType_t xDelay = 10 / portTICK_PERIOD_MS;

    for (;;)
    {
        //if (xSemaphoreTake(DisplayFK::instance->m_loopSemaphore, pdMS_TO_TICKS(100)) == pdTRUE){
            if(DisplayFK::instance){
                DisplayFK::instance->loopTask();
            }
            // Libera o semáforo novamente para próxima iteração
        //    xSemaphoreGive(DisplayFK::instance->m_loopSemaphore);
        //}
        if(DisplayFK::instance->m_enableWTD){esp_task_wdt_reset();}
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

/**
 * @brief Returns the touch event task handle
 * @return Task handle
 */
TaskHandle_t DisplayFK::getTaskHandle() const
{
    return m_hndTaskEventoTouch;
}

/** @brief Enables touch log */
void DisplayFK::enableTouchLog(){
    m_debugTouch = true;
    #if defined(HAS_TOUCH)
    if(touchExterno){
        touchExterno->setLogMessages(true);
    }
    #endif
    
}

/** @brief Disables touch log */
void DisplayFK::disableTouchLog(){
    m_debugTouch = false;
    #if defined(HAS_TOUCH)
    if(touchExterno){
        touchExterno->setLogMessages(false);
    }
    #endif
}

#if defined(TOUCH_XPT2046) && defined(HAS_TOUCH)

/**
 * @brief Checks and performs touch calibration
 */
void DisplayFK::checkCalibration() const
{
#if defined(HAS_TOUCH)
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
        //WidgetBase::objTFT->setRotation(0);
        WidgetBase::objTFT->setRotation(m_rotationScreen);
        uint16_t widthScreen = m_widthScreen;// WidgetBase::objTFT->width();
        uint16_t heightScreen = m_heightScreen;// WidgetBase::objTFT->height();
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
    WidgetBase::objTFT->fillScreen(0xffff);
    WidgetBase::objTFT->setCursor(WidgetBase::objTFT->width() / 2, WidgetBase::objTFT->height() / 4);
    WidgetBase::objTFT->print("Calibration done");
    // WidgetBase::objTFT->setTextPadding(WidgetBase::objTFT->textWidth(msg));
    // WidgetBase::objTFT->drawString(msg, WidgetBase::objTFT->width() / 2, WidgetBase::objTFT->height() / 2, 4);
    vTaskDelay(pdMS_TO_TICKS(1000));
    WidgetBase::objTFT->fillScreen(CFK_BLACK);
    WidgetBase::objTFT->setRotation(touchExterno->getRotation());
#else
    ESP_LOGW(TAG, "TOUCH_CS wasnt defined on TFT_eSPI library.");
#endif
}

/**
 * @brief Forces a new touch calibration
 */
void DisplayFK::recalibrate() const
{
    m_configs.begin("iniciais", false);
    // bool jaCalibrado = configs.getBool("jaCalibrado", false);
    m_configs.putBool("jaCalibrado", false);
    m_configs.end();
    // ESP.restart();
}
#endif

#if defined(USING_GRAPHIC_LIB)
void DisplayFK::printText(const char* _texto, uint16_t _x, uint16_t _y, uint8_t _datum, uint16_t _colorText, uint16_t _colorPadding, const GFXfont* _font){
    CHECK_TFT_VOID
    if(_font){
        WidgetBase::objTFT->setFont(_font);
    }else{
        WidgetBase::objTFT->setFont((GFXfont *)0);
    }
    
    
    WidgetBase::objTFT->setTextColor(_colorText, _colorPadding);
    WidgetBase::recalculateTextPosition(_texto, &_x, &_y, _datum);
    WidgetBase::objTFT->setCursor(_x, _y);
    WidgetBase::objTFT->print(_texto);
    WidgetBase::objTFT->setFont((GFXfont *)0);
}
#endif
/**
 * @brief Updates circular bars (optimized for current screen)
 */
void DisplayFK::updateCircularBar() {
#ifdef DFK_CIRCULARBAR
    if (!m_circularBarConfigured) return;
    
    for (uint32_t indice = 0; indice < qtdCircularBar; indice++) {
        // Skip widgets not on current screen
        if (!arrayCircularBar[indice]->showingMyScreen()) continue;
        
        arrayCircularBar[indice]->redraw();
    }
#endif
}

/**
 * @brief Updates gauges (optimized for current screen)
 */
void DisplayFK::updateGauge() {
#ifdef DFK_GAUGE
    if (!m_gaugeConfigured) return;
    
    for (uint32_t indice = 0; indice < qtdGauge; indice++) {
        // Skip widgets not on current screen
        if (!arrayGauge[indice]->showingMyScreen()) continue;
        
        arrayGauge[indice]->redraw();
    }
#endif
}

/**
 * @brief Updates labels (optimized for current screen)
 */
void DisplayFK::updateLabel() {
#ifdef DFK_LABEL
    if (!m_labelConfigured) return;
    
    for (uint32_t indice = 0; indice < qtdLabel; indice++) {
        // Skip widgets not on current screen
        if (!arrayLabel[indice]->showingMyScreen()) continue;
        
        arrayLabel[indice]->redraw();
    }
#endif
}

/**
 * @brief Updates LEDs (optimized for current screen)
 */
void DisplayFK::updateLed() {
#ifdef DFK_LED
    if (!m_ledConfigured) return;
    
    for (uint32_t indice = 0; indice < qtdLed; indice++) {
        // Skip widgets not on current screen
        if (!arrayLed[indice]->showingMyScreen()) continue;
        
        arrayLed[indice]->redraw();
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
            if (!arrayLineChart[indice]->showingMyScreen()) continue;
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
            if (!arrayVBar[indice]->showingMyScreen()) continue;
            arrayVBar[indice]->redraw();
        }
    }
#endif
}


/**
 * @brief Updates vertical bars
 */
void DisplayFK::updateThermometer() {
#ifdef DFK_THERMOMETER
    if (m_thermometerConfigured) {
        for (uint32_t indice = 0; indice < qtdThermometer; indice++) {
            if (!arrayThermometer[indice]->showingMyScreen()) continue;
            arrayThermometer[indice]->redraw();
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
            if (!arrayVAnalog[indice]->showingMyScreen()) continue;
            arrayVAnalog[indice]->redraw();
        }
    }
#endif
}

/**
 * @brief Updates checkboxes (optimized for current screen)
 */
void DisplayFK::updateCheckbox() {
#ifdef DFK_CHECKBOX
    if (!m_checkboxConfigured) return;
    
    for (uint32_t indice = 0; indice < qtdCheckbox; indice++) {
        // Skip widgets not on current screen
        if (!arrayCheckbox[indice]->showingMyScreen()) continue;
        
        arrayCheckbox[indice]->redraw();
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
            if (!arrayCircleBtn[indice]->showingMyScreen()) continue;
            arrayCircleBtn[indice]->redraw();
        }
    }
#endif
}

/**
 * @brief Updates horizontal sliders (optimized for current screen)
 */
void DisplayFK::updateHSlider() {
#ifdef DFK_HSLIDER
    if (!m_hSliderConfigured) return;
    
    for (uint32_t indice = 0; indice < qtdHSlider; indice++) {
        // Skip widgets not on current screen
        if (!arrayHSlider[indice]->showingMyScreen()) continue;
        
        arrayHSlider[indice]->redraw();
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
            if (!arrayRadioGroup[indice]->showingMyScreen()) continue;
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
            if (!arrayRectBtn[indice]->showingMyScreen()) continue;
            arrayRectBtn[indice]->redraw();
        }
    }
#endif
}

/**
 * @brief Updates toggles (optimized for current screen)
 */
void DisplayFK::updateToggle() {
#ifdef DFK_TOGGLE
    if (!m_toggleConfigured) return;
    
    for (uint32_t indice = 0; indice < qtdToggle; indice++) {
        // Skip widgets not on current screen
        if (!arrayToggleBtn[indice]->showingMyScreen()) continue;
        
        arrayToggleBtn[indice]->redraw();
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
            if (!arrayImage[indice]->showingMyScreen()) continue;
            arrayImage[indice]->drawBackground();
        }
        for (uint32_t indice = 0; indice < qtdImage; indice++) {
            if (!arrayImage[indice]->showingMyScreen()) continue;
            arrayImage[indice]->draw();
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
            if (!arrayTextButton[indice]->showingMyScreen()) continue;
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
            if (!arraySpinbox[indice]->showingMyScreen()) continue;
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
            if (!arrayTextBox[indice]->showingMyScreen()) continue;
            arrayTextBox[indice]->redraw();
        }
    }
#endif
}

void DisplayFK::updateNumberBox(){
    #ifdef DFK_NUMBERBOX
    if (m_numberboxConfigured) {
        for (uint32_t indice = 0; indice < qtdNumberBox; indice++) {
            if (!arrayNumberbox[indice]->showingMyScreen()) continue;
            arrayNumberbox[indice]->redraw();
        }
    }
#endif
}

void DisplayFK::setupAutoClick(uint32_t intervalMs, uint16_t x, uint16_t y) {
    // Robust input validation
    if (intervalMs == 0) {
        ESP_LOGE(TAG, "Invalid interval: %d ms (must be > 0)", intervalMs);
        return;
    }
    
    if (intervalMs > 86400000) {  // 24 hours max
        ESP_LOGW(TAG, "Interval too large: %d ms (max 24h), limiting to 24h", intervalMs);
        intervalMs = 86400000;
    }
    
    if (intervalMs < 10) {  // 10ms min for reasonable performance
        ESP_LOGW(TAG, "Interval too small: %d ms (min 10ms), setting to 10ms", intervalMs);
        intervalMs = 10;
    }
    
    m_intervalMs = intervalMs;
    m_xAutoClick = x;
    m_yAutoClick = y;

    // Clean up existing timer
    if (m_timer != nullptr) {
        xTimerDelete(m_timer, 0);
        m_timer = nullptr;
    }

    // Create timer with exception safety
    if (!createTimerSafe()) {
        ESP_LOGE(TAG, "Failed to create auto-click timer safely");
        return;
    }
    
    ESP_LOGD(TAG, "Auto-click timer configured: %d ms at (%d, %d)", intervalMs, x, y);
}

void DisplayFK::startAutoClick() {
    if (m_timer != nullptr) {
        xTimerStart(m_timer, 0);
    }
}

void DisplayFK::stopAutoClick() {
    if (m_timer != nullptr) {
        m_simulateAutoClick = false; // Para simular o clique
        xTimerStop(m_timer, 0);
    }
}

void DisplayFK::timerCallback(TimerHandle_t xTimer) {
    // Recupera a instância da classe
    DisplayFK *instance = reinterpret_cast<DisplayFK *>(pvTimerGetTimerID(xTimer));
    if (instance) {
        instance->m_simulateAutoClick = true;
    }
}

bool DisplayFK::isRunningAutoClick() const {
    if (m_timer == nullptr) return false;
    return (xTimerIsTimerActive(m_timer) == pdTRUE);
}

/**
 * @brief Frees a string from the string pool
 * @param str Pointer to the string to free
 */
void DisplayFK::freeStringFromPool(const char* str) {
    if (str) {
        stringPool.deallocate(const_cast<char*>(str));
    }
}

/**
 * @brief Prints StringPool allocation statistics
 */
void DisplayFK::printStringPoolStats() const {
    uint32_t activeAllocations, totalAllocations, totalDeallocations;
    stringPool.getStats(activeAllocations, totalAllocations, totalDeallocations);
    
    ESP_LOGI(TAG, "StringPool Stats:");
    ESP_LOGI(TAG, "  Active allocations: %u", activeAllocations);
    ESP_LOGI(TAG, "  Total allocations: %u", totalAllocations);
    ESP_LOGI(TAG, "  Total deallocations: %u", totalDeallocations);
    
    if (activeAllocations > 0) {
        ESP_LOGW(TAG, "  WARNING: %u active allocations detected", activeAllocations);
        
        const StringAllocation_t* allocations = stringPool.getActiveAllocations();
        uint8_t count = stringPool.getActiveCount();
        
        for (uint8_t i = 0; i < count; i++) {
            const StringAllocation_t* alloc = &allocations[i];
            ESP_LOGD(TAG, "    Allocation %u: ptr=%p, caller=%s, timestamp=%u", 
                     i, alloc->ptr, alloc->caller ? alloc->caller : "unknown", alloc->timestamp);
        }
    }
}

/**
 * @brief Cleans up all StringPool allocations
 */
void DisplayFK::cleanupStringPool() const {
    ESP_LOGD(TAG, "Cleaning up StringPool allocations");
    stringPool.cleanup();
    
    uint32_t activeAllocations, totalAllocations, totalDeallocations;
    stringPool.getStats(activeAllocations, totalAllocations, totalDeallocations);
    
    if (activeAllocations == 0) {
        ESP_LOGD(TAG, "StringPool cleanup successful");
    } else {
        ESP_LOGW(TAG, "StringPool cleanup incomplete: %u allocations remaining", activeAllocations);
    }
}

/**
 * @brief Safe keyboard initialization without exceptions
 * @return true if successful, false otherwise
 */
bool DisplayFK::startKeyboardsSafe() {
    bool success = true;
    
    #ifdef DFK_TEXTBOX
    if (!keyboard) {
        // Use nothrow allocation
        keyboard = std::unique_ptr<WKeyboard>(new(std::nothrow) WKeyboard());
        if (!keyboard) {
            ESP_LOGE(TAG, "Memory allocation failed for keyboard");
            success = false;
        } else {
            // Safe setup with validation
            if (keyboard->setup()) {
                ESP_LOGD(TAG, "Keyboard initialized safely");
            } else {
                ESP_LOGW(TAG, "Keyboard setup method not available");
            }
        }
    }
    #endif
    
    #ifdef DFK_NUMBERBOX
    if (!numpad) {
        // Use nothrow allocation
        numpad = std::unique_ptr<Numpad>(new(std::nothrow) Numpad());
        if (!numpad) {
            ESP_LOGE(TAG, "Memory allocation failed for numpad");
            success = false;
        } else {
            // Safe setup with validation
            if (numpad->setup()) {
                ESP_LOGD(TAG, "Numpad initialized safely");
            } else {
                ESP_LOGW(TAG, "Numpad setup method not available");
            }
        }
    }
    #endif
    
    // Rollback on failure
    if (!success) {
        ESP_LOGE(TAG, "Keyboard initialization failed, cleaning up");
        keyboard.reset();
        numpad.reset();
    }
    
    return success;
}

/**
 * @brief Safe timer creation without exceptions
 * @return true if successful, false otherwise
 */
bool DisplayFK::createTimerSafe() {
    // Validate parameters before creation
    if (m_intervalMs == 0) {
        ESP_LOGE(TAG, "Invalid timer interval: %d ms", m_intervalMs);
        return false;
    }
    
    if (m_intervalMs > 86400000) { // 24 hours max
        ESP_LOGW(TAG, "Timer interval too large: %d ms, limiting to 24h", m_intervalMs);
        m_intervalMs = 86400000;
    }
    
    // Create timer with validation
    m_timer = xTimerCreate("AutoClick", 
                          pdMS_TO_TICKS(m_intervalMs),
                          pdFALSE, 
                          DisplayFK::instance, 
                          DisplayFK::timerCallback);
    
    if (!m_timer) {
        ESP_LOGE(TAG, "Failed to create timer - insufficient memory");
        return false;
    }
    
    ESP_LOGD(TAG, "Timer created safely with interval: %d ms", m_intervalMs);
    return true;
}

/**
 * @brief Safe semaphore creation without exceptions
 * @return true if successful, false otherwise
 */
bool DisplayFK::createSemaphoreSafe() {
    // Check if semaphore already exists
    if (m_loopSemaphore != nullptr) {
        ESP_LOGW(TAG, "Semaphore already exists, cleaning up first");
        vSemaphoreDelete(m_loopSemaphore);
        m_loopSemaphore = nullptr;
    }
    
    // Create loop semaphore with validation
    m_loopSemaphore = xSemaphoreCreateBinary();
    if (!m_loopSemaphore) {
        ESP_LOGE(TAG, "Failed to create loop semaphore - insufficient memory");
        return false;
    }
    
    // Check if transaction semaphore already exists
    if (m_transactionSemaphore != nullptr) {
        ESP_LOGW(TAG, "Transaction semaphore already exists, cleaning up first");
        vSemaphoreDelete(m_transactionSemaphore);
        m_transactionSemaphore = nullptr;
    }
    
    // Create transaction mutex with validation (using mutex for shared resource protection)
    m_transactionSemaphore = xSemaphoreCreateMutex();
    if (!m_transactionSemaphore) {
        ESP_LOGE(TAG, "Failed to create transaction semaphore - insufficient memory");
        vSemaphoreDelete(m_loopSemaphore);
        m_loopSemaphore = nullptr;
        return false;
    }
    
    ESP_LOGD(TAG, "Semaphores created safely");
    return true;
}

/**
 * @brief Safe checkbox configuration without exceptions
 * @param array CheckBox array pointer
 * @param amount Number of checkboxes
 * @return true if successful, false otherwise
 */
bool DisplayFK::setCheckboxSafe(CheckBox *array[], uint8_t amount) {
    if (m_checkboxConfigured) {
        ESP_LOGW(TAG, "Checkbox already configured");
        return false;
    }
    
    // Validate input parameters
    if (!validateArray(array, amount)) {
        ESP_LOGE(TAG, "Invalid checkbox array configuration");
        return false;
    }
    
    // Store original state for rollback
    bool originalConfigured = m_checkboxConfigured;
    CheckBox** originalArray = arrayCheckbox;
    uint8_t originalAmount = qtdCheckbox;
    
    // Configure with validation
    m_checkboxConfigured = true;
    arrayCheckbox = array;
    qtdCheckbox = amount;
    
    // Validate configuration was successful
    if (m_checkboxConfigured && arrayCheckbox == array && qtdCheckbox == amount) {
        ESP_LOGD(TAG, "Checkbox array configured safely with %d elements", amount);
        return true;
    } else {
        ESP_LOGE(TAG, "Checkbox configuration failed, rolling back");
        // Rollback on failure
        m_checkboxConfigured = originalConfigured;
        arrayCheckbox = originalArray;
        qtdCheckbox = originalAmount;
        return false;
    }
}

/**
 * @brief Safe string allocation without exceptions
 * @param caller Function name for logging
 * @return Allocated string or nullptr
 */
char* DisplayFK::allocateStringSafe(const char* caller) {
    // Validate caller parameter
    if (!caller) {
        ESP_LOGW(TAG, "Null caller parameter for string allocation");
    }
    
    // Attempt allocation with validation
    char* result = stringPool.allocate(caller);
    if (!result) {
        ESP_LOGW(TAG, "StringPool exhausted for %s", caller ? caller : "unknown");
        return nullptr;
    }
    
    // Validate allocated string
    if (result && strlen(result) < STRING_POOL_SIZE) {
        ESP_LOGD(TAG, "String allocated safely for %s", caller ? caller : "unknown");
        return result;
    } else {
        ESP_LOGE(TAG, "Invalid string allocation result");
        if (result) {
            stringPool.deallocate(result);
        }
        return nullptr;
    }
}

/**
 * @brief Safe string deallocation without exceptions
 * @param str String to deallocate
 */
void DisplayFK::deallocateStringSafe(char* str) {
    if (!str) {
        ESP_LOGW(TAG, "Attempting to deallocate null string");
        return;
    }
    
    // Validate string before deallocation
    if (strlen(str) >= STRING_POOL_SIZE) {
        ESP_LOGE(TAG, "Invalid string length for deallocation: %zu", strlen(str));
        return;
    }
    
    // Safe deallocation
    stringPool.deallocate(str);
    ESP_LOGD(TAG, "String deallocated safely");
}

/**
 * @brief Safe SD card initialization without exceptions
 * @param pinCS CS pin number
 * @param spiShared SPI instance
 * @param hz SPI frequency
 * @return true if successful, false otherwise
 */
bool DisplayFK::startSDSafe(uint8_t pinCS, SPIClass *spiShared, int hz) {
    // Validate input parameters
    if (!validatePinNumber(pinCS)) {
        ESP_LOGE(TAG, "Invalid CS pin: %d", pinCS);
        return false;
    }
    
    if (!validateFrequency(hz)) {
        ESP_LOGW(TAG, "Invalid frequency: %d, using default", hz);
        hz = 4000000;
    }
    
    if (!spiShared) {
        ESP_LOGE(TAG, "SPI for SD not configured");
        return false;
    }
    
    // Store original state for rollback
    bool originalSdcardOK = sdcardOK;
    
    // Configure SPI with validation
    spiShared->beginTransaction(SPISettings(hz, MSBFIRST, SPI_MODE0));
    sdcardOK = SD.begin(pinCS, *spiShared);
    spiShared->endTransaction();
    
    if (sdcardOK) {
        ESP_LOGI(TAG, "SD card initialized safely");
        return true;
    } else {
        ESP_LOGE(TAG, "SD card initialization failed");
        // Rollback on failure
        sdcardOK = originalSdcardOK;
        return false;
    }
}

/**
 * @brief Validates input string for length and content
 * @param input String to validate
 * @param maxLength Maximum allowed length
 * @return true if valid, false otherwise
 */
bool DisplayFK::validateInput(const char* input, size_t maxLength) const {
    if (!input) {
        ESP_LOGE(TAG, "Input string is null");
        return false;
    }
    
    size_t length = strlen(input);
    if (length == 0) {
        ESP_LOGE(TAG, "Input string is empty");
        return false;
    }
    
    if (length > maxLength) {
        ESP_LOGE(TAG, "Input string too long: %d > %d", length, maxLength);
        return false;
    }
    
    // Check for null characters in the middle of the string
    for (size_t i = 0; i < length; i++) {
        if (input[i] == '\0' && i < length - 1) {
            ESP_LOGE(TAG, "Null character found in middle of string at position %d", i);
            return false;
        }
    }
    
    return true;
}

/**
 * @brief Validates pin number for ESP32
 * @param pin Pin number to validate
 * @return true if valid, false otherwise
 */
bool DisplayFK::validatePinNumber(int8_t pin) const {
    if (pin < 0) {
        ESP_LOGE(TAG, "Invalid pin number: %d (negative)", pin);
        return false;
    }
    
    if (pin > 39) {
        ESP_LOGE(TAG, "Invalid pin number: %d (max 39 for ESP32)", pin);
        return false;
    }
    
    // Check for reserved pins on ESP32
    if (pin == 6 || pin == 7 || pin == 8 || pin == 9 || pin == 10 || pin == 11) {
        ESP_LOGW(TAG, "Pin %d is reserved for flash/PSRAM on ESP32", pin);
        return false;
    }
    
    return true;
}

/**
 * @brief Validates frequency for SPI/I2C communication
 * @param frequency Frequency in Hz
 * @return true if valid, false otherwise
 */
bool DisplayFK::validateFrequency(int frequency) const {
    if (frequency <= 0) {
        ESP_LOGE(TAG, "Invalid frequency: %d Hz (must be positive)", frequency);
        return false;
    }
    
    if (frequency > 20000000) {  // 20MHz max for ESP32
        ESP_LOGW(TAG, "Frequency too high: %d Hz (max 20MHz), limiting to 20MHz", frequency);
        return false;
    }
    
    if (frequency < 100000) {  // 100kHz min for reliable communication
        ESP_LOGW(TAG, "Frequency too low: %d Hz (min 100kHz), setting to 100kHz", frequency);
        return false;
    }
    
    return true;
}

/**
 * @brief Validates screen dimensions
 * @param width Screen width
 * @param height Screen height
 * @return true if valid, false otherwise
 */
bool DisplayFK::validateDimensions(uint16_t width, uint16_t height) const {
    if (width == 0 || height == 0) {
        ESP_LOGE(TAG, "Invalid dimensions: %dx%d (zero dimension)", width, height);
        return false;
    }
    
    if (width > 4096 || height > 4096) {
        ESP_LOGE(TAG, "Dimensions too large: %dx%d (max 4096x4096)", width, height);
        return false;
    }
    
    if (width < 32 || height < 32) {
        ESP_LOGW(TAG, "Dimensions very small: %dx%d (min recommended 32x32)", width, height);
    }
    
    return true;
}

/**
 * @brief Validates array pointer and count
 * @param array Pointer to array
 * @param count Number of elements
 * @return true if valid, false otherwise
 */
bool DisplayFK::validateArray(const void* array, uint8_t count) const {
    if (!array) {
        ESP_LOGE(TAG, "Array pointer is null");
        return false;
    }
    
    if (count == 0) {
        ESP_LOGE(TAG, "Array count is zero");
        return false;
    }
    
    if (count > 255) {
        ESP_LOGE(TAG, "Array count too large: %d (max 255)", count);
        return false;
    }

    // Validate all pointers in the array
    for (uint8_t i = 0; i < count; i++) {
        if (!((const void**)array)[i]) {
            ESP_LOGE(TAG, "Null pointer in array pointer at index %d", i);
            return false;
        }
    }
    
    return true;
}

/**
 * @brief Validates string with specific length constraints
 * @param str String to validate
 * @param maxLength Maximum allowed length
 * @return true if valid, false otherwise
 */
bool DisplayFK::validateString(const char* str, size_t maxLength) const {
    if (!str) {
        ESP_LOGE(TAG, "String pointer is null");
        return false;
    }
    
    size_t length = strlen(str);
    if (length > maxLength) {
        ESP_LOGE(TAG, "String too long: %d > %d", length, maxLength);
        return false;
    }
    
    // Check for valid characters (printable ASCII)
    for (size_t i = 0; i < length; i++) {
        if (str[i] < 32 || str[i] > 126) {
            ESP_LOGE(TAG, "Invalid character at position %d: 0x%02X", i, str[i]);
            return false;
        }
    }
    
    return true;
}