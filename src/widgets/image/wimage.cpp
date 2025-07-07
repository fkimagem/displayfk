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
Image::Image(uint16_t _x, uint16_t _y, uint8_t _screen) : WidgetBase(_x, _y, _screen), 
                                                         m_pixels(nullptr), 
                                                         m_maskAlpha(nullptr), 
                                                         m_fs(nullptr),
                                                         m_path(nullptr)
{
}

/**
 * @brief Destructor for the Image class.
 */
Image::~Image()
{
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
  * @brief Detects if the Image has been touched
  * @param _xTouch Pointer to the X-coordinate of the touch
  * @param _yTouch Pointer to the Y-coordinate of the touch
  * @return True if the touch is within the Image area, otherwise false
  */
bool Image::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch)
{
  if (WidgetBase::usingKeyboard || WidgetBase::currentScreen != screen || !loaded || !cb)
  {
    return false;
  }
  if (millis() - m_myTime < TIMEOUT_CLICK)
  {
    return false;
  }
  m_myTime = millis();
  bool detectado = false;
  uint16_t xMax = xPos + m_width;
  uint16_t yMax = yPos + m_height;

  if ((*_xTouch > xPos) && (*_xTouch < xMax) && (*_yTouch > yPos) && (*_yTouch < yMax))
  {
    detectado = true;
  }
  return detectado;
}

/**
 * @brief Retrieves the callback function associated with the image.
 * @return Pointer to the callback function.
 */
functionCB_t Image::getCallbackFunc()
{
  return cb;
}


/**
 * @brief Draws the image on the screen.
 */
void Image::draw()
{
  if (WidgetBase::currentScreen != screen || WidgetBase::usingKeyboard == true || !m_update || !loaded)
  {
    return;
  }

  m_update = false;

  if (m_source == SourceFile::SD || m_source == SourceFile::SPIFFS) {
    // Libera memória existente antes de alocar nova
    if(m_pixels) {
      delete[] m_pixels;
      m_pixels = nullptr;
    }
    if(m_maskAlpha) {
      delete[] m_maskAlpha;
      m_maskAlpha = nullptr;
    }

    if(!m_fs){
      log_e("No source defined to find image");
      return;
    }

    log_d("Redrawing image %s", m_path);

    fs::File file = m_fs->open(m_path, "r");
    if (!file)
    {
      log_e("Invalid file");
      return;
    }

    if(file.isDirectory()) {
      log_e("File is a directory");
      file.close();
      return;
    }

    if(!file.available()) {
      log_e("File is empty");
      file.close();
      return;
    }

    size_t size = file.size();
    if(size < 4) {
      log_e("File is too small");
      file.close();
      return;
    }

    // obtem a largura (width)
    uint16_t arqWidth = ((file.read()) << 8) | file.read();
    // obtem a altura (height)
    uint16_t arqHeight = ((file.read()) << 8) | file.read();

    m_width = arqWidth;
    m_height = arqHeight;

    if(m_width == 0 || m_height == 0) {
      log_e("Invalid image size");
      file.close();
      return;
    }

    DEBUG_D("Image size: %d x %d", m_width, m_height);
    
    uint32_t bytesOfColor = m_width * m_height;

    m_pixels = new uint16_t[bytesOfColor];

    if(!m_pixels) {
      log_e("Failed to allocate memory for image pixels");
      file.close();
      return;
    }

    for(int i = 0; i < bytesOfColor; i++) {
      m_pixels[i] = 0;
    }

    uint8_t read_pixels = 2;
    uint8_t pixel[read_pixels];// 3 bytes per pixel (core 565 + alpha)
    for(int i = 0; i < read_pixels; i++) {
      pixel[i] = 0;
    }
    

    for (int y = 0; y < m_height; y++) {
      for (int x = 0; x < m_width; x++) {
        if (file.read(pixel, read_pixels) != read_pixels) {
          log_e("Error reading pixel %d,%d", x, y);
          file.close();
          delete[] m_pixels;
          m_pixels = nullptr;
          return;
        }
        //Serial.printf("Pixel: %2x %2x %d\n", pixel[0], pixel[1], pixel[2]);
        uint16_t cor = (pixel[0] << 8) | pixel[1];
        m_pixels[y * m_width + x] = cor;
      }
    }

    uint16_t maskLen = ((file.read()) << 8) | file.read();

    if(maskLen == 0) {
      log_e("Invalid mask length");
      file.close();
      delete[] m_pixels;  // Libera pixels antes de retornar em caso de erro
      m_pixels = nullptr;
      return;
    }
    
    m_maskAlpha = new uint8_t[maskLen];

    if(!m_maskAlpha) {
      log_e("Failed to allocate memory for image mask");
      file.close();
      delete[] m_pixels;  // Libera pixels antes de retornar em caso de erro
      m_pixels = nullptr;
      return;
    }

    for(int i = 0; i < maskLen; i++) {
      m_maskAlpha[i] = 0;
    }

    for(int i = 0; i < maskLen; i++) {
      m_maskAlpha[i] = file.read();
    }

    file.close();
  }

  if(!m_pixels) {
    log_e("Image not loaded. Missing pixels");
    return;
  }

  if (m_angle != 0) {
    drawRotatedImageOptimized(m_pixels, m_width, m_height, m_angle, m_width / 2, m_height / 2, xPos, yPos);
  } else {
    if(m_maskAlpha) {
      DEBUG_D("Drawing image with mask");
      draw16bitRGBBitmapWithMask(xPos, yPos, m_pixels, m_maskAlpha, m_width, m_height);
    }
    else {
      DEBUG_D("Drawing image without mask");
      WidgetBase::objTFT->draw16bitRGBBitmap(xPos, yPos, m_pixels, m_width, m_height);
    }
  }

  // Libera memória após desenhar para SD/SPIFFS
  if(m_source == SourceFile::SD || m_source == SourceFile::SPIFFS){
    if(m_maskAlpha) {
      DEBUG_D("Freeing mask alpha");
      delete[] m_maskAlpha;
      m_maskAlpha = nullptr;
    }

    if(m_pixels) {
      DEBUG_D("Freeing pixels");
      delete[] m_pixels;
      m_pixels = nullptr;
    }
  }
  DEBUG_D("Image drawn");
}


/**
 * @brief Draws a 16-bit RGB bitmap with a mask.
 * @param x The x-coordinate of the top-left corner of the bitmap.
 * @param y The y-coordinate of the top-left corner of the bitmap.
 * @param bitmap A pointer to the 16-bit RGB bitmap data.
 * @param mask A pointer to the mask data.
 * @param w The width of the bitmap.
 * @param h The height of the bitmap.
 */
void Image::draw16bitRGBBitmapWithMask(int16_t x, int16_t y, uint16_t *bitmap, uint8_t *mask, int16_t w, int16_t h)
{
  if (!bitmap || !mask) return;  // Proteção contra ponteiros nulos
  
  DEBUG_D("Drawing image with mask embed function");
  int32_t offset = 0, maskIdx = 0;
  uint8_t byte = 0;
  WidgetBase::objTFT->startWrite();
  for (int16_t j = 0; j < h; j++, y++)
  {
    //Serial.println("");
    for (int16_t i = 0; i < w; i++)
    {
      if (i & 7)
      {
        byte <<= 1;
      }
      else
      {
        byte = mask[maskIdx++];
      }
      if (byte & 0x80)
      {
        //Serial.printf("1");
        WidgetBase::objTFT->writePixel(x + i, y, bitmap[offset]);
      }
      else
      {
        //Serial.printf("0");
      }
      offset++;
    }
  }
  WidgetBase::objTFT->endWrite();
}

/**
 * @brief Redraws the image on the screen.
 * 
 * Handles loading and drawing the image from its source, which could be
 * embedded memory, SD card, or SPIFFS filesystem.
 */
void Image::redraw()
{
  return;
}

/**
 * @brief Forces the image to update.
 * 
 * Sets the update flag to trigger a redraw on the next cycle.
 */
void Image::forceUpdate()
{
  m_update = true;
}

/**
 * @brief Configures the Image widget with a source file.
 * @param _source Source location of the image file (SD, SPIFFS, or EMBED).
 * @param _path Path to the image file.
 * @param _cb Optional callback function to execute when the image is interacted with.
 * @param _angle Optional rotation angle in degrees.
 */
void Image::setup(SourceFile _source, const char *_path, functionCB_t _cb, float _angle)
{
  if (loaded) {
    log_w("Widget already loaded");
    return;
  }
  
  m_width = 0;
  m_height = 0;
  cb = _cb;
  m_update = true;
  m_source = _source;
  m_path = _path;
  m_angle = _angle;
  
  // Libera memória existente se existir
  if(m_pixels) {
    delete[] m_pixels;
    m_pixels = nullptr;
  }
  if(m_maskAlpha) {
    delete[] m_maskAlpha;
    m_maskAlpha = nullptr;
  }

  if(_path == nullptr) {
    log_e("Path is nullptr");
    return;
  }

  #if defined(DFK_SD)
  if (m_source == SourceFile::SD)
  {
    if (!WidgetBase::mySD)
    {
      log_e("SD not configured");
      return;
    }
    m_fs = WidgetBase::mySD;
  }
  else if (m_source == SourceFile::SPIFFS)
  {
    if (!SPIFFS.begin(false))
    {
      Serial.println("SPIFFS Mount Failed");
      return;
    }

    m_fs = &SPIFFS;
  }
#else
  if (m_source == SourceFile::SD)
  {
    log_w("SD not configured");
    return;
  }
  else if (m_source == SourceFile::SPIFFS)
  {
    m_fs = &SPIFFS;
  }
#endif

  if (!m_fs)
  {
    log_e("No source defined to find image");
    return;
  }
  DEBUG_D("Image setup with source %d", m_source);

  loaded = true;
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
void Image::setup(uint16_t *_pixels, uint16_t _width, uint16_t _height, uint8_t *_maskAlpha, float _angle, functionCB_t _cb)
{
  if (loaded) {
    log_w("Widget already loaded");
    return;
  }
  
  // Libera memória existente se existir
  if(m_pixels && m_source != SourceFile::EMBED) {
    delete[] m_pixels;
    m_pixels = nullptr;
  }
  if(m_maskAlpha && m_source != SourceFile::EMBED) {
    delete[] m_maskAlpha;
    m_maskAlpha = nullptr;
  }
  
  m_source = SourceFile::EMBED;
  m_pixels = _pixels;
  m_maskAlpha = _maskAlpha;
  m_width = _width;
  m_height = _height;
  m_angle = _angle;
  cb = _cb;

  if(_pixels == nullptr) {
    log_e("Pixels is nullptr");
    return;
  }

  DEBUG_D("Image setup with pixels (%d x %d)", _width, _height);

  loaded = true;
  m_update = true;
}


/**
 * @brief Configures the Image widget with a file source.
 * 
 * Sets up the image with a file source, path, callback function, and rotation angle.
 * 
 * @param config Configuration structure containing file source, path, callback function, and rotation angle.
 */
void Image::setup(const ImageFromFileConfig& config)
{
  setup(config.source, config.path, config.cb, config.angle);
}

/**
 * @brief Configures the Image widget with pixel data.
 * 
 * Sets up the image with pixel data, width, height, mask alpha, angle, and callback function.
 * 
 * @param config Configuration structure containing pixel data, width, height, mask alpha, angle, and callback function.
 */
void Image::setup(const ImageFromPixelsConfig& config)
{
  setup(config.pixels, config.width, config.height, config.maskAlpha, config.angle, config.cb);
}

