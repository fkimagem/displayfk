/**
 * Captura MIPI-CSI na ESP32-P4-EYE (OV2710 + tela ST7789 240x240 SPI).
 *
 * A cópia câmera → tela usa o PPA (DMA 2D) dentro do DisplayFK, no widget CameraMipi.
 * O ST7789 não tem framebuffer DSI: o PPA escreve num preview 240x240 e o SPI envia o bitmap.
 *
 * Esta placa exige dois passos de hardware antes de startCamera():
 * - enableBoardPower(GPIO 12): o enable da câmera e do LCD é o mesmo pino RTC.
 *   Sem ligá-lo (e sem hold), o sensor e a tela ficam sem alimentação.
 * - startXclk(GPIO 11, 24 MHz): a OV2710 não tem cristal no módulo. O clock mestre
 *   (XCLK/MCLK) tem que sair do ESP. Sem esse clock o núcleo do sensor não roda,
 *   o SCCB não responde na detecção e o MIPI não transmite. O driver e o
 *   startCamera() não geram esse sinal; só abrem I2C/CSI.
 *
 * user_setup.h: USE_CAMERA_CSI + CAM_OV2710
 * Serial 115200: detecção e PPA no setup.
 * Placa: https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32p4/esp32-p4-eye/user_guide.html
 * Datasheet: https://dl.espressif.com/AE/esp-dev-kits/SCH_ESP32-P4-EYE-MB_V2.3_20250416.pdf
 */
#include "Arduino.h"
#include <ESP_Video.h>
#include <SPI.h>
#ifndef ESP32SPIDMA_MAX_PIXELS_AT_ONCE
#define ESP32SPIDMA_MAX_PIXELS_AT_ONCE 4096
#endif
#include <Arduino_GFX_Library.h>
#include <displayfk.h>

#define ORIENTATION 0
#define CAMERA_MIRROR_X false
#define CAMERA_MIRROR_Y false

#if (ORIENTATION < 0) || (ORIENTATION > 3)
#error "ORIENTATION must be 0, 1, 2 or 3"
#endif

#define FORMAT_SPIFFS_IF_FAILED false
const int PANEL_NATIVE_W = 240;
const int PANEL_NATIVE_H = 240;
const int DISPLAY_W = 240;
const int DISPLAY_H = 240;
const int DISP_RST = 15;
const int DISP_DC = 19;
const int DISP_CS = 18;
const int DISP_SCK = 17;
const int DISP_MOSI = 16;
const int DISP_MISO = -1;
const uint8_t rotationScreen = ORIENTATION;
const bool isIPS = true;

void screen0();
void loadWidgets();

#define DISP_LED 20
#define DISP_LED_ON LOW
#define P4_EYE_PWR_EN 12
#define P4_EYE_CAM_XCLK 11
#define P4_EYE_CAM_RST 26
#define P4_EYE_XCLK_HZ 24000000
#define ST7789_SPI_HZ 40000000

const int ZOOM_PIN_1X = 3;
const int ZOOM_PIN_2X = 4;
const int ZOOM_PIN_3X = 5;

Arduino_DataBus *bus = nullptr;
Arduino_ST7789 *tft = nullptr;
DisplayFK myDisplay;

CameraMipi cam01(0, 0, 0);
const uint8_t qtdCameraMipi = 1;
CameraMipi *arrayCameraMipi[qtdCameraMipi] = {&cam01};

static void beginZoomButtons()
{
    pinMode(ZOOM_PIN_1X, INPUT_PULLUP);
    pinMode(ZOOM_PIN_2X, INPUT_PULLUP);
    pinMode(ZOOM_PIN_3X, INPUT_PULLUP);
}

static void pollZoomButtons()
{
    if (digitalRead(ZOOM_PIN_1X) == LOW) {
        cam01.setZoomLevel(1);
        return;
    }
    if (digitalRead(ZOOM_PIN_2X) == LOW) {
        cam01.setZoomLevel(2);
        return;
    }
    if (digitalRead(ZOOM_PIN_3X) == LOW) {
        cam01.setZoomLevel(4);
    }
}

void startDisplay(){
    Serial.begin(115200);
    bus = new Arduino_ESP32SPIDMA(DISP_DC, DISP_CS, DISP_SCK, DISP_MOSI, DISP_MISO, SPI2_HOST + 1, false);
    tft = new Arduino_ST7789(bus, DISP_RST, rotationScreen, isIPS, DISPLAY_W, DISPLAY_H, 0, 0, 0, 0);
    tft->begin(ST7789_SPI_HZ);
    myDisplay.getCamera().startPpa();
#if defined(DISP_LED)
    pinMode(DISP_LED, OUTPUT);
    digitalWrite(DISP_LED, DISP_LED_ON);
#endif
    myDisplay.setDrawObject(tft);
    loadWidgets();
    myDisplay.loadScreen(screen0);
    beginZoomButtons();
    myDisplay.createTask(false, 3);
    Serial.println("Display started");
}

void screen0(){
    tft->fillScreen(CFK_WHITE);
    WidgetBase::backgroundColor = CFK_WHITE;
    myDisplay.drawWidgetsOnScreen(0);
}

void loadWidgets(){
    CameraMipiConfig config = { .width = 240, .height = 240, .showZoomLevel = true };
    cam01.setup(config);
    cam01.setZoomLevel(1);
    myDisplay.setCameraMipi(arrayCameraMipi, qtdCameraMipi);
}

#if !defined(USE_CAMERA_CSI) || !defined(CAM_OV2710)
#error "This example requires USE_CAMERA_CSI and CAM_OV2710 in user_setup.h"
#endif

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 4, 0)
#if CONFIG_IDF_TARGET_ESP32P4
#define EXAMPLE_MIPI_CSI_SCCB_I2C_PORT    0
#define EXAMPLE_MIPI_CSI_SCCB_I2C_SCL_PIN 13
#define EXAMPLE_MIPI_CSI_SCCB_I2C_SDA_PIN 14
#else
#error "The selected target SoC is not supported"
#endif

void setup() {
    Serial.begin(115200);

    if (!myDisplay.getCamera().enableBoardPower(P4_EYE_PWR_EN)) {
        Serial.println("failed to enable P4-EYE camera/LCD power (GPIO 12)");
        return;
    }
    if (!myDisplay.getCamera().startXclk(P4_EYE_CAM_XCLK, P4_EYE_XCLK_HZ)) {
        return;
    }

    myDisplay.setRotationCamera(ORIENTATION, CAMERA_MIRROR_X, CAMERA_MIRROR_Y);
    if (!myDisplay.startCamera(EXAMPLE_MIPI_CSI_SCCB_I2C_PORT, EXAMPLE_MIPI_CSI_SCCB_I2C_SCL_PIN, EXAMPLE_MIPI_CSI_SCCB_I2C_SDA_PIN, 400000, P4_EYE_CAM_RST, -1)) {
        Serial.println("failed to start CSI camera");
        return;
    }

    startDisplay();
}

void loop() {
    pollZoomButtons();
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
