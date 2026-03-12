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
 * @param out Ponteiro para o array de saída.
 * @param outSize Tamanho do array de saída.
 * @return true se a geração foi bem-sucedida, false caso contrário.
 * @note O chamador deve garantir que `out` tenha pelo menos `numTons` espaço e não retornar nullptr.
 */
 inline bool gerarDegradeHSV(float h, int numTons, uint16_t* out, int outSize) {
  if (numTons <= 0 || out == nullptr || outSize < numTons) return false;

  float s = 1.0f;
  for (int i = 0; i < numTons; i++) {
    float v = (numTons > 1) ? (0.3f + (0.7f / (numTons - 1)) * i) : 1.0f;
    out[i] = hsvToRgb565Fast(h, s, v);
  }

  return true;
}

/**
 * @brief Gera um blend linear em HSV entre duas cores RGB565.
 * @param startColor Cor inicial em RGB565.
 * @param endColor Cor final em RGB565.
 * @param numTons Número de tons a gerar (>=2).
 * @param out Ponteiro para o array de saída.
 * @param outSize Tamanho do array de saída.
 * @return true se a geração foi bem-sucedida, false caso contrário.
 * @note O chamador deve garantir que `out` tenha pelo menos `numTons` espaço e não retornar nullptr.
 */
 inline bool blendColors(uint16_t startColor, uint16_t endColor, int numTons, uint16_t* out, int outSize) {
  if (numTons < 2 || out == nullptr || outSize < numTons) return false;

  HSV_t a = rgb565ToHsv(startColor);
  HSV_t b = rgb565ToHsv(endColor);

  if (a.s < 1e-6f) a.h = b.h;
  if (b.s < 1e-6f) b.h = a.h;

  float dh = b.h - a.h;
  if (dh >  0.5f) dh -= 1.0f;
  if (dh < -0.5f) dh += 1.0f;

  for (int i = 0; i < numTons; i++) {
    float t = (float)i / (numTons - 1);

    float h = a.h + dh * t;
    if (h < 0.0f) h += 1.0f;
    if (h >= 1.0f) h -= 1.0f;

    float s = a.s + (b.s - a.s) * t;
    float v = a.v + (b.v - a.v) * t;

    out[i] = hsvToRgb565Fast(h, s, v);
  }
  return true;
}

/**
 * @brief Clareia uma cor RGB565 em direção ao branco.
 * 
 * @param color Cor original no formato RGB565.
 * @param value Intensidade do clareamento [0.0 .. 1.0].
 *        - 0.0 → retorna a cor original.
 *        - 1.0 → retorna branco puro (0xFFFF).
 * @return Cor clareada no formato RGB565.
 */
uint16_t lighten565(uint16_t color, float value) {
  if (value <= 0.0f) return color;   // sem alteração
  if (value >= 1.0f) return 0xFFFF;  // branco puro

  // Converte para HSV
  HSV_t hsv = rgb565ToHsv(color);

  // Interpola o brilho (v) e reduz levemente a saturação
  hsv.v = hsv.v + (1.0f - hsv.v) * value;  // aproxima do branco
  hsv.s = hsv.s * (1.0f - 0.5f * value);   // desatura um pouco ao clarear

  // Converte de volta para RGB565
  return hsvToRgb565Fast(hsv.h, hsv.s, hsv.v);
}

/**
 * @brief Escurece uma cor RGB565 em direção ao preto.
 * 
 * @param color Cor original no formato RGB565.
 * @param value Intensidade do escurecimento [0.0 .. 1.0].
 *        - 0.0 → retorna a cor original.
 *        - 1.0 → retorna preto (0x0000).
 * @return Cor escurecida no formato RGB565.
 */
uint16_t darken565(uint16_t color, float value) {
  if (value <= 0.0f) return color;   // sem alteração
  if (value >= 1.0f) return 0x0000;  // preto puro

  // Converte para HSV
  HSV_t hsv = rgb565ToHsv(color);

  // Reduz o brilho (v) proporcionalmente
  hsv.v = hsv.v * (1.0f - value);

  // Mantém a saturação e matiz originais
  return hsvToRgb565Fast(hsv.h, hsv.s, hsv.v);
}

#endif