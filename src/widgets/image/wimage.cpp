#include "wimage.h"
#include <esp_log.h>

const char* Image::TAG = "Image";

/**
 * @brief Constructor for the Image class.
 * @param _x X position of the image.
 * @param _y Y position of the image.
 * @param _screen Screen number.
 */
Image::Image(uint16_t _x, uint16_t _y, uint8_t _screen)
    : WidgetBase(_x, _y, _screen),
#if defined(USING_GRAPHIC_LIB)
      m_pixels(nullptr),
#endif
      m_maskAlpha(nullptr), m_fs(nullptr), m_path(nullptr) {

        //initialize m_config with default values
        m_config = {
          .pixels = nullptr,
          .width = 0,
          .height = 0,
          .maskAlpha = nullptr,
          .cb = nullptr,
          .backgroundColor = 0,
          .angle = 0.0f
        };
}

/**
 * @brief Destructor for the Image class.
 */
Image::~Image() { clearBuffers(); }

/**
 * @brief Detects if the Image has been touched
 * @param _xTouch Pointer to the X-coordinate of the touch
 * @param _yTouch Pointer to the Y-coordinate of the touch
 * @return True if the touch is within the Image area, otherwise false
 */
bool Image::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) {
  // Early validation checks using macros
  CHECK_VISIBLE_BOOL
  CHECK_LOADED_BOOL
  CHECK_USINGKEYBOARD_BOOL
  CHECK_CURRENTSCREEN_BOOL
  CHECK_POINTER_TOUCH_NULL_BOOL
  CHECK_LOCKED_BOOL
  CHECK_INITIALIZED_BOOL
  CHECK_DEBOUNCE_CLICK_BOOL
  CHECK_ENABLED_BOOL

  


  bool detected = POINT_IN_RECT(*_xTouch, *_yTouch, m_xPos, m_yPos, m_config.width, m_config.height);
if(detected) {
  m_myTime = millis();
}
  

  return detected;
}

/**
 * @brief Retrieves the callback function associated with the image.
 * @return Pointer to the callback function.
 */
functionCB_t Image::getCallbackFunc() { return m_callback; }

bool Image::readFileFromDisk() {
  if (m_source == SourceFile::EMBED) {
    ESP_LOGE(TAG, "Cannot read from embedded source");
    return false;
  }

  if (!m_fs) {
    ESP_LOGE(TAG, "No source defined to find image");
    return false;
  }

  ESP_LOGD(TAG, "Looking for file: %s", m_path);

  // Start file load performance timing
  uint32_t fileLoadStartTime = micros();
  m_metrics.fileLoadCount++;

  fs::File file = m_fs->open(m_path, "r");
  if (!file) {
    ESP_LOGE(TAG, "Cant open file");
    return false;
  }

  if (file.isDirectory()) {
    ESP_LOGE(TAG, "Path is a directory");
    file.close();
    return false;
  }

  if (!file.available()) {
    ESP_LOGE(TAG, "File is empty");
    file.close();
    return false;
  }

  size_t size = file.size();
  if (size < 4) {
    ESP_LOGE(TAG, "File is too small");
    file.close();
    return false;
  }

  // Get width
  uint16_t arqWidth = ((file.read()) << 8) | file.read();
  // Get height
  uint16_t arqHeight = ((file.read()) << 8) | file.read();

  if (arqWidth == 0 || arqHeight == 0) {
    ESP_LOGE(TAG, "Invalid image size");
    file.close();
    return false;
  }

  ESP_LOGD(TAG, "Image size: %d x %d", arqWidth, arqHeight);

  // Store dimensions in m_config
  m_config.width = arqWidth;
  m_config.height = arqHeight;

  uint32_t bytesOfColor = arqWidth * arqHeight;

  m_pixels = new pixel_t[bytesOfColor];

  if (!m_pixels) {
    ESP_LOGE(TAG, "Failed to allocate memory for image pixels");
    file.close();
    return false;
  }

  memset(m_pixels, 0, bytesOfColor * sizeof(uint16_t));

#if defined(DISP_DEFAULT)
  const uint8_t read_pixels = 2;
#elif defined(DISP_PCD8544) || defined(DISP_SSD1306) || defined(DISP_U8G2)
  const uint8_t read_pixels = 1;
#endif

  // Optimized reading: read entire lines at once
  const uint32_t lineSize = arqWidth * read_pixels;
  uint8_t *lineBuffer = new uint8_t[lineSize];
  
  if (!lineBuffer) {
    ESP_LOGE(TAG, "Failed to allocate line buffer");
    file.close();
    clearBuffers();
    return false;
  }

  for (int y = 0; y < arqHeight; y++) {
    // Read entire line at once
    if (file.read(lineBuffer, lineSize) != lineSize) {
      ESP_LOGE(TAG, "Error reading line %d", y);
      delete[] lineBuffer;
      file.close();
      clearBuffers();
      return false;
    }
    
    // Process line pixels
    for (int x = 0; x < arqWidth; x++) {
#if defined(DISP_DEFAULT)
      uint16_t color = (lineBuffer[x * 2] << 8) | lineBuffer[x * 2 + 1];
      m_pixels[y * arqWidth + x] = color;
#elif defined(DISP_PCD8544) || defined(DISP_SSD1306) || defined(DISP_U8G2)
      uint8_t color = lineBuffer[x];
      m_pixels[y * arqWidth + x] = color;
#endif
    }
  }
  
  delete[] lineBuffer;

  uint16_t maskLen = ((file.read()) << 8) | file.read();

  if (maskLen == 0) {
    ESP_LOGE(TAG, "Invalid mask length");
    file.close();
    clearBuffers();
    return false;
  }

  m_maskAlpha = new uint8_t[maskLen];

  if (!m_maskAlpha) {
    ESP_LOGE(TAG, "Failed to allocate memory for image mask");
    file.close();
    return false;
  }

  memset(m_maskAlpha, 0, maskLen);

  for (int i = 0; i < maskLen; i++) {
    m_maskAlpha[i] = file.read();
  }

  file.close();
  
  // End file load performance timing
  uint32_t fileLoadEndTime = micros();
  m_metrics.lastFileLoadTime = fileLoadEndTime - fileLoadStartTime;
  m_metrics.totalFileLoadTime += m_metrics.lastFileLoadTime;
  if (m_metrics.lastFileLoadTime > m_metrics.maxFileLoadTime) {
    m_metrics.maxFileLoadTime = m_metrics.lastFileLoadTime;
  }
  
  return true;
}

/**
 * @brief Draws the background of the image widget.
 *
 * Fills the image area with the background color. Only draws if the widget
 * is on the current screen and needs updating.
 */
void Image::drawBackground() {
  CHECK_TFT_VOID
  CHECK_VISIBLE_VOID
  CHECK_CURRENTSCREEN_VOID
  CHECK_USINGKEYBOARD_VOID
  CHECK_LOADED_VOID
  CHECK_SHOULDREDRAW_VOID
  
  // Validate configuration using centralized method
  if (!validateConfig()) {
    ESP_LOGW(TAG, "Invalid configuration for background drawing");
    return;
  }
  
  WidgetBase::objTFT->fillRect(m_xPos, m_yPos, m_config.width, m_config.height, m_config.backgroundColor);
  ESP_LOGD(TAG, "Background drawn: %dx%d at (%d,%d)", m_config.width, m_config.height, m_xPos, m_yPos);
}

/**
 * @brief Draws the image on the screen.
 *
 * Loads and renders the image from its source (file or pixels) with optional
 * rotation. Only draws if the widget is on the current screen and needs
 * updating.
 */
void Image::draw() {
  CHECK_TFT_VOID
  CHECK_VISIBLE_VOID
  CHECK_CURRENTSCREEN_VOID
  CHECK_USINGKEYBOARD_VOID
  CHECK_LOADED_VOID
  CHECK_SHOULDREDRAW_VOID

#if defined(USING_GRAPHIC_LIB) || defined(DISP_U8G2)

  m_shouldRedraw = false;

  // If not visible, draw background only
  if (!m_visible) {
    // Validate configuration before drawing background
    if (validateConfig()) {
      WidgetBase::objTFT->fillRect(m_xPos, m_yPos, m_config.width, m_config.height, m_config.backgroundColor);
      ESP_LOGD(TAG, "Image hidden - background drawn: %dx%d", m_config.width, m_config.height);
    } else {
      ESP_LOGW(TAG, "Cannot draw background - invalid configuration");
    }
    return;
  }

  // Validate configuration before drawing
  if (!validateConfig()) {
    ESP_LOGW(TAG, "Cannot draw image - invalid configuration");
    return;
  }

  // Start performance timing
  uint32_t startTime = micros();
  m_metrics.drawCount++;

  ESP_LOGD(TAG, "Redraw image: %d x %d (angle: %.1f°)", m_config.width, m_config.height, m_config.angle);

  // Apply rotation if angle is not zero
  if (m_config.angle != 0.0f) {
    drawRotatedImage();
  } else {
    // Draw without rotation
#if defined(DISP_DEFAULT)
    WidgetBase::objTFT->draw16bitRGBBitmapWithMask(
        m_xPos, m_yPos, m_config.pixels, m_config.maskAlpha, m_config.width, m_config.height);
#elif defined(DISP_PCD8544) || defined(DISP_SSD1306)
    WidgetBase::objTFT->drawBitmap(m_xPos, m_yPos, m_config.pixels, m_config.width, m_config.height,
                                   CFK_BLACK);
#elif defined(DISP_U8G2)
    WidgetBase::objTFT->drawXBMP(m_xPos, m_yPos, m_config.width, m_config.height, m_config.pixels);
#endif
  }
  
  // End performance timing
  uint32_t endTime = micros();
  m_metrics.lastDrawTime = endTime - startTime;
  m_metrics.totalDrawTime += m_metrics.lastDrawTime;
  if (m_metrics.lastDrawTime > m_metrics.maxDrawTime) {
    m_metrics.maxDrawTime = m_metrics.lastDrawTime;
  }
#endif
}

/**
 * @brief Redraws the image on the screen.
 *
 * Simply calls the draw() method to render the image.
 * All validation is handled by the draw() method itself.
 */
void Image::redraw() {
  draw();
}

/**
 * @brief Forces the image to update.
 *
 * Sets the update flag to trigger a redraw on the next cycle.
 */
void Image::forceUpdate() { 
  m_shouldRedraw = true; 
  ESP_LOGD(TAG, "Image force update requested");
}


/**
 * @brief Configures the Image widget with a file source.
 *
 * Sets up the image with a file source, path, callback function, and rotation angle.
 * Uses readFileFromDisk() to convert file to pixel data and maps to m_config.
 *
 * @param config Configuration structure containing file source, path, callback
 * function, and rotation angle.
 */
void Image::setupFromFile(ImageFromFileConfig &config) {
  if (!WidgetBase::objTFT) {
    ESP_LOGW(TAG, "TFT not defined on WidgetBase");
    return;
  }

  if (m_loaded) {
    ESP_LOGW(TAG, "Image widget already configured");
    return;
  }

  // Validate file configuration
  if (config.path == nullptr) {
    ESP_LOGE(TAG, "File path is null");
    return;
  }

  if (strlen(config.path) == 0) {
    ESP_LOGE(TAG, "File path is empty");
    return;
  }

  // Clean up any existing memory
  clearBuffers();

  // Set up file system and load file
  m_source = config.source;
  m_path = config.path;
  m_callback = config.cb;
  //m_backgroundColor = config.backgroundColor;
  //m_angle = 0.0f; // File rotation not implemented yet
  m_ownsMemory = true; // We will own the memory for file images

  // Define file system
  defineFileSystem(config.source);

  // Load file from disk
  if (readFileFromDisk()) {
    // Map loaded data to unified configuration
    m_config.pixels = m_pixels;
    m_config.maskAlpha = m_maskAlpha;
    m_config.cb = config.cb;
    m_config.backgroundColor = config.backgroundColor;
    m_config.angle = 0.0;

    // Validate loaded configuration
    if (!validateConfig()) {
      ESP_LOGE(TAG, "Loaded image configuration is invalid");
      clearBuffers();
      return;
    }

    m_loaded = true;
    m_shouldRedraw = true;
    
    ESP_LOGD(TAG, "Image setup from file completed at (%d, %d) - %dx%d from %s", 
             m_xPos, m_yPos, m_config.width, m_config.height, config.path);
  } else {
    ESP_LOGE(TAG, "Failed to load image from file: %s", config.path);
  }
}


/**
 * @brief Configures the Image widget with pixel data.
 *
 * Sets up the image with pixel data, dimensions, alpha mask, and callback function.
 * Uses direct reference to avoid memory overhead for embedded images.
 *
 * @param config Configuration structure containing pixel data, dimensions, 
 * alpha mask, and callback function.
 */
void Image::setupFromPixels(ImageFromPixelsConfig &config) {
  if (!WidgetBase::objTFT) {
    ESP_LOGW(TAG, "TFT not defined on WidgetBase");
    return;
  }

  if (m_loaded) {
    ESP_LOGW(TAG, "Image widget already configured");
    return;
  }

  // Clean up any existing memory
  clearBuffers();

  // Store configuration first for validation
  m_config = config;
  
  // Validate configuration using centralized method
  if (!validateConfig()) {
    ESP_LOGE(TAG, "Invalid pixel configuration provided");
    m_config = {}; // Reset to default
    return;
  }

  
  m_ownsMemory = false; // We don't own the memory for embedded images
  
  // Map to legacy variables for compatibility
  m_pixels = const_cast<pixel_t*>(config.pixels);
  m_maskAlpha = const_cast<uint8_t*>(config.maskAlpha);
  m_callback = config.cb;
  //m_backgroundColor = config.backgroundColor;
  m_source = SourceFile::EMBED;

  m_loaded = true;
  m_shouldRedraw = true;
  m_initialized = true;
  
  ESP_LOGD(TAG, "Image setup from pixels completed at (%d, %d) - %dx%d", 
           m_xPos, m_yPos, m_config.width, m_config.height);
}

/**
 * @brief Defines the file system for the image.
 * @param source Source of the image file.
 */
void Image::defineFileSystem(SourceFile source) {
  if (source == SourceFile::SD) {
    if (!WidgetBase::mySD) {
      ESP_LOGE(TAG, "SD not configured");
      return;
    }
    m_fs = WidgetBase::mySD;
  } else if (source == SourceFile::SPIFFS) {
    if (!SPIFFS.begin(false)) {
      ESP_LOGE(TAG, "SPIFFS Mount Failed");
      return;
    }
    m_fs = &SPIFFS;
  } else {
    ESP_LOGE(TAG, "Invalid source");
    return;
  }
}

/**
 * @brief Prints performance metrics to the log.
 *
 * Displays comprehensive performance statistics including draw times,
 * file load times, rotation times, and their averages.
 */
void Image::printMetrics() const {
  ESP_LOGI(TAG, "=== Image Performance Metrics ===");
  ESP_LOGI(TAG, "Draw Operations: %u calls", m_metrics.drawCount);
  ESP_LOGI(TAG, "File Loads: %u calls", m_metrics.fileLoadCount);
  ESP_LOGI(TAG, "Rotation Draws: %u calls", m_metrics.rotationDrawCount);
  ESP_LOGI(TAG, "");
  ESP_LOGI(TAG, "Draw Times (μs):");
  ESP_LOGI(TAG, "  Last: %u", m_metrics.lastDrawTime);
  ESP_LOGI(TAG, "  Average: %.2f", m_metrics.getAverageDrawTime());
  ESP_LOGI(TAG, "  Maximum: %u", m_metrics.maxDrawTime);
  ESP_LOGI(TAG, "  Total: %u", m_metrics.totalDrawTime);
  ESP_LOGI(TAG, "");
  ESP_LOGI(TAG, "File Load Times (μs):");
  ESP_LOGI(TAG, "  Last: %u", m_metrics.lastFileLoadTime);
  ESP_LOGI(TAG, "  Average: %.2f", m_metrics.getAverageFileLoadTime());
  ESP_LOGI(TAG, "  Maximum: %u", m_metrics.maxFileLoadTime);
  ESP_LOGI(TAG, "  Total: %u", m_metrics.totalFileLoadTime);
  ESP_LOGI(TAG, "");
  ESP_LOGI(TAG, "Rotation Times (μs):");
  ESP_LOGI(TAG, "  Last: %u", m_metrics.lastRotationTime);
  ESP_LOGI(TAG, "  Average: %.2f", m_metrics.getAverageRotationTime());
  ESP_LOGI(TAG, "  Maximum: %u", m_metrics.maxRotationTime);
  ESP_LOGI(TAG, "  Total: %u", m_metrics.totalRotationTime);
  ESP_LOGI(TAG, "=====================================");
}

/**
 * @brief Draws the image with rotation applied.
 *
 * Implements pixel-by-pixel rotation using trigonometric calculations.
 * Supports rotation around the center of the image.
 */
void Image::drawRotatedImage() {
  // Start rotation performance timing
  uint32_t rotationStartTime = micros();
  m_metrics.rotationDrawCount++;
  
  // Convert angle to radians
  float angleRad = m_config.angle * PI / 180.0f;
  float cosAngle = cos(angleRad);
  float sinAngle = sin(angleRad);
  
  // Calculate center point
  float centerX = m_config.width / 2.0f;
  float centerY = m_config.height / 2.0f;
  
  // Calculate rotated dimensions (bounding box)
  float cosAbs = fabs(cosAngle);
  float sinAbs = fabs(sinAngle);
  int rotatedWidth = (int)(m_config.width * cosAbs + m_config.height * sinAbs);
  int rotatedHeight = (int)(m_config.width * sinAbs + m_config.height * cosAbs);
  
  ESP_LOGD(TAG, "Drawing rotated image: %dx%d -> %dx%d (%.1f°)", 
           m_config.width, m_config.height, rotatedWidth, rotatedHeight, m_config.angle);
  
  // Draw rotated image pixel by pixel
  for (int y = 0; y < rotatedHeight; y++) {
    for (int x = 0; x < rotatedWidth; x++) {
      // Calculate position relative to rotated center
      float relX = x - rotatedWidth / 2.0f;
      float relY = y - rotatedHeight / 2.0f;
      
      // Apply inverse rotation to get original coordinates
      float origX = relX * cosAngle + relY * sinAngle + centerX;
      float origY = -relX * sinAngle + relY * cosAngle + centerY;
      
      // Check if original coordinates are within bounds
      if (origX >= 0 && origX < m_config.width && origY >= 0 && origY < m_config.height) {
        int origXInt = (int)origX;
        int origYInt = (int)origY;
        int pixelIndex = origYInt * m_config.width + origXInt;
        
        // Draw pixel with rotation
        int screenX = m_xPos + x - rotatedWidth / 2 + m_config.width / 2;
        int screenY = m_yPos + y - rotatedHeight / 2 + m_config.height / 2;
        
        if (screenX >= 0 && screenX < WidgetBase::objTFT->width() && 
            screenY >= 0 && screenY < WidgetBase::objTFT->height()) {
          
#if defined(DISP_DEFAULT)
          uint16_t color = m_config.pixels[pixelIndex];
          // Apply alpha mask if available
          if (m_config.maskAlpha && m_config.maskAlpha[pixelIndex] < 255) {
            // Skip transparent pixels
            continue;
          }
          WidgetBase::objTFT->drawPixel(screenX, screenY, color);
#elif defined(DISP_PCD8544) || defined(DISP_SSD1306) || defined(DISP_U8G2)
          uint8_t color = m_config.pixels[pixelIndex];
          if (color != 0) { // Only draw non-transparent pixels
            WidgetBase::objTFT->drawPixel(screenX, screenY, color);
          }
#endif
        }
      }
    }
  }
  
  // End rotation performance timing
  uint32_t rotationEndTime = micros();
  m_metrics.lastRotationTime = rotationEndTime - rotationStartTime;
  m_metrics.totalRotationTime += m_metrics.lastRotationTime;
  if (m_metrics.lastRotationTime > m_metrics.maxRotationTime) {
    m_metrics.maxRotationTime = m_metrics.lastRotationTime;
  }
}

/**
 * @brief Validates the current image configuration.
 *
 * Performs comprehensive validation of the image configuration including
 * dimensions, pixel data, and display compatibility. Centralizes all
 * validation logic to ensure consistency across the widget.
 *
 * @return true if configuration is valid, false otherwise
 */
bool Image::validateConfig() {
  // Validate basic configuration
  if (m_config.pixels == nullptr) {
    ESP_LOGE(TAG, "Image pixels are null");
    return false;
  }
  
  if (m_config.width == 0 || m_config.height == 0) {
    ESP_LOGE(TAG, "Invalid image dimensions: %dx%d", m_config.width, m_config.height);
    return false;
  }
  
  // Validate reasonable size limits
  if (m_config.width > 4096 || m_config.height > 4096) {
    ESP_LOGE(TAG, "Image dimensions too large: %dx%d (max: 4096x4096)", 
             m_config.width, m_config.height);
    return false;
  }
  
  // Validate pixel count doesn't overflow
  uint32_t pixelCount = static_cast<uint32_t>(m_config.width) * static_cast<uint32_t>(m_config.height);
  if (pixelCount == 0 || pixelCount > 16777216) { // 16M pixels max
    ESP_LOGE(TAG, "Invalid pixel count: %u", pixelCount);
    return false;
  }
  
  // Validate angle is reasonable
  if (m_config.angle < -360.0f || m_config.angle > 360.0f) {
    ESP_LOGW(TAG, "Angle out of range: %.1f° (normalizing to 0-360°)", m_config.angle);
    // Normalize angle to 0-360 range
    while (m_config.angle < 0.0f) m_config.angle += 360.0f;
    while (m_config.angle >= 360.0f) m_config.angle -= 360.0f;
  }
  
  // Validate display compatibility
  if (!WidgetBase::objTFT) {
    ESP_LOGE(TAG, "TFT display not initialized");
    return false;
  }
  
  // Validate position is within display bounds
  if (m_xPos < 0 || m_yPos < 0) {
    ESP_LOGW(TAG, "Image position is negative: (%d, %d)", m_xPos, m_yPos);
    return false;
  }
  
  // Check if image fits within display (with rotation consideration)
  int maxWidth = WidgetBase::objTFT->width();
  int maxHeight = WidgetBase::objTFT->height();
  
  if (m_config.angle == 0.0f) {
    // Simple bounds check for non-rotated images
    if (m_xPos + m_config.width > maxWidth || m_yPos + m_config.height > maxHeight) {
      ESP_LOGW(TAG, "Image extends beyond display bounds: (%d+%d, %d+%d) > (%d, %d)", 
               m_xPos, m_config.width, m_yPos, m_config.height, maxWidth, maxHeight);
      return false;
    }
  } else {
    // For rotated images, check bounding box
    float angleRad = m_config.angle * PI / 180.0f;
    float cosAbs = fabs(cos(angleRad));
    float sinAbs = fabs(sin(angleRad));
    int rotatedWidth = (int)(m_config.width * cosAbs + m_config.height * sinAbs);
    int rotatedHeight = (int)(m_config.width * sinAbs + m_config.height * cosAbs);
    
    if (m_xPos + rotatedWidth > maxWidth || m_yPos + rotatedHeight > maxHeight) {
      ESP_LOGW(TAG, "Rotated image extends beyond display bounds: (%d+%d, %d+%d) > (%d, %d)", 
               m_xPos, rotatedWidth, m_yPos, rotatedHeight, maxWidth, maxHeight);
      return false;
    }
  }
  
  ESP_LOGD(TAG, "Configuration validation passed: %dx%d at (%d,%d) angle=%.1f°", 
           m_config.width, m_config.height, m_xPos, m_yPos, m_config.angle);
  return true;
}

/**
 * @brief Clears all buffers and resets memory management.
 * 
 * Handles memory cleanup based on whether the image is embedded or loaded from file.
 * For embedded images, only clears references. For file images, deallocates memory.
 */
void Image::clearBuffers() {
  // Clear unified configuration
  m_config.pixels = nullptr;
  m_config.maskAlpha = nullptr;
  m_config.width = 0;
  m_config.height = 0;
  m_config.cb = nullptr;
  m_config.backgroundColor = 0x0000;
  m_config.angle = 0.0f;
  
  // Clear legacy variables
  if (m_pixels) {
    // Only delete if we own the memory
    if (m_ownsMemory) {
      delete[] m_pixels;
    }
    m_pixels = nullptr;
  }
  
  if (m_maskAlpha) {
    // Only delete if we own the memory
    if (m_ownsMemory) {
      delete[] m_maskAlpha;
    }
    m_maskAlpha = nullptr;
  }
  
  // Clear file system references
  if (m_fs) {
    m_fs = nullptr;
  }
  if (m_path) {
    m_path = nullptr;
  }
  
  
  // Reset state
  m_ownsMemory = false;
  m_loaded = false;
  m_shouldRedraw = false;
}


/**
 * @brief Configures the Image widget with a file source.
 *
 * Sets up the image with a file source, path, callback function, and rotation
 * angle.
 *
 * @param config Configuration structure containing file source, path, callback
 * function, and rotation angle.
 */

/**
 * @brief Shows the image widget.
 *
 * Makes the image visible and marks it for redraw.
 * Only works if the widget is properly loaded.
 */
void Image::show() {
  if (!m_loaded) {
    ESP_LOGW(TAG, "Cannot show unloaded image widget");
    return;
  }
  
  m_visible = true;
  m_shouldRedraw = true;
  ESP_LOGD(TAG, "Image shown at (%d, %d)", m_xPos, m_yPos);
}

/**
 * @brief Hides the image widget.
 *
 * Makes the image invisible and marks it for redraw.
 * Only works if the widget is properly loaded.
 */
void Image::hide() {
  if (!m_loaded) {
    ESP_LOGW(TAG, "Cannot hide unloaded image widget");
    return;
  }
  
  m_visible = false;
  m_shouldRedraw = true;
  ESP_LOGD(TAG, "Image hidden at (%d, %d)", m_xPos, m_yPos);
}