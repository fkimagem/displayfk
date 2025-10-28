# Documentação da Classe VAnalog (VerticalAnalog)

## Visão Geral

A classe `VAnalog` é um widget que exibe valores numéricos em uma escala analógica vertical com seta indicadora. Ele herda de `WidgetBase` e fornece funcionalidade para criar displays analógicos verticais com marcações graduadas, seta que se move conforme o valor e área de texto para exibição numérica.

### Características Principais

- 📊 Escala analógica vertical com seta indicadora
- 🎨 Cores personalizáveis para seta, fundo, borda e texto
- 📏 Marcações graduadas configuráveis
- 🔢 Exibição opcional de valor numérico
- 📈 Faixa de valores configurável (min/max)
- 🎯 Áreas de desenho otimizadas (seta, escala, texto)
- 👁️ Pode ser mostrado/ocultado dinamicamente
- 🔗 Integra-se automaticamente com o sistema DisplayFK

---

## 📋 Estruturas de Configuração

### VerticalAnalogConfig

Estrutura que contém todos os parâmetros de configuração:

```cpp
struct VerticalAnalogConfig {
  uint16_t width;           // Largura do display
  uint16_t height;          // Altura do display
  int minValue;            // Valor mínimo da escala
  int maxValue;            // Valor máximo da escala
  uint8_t steps;           // Número de divisões na escala
  uint16_t arrowColor;     // Cor da seta indicadora
  uint16_t textColor;      // Cor do texto (valores)
  uint16_t backgroundColor; // Cor de fundo
  uint16_t borderColor;    // Cor da borda
};
```

---

## 🔧 Métodos Públicos

### Construtor

```cpp
VAnalog(uint16_t _x, uint16_t _y, uint8_t _screen)
```

Cria um novo display analógico vertical.

**Parâmetros:**
- `_x`: Coordenada X do canto superior esquerdo
- `_y`: Coordenada Y do canto superior esquerdo
- `_screen`: Identificador da tela (0 = primeira tela)

**Nota:** Após criar o objeto, chame `setup()` antes de usá-lo.

### Destrutor

```cpp
~VAnalog()
```

Libera os recursos do VAnalog.

### setup()

```cpp
void setup(const VerticalAnalogConfig& config)
```

Configura o display analógico. **Este método deve ser chamado após a criação do objeto.**

**Parâmetros:**
- `config`: Estrutura `VerticalAnalogConfig` com as configurações

### drawBackground()

```cpp
void drawBackground()
```

Desenha o fundo do display (escala e marcações). **Este método deve ser chamado uma vez após setup().**

### setValue()

```cpp
void setValue(int newValue, bool _viewValue)
```

Define o valor atual e se deve exibir o texto numérico.

**Parâmetros:**
- `newValue`: Novo valor (será limitado entre minValue e maxValue)
- `_viewValue`: true para exibir texto numérico, false caso contrário

### show()

```cpp
void show()
```

Torna o display visível.

### hide()

```cpp
void hide()
```

Oculta o display.

---

## 🔒 Métodos Privados (Apenas para Referência)

Estes métodos são chamados internamente:

- `detectTouch()`: Não processa eventos de toque
- `redraw()`: Redesenha o display usando métodos especializados
- `forceUpdate()`: Força atualização
- `getCallbackFunc()`: Retorna callback
- `cleanupMemory()`: Limpa memória
- `start()`: Inicializa configurações e calcula áreas
- `calculateArrowVerticalPosition()`: Calcula posição da seta
- `drawArrow()`: Desenha a seta indicadora
- `clearArrow()`: Limpa a seta anterior
- `drawText()`: Desenha o texto numérico

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

// Criar displays analógicos
VAnalog display1(50, 50, 0);
VAnalog display2(120, 50, 0);

const uint8_t qtdVAnalog = 2;
VAnalog *arrayVAnalog[qtdVAnalog] = {&display1, &display2};
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

### 🔧 Passo 4: Configurar os Displays Analógicos

```cpp
void loadWidgets() {
    // Configurar Display 1
    VerticalAnalogConfig configDisplay1 = {
        .width = 60,
        .height = 150,
        .minValue = 0,
        .maxValue = 100,
        .steps = 20,
        .arrowColor = CFK_RED,
        .textColor = CFK_BLACK,
        .backgroundColor = CFK_WHITE,
        .borderColor = CFK_BLACK
    };
    display1.setup(configDisplay1);
    display1.drawBackground();
    
    // Configurar Display 2
    VerticalAnalogConfig configDisplay2 = {
        .width = 60,
        .height = 120,
        .minValue = -50,
        .maxValue = 50,
        .steps = 10,
        .arrowColor = CFK_BLUE,
        .textColor = CFK_WHITE,
        .backgroundColor = CFK_GREY11,
        .borderColor = CFK_GREY5
    };
    display2.setup(configDisplay2);
    display2.drawBackground();
    
    // Registrar no DisplayFK
    myDisplay.setVAnalog(arrayVAnalog, qtdVAnalog);
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
    int valor2 = random(-50, 51);
    
    // Atualizar displays
    display1.setValue(valor1, true);  // Com texto
    display2.setValue(valor2, false); // Sem texto
    
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

// Criar displays analógicos
VAnalog temperatura(50, 50, 0);
VAnalog umidade(120, 50, 0);

const uint8_t qtdVAnalog = 2;
VAnalog *arrayVAnalog[qtdVAnalog] = {&temperatura, &umidade};

void setup() {
    Serial.begin(115200);
    
    // Inicializar display
    bus = new Arduino_ESP32SPI(/* ... */);
    gfx = new Arduino_ST7789(/* ... */);
    gfx->begin();
    
    myDisplay.setDrawObject(gfx);
    
    // Configurar Temperatura
    VerticalAnalogConfig configTemp = {
        .width = 60,
        .height = 150,
        .minValue = -20,
        .maxValue = 60,
        .steps = 16,
        .arrowColor = CFK_RED,
        .textColor = CFK_BLACK,
        .backgroundColor = CFK_WHITE,
        .borderColor = CFK_BLACK
    };
    temperatura.setup(configTemp);
    temperatura.drawBackground();
    
    // Configurar Umidade
    VerticalAnalogConfig configUmidade = {
        .width = 60,
        .height = 150,
        .minValue = 0,
        .maxValue = 100,
        .steps = 20,
        .arrowColor = CFK_BLUE,
        .textColor = CFK_BLACK,
        .backgroundColor = CFK_WHITE,
        .borderColor = CFK_BLACK
    };
    umidade.setup(configUmidade);
    umidade.drawBackground();
    
    myDisplay.setVAnalog(arrayVAnalog, qtdVAnalog);
    myDisplay.loadScreen(minhaTela);
    myDisplay.createTask(false, 3);
}

void loop() {
    // Simular leituras de sensores
    float temp = 25.0 + 10.0 * sin(millis() / 5000.0);
    float umid = 50.0 + 20.0 * cos(millis() / 7000.0);
    
    temperatura.setValue((int)temp, true);
    umidade.setValue((int)umid, true);
    
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
- **Pequenos:** 50x100 pixels
- **Médios:** 60x150 pixels
- **Grandes:** 80x200 pixels

**Nota:** A largura mínima é de 40 pixels conforme definido no código.

### 🎨 Cores
- **arrowColor:** Use cores vibrantes para destaque
- **textColor:** Preto ou branco para contraste
- **backgroundColor:** Branco ou cinza claro
- **borderColor:** Preto ou cinza escuro

### 📏 Marcações e Escalas
- Use `steps` adequado para legibilidade
- Marcações maiores a cada 5 divisões
- Configure faixa realista (min/max)
- Considere precisão necessária

### ⚡ Performance
- Redesenho apenas quando valor muda
- Fundo desenhado uma vez apenas
- Atualização eficiente da seta
- Texto opcional para performance
- Áreas de desenho otimizadas

### 👥 Usabilidade
- Posicione texto abaixo da escala
- Use cores intuitivas
- Marcações claras para fácil leitura
- Espaço adequado entre displays

### 🎨 Visual
- Seta triangular contra fundo claro
- Marcações graduadas uniformes
- Borda definida para contorno
- Texto numérico opcional
- Layout otimizado com áreas separadas

### 🔔 Valores
- Valores limitados automaticamente
- Texto atualizado automaticamente
- Seta move proporcionalmente
- Faixa configurável

---

## 🔗 Herança de WidgetBase

A classe `VAnalog` herda métodos de `WidgetBase`:

- `isEnabled()` / `setEnabled()`: Habilitar/desabilitar
- `isInitialized()`: Verificar configuração
- `m_visible`: Controla visibilidade
- `m_shouldRedraw`: Flag para redesenho

---

## 🔗 Integração com DisplayFK

O `VAnalog` integra-se com o sistema DisplayFK:

1. **Renderização:** Automática com `drawWidgetsOnScreen()`
2. **Atualização:** Valores atualizados com `setValue()`
3. **Gerenciamento:** Controlado pelo loop principal
4. **Performance:** Redesenho eficiente
5. **Valores:** Limitação automática respeitada

---

## 🎨 Detalhes de Renderização

O VAnalog renderiza em áreas otimizadas:

1. **Fundo Principal:**
   - Retângulo com backgroundColor
   - Borda externa com borderColor

2. **Área de Desenho (m_drawArea):**
   - Marcações graduadas horizontais
   - Linhas menores a cada divisão
   - Linhas maiores a cada 5 divisões
   - Espaçamento uniforme na escala

3. **Área da Seta (m_arrowArea):**
   - Triângulo que se move verticalmente
   - Posição baseada no valor atual
   - Cor definida por arrowColor
   - Limpeza automática da posição anterior

4. **Área de Texto (m_textArea):**
   - Exibido abaixo da escala
   - Mostra valor atual
   - Cor definida por textColor
   - Fonte RobotoRegular5pt7b

5. **Margens (m_margin):**
   - Espaçamento interno configurável
   - Separação entre áreas
   - Layout otimizado

---

## 🔧 Solução de Problemas

### Display não aparece
- Verifique se chamou `setup()` após criar
- Confirme que `drawBackground()` foi chamado
- Verifique valores de width e height > 40
- Chame `myDisplay.setVAnalog()`

### Valor não muda visualmente
- Verifique se chamou `setValue()` com novo valor
- Confirme que valor está na faixa min/max
- Verifique logs para erros
- Teste com valores extremos

### Texto não aparece
- Verifique se `_viewValue` está true
- Confirme que `drawBackground()` foi chamado
- Teste com texto simples
- Verifique área de texto disponível

### Visual incorreto
- Verifique cores configuradas
- Confirme dimensões adequadas
- Teste com mais espaçamento
- Verifique steps apropriado

### Problemas de escala
- Verifique configuração de min/max
- Confirme que steps está adequado
- Teste com diferentes faixas
- Verifique proporção height/steps

### Seta não se move
- Verifique se valor está mudando
- Confirme que `redraw()` está sendo chamado
- Teste com valores extremos
- Verifique cores da seta

---

## 📚 Referências

- **Classe Base:** `WidgetBase` (src/widgets/widgetbase.h)
- **DisplayFK Principal:** `DisplayFK` (src/displayfk.h)
- **Cores Padrão:** Definidas em displayfk.h
- **Exemplos:** examples/Embed_ESP32S3/Display_Test/