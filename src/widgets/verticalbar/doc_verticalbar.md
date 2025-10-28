# Documentação da Classe VBar (VerticalBar)

## Visão Geral

A classe `VBar` é um widget que exibe valores numéricos como uma barra vertical ou horizontal preenchida proporcionalmente. Ele herda de `WidgetBase` e fornece funcionalidade para criar barras de progresso visuais com cantos arredondados e orientação configurável.

### Características Principais

- 📊 Barra de progresso vertical ou horizontal
- 🎨 Cor personalizável para preenchimento
- 📏 Cantos arredondados configuráveis
- 📈 Faixa de valores configurável (min/max)
- 🔄 Suporte a orientação vertical e horizontal
- 👁️ Pode ser mostrado/ocultado dinamicamente
- 🔗 Integra-se automaticamente com o sistema DisplayFK

---

## 📋 Estruturas de Configuração

### VerticalBarConfig

Estrutura que contém todos os parâmetros de configuração:

```cpp
struct VerticalBarConfig {
  uint16_t width;         // Largura da barra
  uint16_t height;        // Altura da barra
  uint16_t filledColor;   // Cor da porção preenchida
  int minValue;           // Valor mínimo da faixa
  int maxValue;           // Valor máximo da faixa
  int round;             // Raio para cantos arredondados
  Orientation orientation; // VERTICAL ou HORIZONTAL
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
void setValue(uint32_t newValue)
```

Define o valor atual da barra.

**Parâmetros:**
- `newValue`: Novo valor (será limitado entre minValue e maxValue)

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

## 🔒 Métodos Privados (Apenas para Referência)

Estes métodos são chamados internamente:

- `detectTouch()`: Não processa eventos de toque
- `redraw()`: Redesenha a barra
- `forceUpdate()`: Força atualização
- `getCallbackFunc()`: Retorna callback
- `cleanupMemory()`: Limpa memória
- `start()`: Inicializa configurações

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
        .width = 30,
        .height = 150,
        .filledColor = CFK_GREEN,
        .minValue = 0,
        .maxValue = 100,
        .round = 5,
        .orientation = Orientation::VERTICAL
    };
    barra1.setup(configBarra1);
    barra1.drawBackground();
    
    // Configurar Barra Horizontal
    VerticalBarConfig configBarra2 = {
        .width = 150,
        .height = 30,
        .filledColor = CFK_BLUE,
        .minValue = 0,
        .maxValue = 100,
        .round = 10,
        .orientation = Orientation::HORIZONTAL
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

// Criar barras
VBar bateria(50, 50, 0);
VBar progresso(50, 120, 0);

const uint8_t qtdVBar = 2;
VBar *arrayVBar[qtdVBar] = {&bateria, &progresso};

void setup() {
    Serial.begin(115200);
    
    // Inicializar display
    bus = new Arduino_ESP32SPI(/* ... */);
    gfx = new Arduino_ST7789(/* ... */);
    gfx->begin();
    
    myDisplay.setDrawObject(gfx);
    
    // Configurar Barra de Bateria (Vertical)
    VerticalBarConfig configBateria = {
        .width = 30,
        .height = 100,
        .filledColor = CFK_GREEN,
        .minValue = 0,
        .maxValue = 100,
        .round = 5,
        .orientation = Orientation::VERTICAL
    };
    bateria.setup(configBateria);
    bateria.drawBackground();
    
    // Configurar Barra de Progresso (Horizontal)
    VerticalBarConfig configProgresso = {
        .width = 200,
        .height = 20,
        .filledColor = CFK_BLUE,
        .minValue = 0,
        .maxValue = 100,
        .round = 10,
        .orientation = Orientation::HORIZONTAL
    };
    progresso.setup(configProgresso);
    progresso.drawBackground();
    
    myDisplay.setVBar(arrayVBar, qtdVBar);
    myDisplay.loadScreen(minhaTela);
    myDisplay.createTask(false, 3);
}

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
- Faixa configurável

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

---

## 📚 Referências

- **Classe Base:** `WidgetBase` (src/widgets/widgetbase.h)
- **DisplayFK Principal:** `DisplayFK` (src/displayfk.h)
- **Cores Padrão:** Definidas em displayfk.h
- **Exemplos:** examples/Embed_ESP32S3/Display_Test/

