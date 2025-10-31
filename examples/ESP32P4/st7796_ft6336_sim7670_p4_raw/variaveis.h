#ifndef PINOUT_H
#define PINOUT_H
#include <Arduino.h>

const char* ssid     = "Labmaker"; ///< SSID do WiFi.
const char* password = "Anglo2020pp"; ///< Senha do WiFi.

const char* serverIP = "192.168.0.128";  ///< Endereço IP do servidor alvo para conexão TCP.
const uint16_t serverPort = 12345;        ///< Porta TCP do servidor.

const uint8_t rotationScreen = 1; ///< Rotação atual do display (valores conforme driver/ArduinoGFX).


const int DISPLAY_W = 480; ///< Largura do display em pixels.
const int DISPLAY_H = 320; ///< Altura do display em pixels.
const int DISP_FREQUENCY = 27000000; ///< Frequência do barramento do display (Hz).
const int TOUCH_MAP_X0 = 0; ///< Mapeamento do touch: origem X mínima.
const int TOUCH_MAP_Y0 = 0; ///< Mapeamento do touch: origem Y mínima.
const int TOUCH_MAP_X1 = 320; ///< Mapeamento do touch: limite X máxima (raw -> coordenada).
const int TOUCH_MAP_Y1 = 480; ///< Mapeamento do touch: limite Y máxima (raw -> coordenada).
const bool TOUCH_SWAP_XY = true; ///< Se verdadeiro, troca eixos X/Y do touch.
const bool TOUCH_INVERT_X = true; ///< Se verdadeiro, inverte o eixo X do touch.
const bool TOUCH_INVERT_Y = false; ///< Se verdadeiro, inverte o eixo Y do touch.
const int DISP_MOSI = 23; ///< Pino MOSI do display.
const int DISP_MISO = -1; ///< Pino MISO do display (-1 se não utilizado).
const int DISP_SCLK = 26; ///< Pino SCLK do display.
const int DISP_CS = 20; ///< Pino CS (chip select) do display.
const int DISP_DC = 22; ///< Pino DC (data/command) do display.
const int DISP_RST = 21; ///< Pino RST (reset) do display.
const int TOUCH_SCL = 31; ///< Pino SCL (I2C) do controlador de touch FT6336U.
const int TOUCH_SDA = 30; ///< Pino SDA (I2C) do controlador de touch FT6336U.
const int TOUCH_INT = 47; ///< Pino INT (interrupção) do controlador de touch FT6336U.
const int TOUCH_RST = 33; ///< Pino RST do controlador de touch FT6336U.

const int RX1_PIN = 49; ///< Pino RX1 (UART) utilizado pelo projeto.
const int TX1_PIN = 50; ///< Pino TX1 (UART) utilizado pelo projeto.

const int POTENCIOMETRO = 51; ///< Pino analógico conectado ao potenciômetro.


#endif
