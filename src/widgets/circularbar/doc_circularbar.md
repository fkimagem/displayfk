# Documentação da Classe CircularBar

## Visão Geral

A classe `CircularBar` é um widget visual que exibe um valor como um arco circular preenchido dentro de uma faixa especificada. Ele herda de `WidgetBase` e fornece uma interface gráfica completa para criar barras circulares em displays.

### Características Principais

- 🔄 Barra circular com arco preenchido proporcionalmente
- 📊 Exibição de valor numérico no centro
- 🎨 Cores personalizáveis para barra e fundo
- 📐 Configuração flexível de ângulos (0-360°)
- 📏 Espessura ajustável da barra
- 🔄 Suporte a direção invertida de preenchimento
- 👁️ Pode ser mostrado/ocultado dinamicamente
- 🔗 Integra-se automaticamente com o sistema DisplayFK
- ⚡ Renderização eficiente com atualizações incrementais

---

## 📋 Estruturas de Configuração

### CircularBarConfig

Estrutura que contém todos os parâmetros de configuração da barra circular (ordem conforme `wcircularbar.h`):

```cpp
struct CircularBarConfig {
  int minValue;              // Valor mínimo da faixa da barra.
  int maxValue;              // Valor máximo da faixa da barra.
  uint16_t radius;           // Raio da barra circular em pixels.
  uint16_t startAngle;       // Ângulo inicial da barra em graus (0-360).
  uint16_t endAngle;         // Ângulo final da barra em graus (0-360).
  uint16_t color;            // Cor RGB565 da barra quando preenchida.
  uint16_t backgroundColor;  // Cor RGB565 de fundo da barra.
  uint16_t textColor;        // Cor RGB565 do texto do valor.
  uint16_t backgroundText;   // Cor RGB565 do fundo da área do texto.
  uint8_t thickness;         // Espessura da linha da barra em pixels.
  bool showValue;            // Flag para mostrar/ocultar o valor no centro.
  bool inverted;             // Flag para direção invertida de preenchimento.
};
```

---

## 🔧 Métodos Públicos

### Construtor

```cpp
CircularBar(uint16_t _x, uint16_t _y, uint8_t _screen)
```

Cria uma nova barra circular na posição especificada. A posição (`_x`, `_y`) representa o **centro** da barra.

**Parâmetros:**
- `_x`: Coordenada X do centro da barra na tela
- `_y`: Coordenada Y do centro da barra na tela
- `_screen`: Identificador da tela (0 = primeira tela)

**Nota:** Após criar o objeto, é obrigatório chamar `setup()` antes de usá-lo.

### Destrutor

```cpp
~CircularBar()
```

Libera os recursos da barra circular automaticamente.

### setup()

```cpp
void setup(const CircularBarConfig& config)
```

Configura a barra circular com os parâmetros especificados. **Este método deve ser chamado após a criação do objeto.**

**Parâmetros:**
- `config`: Estrutura `CircularBarConfig` com as configurações

**Validações automáticas:**
- Valores min/max são trocados automaticamente se min > max
- Exibição de valor é desabilitada se `radius - thickness < 15`
- Valores são inicializados com o valor mínimo

### setValue()

```cpp
void setValue(int newValue)
```

Define o valor atual da barra circular e marca para redesenho.

**Parâmetros:**
- `newValue`: Novo valor a ser exibido na barra

**Características:**
- Armazena o valor anterior para renderização eficiente
- Marca o widget para redesenho automático
- Valor é limitado automaticamente dentro da faixa `[minValue, maxValue]`

### setMinValue()

```cpp
void setMinValue(int newValue)
```

Define o valor mínimo da faixa da barra circular.

**Parâmetros:**
- `newValue`: Novo valor mínimo

**Características:**
- Ordena automaticamente os valores se min > max
- Marca para redesenho completo da barra
- Ativa flag interna `m_changedScale` que força limpeza completa do arco antes de redesenhar

### setMaxValue()

```cpp
void setMaxValue(int newValue)
```

Define o valor máximo da faixa da barra circular.

**Parâmetros:**
- `newValue`: Novo valor máximo

**Características:**
- Ordena automaticamente os valores se min > max
- Marca para redesenho completo da barra
- Ativa flag interna `m_changedScale` que força limpeza completa do arco antes de redesenhar

### getMinValue()

```cpp
int getMinValue()
```

Retorna o valor mínimo atual da faixa da barra circular.

**Retorno:**
- Valor mínimo configurado

### getMaxValue()

```cpp
int getMaxValue()
```

Retorna o valor máximo atual da faixa da barra circular.

**Retorno:**
- Valor máximo configurado

### setScale()

```cpp
void setScale(int newMinValue, int newMaxValue)
```

Define simultaneamente os valores mínimo e máximo da faixa da barra circular.

**Parâmetros:**
- `newMinValue`: Novo valor mínimo
- `newMaxValue`: Novo valor máximo

**Características:**
- Ordena automaticamente os valores se min > max
- Ajusta o valor atual para ficar dentro da nova faixa
- Marca para redesenho completo da barra
- Ativa flag interna `m_changedScale` que força limpeza completa do arco antes de redesenhar
- Útil para mudanças dinâmicas de escala

### drawBackground()

```cpp
void drawBackground()
```

Desenha o fundo da barra circular. **Este método deve ser chamado uma vez após setup() para inicializar o fundo.**

**Características:**
- Desenha o arco de fundo completo
- Inicializa a exibição da barra
- Deve ser chamado antes de usar `setValue()`

### show()

```cpp
void show()
```

Torna a barra circular visível na tela.

### hide()

```cpp
void hide()
```

Oculta a barra circular da tela.

---

## 🔒 Métodos Internos (Apenas para Referência)

Estes métodos existem na classe, porém normalmente são usados no ciclo interno de renderização:

- `detectTouch()`: Não processa eventos de toque (sempre retorna false)
- `redraw()`: Redesenha a barra na tela com renderização eficiente
- `forceUpdate()`: Força uma atualização imediata
- `getCallbackFunc()`: Retorna a função callback
- `sortValues()`: Ordena valores min/max se necessário (chamado automaticamente)

### Membros Privados

- `m_value`: Valor atual a ser exibido na barra
- `m_lastValue`: Último valor representado (usado para otimização de redesenho incremental)
- `m_config`: Estrutura contendo configuração completa da barra circular
- `m_changedScale`: Flag que indica quando a escala foi alterada (força redesenho completo do arco)

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

// Criar a barra circular: CircularBar(centro_x, centro_y, tela)
CircularBar circload(335, 215, 0);

// Criar array de ponteiros
const uint8_t qtdCircBar = 1;
CircularBar *arrayCircularbar[qtdCircBar] = {&circload};
```

### 📝 Passo 3: Funções auxiliares (opcional)

```cpp
// CircularBar não dispara callback no fluxo padrão
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
    // Configurar barra circular
    CircularBarConfig configCirculaBar = {
        .minValue = 0,
        .maxValue = 100,
        .radius = 40,
        .startAngle = 0,
        .endAngle = 360,
        .color = CFK_COLOR09,
        .backgroundColor = CFK_WHITE,
        .textColor = CFK_BLACK,
        .backgroundText = CFK_WHITE,
        .thickness = 10,
        .showValue = true,
        .inverted = false
    };
    circload.setup(configCirculaBar);
    
    // Desenhar fundo uma vez
    circload.drawBackground();
    
    // Registrar no DisplayFK
    myDisplay.setCircularBar(arrayCircularbar, qtdCircBar);
}
```

### 🔔 Passo 6: Atualizar valor dinamicamente

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
    // Simular mudança de valor
    static int valor = 0;
    static bool incrementando = true;
    
    if (incrementando) {
        valor += 5;
        if (valor >= 100) incrementando = false;
    } else {
        valor -= 5;
        if (valor <= 0) incrementando = true;
    }
    
    // Atualizar a barra circular
    circload.setValue(valor);
    
    delay(100);
}
```

---

## 📝 Exemplo Completo

```cpp
#include <displayfk.h>

DisplayFK myDisplay;
Arduino_DataBus *bus = nullptr;
Arduino_GFX *gfx = nullptr;

// Criar barra circular
CircularBar minhaBarra(150, 150, 0);

const uint8_t qtdBarra = 1;
CircularBar *arrayBarra[qtdBarra] = {&minhaBarra};

void setup() {
    Serial.begin(115200);
    
    // Inicializar display
    bus = new Arduino_ESP32SPI(/* ... */);
    gfx = new Arduino_ST7789(/* ... */);
    gfx->begin();
    
    myDisplay.setDrawObject(gfx);
    myDisplay.startTouchGT911(/* ... */);
    
    // Configurar barra circular
    CircularBarConfig config = {
        .radius = 60,
        .minValue = 0,
        .maxValue = 100,
        .startAngle = 0,
        .endAngle = 360,
        .thickness = 15,
        .color = CFK_COLOR09,
        .backgroundColor = CFK_GREY7,
        .textColor = CFK_BLACK,
        .backgroundText = CFK_WHITE,
        .showValue = true,
        .inverted = false
    };
    minhaBarra.setup(config);
    
    // Desenhar fundo
    minhaBarra.drawBackground();
    
    myDisplay.setCircularBar(arrayBarra, qtdBarra);
    myDisplay.loadScreen(minhaTela);
    myDisplay.createTask(false, 3);
}

void loop() {
    // Simular dados de sensor
    int valorSensor = random(0, 101);
    minhaBarra.setValue(valorSensor);
    
    delay(1000);
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
- **Pequenos:** Raio 20-30 pixels (ícones discretos)
- **Médios:** Raio 40-60 pixels (uso geral)
- **Grandes:** Raio 70-100 pixels (elementos principais)

### 🎨 Cores
- Use cores contrastantes entre barra e fundo
- Considere o tema claro/escuro da interface
- Texto deve contrastar com o fundo da área central
- Cores vibrantes para a barra preenchida

### 🔔 Callbacks
- O `CircularBar` não dispara callback no código atual
- Para reação a mudanças, faça a lógica no ponto onde `setValue()` é chamado

### ⚡ Performance
- Renderização eficiente com atualizações incrementais
- Desenha apenas a diferença entre valores
- Evite atualizações muito frequentes
- Use `drawBackground()` apenas uma vez
- Mudanças de escala redesenham completamente a barra

### 👥 Usabilidade
- Deixe espaço suficiente ao redor da barra
- Use tamanhos adequados para a densidade do display
- Considere a legibilidade do texto no centro
- Teste diferentes ângulos de início e fim

### 🎨 Design Visual
- A barra desenha um arco circular preenchido
- Fundo: arco completo com cor de fundo
- Preenchimento: proporcional ao valor atual
- Centro: círculo com valor numérico (se habilitado)
- Espessura ajustável para diferentes estilos

### 📊 Configuração de Ângulos
- **0°**: Direita (3 horas)
- **90°**: Baixo (6 horas)
- **180°**: Esquerda (9 horas)
- **270°**: Cima (12 horas)
- **360°**: Volta completa

### 🔔 Valores e Escala
- Faixa configurável dinamicamente com `setMinValue()`, `setMaxValue()` ou `setScale()`
- Consulta de valores com `getMinValue()` e `getMaxValue()`
- Ordenação automática de valores min/max
- `setValue()` limita automaticamente dentro da faixa
- `setScale()` também limita o valor atual na nova faixa

---

## 🔗 Herança de WidgetBase

A classe `CircularBar` herda todos os métodos de `WidgetBase`:

- `isEnabled()` / `setEnabled()`: Habilitar/desabilitar widget
- `isInitialized()`: Verificar se foi configurado
- `m_visible`: Controla visibilidade interna
- `m_shouldRedraw`: Flag para redesenho automático

---

## 🔗 Integração com DisplayFK

O `CircularBar` integra-se automaticamente com o sistema DisplayFK:

1. **Renderização:** Automática quando usando `drawWidgetsOnScreen()`
2. **Sincronização:** Estados sincronizados entre diferentes telas
3. **Gerenciamento:** Controlado pelo loop principal do DisplayFK
4. **Performance:** Renderização eficiente com atualizações incrementais
5. **Fontes:** Usa `RobotoBold10pt7b` para exibição de valores (quando `showValue = true`)

---

## 🎨 Detalhes de Renderização

O `CircularBar` é renderizado em camadas:

1. **Fundo** (drawBackground()):
   - Arco completo com cor de fundo
   - Desenhado uma vez após setup()

2. **Preenchimento** (redraw()):
   - Arco proporcional ao valor atual
   - Atualização incremental (apenas diferença)
   - Cor da barra configurável

3. **Centro** (se showValue = true):
   - Círculo com cor de fundo do texto
   - Valor numérico centralizado
   - Fonte RobotoBold10pt7b

4. **Otimizações**:
   - Desenha apenas a diferença entre valores (usando `m_lastValue`)
   - Flag `m_changedScale` força redesenho completo do arco quando a escala muda
   - Debounce para evitar redesenhos excessivos
   - Validações de estado antes de renderizar (TFT, visibilidade, inicialização, tela atual, etc.)

---

## 🔧 Solução de Problemas

### Barra não aparece na tela
- Verifique se chamou `setup()` após criar o objeto
- Confirme que `drawBackground()` foi chamado após setup()
- Verifique se está na tela correta
- Certifique-se de chamar `myDisplay.setCircularBar()`
- Confirme que o raio é maior que 0

### Barra não atualiza o valor
- Verifique se chamou `setValue()` com novo valor
- Confirme que o valor está dentro da faixa min/max
- Verifique se o widget está habilitado
- Certifique-se de que `drawBackground()` foi chamado

### Mudança de escala não funciona
- Use `setScale()` para alterar min/max simultaneamente
- Ou use `setMinValue()` e `setMaxValue()` separadamente
- Valores são ordenados automaticamente se min > max
- A barra é redesenhada completamente quando a escala muda
- Se quiser também limitar imediatamente o valor atual na nova faixa, prefira `setScale()`

### Texto não aparece no centro
- Verifique se `showValue = true` na configuração
- Confirme que raio interno (radius - thickness) >= 20 pixels
- Verifique se as cores do texto contrastam com o fundo
- Certifique-se de que a fonte está disponível

### Problemas visuais
- Verifique se as cores contrastam adequadamente
- Confirme que a barra está dentro dos limites da tela
- Verifique se os ângulos estão configurados corretamente
- Para barras grossas, considere ajustar a espessura

### Performance lenta
- Evite atualizações muito frequentes de valor
- Use valores dentro da faixa configurada
- Considere aumentar o debounce se necessário
- Verifique se não há muitos widgets na mesma tela

### Ângulos incorretos
- Lembre-se: 0° = direita, 90° = baixo, 180° = esquerda, 270° = cima
- Verifique se startAngle < endAngle
- Considere usar ângulos negativos se necessário
- Teste diferentes configurações de ângulo

---

## 📚 Referências

- **Classe Base:** `WidgetBase` (src/widgets/widgetbase.h)
- **DisplayFK Principal:** `DisplayFK` (src/displayfk.h)
- **Exemplos:** examples/Embed_ESP32S3/Display_Test/
- **Cores Padrão:** Definidas em displayfk.h (CFK_COLOR01-CFK_COLOR28, CFK_GREY*, CFK_WHITE, CFK_BLACK)
- **Fonte:** `RobotoBold10pt7b` (src/fonts/RobotoRegular/)

