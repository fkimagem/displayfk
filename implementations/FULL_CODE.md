Codigo exemplo usando todos os widgets

```cpp
 /* Project setup:
    * MCU: ESP32S3_EMBEDDED
    * Display: ESP32-8048S043
    * Touch: GT911
    */
#include "iKeypng.h"
// Include for plugins of chip ESP32S3_EMBEDDED
// Include external libraries and files
#include <SPI.h>
#include <Arduino_GFX_Library.h>
#include <displayfk.h>
// #define DFK_SD
// Defines for font and files
#define FORMAT_SPIFFS_IF_FAILED false
const int DISPLAY_W = 800;
const int DISPLAY_H = 480;
const int TOUCH_MAP_X0 = 0;
const int TOUCH_MAP_X1 = 800;
const int TOUCH_MAP_Y0 = 0;
const int TOUCH_MAP_Y1 = 480;
const bool TOUCH_SWAP_XY = false;
const bool TOUCH_INVERT_X = false;
const bool TOUCH_INVERT_Y = false;
const int TOUCH_SCL = 20;
const int TOUCH_SDA = 19;
const int TOUCH_INT = -1;
const int TOUCH_RST = 38;
const uint8_t rotationScreen = 0; // This value can be changed depending of orientation of your screen
const bool isIPS = false; // Come display can use this as bigEndian flag
// Prototypes for each screen
void screen0();
void loadWidgets();
// Prototypes for callback functions
void touchaarea_cb();
void cirbtn_cb();
void rectbutton_cb();
void toggle_cb();
void slider_cb();
void textbox_cb();
void checkbox_cb();
void rotateccwpng_cb();
void spinbox_cb();
void textbutton_cb();
void callbackOfGroup_grupo1();
#define DISP_LED 2
Arduino_ESP32RGBPanel *bus = nullptr;
Arduino_RGB_Display *tft = nullptr;
DisplayFK myDisplay;
// Create global objects. Constructor is: xPos, yPos and indexScreen
CheckBox checkbox(228, 44, 0);
const uint8_t qtdCheckbox = 1;
CheckBox *arrayCheckbox[qtdCheckbox] = {&checkbox};
CircleButton cirbtn(345, 84, 0);
const uint8_t qtdCircleBtn = 1;
CircleButton *arrayCirclebtn[qtdCircleBtn] = {&cirbtn};
GaugeSuper widget(645, 450, 0);
const uint8_t qtdGauge = 1;
GaugeSuper *arrayGauge[qtdGauge] = {&widget};
const uint8_t qtdIntervalG0 = 4;
int range0[qtdIntervalG0] = {0,25,50,75};
uint16_t colors0[qtdIntervalG0] = {CFK_WHITE,CFK_COLOR05,CFK_COLOR04,CFK_COLOR01};
HSlider slider(217, 326, 0);
const uint8_t qtdHSlider = 1;
HSlider *arrayHslider[qtdHSlider] = {&slider};
Label vbar1(108, 405, 0);
Label text(233, 405, 0);
Label term1(408, 17, 0);
const uint8_t qtdLabel = 3;
Label *arrayLabel[qtdLabel] = {&vbar1, &text, &term1};
Led led(33, 379, 0);
const uint8_t qtdLed = 1;
Led *arrayLed[qtdLed] = {&led};
LineChart linechart(450, 88, 0);
const uint8_t qtdLineChart = 1;
LineChart *arrayLinechart[qtdLineChart] = {&linechart};
const uint8_t qtdLinesChart0 = 2;
uint16_t colorsChart0[qtdLinesChart0] = {CFK_BLACK,CFK_BLACK};
RadioGroup grupo1(0);
const uint8_t qtdRadioGroup = 1;
RadioGroup *arrayRadio[qtdRadioGroup] = {&grupo1};
radio_t radiosOfGroup_grupo1[2] = {{191, 370, 1, CFK_GREY3},{141, 370, 2, CFK_GREY3}};
RectButton rectbutton(623, 19, 0);
const uint8_t qtdRectBtn = 1;
RectButton *arrayRectbtn[qtdRectBtn] = {&rectbutton};
bool recbtn0_val = false; // Global variable that stores the value of the widget rectbutton
SpinBox spinbox(225, 229, 0);
const uint8_t qtdSpinbox = 1;
SpinBox *arraySpinbox[qtdSpinbox] = {&spinbox};
TextBox textbox(200, 158, 0);
const uint8_t qtdTextbox = 1;
TextBox *arrayTextbox[qtdTextbox] = {&textbox};
TextButton textbutton(433, 414, 0);
const uint8_t qtdTextButton = 1;
TextButton *arrayTextButton[qtdTextButton] = {&textbutton};
ToggleButton toggle(525, 26, 0);
const uint8_t qtdToggleBtn = 1;
ToggleButton *arrayTogglebtn[qtdToggleBtn] = {&toggle};
TouchArea touchaarea(20, 418, 0);
const uint8_t qtdTouchArea = 1;
TouchArea *arrayToucharea[qtdTouchArea] = {&touchaarea};
VAnalog vertalog(0, 141, 0);
const uint8_t qtdVAnalog = 1;
VAnalog *arrayVanalog[qtdVAnalog] = {&vertalog};
VBar vertbar(75, 52, 0);
const uint8_t qtdVBar = 1;
VBar *arrayVbar[qtdVBar] = {&vertbar};
Image rotateccwpng(58, 8, 0);
Image keypng(0, 8, 0);
const uint8_t qtdImagem = 2;
Image *arrayImagem[qtdImagem] = {&rotateccwpng, &keypng};
CircularBar circload(153, 164, 0);
const uint8_t qtdCircBar = 1;
CircularBar *arrayCircularbar[qtdCircBar] = {&circload};
Thermometer thermometer(406, 188, 0);
const uint8_t qtdThermometer = 1;
Thermometer *arrayThermometer[qtdThermometer] = {&thermometer};
void setup(){
    Serial.begin(115200);
    bus = new Arduino_ESP32RGBPanel(
    40 /* DE */, 41 /* VSYNC */, 39 /* HSYNC */, 42 /* PCLK */,
    45 /* R0 */, 48 /* R1 */, 47 /* R2 */, 21 /* R3 */, 14 /* R4 */,
    5 /* G0 */, 6 /* G1 */, 7 /* G2 */, 15 /* G3 */, 16 /* G4 */, 4 /* G5 */,
    8 /* B0 */, 3 /* B1 */, 46 /* B2 */, 9 /* B3 */, 1 /* B4 */,
    0 /* hsync_polarity */, 8 /* hsync_front_porch */, 4 /* hsync_pulse_width */, 8 /* hsync_back_porch */,
    0 /* vsync_polarity */, 8 /* vsync_front_porch */, 4 /* vsync_pulse_width */, 8 /* vsync_back_porch */,
    1 /* pclk_active_neg */, 16000000 /* prefer_speed */, isIPS /* useBigEndian */,
    0 /* de_idle_high */, 0 /* pclk_idle_high */, 0 /* bounce_buffer_size_px */);
    tft = new Arduino_RGB_Display(800, 480, bus, rotationScreen, true);
    tft->begin();
    #if defined(DISP_LED)
    pinMode(DISP_LED, OUTPUT);
    digitalWrite(DISP_LED, HIGH);
    #endif
    myDisplay.setDrawObject(tft); // Reference to object to draw on screen
    // Setup touch
    myDisplay.setTouchCorners(TOUCH_MAP_X0, TOUCH_MAP_X1, TOUCH_MAP_Y0, TOUCH_MAP_Y1);
    myDisplay.setInvertAxis(TOUCH_INVERT_X, TOUCH_INVERT_Y);
    myDisplay.setSwapAxis(TOUCH_SWAP_XY);
    myDisplay.startTouchGT911(DISPLAY_W, DISPLAY_H, rotationScreen, TOUCH_SDA, TOUCH_SCL, TOUCH_INT, TOUCH_RST);
    //myDisplay.enableTouchLog();
    loadWidgets(); // This function is used to setup with widget individualy
    myDisplay.loadScreen(screen0); // Use this line to change between screens
    myDisplay.createTask(false, 3); // Initialize the task to read touch and draw
}
void loop(){
    widget.setValue(random(0, 100)); //Use this command to change widget value.
    text.setText("Custom text"); //Use this command to change widget value.
    led.setState(true);
    linechart.push(0, random(0, 100)); //Use this line to add value in serie 0.
    linechart.push(1, random(0, 100)); //Use this line to add value in serie 1.
    // Use the variable recbtn0_val to check value/status of widget rectbutton]
    vertalog.setValue(random(0, 100), true); //Use this command to change widget value.
    vertbar.setValue(random(0, 100)); //Use this command to change widget value.
    circload.setValue(random(0, 100)); //Use this command to change widget value.
    thermometer.setValue(random(0, 100)); //Use this command to change widget value.
    delay(2000);
}
void screen0(){
    tft->fillScreen(CFK_WHITE);
    WidgetBase::backgroundColor = CFK_WHITE;
    tft->fillRect(108, 26, 40, 36, CFK_COLOR01);
    tft->drawRect(108, 26, 40, 36, CFK_COLOR01);
    //This screen has a/an rectRange
    //This screen has a/an label
    //This screen has a/an label
    //This screen has a/an checkbox
    //This screen has a/an circleButton
    //This screen has a/an ponteiroAnalog
    //This screen has a/an led
    //This screen has a/an grafico
    //This screen has a/an radiobutton
    //This screen has a/an radiobutton
    //This screen has a/an rectButton
    //This screen has a/an slider
    //This screen has a/an caixaTexto
    //This screen has a/an spinbox
    //This screen has a/an textButton
    //This screen has a/an toggleButton
    //This screen has a/an touchArea
    //This screen has a/an verticalAnalog
    //This screen has a/an circularBar
    //This screen has a/an thermometer
    //This screen has a/an imagem
    //This screen has a/an imagem
    //This screen has a/an label
    tft->fillRect(198, 5, 25, 18, CFK_WHITE);
    tft->drawRect(198, 5, 25, 18, CFK_BLACK);
    myDisplay.drawWidgetsOnScreen(0);
}
// Configure each widgtes to be used
void loadWidgets(){
    CheckBoxConfig configCheckbox0 = {
            .size = 21,
            .checkedColor = CFK_COLOR01,
            .uncheckedColor = CFK_GREY10,
            .weight = CheckBoxWeight::MEDIUM,
            .callback = checkbox_cb
        };
    checkbox.setup(configCheckbox0);
    myDisplay.setCheckbox(arrayCheckbox,qtdCheckbox);

    CircleButtonConfig configCircleButton0 = {
            .radius = 40,
            .pressedColor = CFK_COLOR01,
            .callback = cirbtn_cb
        };

    cirbtn.setup(configCircleButton0);
    myDisplay.setCircleButton(arrayCirclebtn,qtdCircleBtn);

    GaugeConfig configGauge0 = {
            .width = 170,
            .title = "Title",
            .intervals = range0,
            .colors = colors0,
            .amountIntervals = qtdIntervalG0,
            .minValue = 0,
            .maxValue = 100,
            .borderColor = CFK_COLOR01,
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
            .width = 109,
            .pressedColor = CFK_GREY3,
            .backgroundColor = CFK_WHITE,
            .minValue = 0,
            .maxValue = 100,
            .radius = 11,
            .callback = slider_cb
        };
    slider.setup(configHSlider0);
    myDisplay.setHSlider(arrayHslider,qtdHSlider);

    LabelConfig configLabel0 = {
            .text = "VBar 1",
            .fontFamily = &RobotoRegular10pt7b,
            .datum = TL_DATUM,
            .fontColor = CFK_COLOR01,
            .backgroundColor = CFK_WHITE,
            .prefix = "",
            .suffix = "%"
        };
    vbar1.setup(configLabel0);
    LabelConfig configLabel1 = {
            .text = "Custom text",
            .fontFamily = &RobotoRegular10pt7b,
            .datum = TL_DATUM,
            .fontColor = CFK_COLOR01,
            .backgroundColor = CFK_WHITE,
            .prefix = "",
            .suffix = ""
        };
    text.setup(configLabel1);
    LabelConfig configLabel2 = {
            .text = "Term 1",
            .fontFamily = &RobotoRegular10pt7b,
            .datum = TL_DATUM,
            .fontColor = CFK_COLOR13,
            .backgroundColor = CFK_WHITE,
            .prefix = "",
            .suffix = "C"
        };
    term1.setup(configLabel2);
    myDisplay.setLabel(arrayLabel,qtdLabel);

    LedConfig configLed0 = {
            .radius = 16,
            .colorOn = CFK_COLOR01,
            .colorOff = CFK_GREY3,
            .initialState = false
        };
    led.setup(configLed0);
    myDisplay.setLed(arrayLed,qtdLed);

    LineChartConfig configLineChart0 = {
            .width = 200,
            .height = 113,
            .minValue = 0,
            .maxValue = 100,
            .amountSeries = qtdLinesChart0,
            .colorsSeries = colorsChart0,
            .gridColor = CFK_BLACK,
            .borderColor = CFK_BLACK,
            .backgroundColor = CFK_GREY3,
            .textColor = CFK_WHITE,
            .verticalDivision = 10,
            .workInBackground = false,
            .showZeroLine = true,
            .boldLine = false,
            .showDots = false,
            .maxPointsAmount = LineChart::SHOW_ALL,
            .font = &RobotoRegular5pt7b,
            .subtitles = nullptr
        };
    linechart.setup(configLineChart0);
    myDisplay.setLineChart(arrayLinechart,qtdLineChart);

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
            .width = 71,
            .height = 26,
            .pressedColor = CFK_GREY3,
            .callback = rectbutton_cb
        };
rectbutton.setup(configRectButton0);
    myDisplay.setRectButton(arrayRectbtn,qtdRectBtn);

    SpinBoxConfig configSpinBox0 = {
            .width = 97,
            .height = 35,
            .step = 1,
            .minValue = 0,
            .maxValue = 100,
            .startValue = 50,
            .color = CFK_COLOR05,
            .textColor = CFK_BLACK,
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
            .letterColor = CFK_GREY3,
            .backgroundColor = CFK_WHITE,
            .startValue = "YOUR TEXT HERE...",
            .font = &RobotoRegular10pt7b,
            .funcPtr = screen0,
            .cb = textbox_cb
        };
    textbox.setup(configTextBox0);
    myDisplay.setTextbox(arrayTextbox,qtdTextbox);

    TextButtonConfig configTextButton0 = {
            .width = 84,
            .height = 36,
            .radius = 10,
            .backgroundColor = CFK_GREY3,
            .textColor = CFK_WHITE,
            .text = "Button",
            .fontFamily = &RobotoBold10pt7b,
            .callback = textbutton_cb
        };

    textbutton.setup(configTextButton0);
    myDisplay.setTextButton(arrayTextButton,qtdTextButton);

    ToggleButtonConfig configToggle0 = {
            .width = 41,
            .height = 21,
            .pressedColor = CFK_GREY3,
            .callback = toggle_cb
        };
    toggle.setup(configToggle0);
    myDisplay.setToggle(arrayTogglebtn,qtdToggleBtn);

    TouchAreaConfig configTouchArea0 = {
            .width = 37,
            .height = 32,
            .callback = touchaarea_cb
        };
    touchaarea.setup(configTouchArea0);
    myDisplay.setTouchArea(arrayToucharea,qtdTouchArea);

    VerticalAnalogConfig configVAnalog0 = {
            .width = 41,
            .height = 101,
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
            .width = 21,
            .height = 191,
            .filledColor = CFK_COLOR01,
            .minValue = 0,
            .maxValue = 100,
            .round = 10,
            .orientation = Orientation::VERTICAL,
            .subtitle = &vbar1
        };
    vertbar.setup(configVBar0);
    myDisplay.setVBar(arrayVbar,qtdVBar);

    ImageFromFileConfig configImage0 = {
            .source = SourceFile::SPIFFS,
            .path = "/RotateCCWpng.fki",
            .cb = rotateccwpng_cb,
            .backgroundColor = CFK_WHITE
        };
    rotateccwpng.setupFromFile(configImage0);
    ImageFromPixelsConfig configImage1 = {
            .pixels = iKeypngPixels,
            .width = iKeypngW,
            .height = iKeypngH,
            .maskAlpha = iKeypngMask,
            .cb = nullptr,
            .backgroundColor = CFK_WHITE
        };
    keypng.setupFromPixels(configImage1);
    myDisplay.setImage(arrayImagem,qtdImagem);

    CircularBarConfig configCirculaBar0 = {
            .radius = 40,
            .minValue = 0,
            .maxValue = 100,
            .startAngle = 0,
            .endAngle = 360,
            .thickness = 20,
            .color = CFK_COLOR03,
            .backgroundColor = CFK_COLOR05,
            .textColor = CFK_BLACK,
            .backgroundText = CFK_WHITE,
            .showValue = true,
            .inverted = false
        };
    circload.setup(configCirculaBar0);
    myDisplay.setCircularBar(arrayCircularbar,qtdCircBar);

    ThermometerConfig configThermometer0 = {
            .width = 26,
            .height = 125,
            .filledColor = CFK_COLOR13,
            .backgroundColor = CFK_WHITE,
            .markColor = CFK_BLACK,
            .minValue = 0,
            .maxValue = 100,
            .subtitle = &term1,
            .unit = "C",
            .decimalPlaces = 1
        };
    thermometer.setup(configThermometer0);
    myDisplay.setThermometer(arrayThermometer,qtdThermometer);

}
// This function is a callback of this element checkbox.
// You dont need call it. Make sure it is as short and quick as possible.
void checkbox_cb(){
    bool myValue = checkbox.getStatus();
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
// This function is a callback of radio buttons of group 1.
// You dont need call it. Make sure it is as short and quick as possible.
void callbackOfGroup_grupo1(){
    uint16_t selectedOption = grupo1.getSelected();
    Serial.print("New value for radiogroup is: ");Serial.println(selectedOption);
}
// This function is a callback of this element rectbutton.
// You dont need call it. Make sure it is as short and quick as possible.
void rectbutton_cb(){
    recbtn0_val = rectbutton.getStatus();
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
// This function is a callback of this element touchaarea.
// You dont need call it. Make sure it is as short and quick as possible.
void touchaarea_cb(){
    Serial.println("AreaTouch was clicked");
}
// This function is a callback of this element rotateccwpng.
// You dont need call it. Make sure it is as short and quick as possible.
void rotateccwpng_cb(){
    // Image RotateCCWpng clicked
    Serial.println("Image RotateCCWpng clicked");
}
```
