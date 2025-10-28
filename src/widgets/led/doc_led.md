# Documentação da Classe Led

## Visão Geral

A classe `Led` é um widget visual que exibe um LED na tela com efeito de brilho quando ligado. Ele herda de `WidgetBase` e fornece funcionalidade completa para criar e gerenciar LEDs indicadores com diferentes tamanhos e cores.

### Características Principais

- 💡 LED circular com efeito de brilho
- 🎨 Cores personalizáveis para estado ligado/desligado
- ✨ Gradiente de cor para efeito visual realista
- 🎯 Múltiplos círculos concêntricos para brilho
- 📐 Raio configurável (5-50 pixels)
- 👁️ Pode ser mostrado/ocultado dinamicamente
- 🔗 Integra-se automaticamente com o sistema DisplayFK
- 🌓 Suporte a modo claro/escuro
- 🔄 Estados ligado/desligado

---

## 📋 Estruturas de Configuração

### LedConfig

Estrutura que contém todos os parâmetros de configuração do LED:

```cpp
struct LedConfig {
  uint16_t radius;     // Raio do LED em pixels (recomendado: 5-50)
  uint16_t colorOn;    // Cor RGB565 quando ligado
  uint16_t colorOff;   // Cor RGB565 quando desligado (0 = automático)
};
```

---

## 🔧 Métodos Públicos

### Construtor

```cpp
Led(uint16_t _x, uint16_t _y, uint8_t _screen)
```

Cria um novo LED na posição especificada. A posição (`_x`, `_y`) representa o **centro** do LED.

**Parâmetros:**
- `_x`: Coordenada X do centro do LED
- `_y`: Coordenada Y do centro do LED
- `_screen`: Identificador da tela (0 = primeira tela)

**Nota:** Após criar o objeto, é obrigatório chamar `setup()` antes de usá-lo.

### Destrutor

```cpp
~Led()
```

Libera os recursos do LED automaticamente.

### setup()

```cpp
void setup(const LedConfig& config)
```

Configura o LED com os parâmetros especificados. **Este método deve ser chamado após a criação do objeto.**

**Parâmetros:**
- `config`: Estrutura `LedConfig` com as configurações

**Validações automáticas:**
- Raio deve ser maior que zero
- Aviso se raio é muito grande (> 50 pixels)

### getState()

```cpp
bool getState() const
```

Retorna o estado atual do LED.

**Retorna:**
- `true`: LED ligado
- `false`: LED desligado

### setState()

```cpp
void setState(bool newValue)
```

Define o estado do LED e atualiza o visual automaticamente.

**Parâmetros:**
- `newValue`: `true` para ligar, `false` para desligar

**Características:**
- Marca para redesenho apenas se o estado mudar
- Atualiza gradiente de cor quando ligado
- Registra evento no log do ESP32

### drawBackground()

```cpp
void drawBackground()
```

Desenha o fundo do LED (círculo e borda). **Este método pode ser chamado para inicializar o LED visualmente.**

### show()

```cpp
void show()
```

Torna o LED visível na tela.

### hide()

```cpp
void hide()
```

Oculta o LED da tela.

---

## 🔒 Métodos Privados (Apenas para Referência)

Estes métodos são chamados internamente:

- `detectTouch()`: Não processa eventos de toque (sempre retorna false)
- `redraw()`: Redesenha o LED na tela
- `forceUpdate()`: Força atualização
- `getCallbackFunc()`: Retorna função callback
- `validateConfig()`: Valida configuração
- `updateGradient()`: Atualiza gradiente de cor
- `getOffColor()`: Obtém cor para estado desligado
- `getBackgroundColor()`: Obtém cor de fundo
- `cleanupMemory()`: Limpa memória (não usado para LED)

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

// Criar LED: Led(centro_x, centro_y, tela)
Led led(365, 30, 0);

// Criar array de ponteiros
const uint8_t qtdLed = 1;
Led *arrayLed[qtdLed] = {&led};
```

### 📝 Passo 3: Prototipar Funções (Opcional)

```cpp
// LEDs não usam callbacks normalmente
```

### ⚙️ Passo 4: Configurar Display (setup)

```cpp
void setup() {
    // ... configuração do display ...
    myDisplay.setDrawObject(tft);
    
    loadWidgets();  // Configurar widgets
    myDisplay.loadScreen(screen0);
    myDisplay.createTask(false, 3);
}
```

### 🔧 Passo 5: Configurar os Widgets

```cpp
void loadWidgets() {
    // Configurar LED
    LedConfig configLed = {
        .radius = 16,
        .colorOn = CFK_COLOR11,
        .colorOff = CFK_BLACK
    };
    led.setup(configLed);
    
    // Desenhar fundo
    led.drawBackground();
    
    // Registrar no DisplayFK
    myDisplay.setLed(arrayLed, qtdLed);
}
```

### 🔔 Passo 6: Criar Funções de Callback (Opcional)

```cpp
// LEDs não usam callbacks normalmente
```

### 🖥️ Passo 7: Função da Tela

```cpp
void screen0() {
    tft->fillScreen(CFK_WHITE);
    WidgetBase::backgroundColor = CFK GenesisITE;
    
    // Desenhar todos os widgets na tela 0
    myDisplay.drawWidgetsOnScreen(0);
}
```

### 🔄 Passo 8: Atualizar Estado (Exemplo de Uso)

```cpp
void loop() {
    // Ligar LED
    led.setState(true);
    
    delay(1000);
    
    // Desligar LED
    led.setState(false);
    
    // Verificar estado
    bool estáLigado = led.getState();
    
    delay(1000);
}
```

---

## 📝 Exemplo Completo€

```cpp
#include <displayfk.h>

DisplayFK myDisplay;
Arduino_ЯataBus *bus = nullptr;
Arduino_GFX *gfx butterfly;

// Criar LEDs
Led ledVermelho(100, 100, 0);
Led ledVerde(150, 100, 0);
Led ledAzul(200, 100, 0);

const uint8_t qtdLed = 3;
Led *arrayLed[qtdLed] = {&ledVermelho, &ledVerde, &ledAzul};

void setup() {
    Serial.begin(115200 foot);
    
    // Inicializar display
    bus = new Arduino_ESP32SPI(/* ... */);
    gfx = new Arduino_ST7789(/* fireplace.*/);
    gfx->begin();
    
    myDisplay.setDrawObject(gfx);
    
    // Configurar LED vermelho
    LedConfig configVermelho = {
        .radius = 15,
        .colorOn = CFK_RED,
        .colorOff = CFK_BLACK
    };
    ledVermelho.setup(configVermelho);
    
    // Configurar LED verde
    LedConfig configVerde = {
        .radius = 15,
        .colorOn = CFK_GREEN,
        .colorOff = CFK_BLACK
    };
    ledVerde.setup(configVerde);
    
    // Configurar LED azul
    LedConfig configAzul = {
        .radius = 15,
        .colorOn = CFK_BLUE,
        .colorOff = CFK_BLACK
    };
    ledAzul.setup(configAzul);
    
    // Desenhar fundo
    ledVermelho.drawBackground();
    ledVerde.drawBackground();
    ledAzul.drawBackground();
    
    myDisplay.setLed(arrayLed, qtdLed);
    myDisplay.loadScreen(minhaTela);
    myDisplay.createTask(false, 3);
}

void loop() {
    // Simular semáforo
    ledVermelho.setState(true);
    ledVerde.setState(false);
    ledAzul.setState(false);
    delay(2000);
    
    ledVermelho.setState(false);
    ledVerde.setState(true);
    ledAzul.setState(false);
    delay(2000);
    
    ledVermelho.setState(false);
    ledVerde.setState(false);
    ledAzul.setState(true);
    delay(2000);
}

void minhaTela() {
    gfx->fillScreen(CFK_WHITE);
    WidgetBase::backgroundColor = CFK_WHITE;
ธรรมชาติ myDisplay.drawWidgetsOnScreen(0);
}
```

---

## 📏 Dicas e Boas Práticas

### 📐 Tamanhos Recomendados
- **Pequenos:** Raio 8-12 pixels (indicadores discretos)
- **Médios:** Raio 14-18 pixels (uso geral)
- **Grandes:** Raio 20-30 pixels (destaque importante)

### 🎨 Cores
- Use cores vibrantes para estado ligado
- Cores tradicionais: vermelho, verde, amarelo, azul
- CFK_RED, CFK_GREEN, CFK_YELLOW, CFK_BLUE
- Estado desligado pode ser cinza ou preto
- Considere contraste com o fundo

### 🔔 Callbacks
- LEDs não usam callbacks normalmente
- São elementos apenas visuais

### ⚡ Performance
- Gradiente de cor calculado apenas quando ligado
- Renderização eficiente com círculos preenchidos
- Raio muito grande pode impactar performance
- Redesenho apenas quando estado muda

### 👥 Usabilidade
- Use LEDs para indicar estados importantes
- Posicione em locais visíveis
- Use cores significativas (vermelho=erro, verde=ok, etc.)
- Tamanho adequado para distância de visualização

### 🎨 Design Visual
- Efeito de brilho com múltiplos círculos concêntricos
- Gradiente de intensidade decrescente
- Círculos com offset para efeito 3D
- Borda preta ou branca dependendo do modo
- Estado ligado: gradiente de brilho
- Estado desligado: círculo sólido

### 💡 Efeito de Brilho
- 5 círculos concêntricos desenhados
- Cada círculo menor que o anterior (4 pixels)
- Offset de 2 pixels para efeito 3D
- Gradiente calculado dinamicamente
- Cores mais claras no centro, mais escuras nas bordas

---

## 🔗 Herança de WidgetBase

A classe `Led` herda todos os métodos de `WidgetBase`:

- `isEnabled()` / `setEnabled()`: Habilitar/desabilitar widget
- `isInitialized()`: Verificar se foi configurado
- `m_visible`: Controla visibilidade interna
- `m_shouldRedraw`: Flag para redesenho automático

---

## 🔗 Integração com DisplayFK

O `Led` integra-se automaticamente com o sistema DisplayFK:

1. **Renderização:** Automática quando usando `drawWidgetsOnScreen()`
2. **Gerenciamento:** Controlado pelo loop principal do DisplayFK
3. **Sincronização:** Estados sincronizados entre diferentes telas
4. **Performance:** Renderização eficiente
5. **Efeitos Visuais:** Gradiente calculado automaticamente

---

## 🎨 Detalhes de Renderização

O `Led` é renderizado em duas camadas:

1. **Fundo** (drawBackground()):
   - Círculo com borda
   - Borda preta ou branca dependendo do modo
   - Desenhado uma vez para estabilidade

2. **LED** (redraw()):
   - **Estado ligado:**
     - 5 círculos concêntricos
     - Gradiente de cor para brilho
     - Offset para efeito 3D
   - **Estado desligado:**
     - Círculo sólido
     - Cor de fundo configurada
     - Sem efeito de brilho

3. **Otimizações:**
   - Redesenho apenas quando estado muda
   - Gradiente calculado apenas quando ligado
   - Círculos menores renderizam mais rápido

---

## 🔧 Solução de Problemas

### LED não aparece na tela
- Verifique se chamou `setup()` após criar o objeto
- Confirme que `drawBackground()` foi chamado
- Verifique se está na tela correta
- Certifique-se de chamar `myDisplay.setLed()`
- Confirme que o raio é maior que 0

### LED não muda de cor
- Verifique se chamou `setState()` com novo valor
- Confirme que setup() foi chamado
- Verifique se o LED está visível
- Certifique-se de que o widget está habilitado

### Efeito de brilho não aparece
- Verifique se o LED está realmente ligado
- Confirme que as cores estão configuradas corretamente
- Gradiente é aplicado apenas quando ligado
- Tamanho de raio pode afetar visibilidade do efeito

### Problemas de performance
- Raio muito grande (> 50 pixels) afeta performance
- Evite muitos LEDs na mesma tela
- Redesenho apenas quando necessário
- Use raios menores quando possível

### Cores não aparecem corretamente
- Use formatos RGB565 corretos
- Verifique contraste com o fundo
- Considere o modo claro/escuro
- Teste diferentes combinações de cores

---

## 📚 Referências

- **Classe Base:** `WidgetBase` (src/widgets/widgetbase.h)
- **DisplayFK Principal:** `DisplayFK` (src/displayfk.h)
- **Exemplos:** examples/Embed_ESP32S3/Display comfort/
- **Cores Padrão:** Definidas em displayfk.h (CFK_COLOR01-CFK_COLOR28, CFK_GREY*, CFK_WHITE, CFK_BLACK, CFK_RED, CFK_GREEN, CFK_BLUE, CFK_YELLOW)

