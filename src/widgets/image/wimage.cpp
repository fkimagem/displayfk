#include "wimage.h"

#define DEBUG_IMAGE

#if defined(DEBUG_IMAGE)
#define DEBUG_D(format, ...) log_d(format, ##__VA_ARGS__)
#define DEBUG_E(format, ...) log_e(format, ##__VA_ARGS__)
#define DEBUG_W(format, ...) log_w(format, ##__VA_ARGS__)
#else
#define DEBUG_D(format, ...)
#define DEBUG_E(format, ...)
#define DEBUG_W(format, ...)
#endif

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
  if (!visible) {
    return false;
  }
  if (WidgetBase::usingKeyboard || WidgetBase::currentScreen != screen ||
      !loaded || !cb) {
    return false;
  }
  if (millis() - m_myTime < TIMEOUT_CLICK) {
    return false;
  }
  m_myTime = millis();
  bool detected = false;
  uint16_t xMax = xPos + m_width;
  uint16_t yMax = yPos + m_height;

  if ((*_xTouch > xPos) && (*_xTouch < xMax) && (*_yTouch > yPos) &&
      (*_yTouch < yMax)) {
    detected = true;
  }
  return detected;
}

/**
 * @brief Retrieves the callback function associated with the image.
 * @return Pointer to the callback function.
 */
functionCB_t Image::getCallbackFunc() { return cb; }

bool Image::readFileFromDisk() {
  if (m_source == SourceFile::EMBED) {
    DEBUG_E("Cannot read from embedded source");
    return false;
  }

  if (!m_fs) {
    DEBUG_E("No source defined to find image");
    return false;
  }

  log_d("Looking for file: %s", m_path);

  fs::File file = m_fs->open(m_path, "r");
  if (!file) {
    DEBUG_E("Cant open file");
    return false;
  }

  if (file.isDirectory()) {
    DEBUG_E("Path is a directory");
    file.close();
    return false;
  }

  if (!file.available()) {
    DEBUG_E("File is empty");
    file.close();
    return false;
  }

  size_t size = file.size();
  if (size < 4) {
    log_e("File is too small");
    file.close();
    return false;
  }

  // Get width
  uint16_t arqWidth = ((file.read()) << 8) | file.read();
  // Get height
  uint16_t arqHeight = ((file.read()) << 8) | file.read();

  m_width = arqWidth;
  m_height = arqHeight;

  if (m_width == 0 || m_height == 0) {
    log_e("Invalid image size");
    file.close();
    return false;
  }

  DEBUG_D("Image size: %d x %d", m_width, m_height);

  uint32_t bytesOfColor = m_width * m_height;

  m_pixels = new pixel_t[bytesOfColor];

  if (!m_pixels) {
    DEBUG_E("Failed to allocate memory for image pixels");
    file.close();
    return false;
  }

  memset(m_pixels, 0, bytesOfColor * sizeof(uint16_t));

#if defined(DISP_DEFAULT)
  const uint8_t read_pixels = 2;
#elif defined(DISP_PCD8544) || defined(DISP_SSD1306) || defined(DISP_U8G2)
  const uint8_t read_pixels = 1;
#endif

  uint8_t pixel[read_pixels]; // 2 bytes per pixel (color 565)
  memset(pixel, 0, read_pixels);

  for (int y = 0; y < m_height; y++) {
    for (int x = 0; x < m_width; x++) {
      if (file.read(pixel, read_pixels) != read_pixels) {
        DEBUG_E("Error reading pixel %d,%d", x, y);
        file.close();
        clearBuffers();
        return false;
      }
#if defined(DISP_DEFAULT)
      uint16_t color = (pixel[0] << 8) | pixel[1];
      m_pixels[y * m_width + x] = color;
#elif defined(DISP_PCD8544) || defined(DISP_SSD1306) || defined(DISP_U8G2)
      uint8_t color = pixel[0];
      m_pixels[y * m_width + x] = color;
#endif
    }
  }

  uint16_t maskLen = ((file.read()) << 8) | file.read();

  if (maskLen == 0) {
    DEBUG_E("Invalid mask length");
    file.close();
    clearBuffers();
    return false;
  }

  m_maskAlpha = new uint8_t[maskLen];

  if (!m_maskAlpha) {
    DEBUG_E("Failed to allocate memory for image mask");
    file.close();
    return false;
  }

  memset(m_maskAlpha, 0, maskLen);

  for (int i = 0; i < maskLen; i++) {
    m_maskAlpha[i] = file.read();
  }

  file.close();
  return true;
}

void Image::drawBackground() {
  if (!visible) {
    return;
  }
#if defined(USING_GRAPHIC_LIB) || defined(DISP_U8G2)
  if (WidgetBase::currentScreen != screen ||
      WidgetBase::usingKeyboard == true || !m_shouldRedraw || !loaded) {
    return;
  }
  objTFT->fillRect(xPos, yPos, m_width, m_height, m_backgroundColor);
#endif
}

/**
 * @brief Draws the image on the screen.
 *
 * Loads and renders the image from its source (file or pixels) with optional
 * rotation. Only draws if the widget is on the current screen and needs
 * updating.
 */
void Image::draw() {
  if (!visible) {
    return;
  }
#if defined(USING_GRAPHIC_LIB) || defined(DISP_U8G2)
  if (WidgetBase::currentScreen != screen ||
      WidgetBase::usingKeyboard == true || !m_shouldRedraw || !loaded) {
    return;
  }

  m_shouldRedraw = false;

  if (!m_showImage) {
    return;
  };

  log_d("Redraw image: %d x %d", m_width, m_height);

#if defined(DISP_DEFAULT)
  WidgetBase::objTFT->draw16bitRGBBitmapWithMask(
      xPos, yPos, m_pixels, m_maskAlpha, m_width, m_height);
#elif defined(DISP_PCD8544) || defined(DISP_SSD1306)
  WidgetBase::objTFT->drawBitmap(xPos, yPos, m_pixels, m_width, m_height,
                                 CFK_BLACK);
#elif defined(DISP_U8G2)
  WidgetBase::objTFT->drawXBMP(xPos, yPos, m_width, m_height, m_pixels);
#endif
#endif
}

/**
 * @brief Redraws the image on the screen.
 *
 * Handles loading and drawing the image from its source, which could be
 * embedded memory, SD card, or SPIFFS filesystem.
 */
void Image::redraw() {
  if (!visible) {
    return;
  }
  return;
}

/**
 * @brief Forces the image to update.
 *
 * Sets the update flag to trigger a redraw on the next cycle.
 */
void Image::forceUpdate() { m_shouldRedraw = true; }

/**
 * @brief Configures the Image widget with a source file.
 * @param _source Source location of the image file (SD, SPIFFS, or EMBED).
 * @param _path Path to the image file.
 * @param _cb Optional callback function to execute when the image is interacted
 * with.
 * @param _angle Optional rotation angle in degrees.
 */
void Image::setup(SourceFile _source, const char *_path, functionCB_t _cb,
                  float _angle) {
#if defined(USING_GRAPHIC_LIB) || defined(DISP_U8G2)
  if (loaded) {
    log_w("Reconfigure Image");
    // return;
  }

  clearBuffers();

  m_width = 0;
  m_height = 0;
  cb = _cb;

  m_source = _source;
  m_path = _path;
  m_angle = _angle;

  if (_path == nullptr) {
    log_e("Path is nullptr");
    return;
  }

#if defined(DFK_SD)
  if (m_source == SourceFile::SD) {
    if (!WidgetBase::mySD) {
      log_e("SD not configured");
      return;
    }
    m_fs = WidgetBase::mySD;
  } else if (m_source == SourceFile::SPIFFS) {
    if (!SPIFFS.begin(false)) {
      Serial.println("SPIFFS Mount Failed");
      return;
    }

    m_fs = &SPIFFS;
  }
#else
  if (m_source == SourceFile::SD) {
    log_w("SD not configured");
    return;
  } else if (m_source == SourceFile::SPIFFS) {
    m_fs = &SPIFFS;
  }
#endif

  if (!m_fs) {
    log_e("No source defined to find image");
    return;
  }
  DEBUG_D("Image setup with source %d", m_source);

  loaded = readFileFromDisk();
#endif
  m_shouldRedraw = true;
}

void Image::clearBuffers() {
  if (m_pixels) {
    delete[] m_pixels;
    m_pixels = nullptr;
  }
  if (m_maskAlpha) {
    delete[] m_maskAlpha;
    m_maskAlpha = nullptr;
  }
}

/**
 * @brief Configures the Image widget with pixel data.
 * @param _pixels Pointer to the pixel data array.
 * @param _width Width of the image.
 * @param _height Height of the image.
 * @param _maskAlpha Pointer to the alpha mask array for transparency.
 * @param _angle Rotation angle in degrees.
 * @param _cb Callback function to execute when the image is interacted with.
 */
void Image::setup(const pixel_t *_pixels, uint16_t _width, uint16_t _height,
                  const uint8_t *_maskAlpha, float _angle, functionCB_t _cb) {
  if (loaded) {
    log_w("Reconfigure Image");
    // return;
    return;
  }

  // Libera memória existente se existir
  clearBuffers();

  m_source = SourceFile::EMBED;
  m_pixels = const_cast<pixel_t *>(_pixels);
  m_maskAlpha = const_cast<uint8_t *>(_maskAlpha);
  m_width = _width;
  m_height = _height;
  m_angle = _angle;
  cb = _cb;

  if (_pixels == nullptr) {
    log_e("Pixels is nullptr");
    return;
  }

  DEBUG_D("Image setup with pixels (%d x %d)", _width, _height);

  loaded = true;
  m_shouldRedraw = true;
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
void Image::setup(ImageFromFileConfig &config) {
  log_d("Need to load file %s", config.toString().c_str());
  setup(config.source, config.path, config.cb, 0);
  m_backgroundColor = config.backgroundColor;
}

/**
 * @brief Configures the Image widget with pixel data.
 *
 * Sets up the image with pixel data, width, height, mask alpha, angle, and
 * callback function.
 *
 * @param config Configuration structure containing pixel data, width, height,
 * mask alpha, angle, and callback function.
 */
void Image::setup(ImageFromPixelsConfig &config) {
  setup(config.pixels, config.width, config.height, config.maskAlpha, 0,
        config.cb);
  m_backgroundColor = config.backgroundColor;
}

void Image::show() {
  visible = true;
  m_shouldRedraw = true;
}

void Image::hide() {
  visible = false;
  m_shouldRedraw = true;
}