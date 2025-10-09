#ifndef WTEXTBUTTON
#define WTEXTBUTTON

#include "../widgetbase.h"

/// @brief Configuration structure for TextButton
struct TextButtonConfig {
  uint16_t width;         ///< Width of the button
  uint16_t height;        ///< Height of the button
  uint16_t radius;        ///< Radius for the button's rounded corners
  uint16_t backgroundColor;  ///< Color displayed when the button is pressed
  uint16_t textColor;     ///< Color of the text displayed on the button
  const char* text;       ///< Text to display on the button
  functionCB_t callback;  ///< Callback function to execute when the button is pressed
};

/// @brief Represents a button widget with customizable text, colors, and rounded corners.
class TextButton : public WidgetBase
{
private:
  unsigned long m_myTime; ///< Timestamp for handling timing-related functions.
  uint16_t m_width; ///< Width of the TextButton.
  uint16_t m_height; ///< Height of the TextButton.
  uint16_t m_pressedColor; ///< Color displayed when the button is pressed.
  uint16_t m_textColor; ///< Color of the text displayed on the button.
  uint16_t m_radius; ///< Radius for the button's rounded corners.
  uint8_t m_offsetMargin = 5; ///< Margin offset for positioning the text within the button.
  const char* m_text; ///< Text displayed on the button.
  bool m_enabled = true; ///< Indicates whether the button is enabled or disabled.
  bool m_shouldRedraw = false; ///< Flag indicating if the TextButton should be redrawn.

  void start();
  void setup(uint16_t _width, uint16_t _height, uint16_t _radius, uint16_t _pressedColor, uint16_t _textColor, const char* _text, functionCB_t _cb);

public:
  TextButton(uint16_t _x, uint16_t _y, uint8_t _screen);
  ~TextButton();
  bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) override;
  functionCB_t getCallbackFunc() override;
  void redraw();
  void onClick();
  void setup(const TextButtonConfig& config);
  bool getEnabled();
  void setEnabled(bool newState);
  void show() override;
  void hide() override;
};

#endif