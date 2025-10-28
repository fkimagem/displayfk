# Documentação da Classe LineChart

## Visão Geral

A classe `LineChart` é um widget visual avançado que exibe gráficos de linhas com múltiplas séries de dados. Ele herda de `WidgetBase` e fornece funcionalidade completa para criar e gerenciar gráficos com atualizações em tempo real, grade de referência e suporte a até 10 séries simultâneas.

### Características Principais

- 📊 Gráfico de linhas com múltiplas séries
- 🎨 Cores personalizáveis para cada série
- 📈 Suporte a até 10 séries simultâneas
- 🔢 Atualizações em tempo real
- 📏 Grade de referência configurável
- 🔄 Suporte a mutex para thread-safety
- 💾 Gerenciamento inteligente de memória
- 🎯 Rótulos de legenda opcionais
- 🔵 Pontos nas linhas opcionais
- 👁️ Pode ser mostrado/ocultado dinamicamente
- 🔗 Integra-se automaticamente com o sistema DisplayFK

---

## 📋 Estruturas de Configuração

### LineChartConfig

Estrutura que contém todos os parâmetros de configuração do gráfico:

```cpp
struct LineChartConfig {
  uint16_t width;                // Largura do gráfico em pixels
  uint16_t height;               // Altura do gráfico em pixels
  int minValue;                  // Valor mínimo da faixa
  int maxValue;                  // Valor máximo da faixa
  uint8_t amountSeries;          // Número de séries (máx 10)
  uint16_t* colorsSeries;        // Array de cores para cada série
  uint16_t gridColor;            // Cor das linhas da grade
  uint16_t borderColor;          // Cor da borda
  uint16_t backgroundColor;      // Cor de fundo
  uint16_t textColor;            // Cor do texto
  uint16_t verticalDivision;     // Espaçamento entre linhas verticais
  bool workInBackground;         // Flag para desenho em background
  bool showZeroLine;             // Flag para mostrar linha zero
  bool boldLine;                 // Flag para linha em negrito
  bool showDots;                 // Flag para mostrar pontos
  uint16_t maxPointsAmount;      // Número máximo de pontos
  const GFXfont* font;           // Fonte para texto
  Label** subtitles;             // Array de Labels para legenda
};
```

### Constantes

```cpp
static constexpr uint16_t SHOW_ALL = 9999; // Mostra todos os pontos
static constexpr uint8_t MAX_SERIES = 10;  // Máximo de séries
```

---

## 🔧 Métodos Públicos

### Construtor

```cpp
LineChart(uint16_t _x, uint16_t _y, uint8_t _screen)
```

Cria um novo gráfico de linhas na posição especificada.

**Parâmetros:**
- `_x`: Coordenada X do canto superior esquerdo
- `_y`: Coordenada Y do canto superior esquerdo
- `_screen`: Identificador da tela (0 = primeira tela)

**Nota:** Após criar o objeto, é obrigatório chamar `setup()` antes de usá-lo.

### Destrutor

```cpp
~LineChart()
```

Libera automaticamente a memória alocada para séries de dados.

### setup()

```cpp
void setup(const LineChartConfig& config)
```

Configura o gráfico com os parâmetros especificados. **Este método deve ser chamado após a criação do objeto.**

**Parâmetros:**
- `config`: Estrutura `LineChartConfig` com as configurações

### push()

```cpp
bool push(uint16_t serieIndex, int newValue)
```

Adiciona um novo valor a uma série específica do gráfico.

**Parâmetros:**
- `serieIndex`: Índice da série (0 a amountSeries-1)
- `newValue`: Novo valor a ser adicionado

**Retorna:**
- `true`: Valor adicionado com sucesso
- `false`: Erro ao adicionar valor

**Características:**
- Valor é limitado automaticamente entre minValue e maxValue
- Desloca valores antigos para frente
- Thread-safe com mutex
- Marca para redesenho

### drawBackground()

```cpp
void drawBackground()
```

Desenha o fundo do gráfico (borda, grade e fundo). **Este método deve ser chamado uma vez após setup().**

### show()

```cpp
void show()
```

Torna o gráfico visível na tela.

### hide()

```cpp
void hide()
```

Oculta o gráfico da tela.

---

## 🔒 Métodos Privados (Apenas para Referência)

Estes métodos são chamados internamente:

- `detectTouch()`: Não processa eventos de toque
- `redraw()`: Redesenha o gráfico
- `forceUpdate()`: Força atualização
- `validateConfig()`: Valida configuração
- `cleanupMemory()`: Limpa memória
- `drawGrid()`: Desenha grade de referência
- `drawSerie()`: Desenha uma série específica
- `drawAllSeries()`: Desenha todas as séries

---

## 💻 Como Usar no Arduino IDE

### 📚 Passo 1: Incluir as Bibliotecas

```cpp
#include <displayfk.h>
// Incluir outras bibliotecas necessárias
```

### 🏗️ Passo 2: Declarar Objetos Globais

```cpp
DisplayFK myDisplay;

// Labels para legenda
Label *seriesGrafico0[2] = {&line1, &line2};
const uint8_t qtdLinesChart0 = 2;
uint16_t colorsChart0[qtdLinesChart0] = {CFK_COLOR09, CFK_COLOR04};

// Criar gráfico
LineChart linechart(20, 500, 0);

// Criar array de ponteiros
const uint8_t qtdLineChart = 1;
LineChart *arrayLinechart[qtdLineChart] = {&linechart};
```

### 📝 Passo 3: Prototipar Funções (Opcional)

```cpp
// LineChart não usa callbacks normalmente
```

### ⚙️ Passo 4: Configurar Display (setup)

```cpp
void setup() {
    // ... configuração do display ...
    myDisplay.setDrawObject(tft);
    
    loadWidgets();
    myDisplay.loadScreen(screen0);
    myDisplay.createTask(false, 3);
}
```

### 🔧 Passo 5: Configurar o Gráfico

```cpp
void loadWidgets() {
    // Configurar gráfico de linhas
    LineChartConfig configLineChart = {
        .width = 500,
        .height = 113,
        .minValue = 0,
        .maxValue = 100,
        .amountSeries = qtdLinesChart0,
        .colorsSeries = colorsChart0,
        .gridColor = CFK_COLOR01,
        .borderColor = CFK_BLACK,
        .backgroundColor = CFK_GREY4,
        .textColor = CFK_COLOR19,
        .verticalDivision = 10,
        .workInBackground = false,
        .showZeroLine = true,
        .boldLine = false,
        .showDots = false,
        .maxPointsAmount = LineChart::SHOW_ALL,
        .font = &RobotoRegular5pt7b,
        .subtitles = seriesGrafico0
    };
    linechart.setup(configLineChart);
    
    // Desenhar fundo
    linechart.drawBackground();
    
    // Registrar no DisplayFK
    myDisplay.setLineChart(arrayLinechart, qtdLineChart);
}
```

### 🔔 Passo 6: Função da Tela

```cpp
void screen0() {
    tft->fillScreen(CFK_WHITE);
    WidgetBase::backgroundColor = CFK_WHITE;
    
    myDisplay.drawWidgetsOnScreen(0);
}
```

### 🔄 Passo 7: Adicionar Dados (Exemplo)

```cpp
void loop() {
    // Adicionar valores às séries
    int valorSerie0 = random(0, 100);
    int valorSerie1 = random(0, 100);
    
    linechart.push(0, valorSerie0);
    linechart.push(1, valorSerie1);
    
    delay(100);
}
```

---

## 📝 Exemplo Completo

```cpp
#include <displayfk.h>

DisplayFK myDisplay;

// Labels para série
Label tempLabel(100, 450, 0);
Label umidadeLabel(200, 450, 0);

Label *seriesLabel[2] = {&tempLabel, &umidadeLabel};
uint16_t seriesColors[2] = {CFK_RED, CFK_BLUE};

LineChart meuGrafico(50, 100, 0);

const uint8_t qtdGrafico = 1;
LineChart *arrayGrafico[qtdGrafico] = {&meuGrafico};

void setup() {
    Serial.begin(115200);
    
    // Configurar labels
    tempLabel.setup({.text = "Temp", /* ... */});
    umidadeLabel.setup({.text = "Umidade", /* ... */});
    
    // Configurar gráfico
    LineChartConfig config = {
        .width = 400,
        .height = 200,
        .minValue = 0,
        .maxValue = 100,
        .amountSeries = 2,
        .colorsSeries = seriesColors,
        .gridColor = CFK_GREY8,
        .borderColor = CFK_BLACK,
        .backgroundColor = CFK_WHITE,
        .textColor = CFK_BLACK,
        .verticalDivision = 10,
        .workInBackground = false,
        .showZeroLine = true,
        .boldLine = false,
        .showDots = true,
        .maxPointsAmount = 50,
        .font = &RobotoRegular10pt7b,
        .subtitles = seriesLabel
    };
    meuGrafico.setup(config);
    meuGrafico.drawBackground();
    
    myDisplay.setLineChart(arrayGrafico, qtdGrafico);
    myDisplay.loadScreen(minhaTela);
    myDisplay.createTask(false, 3);
}

void loop() {
    float temperatura = random(200, 300) / 10.0;
    float umidade = random(40, 80);
    
    meuGrafico.push(0, (int)temperatura);
    meuGrafico.push(1, (int)umidade);
    
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
- **Pequenos:** 200x100 pixels
- **Médios:** 400x200 pixels  
- **Grandes:** 600x300 pixels

### 🎨 Configuração de Séries
- Máximo de 10 séries por gráfico
- Uma cor para cada série
- Labels opcionais para legendas
- Valores devem estar entre minValue e maxValue

### 🔔 Performance
- Atualizações thread-safe com mutex
- Evite muitas atualizações muito rápidas
- Use maxPointsAmount para limitar pontos
- Trabalhar em background se necessário
- Configurações

### 👥 Usabilidade
- Configure grade clara e legível
- Use cores contrastantes para séries
- Mostre linha zero quando relevante
- Adicione legendas para clareza
- Pontos nas linhas melhoram visibilidade

### 🎨 Efeitos Visuais
- `showDots`: Mostra pontos nas linhas
- `boldLine`: Linha em negrito para destaque
- `showZeroLine`: Linha de referência em zero
- Grid vertical para fácil leitura
- Cores diferentes para cada série

---

## 🔗 Herança de WidgetBase

A classe `LineChart` herda métodos de `WidgetBase`:

- `isEnabled()` / `setEnabled()`: Habilitar/desabilitar
- `isInitialized()`: Verificar configuração
- `m_visible`: Controla visibilidade
- `m_shouldRedraw`: Flag para redesenho

---

## 🔗 Integração com DisplayFK

O `LineChart` integra-se com DisplayFK:

1. **Renderização:** Automática com `drawWidgetsOnScreen()`
2. **Thread-Safety:** Mutex para acesso seguro
3. **Gerenciamento:** Controlado pelo loop principal
4. **Performance:** Redesenho eficiente
5. **Séries:** Múltiplas séries simultâneas

---

## 🎨 Detalhes de Renderização

O gráfico renderiza em camadas:

1. **Fundo:**
   - Borda ao redor do gráfico
   - Fundo com cor configurada
   - Grade de referência

2. **Linhas:**
   - Uma linha por série
   - Cores diferentes por série
   - Pontos opcionais nas linhas

3. **Legendas:**
   - Labels opcionais para séries
   - Posicionadas abaixo do gráfico

---

## 🔧 Solução de Problemas

### Gráfico não aparece
- Chame `setup()` após criar o objeto
- Confirme que `drawBackground()` foi chamado
- Verifique se está na tela correta
- Chame `myDisplay.setLineChart()`

### Dados não aparecem
- Use `push()` para adicionar valores
- Verifique índice da série (0 a amountSeries-1)
- Confirme valores entre minValue e maxValue
- Verifique se gráfico está visível

### Problemas de performance
- Limite número de pontos com maxPointsAmount
- Evite muitas atualizações rápidas
- Reduza número de séries se necessário
- Use workInBackground se apropriado

---

## 📚 Referências

- **Classe Base:** `WidgetBase` (src/widgets/widgetbase.h)
- **DisplayFK Principal:** `DisplayFK` (src/displayfk.h)
- **Exemplos:** examples/Embed_ESP32S3/Display_Test/
- **Cores:** Definidas em displayfk.h
- **Fontes:** src/fonts/RobotoRegular/

