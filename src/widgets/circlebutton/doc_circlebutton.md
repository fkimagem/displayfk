# Documentação da Classe CircleButton

## Visão Geral

A classe `CircleButton` é um widget interativo que representa um botão circular com feedback visual quando pressionado. Ele herda de `WidgetBase` e fornece uma interface gráfica completa para criar botões circulares em displays touchscreen.

### Características Principais

- ⭕ Botão completamente circular
- 🔄 Duplo círculo (externo e interno) para feedback visual
- 👆 Suporte a toque (touchscreen)
- 🎯 Detecção precisa de toque dentro da área circular
- 🎨 Cores personalizáveis
- 🔔 Função de callback para responder a interações
- 👁️ Pode ser mostrado/ocultado dinamicamente
- 🔗 Integra-se automaticamente com o sistema DisplayFK
- 🌓 Ajuste automático de cores para modos claro/escuro

---

## 📋 Estruturas de Configuração

### CircleButtonConfig

Estrutura que contém todos os parâmetros de configuração do botão circular (ordem conforme `wcirclebutton.h`):

```cpp
struct CircleButtonConfig {
  functionCB_t callback;   // Callback executada quando o botão é pressionado.
  uint16_t radius;        // Raio do botão circular em pixels (5-200 recomendado).
  uint16_t pressedColor;  // Cor RGB565 exibida quando o botão está pressionado.
};
```

---

## 🔧 Métodos Públicos

### Construtor

```cpp
CircleButton(uint16_t _x, uint16_t _y, uint8_t _screen)
```

Cria um novo botão circular na posição especificada. A posição (`_x`, `_y`) representa o **centro** do círculo.

**Parâmetros:**
- `_x`: Coordenada X do centro do botão na tela
- `_y`: Coordenada Y do centro do botão na tela
- `_screen`: Identificador da tela (0 = primeira tela)

**Nota:** Após criar o objeto, é obrigatório chamar `setup()` antes de usá-lo.

### Destrutor

```cpp
~CircleButton()
```

Libera os recursos do botão circular automaticamente.

### setup()

```cpp
void setup(const CircleButtonConfig& config)
```

Configura o botão circular com os parâmetros especificados. **Este método deve ser chamado após a criação do objeto.**

**Parâmetros:**
- `config`: Estrutura `CircleButtonConfig` com as configurações

**Validações automáticas:**
- Raio é limitado entre 5 e 200 pixels
- Valores fora da faixa são ajustados automaticamente

### getStatus()

```cpp
bool getStatus() const
```

Retorna o estado atual do botão.

**Retorna:**
- `true`: Botão pressionado
- `false`: Botão não pressionado

### setStatus()

```cpp
void setStatus(bool _status)
```

Define programaticamente o estado do botão e dispara o callback (se configurado).

**Parâmetros:**
- `_status`: `true` para pressionar, `false` para liberar

**Características:**
- Não executa atualização se o estado já é o solicitado
- Dispara callback automaticamente quando o estado muda

### show()

```cpp
void show()
```

Torna o botão circular visível na tela.

### hide()

```cpp
void hide()
```

Oculta o botão circular da tela.

---

## 🔒 Métodos Privados (Apenas para Referência)

Estes métodos são chamados internamente e não precisam ser invocados diretamente:

- `detectTouch()`: Detecta toque do usuário no botão
- `redraw()`: Redesenha o botão na tela
- `forceUpdate()`: Força uma atualização imediata
- `getCallbackFunc()`: Retorna a função callback
- `changeState()`: Alterna o estado internamente
- `start()`: Aplica restrições de tamanho

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

// Criar o botão circular: CircleButton(centro_x, centro_y, tela)
CircleButton cirbtn(280, 45, 0);

// Criar array de ponteiros
const uint8_t qtdCircleBtn = 1;
CircleButton *arrayCirclebtn[qtdCircleBtn] = {&cirbtn};
```

### 📝 Passo 3: Prototipar Funções de Callback

```cpp
void cirbtn_cb();
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
    // Configurar botão circular
    CircleButtonConfig configCircleButton = {
        .callback = cirbtn_cb,
        .radius = 40,
        .pressedColor = CFK_COLOR15
    };
    cirbtn.setup(configCircleButton);
    
    // Registrar no DisplayFK
    myDisplay.setCircleButton(arrayCirclebtn, qtdCircleBtn);
}
```

### 🔔 Passo 6: Criar Funções de Callback

```cpp
void cirbtn_cb() {
    bool estado = cirbtn.getStatus();
    Serial.print("Botão circular pressionado: ");
    Serial.println(estado);
    
    // Faça algo quando o botão for tocado
    if (estado) {
        // Botão foi pressionado
    } else {
        // Botão foi liberado
    }
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
    // Pressionar botão programaticamente
    cirbtn.setStatus(true);
    
    // Liberar botão
    cirbtn.setStatus(false);
    
    // Verificar estado
    bool estáPressionado = cirbtn.getStatus();
    
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

// Criar botão circular
CircleButton meuBotao(150, 100, 0);

const uint8_t qtdBotao = 1;
CircleButton *arrayBotao[qtdBotao] = {&meuBotao};

void setup() {
    Serial.begin(115200);
    
    // Inicializar display
    bus = new Arduino_ESP32SPI(/* ... */);
    gfx = new Arduino_ST7789(/* ... */);
    gfx->begin();
    
    myDisplay.setDrawObject(gfx);
    myDisplay.startTouchGT911(/* ... */);
    
    // Configurar botão circular
    CircleButtonConfig config = {
        .callback = botao_callback,
        .radius = 50,
        .pressedColor = CFK_COLOR15
    };
    meuBotao.setup(config);
    
    myDisplay.setCircleButton(arrayBotao, qtdBotao);
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

void botao_callback() {
    Serial.print("Estado do botão: ");
    Serial.println(meuBotao.getStatus());
}
```

---

## 📏 Dicas e Boas Práticas

### 📐 Tamanhos Recomendados
- **Pequenos:** 20-30 pixels (ícones discretos)
- **Médios:** 40-60 pixels (uso geral)
- **Grandes:** 70-100 pixels (botões importantes)

### 🎨 Cores
- Use cores vibrantes para o estado pressionado
- O círculo externo usa cor automática baseada no tema
- Cores de fundo ajustam-se automaticamente aos modos claro/escuro

### 🔔 Callbacks
- Mantenha as callbacks curtas e rápidas
- Não bloqueie a execução dentro do callback
- Use variáveis globais para armazenar dados importantes

### ⚡ Performance
- Menos widgets na mesma tela = melhor performance
- Botões pequenos renderizam mais rápido
- Evite muitos botões na mesma área da tela

### 👥 Usabilidade
- Deixe espaço suficiente ao redor do botão
- Use tamanhos maiores em interfaces touchscreen
- Considere agrupar botões relacionados visualmente
- Teste a área de toque - parece pequena mas pode estar correta

### 🎨 Design Visual
- O botão desenha dois círculos concêntricos
- Círculo externo: borda e fundo
- Círculo interno: 75% do raio, muda de cor quando pressionado
- Bordas ajustam-se automaticamente ao tema claro/escuro

---

## 🔗 Herança de WidgetBase

A classe `CircleButton` herda todos os métodos de `WidgetBase`:

- `isEnabled()` / `setEnabled()`: Habilitar/desabilitar widget
- `isInitialized()`: Verificar se foi configurado
- `m_visible`: Controla visibilidade interna
- `m_shouldRedraw`: Flag para redesenho automático

---

## 🔗 Integração com DisplayFK

O `CircleButton` integra-se automaticamente com o sistema DisplayFK:

1. **Detecção de toque:** Automática quando usando `startTouchGT911()` ou similar
2. **Sincronização:** Estados sincronizados entre diferentes telas
3. **Gerenciamento:** Controlado pelo loop principal do DisplayFK
4. **Callbacks:** Executados de forma segura e não-bloqueante
5. **Modos claro/escuro:** Ajustes automáticos de cor

---

## 🎨 Detalhes de Renderização

O `CircleButton` é renderizado em camadas:

1. **Círculo externo** (100% do raio):
   - Fundo: Cinza claro (CFK_GREY11) ou cinza escuro (CFK_GREY3) dependendo do tema
   - Borda: Preta ou branca dependendo do tema

2. **Círculo interno** (75% do raio):
   - Fundo: Cor de estado pressionado ou fundo padrão
   - Borda: Preta ou branca dependendo do tema

3. **Feedback visual**:
   - Quando não pressionado: Círculo interno com fundo padrão
   - Quando pressionado: Círculo interno com `pressedColor`

---

## 🔧 Solução de Problemas

### Botão não aparece na tela
- Verifique se chamou `setup()` após criar o objeto
- Confirme que `show()` foi chamado (padrão é visível)
- Verifique se está na tela correta
- Certifique-se de chamar `myDisplay.setCircleButton()`
- Confirme que o raio é maior que 0

### Botão não responde ao toque
- Verifique se o touch foi inicializado corretamente
- Confirme que as coordenadas do toque estão mapeadas
- Verifique se o widget está habilitado (`setEnabled(true)`)
- O toque deve estar dentro da área circular (centro + raio)
- Confirme que o HAS_TOUCH está definido

### Callback não é executado
- Verifique se a função callback foi configurada corretamente
- Confirme que o nome da função está correto
- Certifique-se de não ter erros de compilação
- Verifique se o estado realmente mudou (`setStatus` não dispara callback se o estado já está correto)

### Problemas visuais
- Verifique se as cores contrastam com o fundo
- Confirme que o botão está dentro dos limites da tela
- Verifique se o background foi desenhado corretamente na tela
- Para modo escuro, as bordas são brancas por padrão

### Botão muito pequeno/grande
- Ajuste o raio na configuração (5-200 pixels)
- Tamanho total = raio × 2 (diâmetro)
- Considere a densidade de pixels do display

---

## 📚 Referências

- **Classe Base:** `WidgetBase` (src/widgets/widgetbase.h)
- **DisplayFK Principal:** `DisplayFK` (src/displayfk.h)
- **Exemplos:** examples/Embed_ESP32S3/Display_Test/
- **Cores Padrão:** Definidas em displayfk.h (CFK_COLOR01-CFK_COLOR28, CFK_GREY*, CFK_WHITE, CFK_BLACK)

