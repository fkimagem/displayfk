// Include for plugins of chip 0
// Include external libraries and files
#include <SPI.h>
#include <Arduino_GFX_Library.h>
#include <displayfk.h>
// #define DFK_SD
// Defines for font and files
#define FORMAT_SPIFFS_IF_FAILED false
const int DISPLAY_W = 600;
const int DISPLAY_H = 1024;
const int DISP_FREQUENCY = 27000000;
const int TOUCH_MAP_X0 = 0;
const int TOUCH_MAP_X1 = 600;
const int TOUCH_MAP_Y0 = 0;
const int TOUCH_MAP_Y1 = 1024;
const bool TOUCH_SWAP_XY = true;
const bool TOUCH_INVERT_X = true;
const bool TOUCH_INVERT_Y = false;
const int DISP_RST = 27;
const int TOUCH_SCL = 8;
const int TOUCH_SDA = 7;
const int TOUCH_INT = -1;
const int TOUCH_RST = 22;
// Prototypes for each screen
void screen0();
void loadWidgets();
// Prototypes for callback functions

#define DISP_LED 23
Arduino_ESP32DSIPanel *bus = nullptr;
Arduino_DSI_Display *tft = nullptr;
const uint8_t rotationScreen = 3; // This value can be changed depending of orientation of your screen
DisplayFK myDisplay;

// create 7 'int count' with constexpr offset between 0 and 6
constexpr int offsetCount = 20;
int countGeral = 0;
const int countSize = 7;
float counters[countSize] = {
    0,0,0,0,0,0,0
};


void increaseCounters(){
    for (size_t i = 0; i < countSize; i++)
    {
        counters[i] = sin((countGeral + (offsetCount * i)) * DEGTORAD);
        counters[i] = counters[i] * 50 + 50;// De 0 a 100
    }
    countGeral++;
}

bool flagAtualizarTela = false;

TimerHandle_t despertador;
void vTimerCallback(TimerHandle_t xTimer)
{
    if(!flagAtualizarTela){
        flagAtualizarTela = true;      
    }
  
}

void iniciarDespertador(){
    despertador = xTimerCreate(
    "Timer200ms",                 // nome do timer (opcional)
    pdMS_TO_TICKS(200),           // período em ticks (100ms)
    pdTRUE,                       // pdTRUE = auto-reload (infinito)
    (void*)0,                     // ID opcional do timer
    vTimerCallback                // função de callback
  );

  if (despertador != NULL) {
    // Inicia o timer (sem bloqueio)
    xTimerStart(despertador, 0);
    Serial.println("Timer iniciado!");
  } else {
    Serial.println("Falha ao criar o timer!");
  }
}



void drawCorners(){
    if (!tft) return;
    // draw square with size 50px with inside text 0 to 3. Position order is: 0,0; width,0; width,height; 0,height
    const int size = 50;
    const uint16_t color = 0xF800;

    /*Rect_t points[4] = {
        {0, 0, size, size},
        {DISPLAY_W - size, 0, size, size},
        {DISPLAY_W - size, DISPLAY_H - size, size, size},
        {0, DISPLAY_H - size, size, size}
    };

    for(int i = 0; i < 4; i++){
        
        tft->fillRect(points[i].x, points[i].y, points[i].width, points[i].height, color);
        tft->setCursor(points[i].x + 10, points[i].y + 10);
        tft->setTextColor(CFK_WHITE);
        tft->print(i);
    }*/

    tft->setTextColor(CFK_WHITE);

    tft->fillRect(0,0,size,size,color);
    //tft->setCursor(0, 0);
    //tft->println("0");
    //tft->print("1");

    tft->fillRect(DISPLAY_W - size,0,size,size,color);
    //tft->setCursor(DISPLAY_W - size, 0);
    //tft->print("2");

    tft->fillRect(DISPLAY_W - size,DISPLAY_H - size,size,size,color);
    /*tft->setCursor(DISPLAY_W - size, DISPLAY_H - size);
    tft->print("3");*/

    tft->fillRect(0,DISPLAY_H - size,size,size,color);
    /*tft->setCursor(0, DISPLAY_H - size);
    tft->print("4");*/

}

void drawTestColor(){
    // create variable uint16_t for: red, gree, blue, yellow, cyan, magenta
    const uint16_t red = 0xF800;
    const uint16_t green = 0x07E0;
    const uint16_t blue = 0x001F;
    const uint16_t yellow = 0xFFE0;
    const uint16_t cyan = 0x07FF;
    const uint16_t magenta = 0xF81F;

    // draw circle in row with radius 20px, offset 20px, starting x = offset and y = half height if screen
    uint16_t colors[] = {red, green, blue, yellow, cyan, magenta};
    uint8_t colorLen = 6;
    const int radius = 20;
    const int offset = 20;
    const int halfHeight = DISPLAY_H / 2;

    const uint8_t raio = 50;
    int intervalo = 360 / colorLen;
    int centerX = DISPLAY_W / 2;
    int centerY = DISPLAY_H / 2;


    for(int i = 0; i < colorLen; i++){
        float angulo = i * intervalo;
        float radianos = angulo * DEGTORAD;
        int x = cos(radianos) * raio + centerX;
        int y = sin(radianos) * raio + centerY;
        tft->fillCircle(x, y, radius, colors[i]);
    }
}

void setup(){
    Serial.begin(115200);
    bus = new Arduino_ESP32DSIPanel(
    40 /* hsync_pulse_width */, 160 /* hsync_back_porch */, 160 /* hsync_front_porch */,
    10 /* vsync_pulse_width */, 23 /*vsync_back_porch  */, 12 /* vsync_front_porch */,
    48000000 /* prefer_speed */);
    tft = new Arduino_DSI_Display(
    1024 /* width */, 600 /* height */, bus, rotationScreen /* rotation */, true /* auto_flush */,
    27 /* RST */, jd9165_init_operations, sizeof(jd9165_init_operations) / sizeof(lcd_init_cmd_t));
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
    //iniciarDespertador();
}
void loop(){
    Serial.println("Loop");
    /*if(flagAtualizarTela){
        
        myDisplay.finishCustomDraw();// Libera a task do displayfk para desenha
        flagAtualizarTela = false;
        increaseCounters();
    }*/
    delay(1000);
}
void screen0(){
    tft->fillScreen(CFK_WHITE);
    WidgetBase::backgroundColor = CFK_GREY5;

    myDisplay.printText("Caldeira", 50, 50, 1, CFK_BLACK, CFK_RED, &RobotoBold10pt7b);

    drawCorners();
    drawTestColor();

    myDisplay.drawWidgetsOnScreen(0);
}
// Configure each widgtes to be used
void loadWidgets(){
    

}
