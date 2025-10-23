#ifndef WGAUGESUPER
#define WGAUGESUPER

#include "../widgetbase.h"
#if defined(USING_GRAPHIC_LIB)
#include "../../fonts/RobotoRegular/RobotoRegular10pt7b.h"
#endif

/// @brief Configuration structure for GaugeSuper
struct GaugeConfig {
  uint16_t width;                ///< Width of the gauge
  const char* title;             ///< Title displayed on the gauge
  const int* intervals;          ///< Array of interval values
  const uint16_t* colors;        ///< Array of colors corresponding to intervals
  uint8_t amountIntervals;       ///< Number of intervals and colors
  int minValue;                  ///< Minimum value of the gauge range
  int maxValue;                  ///< Maximum value of the gauge range
  uint16_t borderColor;          ///< Color of the gauge border
  uint16_t textColor;            ///< Color for text and value display
  uint16_t backgroundColor;      ///< Background color of the gauge
  uint16_t titleColor;           ///< Color of the title text
  uint16_t needleColor;          ///< Color of the needle
  uint16_t markersColor;         ///< Color of the markers
  bool showLabels;               ///< Flag to show text labels for intervals
  #if defined(USING_GRAPHIC_LIB)
  const GFXfont* fontFamily;     ///< Font used for text rendering
  #endif
};

/// @brief Represents a gauge widget with a needle and color-coded intervals.
class GaugeSuper : public WidgetBase
{
private:
  static constexpr uint8_t MAX_SERIES = 10;  // Define according to your needs/limits
  static constexpr uint8_t MAX_TITLE_LENGTH = 20;  // Maximum title length
  static const char* TAG; ///< Tag for logging identification
  
  // Configuration
  GaugeConfig m_config; ///< Configuration for the GaugeSuper
  
  // Internal state
  float m_ltx = 0; ///< Last calculated tangent for the needle angle
  CoordPoint_t m_lastPointNeedle; ///< Coordinates of the needle's end point
  uint16_t m_stripColor; ///< Color for the colorful strip
  uint16_t m_indexCurrentStrip; ///< Index for the current color strip segment
  int m_divisor; ///< Multiplier for range values (used to display values >999)
  bool m_isFirstDraw; ///< Flag indicating if the gauge is being drawn for the first time
  
  // Dynamic arrays (managed carefully to avoid memory leaks)
  int *m_intervals = nullptr; ///< Array of interval values
  uint16_t *m_colors = nullptr; ///< Array of colors corresponding to intervals
  char *m_title = nullptr; ///< Title displayed on the gauge
  
  // Memory management flags
  bool m_intervalsAllocated = false; ///< Flag to track intervals allocation
  bool m_colorsAllocated = false; ///< Flag to track colors allocation
  bool m_titleAllocated = false; ///< Flag to track title allocation
  
  // Calculated dimensions
  uint32_t m_height; ///< Height of the gauge rectangle
  uint32_t m_radius; ///< Radius of the gauge circle
  int m_currentValue; ///< Current value to display on the gauge
  int m_lastValue; ///< Last value displayed by the needle
  
  // Drawing parameters
  int m_stripWeight = 16; ///< Width of the colored strip
  int m_maxAngle = 40; ///< Half of the total angle in degrees of the gauge
  int m_offsetYAgulha = 40; ///< Y-offset for the needle's pivot point
  int m_rotation = 90; ///< Rotation angle of the gauge (0 = middle right)
  int m_distanceAgulhaArco = 2; ///< Distance from the needle end point to the arc
  uint8_t m_borderSize = 5; ///< Width of the border
  uint16_t m_availableWidth; ///< Available width for drawing
  uint16_t m_availableHeight; ///< Available height for drawing
  
  #if defined(USING_GRAPHIC_LIB)
  const GFXfont* m_usedFont = nullptr; ///< Font used for text rendering
  #endif
  TextBound_t m_textBoundForValue; ///< Bounding box for the displayed value text
  CoordPoint_t m_origem; ///< Center of the gauge clock
  
  void start();
  void cleanupMemory();
  bool validateConfig(const GaugeConfig& config);
  bool isTitleVisible() const;
  bool isLabelsVisible() const;
  
public:
  GaugeSuper(uint16_t _x, uint16_t _y, uint8_t _screen);
  virtual ~GaugeSuper();

  bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) override;
  functionCB_t getCallbackFunc() override;
  void redraw() override;
  void forceUpdate() override;
  void show() override;
  void hide() override;
  
  void setup(const GaugeConfig& config);
  void drawBackground();
  void setValue(int newValue);
};

#endif