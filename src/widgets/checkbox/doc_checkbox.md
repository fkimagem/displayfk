# Documentação da Classe CheckBox

## Visão Geral

A classe `CheckBox` é um widget interativo que permite ao usuário alternar entre dois estados: marcado (✓) e desmarcado. Ele herda de `WidgetBase` e fornece uma interface gráfica completa para criar caixas de seleção em displays touchscreen.

### Características Principais

- 📦 Quadrado com cantos arredondados
- ✓ Marca de verificação branca quando marcado
- 👆 Suporte a toque (touchscreen)
- 📏 Três espessuras diferentes da marca
- 🎨 Cores personalizáveis para estado marcado e desmarcado
- 🔔 Função de callback para responder a mudanças de estado
- 👁️ Pode ser mostrado/ocultado dinamicamente
- 🔗 Integra-se automaticamente com o sistema DisplayFK

---

## 📋 Estruturas de Configuração

### CheckBoxConfig

Estrutura que contém todos os parâmetros de configuração do checkbox:

```cpp
struct CheckBoxConfig {
  uint16_t size;             // Tamanho do checkbox (largura = altura) em pixels
  uint16_t checkedColor;     // Cor quando marcado (formato RGB565)
  uint16_t uncheckedColor;   // Cor quando desmarcado (0 = automático baseado no modo)
  CheckBoxWeight weight;     // Espessura da marca (LIGHT, MEDIUM, HEAVY)
  functionCB_t callback;     // Ponteiro para função callback
};
```

### CheckBoxWeight (enum)

Define a espessura da marca de verificação:

```cpp
enum class CheckBoxWeight {
  LIGHT = 1,   // Marca com 1 linha
  MEDIUM = 2,  // Marca com 3 linhas
  HEAVY = 3    // Marca com 5 linhas
};
```

---

## 🔧 Métodos Públicos

### Construtor

```cpp
CheckBox(uint16_t _x, uint16_t _y, uint8_t _screen)
```

Cria um novo checkbox na posição especificada.

**Parâmetros:**
- `_x`: Posição horizontal (coluna) na tela
- `_y`: Posição vertical (linha) na tela
- `_screen`: Identificador da tela (0 = primeira tela)

**Nota:** Após criar o objeto, é obrigatório chamar `setup()` antes de usá-lo.

### Destrutor

```cpp
~CheckBox()
```

Libera os recursos do checkbox automaticamente.

### setup()

```cpp
void setup(const CheckBoxConfig& config)
```

Configura o checkbox com os parâmetros especificados. **Este método deve ser chamado após a criação do objeto.**

**Parâmetros:**
- `config`: Estrutura `CheckBoxConfig` com as configurações

**Validações automáticas:**
- Tamanho (size) deve estar entre 10 e 100 pixels
- Valores fora da faixa são ajustados automaticamente

### getStatus()

```cpp
bool getStatus() const
```

Retorna o estado atual do checkbox.

**Retorna:**
- `true`: Checkbox marcado
- `false`: Checkbox desmarcado

### setStatus()

```cpp
void setStatus(bool status)
```

Define programaticamente o estado do checkbox e dispara o callback (se configurado).

**Parâmetros:**
- `status`: `true` para marcar, `false` para desmarcar

### show()

```cpp
void show()
```

Torna o checkbox visível na tela.

### hide()

```cpp
void hide()
```

Oculta o checkbox da tela.

### setSize()

```cpp
void setSize(uint16_t newSize)
```

Altera o tamanho do checkbox em tempo de execução.

**Parâmetros:**
- `newSize`: Novo tamanho em pixels (10-100 recomendado)

### getSize()

```cpp
uint16_t getSize() const
```

Retorna o tamanho atual do checkbox.

### setColors()

```cpp
void setColors(uint16_t checkedColor, uint16_t uncheckedColor = 0)
```

Altera as cores do checkbox.

**Parâmetros:**
- `checkedColor`: Cor RGB565 para o estado marcado
- `uncheckedColor`: Cor RGB565 para o estado desmarcado (0 = automático)

---

## 🔒 Métodos Privados (Apenas para Referência)

Estes métodos são chamados internamente e não precisam ser invocados diretamente:

- `detectTouch()`: Detecta toque do usuário no checkbox
- `redraw()`: Redesenha o checkbox na tela
- `forceUpdate()`: Força uma atualização imediata
- `getCallbackFunc()`: Retorna a função callback
- `changeState()`: Alterna o estado internamente
- `drawCheckmark()`: Desenha a marca de verificação
- `drawBorder()`: Desenha a borda do checkbox
- `getUncheckedColor()`: Calcula a cor do estado desmarcado
- `getBorderColor()`: Calcula a cor da borda

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

// Criar o checkbox: CheckBox(x, y, tela)
CheckBox checkbox(200, 10, 0);
CheckBox checkbox1(200, 42, 0);

// Criar array de ponteiros
const uint8_t qtdCheckbox = 2;
CheckBox *arrayCheckbox[qtdCheckbox] = {&checkbox, &checkbox1};
```

### 📝 Passo 3: Prototipar Funções de Callback

```cpp
void checkbox_cb();
void checkbox1_cb();
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
    // Configurar checkbox
    CheckBoxConfig configCheckbox = {
        .size = 21,
        .checkedColor = CFK_COLOR28,
        .uncheckedColor = CFK_GREY7,
        .weight = CheckBoxWeight::MEDIUM,
        .callback = checkbox_cb
    };
    checkbox.setup(configCheckbox);
    
    CheckBoxConfig configCheckbox1 = {
        .size = 21,
        .checkedColor = CFK_COLOR03,
        .uncheckedColor = CFK_GREY10,
        .weight = CheckBoxWeight::HEAVY,
        .callback = checkbox1_cb
    };
    checkbox1.setup(configCheckbox1);
    
    // Registrar no DisplayFK
    myDisplay.setCheckbox(arrayCheckbox, qtdCheckbox);
}
```

### 🔔 Passo 6: Criar Funções de Callback

```cpp
void checkbox_cb() {
    bool estado = checkbox.getStatus();
    Serial.print("Checkbox marcado: ");
    Serial.println(estado);
    
    // Faça algo quando o checkbox for tocado
    if (estado) {
        // Checkbox foi marcado
    } else {
        // Checkbox foi desmarcado
    }
}

void checkbox1_cb() {
    bool estado = checkbox1.getStatus();
    Serial.print("Checkbox1 marcado: ");
    Serial.println(estado);
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

### 🔄 Passo 8: Alterar Estado Programaticamente (Opcional)

```cpp
void loop() {
    // Marcar checkbox programaticamente
    checkbox.setStatus(true);
    
    // Desmarcar checkbox
    checkbox.setStatus(false);
    
    // Verificar estado
    bool estáMarcado = checkbox.getStatus();
    
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

// Criar checkbox
CheckBox meuCheckbox(100, 50, 0);

const uint8_t qtdCheckbox = 1;
CheckBox *arrayCheckbox[qtdCheckbox] = {&meuCheckbox};

void setup() {
    Serial.begin(115200);
    
    // Inicializar display
    bus = new Arduino_ESP32SPI(/* ... */);
    gfx = new Arduino_ST7789(/* ... */);
    gfx->begin();
    
    myDisplay.setDrawObject(gfx);
    myDisplay.startTouchGT911(/* ... */);
    
    // Configurar checkbox
    CheckBoxConfig config = {
        .size = 25,
        .checkedColor = CFK_COLOR28,
        .uncheckedColor = 0,  // Automático
        .weight = CheckBoxWeight::MEDIUM,
        .callback = checkbox_callback
    };
    meuCheckbox.setup(config);
    
    myDisplay.setCheckbox(arrayCheckbox, qtdCheckbox);
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

void checkbox_callback() {
    Serial.print("Estado: ");
    Serial.println(meuCheckbox.getStatus());
}
```

---

## 📏 Dicas e Boas Práticas

### 📐 Tamanhos Recomendados
- **Pequenos:** 15-20 pixels (ícones discretos)
- **Médios:** 21-30 pixels (uso geral)
- **Grandes:** 35-50 pixels (fácil toque para usuários)

### 🎨 Cores
- Use cores contrastantes para melhor visibilidade
- Considere o tema claro/escuro da interface
- Defina `uncheckedColor = 0` para seleção automática fácil

### 🔔 Callbacks
- Mantenha as callbacks curtas e rápidas
- Não bloqueie a execução dentro do callback
- Use variáveis globais para armazenar dados importantes

### ⚡ Performance
- Menos widgets na mesma tela = melhor performance
- Evite redesenhar a tela inteira desnecessariamente
- Use `forceUpdate()` apenas quando realmente necessário

### 👥 Usabilidade
- Deixe espaço suficiente ao redor do checkbox (mínimo 5 pixels)
- Use tamanhos maiores em interfaces touchscreen
- Considere agrupar checkboxes relacionados visualmente

---

## 🔗 Herança de WidgetBase

A classe `CheckBox` herda todos os métodos de `WidgetBase`:

- `isEnabled()` / `setEnabled()`: Habilitar/desabilitar widget
- `isInitialized()`: Verificar se foi configurado
- `m_visible`: Controla visibilidade interna
- `m_shouldRedraw`: Flag para redesenho automático

---

## 🔗 Integração com DisplayFK

O `CheckBox` integra-se automaticamente com o sistema DisplayFK:

1. **Detecção de toque:** Automática quando usando `startTouchGT911()` ou similar
2. **Sincronização:** Estados sincronizados entre diferentes telas
3. **Gerenciamento:** Controlado pelo loop principal do DisplayFK
4. **Callbacks:** Executados de forma segura e não-bloqueante

---

## 🔧 Solução de Problemas

### Checkbox não aparece na tela
- Verifique se chamou `setup()` após criar o objeto
- Confirme que `show()` foi chamado (padrão é visível)
- Verifique se está na tela correta
- Certifique-se de chamar `myDisplay.setCheckbox()`

### Checkbox não responde ao toque
- Verifique se o touch foi inicializado corretamente
- Confirme que as coordenadas do toque estão mapeadas
- Verifique se o widget está habilitado (`setEnabled(true)`)

### Callback não é executado
- Verifique se a função callback foi configurada corretamente
- Confirme que o nome da função está correto
- Certifique-se de não ter erros de compilação

### Problemas visuais
- Verifique se as cores não são muito parecidas com o fundo
- Confirme que o tamanho está dentro da tela
- Verifique se o background foi desenhado corretamente na tela

---

## 📚 Referências

- **Classe Base:** `WidgetBase` (src/widgets/widgetbase.h)
- **DisplayFK Principal:** `DisplayFK` (src/displayfk.h)
- **Exemplos:** examples/Embed_ESP32S3/Display_Test/
- **Cores Padrão:** Definidas em displayfk.h (CFK_COLOR01-CFK_COLOR28, CFK_GREY*, CFK_WHITE, CFK_BLACK)

