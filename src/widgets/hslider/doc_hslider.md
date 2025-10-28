# Documentação da Classe HSlider

## Visão Geral

A classe `HSlider` é um widget interativo que exibe um slider horizontal com um controle deslizante que pode ser movido pelo usuário para ajustar valores dentro de uma faixa especificada. Ele herda de `WidgetBase` e fornece uma interface gráfica completa para criar sliders em displays touchscreen.

### Características Principais

- 📊 Slider horizontal com trilha e controle deslizante
- 👆 Suporte completo a toque (touchscreen)
- 🎨 Barra de progresso visual
- 🎯 Detecção precisa de toque na área do slider
- 🔔 Função de callback para responder a mudanças de valor
- 🎨 Cores personalizáveis
- 📏 Faixa de valores configurável
- ⚪ Raio customizável do controle
- 🔄 Atualização suave do controle
- 👁️ Pode ser mostrado/ocultado dinamicamente
- 🔗 Integra-se automaticamente com o sistema DisplayFK

---

## 📋 Estruturas de Configuração

### HSliderConfig

Estrutura que contém todos os parâmetros de configuração do slider:

```cpp
struct HSliderConfig {
  uint16_t width;              // Largura da trilha do slider em pixels
  uint16_t pressedColor;       // Cor RGB565 do controle e barra de progresso
  uint16_t backgroundColor;    // Cor RGB565 de fundo do slider
  int minValue;                // Valor mínimo da faixa
  int maxValue;                // Valor máximo da faixa
  uint32_t radius;             // Raio do controle deslizante em pixels
  functionCB_t callback;       // Função callback para mudanças de valor
};
```

---

## 🔧 Métodos Públicos

### Construtor

```cpp
HSlider(uint16_t _x, uint16_t _y, uint8_t _screen)
```

Cria um novo slider horizontal na posição especificada. A posição (`_x`, `_y`) representa o canto superior esquerdo da trilha.

**Parâmetros:**
- `_x`: Coordenada X do início da trilha
- `_y`: Coordenada Y do topo da trilha
- `_screen`: Identificador da tela (0 = primeira tela)

**Nota:** Após criar o objeto, é obrigatório chamar `setup()` antes de usá-lo.

### Destrutor

```cpp
~HSlider()
```

Libera os recursos do slider automaticamente.

### setup()

```cpp
void setup(const HSliderConfig& config)
```

Configura o slider com os parâmetros especificados. **Este método deve ser chamado após a criação do objeto.**

**Parâmetros:**
- `config`: Estrutura `HSliderConfig` com as configurações

**Validações automáticas:**
- Largura deve ser maior que zero
- Raio deve ser maior que zero
- minValue deve ser menor que maxValue

### getValue()

```cpp
int getValue() const
```

Retorna o valor atual do slider dentro da faixa configurada.

**Retorna:**
- Valor atual (entre minValue e maxValue)

### setValue()

```cpp
void setValue(int newValue)
```

Define programaticamente o valor do slider e move o controle para a posição correspondente.

**Parâmetros:**
- `newValue`: Novo valor para o slider (será limitado à faixa configurada)

**Características:**
- Valor é automaticamente restringido entre minValue e maxValue
- Control move-se automaticamente para a posição correspondente
- Dispara callback se configurado
- Marca para redesenho

### drawBackground()

```cpp
void drawBackground()
```

Desenha o fundo do slider (trilha e borda). **Este método deve ser chamado uma vez após setup() para inicializar o slider.**

### show()

```cpp
void show()
```

Torna o slider visível na tela.

### hide()

```cpp
void hide()
```

Oculta o slider da tela.

---

## 🔒 Métodos Privados (Apenas para Referência)

Estes métodos são chamados internamente e não precisam ser invocados diretamente:

- `detectTouch()`: Detecta toque do usuário no slider
- `redraw()`: Redesenha o slider na tela
- `forceUpdate()`: Força uma atualização imediata
- `getCallbackFunc()`: Retorna a função callback
- `start()`: Calcula dimensões e inicializa
- `validateConfig()`: Valida configuração
- `updateDimensions()`: Atualiza dimensões do slider
- `updateValue()`: Atualiza valor baseado na posição

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

// Criar o slider: HSlider(inicio_x, inicio_y, tela)
HSlider slider(320, 75, 0);

// Criar array de ponteiros
const uint8_t qtdHSlider = 1;
HSlider *arrayHslider[qtdHSlider] = {&slider};
```

### 📝 Passo 3: Prototipar Funções de Callback

```cpp
void slider_cb();
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
    // Configurar slider horizontal
    HSliderConfig configHSlider = {
        .width = 155,
        .pressedColor = CFK_COLOR20,
        .backgroundColor = CFK_COLOR22,
        .minValue = 0,
        .maxValue = 100,
        .radius = 17,
        .callback = slider_cb
    };
    slider.setup(configHSlider);
    
    // Desenhar fundo uma vez
    slider.drawBackground();
    
    // Registrar no DisplayFK
    myDisplay.setHSlider(arrayHslider, qtdHSlider);
}
```

### 🔔 Passo 6: Criar Funções de Callback

```cpp
void slider_cb() {
    int valor = slider.getValue();
    Serial.print("Valor do slider: ");
    Serial.println(valor);
    
    // Faça algo quando o valor mudar
}
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

### 🔄 Passo 8: Alterar Valor Programaticamente (Opcional)

```cpp
void loop() {
    // Definir valor programaticamente
    slider.setValue(50);
    
    // Obter valor atual
    int valorAtual = slider.getValue();
    
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

// Criar slider horizontal
HSlider meuSlider(100, 100, 0);

const uint8_t qtdSlider = 1;
HSlider *arraySlider[qtdSlider] = {&meuSlider};

void setup() {
    Serial.begin(115200);
    
    // Inicializar display
    bus = new Arduino_ESP32SPI(/* ... */);
    gfx = new Arduino_ST7789(/* ... */);
    gfx->begin();
    
    myDisplay.setDrawObject(gfx);
    myDisplay.startTouchGT911(/* ... */);
    
    // Configurar slider
    HSliderConfig config = {
        .width = 200,
        .pressedColor = CFK_COLOR20,
        .backgroundColor = CFK_GREY7,
        .minValue = 0,
        .maxValue = 100,
        .radius = 15,
        .callback = slider_callback
    };
    meuSlider.setup(config);
    
    // Desenhar fundo
    meuSlider.drawBackground();
    
    myDisplay.setHSlider(arraySlider, qtdSlider);
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

void slider_callback() {
    Serial.print("Valor: ");
    Serial.println(meuSlider.getValue());
}
```

---

## 📏 Dicas e Boas Práticas

### 📐 Tamanhos Recomendados
- **Pequenos:** Largura 100-150 pixels, raio 10-12 pixels
- **Médios:** Largura 150-250 pixels, raio 15-20 pixels
- **Grandes:** Largura 250-350 pixels, raio 20-25 pixels

### 🎨 Cores
- Use cores contrastantes entre fundo e controle
- Cores vibrantes para pressedColor destacam a seleção
- Considere o tema claro/escuro da interface
- Barra de progresso usa a mesma cor do controle

### 🔔 Callbacks
- Callback é executado quando o valor muda
- Mantenha callbacks curtas e rápidas
- Não bloqueie a execução dentro do callback
- Use variáveis globais para armazenar dados

### ⚡ Performance
- Renderização eficiente com atualizações incrementais
- Redesenha apenas quando necessário
- Evite updates muito frequentes
- Use valores dentro da faixa configurada

### 👥 Usabilidade
- Use larguras adequadas para fácil manipulação
- Raio maior facilita toque mas ocupa mais espaço
- Espaçamento suficiente ao redor do slider
- Valores min/max devem fazer sentido para o contexto

### 🎨 Design Visual
- O slider desenha uma trilha horizontal arredondada
- Controle deslizante circular no topo da trilha
- Barra de progresso mostra parte preenchida
- Altura da trilha é calculada automaticamente (2x o raio)
- Controle tem círculo interno com cor de destaque

### 📊 Configuração de Valores
- minValue e maxValue definem a faixa de valores
- Valores podem ser negativos
- Faixa de valores ampla permite precisão maior
- Valores são mapeados proporcionalmente para posições

---

## 🔗 Herança de WidgetBase

A classe `HSlider` herda todos os métodos de `WidgetBase`:

- `isEnabled()` / `setEnabled()`: Habilitar/desabilitar widget
- `isInitialized()`: Verificar se foi configurado
- `m_visible`: Controla visibilidade interna
- `m_shouldRedraw`: Flag para redesenho automático

---

## 🔗 Integração com DisplayFK

O `HSlider` integra-se automaticamente com o sistema DisplayFK:

1. **Detecção de toque:** Automática quando usando `startTouchGT911()` ou similar
2. **Sincronização:** Estados sincronizados entre diferentes telas
3. **Gerenciamento:** Controlado pelo loop principal do DisplayFK
4. **Callbacks:** Executados de forma segura e não-bloqueante
5. **Performance:** Renderização eficiente com atualizações incrementais

---

## 🎨 Detalhes de Renderização

O `HSlider` é renderizado em camadas:

1. **Fundo** (drawBackground()):
   - Trilha com cantos arredondados e cor de fundo
   - Borda adaptada ao modo claro/escuro
   - Desenhado uma vez após setup()

2. **Barra de Progresso** (redraw()):
   - Parte preenchida da trilha
   - Mostra porcentagem do valor atual
   - Usa pressedColor

3. **Controle** (redraw()):
   - Círculo externo com cor de fundo padrão
   - Círculo interno com pressedColor
   - Posicionado conforme o valor atual

4. **Otimizações**:
   - Redesenha apenas posição do controle
   - Mantém trilha e fundo estáticos
   - Atualização suave sem cintilação

---

## 🔧 Solução de Problemas

### Slider não aparece na tela
- Verifique se chamou `setup()` após criar o objeto
- Confirme que `drawBackground()` foi chamado após setup()
- Verifique se está na tela correta
- Certifique-se de chamar `myDisplay.setHSlider()`
- Confirme que largura e raio são maiores que 0

### Slider não responde ao toque
- Verifique se o touch foi inicializado corretamente
- Confirme que as coordenadas do toque estão mapeadas
- Verifique se o widget está habilitado (`setEnabled(true)`)
- Toque deve estar na área da trilha
- Confirme que DETECT_ON_HANDLER não está definido (usar área total)

### Valor não atualiza corretamente
- Verifique se `setValue()` foi chamado corretamente
- Confirme que o valor está dentro da faixa min/max
- Verifique se o widget está habilitado
- Certifique-se de que `drawBackground()` foi chamado

### Callback não é executado
- Verifique se a função callback foi configurada
- Confirme que o nome da função está correto
- Certifique-se de não ter erros de compilação
- Verifique se o valor realmente mudou

### Problemas visuais
- Verifique se as cores contrastam adequadamente
- Confirme que o slider está dentro dos limites da tela
- Verifique se há espaço suficiente para o controle se mover
- Para trilhas muito curtas, aumente a largura

### Controle não se move suavemente
- Aumente a largura do slider para maior precisão
- Use faixa de valores maior para menor precisão por pixel
- Verifique debounce settings se necessário
- Considere usar valores não-inteiros internamente

---

## 📚 Referências

- **Classe Base:** `WidgetBase` (src/widgets/widgetbase.h)
- **DisplayFK Principal:** `DisplayFK` (src/displayfk.h)
- **Exemplos:** examples/Embed_ESP32S3/Display_Test/
- **Cores Padrão:** Definidas em displayfk.h (CFK_COLOR01-CFK_COLOR28, CFK_GREY*, CFK_WHITE, CFK_BLACK)

