# Documentação da Classe SpinBox

## Visão Geral

A classe `SpinBox` é um widget interativo que exibe uma caixa de spin com botões de incremento e decremento para entrada numérica. Ele herda de `WidgetBase` e fornece funcionalidade completa para criar controles numéricos com limites e passos configuráveis.

### Características Principais

- 🔢 Controle numérico com incremento/decremento
- ➕➖ Botões de mais e menos integrados
- 📊 Faixa de valores configurável (min/max)
- 🔢 Passo de incremento personalizável
- 🎨 Cores personalizáveis para fundo e texto
- 🔔 Callback para mudanças de valor
- 👆 Suporte a toque nos botões
- 📐 Dimensões configuráveis
- 👁️ Pode ser mostrado/ocultado dinamicamente
- 🔗 Integra-se automaticamente com o sistema DisplayFK

---

## 📋 Estruturas de Configuração

### SpinBoxConfig

Estrutura que contém todos os parâmetros de configuração  
(reorganizada para melhor alinhamento em 32 bits):

```cpp
struct SpinBoxConfig {
  functionCB_t callback;  // Função callback chamada quando o valor muda.
  int minValue;           // Valor mínimo da faixa.
  int maxValue;           // Valor máximo da faixa.
  int startValue;         // Valor inicial exibido.
  uint16_t width;         // Largura da caixa de spin.
  uint16_t height;        // Altura da caixa de spin.
  uint16_t step;          // Passo para incremento/decremento.
  uint16_t color;         // Cor de fundo e bordas.
  uint16_t textColor;     // Cor do texto.
};
```

---

## 🔧 Métodos Públicos

### Construtor

```cpp
SpinBox(uint16_t _x, uint16_t _y, uint8_t _screen)
```

Cria um novo SpinBox.

**Parâmetros:**
- `_x`: Coordenada X do canto superior esquerdo
- `_y`: Coordenada Y do canto superior esquerdo
- `_screen`: Identificador da tela (0 = primeira tela)

**Nota:** Após criar o objeto, chame `setup()` antes de usá-lo.

### Destrutor

```cpp
~SpinBox()
```

Libera os recursos do SpinBox.

### setup()

```cpp
void setup(const SpinBoxConfig& config)
```

Configura o SpinBox. **Este método deve ser chamado após a criação do objeto.**

**Parâmetros:**
- `config`: Estrutura `SpinBoxConfig` com as configurações

### drawBackground()

```cpp
void drawBackground()
```

Desenha o fundo do SpinBox (botões e estrutura). **Este método deve ser chamado uma vez após setup().**

### getValue()

```cpp
int getValue()
```

Retorna o valor atual do SpinBox.

### setValue()

```cpp
void setValue(int _value)
```

Define o valor do SpinBox programaticamente.

**Parâmetros:**
- `_value`: Novo valor (será limitado entre minValue e maxValue)

### show()

```cpp
void show()
```

Torna o SpinBox visível.

### hide()

```cpp
void hide()
```

Oculta o SpinBox.

---

## 🔒 Métodos Privados (Apenas para Referência)

Estes métodos são chamados internamente:

- `detectTouch()`: Detecta toque nos botões
- `redraw()`: Redesenha o valor atual
- `forceUpdate()`: Força atualização
- `getCallbackFunc()`: Retorna callback
- `cleanupMemory()`: Limpa memória
- `increaseValue()`: Incrementa valor
- `decreaseValue()`: Decrementa valor

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

// Criar SpinBox
SpinBox spinbox(110, 240, 0);

const uint8_t qtdSpinbox = 1;
SpinBox *arraySpinbox[qtdSpinbox] = {&spinbox};
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

### 🔧 Passo 4: Configurar o SpinBox

```cpp
void loadWidgets() {
    // Configurar SpinBox
    SpinBoxConfig configSpinBox = {
        .callback  = spinbox_cb,
        .minValue  = 0,
        .maxValue  = 100,
        .startValue = 50,
        .width     = 101,
        .height    = 25,
        .step      = 1,
        .color     = CFK_COLOR01,
        .textColor = CFK_WHITE
    };
    spinbox.setup(configSpinBox);
    
    // Desenhar fundo
    spinbox.drawBackground();
    
    // Registrar no DisplayFK
    myDisplay.setSpinbox(arraySpinbox, qtdSpinbox);
}
```

### 🔔 Passo 5: Criar Função de Callback

```cpp
void spinbox_cb() {
    int valor = spinbox.getValue();
    Serial.print("Novo valor: ");
    Serial.println(valor);
}
```

### 🖥️ Passo 6: Função da Tela

```cpp
void screen0() {
    tft->fillScreen(CFK_WHITE);
    WidgetBase::backgroundColor = CFK_WHITE;
    
    myDisplay.drawWidgetsOnScreen(0);
}
```

### 🔄 Passo 7: Exemplo de Uso

```cpp
void loop() {
    // Ler valor atual
    int valorAtual = spinbox.getValue();
    
    // Alterar valor programaticamente
    spinbox.setValue(75);
    
    delay(1000);
}
```

---

## 📝 Exemplo Completo

```cpp
#include <displayfk.h>

DisplayFK myDisplay;
Arduino_DataBus *bus = nullptr;
Arduino_GFX *gfx = nullptr;

// Criar SpinBox
SpinBox controleVolume(100, 100, 0);

const uint8_t qtdSpinbox = 1;
SpinBox *arraySpinbox[qtdSpinbox] = {&controleVolume};

void setup() {
    Serial.begin(115200);
    
    // Inicializar display
    bus = new Arduino_ESP32SPI(/* ... */);
    gfx = new Arduino_ST7789(/* ... */);
    gfx->begin();
    
    myDisplay.setDrawObject(gfx);
    
    // Configurar SpinBox
    SpinBoxConfig config = {
        .width = 150,
        .height = 40,
        .step = 5,
        .minValue = 0,
        .maxValue = 100,
        .startValue = 50,
        .color = CFK_BLUE,
        .textColor = CFK_WHITE,
        .callback = volume_callback
    };
    controleVolume.setup(config);
    controleVolume.drawBackground();
    
    myDisplay.setSpinbox(arraySpinbox, qtdSpinbox);
    myDisplay.loadScreen(minhaTela);
    myDisplay.createTask(false, 3);
}

void loop() {
    // Seu código aqui
}

void minhaTela() {
    gfx->fillScreen(CFK_WHITE);
    WidgetBase::backgroundColor = CFK_WHITE;
    myDisplay.drawWidgetsOnScreen(0);
}

void volume_callback() {
    Serial.print("Volume: ");
    Serial.println(controleVolume.getValue());
}
```

---

## 📏 Dicas e Boas Práticas

### 📐 Tamanhos Recomendados
- **Pequenos:** 80x25 pixels
- **Médios:** 120x35 pixels
- **Grandes:** 180x45 pixels

### 🎨 Cores
- Use cores contrastantes para fundo e texto
- Fundo escuro com texto claro é comum
- Considere o tema claro/escuro
- Botões têm cor mais clara automaticamente

### 🔢 Valores e Passos
- Configure faixa adequada (minValue a maxValue)
- Use passos apropriados para o contexto
- Valor inicial deve estar na faixa
- Valores são automaticamente limitados

### ⚡ Performance
- Redesenho apenas quando valor muda
- Detecção de toque eficiente
- Callback executado apenas na mudança
- Fundo desenhado uma vez apenas

### 👥 Usabilidade
- Botões grandes o suficiente para toque
- Valor exibido claramente no centro
- Sinais + e - bem visíveis
- Feedback visual ao tocar

### 🎨 Visual
- Botões com bordas arredondadas
- Sinais + e - desenhados geometricamente
- Área central para exibição do valor
- Fonte otimizada para o espaço disponível

### 🔔 Funcionalidade
- Incremento/decremento por passo
- Limites automáticos respeitados
- Callback executa na mudança
- Estado persistente entre ciclos

---

## 🔗 Herança de WidgetBase

A classe `SpinBox` herda métodos de `WidgetBase`:

- `isEnabled()` / `setEnabled()`: Habilitar/desabilitar
- `isInitialized()`: Verificar configuração
- `m_visible`: Controla visibilidade
- `m_shouldRedraw`: Flag para redesenho

---

## 🔗 Integração com DisplayFK

O `SpinBox` integra-se com o sistema DisplayFK:

1. **Renderização:** Automática com `drawWidgetsOnScreen()`
2. **Callbacks:** Executados quando valor muda
3. **Gerenciamento:** Controlado pelo loop principal
4. **Performance:** Redesenho eficiente
5. **Valores:** Limitação automática respeitada

---

## 🎨 Detalhes de Renderização

O SpinBox renderiza em camadas:

1. **Fundo Principal:**
   - Retângulo arredondado com cor de fundo
   - Borda externa com cor de contraste

2. **Botões Laterais:**
   - Botão esquerdo (decremento) com sinal "-"
   - Botão direito (incremento) com sinal "+"
   - Cores mais claras que o fundo principal
   - Bordas arredondadas

3. **Área Central:**
   - Retângulo arredondado para exibir valor
   - Texto centralizado com fonte otimizada
   - Cor de fundo igual ao fundo principal
   - Atualizada apenas quando valor muda

4. **Sinais Visuais:**
   - Sinal "-" no botão esquerdo
   - Sinal "+" no botão direito
   - Desenhados geometricamente
   - Cor do texto configurada

---

## 🔧 Solução de Problemas

### SpinBox não aparece
- Verifique se chamou `setup()` após criar
- Confirme que `drawBackground()` foi chamado
- Verifique valores de width e height > 0
- Chame `myDisplay.setSpinbox()`

### Botões não respondem
- Verifique se o usuário tocou na área correta
- Confirme que o SpinBox está visível
- Verifique logs para erros
- Teste com SpinBox maior

### Valor não muda
- Verifique se callback foi configurado
- Confirme que toque foi detectado
- Verifique logs para erros
- Teste com setValue() programático

### Valor sai da faixa
- Verifique configuração de minValue/maxValue
- Confirme que step está correto
- Valores são automaticamente limitados
- Teste com valores extremos

### Visual incorreto
- Verifique cores configuradas
- Confirme que drawBackground() foi chamado
- Verifique dimensões adequadas
- Teste com cores mais contrastantes

---

## 📚 Referências

- **Classe Base:** `WidgetBase` (src/widgets/widgetbase.h)
- **DisplayFK Principal:** `DisplayFK` (src/displayfk.h)
- **Cores Padrão:** Definidas em displayfk.h
- **Exemplos:** examples/Embed_ESP32S3/Display_Test/

