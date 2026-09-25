/**
 * Captura MIPI-CSI na ESP32-P4-Function-EV-Board (SC2336 + tela EK79007 1024x600).
 *
 * A cópia câmera → tela usa o PPA (DMA 2D) dentro do DisplayFK, no widget CameraMipi.
 *
 * user_setup.h: USE_CAMERA_CSI + CAM_SC2336
 * Serial 115200: detecção e PPA no setup.
 * Placa: https://github.com/espressif/esp-dev-kits/blob/master/examples/esp32-p4-function-ev-board/examples/esp_brookesia_phone/README.md
 */
#include "Arduino.h"
#include <SPI.h>
#include <Arduino_GFX_Library.h>
#include <displayfk.h>
#include <ESP_Video.h>

#define ORIENTATION 0  // 0..3, igual Arduino_GFX; 0 = nativo 1024x600
#define CAMERA_MIRROR_X true
#define CAMERA_MIRROR_Y true

#if (ORIENTATION < 0) || (ORIENTATION > 3)
#error "ORIENTATION must be 0, 1, 2 or 3"
#endif

#define FORMAT_SPIFFS_IF_FAILED false
const int PANEL_NATIVE_W = 1024;
const int PANEL_NATIVE_H = 600;
#if (ORIENTATION % 2) == 0
const int DISPLAY_W = PANEL_NATIVE_W;
const int DISPLAY_H = PANEL_NATIVE_H;
#else
const int DISPLAY_W = PANEL_NATIVE_H;
const int DISPLAY_H = PANEL_NATIVE_W;
#endif
const int TOUCH_MAP_X0 = 0;
const int TOUCH_MAP_X1 = 1024;
const int TOUCH_MAP_Y0 = 0;
const int TOUCH_MAP_Y1 = 600;
const bool TOUCH_SWAP_XY = false;
const bool TOUCH_INVERT_X = true;
const bool TOUCH_INVERT_Y = true;
const int DISP_RST = 27;
const int TOUCH_SCL = 8;
const int TOUCH_SDA = 7;
const int TOUCH_INT = -1;
const int TOUCH_RST = -1;
const uint8_t rotationScreen = ORIENTATION;
const bool isIPS = true;

void screen0();
void loadWidgets();

#define DISP_LED 26
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
    10, 160, 160,
    1, 23, 12,
    52000000);
    tft = new Arduino_DSI_Display(
    1024, 600, bus, rotationScreen, isIPS,
    DISP_RST, ek79007_init_operations, sizeof(ek79007_init_operations) / sizeof(lcd_init_cmd_t));
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
    // GT911 no mesmo I2C do SCCB (GPIO 7/8). Comentar evita NACK na detecção da câmera.
    myDisplay.startTouchGT911(DISPLAY_W, DISPLAY_H, rotationScreen, TOUCH_SDA, TOUCH_SCL, TOUCH_INT, TOUCH_RST);
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
    CameraMipiConfig config = { .width = 200, .height = 100 };
    cam01.setup(config);

    CameraMipiConfig config2 = { .width = 300, .height = 300 };
    cam02.setup(config2);
    myDisplay.setCameraMipi(arrayCameraMipi, qtdCameraMipi);
}

#if !defined(USE_CAMERA_CSI) || !defined(CAM_SC2336)
#error "This example requires USE_CAMERA_CSI and CAM_SC2336 in user_setup.h"
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
