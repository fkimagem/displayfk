# Documentação da Classe RectButton

## Visão Geral

A classe `RectButton` é um widget interativo que exibe um botão retangular com bordas arredondadas que muda de aparência quando pressionado. Ele herda de `WidgetBase` e fornece funcionalidade completa para criar botões com estados visuais distintos.

### Características Principais

- 🔲 Botão retangular com bordas arredondadas
- 🎨 Cor personalizável para estado pressionado (área interna)
- 📐 Dimensões configuráveis (largura e altura)
- 🔄 Estados ligado/desligado com mudança visual
- 🔔 Callback para interações
- 👆 Suporte a toque para mudança de estado
- 🔒 Pode ser habilitado/desabilitado
- 👁️ Pode ser mostrado/ocultado dinamicamente
- 🌓 Suporte a modo claro/escuro
- 🔗 Integra-se automaticamente com o sistema DisplayFK

---

## 📋 Estruturas de Configuração

### RectButtonConfig

Estrutura que contém todos os parâmetros de configuração:

```cpp
struct RectButtonConfig {
  uint16_t width;         // Largura do botão em pixels
  uint16_t height;        // Altura do botão em pixels
  uint16_t pressedColor;  // Cor da área interna quando pressionado
  functionCB_t callback;  // Função callback
};
```

---

## 🔧 Métodos Públicos

### Construtor

```cpp
RectButton(uint16_t _x, uint16_t _y, uint8_t _screen)
```

Cria um novo botão retangular.

**Parâmetros:**
- `_x`: Coordenada X do canto superior esquerdo
- `_y`: Coordenada Y do canto superior esquerdo
- `_screen`: Identificador da tela (0 = primeira tela)

**Nota:** Após criar o objeto, chame `setup()` antes de usá-lo.

### Destrutor

```cpp
~RectButton()
```

Libera os recursos do botão.

### setup()

```cpp
void setup(const RectButtonConfig& config)
```

Configura o botão retangular. **Este método deve ser chamado após a criação do objeto.**

**Parâmetros:**
- `config`: Estrutura `RectButtonConfig` com as configurações

### getStatus()

```cpp
bool getStatus()
```

Retorna o estado atual do botão (true = pressionado, false = solto).

### setStatus()

```cpp
void setStatus(bool _status)
```

Define o estado do botão programaticamente.

**Parâmetros:**
- `_status`: Novo estado (true = pressionado, false = solto)

### getEnabled()

```cpp
bool getEnabled()
```

Retorna se o botão está habilitado.

### setEnabled()

```cpp
void setEnabled(bool newState)
```

Habilita ou desabilita o botão.

**Parâmetros:**
- `newState`: true para habilitar, false para desabilitar

### changeState()

```cpp
void changeState()
```

Inverte o estado atual do botão (toggle).

### show()

```cpp
void show()
```

Torna o botão visível.

### hide()

```cpp
void hide()
```

Oculta o botão.

---

## 🔒 Métodos Privados (Apenas para Referência)

Estes métodos são chamados internamente:

- `detectTouch()`: Detecta toque no botão
- `redraw()`: Redesenha o botão
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

// Criar botões
RectButton botao1(50, 100, 0);
RectButton botao2(200, 100, 0);

const uint8_t qtdRectbutton = 2;
RectButton *arrayRectbutton[qtdRectbutton] = {&botao1, &botao2};
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

### 🔧 Passo 4: Configurar os Botões

```cpp
void loadWidgets() {
    // Configurar Botão 1
    RectButtonConfig configBotao1 = {
        .width = 100,
        .height = 50,
        .pressedColor = CFK_COLOR01,
        .callback = botao1_cb
    };
    botao1.setup(configBotao1);
    
    // Configurar Botão 2
    RectButtonConfig configBotao2 = {
        .width = 120,
        .height = 60,
        .pressedColor = CFK_COLOR02,
        .callback = botao2_cb
    };
    botao2.setup(configBotao2);
    
    // Registrar no DisplayFK
    myDisplay.setRectbutton(arrayRectbutton, qtdRectbutton);
}
```

### 🔔 Passo 5: Criar Funções de Callback

```cpp
void botao1_cb() {
    bool estado = botao1.getStatus();
    Serial.print("Botão 1: ");
    Serial.println(estado ? "Pressionado" : "Solto");
}

void botao2_cb() {
    bool estado = botao2.getStatus();
    Serial.print("Botão 2: ");
    Serial.println(estado ? "Pressionado" : "Solto");
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
    // Ler estados dos botões
    bool estadoBotao1 = botao1.getStatus();
    bool estadoBotao2 = botao2.getStatus();
    
    // Alterar estado programaticamente
    botao1.setStatus(true);
    botao2.setEnabled(false);
    
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

// Criar botões
RectButton botaoLigar(50, 100, 0);
RectButton botaoDesligar(200, 100, 0);

const uint8_t qtdRectbutton = 2;
RectButton *arrayRectbutton[qtdRectbutton] = {&botaoLigar, &botaoDesligar};

void setup() {
    Serial.begin(115200);
    
    // Inicializar display
    bus = new Arduino_ESP32SPI(/* ... */);
    gfx = new Arduino_ST7789(/* ... */);
    gfx->begin();
    
    myDisplay.setDrawObject(gfx);
    
    // Configurar botão Ligar
    RectButtonConfig configLigar = {
        .width = 120,
        .height = 60,
        .pressedColor = CFK_GREEN,
        .callback = ligar_callback
    };
    botaoLigar.setup(configLigar);
    
    // Configurar botão Desligar
    RectButtonConfig configDesligar = {
        .width = 120,
        .height = 60,
        .pressedColor = CFK_RED,
        .callback = desligar_callback
    };
    botaoDesligar.setup(configDesligar);
    
    myDisplay.setRectbutton(arrayRectbutton, qtdRectbutton);
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

void ligar_callback() {
    Serial.println("Botão Ligar pressionado!");
    // Lógica para ligar algo
}

void desligar_callback() {
    Serial.println("Botão Desligar pressionado!");
    // Lógica para desligar algo
}
```

---

## 📏 Dicas e Boas Práticas

### 📐 Tamanhos Recomendados
- **Pequenos:** 60x30 pixels
- **Médios:** 100x50 pixels
- **Grandes:** 150x70 pixels

### 🎨 Cores
- **pressedColor:** Cor da área interna quando pressionado
- **Estado solto:** Usa cor de fundo padrão (lightBg)
- **Estado pressionado:** Usa pressedColor configurada
- Use cores contrastantes para pressedColor
- Considere o tema claro/escuro
- Verde para ações positivas
- Vermelho para ações negativas
- Azul para ações neutras

### 🔔 Estados
- Botão pressionado = true
- Botão solto = false
- Estado pode ser alterado programaticamente
- Callback executa na mudança de estado

### ⚡ Performance
- Redesenho apenas quando necessário
- Detecção de toque eficiente
- Estados internos otimizados
- Callbacks executados apenas na mudança

### 👥 Usabilidade
- Botões grandes o suficiente para toque
- Feedback visual claro do estado
- Cores intuitivas para diferentes ações
- Posições lógicas na interface

### 🎨 Visual
- Bordas arredondadas para aparência moderna
- Efeito de profundidade com múltiplas camadas
- Cores adaptadas ao modo claro/escuro
- Renderização suave e profissional

### 🔒 Controle
- Pode ser habilitado/desabilitado
- Estado persistente até mudança
- Controle programático disponível
- Integração com sistema de callbacks

---

## 🔗 Herança de WidgetBase

A classe `RectButton` herda métodos de `WidgetBase`:

- `isEnabled()` / `setEnabled()`: Habilitar/desabilitar
- `isInitialized()`: Verificar configuração
- `m_visible`: Controla visibilidade
- `m_shouldRedraw`: Flag para redesenho

---

## 🔗 Integração com DisplayFK

O `RectButton` integra-se com o sistema DisplayFK:

1. **Renderização:** Automática com `drawWidgetsOnScreen()`
2. **Callbacks:** Executados quando estado muda
3. **Gerenciamento:** Controlado pelo loop principal
4. **Performance:** Redesenho eficiente
5. **Estados:** Persistência entre ciclos

---

## 🎨 Detalhes de Renderização

O RectButton renderiza em camadas:

1. **Fundo Principal:**
   - Retângulo arredondado com cor de fundo padrão (lightBg)
   - Borda externa com cor de contraste

2. **Área Interna (Central):**
   - Retângulo arredondado menor (área interna)
   - **Cor muda baseada no estado:**
     - **Estado solto (false):** Cor de fundo padrão (lightBg)
     - **Estado pressionado (true):** pressedColor configurada
   - Borda interna com cor de contraste

3. **Efeito Visual:**
   - Múltiplas camadas para profundidade
   - Bordas arredondadas consistentes
   - Cores adaptadas ao tema claro/escuro
   - Mudança visual clara entre estados

---

## 🔧 Solução de Problemas

### Botão não aparece
- Verifique se chamou `setup()` após criar
- Confirme valores de width e height > 0
- Verifique se está na tela correta
- Chame `myDisplay.setRectbutton()`

### Toque não funciona
- Verifique se o usuário tocou dentro da área
- Confirme que o botão está habilitado
- Verifique logs para erros
- Teste com botão maior

### Estado não muda visualmente
- Verifique se callback foi configurado
- Confirme que detectTouch() está funcionando
- Verifique logs para erros
- Teste com setStatus() programático
- **Importante:** A cor só muda na área interna central do botão

### Callback não executa
- Verifique se callback foi configurado corretamente
- Confirme que estado realmente mudou
- Verifique logs para erros
- Teste com mudança programática

### Visual incorreto
- Verifique cores configuradas
- Confirme pressedColor contrasta com fundo
- Verifique dimensões adequadas
- Teste em modo claro/escuro
- **Nota:** A mudança de cor ocorre apenas na área interna central (não no fundo completo)

---

## 📚 Referências

- **Classe Base:** `WidgetBase` (src/widgets/widgetbase.h)
- **DisplayFK Principal:** `DisplayFK` (src/displayfk.h)
- **Cores Padrão:** Definidas em displayfk.h
- **Exemplos:** examples/Embed_ESP32S3/Display_Test/

