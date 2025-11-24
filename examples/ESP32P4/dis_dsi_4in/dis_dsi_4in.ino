// Include for plugins of chip 0
// Include external libraries and files
#include <SPI.h>
#include <Arduino_GFX_Library.h>
#include <displayfk.h>
// #define DFK_SD
// Defines for font and files
#define FORMAT_SPIFFS_IF_FAILED false
const int DISPLAY_W = 800;
const int DISPLAY_H = 480;
const int DISP_FREQUENCY = 27000000;
const int TOUCH_MAP_X0 = 0;
const int TOUCH_MAP_X1 = 800;
const int TOUCH_MAP_Y0 = 0;
const int TOUCH_MAP_Y1 = 480;
const bool TOUCH_SWAP_XY = true;
const bool TOUCH_INVERT_X = false;
const bool TOUCH_INVERT_Y = true;
const int DISP_RST = 5;
const int TOUCH_SCL = 8;
const int TOUCH_SDA = 7;
const int TOUCH_INT = -1;
const int TOUCH_RST = -1;
const uint8_t rotationScreen = 1; // This value can be changed depending of orientation of your screen

// Prototypes for each screen
void screen0();
void loadWidgets();
// Prototypes for callback functions
// Prototypes for callback functions

#define DISP_LED 23
Arduino_ESP32DSIPanel *bus = nullptr;
Arduino_DSI_Display *tft = nullptr;

DisplayFK myDisplay;

// Create global objects. Constructor is: xPos, yPos and indexScreen
// create 7 'int count' with constexpr offset between 0 and 6
constexpr int offsetCount = 20;
int countGeral = 0, auxCountGeral = 0;
const int countSize = 8;
float counters[countSize] = {
    0,0,0,0,0,0,0,0
};

float getCurve(float x){
  //y\ =\ \sin\left(x\right)+\sin\left(3x\right)+\left(2\cdot\sin\left(x\right)\right)

  float result = sin(x * DEGTORAD) + sin(3 * x * DEGTORAD);

  // current result is from -3 to 3, need to normatize value from 0 to 100.
  // so need mutiply by 30 and add 30
  result = result * 25 + 50;

  return result;
}

void increaseCounters(){
    for (size_t i = 0; i < countSize; i++)
    {
        counters[i] = sin((countGeral + (offsetCount * i)) * DEGTORAD);
        counters[i] = counters[i] * 50 + 50;// De 0 a 100
    }
    counters[7] = getCurve(auxCountGeral);
    countGeral+=5;
    auxCountGeral++;
}

void drawHueCircleDark(int16_t cx, int16_t cy, uint16_t radius) {
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

void drawHueCircleLight(int16_t cx, int16_t cy, uint16_t radius) {
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



void testColorScreen(){
    if (!tft) return;
    tft->fillScreen(CFK_BLACK);
    drawHueCircleLight(DISPLAY_W / 2, DISPLAY_H / 2, 250);
    delay(2000);
    tft->fillScreen(CFK_WHITE);
    drawHueCircleDark(DISPLAY_W / 2, DISPLAY_H / 2, 250);
    delay(2000);
}

void setup(){
    Serial.begin(115200);
    bus = new Arduino_ESP32DSIPanel(
     12  /* hsync_pulse_width */, 
    42  /* hsync_back_porch */, 
    42  /* hsync_front_porch */,
    2   /* vsync_pulse_width */, 
    8   /* vsync_back_porch */, 
    166 /* vsync_front_porch */,
    34000000 /* prefer_speed - 34 MHz conforme configuração do projeto */);

    tft = new Arduino_DSI_Display(
    480 /* width */, 
    800 /* height */, 
    bus, 
    rotationScreen,      // rotation
    true,   // IPS
    DISP_RST,  // RST pin (DISP_RST)
    st7701_init_operations, 
    sizeof(st7701_init_operations) / sizeof(lcd_init_cmd_t));
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
    myDisplay.enableTouchLog();
    //myDisplay.setupAutoClick(2000, 100, 100); // Setup auto click
    //myDisplay.startAutoClick(); // Start auto click
    loadWidgets(); // This function is used to setup with widget individualy
    myDisplay.loadScreen(screen0); // Use this line to change between screens
    myDisplay.createTask(false, 3); // Initialize the task to read touch and draw
}
void loop(){
    delay(10);
}
void screen0(){
    tft->fillScreen(CFK_GREY3);
    WidgetBase::backgroundColor = CFK_GREY3;
    tft->fillScreen(CFK_BLACK);
    tft->drawRect(10, 10, 50, 50, CFK_RED);
    drawHueCircleLight(DISPLAY_W / 2, DISPLAY_H / 2, 100);
    myDisplay.drawWidgetsOnScreen(0);
}
// Configure each widgtes to be used
void loadWidgets(){

}