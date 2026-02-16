#ifndef WLINECHART
#define WLINECHART

#include "../widgetbase.h"
#if defined(USING_GRAPHIC_LIB)
#include "../../fonts/RobotoRegular/RobotoRegular10pt7b.h"
#endif
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "../label/wlabel.h" // Para ponteiro Label

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
  uint16_t verticalDivision;     ///< Quantidade de divisões verticais (linhas horizontais na grade).
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
/// @details
///  - `push()` atualiza o buffer (ring buffer) de cada série, de forma O(1).
///  - O task de desenho faz um "snapshot" do buffer sob mutex e desenha SEM segurar o mutex.
///  - O apagamento é feito usando o "último desenhado", mantido pelo task de desenho.
///  - Isso evita deadlock e reduz contenção entre tasks.
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
  static const char* TAG;
  static constexpr uint8_t MAX_SERIES = 10;

  uint8_t m_dotRadius;
  uint8_t m_minSpaceToShowDot;
  uint32_t m_maxHeight;
  uint32_t m_maxWidth;
  uint16_t m_maxAmountValues;
  uint16_t m_amountPoints;
  float m_spaceBetweenPoints;
  int16_t m_leftPadding;
  uint8_t m_topBottomPadding;
  uint16_t m_borderSize;
  uint16_t m_yTovmin;
  uint16_t m_yTovmax;

  /// @brief Sinaliza que houve atualização nos dados (setado em push()).
  /// @note É um "dirty flag". O draw task pode limpar esta flag quando concluir um redraw
  ///       para a versão capturada do buffer (ver m_dataVersion).
  volatile bool m_shouldRedraw;

  /// @brief Contador de versão dos dados (incrementado em push()).
  /// @details Permite o draw task detectar se houve alterações durante o desenho
  ///          e agendar um novo redraw.
  volatile uint32_t m_dataVersion;

  /// @brief Heads do ring buffer (por série): indica o próximo índice a ser escrito (0..m_amountPoints-1).
  uint16_t m_headBySeries[MAX_SERIES];

  /// @brief Buffer atual (ring) por série.
  ///        A ordem lógica dos pontos é derivada de m_head (mais antigo -> mais novo).
  int** m_ringValues;

  /// @brief Snapshot usado para desenhar (linear, mais antigo -> mais novo), por série.
  int** m_snapshotValues;

  /// @brief Últimos valores desenhados (linear), por série. Usado para apagar a linha anterior.
  int** m_lastDrawnValues;

  uint16_t m_colorsSeries[MAX_SERIES];
  Label* m_subtitles[MAX_SERIES];

  SemaphoreHandle_t m_mutex;
  LineChartConfig m_config;

  // Lifecycle / memory
  void initMutex();
  void destroyMutex();
  void cleanupMemory();
  void clearColorsSeries();
  void clearSubtitles();
  void clearBuffers();

  // Setup / validation
  bool validateConfig(const LineChartConfig& config);
  void start();

  // Drawing helpers
  void drawGrid();
  void drawMarkLineAt(int value);

  void eraseSerieFromBuffer(uint8_t serieIndex, const int* values);
  void drawSerieFromBuffer(uint8_t serieIndex, const int* values);

  void eraseAllFromLastDrawn();
  void drawAllFromSnapshot();

  // Snapshot
  /// @brief Copia o ring buffer para m_snapshotValues em ordem lógica (mais antigo -> mais novo).
  /// @return versão capturada do buffer (m_dataVersion).
  uint32_t snapshotUnderMutex();

  // Debug
  void printValues(uint8_t serieIndex);
};

#endif
