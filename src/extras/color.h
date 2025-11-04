#ifndef COLOR_HELPER
#define COLOR_HELPER

#include<Arduino.h>

#define COLOR_RED             0xF800  // vermelho puro
#define COLOR_ORANGE          0xFD20  // laranja intenso
#define COLOR_AMBER           0xFEC0  // âmbar
#define COLOR_YELLOW          0xFFE0  // amarelo puro
#define COLOR_LIME            0x87E0  // verde-limão
#define COLOR_GREEN           0x07E0  // verde puro
#define COLOR_SEA_GREEN       0x07F6  // verde-água
#define COLOR_CYAN            0x07FF  // ciano puro
#define COLOR_SKY_BLUE        0x5D9F  // azul-celeste
#define COLOR_BLUE            0x001F  // azul puro
#define COLOR_VIOLET          0x801F  // violeta
#define COLOR_MAGENTA         0xF81F  // magenta puro
#define COLOR_PINK            0xF8BE  // rosa forte
#define COLOR_ROSE            0xF9DF  // rosa quente
#define COLOR_SCARLET         0xFCA0  // vermelho alaranjado
#define COLOR_CHARTREUSE      0xAFE5  // verde-amarelado
#define COLOR_TURQUOISE       0x07FB  // turquesa viva
#define COLOR_INDIGO          0x481F  // azul arroxeado
#define COLOR_PURPLE          0xA01F  // roxo
#define COLOR_FUCHSIA         0xF81F  // fúcsia
#define COLOR_CORAL           0xFB88  // coral intenso

#define COLOR_PASTEL_RED      0xFBAE
#define COLOR_PASTEL_ORANGE   0xFEC9
#define COLOR_PASTEL_YELLOW   0xFFF3
#define COLOR_PASTEL_LIME     0xCFF3
#define COLOR_PASTEL_GREEN    0x9FF3
#define COLOR_PASTEL_CYAN     0xAFFF
#define COLOR_PASTEL_SKY      0xB5BF
#define COLOR_PASTEL_BLUE     0xAD5F
#define COLOR_PASTEL_INDIGO   0xA49F
#define COLOR_PASTEL_VIOLET   0xD49F
#define COLOR_PASTEL_PINK     0xFDBD
#define COLOR_PASTEL_MAGENTA  0xFBBF
#define COLOR_PASTEL_ROSE     0xFDDD
#define COLOR_PASTEL_CORAL    0xFD9A
#define COLOR_PASTEL_TURQUOISE 0x8FFD
#define COLOR_PASTEL_MINT     0xA7FB
#define COLOR_PASTEL_PEACH    0xFEDA
#define COLOR_PASTEL_LAVENDER 0xD69E
#define COLOR_PASTEL_SALMON   0xFDD4
#define COLOR_PASTEL_BEIGE    0xFFF4
#define COLOR_PASTEL_CREAM    0xFFFA

#define COLOR_DARK_RED        0x8000
#define COLOR_DARK_ORANGE     0xCA20
#define COLOR_DARK_AMBER      0xC620
#define COLOR_DARK_YELLOW     0xC660
#define COLOR_DARK_LIME       0x4680
#define COLOR_DARK_GREEN      0x03E0
#define COLOR_DARK_SEA        0x03B3
#define COLOR_DARK_CYAN       0x03BF
#define COLOR_DARK_BLUE       0x0012
#define COLOR_DARK_INDIGO     0x4013
#define COLOR_DARK_VIOLET     0x8012
#define COLOR_DARK_PURPLE     0x9012
#define COLOR_DARK_MAGENTA    0x9815
#define COLOR_DARK_PINK       0xA814
#define COLOR_DARK_ROSE       0xB815
#define COLOR_DARK_TEAL       0x036D
#define COLOR_DARK_OLIVE      0x6B40
#define COLOR_DARK_BROWN      0x6200
#define COLOR_DARK_GRAY       0x4208
#define COLOR_BLACK           0x0000
#define COLOR_DARK_NAVY       0x0010


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

/**
 * @brief Converte uma cor HSV normalizada para RGB565 de forma rápida.
 * @param h Matiz [0.0..1.0].
 * @param s Saturação [0.0..1.0].
 * @param v Valor (brilho) [0.0..1.0].
 * @return Cor no formato RGB565.
 */
inline uint16_t hsvToRgb565Fast(float h, float s, float v) {
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
inline HSV_t rgb565ToHsv(uint16_t color565) {
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
inline uint16_t* gerarDegradeHSV(float h, int numTons) {
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
inline uint16_t* blendColors(uint16_t startColor, uint16_t endColor, int numTons) {
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

/**
 * @brief Clareia uma cor RGB565 em direção ao branco.
 * 
 * @param color Cor original no formato RGB565.
 * @param value Intensidade do clareamento [0.0 .. 1.0].
 *        - 0.0 → retorna a cor original.
 *        - 1.0 → retorna branco puro (0xFFFF).
 * @return Cor clareada no formato RGB565.
 */
inline uint16_t lighten565(uint16_t color, float value) {
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
inline uint16_t darken565(uint16_t color, float value) {
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