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
 * @brief Configuration structure for loading an image from pixels.
 *
 * This structure defines the configuration for loading an image from pixels.
 * It includes the pixels, the width and height of the image,
 * the mask alpha, and the callback function to be called when the image is
 * touched.
 */
struct ImageFromPixelsConfig {
#if defined(DISP_DEFAULT)
  const uint16_t *pixels; ///< Pointer to the pixel map data.
#elif defined(DISP_PCD8544) || defined(DISP_SSD1306) || defined(DISP_U8G2)
  const uint8_t *pixels; ///< Pointer to the pixel map data.
#endif
  uint16_t width;  ///< Width of the image.
  uint16_t height; ///< Height of the image.
  const uint8_t
      *maskAlpha; ///< Pointer to the alpha mask data for transparency.
  functionCB_t
      cb; ///< Callback function to be called when the image is touched.
  uint16_t backgroundColor; ///< Background color of the image.
  // float angle; ///< Rotation angle of the image.
};

/// @brief Represents an image widget that can load images from various sources.
class Image : public WidgetBase {
public:
  Image(uint16_t _x, uint16_t _y, uint8_t _screen);

  ~Image();

  bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) override;

  functionCB_t getCallbackFunc() override;

  void redraw();

  void drawBackground();

  void draw();

  void show() override;
  void hide() override;

  void forceUpdate();

  void setup(ImageFromFileConfig &config);
  void setup(ImageFromPixelsConfig &config);

private:
  bool m_showImage = true;
  uint16_t m_backgroundColor = 0x0000;
  pixel_t *m_pixels = nullptr; ///< Pointer to the pixel map data.
  uint8_t *m_maskAlpha = nullptr; ///< Pointer to the alpha mask data for transparency.
  SourceFile m_source = SourceFile::EMBED; ///< Source of the image file.
  fs::FS *m_fs = nullptr;
  const char *m_path = nullptr;            ///< Path to the image file.
  uint16_t m_width = 0;                    ///< Width of the image.
  uint16_t m_height = 0;                   ///< Height of the image.
  unsigned long m_myTime = 0; ///< Timestamp for handling timing-related functions.
  bool m_shouldRedraw = false; ///< Flag indicating if the Image should be redrawn.
  
  float m_angle = 0; ///< Rotation angle in degrees.
  
  void setup(SourceFile _source, const char *_path, functionCB_t _cb, float _angle);
  void setup(const pixel_t *_pixels, uint16_t _width, uint16_t _height, const uint8_t *_maskAlpha, float _angle, functionCB_t _cb);
  bool readFileFromDisk();

  void clearBuffers();
};

#endif
