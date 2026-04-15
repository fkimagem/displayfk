# Documentação da Classe VBar (VerticalBar)

## Visão Geral

A classe `VBar` é um widget que exibe valores numéricos como uma barra vertical ou horizontal preenchida proporcionalmente. Ele herda de `WidgetBase` e fornece funcionalidade para criar barras de progresso visuais com cantos arredondados e orientação configurável.

### Características Principais

- 📊 Barra de progresso vertical ou horizontal
- 🎨 Cor personalizável para preenchimento
- 📏 Cantos arredondados configuráveis
- 📈 Faixa de valores configurável (min/max)
- 🔄 Suporte a orientação vertical e horizontal
- 🔢 Integração opcional com Label para exibir valor numérico
- 👁️ Pode ser mostrado/ocultado dinamicamente
- 🔗 Integra-se automaticamente com o sistema DisplayFK

---

## 📋 Estruturas de Configuração

### VerticalBarConfig

Estrutura que contém todos os parâmetros de configuração (ordem conforme `wverticalbar.h`):

```cpp
struct VerticalBarConfig {
  Label* subtitle;           // Ponteiro para Label opcional para exibir o valor (pode ser nullptr).
  int minValue;              // Valor mínimo da faixa.
  int maxValue;              // Valor máximo da faixa.
  int round;                 // Raio para cantos arredondados.
  Orientation orientation;   // VERTICAL ou HORIZONTAL.
  uint16_t width;            // Largura da barra em pixels.
  uint16_t height;           // Altura da barra em pixels.
  uint16_t filledColor;      // Cor da porção preenchida.
};
```

---

## 🔧 Métodos Públicos

### Construtor

```cpp
VBar(uint16_t _x, uint16_t _y, uint8_t _screen)
```

Cria uma nova barra de progresso.

**Parâmetros:**
- `_x`: Coordenada X do canto superior esquerdo
- `_y`: Coordenada Y do canto superior esquerdo
- `_screen`: Identificador da tela (0 = primeira tela)

**Nota:** Após criar o objeto, chame `setup()` antes de usá-lo.

### Destrutor

```cpp
~VBar()
```

Libera os recursos do VBar.

### setup()

```cpp
void setup(const VerticalBarConfig& config)
```

Configura a barra de progresso. **Este método deve ser chamado após a criação do objeto.**

**Parâmetros:**
- `config`: Estrutura `VerticalBarConfig` com as configurações

### drawBackground()

```cpp
void drawBackground()
```

Desenha o fundo da barra (container). **Este método deve ser chamado uma vez após setup().**

### setValue()

```cpp
void setValue(int newValue)
```

Define o valor atual da barra.

**Parâmetros:**
- `newValue`: Novo valor (será limitado entre minValue e maxValue)

### setMinValue()

```cpp
void setMinValue(int newValue)
```

Define o valor mínimo da faixa da barra.

**Parâmetros:**
- `newValue`: Novo valor mínimo

**Características:**
- Ordena automaticamente os valores se min > max
- Ajusta o valor atual para ficar dentro da nova faixa
- Marca para redesenho completo da barra
- Ativa flag interna `m_changedScale` que força limpeza completa antes de redesenhar

### setMaxValue()

```cpp
void setMaxValue(int newValue)
```

Define o valor máximo da faixa da barra.

**Parâmetros:**
- `newValue`: Novo valor máximo

**Características:**
- Ordena automaticamente os valores se min > max
- Ajusta o valor atual para ficar dentro da nova faixa
- Marca para redesenho completo da barra
- Ativa flag interna `m_changedScale` que força limpeza completa antes de redesenhar

### getMinValue()

```cpp
int getMinValue()
```

Retorna o valor mínimo atual da faixa da barra.

**Retorno:**
- Valor mínimo configurado

### getMaxValue()

```cpp
int getMaxValue()
```

Retorna o valor máximo atual da faixa da barra.

**Retorno:**
- Valor máximo configurado

### setScale()

```cpp
void setScale(int newMinValue, int newMaxValue)
```

Define simultaneamente os valores mínimo e máximo da faixa da barra.

**Parâmetros:**
- `newMinValue`: Novo valor mínimo
- `newMaxValue`: Novo valor máximo

**Características:**
- Ordena automaticamente os valores se min > max
- Ajusta o valor atual para ficar dentro da nova faixa
- Marca para redesenho completo da barra
- Ativa flag interna `m_changedScale` que força limpeza completa antes de redesenhar
- Útil para mudanças dinâmicas de escala

### show()

```cpp
void show()
```

Torna a barra visível.

### hide()

```cpp
void hide()
```

Oculta a barra.

---

## 🔒 Métodos Internos (Apenas para Referência)

Estes métodos existem na classe e são usados internamente:

- `detectTouch()`: Não processa eventos de toque
- `redraw()`: Redesenha a barra
- `forceUpdate()`: Força atualização
- `getCallbackFunc()`: Retorna `m_callback` (não há configuração de callback específica no `VBar`)
- `cleanupMemory()`: Limpeza interna (sem alocação dinâmica relevante)
- `start()`: Ajusta `width` e `height` com `constrain()` (mínimo 20 e máximo limitado ao tamanho atual do display)
- `sortValues()`: Ordena valores min/max se necessário (chamado automaticamente)

### Membros Privados

- `m_currentValue`: Valor atual representado pela porção preenchida da barra
- `m_lastValue`: Último valor representado (usado para otimização de redesenho)
- `m_config`: Estrutura de configuração do VBar
- `m_changedScale`: Flag que indica quando a escala foi alterada (força redesenho completo)

---

## 💻 Como Usar no Arduino IDE

### 📚 Passo 1: Incluir as Bibliotecas

```cpp
#include <displayfk.h>
// Incluir outras bibliotecas necessárias
```

### 📝 Passo 2: Declarar Objetos Globais

```cpp
DisplayFK myDisplay;

// Criar barras
VBar barra1(50, 50, 0);
VBar barra2(200, 50, 0);

const uint8_t qtdVBar = 2;
VBar *arrayVBar[qtdVBar] = {&barra1, &barra2};
```

### ⚙️ Passo 3: Configurar Display (setup)

```cpp
void setup() {
    Serial.begin(115200);
    
    // ... configuração do display ...
    myDisplay.setDrawObject(tft);
    
    loadWidgets();
    myDisplay.loadScreen(screen0);
    myDisplay.createTask(false, 3);
}
```

### 🔧 Passo 4: Configurar as Barras

```cpp
void loadWidgets() {
    // Configurar Barra Vertical
    VerticalBarConfig configBarra1 = {
        .subtitle = nullptr,
        .minValue = 0,
        .maxValue = 100,
        .round = 5,
        .orientation = Orientation::VERTICAL,
        .width = 30,
        .height = 150,
        .filledColor = CFK_GREEN
    };
    barra1.setup(configBarra1);
    barra1.drawBackground();
    
    // Configurar Barra Horizontal
    VerticalBarConfig configBarra2 = {
        .subtitle = nullptr,
        .minValue = 0,
        .maxValue = 100,
        .round = 10,
        .orientation = Orientation::HORIZONTAL,
        .width = 150,
        .height = 30,
        .filledColor = CFK_BLUE
    };
    barra2.setup(configBarra2);
    barra2.drawBackground();
    
    // Registrar no DisplayFK
    myDisplay.setVBar(arrayVBar, qtdVBar);
}
```

### 🖥️ Passo 5: Função da Tela

```cpp
void screen0() {
    tft->fillScreen(CFK_WHITE);
    WidgetBase::backgroundColor = CFK_WHITE;
    
    myDisplay.drawWidgetsOnScreen(0);
}
```

### 🔄 Passo 6: Exemplo de Uso

```cpp
void loop() {
    // Simular valores
    int valor1 = random(0, 101);
    int valor2 = random(0, 101);
    
    // Atualizar barras
    barra1.setValue(valor1);
    barra2.setValue(valor2);
    
    delay(500);
}
```

---

## 📝 Exemplo Completo

```cpp
#include <displayfk.h>

DisplayFK myDisplay;
Arduino_DataBus *bus = nullptr;
Arduino_GFX *gfx = nullptr;

// Criar barras e labels
VBar bateria(50, 50, 0);
VBar progresso(50, 120, 0);
Label labelBateria(85, 100, 0);  // Label para exibir valor da bateria

const uint8_t qtdVBar = 2;
VBar *arrayVBar[qtdVBar] = {&bateria, &progresso};

const uint8_t qtdLabel = 1;
Label *arrayLabel[qtdLabel] = {&labelBateria};

void setup() {
    Serial.begin(115200);
    
    // Inicializar display
    bus = new Arduino_ESP32SPI(/* ... */);
    gfx = new Arduino_ST7789(/* ... */);
    gfx->begin();
    
    myDisplay.setDrawObject(gfx);
    
    // Configurar Label para exibir valor da bateria
    LabelConfig configLabelBateria = {
        .text = "0",
        .prefix = "",
        .suffix = "",
        .fontFamily = &RobotoRegular10pt7b,
        .datum = TL_DATUM,
        .fontColor = CFK_BLACK,
        .backgroundColor = CFK_WHITE
    };
    labelBateria.setup(configLabelBateria);
    
    // Configurar Barra de Bateria (Vertical) com Label
    VerticalBarConfig configBateria = {
        .subtitle = &labelBateria,
        .minValue = 0,
        .maxValue = 100,
        .round = 5,
        .orientation = Orientation::VERTICAL,
        .width = 30,
        .height = 100,
        .filledColor = CFK_GREEN
    };
    bateria.setup(configBateria);
    bateria.drawBackground();
    
    // Registrar widgets
    myDisplay.setLabel(arrayLabel, qtdLabel);
    
    // Configurar Barra de Progresso (Horizontal)
    VerticalBarConfig configProgresso = {
        .subtitle = nullptr,
        .minValue = 0,
        .maxValue = 100,
        .round = 10,
        .orientation = Orientation::HORIZONTAL,
        .width = 200,
        .height = 20,
        .filledColor = CFK_BLUE
    };
    progresso.setup(configProgresso);
    progresso.drawBackground();
    
    myDisplay.setVBar(arrayVBar, qtdVBar);
    myDisplay.loadScreen(minhaTela);
    myDisplay.createTask(false, 3);
}

// Nota: O Label será atualizado automaticamente quando setValue() for chamado

void loop() {
    // Simular nível de bateria
    static int nivel = 100;
    nivel = (nivel + random(-2, 2)) % 101;
    bateria.setValue(nivel);
    
    // Simular progresso
    static int progress = 0;
    progress = (progress + 1) % 101;
    progresso.setValue(progress);
    
    delay(100);
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
- **Vertical:** 20-40 pixels de largura, 100-200 pixels de altura
- **Horizontal:** 100-200 pixels de largura, 20-40 pixels de altura

### 🎨 Cores
- Use cores intuitivas (verde = OK, amarelo = atenção, vermelho = alerta)
- Verde para níveis normais
- Vermelho para níveis críticos
- Azul para progresso

### 🔄 Orientação
- **VERTICAL:** Para altura, nível, quantidade
- **HORIZONTAL:** Para progresso, carga, porcentagem

### ⚡ Performance
- Redesenho apenas quando valor muda
- Fundo desenhado uma vez apenas
- Atualização eficiente do preenchimento
- Otimização para valores em mudança

### 👥 Usabilidade
- Cores claras para diferentes estados
- Posições lógicas na interface
- Tamanho adequado para visualização
- Feedback visual imediato

### 🎨 Visual
- Cantos arredondados para aparência moderna
- Preenchimento proporcional ao valor
- Fundo cinza claro como padrão
- Borda preta para contorno

### 🔔 Valores
- Valores limitados automaticamente
- Preenchimento proporcional automático
- Suporta valores negativos
- Faixa configurável dinamicamente com `setMinValue()`, `setMaxValue()` ou `setScale()`
- Consulta de valores com `getMinValue()` e `getMaxValue()`
- Ordenação automática de valores min/max
- Integração com Label: se `subtitle` for configurado, o valor é exibido automaticamente no Label

---

## 🔗 Herança de WidgetBase

A classe `VBar` herda métodos de `WidgetBase`:

- `isEnabled()` / `setEnabled()`: Habilitar/desabilitar
- `isInitialized()`: Verificar configuração
- `m_visible`: Controla visibilidade
- `m_shouldRedraw`: Flag para redesenho

---

## 🔗 Integração com DisplayFK

O `VBar` integra-se com o sistema DisplayFK:

1. **Renderização:** Automática com `drawWidgetsOnScreen()`
2. **Atualização:** Valores atualizados com `setValue()`
3. **Gerenciamento:** Controlado pelo loop principal
4. **Performance:** Redesenho eficiente
5. **Valores:** Limitação automática respeitada

---

## 🎨 Detalhes de Renderização

O VBar renderiza em camadas:

1. **Fundo:**
   - Retângulo com cantos arredondados (cinza claro)
   - Borda preta ao redor

2. **Preenchimento (Vertical):**
   - Preenche de baixo para cima
   - Altura proporcional ao valor
   - Cor definida por filledColor

3. **Preenchimento (Horizontal):**
   - Preenche da esquerda para direita
   - Largura proporcional ao valor
   - Cor definida por filledColor

4. **Limpeza:**
   - Apaga área se valor diminuiu
   - Mantém preenchimento se valor aumentou
   - Quando `m_changedScale` é true, limpa toda a área antes de redesenhar

5. **Label (subtitle):**
   - Se configurado, atualiza automaticamente o Label com o valor atual
   - Usa `setTextInt()` para atualizar o texto do Label
   - Atualização ocorre a cada redesenho

6. **Otimizações:**
   - Usa `m_lastValue` para calcular apenas a diferença a ser redesenhada
   - Flag `m_changedScale` força redesenho completo quando a escala muda
   - Validações de estado antes de renderizar (TFT, visibilidade, tela atual, etc.)

---

## 🔧 Solução de Problemas

### Barra não aparece
- Verifique se chamou `setup()` após criar
- Confirme que `drawBackground()` foi chamado
- Verifique valores de width e height > 20
- Chame `myDisplay.setVBar()`

### Valor não muda visualmente
- Verifique se chamou `setValue()` com novo valor
- Confirme que valor está na faixa min/max
- Verifique logs para erros
- Teste com valores extremos

### Mudança de escala não funciona
- Use `setScale()` para alterar min/max simultaneamente
- Ou use `setMinValue()` e `setMaxValue()` separadamente
- Valores são ordenados automaticamente se min > max
- A barra é redesenhada completamente quando a escala muda

### Visual incorreto
- Verifique cores configuradas
- Confirme dimensões adequadas
- Verifique valor de round
- Teste com orientação diferente

### Problemas de proporção
- Verifique configuração de min/max
- Confirme que orientação está correta
- Teste com diferentes valores
- Verifique dimensões da barra

### Label não atualiza
- Verifique se `subtitle` foi configurado corretamente
- Confirme que o Label foi configurado antes do VBar
- Verifique se o Label está visível
- Teste com `subtitle->setTextInt()` manualmente

---

## 📚 Referências

- **Classe Base:** `WidgetBase` (src/widgets/widgetbase.h)
- **DisplayFK Principal:** `DisplayFK` (src/displayfk.h)
- **Cores Padrão:** Definidas em displayfk.h
- **Exemplos:** examples/Embed_ESP32S3/Display_Test/

