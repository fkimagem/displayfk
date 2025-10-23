#ifndef WIMAGE
#define WIMAGE

#include "../widgetbase.h"
#if defined(DFK_SD)
#include "SD.h"
#endif
#include "SPIFFS.h"
#include <FS.h>

/// @brief Enum for specifying the source file location of the image.
enum class SourceFile {
  SD = 0,     ///< Image source from SD card.
  SPIFFS = 1, ///< Image source from SPIFFS filesystem.
  EMBED = 2   ///< Image source embedded within the code.
};

#if defined(DISP_DEFAULT)
  typedef uint16_t pixel_t;
#else
  typedef uint8_t pixel_t;
#endif
/**
 * @brief Configuration structure for loading an image from a file.
 *
 * This structure defines the configuration for loading an image from a file.
 * It includes the source file location, the path to the image file,
 * the callback function to be called when the image is touched,
 * and the rotation angle of the image.
 */
struct ImageFromFileConfig {
  SourceFile source; ///< Source of the image file.
  const char *path;  ///< Path to the image file.
  functionCB_t
      cb; ///< Callback function to be called when the image is touched.
  uint16_t backgroundColor; ///< Background color of the image.
  // float angle; ///< Rotation angle of the image.
  String toString(){
    return String((int)source) + " " + path;
  }
};

/**
 * @brief Configuration structure for loading an image from pixel data.
 *
 * This structure defines the configuration for loading an image from pixel data.
 * It includes the pixel data, dimensions, alpha mask, and callback function.
 * 
 * @note The pixel data format depends on the display type:
 *       - DISP_DEFAULT: 16-bit RGB565 format
 *       - DISP_PCD8544/SSD1306/U8G2: 8-bit monochrome format
 */
struct ImageFromPixelsConfig {
  const pixel_t *pixels;        ///< Pointer to the pixel map data (format depends on display type)
  uint16_t width;               ///< Width of the image (must be > 0)
  uint16_t height;              ///< Height of the image (must be > 0)
  const uint8_t *maskAlpha;     ///< Pointer to the alpha mask data for transparency (optional)
  functionCB_t cb;              ///< Callback function to be called when the image is touched
  uint16_t backgroundColor;      ///< Background color of the image
  float angle;                  ///< Rotation angle of the image in degrees (0.0 = no rotation)
  
  /**
   * @brief Validates the configuration structure
   * @return true if configuration is valid, false otherwise
   */
  bool isValid() const {
    if (pixels == nullptr) return false;
    if (width == 0 || height == 0) return false;
    if (width > 4096 || height > 4096) return false; // Reasonable limits
    return true;
  }
  
  /**
   * @brief Gets the total number of pixels
   * @return Total pixel count
   */
  uint32_t getPixelCount() const {
    return static_cast<uint32_t>(width) * static_cast<uint32_t>(height);
  }
};
typedef struct  {
  uint32_t drawCount = 0;           ///< Number of times draw() was called
  uint32_t fileLoadCount = 0;       ///< Number of times file was loaded
  uint32_t rotationDrawCount = 0;   ///< Number of times rotated image was drawn
  uint32_t totalDrawTime = 0;       ///< Total time spent in draw() (microseconds)
  uint32_t totalFileLoadTime = 0;   ///< Total time spent loading files (microseconds)
  uint32_t totalRotationTime = 0;   ///< Total time spent in rotation (microseconds)
  uint32_t maxDrawTime = 0;         ///< Maximum single draw time (microseconds)
  uint32_t maxFileLoadTime = 0;     ///< Maximum single file load time (microseconds)
  uint32_t maxRotationTime = 0;     ///< Maximum single rotation time (microseconds)
  uint32_t lastDrawTime = 0;        ///< Last draw time (microseconds)
  uint32_t lastFileLoadTime = 0;    ///< Last file load time (microseconds)
  uint32_t lastRotationTime = 0;    ///< Last rotation time (microseconds)
  
  void reset() {
    drawCount = 0;
    fileLoadCount = 0;
    rotationDrawCount = 0;
    totalDrawTime = 0;
    totalFileLoadTime = 0;
    totalRotationTime = 0;
    maxDrawTime = 0;
    maxFileLoadTime = 0;
    maxRotationTime = 0;
    lastDrawTime = 0;
    lastFileLoadTime = 0;
    lastRotationTime = 0;
  }
  
  float getAverageDrawTime() const {
    return drawCount > 0 ? (float)totalDrawTime / drawCount : 0.0f;
  }
  
  float getAverageFileLoadTime() const {
    return fileLoadCount > 0 ? (float)totalFileLoadTime / fileLoadCount : 0.0f;
  }
  
  float getAverageRotationTime() const {
    return rotationDrawCount > 0 ? (float)totalRotationTime / rotationDrawCount : 0.0f;
  }
} PerformanceMetrics_t;

/// @brief Represents an image widget that can load images from various sources.
class Image : public WidgetBase {
public:
  Image(uint16_t _x, uint16_t _y, uint8_t _screen);

  ~Image();

  bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) override;

  functionCB_t getCallbackFunc() override;

  void redraw() override;

  void drawBackground();

  void draw();

  void show() override;
  void hide() override;

  void forceUpdate() override;

  void setupFromFile(ImageFromFileConfig &config);
  void setupFromPixels(ImageFromPixelsConfig &config);
  
  // Performance metrics access
  const PerformanceMetrics_t& getMetrics() const { return m_metrics; }
  void resetMetrics() { m_metrics.reset(); }
  void printMetrics() const;

private:
  static const char* TAG; ///< Tag for logging identification
  
  // Unified configuration - single source of truth
  ImageFromPixelsConfig m_config;
  bool m_ownsMemory = false; ///< Flag to control memory ownership (true = we allocated, false = external reference)
  
  // Performance metrics
  
  
  // Legacy variables (kept for readFileFromDisk compatibility)
  pixel_t *m_pixels = nullptr; ///< Pointer to the pixel map data (legacy)
  uint8_t *m_maskAlpha = nullptr; ///< Pointer to the alpha mask data for transparency (legacy)
  SourceFile m_source = SourceFile::EMBED; ///< Source of the image file (legacy)
  fs::FS *m_fs = nullptr; ///< File system pointer (legacy)
  const char *m_path = nullptr; ///< Path to the image file (legacy)
  
  // Display state managed by WidgetBase::m_visible

  PerformanceMetrics_t m_metrics;
  
  bool readFileFromDisk();
  void defineFileSystem(SourceFile source);
  void drawRotatedImage();
  bool validateConfig();
  void clearBuffers();
};

#endif