#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// ==========================================================
// Estrutura DisplayConfig (baseada no seu exemplo anterior)
// ==========================================================
typedef struct
{
  const char *name;

  uint32_t hsync_pulse_width;
  uint32_t hsync_back_porch;
  uint32_t hsync_front_porch;
  uint32_t vsync_pulse_width;
  uint32_t vsync_back_porch;
  uint32_t vsync_front_porch;
  uint32_t prefer_speed;
  uint32_t lane_bit_rate;

  uint16_t width;
  uint16_t height;
  int8_t rotation;
  bool auto_flush;
  int8_t rst_pin;

  const lcd_init_cmd_t *init_cmds;
  size_t init_cmds_size;

  uint8_t i2c_address;
  int8_t i2c_sda_pin;
  int8_t i2c_scl_pin;
  uint32_t i2c_clock_speed;
  const uint8_t (*i2c_init_seq)[2];
  size_t i2c_init_seq_size;
} DisplayConfig;

// ==========================================================
// Inicialização específica do JD9365 (extraída do driver original)
// ==========================================================

// Sequência de inicialização DCS do painel
static const lcd_init_cmd_t vendor_specific_init_default[] = {
  // {cmd, { data }, data_size, delay_ms}
  //-------------------- Page 0 --------------------//
    {0xE0, (uint8_t[]){0x00}, 1, 0},
    {0xE1, (uint8_t[]){0x93}, 1, 0},
    {0xE2, (uint8_t[]){0x65}, 1, 0},
    {0xE3, (uint8_t[]){0xF8}, 1, 0},
    {0x80, (uint8_t[]){0x01}, 1, 0},

    //-------------------- Page 1 --------------------//
    {0xE0, (uint8_t[]){0x01}, 1, 0},
    {0x00, (uint8_t[]){0x00}, 1, 0},
    {0x01, (uint8_t[]){0x38}, 1, 0},
    {0x03, (uint8_t[]){0x10}, 1, 0},
    {0x04, (uint8_t[]){0x38}, 1, 0},
    {0x0C, (uint8_t[]){0x74}, 1, 0},
    {0x17, (uint8_t[]){0x00}, 1, 0},
    {0x18, (uint8_t[]){0xAF}, 1, 0},
    {0x19, (uint8_t[]){0x00}, 1, 0},
    {0x1A, (uint8_t[]){0x00}, 1, 0},
    {0x1B, (uint8_t[]){0xAF}, 1, 0},
    {0x1C, (uint8_t[]){0x00}, 1, 0},

    {0x35, (uint8_t[]){0x26}, 1, 0}, // TE
    {0x37, (uint8_t[]){0x09}, 1, 0},
    {0x38, (uint8_t[]){0x04}, 1, 0},
    {0x39, (uint8_t[]){0x00}, 1, 0},
    {0x3A, (uint8_t[]){0x01}, 1, 0},
    {0x3C, (uint8_t[]){0x78}, 1, 0},
    {0x3D, (uint8_t[]){0xFF}, 1, 0},
    {0x3E, (uint8_t[]){0xFF}, 1, 0},
    {0x3F, (uint8_t[]){0x7F}, 1, 0},

    {0x40, (uint8_t[]){0x06}, 1, 0},
    {0x41, (uint8_t[]){0xA0}, 1, 0},
    {0x42, (uint8_t[]){0x81}, 1, 0},
    {0x43, (uint8_t[]){0x1E}, 1, 0},
    {0x44, (uint8_t[]){0x0D}, 1, 0},
    {0x45, (uint8_t[]){0x28}, 1, 0},

    {0x55, (uint8_t[]){0x02}, 1, 0},
    {0x57, (uint8_t[]){0x69}, 1, 0},
    {0x59, (uint8_t[]){0x0A}, 1, 0},
    {0x5A, (uint8_t[]){0x2A}, 1, 0},
    {0x5B, (uint8_t[]){0x17}, 1, 0},

    // --- gamma settings mantidos ---
    {0x5D, (uint8_t[]){0x7F}, 1, 0},
    {0x5E, (uint8_t[]){0x6A}, 1, 0},
    {0x5F, (uint8_t[]){0x5B}, 1, 0},
    {0x60, (uint8_t[]){0x4F}, 1, 0},
    {0x61, (uint8_t[]){0x4A}, 1, 0},
    {0x62, (uint8_t[]){0x3D}, 1, 0},
    {0x63, (uint8_t[]){0x41}, 1, 0},
    {0x64, (uint8_t[]){0x2A}, 1, 0},
    {0x65, (uint8_t[]){0x44}, 1, 0},
    {0x66, (uint8_t[]){0x43}, 1, 0},
    {0x67, (uint8_t[]){0x44}, 1, 0},
    {0x68, (uint8_t[]){0x62}, 1, 0},
    {0x69, (uint8_t[]){0x52}, 1, 0},
    {0x6A, (uint8_t[]){0x59}, 1, 0},
    {0x6B, (uint8_t[]){0x4C}, 1, 0},
    {0x6C, (uint8_t[]){0x48}, 1, 0},
    {0x6D, (uint8_t[]){0x3A}, 1, 0},
    {0x6E, (uint8_t[]){0x26}, 1, 0},
    {0x6F, (uint8_t[]){0x00}, 1, 0},

    //-------------------- Page 2 (gamma mirror) --------------------//
    {0xE0, (uint8_t[]){0x02}, 1, 0},
    // valores mantidos, enxugados
    {0x00, (uint8_t[]){0x42}, 1, 0},
    {0x01, (uint8_t[]){0x42}, 1, 0},
    {0x02, (uint8_t[]){0x40}, 1, 0},
    {0x03, (uint8_t[]){0x40}, 1, 0},
    {0x04, (uint8_t[]){0x5E}, 1, 0},
    {0x05, (uint8_t[]){0x5E}, 1, 0},
    {0x06, (uint8_t[]){0x5F}, 1, 0},
    {0x07, (uint8_t[]){0x5F}, 1, 0},
    {0x08, (uint8_t[]){0x5F}, 1, 0},

    //-------------------- Page 4 (interface) --------------------//
    {0xE0, (uint8_t[]){0x04}, 1, 0},
    {0x00, (uint8_t[]){0x0E}, 1, 0},
    {0x02, (uint8_t[]){0xB3}, 1, 0},
    {0x09, (uint8_t[]){0x61}, 1, 0},
    {0x0E, (uint8_t[]){0x48}, 1, 0},
    {0x37, (uint8_t[]){0x58}, 1, 0},
    {0x2B, (uint8_t[]){0x0F}, 1, 0},

    //-------------------- Page 0 (back to default) --------------------//
    {0xE0, (uint8_t[]){0x00}, 1, 0},

    //-------------------- Interface Pixel Format --------------------//
    {0x3A, (uint8_t[]){0x55}, 1, 0},   // 16-bit RGB565

    //-------------------- Exit Sleep --------------------//
    {0x11, (uint8_t[]){0x00}, 0, 250}, // Sleep Out com delay suficiente

    //-------------------- Display ON --------------------//
    {0x29, (uint8_t[]){0x00}, 0, 20},  // Display ON
};

// Sequência de inicialização I2C do touch GSL3680
static const uint8_t i2c_init_seq_default[][2] = {
    {0x95, 0x11},
    {0x95, 0x17},
    {0x96, 0x00},
    {0x96, 0xFF}
};

// ==========================================================
// Configuração do display (MIPI DSI)
// ==========================================================
static const DisplayConfig SCREEN_JD9365_DSI = {
    .name = "10.1-DSI-TOUCH-A",
    .hsync_pulse_width = 4,
    .hsync_back_porch = 8,
    .hsync_front_porch = 8,
    .vsync_pulse_width = 2,
    .vsync_back_porch = 4,
    .vsync_front_porch = 4,
    .prefer_speed = 60000000,
    .lane_bit_rate = 800,
    .width = 800,
    .height = 1280,
    .rotation = 0,
    .auto_flush = true,
    .rst_pin = -1,
    .init_cmds = vendor_specific_init_default,
    .init_cmds_size = sizeof(vendor_specific_init_default) / sizeof(lcd_init_cmd_t),
    .i2c_address = 0x45,
    .i2c_sda_pin = 7,
    .i2c_scl_pin = 8,
    .i2c_clock_speed = 100000,
    .i2c_init_seq = i2c_init_seq_default,
    .i2c_init_seq_size = sizeof(i2c_init_seq_default) / (2 * sizeof(uint8_t))
};

// ==========================================================
// Instância do painel DSI
// ==========================================================
Arduino_ESP32DSIPanel *dsipanel = new Arduino_ESP32DSIPanel(
    SCREEN_JD9365_DSI.hsync_pulse_width,
    SCREEN_JD9365_DSI.hsync_back_porch,
    SCREEN_JD9365_DSI.hsync_front_porch,
    SCREEN_JD9365_DSI.vsync_pulse_width,
    SCREEN_JD9365_DSI.vsync_back_porch,
    SCREEN_JD9365_DSI.vsync_front_porch,
    SCREEN_JD9365_DSI.prefer_speed,
    SCREEN_JD9365_DSI.lane_bit_rate
);

Arduino_DSI_Display *gfx = new Arduino_DSI_Display(
    SCREEN_JD9365_DSI.width,
    SCREEN_JD9365_DSI.height,
    dsipanel,
    0, // rotation offset
    SCREEN_JD9365_DSI.auto_flush,
    SCREEN_JD9365_DSI.rst_pin,
    SCREEN_JD9365_DSI.init_cmds,
    SCREEN_JD9365_DSI.init_cmds_size,
    false // false = framebuffer em RAM interna
);

const int PIN_BL = 23;

// ==========================================================
// Setup
// ==========================================================
void setup()
{
  Serial.begin(115200);
  Serial.println("Inicializando JD9365 via MIPI DSI...");
  pinMode(PIN_BL, OUTPUT);
  digitalWrite(PIN_BL, HIGH);
  // Inicializa display
  if (!gfx->begin())
  {
    Serial.println("Falha ao inicializar o display JD9365!");
    while (true)
      delay(1000);
  }

  gfx->fillScreen(RED);
  delay(1000);
  gfx->fillScreen(GREEN);
  delay(1000);
  gfx->fillScreen(BLUE);
  delay(1000);
  gfx->fillScreen(WHITE);

  Serial.println("Display inicializado com sucesso!");
}

// ==========================================================
// Loop
// ==========================================================
void loop()
{
  // Exemplo simples: desenhar um retângulo que muda de cor
  static uint8_t colorIndex = 0;
  uint16_t colors[] = {RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, WHITE};

  gfx->fillRect(100, 100, 300, 300, colors[colorIndex]);
  colorIndex = (colorIndex + 1) % 7;
  delay(500);
}
