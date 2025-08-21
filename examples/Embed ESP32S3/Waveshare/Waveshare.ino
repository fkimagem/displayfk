// #define DFK_SD
// Defines for font and files
#define FORMAT_SPIFFS_IF_FAILED false
#define DISPLAY_W 800
#define DISPLAY_H 480
#define TC_SCL	9
#define TC_SDA	8
#define TC_INT	-1
#define TC_RST	-1
#define TOUCH_ROTATION	ROTATION_NORMAL
#define TOUCH_MAP_X0	0
#define TOUCH_MAP_X1	800
#define TOUCH_MAP_Y0	0
#define TOUCH_MAP_Y1	480
#define TOUCH_SWAP_XY	false
#define TOUCH_INVERT_X	false
#define TOUCH_INVERT_Y	false

#define EXAMPLE_LCD_NAME                        ST7262 // LCD model name
#define EXAMPLE_LCD_WIDTH                       (800) // LCD width in pixels
#define EXAMPLE_LCD_HEIGHT                      (480) // LCD height in pixels
#define EXAMPLE_LCD_COLOR_BITS                  (24)  // Color depth in bits
#define EXAMPLE_LCD_RGB_DATA_WIDTH              (16)  // Width of RGB data
#define EXAMPLE_LCD_RGB_COLOR_BITS          (16)    // |      24      |      16       |

#define EXAMPLE_LCD_RGB_TIMING_FREQ_HZ          (16 * 1000 * 1000) // RGB timing frequency
#define EXAMPLE_LCD_RGB_TIMING_HPW              (4)   // Horizontal pulse width
#define EXAMPLE_LCD_RGB_TIMING_HBP              (8)   // Horizontal back porch
#define EXAMPLE_LCD_RGB_TIMING_HFP              (8)   // Horizontal front porch
#define EXAMPLE_LCD_RGB_TIMING_VPW              (4)   // Vertical pulse width
#define EXAMPLE_LCD_RGB_TIMING_VBP              (8)   // Vertical back porch
#define EXAMPLE_LCD_RGB_TIMING_VFP              (8)   // Vertical front porch
#define EXAMPLE_LCD_RGB_BOUNCE_BUFFER_SIZE  (EXAMPLE_LCD_WIDTH * 10)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// Please update the following configuration according to your board spec ////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define EXAMPLE_LCD_RGB_IO_DISP            (-1)  // RGB display pin number
#define EXAMPLE_LCD_RGB_IO_VSYNC           (3)   // VSYNC pin number
#define EXAMPLE_LCD_RGB_IO_HSYNC           (46)  // HSYNC pin number
#define EXAMPLE_LCD_RGB_IO_DE              (5)   // Data enable pin number
#define EXAMPLE_LCD_RGB_IO_PCLK            (7)   // Pixel clock pin number
#define EXAMPLE_LCD_RGB_IO_DATA0           (14)  // RGB data pin 0
#define EXAMPLE_LCD_RGB_IO_DATA1           (38)  // RGB data pin 1
#define EXAMPLE_LCD_RGB_IO_DATA2           (18)  // RGB data pin 2
#define EXAMPLE_LCD_RGB_IO_DATA3           (17)  // RGB data pin 3
#define EXAMPLE_LCD_RGB_IO_DATA4           (10)  // RGB data pin 4
#define EXAMPLE_LCD_RGB_IO_DATA5           (39)  // RGB data pin 5
#define EXAMPLE_LCD_RGB_IO_DATA6           (0)   // RGB data pin 6
#define EXAMPLE_LCD_RGB_IO_DATA7           (45)  // RGB data pin 7
#if EXAMPLE_LCD_RGB_DATA_WIDTH > 8
#define EXAMPLE_LCD_RGB_IO_DATA8           (48)  // RGB data pin 8
#define EXAMPLE_LCD_RGB_IO_DATA9           (47)  // RGB data pin 9
#define EXAMPLE_LCD_RGB_IO_DATA10          (21)  // RGB data pin 10
#define EXAMPLE_LCD_RGB_IO_DATA11          (1)   // RGB data pin 11
#define EXAMPLE_LCD_RGB_IO_DATA12          (2)   // RGB data pin 12
#define EXAMPLE_LCD_RGB_IO_DATA13          (42)  // RGB data pin 13
#define EXAMPLE_LCD_RGB_IO_DATA14          (41)  // RGB data pin 14
#define EXAMPLE_LCD_RGB_IO_DATA15          (40)  // RGB data pin 15
#endif
#define EXAMPLE_LCD_RST_IO                 (-1)  // Reset pin number
#define EXAMPLE_LCD_BL_IO            (-1)  // Backlight pin number
#define EXAMPLE_LCD_BL_ON_LEVEL           (1)   // Backlight ON level
#define EXAMPLE_LCD_BL_OFF_LEVEL !EXAMPLE_LCD_BL_ON_LEVEL // Backlight OFF level

// Prototypes for each screen
void screen0();
void loadWidgets();

// Prototypes for callback functions

// Include for plugins of chip 1
#if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
#define VSPI FSPI
#endif
// Include external libraries and files
#include <SPI.h>
#include <Arduino_GFX_Library.h>
#include <displayfk.h>

// Create global objects. Constructor is: xPos, yPos and indexScreen
// Create global objects. Constructor is: xPos, yPos and indexScreen
#define DISP_LED 2
Arduino_ESP32RGBPanel *rgbpanel = nullptr;
Arduino_RGB_Display *tft = nullptr;
uint8_t rotationScreen = 1;
DisplayFK myDisplay;

void setup(){
    Serial.begin(115200);
    rgbpanel = new Arduino_ESP32RGBPanel(
    EXAMPLE_LCD_RGB_IO_DE, EXAMPLE_LCD_RGB_IO_VSYNC, EXAMPLE_LCD_RGB_IO_HSYNC, EXAMPLE_LCD_RGB_IO_PCLK,
    EXAMPLE_LCD_RGB_IO_DATA0, EXAMPLE_LCD_RGB_IO_DATA1, EXAMPLE_LCD_RGB_IO_DATA2, EXAMPLE_LCD_RGB_IO_DATA3, EXAMPLE_LCD_RGB_IO_DATA4,
    EXAMPLE_LCD_RGB_IO_DATA5, EXAMPLE_LCD_RGB_IO_DATA6, EXAMPLE_LCD_RGB_IO_DATA7, EXAMPLE_LCD_RGB_IO_DATA8, EXAMPLE_LCD_RGB_IO_DATA9, EXAMPLE_LCD_RGB_IO_DATA10,
    EXAMPLE_LCD_RGB_IO_DATA11, EXAMPLE_LCD_RGB_IO_DATA12, EXAMPLE_LCD_RGB_IO_DATA13, EXAMPLE_LCD_RGB_IO_DATA14, EXAMPLE_LCD_RGB_IO_DATA15,
    0, EXAMPLE_LCD_RGB_TIMING_HFP, EXAMPLE_LCD_RGB_TIMING_HPW, EXAMPLE_LCD_RGB_TIMING_HBP,
    0, EXAMPLE_LCD_RGB_TIMING_VFP, EXAMPLE_LCD_RGB_TIMING_VPW, EXAMPLE_LCD_RGB_TIMING_VBP,
    1, EXAMPLE_LCD_RGB_TIMING_FREQ_HZ, false,
    0, 0, EXAMPLE_LCD_RGB_BOUNCE_BUFFER_SIZE);
    tft = new Arduino_RGB_Display(DISPLAY_W, DISPLAY_H, rgbpanel, 0 , true);
    tft->begin();
    #if defined(DISP_LED)
    pinMode(DISP_LED, OUTPUT);
    digitalWrite(DISP_LED, HIGH);
    #endif
    WidgetBase::objTFT = tft; // Reference to object to draw on screen
    // Setup touch
    myDisplay.setTouchCorners(TOUCH_MAP_X0, TOUCH_MAP_X1, TOUCH_MAP_Y0, TOUCH_MAP_Y1);
    myDisplay.setInvertAxis(TOUCH_INVERT_X, TOUCH_INVERT_Y);
    myDisplay.setSwapAxis(TOUCH_SWAP_XY);
    myDisplay.startTouchGT911(DISPLAY_W, DISPLAY_H, rotationScreen, TC_SDA, TC_SCL, TC_INT, TC_RST);
    //myDisplay.setupAutoClick(2000, 100, 100); // Setup auto click
    //myDisplay.startAutoClick(); // Start auto click
    loadWidgets(); // This function is used to setup with widget individualy
    WidgetBase::loadScreen = screen0; // Use this line to change between screens
    myDisplay.createTask(false, 3); // Initialize the task to read touch and draw
}

void loop(){
    delay(2000);
}

void screen0(){
    tft->fillScreen(CFK_WHITE);
    WidgetBase::backgroundColor = CFK_WHITE;
    tft->fillRect(155, 80, 49, 53, CFK_COLOR01);
    tft->drawRect(155, 80, 49, 53, CFK_COLOR01);
    tft->fillCircle(345, 210, 40, CFK_COLOR15);
    tft->drawCircle(345, 210, 40, CFK_COLOR01);
    myDisplay.printText("Custom text", 450, 307, TL_DATUM, CFK_COLOR01, CFK_WHITE, &RobotoRegular10pt7b);
    myDisplay.drawWidgetsOnScreen(0);
}

// Configure each widgtes to be used
void loadWidgets(){
}
