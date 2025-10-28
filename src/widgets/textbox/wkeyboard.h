#ifndef WKEYBOARD
#define WKEYBOARD

#include "../widgetbase.h"
#include "wtextbox.h"
#include "../../extras/charstring.h"
#if defined(USING_GRAPHIC_LIB)
#include "../../fonts/RobotoRegular/RobotoRegular10pt7b.h"
#endif

/// @brief Número de linhas na grade do teclado
#define AROWS 5

/// @brief Número de colunas na grade do teclado
#define ACOLS 10

/// @brief Widget de teclado na tela com modos de entrada alfanumérica e numérica.
/// @details Esta classe herda de @ref WidgetBase e fornece funcionalidade completa para criar e gerenciar 
///          teclados virtuais na tela. O WKeyboard desenha um teclado QWERTY completo com layout minúsculo/maiúsculo,
///          números, símbolos e teclas de controle (DEL, CAP, CLR, OK). O widget pode ser configurado
///          com diferentes cores e trabalha em conjunto com TextBox para entrada de texto.
///          O teclado é totalmente funcional com suporte a caps lock, detecção de toque, área de preview
///          para exibir entrada e callbacks para ações de teclas especiais.
class WKeyboard : public WidgetBase
{
public:
  static uint16_t m_backgroundColor; ///< Cor de fundo estática para o teclado.
  static uint16_t m_letterColor; ///< Cor de letras estática para o teclado.
  static uint16_t m_keyColor; ///< Cor de teclas estática para o teclado.
  

  /// @brief Enum para especificar tipos de entrada do teclado.
  enum class KeyboardType
  {
    NONE = 0,         ///< Nenhum tipo de entrada específico.
    ALPHANUMERIC = 1, ///< Tipo de entrada alfanumérica.
    INTEGER = 2,      ///< Tipo de entrada inteira.
    DECIMAL = 3       ///< Tipo de entrada decimal.
  };

  TextBox *m_field; ///< Ponteiro para o campo TextBox associado.
  
  WKeyboard(uint16_t _x, uint16_t _y, uint8_t _screen);
  WKeyboard();
  ~WKeyboard();
  
  bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) override;
  functionCB_t getCallbackFunc() override;
  bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch, PressedKeyType *pressedKey);
  
  void redraw() override;
  void drawKeys(bool fullScreen, bool onlyContent);
  void forceUpdate() override;
  
  void clear();
  bool setup();
  void open(TextBox *_field);
  void close();
  void insertChar(char c);
  
  void show() override;
  void hide() override;

private:
  static const char* TAG; ///< Tag estática para identificação em logs do ESP32.
  
  const uint16_t aRows = AROWS; ///< Número de linhas no layout do alfabeto.
  const uint16_t aCols = ACOLS; ///< Número de colunas no layout do alfabeto.
  static const Key_t m_alphabet[AROWS][ACOLS]; ///< Layout do alfabeto minúsculo para o teclado.
  static const Key_t m_alphabetCap[AROWS][ACOLS]; ///< Layout do alfabeto maiúsculo para o teclado.
  
  uint32_t m_yStart; ///< Coordenada Y para a posição inicial do teclado.
  uint32_t m_keyW; ///< Largura de cada tecla.
  uint32_t m_keyH; ///< Altura de cada tecla.
  unsigned long m_myTime; ///< Timestamp para funções relacionadas a tempo.
  int32_t m_screenW; ///< Largura da tela.
  int32_t m_screenH; ///< Altura da tela.
  uint16_t m_availableWidth; ///< Largura disponível para renderizar o teclado.
  uint16_t m_availableHeight; ///< Altura disponível para renderizar o teclado.
  bool m_capsLock; ///< Status do caps lock (true se habilitado).
  CharString m_content; ///< Armazena o conteúdo atual inserido pelo usuário.
  TextBound_t lastArea; ///< Última área calculada para o texto.
  Rect_t m_pontoPreview; ///< Retângulo de preview para o ponto de toque.
  bool m_shouldRedraw; ///< Flag indicando se o WKeyboard deve ser redesenhado.
  
  #if defined(USING_GRAPHIC_LIB)
  GFXfont *m_fontKeys; ///< Fonte para as teclas.
  GFXfont *m_fontPreview; ///< Fonte para a área de preview.
  #endif
  
  void addLetter(char c);
  void removeLetter();
};

#endif