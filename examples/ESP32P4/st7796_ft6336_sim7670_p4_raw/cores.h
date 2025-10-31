#ifndef COR_H
#define COR_H
#include <Arduino.h>


/** @brief Representa uma cor no espaço HSV normalizado. */
struct HSV_t {
  float h;  // Hue (0.0 – 1.0)
  float s;  // Saturation (0.0 – 1.0)
  float v;  // Value (0.0 – 1.0)
};

/**
 * @brief Converte RGB (8 bits por canal) para o formato RGB565 (16 bits).
 * @param r Valor do canal vermelho [0..255].
 * @param g Valor do canal verde [0..255].
 * @param b Valor do canal azul [0..255].
 * @return Cor combinada no formato RGB565.
 */
constexpr uint16_t rgbTo565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

// Cores da tabela
constexpr uint16_t COLOR_WHITE   = rgbTo565(255, 255, 255); ///< Branco em RGB565
constexpr uint16_t COLOR_SILVER  = rgbTo565(192, 192, 192); ///< Prata em RGB565
constexpr uint16_t COLOR_GRAY    = rgbTo565(128, 128, 128); ///< Cinza em RGB565
constexpr uint16_t COLOR_BLACK   = rgbTo565(0, 0, 0); ///< Preto em RGB565
constexpr uint16_t COLOR_MAROON  = rgbTo565(128, 0, 0); ///< Bordô em RGB565
constexpr uint16_t COLOR_RED     = rgbTo565(255, 0, 0); ///< Vermelho em RGB565
constexpr uint16_t COLOR_ORANGE  = rgbTo565(255, 165, 0); ///< Laranja em RGB565
constexpr uint16_t COLOR_YELLOW  = rgbTo565(255, 255, 0); ///< Amarelo em RGB565
constexpr uint16_t COLOR_OLIVE   = rgbTo565(128, 128, 0); ///< Oliva em RGB565
constexpr uint16_t COLOR_LIME    = rgbTo565(0, 255, 0); ///< Lima (verde vivo) em RGB565
constexpr uint16_t COLOR_GREEN   = rgbTo565(0, 128, 0); ///< Verde em RGB565
constexpr uint16_t COLOR_AQUA    = rgbTo565(0, 255, 255); ///< Ciano (Aqua) em RGB565
constexpr uint16_t COLOR_BLUE    = rgbTo565(0, 0, 255); ///< Azul em RGB565
constexpr uint16_t COLOR_NAVY    = rgbTo565(0, 0, 128); ///< Azul marinho em RGB565
constexpr uint16_t COLOR_TEAL    = rgbTo565(0, 128, 128); ///< Azul petróleo em RGB565
constexpr uint16_t COLOR_FUCHSIA = rgbTo565(255, 0, 255); ///< Fúcsia (magenta) em RGB565
constexpr uint16_t COLOR_PURPLE  = rgbTo565(128, 0, 128); ///< Roxo em RGB565

const int amountColors = 17; ///< Quantidade de cores definidas na tabela `colors`.
const uint16_t colors[amountColors] = { ///< Tabela de cores RGB565 padronizadas.
    COLOR_WHITE,
    COLOR_SILVER,
    COLOR_GRAY,
    COLOR_BLACK,
    COLOR_MAROON,
    COLOR_RED,
    COLOR_ORANGE,
    COLOR_YELLOW,
    COLOR_OLIVE,
    COLOR_LIME,
    COLOR_GREEN,
    COLOR_AQUA,
    COLOR_BLUE,
    COLOR_NAVY,
    COLOR_TEAL,
    COLOR_FUCHSIA,
    COLOR_PURPLE
};



/**
 * @brief Converte uma cor HSV normalizada para RGB565 de forma rápida.
 * @param h Matiz [0.0..1.0].
 * @param s Saturação [0.0..1.0].
 * @param v Valor (brilho) [0.0..1.0].
 * @return Cor no formato RGB565.
 */
uint16_t hsvToRgb565Fast(float h, float s, float v) {
  float r, g, b;
  int i = int(h * 6.0f);
  float f = h * 6.0f - i;
  float p = v * (1.0f - s);
  float q = v * (1.0f - f * s);
  float t = v * (1.0f - (1.0f - f) * s);

  switch (i % 6) {
    case 0: r = v; g = t; b = p; break;
    case 1: r = q; g = v; b = p; break;
    case 2: r = p; g = v; b = t; break;
    case 3: r = p; g = q; b = v; break;
    case 4: r = t; g = p; b = v; break;
    default: r = v; g = p; b = q; break;
  }

  uint16_t R = (uint16_t)(r * 31.0f);
  uint16_t G = (uint16_t)(g * 63.0f);
  uint16_t B = (uint16_t)(b * 31.0f);
  return (R << 11) | (G << 5) | B;
}

/**
 * @brief Converte uma cor RGB565 para HSV normalizado.
 * @param color565 Cor no formato RGB565.
 * @return Estrutura HSV com h, s, v em [0.0..1.0].
 */
HSV_t rgb565ToHsv(uint16_t color565) {
  // Extrai os canais RGB
  uint8_t r = (color565 >> 11) & 0x1F; // 5 bits
  uint8_t g = (color565 >> 5)  & 0x3F; // 6 bits
  uint8_t b =  color565        & 0x1F; // 5 bits

  // Converte para 0–1 float
  float rf = r / 31.0f;
  float gf = g / 63.0f;
  float bf = b / 31.0f;

  // Encontra maior e menor
  float maxVal = max(rf, max(gf, bf));
  float minVal = min(rf, min(gf, bf));
  float delta = maxVal - minVal;

  HSV_t hsv;
  hsv.v = maxVal; // valor = brilho

  // Saturação
  hsv.s = (maxVal == 0) ? 0 : (delta / maxVal);

  // Matiz
  if (delta == 0) {
    hsv.h = 0; // neutro (sem cor)
  } else if (maxVal == rf) {
    hsv.h = fmodf(((gf - bf) / delta), 6.0f);
  } else if (maxVal == gf) {
    hsv.h = ((bf - rf) / delta) + 2.0f;
  } else {
    hsv.h = ((rf - gf) / delta) + 4.0f;
  }

  hsv.h /= 6.0f;
  if (hsv.h < 0) hsv.h += 1.0f; // normaliza para [0,1]

  return hsv;
}

/**
 * @brief Gera um degradê (array) em RGB565 variando o brilho para um hue fixo.
 * @param h Matiz fixo [0.0..1.0].
 * @param numTons Quantidade de tons a gerar (>0).
 * @return Ponteiro para array dinâmico de `numTons` elementos RGB565, ou nullptr.
 * @note O chamador deve liberar com `delete[]`.
 */
uint16_t* gerarDegradeHSV(float h, int numTons) {
  if (numTons <= 0) return nullptr;

  uint16_t* tons = new uint16_t[numTons];

  float s = 1.0f; // saturação total
  for (int i = 0; i < numTons; i++) {
    // brilho de 0.3 até 1.0
    float v = 0.3f + (0.7f / (numTons - 1)) * i;
    tons[i] = hsvToRgb565Fast(h, s, v);
  }

  return tons; // lembre de liberar depois com delete[]
}

/**
 * @brief Gera um blend linear em HSV entre duas cores RGB565.
 * @param startColor Cor inicial em RGB565.
 * @param endColor Cor final em RGB565.
 * @param numTons Número de tons a gerar (>=2).
 * @return Ponteiro para array dinâmico de `numTons` elementos RGB565, ou nullptr.
 * @note O chamador deve liberar com `delete[]`.
 */
uint16_t* blendColors(uint16_t startColor, uint16_t endColor, int numTons) {
  if (numTons < 2) return nullptr;

  uint16_t* out = new uint16_t[numTons];
  HSV_t a = rgb565ToHsv(startColor);
  HSV_t b = rgb565ToHsv(endColor);

  // Tratar casos acinzentados (s ~ 0): “herdar” h para evitar saltos
  if (a.s < 1e-6f) a.h = b.h;
  if (b.s < 1e-6f) b.h = a.h;

  // Δh normalizado para o menor arco no círculo: [-0.5, +0.5]
  float dh = b.h - a.h;
  if (dh >  0.5f) dh -= 1.0f;
  if (dh < -0.5f) dh += 1.0f;

  for (int i = 0; i < numTons; i++) {
    float t = (float)i / (numTons - 1);

    float h = a.h + dh * t;
    // wrap de volta para [0,1]
    if (h < 0.0f) h += 1.0f;
    if (h >= 1.0f) h -= 1.0f;

    float s = a.s + (b.s - a.s) * t;
    float v = a.v + (b.v - a.v) * t;

    out[i] = hsvToRgb565Fast(h, s, v);
  }
  return out;
}

#endif