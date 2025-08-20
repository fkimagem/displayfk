#ifndef WGAUGESUPER
#define WGAUGESUPER

#include "../widgetbase.h"
#include "../../fonts/RobotoRegular/RobotoRegular10pt7b.h"

/// @brief Configuration structure for GaugeSuper
struct GaugeConfig {
  uint16_t width;                ///< Width of the gauge
  const char* title;             ///< Title displayed on the gauge
  const int* intervals;          ///< Array of interval values
  const uint16_t* colors;        ///< Array of colors corresponding to intervals
  uint8_t amountIntervals;       ///< Number of intervals and colors
  int minValue;                      ///< Minimum value of the gauge range
  int maxValue;                      ///< Maximum value of the gauge range
  uint16_t borderColor;          ///< Color of the gauge border
  uint16_t textColor;            ///< Color for text and value display
  uint16_t backgroundColor;      ///< Background color of the gauge
  uint16_t titleColor;           ///< Color of the title text
  uint16_t needleColor;          ///< Color of the needle
  uint16_t markersColor;      ///< Color of the markers
  bool showLabels;               ///< Flag to show text labels for intervals
  #if defined(USING_GRAPHIC_LIB)
  const GFXfont* fontFamily;     ///< Font used for text rendering
  #endif
};

/// @brief Represents a gauge widget with a needle and color-coded intervals.
class GaugeSuper : public WidgetBase
{
private:
  float m_ltx = 0; ///< Last calculated tangent for the needle angle.
  CoordPoint_t m_lastPointNeedle; ///< Coordinates of the needle's end point.
  uint16_t m_stripColor; ///< Color for the colorful strip.
  uint16_t m_indexCurrentStrip; ///< Index for the current color strip segment.
  int m_divisor; ///< Multiplier for range values (used to display values >999).
  unsigned long m_myTime; ///< Timestamp for updating the needle.
  bool m_update; ///< Flag indicating if the needle should be updated.
  bool m_isFirstDraw; ///< Flag indicating if the gauge is being drawn for the first time.
  bool m_showLabels; ///< Flag to show text labels for intervals.
  bool m_showTitle; ///< Flag to display the title on the gauge.
  int *m_intervals = nullptr; ///< Array of interval values.
  uint16_t *m_colors = nullptr; ///< Array of colors corresponding to intervals.
  int m_vmin; ///< Minimum value of the gauge range.
  int m_vmax; ///< Maximum value of the gauge range.
  uint32_t m_width; ///< Width of the gauge rectangle.
  uint32_t m_height; ///< Height of the gauge rectangle.
  uint32_t m_radius; ///< Radius of the gauge circle.
  char *m_title = nullptr; ///< Title displayed on the gauge.
  uint16_t m_amountInterval; ///< Number of intervals and colors.
  int m_lastValue; ///< Last value displayed by the needle.
  int m_value; ///< Current value to display on the gauge.
  uint16_t m_borderColor; ///< Color of the gauge border.
  uint16_t m_titleColor; ///< Color of the title text.
  uint16_t m_textColor; ///< Color for text and line chart.
  uint16_t m_bkColor; ///< Background color of the gauge.
  uint16_t m_agulhaColor; ///< Color of the needle.
  uint16_t m_marcadoresColor; ///< Color of the markers.

  int m_stripWeight = 16; ///< Width of the colored strip.
  int m_maxAngle = 40; ///< Half of the total angle in degrees of the gauge.
  int m_offsetYAgulha = 40; ///< Y-offset for the needle's pivot point.
  int m_rotation = 90; ///< Rotation angle of the gauge (0 = middle right).
  int m_distanceAgulhaArco = 2; ///< Distance from the needle end point to the arc.
  uint8_t m_borderSize = 5; ///< Width of the border.
  uint16_t m_availableWidth; ///< Available width for drawing.
  uint16_t m_availableHeight; ///< Available height for drawing.
  #if defined(USING_GRAPHIC_LIB)
  const GFXfont* m_usedFont = nullptr; ///< Font used for text rendering.
  #endif
  TextBound_t m_textBoundForValue; ///< Bounding box for the displayed value text.

  CoordPoint_t m_origem; ///< Center of the gauge clock.
  
  void start();
  #if defined(USING_GRAPHIC_LIB)
  void setup(uint16_t width, const char *title, const int *intervals, const uint16_t *colors, uint8_t amountIntervals, int vmin, int vmax, uint16_t borderColor, uint16_t textColor, uint16_t backgroundColor, uint16_t titleColor, uint16_t agulhaColor, uint16_t marcadoresColor, bool showLabels, const GFXfont *_fontFamily);
  #endif
public:
  
  GaugeSuper(uint16_t _x, uint16_t _y, uint8_t _screen);
  
  ~GaugeSuper();

  bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) override;

  functionCB_t getCallbackFunc() override;

  void drawBackground();

  void setValue(int newValue);

  void redraw();

  void forceUpdate();

  void setup(const GaugeConfig& config);
};

#endif