#include "3dgraphpng.h"
#include "Homepng.h"
// Include for plugins of chip 0
#if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
#define VSPI FSPI
#endif
// Include external libraries and files
#include <SPI.h>
#include <Arduino_GFX_Library.h>
#include <displayfk.h>

// #define DFK_SD
// Defines for font and files
#define FORMAT_SPIFFS_IF_FAILED false
const int DISPLAY_W = 800;
const int DISPLAY_H = 1280;
const int DISP_FREQUENCY = 27000000;
const int TOUCH_MAP_X0 = 0;
const int TOUCH_MAP_Y0 = 0;
const int TOUCH_MAP_X1 = 800;
const int TOUCH_MAP_Y1 = 1280;
const bool TOUCH_SWAP_XY = false;
const bool TOUCH_INVERT_X = false;
const bool TOUCH_INVERT_Y = false;

const int DISP_LED = 23;

const int TOUCH_SCL = 8;//8
const int TOUCH_SDA = 7;//7
const int TOUCH_INT = 21;
const int TOUCH_RST = 22;


// Prototypes for each screen
void screen0();
void loadWidgets();

// Prototypes for callback functions
void cirbtn_cb();
void rectbutton_cb();
void toggle_cb();
void slider_cb();
void textbox_cb();
void checkbox_cb();
void checkbox1_cb();
void numberbox_cb();
void spinbox_cb();
void textbutton_cb();
void callbackOfGroup_grupo1();
void homepng_cb();


Arduino_ESP32DSIPanel *bus = nullptr;
Arduino_DSI_Display *tft = nullptr;
uint8_t rotationScreen = 0; // This value can be changed depending of orientation of your screen
DisplayFK myDisplay;
// Create global objects. Constructor is: xPos, yPos and indexScreen
// Create global objects. Constructor is: xPos, yPos and indexScreen
CheckBox checkbox(200, 10, 0);
CheckBox checkbox1(200, 42, 0);
const uint8_t qtdCheckbox = 2;
CheckBox *arrayCheckbox[qtdCheckbox] = {&checkbox, &checkbox1};
CircleButton cirbtn(280, 45, 0);
const uint8_t qtdCircleBtn = 1;
CircleButton *arrayCirclebtn[qtdCircleBtn] = {&cirbtn};
GaugeSuper widget(315, 800, 0);
const uint8_t qtdGauge = 1;
GaugeSuper *arrayGauge[qtdGauge] = {&widget};
const uint8_t qtdIntervalG0 = 4;
int range0[qtdIntervalG0] = {0,25,50,75};
uint16_t colors0[qtdIntervalG0] = {CFK_COLOR09,CFK_COLOR06,CFK_COLOR03,CFK_COLOR01};
HSlider slider(320, 75, 0);
const uint8_t qtdHSlider = 1;
HSlider *arrayHslider[qtdHSlider] = {&slider};
Label text(60, 40, 0);
Label line1(65, 455, 0);
Label line2(180, 455, 0);
const uint8_t qtdLabel = 3;
Label *arrayLabel[qtdLabel] = {&text, &line1, &line2};
Led led(365, 30, 0);
const uint8_t qtdLed = 1;
Led *arrayLed[qtdLed] = {&led};
LineChart linechart(20, 500, 0);
const uint8_t qtdLineChart = 1;
LineChart *arrayLinechart[qtdLineChart] = {&linechart};
Label *seriesGrafico0[2] = {&line1,&line2};
const uint8_t qtdLinesChart0 = 2;
uint16_t colorsChart0[qtdLinesChart0] = {CFK_COLOR09,CFK_COLOR04};
NumberBox numberbox(110, 240, 0);
const uint8_t qtdNumberbox = 1;
NumberBox *arrayNumberbox[qtdNumberbox] = {&numberbox};
RadioGroup grupo1(0);
const uint8_t qtdRadioGroup = 1;
RadioGroup *arrayRadio[qtdRadioGroup] = {&grupo1};
radio_t radiosOfGroup_grupo1[2] = {{278, 108, 1, CFK_COLOR28},{235, 110, 2, CFK_COLOR28}};
RectButton rectbutton(403, 19, 0);
const uint8_t qtdRectBtn = 1;
RectButton *arrayRectbtn[qtdRectBtn] = {&rectbutton};
SpinBox spinbox(50, 90, 0);
const uint8_t qtdSpinbox = 1;
SpinBox *arraySpinbox[qtdSpinbox] = {&spinbox};
TextBox textbox(10, 140, 0);
const uint8_t qtdTextbox = 1;
TextBox *arrayTextbox[qtdTextbox] = {&textbox};
TextButton textbutton(20, 175, 0);
const uint8_t qtdTextButton = 1;
TextButton *arrayTextButton[qtdTextButton] = {&textbutton};
ToggleButton toggle(165, 140, 0);
const uint8_t qtdToggleBtn = 1;
ToggleButton *arrayTogglebtn[qtdToggleBtn] = {&toggle};
VAnalog vertalog(430, 129, 0);
const uint8_t qtdVAnalog = 1;
VAnalog *arrayVanalog[qtdVAnalog] = {&vertalog};
VBar vertbar(390, 130, 0);
const uint8_t qtdVBar = 1;
VBar *arrayVbar[qtdVBar] = {&vertbar};
CircularBar circload(335, 215, 0);
const uint8_t qtdCircBar = 1;
CircularBar *arrayCircularbar[qtdCircBar] = {&circload};
Thermometer thermometer(250, 165, 0);
const uint8_t qtdThermometer = 1;
Thermometer *arrayThermometer[qtdThermometer] = {&thermometer};
Image img3dgraphpng(135, 400, 0);
Image homepng(190, 400, 0);
const uint8_t qtdImagem = 2;
Image *arrayImagem[qtdImagem] = {&img3dgraphpng, &homepng};


void setup(){
    Serial.begin(115200);
    bus = new Arduino_ESP32DSIPanel(
    20 /* hsync_pulse_width */, 20 /* hsync_back_porch */, 40 /* hsync_front_porch */,
    4 /* vsync_pulse_width */, 8 /*vsync_back_porch  */, 20 /* vsync_front_porch */,
    60000000 /* prefer_speed */);

    tft = new Arduino_DSI_Display(
    800 /* width */, 1280 /* height */, bus, rotationScreen /* rotation */, true /* auto_flush */,
    27 /* RST */, jd9365_init_operations, sizeof(jd9365_init_operations) / sizeof(lcd_init_cmd_t));

    pinMode(DISP_LED, OUTPUT);
    digitalWrite(DISP_LED, HIGH);

    tft->begin();
    //WidgetBase::objTFT = tft; // Reference to object to draw on screen
    myDisplay.setDrawObject(tft);
    delay(100);
    // Setup touch
    myDisplay.setTouchCorners(TOUCH_MAP_X0, TOUCH_MAP_X1, TOUCH_MAP_Y0, TOUCH_MAP_Y1);
    myDisplay.setInvertAxis(TOUCH_INVERT_X, TOUCH_INVERT_Y);
    myDisplay.setSwapAxis(TOUCH_SWAP_XY);
    myDisplay.startTouchGSL3680(DISPLAY_W, DISPLAY_H, rotationScreen, TOUCH_SDA, TOUCH_SCL, TOUCH_INT, TOUCH_RST);
    myDisplay.enableTouchLog();
    //myDisplay.setupAutoClick(2000, 100, 100); // Setup auto click
    //myDisplay.startAutoClick(); // Start auto click
    //loadWidgets(); // This function is used to setup with widget individualy
    //WidgetBase::loadScreen = screen0; // Use this line to change between screens
    myDisplay.loadScreen(screen0);
    myDisplay.createTask(false, 3); // Initialize the task to read touch and draw
}

void loop(){
    int r = random(0,100);
    int j = random(0,100);
    myDisplay.startCustomDraw();
    widget.setValue(r); //Use this command to change widget value.
    text.setTextInt(r); //Use this command to change widget value.
    led.setState(r > j);
    linechart.push(0, r); //Use this line to add value in serie 0.
    linechart.push(1, j); //Use this line to add value in serie 1.
    vertalog.setValue(r, true); //Use this command to change widget value.
    vertbar.setValue(r); //Use this command to change widget value.
    circload.setValue(r); //Use this command to change widget value.
    thermometer.setValue(r); //Use this command to change widget value.
    myDisplay.finishCustomDraw();
    delay(2000);
}

// Conversão HSV → RGB565 (versão otimizada)
uint16_t hsvToRgb565Fast(float h, float s, float v) {
  float r, g, b;
  int i = int(h * 6.0f);
  float f = h * 6.0f - i;
  float p = v * (1.0f - s);
  float q = v * (1.0f - f * s);
  float t = v * (1.0f - (1.0f - f) * s);

  switch (i % 6) {
    case 0: r = v; g = t; b = p; break;
    case 1: r = q; g = v; b = p; break;
    case 2: r = p; g = v; b = t; break;
    case 3: r = p; g = q; b = v; break;
    case 4: r = t; g = p; b = v; break;
    default: r = v; g = p; b = q; break;
  }

  uint16_t R = (uint16_t)(r * 31.0f);
  uint16_t G = (uint16_t)(g * 63.0f);
  uint16_t B = (uint16_t)(b * 31.0f);
  return (R << 11) | (G << 5) | B;
}

void drawHueCircleDarkCenter(int16_t cx, int16_t cy, uint16_t radius) {
  const int32_t r2 = radius * radius;

  tft->startWrite();

  for (int16_t dy = -radius; dy <= radius; dy++) {
    int32_t y2 = dy * dy;
    for (int16_t dx = -radius; dx <= radius; dx++) {
      int32_t d2 = dx * dx + y2;
      if (d2 > r2) continue; // fora do círculo

      // Distância normalizada do centro (0.0 no centro, 1.0 na borda)
      float dist = sqrtf((float)d2) / radius;

      // Matiz (ângulo)
      float hue = (atan2f(dy, dx) + PI) / (2.0f * PI);

      // Saturação sempre máxima (1.0), brilho (v) depende da distância
      float s = 1.0f;
      float v = dist; // 0 no centro (preto), 1 na borda

      uint16_t color = hsvToRgb565Fast(hue, s, v);
      tft->writePixel(cx + dx, cy + dy, color);
    }
  }

  tft->endWrite();
}

void drawHueCircle(int16_t cx, int16_t cy, uint16_t radius) {
  const int32_t r2 = radius * radius;

  tft->startWrite();

  for (int16_t dy = -radius; dy <= radius; dy++) {
    int32_t y2 = dy * dy;
    for (int16_t dx = -radius; dx <= radius; dx++) {
      int32_t d2 = dx * dx + y2;
      if (d2 > r2) continue;

      float s = sqrtf((float)d2 / (float)r2);           // saturação (0–1)
      float hue = (atan2f(dy, dx) + PI) / (2.0f * PI);  // matiz (0–1)

      uint16_t color = hsvToRgb565Fast(hue, s, 1.0f);
      tft->writePixel(cx + dx, cy + dy, color);
    }
  }

  tft->endWrite();
}


void screen0(){
   tft->fillScreen(CFK_WHITE);
    WidgetBase::backgroundColor = CFK_WHITE;
    tft->fillCircle(16, 16, 9, CFK_COLOR01);
    tft->drawCircle(16, 16, 9, CFK_BLACK);
    tft->fillCircle(30, 15, 9, CFK_COLOR11);
    tft->drawCircle(30, 15, 9, CFK_BLACK);
    tft->fillCircle(25, 25, 9, CFK_COLOR22);
    tft->drawCircle(25, 25, 9, CFK_BLACK);
    tft->fillCircle(14, 50, 9, CFK_COLOR06);
    tft->drawCircle(14, 50, 9, CFK_BLACK);
    tft->fillCircle(28, 49, 9, CFK_COLOR17);
    tft->drawCircle(28, 49, 9, CFK_BLACK);
    tft->fillCircle(23, 59, 9, CFK_COLOR28);
    tft->drawCircle(23, 59, 9, CFK_BLACK);
    myDisplay.printText("Custom text", 60, 12, TL_DATUM, CFK_BLACK, CFK_WHITE, &RobotoRegular10pt7b);


    drawHueCircleDarkCenter(400, 300, 255);
    drawHueCircle(400, 1000, 255);
    
    myDisplay.drawWidgetsOnScreen(0);
}

// Configure each widgtes to be used
void loadWidgets(){
  Serial.println("------------- START LOAD WIDGETS --------------------");
    CheckBoxConfig configCheckbox0 = {
            .size = 21,
            .checkedColor = CFK_COLOR28,
            .uncheckedColor = CFK_GREY7,
            .weight = CheckBoxWeight::MEDIUM,
            .callback = checkbox_cb
        };
    checkbox.setup(configCheckbox0);
    CheckBoxConfig configCheckbox1 = {
            .size = 21,
            .checkedColor = CFK_COLOR03,
            .uncheckedColor = CFK_GREY10,
            .weight = CheckBoxWeight::HEAVY,
            .callback = checkbox1_cb
        };
    checkbox1.setup(configCheckbox1);
    myDisplay.setCheckbox(arrayCheckbox,qtdCheckbox);

    CircleButtonConfig configCircleButton0 = {
            .radius = 40,
            .pressedColor = CFK_COLOR15,
            .callback = cirbtn_cb
        };

    cirbtn.setup(configCircleButton0);
    myDisplay.setCircleButton(arrayCirclebtn,qtdCircleBtn);

    GaugeConfig configGauge0 = {
            .width = 205,
            .title = "Titulo",
            .intervals = range0,
            .colors = colors0,
            .amountIntervals = qtdIntervalG0,
            .minValue = 0,
            .maxValue = 100,
            .borderColor = CFK_COLOR03,
            .textColor = CFK_BLACK,
            .backgroundColor = CFK_WHITE,
            .titleColor = CFK_BLACK,
            .needleColor = CFK_RED,
            .markersColor = CFK_BLACK,
            .showLabels = true,
            .fontFamily = &RobotoBold10pt7b
        };
    widget.setup(configGauge0);
    myDisplay.setGauge(arrayGauge,qtdGauge);

    HSliderConfig configHSlider0 = {
            .width = 155,
            .pressedColor = CFK_COLOR20,
            .backgroundColor = CFK_COLOR22,
            .minValue = 0,
            .maxValue = 100,
            .radius = 17,
            .callback = slider_cb
        };
    slider.setup(configHSlider0);
    myDisplay.setHSlider(arrayHslider,qtdHSlider);

    LabelConfig configLabel0 = {
            .text = "Custom text",
            .fontFamily = &RobotoRegular10pt7b,
            .datum = TL_DATUM,
            .fontColor = CFK_BLACK,
            .backgroundColor = CFK_WHITE,
            .prefix = "",
            .suffix = ""
        };
    text.setup(configLabel0);
    LabelConfig configLabel1 = {
            .text = "Line 1",
            .fontFamily = &RobotoRegular10pt7b,
            .datum = TL_DATUM,
            .fontColor = CFK_COLOR09,
            .backgroundColor = CFK_WHITE,
            .prefix = "Line",
            .suffix = ""
        };
    line1.setup(configLabel1);
    LabelConfig configLabel2 = {
            .text = "Line 2",
            .fontFamily = &RobotoRegular10pt7b,
            .datum = TL_DATUM,
            .fontColor = CFK_COLOR04,
            .backgroundColor = CFK_WHITE,
            .prefix = "Line",
            .suffix = "C"
        };
    line2.setup(configLabel2);
    myDisplay.setLabel(arrayLabel,qtdLabel);

    LedConfig configLed0 = {
            .radius = 16,
            .colorOn = CFK_COLOR01,
            .colorOff = CFK_BLACK
        };
    led.setup(configLed0);
    myDisplay.setLed(arrayLed,qtdLed);

    LineChartConfig configLineChart0 = {
            .width = 500,
            .height = 113,
            .minValue = 0,
            .maxValue = 100,
            .amountSeries = qtdLinesChart0,
            .colorsSeries = colorsChart0,
            .gridColor = CFK_COLOR01,
            .borderColor = CFK_BLACK,
            .backgroundColor = CFK_GREY4,
            .textColor = CFK_COLOR19,
            .verticalDivision = 10,
            .workInBackground = false,
            .showZeroLine = true,
            .boldLine = false,
            .showDots = false,
            .maxPointsAmount = LineChart::SHOW_ALL,
            .font = &RobotoRegular5pt7b,
            .subtitles = seriesGrafico0
        };
    linechart.setup(configLineChart0);
    myDisplay.setLineChart(arrayLinechart,qtdLineChart);

    Numpad::m_backgroundColor = CFK_GREY3;
    Numpad::m_letterColor = CFK_BLACK;
    Numpad::m_keyColor = CFK_GREY13;

    NumberBoxConfig configNumberBox0 = {
            .width = 101,
            .height = 25,
            .letterColor = CFK_COLOR01,
            .backgroundColor = CFK_WHITE,
            .startValue = 1234.56,
            .font = &RobotoRegular10pt7b,
            .funcPtr = screen0,
            .callback = numberbox_cb
        };
    numberbox.setup(configNumberBox0);
    myDisplay.setNumberbox(arrayNumberbox,qtdNumberbox);

    RadioGroupConfig configRadioGroup0 = {
            .group = 1,
            .radius = 10,
            .amount = 2,
            .buttons = radiosOfGroup_grupo1,
            .defaultClickedId = 1,
            .callback = callbackOfGroup_grupo1
        };
    grupo1.setup(configRadioGroup0);
    myDisplay.setRadioGroup(arrayRadio,qtdRadioGroup);

    RectButtonConfig configRectButton0 = {
            .width = 60,
            .height = 38,
            .pressedColor = CFK_COLOR05,
            .callback = rectbutton_cb
        };
rectbutton.setup(configRectButton0);
    myDisplay.setRectButton(arrayRectbtn,qtdRectBtn);

    SpinBoxConfig configSpinBox0 = {
            .width = 129,
            .height = 36,
            .step = 1,
            .minValue = 0,
            .maxValue = 100,
            .startValue = 50,
            .color = CFK_COLOR22,
            .textColor = CFK_WHITE,
            .callback = spinbox_cb
        };
    spinbox.setup(configSpinBox0);
    myDisplay.setSpinbox(arraySpinbox,qtdSpinbox);

    WKeyboard::m_backgroundColor = CFK_GREY3;
    WKeyboard::m_letterColor = CFK_BLACK;
    WKeyboard::m_keyColor = CFK_GREY13;

    TextBoxConfig configTextBox0 = {
            .width = 141,
            .height = 25,
            .letterColor = CFK_COLOR28,
            .backgroundColor = CFK_WHITE,
            .startValue = "YOUR TEXT HERE...",
            .font = &RobotoRegular10pt7b,
            .funcPtr = screen0,
            .cb = textbox_cb
        };
    textbox.setup(configTextBox0);
    myDisplay.setTextbox(arrayTextbox,qtdTextbox);

    TextButtonConfig configTextButton0 = {
            .width = 107,
            .height = 48,
            .radius = 10,
            .backgroundColor = CFK_COLOR25,
            .textColor = CFK_WHITE,
            .text = "Button",
            .callback = textbutton_cb
        };

    textbutton.setup(configTextButton0);
    myDisplay.setTextButton(arrayTextButton,qtdTextButton);

    ToggleButtonConfig configToggle0 = {
            .width = 66,
            .height = 33,
            .pressedColor = CFK_COLOR25,
            .callback = toggle_cb
        };
    toggle.setup(configToggle0);
    myDisplay.setToggle(arrayTogglebtn,qtdToggleBtn);

    VerticalAnalogConfig configVAnalog0 = {
            .width = 41,
            .height = 132,
            .minValue = 0,
            .maxValue = 100,
            .steps = 10,
            .arrowColor = CFK_COLOR01,
            .textColor = CFK_BLACK,
            .backgroundColor = CFK_WHITE,
            .borderColor = CFK_BLACK
        };
    vertalog.setup(configVAnalog0);
    myDisplay.setVAnalog(arrayVanalog,qtdVAnalog);

    VerticalBarConfig configVBar0 = {
            .width = 25,
            .height = 131,
            .filledColor = CFK_COLOR18,
            .minValue = 0,
            .maxValue = 100,
            .round = 0,
            .orientation = Orientation::VERTICAL
        };
    vertbar.setup(configVBar0);
    myDisplay.setVBar(arrayVbar,qtdVBar);

    CircularBarConfig configCirculaBar0 = {
            .radius = 40,
            .minValue = 0,
            .maxValue = 100,
            .startAngle = 0,
            .endAngle = 360,
            .thickness = 10,
            .color = CFK_COLOR09,
            .backgroundColor = CFK_WHITE,
            .textColor = CFK_BLACK,
            .backgroundText = CFK_WHITE,
            .showValue = true,
            .inverted = false
        };
    circload.setup(configCirculaBar0);
    myDisplay.setCircularBar(arrayCircularbar,qtdCircBar);

    ThermometerConfig configThermometer0 = {
            .width = 30,
            .height = 95,
            .filledColor = CFK_COLOR04,
            .backgroundColor = CFK_WHITE,
            .markColor = CFK_BLACK,
            .minValue = 0,
            .maxValue = 100,
            .subtitle = nullptr,
            .unit = "C",
            .decimalPlaces = 1
        };
    thermometer.setup(configThermometer0);
    myDisplay.setThermometer(arrayThermometer,qtdThermometer);

    ImageFromPixelsConfig configImage0 = {
            .pixels = img3dgraphpngPixels,
            .width = img3dgraphpngW,
            .height = img3dgraphpngH,
            .maskAlpha = img3dgraphpngMask,
            .cb = nullptr,
            .backgroundColor = CFK_WHITE
        };
    img3dgraphpng.setupFromPixels(configImage0);
    ImageFromPixelsConfig configImage1 = {
            .pixels = HomepngPixels,
            .width = HomepngW,
            .height = HomepngH,
            .maskAlpha = HomepngMask,
            .cb = homepng_cb,
            .backgroundColor = CFK_WHITE
        };
    homepng.setupFromPixels(configImage1);
    myDisplay.setImage(arrayImagem,qtdImagem);

    Serial.println("------------- END LOAD WIDGETS --------------------");
}
 // This function is a callback of this element checkbox.
// You dont need call it. Make sure it is as short and quick as possible.
void checkbox_cb(){
    bool myValue = checkbox.getStatus();
    Serial.print("New value for checkbox is: ");Serial.println(myValue);
}
// This function is a callback of this element checkbox1.
// You dont need call it. Make sure it is as short and quick as possible.
void checkbox1_cb(){
    bool myValue = checkbox1.getStatus();
    Serial.print("New value for checkbox is: ");Serial.println(myValue);
}
// This function is a callback of this element cirbtn.
// You dont need call it. Make sure it is as short and quick as possible.
void cirbtn_cb(){
    bool myValue = cirbtn.getStatus();
    Serial.print("New value for circlebutton is: ");Serial.println(myValue);
}
// This function is a callback of this element slider.
// You dont need call it. Make sure it is as short and quick as possible.
void slider_cb(){
    int myValue = slider.getValue();
    Serial.print("New value for slider is: ");Serial.println(myValue);
}
// This function is a callback of this element numberbox.
// You dont need call it. Make sure it is as short and quick as possible.
void numberbox_cb(){
    float myValue = numberbox.getValue();
    Serial.print("New value for numberbox is: ");Serial.println(myValue);
}
// This function is a callback of radio buttons of group 1.
// You dont need call it. Make sure it is as short and quick as possible.
void callbackOfGroup_grupo1(){
    uint16_t selectedOption = grupo1.getSelected();
    Serial.print("New value for radiogroup is: ");Serial.println(selectedOption);
}
// This function is a callback of this element rectbutton.
// You dont need call it. Make sure it is as short and quick as possible.
void rectbutton_cb(){
    bool myValue = rectbutton.getStatus();
    Serial.print("New value for rectbutton is: ");Serial.println(myValue);
}
// This function is a callback of this element spinbox.
// You dont need call it. Make sure it is as short and quick as possible.
void spinbox_cb(){
    int myValue = spinbox.getValue();
    Serial.print("New value for spinbox is: ");Serial.println(myValue);
}
// This function is a callback of this element textbox.
// You dont need call it. Make sure it is as short and quick as possible.
void textbox_cb(){
    const char* myValue = textbox.getValue();
    Serial.print("New value for textbox is: ");Serial.println(myValue);
}
// This function is a callback of this element textbutton.
// You dont need call it. Make sure it is as short and quick as possible.
void textbutton_cb(){
    Serial.print("Clicked on: ");Serial.println("textbutton_cb");
}
// This function is a callback of this element toggle.
// You dont need call it. Make sure it is as short and quick as possible.
void toggle_cb(){
    bool myValue = toggle.getStatus();
    Serial.print("New value for toggle is: ");Serial.println(myValue);
}


void homepng_cb(){
    // Image Homepng clicked
    Serial.println("Image Homepng clicked");
}
