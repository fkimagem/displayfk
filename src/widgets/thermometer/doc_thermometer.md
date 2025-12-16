# Documentação da Classe Thermometer

## Visão Geral

A classe `Thermometer` é um widget visual que exibe um termômetro vertical com bulbo na base e tubo de vidro central. Ele herda de `WidgetBase` e fornece funcionalidade completa para criar termômetros com valores variáveis, cores personalizáveis e integração com Label para exibição numérica.

### Características Principais

- 🌡️ Termômetro vertical com bulbo e tubo de vidro
- 🎨 Cores personalizáveis para preenchimento, fundo e marcas
- 📊 Faixa de valores configurável (min/max)
- 🔢 Integração com Label para exibição numérica
- 📏 Marcações graduadas no tubo
- ✨ Efeito de gradiente no bulbo para realismo
- 👁️ Pode ser mostrado/ocultado dinamicamente
- 🔗 Integra-se automaticamente com o sistema DisplayFK

---

## 📋 Estruturas de Configuração

### ThermometerConfig

Estrutura que contém todos os parâmetros de configuração:

```cpp
struct ThermometerConfig {
  uint16_t width;          // Largura do termômetro
  uint16_t height;         // Altura do termômetro
  uint16_t filledColor;    // Cor da porção preenchida
  uint16_t backgroundColor; // Cor de fundo
  uint16_t markColor;      // Cor das marcas graduadas
  int minValue;            // Valor mínimo da faixa
  int maxValue;            // Valor máximo da faixa
  Label* subtitle;         // Ponteiro para Label (opcional)
  const char* unit;        // Unidade de medida (ex: "°C", "°F")
  uint8_t decimalPlaces;  // Casas decimais para exibição
};
```

---

## 🔧 Métodos Públicos

### Construtor

```cpp
Thermometer(uint16_t _x, uint16_t _y, uint8_t _screen)
```

Cria um novo termômetro.

**Parâmetros:**
- `_x`: Coordenada X do canto superior esquerdo
- `_y`: Coordenada Y do canto superior esquerdo
- `_screen`: Identificador da tela (0 = primeira tela)

**Nota:** Após criar o objeto, chame `setup()` antes de usá-lo.

### Destrutor

```cpp
~Thermometer()
```

Libera os recursos do Thermometer.

### setup()

```cpp
void setup(const ThermometerConfig& config)
```

Configura o termômetro. **Este método deve ser chamado após a criação do objeto.**

**Parâmetros:**
- `config`: Estrutura `ThermometerConfig` com as configurações

**Características:**
- Valida se o widget já está carregado (evita reconfiguração)
- Calcula gradiente de cor para efeito de luz no bulbo
- Configura Label com unidade de medida se `subtitle` for fornecido
- Define valor inicial como `minValue`
- Valida dimensões (altura mínima de 20 pixels)
- Marca o widget como carregado e inicializado

### drawBackground()

```cpp
void drawBackground()
```

Desenha o fundo do termômetro (bulbo, tubo, marcas). **Este método deve ser chamado uma vez após setup().**

**Características:**
- Desenha retângulo de fundo com bordas arredondadas
- Calcula e desenha tubo de vidro central
- Desenha bulbo na base com gradiente de cor para efeito de luz
- Desenha marcações graduadas no lado do tubo (10 divisões)
- Marcações maiores a cada 5 divisões
- Calcula áreas de preenchimento e vidro
- Deve ser chamado antes de usar `setValue()`

### setValue()

```cpp
void setValue(float newValue)
```

Define o valor atual do termômetro.

**Parâmetros:**
- `newValue`: Novo valor (será limitado entre minValue e maxValue)

**Características:**
- Aceita valores do tipo `float` para precisão decimal
- Restringe valor usando `constrain()` dentro da faixa min/max
- Marca para redesenho automático
- Atualiza Label automaticamente se `subtitle` estiver configurado

### setMinValue()

```cpp
void setMinValue(int newValue)
```

Define o valor mínimo da faixa do termômetro.

**Parâmetros:**
- `newValue`: Novo valor mínimo

**Características:**
- Ordena automaticamente os valores se min > max
- Ajusta o valor atual para ficar dentro da nova faixa
- Marca para redesenho completo do termômetro
- Ativa flag interna `m_changedScale` que força limpeza completa antes de redesenhar

### setMaxValue()

```cpp
void setMaxValue(int newValue)
```

Define o valor máximo da faixa do termômetro.

**Parâmetros:**
- `newValue`: Novo valor máximo

**Características:**
- Ordena automaticamente os valores se min > max
- Ajusta o valor atual para ficar dentro da nova faixa
- Marca para redesenho completo do termômetro
- Ativa flag interna `m_changedScale` que força limpeza completa antes de redesenhar

### getMinValue()

```cpp
int getMinValue()
```

Retorna o valor mínimo atual da faixa do termômetro.

**Retorno:**
- Valor mínimo configurado

### getMaxValue()

```cpp
int getMaxValue()
```

Retorna o valor máximo atual da faixa do termômetro.

**Retorno:**
- Valor máximo configurado

### setScale()

```cpp
void setScale(int newMinValue, int newMaxValue)
```

Define simultaneamente os valores mínimo e máximo da faixa do termômetro.

**Parâmetros:**
- `newMinValue`: Novo valor mínimo
- `newMaxValue`: Novo valor máximo

**Características:**
- Ordena automaticamente os valores se min > max
- Ajusta o valor atual para ficar dentro da nova faixa
- Marca para redesenho completo do termômetro
- Ativa flag interna `m_changedScale` que força limpeza completa antes de redesenhar
- Útil para mudanças dinâmicas de escala

### show()

```cpp
void show()
```

Torna o termômetro visível.

### hide()

```cpp
void hide()
```

Oculta o termômetro.

---

## 🔒 Métodos Privados (Apenas para Referência)

Estes métodos são chamados internamente:

- `detectTouch()`: Não processa eventos de toque
- `redraw()`: Redesenha o termômetro com otimização de atualização incremental
- `forceUpdate()`: Força atualização
- `getCallbackFunc()`: Retorna callback
- `cleanupMemory()`: Limpa memória (Thermometer não usa alocação dinâmica)
- `start()`: Inicializa configurações e valida dimensões (altura mínima de 20 pixels)
- `sortValues()`: Ordena valores min/max se necessário (chamado automaticamente)
- `setup(uint16_t, uint16_t, uint16_t, int, int)`: Método interno de configuração (sobrecarga)

### Membros Privados

- `m_currentValue`: Valor atual representado (tipo `float` para precisão decimal)
- `m_lastValue`: Último valor representado (usado para otimização de redesenho)
- `m_bulb`: Estrutura `Circle_t` representando o bulbo do termômetro
- `m_fillArea`: Estrutura `Rect_t` representando a área preenchida do termômetro
- `m_glassArea`: Estrutura `Rect_t` representando a área de vidro do termômetro
- `m_colorLightGradient`: Array de 5 cores para efeito de gradiente no bulbo
- `m_border`: Tamanho da borda ao redor do termômetro (5 pixels)
- `m_config`: Estrutura contendo configuração completa do termômetro
- `m_shouldRedraw`: Flag indicando se o termômetro deve ser redesenhado
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

// Criar Label para exibir valor
Label temperaturaLabel(150, 200, 0);

// Criar Thermometer
Thermometer thermometer(50, 50, 0);

const uint8_t qtdThermometer = 1;
Thermometer *arrayThermometer[qtdThermometer] = {&thermometer};
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

### 🔧 Passo 4: Configurar o Thermometer

```cpp
void loadWidgets() {
    // Configurar Label
    LabelConfig configLabel = {
        .text = "0",
        .fontFamily = &RobotoRegular10pt7b,
        .datum = MC_DATUM,
        .fontColor = CFK_BLACK,
        .backgroundColor = CFK_WHITE,
        .prefix = "",
        .suffix = "°C"
    };
    temperaturaLabel.setup(configLabel);
    
    // Configurar Thermometer
    ThermometerConfig configThermometer = {
        .width = 60,
        .height = 150,
        .filledColor = CFK_RED,
        .backgroundColor = CFK_WHITE,
        .markColor = CFK_BLACK,
        .minValue = 0,
        .maxValue = 100,
        .subtitle = &temperaturaLabel,
        .unit = "°C",
        .decimalPlaces = 1
    };
    thermometer.setup(configThermometer);
    
    // Desenhar fundo
    thermometer.drawBackground();
    
    // Registrar no DisplayFK
    myDisplay.setThermometer(arrayThermometer, qtdThermometer);
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
    // Simular leitura de temperatura
    float temperatura = random(200, 350) / 10.0; // 20.0 a 35.0°C
    
    // Atualizar termômetro
    thermometer.setValue(temperatura);
    
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

// Criar Label para temperatura
Label tempLabel(120, 250, 0);

// Criar Thermometer
Thermometer termometro(50, 50, 0);

const uint8_t qtdThermometer = 1;
Thermometer *arrayThermometer[qtdThermometer] = {&termometro};

void setup() {
    Serial.begin(115200);
    
    // Inicializar display
    bus = new Arduino_ESP32SPI(/* ... */);
    gfx = new Arduino_ST7789(/* ... */);
    gfx->begin();
    
    myDisplay.setDrawObject(gfx);
    
    // Configurar Label
    LabelConfig configLabel = {
        .text = "0",
        .fontFamily = &RobotoRegular10pt7b,
        .datum = MC_DATUM,
        .fontColor = CFK_BLACK,
        .backgroundColor = CFK_WHITE,
        .prefix = "",
        .suffix = "°C"
    };
    tempLabel.setup(configLabel);
    
    // Configurar Thermometer
    ThermometerConfig config = {
        .width = 80,
        .height = 200,
        .filledColor = CFK_RED,
        .backgroundColor = CFK_WHITE,
        .markColor = CFK_BLACK,
        .minValue = 0,
        .maxValue = 50,
        .subtitle = &tempLabel,
        .unit = "°C",
        .decimalPlaces = 1
    };
    termometro.setup(config);
    termometro.drawBackground();
    
    myDisplay.setThermometer(arrayThermometer, qtdThermometer);
    myDisplay.loadScreen(minhaTela);
    myDisplay.createTask(false, 3);
}

void loop() {
    // Simular temperatura variando
    float temp = 25.0 + 10.0 * sin(millis() / 5000.0);
    termometro.setValue(temp);
    
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
- **Pequenos:** 40x100 pixels
- **Médios:** 60x150 pixels
- **Grandes:** 80x200 pixels

### 🎨 Cores
- **filledColor:** Vermelho para temperatura alta
- **backgroundColor:** Branco ou cinza claro
- **markColor:** Preto ou cinza escuro para contraste
- Considere o tema claro/escuro

### 🔢 Valores e Faixas
- Configure faixa adequada (minValue a maxValue)
- Use valores realistas para temperatura
- Faixa configurável dinamicamente com `setMinValue()`, `setMaxValue()` ou `setScale()`
- Consulta de valores com `getMinValue()` e `getMaxValue()`
- Ordenação automática de valores min/max
- Label opcional para exibição numérica
- Unidade de medida clara (°C, °F, etc.)
- Suporta valores decimais (tipo `float`)

### ⚡ Performance
- Redesenho apenas quando valor muda
- Fundo desenhado uma vez apenas (chame `drawBackground()` uma vez)
- Atualização eficiente do preenchimento (apenas diferença entre valores)
- Label atualizado automaticamente com `setTextFloat()`
- Gradiente de cor calculado uma vez durante setup()
- Mudanças de escala redesenham completamente o termômetro

### 👥 Usabilidade
- Posicione Label próximo ao termômetro
- Use cores intuitivas (vermelho = quente)
- Marcações graduadas para fácil leitura
- Tamanho adequado para visualização

### 🎨 Visual
- Bulbo com gradiente para realismo
- Tubo de vidro com bordas definidas
- Marcações graduadas no tubo
- Preenchimento proporcional ao valor

### 🔔 Integração
- Label opcional para valor numérico
- Unidade de medida configurável
- Casas decimais personalizáveis
- Atualização automática do Label

---

## 🔗 Herança de WidgetBase

A classe `Thermometer` herda métodos de `WidgetBase`:

- `isEnabled()` / `setEnabled()`: Habilitar/desabilitar
- `isInitialized()`: Verificar configuração
- `m_visible`: Controla visibilidade
- `m_shouldRedraw`: Flag para redesenho

---

## 🔗 Integração com DisplayFK

O `Thermometer` integra-se com o sistema DisplayFK:

1. **Renderização:** Automática com `drawWidgetsOnScreen()`
2. **Label:** Integração opcional para exibição numérica
3. **Gerenciamento:** Controlado pelo loop principal
4. **Performance:** Redesenho eficiente
5. **Valores:** Limitação automática respeitada

---

## 🎨 Detalhes de Renderização

O Thermometer renderiza em camadas:

1. **Fundo Principal:**
   - Retângulo arredondado com backgroundColor
   - Borda externa com cor de contraste

2. **Tubo de Vidro:**
   - Retângulo arredondado central
   - Borda definida
   - Fundo com backgroundColor

3. **Bulbo:**
   - Círculo na base do termômetro
   - Gradiente de cor com 5 camadas para efeito de luz realista
   - Cores calculadas usando `lighten565()` baseado em `filledColor`
   - Borda preta definida
   - Posicionado na base do termômetro

4. **Marcações:**
   - 10 divisões uniformes ao longo do tubo
   - Linhas menores a cada divisão
   - Linhas maiores (2x espessura) a cada 5 divisões
   - Cor definida por `markColor`
   - Posicionadas no lado esquerdo do tubo
   - Espaçamento calculado proporcionalmente à altura do tubo

5. **Preenchimento:**
   - Área proporcional ao valor
   - Cor definida por filledColor
   - Atualizada apenas quando valor muda
   - Limpeza eficiente quando valor diminui
   - Quando `m_changedScale` é true, limpa toda a área antes de redesenhar

6. **Label (subtitle):**
   - Se configurado, atualiza automaticamente com o valor atual
   - Usa `setTextFloat()` para atualizar o texto com casas decimais
   - Unidade de medida configurada via `unit` e aplicada como sufixo
   - Atualização ocorre a cada redesenho

7. **Otimizações:**
   - Usa `m_lastValue` para calcular apenas a diferença a ser redesenhada
   - Flag `m_changedScale` força redesenho completo quando a escala muda
   - Validações de estado antes de renderizar (TFT, visibilidade, tela atual, etc.)
   - Gradiente de cor calculado uma vez durante setup()

---

## 🔧 Solução de Problemas

### Thermometer não aparece
- Verifique se chamou `setup()` após criar
- Confirme que `drawBackground()` foi chamado
- Verifique valores de width e height > 0
- Chame `myDisplay.setThermometer()`

### Valor não muda visualmente
- Verifique se chamou `setValue()` com novo valor
- Confirme que valor está na faixa min/max
- Verifique logs para erros
- Teste com valores extremos

### Label não atualiza
- Verifique se subtitle foi configurado
- Confirme que Label foi configurado corretamente
- Verifique se unit foi definido
- Teste com decimalPlaces adequado

### Visual incorreto
- Verifique cores configuradas
- Confirme dimensões adequadas
- Verifique se drawBackground() foi chamado
- Teste com cores mais contrastantes

### Problemas de escala
- Verifique configuração de minValue/maxValue
- Confirme que valores estão na faixa
- Teste com diferentes faixas
- Verifique proporção width/height
- Use `setScale()` para alterar min/max simultaneamente
- Valores são ordenados automaticamente se min > max
- O termômetro é redesenhado completamente quando a escala muda

### Mudança de escala não funciona
- Use `setScale()` para alterar min/max simultaneamente
- Ou use `setMinValue()` e `setMaxValue()` separadamente
- Valores são ordenados automaticamente se min > max
- O termômetro é redesenhado completamente quando a escala muda

---

## 📚 Referências

- **Classe Base:** `WidgetBase` (src/widgets/widgetbase.h)
- **Label:** `Label` (src/widgets/label/wlabel.h)
- **DisplayFK Principal:** `DisplayFK` (src/displayfk.h)
- **Cores Padrão:** Definidas em displayfk.h
- **Exemplos:** examples/Embed_ESP32S3/Display_Test/

