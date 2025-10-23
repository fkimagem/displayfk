#ifndef WLINECHART
#define WLINECHART

#include "../widgetbase.h"
#if defined(USING_GRAPHIC_LIB)
#include "../../fonts/RobotoRegular/RobotoRegular10pt7b.h"
#endif
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "../label/wlabel.h" // Para ponteiro Label

typedef struct {
  int currentValue;
  int oldValue;
} LineChartValue_t;

/// @brief Configuration structure for LineChart
struct LineChartConfig {
  uint16_t width;                ///< Width of the chart
  uint16_t height;               ///< Height of the chart
  int minValue;                      ///< Minimum value for the chart range
  int maxValue;                      ///< Maximum value for the chart range
  uint8_t amountSeries;          ///< Number of series to plot
  uint16_t* colorsSeries;        ///< Array of colors for each series
  uint16_t gridColor;            ///< Color of the grid lines
  uint16_t borderColor;          ///< Color of the chart border
  uint16_t backgroundColor;      ///< Background color of the chart
  uint16_t textColor;            ///< Color of text displayed on the chart
  uint16_t verticalDivision;     ///< Spacing between vertical grid lines
  bool workInBackground;         ///< Flag for background drawing
  bool showZeroLine;             ///< Flag to show the zero line on the chart
  bool boldLine;                 ///< Flag to make the line bold
  bool showDots;                 ///< Flag to show the dots on the line
  uint16_t maxPointsAmount;      ///< Maximum number of points to show on the chart
    #if defined(USING_GRAPHIC_LIB)
  const GFXfont* font;           ///< Font used for text on the chart
  #endif
  Label** subtitles;             ///< Array of pointers to Label for each series (can be nullptr)
};

/// @brief Represents a line chart widget for plotting data with optional secondary line.
class LineChart : public WidgetBase
{
private:
  static const char* TAG;                          ///< Logging tag
  static constexpr uint8_t MAX_SERIES = 10;        ///< Maximum number of series allowed
  
  
  
  // Internal state (specific to this widget, not in config)
  uint8_t m_dotRadius = 2;                         ///< Radius of dots on the line
  uint8_t m_minSpaceToShowDot = 10;                ///< Minimum space between points to show dots
  uint32_t m_maxHeight;                            ///< Available height for plotting
  uint32_t m_maxWidth;                             ///< Available width for plotting
  uint16_t m_maxAmountValues;                      ///< Maximum number of values to store in the chart
  uint16_t m_amountPoints;                         ///< Current number of points in the chart
  float m_spaceBetweenPoints;                      ///< Calculated spacing between points
  int16_t m_leftPadding;                           ///< Left padding for the plot area
  uint8_t m_topBottomPadding;                      ///< Top and bottom padding for the plot area
  int16_t m_aux;                                   ///< Auxiliary variable for calculations
  bool m_blocked;                                  ///< Indicates if the chart is currently blocked from updates
  uint16_t m_borderSize = 2;                       ///< Border size for the chart
  uint16_t m_availableWidth;                       ///< Available width for the chart area
  uint16_t m_availableheight;                      ///< Available height for the chart area
  uint16_t m_yTovmin;                              ///< Mapping value for minimum Y
  uint16_t m_yTovmax;                              ///< Mapping value for maximum Y
  bool m_shouldRedraw = false;                     ///< Flag to indicate if the chart should be redrawn
  
  // Memory management with tracking
  bool m_valuesAllocated = false;                  ///< Track if values array was allocated
  bool m_colorsSeriesAllocated = false;            ///< Track if colors array was allocated
  LineChartValue_t** m_values = nullptr;           ///< 2D array of values for all series
  uint16_t m_colorsSeries[MAX_SERIES];             ///< Internal storage for colors (deep copy)
  Label* m_subtitles[MAX_SERIES] = {nullptr};      ///< Internal storage for subtitle pointers
  
  SemaphoreHandle_t m_mutex = nullptr;             ///< Mutex to protect data access

  // Configuration
  LineChartConfig m_config;                        ///< Configuration for the LineChart widget
  
  // Helper methods
  void cleanupMemory();                            ///< Centralized memory cleanup
  void clearColorsSeries();
  void clearSubtitles();
  void clearValues();
  bool validateConfig(const LineChartConfig& config); ///< Configuration validation
  void initMutex();
  void destroyMutex();

  void start();
  void resetArray();
  void printValues(uint8_t serieIndex);
  void drawGrid();
  void clearPreviousValues();
  void drawMarkLineAt(int value);
  void drawSerie(uint8_t serieIndex);
  void drawAllSeries();
  void copyCurrentValuesToOldValues();
public:
  static constexpr uint16_t SHOW_ALL = 9999;//Considering the max amount of points is 9999
  LineChart(uint16_t _x, uint16_t _y, uint8_t _screen);
  ~LineChart();
  bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) override;
  functionCB_t getCallbackFunc() override;
  void drawBackground();
  bool push(uint16_t serieIndex, int newValue);
  void redraw() override;
  void forceUpdate() override;
  void setup(const LineChartConfig& config);
  void show() override;
  void hide() override;
};

#endif