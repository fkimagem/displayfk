#ifndef WTOUCHAREA
#define WTOUCHAREA

#include "../widgetbase.h"

/// @brief Configuration structure for TouchArea
struct TouchAreaConfig {
  uint16_t width;         ///< Width of the TouchArea
  uint16_t height;        ///< Height of the TouchArea
  functionCB_t callback;  ///< Callback function to execute when the area is touched
};

/// @brief Represents a touch-sensitive area widget with customizable size and status tracking.
class TouchArea : public WidgetBase
{
private:
  unsigned long m_myTime; ///< Timestamp for handling timing-related functions.
  uint16_t m_width; ///< Width of the TouchArea.
  uint16_t m_height; ///< Height of the TouchArea.
  bool m_status; ///< Current status of the TouchArea.
  bool m_shouldRedraw = false; ///< Flag indicating if the TouchArea should be redrawn.

  void setup(uint16_t _width, uint16_t _height, functionCB_t _cb);

public:
  TouchArea(uint16_t _x, uint16_t _y, uint8_t _screen);
  ~TouchArea();
  bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) override;
  functionCB_t getCallbackFunc() override;
  void changeState();
  void redraw();
  void setup(const TouchAreaConfig& config);
  bool getStatus();
  void onClick();
  void hide() override;
  void show() override;
};

#endif