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
void slider_cb();


Arduino_ESP32DSIPanel *bus = nullptr;
Arduino_DSI_Display *tft = nullptr;
uint8_t rotationScreen = 0; // This value can be changed depending of orientation of your screen
DisplayFK myDisplay;
// Create global objects. Constructor is: xPos, yPos and indexScreen
CircularBar circload(155, 355, 0);
const uint8_t qtdCircBar = 1;
CircularBar *arrayCircularbar[qtdCircBar] = {&circload};
HSlider slider(35, 485, 0);
const uint8_t qtdHSlider = 1;
HSlider *arrayHslider[qtdHSlider] = {&slider};

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
    loadWidgets(); // This function is used to setup with widget individualy
    //WidgetBase::loadScreen = screen0; // Use this line to change between screens
    myDisplay.loadScreen(screen0);
    myDisplay.createTask(false, 3); // Initialize the task to read touch and draw
}

void loop(){
    circload.setValue(random(0, 100)); //Use this command to change widget value.
    delay(2000);
}

void drawCollunm(int startX, int startY, bool border, int gridSize){
  int steps = 1280 / gridSize;
  int stepCor = 255/steps;
  
  for(auto i = 0; i < steps; i++){
    uint16_t cor = stepCor * i;
    tft->fillRect(startX, startY + i * gridSize, gridSize, gridSize, cor);
    if(border){
      tft->drawRect(startX, startY + i * gridSize, gridSize, gridSize, 0x0);  
    }
  }
}

void screen0(){
    tft->fillScreen(CFK_WHITE);
    WidgetBase::backgroundColor = CFK_WHITE;
    tft->fillCircle(140, 100, 40, CFK_COLOR01);
    tft->drawCircle(140, 100, 40, CFK_COLOR01);
    tft->fillCircle(75, 185, 40, CFK_COLOR22);
    tft->drawCircle(75, 185, 40, CFK_COLOR01);
    tft->fillCircle(200, 190, 40, CFK_COLOR11);
    tft->drawCircle(200, 190, 40, CFK_COLOR01);

    //drawCollunm(400, 0, true, 5);
    

    
    
    myDisplay.drawWidgetsOnScreen(0);
}

// Configure each widgtes to be used
void loadWidgets(){
    CircularBarConfig configCirculaBar0 = {
            .radius = 59,
            .minValue = 0,
            .maxValue = 100,
            .startAngle = 0,
            .endAngle = 360,
            .thickness = 20,
            .color = CFK_COLOR04,
            .backgroundColor = CFK_WHITE,
            .textColor = CFK_BLACK,
            .backgroundText = CFK_WHITE,
            .showValue = true,
            .inverted = false
        };
    circload.setup(configCirculaBar0);
    myDisplay.setCircularBar(arrayCircularbar,qtdCircBar);

    HSliderConfig configHSlider0 = {
            .width = 251,
            .pressedColor = CFK_COLOR01,
            .minValue = 0,
            .maxValue = 100,
            .radius = 20,
            .callback = slider_cb
        };
    slider.setup(configHSlider0);
    myDisplay.setHSlider(arrayHslider,qtdHSlider);

}

void slider_cb(){
    int myValue = slider.getValue();
    Serial.print("New value for slider is: ");Serial.println(myValue);
}
