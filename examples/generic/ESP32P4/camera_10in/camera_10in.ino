/**
 * Captura MIPI-CSI na JC8012P4A1C (ESP32-P4 + câmera OV02C10 + tela JD9365).
 *
 * A cópia câmera → tela usa o PPA (DMA 2D) dentro do DisplayFK, no widget CameraMipi.
 *
 * user_setup.h: USE_CAMERA_CSI + CAM_OV02C10
 * Serial 115200: detecção e PPA no setup.
 */
#include "Arduino.h"
#include <SPI.h>
#include <Arduino_GFX_Library.h>
#include <displayfk.h>
#include <ESP_Video.h>

#define ORIENTATION 1  // 0..3, igual Arduino_GFX; 1 = paisagem
#define CAMERA_MIRROR_X false
#define CAMERA_MIRROR_Y false

#if (ORIENTATION < 0) || (ORIENTATION > 3)
#error "ORIENTATION must be 0, 1, 2 or 3"
#endif

#define FORMAT_SPIFFS_IF_FAILED false
const int PANEL_NATIVE_W = 800;
const int PANEL_NATIVE_H = 1280;
#if (ORIENTATION % 2) == 0
const int DISPLAY_W = PANEL_NATIVE_W;
const int DISPLAY_H = PANEL_NATIVE_H;
#else
const int DISPLAY_W = PANEL_NATIVE_H;
const int DISPLAY_H = PANEL_NATIVE_W;
#endif
const int TOUCH_MAP_X0 = 0;
const int TOUCH_MAP_X1 = 800;
const int TOUCH_MAP_Y0 = 0;
const int TOUCH_MAP_Y1 = 1280;
const bool TOUCH_SWAP_XY = false;
const bool TOUCH_INVERT_X = false;
const bool TOUCH_INVERT_Y = false;
const int DISP_RST = 27;
const int TOUCH_SCL = 8;
const int TOUCH_SDA = 7;
const int TOUCH_INT = 21;
const int TOUCH_RST = 22;
const uint8_t rotationScreen = ORIENTATION;
const bool isIPS = true;

void screen0();
void loadWidgets();

#define DISP_LED 23
Arduino_ESP32DSIPanel *bus = nullptr;
Arduino_DSI_Display *tft = nullptr;
DisplayFK myDisplay;

CameraMipi cam01(50, 50, 0);
CameraMipi cam02(500, 50, 0);
const uint8_t qtdCameraMipi = 2;
CameraMipi *arrayCameraMipi[qtdCameraMipi] = {&cam01, &cam02};

void startDisplay(){
    Serial.begin(115200);
    bus = new Arduino_ESP32DSIPanel(
    20, 20, 40,
    4, 8, 20,
    60000000);
    tft = new Arduino_DSI_Display(
    800, 1280, bus, rotationScreen, true,
    DISP_RST, jd9365_init_operations, sizeof(jd9365_init_operations) / sizeof(lcd_init_cmd_t));
    tft->begin();
    myDisplay.getCamera().startPpa();
#if defined(DISP_LED)
    pinMode(DISP_LED, OUTPUT);
    digitalWrite(DISP_LED, HIGH);
#endif
    myDisplay.setDrawObject(tft);
    myDisplay.setTouchCorners(TOUCH_MAP_X0, TOUCH_MAP_X1, TOUCH_MAP_Y0, TOUCH_MAP_Y1);
    myDisplay.setInvertAxis(TOUCH_INVERT_X, TOUCH_INVERT_Y);
    myDisplay.setSwapAxis(TOUCH_SWAP_XY);
    myDisplay.startTouchGSL3680(DISPLAY_W, DISPLAY_H, rotationScreen, TOUCH_SDA, TOUCH_SCL, TOUCH_INT, TOUCH_RST);
    myDisplay.enableTouchLog();
    loadWidgets();
    myDisplay.loadScreen(screen0);
    myDisplay.createTask(false, 3);
    Serial.println("Display started");
}

void screen0(){
    tft->fillScreen(CFK_WHITE);
    WidgetBase::backgroundColor = CFK_WHITE;
    myDisplay.drawWidgetsOnScreen(0);
}

void loadWidgets(){
    CameraMipiConfig config = { .width = 200, .height = 100, .showZoomLevel = true };
    cam01.setup(config);
    cam01.setZoomLevel(1);

    CameraMipiConfig config2 = { .width = 300, .height = 300, .showZoomLevel = true };
    cam02.setup(config2);
    cam02.setZoomLevel(1);
    myDisplay.setCameraMipi(arrayCameraMipi, qtdCameraMipi);
}

#if !defined(USE_CAMERA_CSI) || !defined(CAM_OV02C10)
#error "This example requires USE_CAMERA_CSI and CAM_OV02C10 in user_setup.h"
#endif

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 4, 0)
#if CONFIG_IDF_TARGET_ESP32P4
#define EXAMPLE_MIPI_CSI_SCCB_I2C_PORT    0
#define EXAMPLE_MIPI_CSI_SCCB_I2C_SCL_PIN 8
#define EXAMPLE_MIPI_CSI_SCCB_I2C_SDA_PIN 7
#else
#error "The selected target SoC is not supported"
#endif

void setup() {
    Serial.begin(115200);

    myDisplay.setRotationCamera(ORIENTATION, CAMERA_MIRROR_X, CAMERA_MIRROR_Y);
    if (!myDisplay.startCamera(EXAMPLE_MIPI_CSI_SCCB_I2C_PORT, EXAMPLE_MIPI_CSI_SCCB_I2C_SCL_PIN, EXAMPLE_MIPI_CSI_SCCB_I2C_SDA_PIN)) {
        Serial.println("failed to start CSI camera");
        return;
    }

    startDisplay();
}

void loop() {
}

#else
void setup() {
    Serial.begin(115200);
    Serial.println("ESP_IDF_VERSION < 5.4.0, this example is not supported");
}

void loop() {
    delay(1000);
}
#endif
