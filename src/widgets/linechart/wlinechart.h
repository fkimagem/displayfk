#ifndef WLINECHART
#define WLINECHART

#include "../widgetbase.h"
#if defined(USING_GRAPHIC_LIB)
#include "../../fonts/RobotoRegular/RobotoRegular10pt7b.h"
#endif
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "../label/wlabel.h" // Para ponteiro Label

typedef struct {
  int currentValue;
  int oldValue;
} LineChartValue_t;

/// @brief Estrutura de configuração para o LineChart.
/// @details Esta estrutura contém todos os parâmetros necessários para configurar um gráfico de linhas.
///          Deve ser preenchida e passada para o método setup().
struct LineChartConfig {
  uint16_t width;                ///< Largura do gráfico em pixels.
  uint16_t height;               ///< Altura do gráfico em pixels.
  int minValue;                  ///< Valor mínimo para a faixa do gráfico.
  int maxValue;                  ///< Valor máximo para a faixa do gráfico.
  uint8_t amountSeries;          ///< Número de séries para plotar.
  uint16_t* colorsSeries;        ///< Array de cores para cada série.
  uint16_t gridColor;            ///< Cor das linhas da grade.
  uint16_t borderColor;          ///< Cor da borda do gráfico.
  uint16_t backgroundColor;      ///< Cor de fundo do gráfico.
  uint16_t textColor;            ///< Cor do texto exibido no gráfico.
  uint16_t verticalDivision;     ///< Espaçamento entre linhas verticais da grade.
  bool workInBackground;         ///< Flag para desenho em background.
  bool showZeroLine;             ///< Flag para mostrar a linha zero no gráfico.
  bool boldLine;                 ///< Flag para tornar a linha em negrito.
  bool showDots;                 ///< Flag para mostrar os pontos na linha.
  uint16_t maxPointsAmount;      ///< Número máximo de pontos para mostrar no gráfico.
  #if defined(USING_GRAPHIC_LIB)
  const GFXfont* font;           ///< Fonte usada para texto no gráfico.
  #endif
  Label** subtitles;             ///< Array de ponteiros para Label para cada série (pode ser nullptr).
};

/// @brief Widget de gráfico de linhas para plotar dados com múltiplas séries opcionais.
/// @details Esta classe herda de @ref WidgetBase e fornece funcionalidade completa para criar e gerenciar 
///          gráficos de linhas interativos na tela. O LineChart desenha um gráfico com múltiplas séries de dados,
///          grade de referência, rótulos de valores e suporte a atualizações em tempo real. O widget pode ser
///          configurado com diferentes tamanhos, faixas de valores, cores para cada série, grid, linhas em negrito,
///          pontos nas linhas e pode exibir até 10 séries simultaneamente. O gráfico é totalmente funcional com
///          suporte a mutex para acesso thread-safe e atualização de dados em background.
class LineChart : public WidgetBase
{
public:
  static constexpr uint16_t SHOW_ALL = 9999; ///< Constante para mostrar todos os pontos (máximo 9999).
  
  LineChart(uint16_t _x, uint16_t _y, uint8_t _screen);
  ~LineChart();
  bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) override;
  functionCB_t getCallbackFunc() override;
  void drawBackground();
  bool push(uint16_t serieIndex, int newValue);
  void redraw() override;
  void forceUpdate() override;
  void setup(const LineChartConfig& config);
  void show() override;
  void hide() override;

private:
  static const char* TAG; ///< Tag estática para identificação em logs do ESP32.
  static constexpr uint8_t MAX_SERIES = 10; ///< Número máximo de séries permitidas.
  
  uint8_t m_dotRadius; ///< Raio dos pontos na linha.
  uint8_t m_minSpaceToShowDot; ///< Espaço mínimo entre pontos para mostrar os pontos.
  uint32_t m_maxHeight; ///< Altura disponível para plotar.
  uint32_t m_maxWidth; ///< Largura disponível para plotar.
  uint16_t m_maxAmountValues; ///< Número máximo de valores para armazenar no gráfico.
  uint16_t m_amountPoints; ///< Número atual de pontos no gráfico.
  float m_spaceBetweenPoints; ///< Espaçamento calculado entre pontos.
  int16_t m_leftPadding; ///< Padding esquerdo para área de plotagem.
  uint8_t m_topBottomPadding; ///< Padding superior e inferior para área de plotagem.
  int16_t m_aux; ///< Variável auxiliar para cálculos.
  bool m_blocked; ///< Indica se o gráfico está atualmente bloqueado para atualizações.
  uint16_t m_borderSize; ///< Tamanho da borda para o gráfico.
  uint16_t m_availableWidth; ///< Largura disponível para área do gráfico.
  uint16_t m_availableheight; ///< Altura disponível para área do gráfico.
  uint16_t m_yTovmin; ///< Valor de mapeamento para Y mínimo.
  uint16_t m_yTovmax; ///< Valor de mapeamento para Y máximo.
  bool m_shouldRedraw; ///< Flag para indicar se o gráfico deve ser redesenhado.
  bool m_valuesAllocated; ///< Rastreia se o array de valores foi alocado.
  bool m_colorsSeriesAllocated; ///< Rastreia se o array de cores foi alocado.
  LineChartValue_t** m_values; ///< Array 2D de valores para todas as séries.
  uint16_t m_colorsSeries[10]; ///< Armazenamento interno para cores (cópia profunda).
  Label* m_subtitles[10]; ///< Armazenamento interno para ponteiros de legenda.
  SemaphoreHandle_t m_mutex; ///< Mutex para proteger acesso aos dados.
  LineChartConfig m_config; ///< Configuração para o widget LineChart.
  
  void cleanupMemory();
  void clearColorsSeries();
  void clearSubtitles();
  void clearValues();
  bool validateConfig(const LineChartConfig& config);
  void initMutex();
  void destroyMutex();
  void start();
  void resetArray();
  void printValues(uint8_t serieIndex);
  void drawGrid();
  void clearPreviousValues();
  void drawMarkLineAt(int value);
  void drawSerie(uint8_t serieIndex);
  void drawAllSeries();
  void copyCurrentValuesToOldValues();
};

#endif