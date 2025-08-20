#ifndef WLINECHART
#define WLINECHART

#include "../widgetbase.h"
#include "../../fonts/RobotoRegular/RobotoRegular10pt7b.h"
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
  uint8_t m_dotRadius = 2;
  uint8_t m_minSpaceToShowDot = 10;

  uint32_t m_maxHeight; ///< Available height for plotting.
  uint32_t m_maxWidth;  ///< Available width for plotting.
  uint16_t m_maxAmountValues; ///< Maximum number of values to store in the chart.
  uint16_t m_amountPoints;
  float m_spaceBetweenPoints;

  uint8_t m_amountSeries;
  LineChartValue_t** m_values;
  uint16_t* m_colorsSeries;

  int16_t m_leftPadding; ///< Left padding for the plot area.
  uint8_t m_topBottomPadding; ///< Top and bottom padding for the plot area.
  int16_t m_aux; ///< Auxiliary variable for calculations.
  unsigned long m_myTime; ///< Timestamp for handling timing-related functions.
  bool m_blocked; ///< Indicates if the chart is currently blocked from updates.
  bool m_update; ///< Flag to indicate if the chart needs to be updated.

  int m_vmin; ///< Minimum value for the chart range.
  int m_vmax; ///< Maximum value for the chart range.
  
  uint32_t m_width; ///< Width of the chart.
  uint32_t m_height; ///< Height of the chart.
  uint16_t m_borderSize = 2; ///< Border size for the chart.

  uint16_t m_availableWidth; ///< Available width for the chart area.
  uint16_t m_availableheight; ///< Available height for the chart area.
  
  uint16_t m_yTovmin; ///< Mapping value for minimum Y.
  uint16_t m_yTovmax; ///< Mapping value for maximum Y.
  uint16_t m_verticalDivision; ///< Spacing between vertical grid lines.
  bool m_workInBackground; ///< Flag for background drawing.
  bool m_showZeroLine = true; ///< Flag to show the zero line on the chart.

  uint16_t m_mainLineColor; ///< Color of the primary line.
  uint16_t m_secondLineColor; ///< Color of the secondary line.
  uint16_t m_gridColor; ///< Color of the grid lines.
  uint16_t m_borderColor; ///< Color of the chart border.
  uint16_t m_backgroundColor; ///< Background color of the chart.
  uint16_t m_textColor; ///< Color of text displayed on the chart.

  bool m_boldLine = false; ///< Flag to make the line bold
  bool m_showDots = false; ///< Flag to show the dots on the line
  LineChartConfig m_config; ///< Pointer to the configuration structure

    #if defined(USING_GRAPHIC_LIB)
  const GFXfont *m_letra = nullptr; ///< Font used for text on the chart.
  #endif
  SemaphoreHandle_t m_mutex = nullptr; // Mutex para proteger acesso aos dados
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
    #if defined(USING_GRAPHIC_LIB)
  void setup(uint16_t _width, uint16_t _height, int _vmin, int _vmax, uint8_t _amountSeries, uint16_t* _colorsSeries, uint16_t _gridColor, uint16_t _borderColor, uint16_t _backgroundColor, uint16_t _textColor,  uint16_t _verticalDivision, bool _workInBackground, bool _showZeroLine, bool _boldLine, bool _showDots, uint16_t _maxPointsAmount, const GFXfont* _font);
#endif
public:
  static constexpr uint16_t SHOW_ALL = 9999;//Considering the max amount of points is 9999
  LineChart(uint16_t _x, uint16_t _y, uint8_t _screen);
  ~LineChart();
  bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) override;
  functionCB_t getCallbackFunc() override;
  void drawBackground();
  bool push(uint16_t serieIndex, int newValue);
  void redraw();
  void forceUpdate();
  void setup(const LineChartConfig& config);
};

#endif