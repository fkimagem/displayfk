# Documentação da Classe Label

## Visão Geral

A classe `Label` é um widget visual que exibe texto na tela com formatação personalizável. Ele herda de `WidgetBase` e fornece funcionalidade completa para criar e gerenciar rótulos de texto com diferentes fontes, cores e alinhamentos.

### Características Principais

- 📝 Exibição de texto customizável
- 🎨 Múltiplas fontes suportadas
- 📊 Suporte a números inteiros e floats
- 🔤 Prefixos e sufixos configuráveis
- 📐 Alinhamento configurável (datum)
- 🎨 Cores de texto e fundo personalizáveis
- 📏 Tamanho de fonte ajustável
- 🔢 Casas decimais configuráveis
- 👁️ Pode ser mostrado/ocultado dinamicamente
- 💾 Gerenciamento inteligente de memória
- 🔗 Integra-se automaticamente com o sistema DisplayFK

---

## 📋 Estruturas de Configuração

### LabelConfig

Estrutura que contém todos os parâmetros de configuração do label (ordem conforme `wlabel.h`):

```cpp
struct LabelConfig {
  const char* text;          // Texto inicial a ser exibido.
  const char* prefix;        // Texto de prefixo a ser exibido.
  const char* suffix;        // Texto de sufixo a ser exibido.
  #if defined(USING_GRAPHIC_LIB)
  const GFXfont* fontFamily; // Ponteiro para a fonte usada no texto.
  #endif
  uint16_t datum;            // Configuração de alinhamento do texto.
  uint16_t fontColor;        // Cor do texto.
  uint16_t backgroundColor;  // Cor de fundo do rótulo.
};
```

---

## 🔧 Métodos Públicos

### Construtor

```cpp
Label(uint16_t _x, uint16_t _y, uint8_t _screen)
```

Cria um novo label na posição especificada.

**Parâmetros:**
- `_x`: Coordenada X para posição do texto
- `_y`: Coordenada Y para posição do texto
- `_screen`: Identificador da tela (0 = primeira tela)

**Nota:** Após criar o objeto, é obrigatório chamar `setup()` antes de usá-lo.

### Destrutor

```cpp
~Label()
```

Libera automaticamente memória alocada para texto, prefixo e sufixo.

### setup()

```cpp
void setup(const LabelConfig& config)
```

Configura o label com os parâmetros especificados. **Este método deve ser chamado após a criação do objeto.**

**Parâmetros:**
- `config`: Estrutura `LabelConfig` com as configurações

### setText()

```cpp
void setText(const char* str)
void setText(const String& str)
```

Define o texto do label. Aceita C-string ou objeto String.

**Parâmetros:**
- `str`: Texto a ser exibido

**Características:**
- Aplica automaticamente prefixo e sufixo
- Aloca memória dinamicamente
- Marca para redesenho

### setPrefix()

```cpp
void setPrefix(const char* str)
```

Define o texto de prefixo exibido antes do texto principal.

**Parâmetros:**
- `str`: Texto do prefixo (nullptr para remover)

### setSuffix()

```cpp
void setSuffix(const char* str)
```

Define o texto de sufixo exibido após o texto principal.

**Parâmetros:**
- `str`: Texto do sufixo (nullptr para remover)

### setTextInt()

```cpp
void setTextInt(int value)
```

Define o texto do label como um número inteiro.

**Parâmetros:**
- `value`: Valor inteiro a ser exibido

### setTextFloat()

```cpp
void setTextFloat(float value, uint8_t decimalPlaces = 2)
```

Define o texto do label como um número float formatado.

**Parâmetros:**
- `value`: Valor float a ser exibido
- `decimalPlaces`: Número de casas decimais (padrão: 2)

### setDecimalPlaces()

```cpp
void setDecimalPlaces(uint8_t places)
```

Define o número de casas decimais para exibição.

**Parâmetros:**
- `places`: Número de casas decimais (0-5)

### setFontSize()

```cpp
void setFontSize(uint8_t newSize)
```

Define o tamanho da fonte.

**Parâmetros:**
- `newSize`: Tamanho da fonte

### show()

```cpp
void show()
```

Torna o label visível na tela.

### hide()

```cpp
void hide()
```

Oculta o label da tela.

---

## 🔒 Métodos Privados (Apenas para Referência)

Estes métodos são chamados internamente:

- `detectTouch()`: Não processa eventos de toque (sempre retorna false)
- `redraw()`: Redesenha o texto na tela
- `forceUpdate()`: Força atualização
- `getCallbackFunc()`: Retorna função callback
- `cleanupMemory()`: Limpa memória alocada

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

// Criar labels: Label(x, y, tela)
Label text(60, 40, 0);
Label line1(65, 455, 0);
Label line2(180, 455, 0);

// Criar array de ponteiros
const uint8_t qtdLabel = 3;
Label *arrayLabel[qtdLabel] = {&text, &line1, &line2};
```

### 📝 Passo 3: Prototipar Funções

```cpp
// Labels não usam callbacks normalmente
```

### ⚙️ Passo 4: Configurar Display (setup)

```cpp
void setup() {
    // ... configuração do display e touch ...
    myDisplay.setDrawObject(tft);
    
    loadWidgets();  // Configurar widgets
    myDisplay.loadScreen(screen0);
    myDisplay.createTask(false, 3);
}
```

### 🔧 Passo 5: Configurar os Widgets

```cpp
void loadWidgets() {
    // Configurar label simples
    LabelConfig configLabel0 = {
        .text = "Custom text",
        .prefix = "",
        .suffix = "",
        .fontFamily = &RobotoRegular10pt7b,
        .datum = TL_DATUM,
        .fontColor = CFK_BLACK,
        .backgroundColor = CFK_WHITE
    };
    text.setup(configLabel0);
    
    // Label com prefixo
    LabelConfig configLabel1 = {
        .text = "Line 1",
        .prefix = "Line",
        .suffix = "",
        .fontFamily = &RobotoRegular10pt7b,
        .datum = TL_DATUM,
        .fontColor = CFK_COLOR09,
        .backgroundColor = CFK_WHITE
    };
    line1.setup(configLabel1);
    
    // Label com sufixo
    LabelConfig configLabel2 = {
        .text = "Line 2",
        .prefix = "Line",
        .suffix = "C",
        .fontFamily = &RobotoRegular10pt7b,
        .datum = TL_DATUM,
        .fontColor = CFK_COLOR04,
        .backgroundColor = CFK_WHITE
    };
    line2.setup(configLabel2);
    
    // Registrar no DisplayFK
    myDisplay.setLabel(arrayLabel, qtdLabel);
}
```

### 🔔 Passo 6: Criar Funções de Callback (Opcional)

```cpp
// Labels não usam callbacks normalmente
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

### 🔄 Passo 8: Atualizar Texto (Exemplo de Uso)

```cpp
void loop() {
    // Atualizar com texto simples
    text.setText("Novo texto");
    
    // Atualizar com número inteiro
    int valor = random(0, 100);
    line1.setTextInt(valor);
    
    // Atualizar com float
    float temperatura = 23.5;
    line2.setTextFloat(temperatura, 1);
    
    // Adicionar prefixo dinamicamente
    text.setPrefix("Temp: ");
    text.setTextFloat(temperatura);
    
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

// Criar labels
Label titulo(200, 50, 0);
Label subtitulo(200, 100, 0);
Label valorTemperatura(200, 150, 0);
Label valorUmidade(200, 200, 0);

const uint8_t qtdLabel = 4;
Label *arrayLabel[qtdLabel] = {&titulo, &subtitulo, &valorTemperatura, &valorUmidade};

void setup() {
    Serial.begin(115200);
    
    // Inicializar display
    bus = new Arduino_ESP32SPI(/* ... */);
    gfx = new Arduino_ST7789(/* ... */);
    gfx->begin();
    
    myDisplay.setDrawObject(gfx);
    
    // Configurar label de título
    LabelConfig configTitulo = {
        .text = "Sistema de Monitoramento",
        .prefix = "",
        .suffix = "",
        .fontFamily = &RobotoBold10pt7b,
        .datum = MC_DATUM,
        .fontColor = CFK_BLACK,
        .backgroundColor = CFK_WHITE
    };
    titulo.setup(configTitulo);
    
    // Configurar subtítulo
    LabelConfig configSubtitulo = {
        .text = "Sensor de Ambiente",
        .prefix = "",
        .suffix = "",
        .fontFamily = &RobotoRegular10pt7b,
        .datum = MC_DATUM,
        .fontColor = CFK_GREY6,
        .backgroundColor = CFK_WHITE
    };
    subtitulo.setup(configSubtitulo);
    
    // Configurar label de temperatura
    LabelConfig configTemp = {
        .text = "0",
        .prefix = "Temperatura: ",
        .suffix = " °C",
        .fontFamily = &RobotoRegular10pt7b,
        .datum = TL_DATUM,
        .fontColor = CFK_BLACK,
        .backgroundColor = CFK_WHITE
    };
    valorTemperatura.setup(configTemp);
    
    // Configurar label de umidade
    LabelConfig configUmidade = {
        .text = "0",
        .prefix = "Umidade: ",
        .suffix = " %",
        .fontFamily = &RobotoRegular10pt7b,
        .datum = TL_DATUM,
        .fontColor = CFK_BLACK,
        .backgroundColor = CFK_WHITE
    };
    valorUmidade.setup(configUmidade);
    
    myDisplay.setLabel(arrayLabel, qtdLabel);
    myDisplay.loadScreen(minhaTela);
    myDisplay.createTask(false, 3);
}

void loop() {
    // Simular dados de sensores
    float temperatura = random(200, 300) / 10.0;
    int umidade = random(40, 80);
    
    // Atualizar labels
    valorTemperatura.setTextFloat(temperatura, 1);
    valorUmidade.setTextInt(umidade);
    
    delay(2000);
}

void minhaTela() {
    gfx->fillScreen(CFK_WHITE);
    WidgetBase::backgroundColor = CFK_WHITE;
    myDisplay.drawWidgetsOnScreen(0);
}
```

---

## 📏 Dicas e Boas Práticas

### 📐 Datums (Alinhamento)
- `TL_DATUM`: Top Left (Canto superior esquerdo)
- `TC_DATUM`: Top Center (Centro superior)
- `TR_DATUM`: Top Right (Canto superior direito)
- `ML_DATUM`: Middle Left (Centro esquerdo)
- `MC_DATUM`: Middle Center (Centro)
- `MR_DATUM`: Middle Right (Centro direito)
- `BL_DATUM`: Bottom Left (Canto inferior esquerdo)
- `BC_DATUM`: Bottom Center (Centro inferior)
- `BR_DATUM`: Bottom Right (Canto inferior direito)

### 🎨 Fontes
- Use fontes adequadas ao tamanho do texto
- Fontes maiores para títulos importantes
- Fontes menores para textos secundários
- RobotoRegular10pt7b é comum para texto normal
- RobotoBold10pt7b para destaque

### 🔔 Callbacks
- Labels não usam callbacks por padrão
- São elementos apenas visuais

### ⚡ Performance
- Atualizações de texto alocam nova memória
- Evite atualizações muito frequentes
- Use setTextInt/setTextFloat para números
- Prefixos e sufixos são aplicados automaticamente

### 👥 Usabilidade
- Use cores contrastantes para legibilidade
- Considere o fundo ao escolher cor do texto
- Alinhamento adequado para contexto
- Tamanho de fonte apropriado para distância de leitura

### 🎨 Design Visual
- Texto é renderizado com fonte configurada
- Cor de fundo é usada para limpar área anterior
- Prefixo e sufixo são concatenados automaticamente
- Tamanho de fonte é aplicado durante renderização
- Alinhamento definido pelo datum

### 📊 Formatação de Números
- Use setTextInt() para inteiros
- Use setTextFloat() para floats com decimais
- Defina casas decimais apropriadas
- Use prefixo/sufixo para unidades
- Considere formatação para display

---

## 🔗 Herança de WidgetBase

A classe `Label` herda todos os métodos de `WidgetBase`:

- `isEnabled()` / `setEnabled()`: Habilitar/desabilitar widget
- `isInitialized()`: Verificar se foi configurado
- `m_visible`: Controla visibilidade interna
- `m_shouldRedraw`: Flag para redesenho automático

---

## 🔗 Integração com DisplayFK

O `Label` integra-se automaticamente com o sistema DisplayFK:

1. **Renderização:** Automática quando usando `drawWidgetsOnScreen()`
2. **Gerenciamento:** Controlado pelo loop principal do DisplayFK
3. **Sincronização:** Estados sincronizados entre diferentes telas
4. **Memória:** Gerenciamento automático de alocação
5. **Performance:** Redesenho apenas quando necessário

---

## 🎨 Detalhes de Renderização

O `Label` é renderizado em uma única operação:

1. **Texto:**
   - Concatenação de prefixo + texto + sufixo
   - Alocação dinâmica de memória
   - Buffer interno para texto completo

2. **Renderização:**
   - Define cor de texto e fundo
   - Aplica fonte e tamanho
   - Usa printText() com datum configurado
   - Limpa área anterior automaticamente
   - Restaura configurações de fonte

3. **Otimizações:**
   - Redesenho apenas quando m_shouldRedraw é true
   - Rastreamento de área anterior para limpeza eficiente
   - Gerenciamento de memória com validação

---

## 🔧 Solução de Problemas

### Label não aparece na tela
- Verifique se chamou `setup()` após criar o objeto
- Confirme que o texto não é vazio
- Verifique se está na tela correta
- Certifique-se de chamar `myDisplay.setLabel()`
- Confirme que fontFamily foi configurado

### Texto não atualiza
- Use `setText()`, `setTextInt()` ou `setTextFloat()`
- Verifique se o label foi inicializado com `setup()`
- Confirme que fontColor contrasta com backgroundColor
- Certifique-se que o widget está visível

### Problemas de memória
- Labels alocam memória dinamicamente para texto
- Textos muito longos podem consumir RAM
- Memória é liberada automaticamente pelo destrutor
- Use cleanupMemory() se necessário para liberar manualmente

### Prefixo ou sufixo não aparece
- Verifique se foram configurados no setup() ou chamando setPrefix/setSuffix
- Confirme que são diferentes de nullptr e ""
- Use setPrefix() e setSuffix() para alterar dinamicamente

### Números não formatados corretamente
- Use setTextFloat() para floats
- Defina casas decimais apropriadas
- setTextInt() para inteiros
- Verifique se valores estão dentro dos limites

### Performance lenta
- Evite atualizações muito frequentes
- Use números inteiros quando possível
- Reduza tamanho de texto se necessário
- Limpe prefixos/sufixos desnecessários

### Cores não aparecem corretamente
- Use formatos RGB565 corretos
- Verifique contraste entre texto e fundo
- Considere modo claro/escuro da interface
- Teste diferentes combinações de cores

---

## 📚 Referências

- **Classe Base:** `WidgetBase` (src/widgets/widgetbase.h)
- **DisplayFK Principal:** `DisplayFK` (src/displayfk.h)
- **Exemplos:** examples/Embed_ESP32S3/Display_Test/
- **Cores Padrão:** Definidas em displayfk.h (CFK_COLOR01-CFK_COLOR28, CFK_GREY*, CFK_WHITE, CFK_BLACK)
- **Datums:** Definidos em Arduino_GFX_Library (TL_DATUM, TC_DATUM, etc.)

