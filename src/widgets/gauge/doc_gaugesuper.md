# Documentação da Classe GaugeSuper

## Visão Geral

A classe `GaugeSuper` é um widget visual avançado que exibe um gauge semicircular com agulha móvel, intervalos codificados por cores, marcadores graduados e rótulos opcionais. Ele herda de `WidgetBase` e fornece uma interface gráfica completa para criar gauges profissionais em displays.

### Características Principais

- 🎯 Gauge semicircular com agulha animada
- 🎨 Intervalos codificados por cores
- 📊 Marcadores graduados customizáveis
- 🏷️ Rótulos de valores opcionais
- 📝 Título opcional no gauge
- 🎨 Cores personalizáveis para todos os elementos
- 📐 Configuração flexível de faixa de valores
- 🔄 Animação suave da agulha
- 👁️ Pode ser mostrado/ocultado dinamicamente
- 🔗 Integra-se automaticamente com o sistema DisplayFK
- ⚡ Renderização eficiente com atualizações incrementais
- 💾 Gerenciamento automático de memória

---

## 📋 Estruturas de Configuração

### GaugeConfig

Estrutura que contém todos os parâmetros de configuração do gauge:

```cpp
struct GaugeConfig {
  uint16_t width;                      // Largura do gauge em pixels
  const char* title;                   // Título exibido no gauge (máx 20 caracteres)
  const int* intervals;                // Array de valores dos intervalos
  const uint16_t* colors;              // Array de cores para cada intervalo
  uint8_t amountIntervals;             // Número de intervalos/cores (máx 10)
  int minValue;                        // Valor mínimo da faixa
  int maxValue;                        // Valor máximo da faixa
  uint16_t borderColor;                // Cor RGB565 da borda
  uint16_t textColor;                  // Cor RGB565 do texto
  uint16_t backgroundColor;            // Cor RGB565 de fundo
  uint16_t titleColor;                 // Cor RGB565 do título
  uint16_t needleColor;                // Cor RGB565 da agulha
  uint16_t markersColor;               // Cor RGB565 dos marcadores
  bool showLabels;                     // Flag para mostrar rótulos
  const GFXfont* fontFamily;          // Fonte para texto
};
```

---

## 🔧 Métodos Públicos

### Construtor

```cpp
GaugeSuper(uint16_t _x, uint16_t _y, uint8_t _screen)
```

Cria um novo gauge super na posição especificada. A posição (`_x`, `_y`) representa o centro horizontal e o topo do retângulo do gauge.

**Parâmetros:**
- `_x`: Coordenada X do centro horizontal do gauge
- `_y`: Coordenada Y do topo do gauge
- `_screen`: Identificador da tela (0 = primeira tela)

**Nota:** Após criar o objeto, é obrigatório chamar `setup()` antes de usá-lo.

### Destrutor

```cpp
~GaugeSuper()
```

Libera todos os recursos do gauge, incluindo memória alocada dinamicamente para arrays.

### setup()

```cpp
void setup(const GaugeConfig& config)
```

Configura o gauge com os parâmetros especificados. **Este método deve ser chamado após a criação do objeto.**

**Parâmetros:**
- `config`: Estrutura `GaugeConfig` com as configurações

**Validações automáticas:**
- Valores min/max são trocados automaticamente se min > max
- Título é truncado se exceder 20 caracteres
- Intervalos são limitados a no máximo 10
- Memória é alocada dinamicamente para arrays

### setValue()

```cpp
void setValue(int newValue)
```

Define o valor atual do gauge e marca para redesenho da agulha.

**Parâmetros:**
- `newValue`: Novo valor a ser exibido pela agulha

**Características:**
- Valor é limitado automaticamente entre minValue e maxValue
- Armazena o valor anterior para renderização eficiente
- Marca o widget para redesenho apenas se o valor mudou
- Registra o evento no log do ESP32

### drawBackground()

```cpp
void drawBackground()
```

Desenha o fundo completo do gauge. **Este método deve ser chamado uma vez após setup() para inicializar o gauge.**

**Características:**
- Desenha bordas e fundo
- Desenha faixas coloridas baseadas nos intervalos
- Desenha marcadores graduados
- Desenha rótulos se habilitados
- Desenha título se fornecido
- Deve ser chamado antes de usar `setValue()`

### show()

```cpp
void show()
```

Torna o gauge visível na tela.

### hide()

```cpp
void hide()
```

Oculta o gauge da tela.

---

## 🔒 Métodos Privados (Apenas para Referência)

Estes métodos são chamados internamente e não precisam ser invocados diretamente:

- `detectTouch()`: Não processa eventos de toque (sempre retorna false)
- `redraw()`: Redesenha apenas a agulha e valor
- `forceUpdate()`: Força uma atualização imediata
- `getCallbackFunc()`: Retorna a função callback
- `start()`: Calcula dimensões e validações
- `cleanupMemory()`: Libera memória alocada
- `validateConfig()`: Valida configuração
- `isTitleVisible()`: Verifica se título está visível
- `isLabelsVisible()`: Verifica se rótulos estão visíveis

---

## 💻 Como Usar no Arduino IDE

### 📚 Passo 1: Incluir as Bibliotecas

```cpp
#include <displayfk.h>
// Incluir outras bibliotecas necessárias (SPI, display, etc.)
```

### 🏗️ Passo 2: Declarar Objetos Globais

```cpp
DisplayFK myDisplay;

// Criar o gauge: GaugeSuper(centro_x, topo_y, tela)
GaugeSuper widget(315, 800, 0);

// Criar array de ponteiros
const uint8_t qtdGauge = 1;
GaugeSuper *arrayGauge[qtdGauge] = {&widget};

// Array de intervalos e cores
const uint8_t qtdIntervalG0 = 4;
int range0[qtdIntervalG0] = {0, 25, 50, 75};
uint16_t colors0[qtdIntervalG0] = {CFK_COLOR09, CFK_COLOR06, CFK_COLOR03, CFK_COLOR01};
```

### 📝 Passo 3: Prototipar Funções

```cpp
// Gauge não usa callbacks por padrão
```

### ⚙️ Passo 4: Configurar Display (setup)

```cpp
void setup() {
    // ... configuração do display e touch ...
    myDisplay.setDrawObject(tft);
    myDisplay.startTouchGT911(/* ... parâmetros ... */);
    
    loadWidgets();  // Configurar widgets
    myDisplay.loadScreen(screen0);
    myDisplay.createTask(false, 3);
}
```

### 🔧 Passo 5: Configurar os Widgets

```cpp
void loadWidgets() {
    // Configurar gauge super
    GaugeConfig configGauge = {
        .width = 205,
        .title = "Titulo",
        .intervals = range0,
        .colors = colors0,
        .amountIntervals = qtdIntervalG0,
        .minValue = 0,
        .maxValue = 100,
        .borderColor = CFK_WHITE,
        .textColor = CFK_BLACK,
        .backgroundColor = CFK_WHITE,
        .titleColor = CFK_BLACK,
        .needleColor = CFK_RED,
        .markersColor = CFK_BLACK,
        .showLabels = true,
        .fontFamily = &RobotoBold10pt7b
    };
    widget.setup(configGauge);
    
    // Desenhar fundo uma vez
    widget.drawBackground();
    
    // Registrar no DisplayFK
    myDisplay.setGauge(arrayGauge, qtdGauge);
}
```

### 🔔 Passo 6: Criar Funções de Callback (Opcional)

```cpp
// Gauge não usa callbacks normalmente
```

### 🖥️ Passo 7: Função da Tela

```cpp
void screen0() {
    tft->fillScreen(CFK_WHITE);
    WidgetBase::backgroundColor = CFK_WHITE;
    
    // Desenhar todos os widgets na tela 0
    myDisplay.drawWidgetsOnScreen(0);
}
```

### 🔄 Passo 8: Atualizar Valor (Exemplo de Uso)

```cpp
void loop() {
    // Simular dados de sensor
    int valorSensor = random(0, 101);
    widget.setValue(valorSensor);
    
    delay(1000);
}
```

---

## 📝 Exemplo Completo

```cpp
#include <displayfk.h>
#include <fonts/RobotoRegular/RobotoRegular10pt7b.h>

DisplayFK myDisplay;
Arduino_DataBus *bus = nullptr;
Arduino_GFX *gfx = nullptr;

// Criar gauge
GaugeSuper meuGauge(320, 200, 0);

const uint8_t qtdGauge = 1;
GaugeSuper *arrayGauge[qtdGauge] = {&meuGauge};

// Intervalos e cores
const uint8_t qtdIntervalos = 3;
int intervalos[qtdIntervalos] = {30, 60, 90};
uint16_t cores[qtdIntervalos] = {CFK_COLOR09, CFK_COLOR06, CFK_COLOR01};

void setup() {
    Serial.begin(115200);
    
    // Inicializar display
    bus = new Arduino_ESP32SPI(/* ... */);
    gfx = new Arduino_ST7789(/* ... */);
    gfx->begin();
    
    myDisplay.setDrawObject(gfx);
    
    // Configurar gauge
    GaugeConfig config = {
        .width = 300,
        .title = "Velocidade",
        .intervals = intervalos,
        .colors = cores,
        .amountIntervals = qtdIntervalos,
        .minValue = 0,
        .maxValue = 100,
        .borderColor = CFK_BLACK,
        .textColor = CFK_BLACK,
        .backgroundColor = CFK_WHITE,
        .titleColor = CFK_NAVY,
        .needleColor = CFK_RED,
        .markersColor = CFK_BLACK,
        .showLabels = true,
        .fontFamily = &RobotoBold10pt7b
    };
    meuGauge.setup(config);
    
    // Desenhar fundo
    meuGauge.drawBackground();
    
    myDisplay.setGauge(arrayGauge, qtdGauge);
    myDisplay.loadScreen(minhaTela);
    myDisplay.createTask(false, 3);
}

void loop() {
    // Simular dados
    int valor = random(0, 101);
    meuGauge.setValue(valor);
    
    delay(500);
}

void minhaTela() {
    gfx->fillScreen(CFK_WHITE);
    WidgetBase::backgroundColor = CFK_WHITE;
    myDisplay.drawWidgetsOnScreen(0);
}
```

---

## 📏 Dicas e Boas Práticas

### 📐 Tamanhos Recomendados
- **Pequenos:** Largura 150-200 pixels
- **Médios:** Largura 250-350 pixels
- **Grandes:** Largura 400-500 pixels

### 🎨 Cores
- Use cores contrastantes entre intervalos
- Cores vibrantes destacam faixas importantes
- Agulha vermelha é tradicional e visível
- Consider o tema claro/escuro da interface

### 🔔 Callbacks
- Gauge não usa callbacks por padrão
- É apenas visual, não interativo

### ⚡ Performance
- Renderização eficiente com atualizações incrementais
- Desenha apenas a agulha e valor na atualização
- Desenhe fundo apenas uma vez com drawBackground()
- Evite atualizações muito frequentes de valor

### 👥 Usabilidade
- Use intervalos logicamente separados
- Rotule claramente os intervalos
- Escolha título descritivo mas curto
- Teste legibilidade dos rótulos

### 🎨 Design Visual
- O gauge desenha um semicírculo com agulha central
- Faixas coloridas representam intervalos de valores
- Marcadores graduados indicam valores específicos
- Título é exibido acima ou dentro do gauge
- Agulha aponta para o valor atual
- Valor é exibido no centro inferior

### 📊 Configuração de Intervalos
- Limite máximo de 10 intervalos
- Intervalos devem estar ordenados entre min e max
- Cada intervalo tem uma cor associada
- Rótulos mostram valores dos intervalos
- Intervalos devem ter espaçamento adequado

---

## 🔗 Herança de WidgetBase

A classe `GaugeSuper` herda todos os métodos de `WidgetBase`:

- `isEnabled()` / `setEnabled()`: Habilitar/desabilitar widget
- `isInitialized()`: Verificar se foi configurado
- `m_visible`: Controla visibilidade interna
- `m_shouldRedraw`: Flag para redesenho automático

---

## 🔗 Integração com DisplayFK

O `GaugeSuper` integra-se automaticamente com o sistema DisplayFK:

1. **Renderização:** Automática quando usando `drawWidgetsOnScreen()`
2. **Sincronização:** Estados sincronizados entre diferentes telas
3. **Gerenciamento:** Controlado pelo loop principal do DisplayFK
4. **Performance:** Renderização eficiente com atualizações incrementais
5. **Fontes:** Usa fonte configurada (RobotoBold por padrão)
6. **Memória:** Gerenciamento automático de memória

---

## 🎨 Detalhes de Renderização

O `GaugeSuper` é renderizado em camadas:

1. **Fundo** (drawBackground()):
   - Bordas do gauge
   - Fundo com cor configurada
   - Faixas coloridas baseadas nos intervalos
   - Marcadores graduados
   - Rótulos de valores (se habilitados)
   - Título (se fornecido)
   - Desenhado uma vez após setup()

2. **Agulha** (redraw()):
   - Cálculo da posição angular baseado no valor
   - Apaga agulha anterior
   - Desenha nova agulha na posição calculada
   - Atualização incremental para eficiência

3. **Valor** (redraw()):
   - Valor atual exibido no centro inferior
   - Fonte configurada
   - Cor de texto customizável

---

## 🔧 Solução de Problemas

### Gauge não aparece na tela
- Verifique se chamou `setup()` após criar o objeto
- Confirme que `drawBackground()` foi chamado após setup()
- Verifique se está na tela correta
- Certifique-se de chamar `myDisplay.setGauge()`
- Confirme que a largura é maior que 0

### Gauge não atualiza a agulha
- Verifique se chamou `setValue()` com novo valor
- Confirme que o valor está dentro da faixa min/max
- Verifique se o widget está habilitado
- Certifique-se de que `drawBackground()` foi chamado

### Intervalos não aparecem ou cores erradas
- Verifique se arrays de intervalos e cores foram fornecidos
- Confirme que amountIntervals está correto
- Verifique se intervalos estão ordenados
- Certifique-se de que não excede 10 intervalos

### Rótulos não aparecem
- Verifique se `showLabels = true` na configuração
- Confirme que fontFamily foi configurado
- Verifique se há espaço suficiente para os rótulos
- Certifique-se de que o gauge tem largura adequada

### Título não aparece
- Verifique se `title` foi fornecido e não é nullptr
- Confirme que o título não excede 20 caracteres
- Verifique se há espaço suficiente no gauge
- Certifique-se de que titleColor contrasta com o fundo

### Problemas de memória
- O gauge aloca memória dinamicamente
- Arrays são liberados automaticamente no destrutor
- Não reconfigure o mesmo gauge muitas vezes
- Verifique logs do ESP32 para erros de alocação

### Performance lenta
- Evite atualizações muito frequentes de valor
- Use valores dentro da faixa configurada
- Considera diminuir a largura do gauge
- Verifique se não há muitos widgets na mesma tela

---

## 📚 Referências

- **Classe Base:** `WidgetBase` (src/widgets/widgetbase.h)
- **DisplayFK Principal:** `DisplayFK` (src/displayfk.h)
- **Exemplos:** examples/Embed_ESP32S3/Display_Test/
- **Cores Padrão:** Definidas em displayfk.h (CFK_COLOR01-CFK_COLOR28, CFK_GREY*, CFK_WHITE, CFK_BLACK, CFK_NAVY, CFK_RED)
- **Fonte:** RobotoBold10pt7b (src/fonts/RobotoRegular/)

