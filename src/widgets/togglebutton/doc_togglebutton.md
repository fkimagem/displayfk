# Documentação da Classe ToggleButton

## Visão Geral

A classe `ToggleButton` é um widget interativo que representa um botão toggle (interruptor) com uma bola deslizante. Ele herda de `WidgetBase` e fornece funcionalidade completa para criar botões que alternam entre estado ligado e desligado a cada toque.

### Características Principais

- 🔄 Botão toggle com bola deslizante
- 🎨 Cor personalizável quando ligado
- 🔔 Callback para ações de alternância
- 👆 Suporte a toque para ativação
- 🔒 Pode ser habilitado/desabilitado
- 👁️ Pode ser mostrado/ocultado dinamicamente
- 🌓 Suporte a modo claro/escuro
- 🔗 Integra-se automaticamente com o sistema DisplayFK

---

## 📋 Estruturas de Configuração

### ToggleButtonConfig

Estrutura que contém todos os parâmetros de configuração:

```cpp
struct ToggleButtonConfig {
  uint16_t width;         // Largura do botão
  uint16_t height;        // Altura do botão
  uint16_t pressedColor;  // Cor quando ligado
  functionCB_t callback;  // Função callback
};
```

---

## 🔧 Métodos Públicos

### Construtor

```cpp
ToggleButton(uint16_t _x, uint16_t _y, uint8_t _screen)
```

Cria um novo botão toggle.

**Parâmetros:**
- `_x`: Coordenada X do canto superior esquerdo
- `_y`: Coordenada Y do canto superior esquerdo
- `_screen`: Identificador da tela (0 = primeira tela)

**Nota:** Após criar o objeto, chame `setup()` antes de usá-lo.

### Destrutor

```cpp
~ToggleButton()
```

Libera os recursos do ToggleButton.

### setup()

```cpp
void setup(const ToggleButtonConfig& config)
```

Configura o botão toggle. **Este método deve ser chamado após a criação do objeto.**

**Parâmetros:**
- `config`: Estrutura `ToggleButtonConfig` com as configurações

### changeState()

```cpp
void changeState()
```

Alterna o estado do botão (ligado ↔ desligado).

### getStatus()

```cpp
bool getStatus()
```

Retorna o estado atual do botão (true = ligado, false = desligado).

### setStatus()

```cpp
void setStatus(bool status)
```

Define o estado do botão programaticamente.

**Parâmetros:**
- `status`: true para ligado, false para desligado

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

- `detectTouch()`: Detecta toque no botão com validações completas
- `redraw()`: Redesenha o botão com cores adaptadas ao tema
- `forceUpdate()`: Força atualização
- `getCallbackFunc()`: Retorna callback
- `cleanupMemory()`: Limpa memória e canvas
- `start()`: Inicializa configurações e valida dimensões
- `blitRowFromCanvas()`: Copia dados do canvas para o display

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

// Criar botões toggle
ToggleButton toggle1(50, 100, 0);
ToggleButton toggle2(200, 100, 0);

const uint8_t qtdTogglebutton = 2;
ToggleButton *arrayTogglebutton[qtdTogglebutton] = {&toggle1, &toggle2};
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

### 🔧 Passo 4: Configurar os Botões Toggle

```cpp
void loadWidgets() {
    // Configurar Toggle 1
    ToggleButtonConfig configToggle1 = {
        .width = 80,
        .height = 40,
        .pressedColor = CFK_GREEN,
        .callback = toggle1_cb
    };
    toggle1.setup(configToggle1);
    
    // Configurar Toggle 2
    ToggleButtonConfig configToggle2 = {
        .width = 100,
        .height = 50,
        .pressedColor = CFK_BLUE,
        .callback = toggle2_cb
    };
    toggle2.setup(configToggle2);
    
    // Registrar no DisplayFK
    myDisplay.setToggle(arrayTogglebutton, qtdTogglebutton);
}
```

### 🔔 Passo 5: Criar Funções de Callback

```cpp
void toggle1_cb() {
    bool estado = toggle1.getStatus();
    Serial.println("Toggle 1: " + String(estado ? "LIGADO" : "DESLIGADO"));
}

void toggle2_cb() {
    bool estado = toggle2.getStatus();
    Serial.println("Toggle 2: " + String(estado ? "LIGADO" : "DESLIGADO"));
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
    // Verificar estados
    bool estado1 = toggle1.getStatus();
    bool estado2 = toggle2.getStatus();
    
    // Alterar estado programaticamente
    if (Serial.available()) {
        char comando = Serial.read();
        if (comando == '1') {
            toggle1.changeState();
        } else if (comando == '2') {
            toggle2.setStatus(!toggle2.getStatus());
        } else if (comando == '3') {
            toggle1.changeState(); // Alterna estado programaticamente
        }
    }
    
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

// Criar botões toggle
ToggleButton wifiToggle(50, 100, 0);
ToggleButton bluetoothToggle(200, 100, 0);

const uint8_t qtdTogglebutton = 2;
ToggleButton *arrayTogglebutton[qtdTogglebutton] = {&wifiToggle, &bluetoothToggle};

void setup() {
    Serial.begin(115200);
    
    // Inicializar display
    bus = new Arduino_ESP32SPI(/* ... */);
    gfx = new Arduino_ST7789(/* ... */);
    gfx->begin();
    
    myDisplay.setDrawObject(gfx);
    
    // Configurar WiFi Toggle
    ToggleButtonConfig configWifi = {
        .width = 80,
        .height = 40,
        .pressedColor = CFK_GREEN,
        .callback = wifi_callback
    };
    wifiToggle.setup(configWifi);
    
    // Configurar Bluetooth Toggle
    ToggleButtonConfig configBluetooth = {
        .width = 80,
        .height = 40,
        .pressedColor = CFK_BLUE,
        .callback = bluetooth_callback
    };
    bluetoothToggle.setup(configBluetooth);
    
    myDisplay.setToggle(arrayTogglebutton, qtdTogglebutton);
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

void wifi_callback() {
    bool wifiLigado = wifiToggle.getStatus();
    Serial.println("WiFi: " + String(wifiLigado ? "LIGADO" : "DESLIGADO"));
    
    if (wifiLigado) {
        // Conectar WiFi
    } else {
        // Desconectar WiFi
    }
}

void bluetooth_callback() {
    bool bluetoothLigado = bluetoothToggle.getStatus();
    Serial.println("Bluetooth: " + String(bluetoothLigado ? "LIGADO" : "DESLIGADO"));
    
    if (bluetoothLigado) {
        // Ativar Bluetooth
    } else {
        // Desativar Bluetooth
    }
}
```

---

## 📏 Dicas e Boas Práticas

### 📐 Tamanhos Recomendados
- **Pequenos:** 60x30 pixels
- **Médios:** 80x40 pixels
- **Grandes:** 100x50 pixels

**Nota:** A largura mínima é de 30 pixels conforme definido no código.

### 🎨 Cores
- Use cores intuitivas (verde = ligado, cinza = desligado)
- Verde para estados positivos (WiFi, Bluetooth)
- Azul para estados neutros
- Vermelho para estados de alerta
- Considere o tema claro/escuro

### 🔔 Estados
- Estado inicial pode ser definido com `setStatus()`
- Use `getStatus()` para verificar estado atual
- Callbacks executados a cada mudança
- Estados persistentes entre ciclos

### ⚡ Performance
- Redesenho apenas quando estado muda
- Detecção de toque eficiente com debounce
- Callbacks executados apenas na alternância
- Estados internos otimizados
- Validações de visibilidade, teclado e carregamento

### 👥 Usabilidade
- Botões grandes o suficiente para toque
- Cores claras para diferentes estados
- Posições lógicas na interface
- Feedback visual imediato

### 🎨 Visual
- Bola deslizante da esquerda (desligado) para direita (ligado)
- Fundo muda de cor quando ligado
- Bordas arredondadas para aparência moderna
- Cores adaptadas ao modo claro/escuro

### 🔒 Controle
- Pode ser habilitado/desabilitado
- Alternância programática disponível
- Integração com sistema de callbacks
- Estados persistentes
- Suporte a canvas interno para renderização avançada

---

## 🔗 Herança de WidgetBase

A classe `ToggleButton` herda métodos de `WidgetBase`:

- `isEnabled()` / `setEnabled()`: Habilitar/desabilitar
- `isInitialized()`: Verificar configuração
- `m_visible`: Controla visibilidade
- `m_shouldRedraw`: Flag para redesenho

---

## 🔗 Integração com DisplayFK

O `ToggleButton` integra-se com o sistema DisplayFK:

1. **Renderização:** Automática com `drawWidgetsOnScreen()`
2. **Callbacks:** Executados quando estado muda
3. **Gerenciamento:** Controlado pelo loop principal
4. **Performance:** Redesenho eficiente
5. **Estados:** Persistência entre ciclos

---

## 🎨 Detalhes de Renderização

O ToggleButton renderiza assim:

1. **Fundo:**
   - Retângulo arredondado com altura/2 como raio
   - Cor muda baseada no estado (pressedColor quando ligado)
   - Cor de fundo adaptada ao tema claro/escuro

2. **Borda:**
   - Borda externa com cor adaptada ao tema
   - Mesmo raio do fundo

3. **Bola Deslizante:**
   - Círculo que se move da esquerda para direita
   - Posição esquerda = desligado
   - Posição direita = ligado
   - Cor branca quando ligado, cinza quando desligado
   - Borda ao redor da bola

4. **Adaptação ao Tema:**
   - Cores ajustadas automaticamente
   - Modo claro/escuro suportado
   - Contraste adequado

5. **Validações de Toque:**
   - Verifica visibilidade do widget
   - Confirma que não está usando teclado virtual
   - Valida tela atual
   - Verifica se widget está carregado
   - Aplica debounce para evitar múltiplos cliques
   - Confirma que widget está habilitado
   - Verifica se não está bloqueado
   - Valida ponteiro de toque

---

## 🔧 Solução de Problemas

### Botão não aparece
- Verifique se chamou `setup()` após criar
- Confirme valores de width e height > 0
- Verifique se está na tela correta
- Chame `myDisplay.setToggle()`

### Toque não funciona
- Verifique se o usuário tocou dentro da área
- Confirme que o botão está habilitado e não bloqueado
- Verifique se não está usando teclado virtual
- Confirme que está na tela correta
- Verifique logs para erros
- Teste com botão maior

### Callback não executa
- Verifique se callback foi configurado corretamente
- Confirme que o botão foi realmente tocado
- Verifique logs para erros
- Teste com `changeState()` programático

### Estado não muda visualmente
- Verifique se `m_shouldRedraw` está sendo definido
- Confirme que `redraw()` está sendo chamado
- Verifique cores configuradas
- Teste com cores mais contrastantes

### Visual incorreto
- Verifique cores configuradas
- Confirme dimensões adequadas
- Verifique proporção width/height
- Teste em modo claro/escuro

---

## 📚 Referências

- **Classe Base:** `WidgetBase` (src/widgets/widgetbase.h)
- **DisplayFK Principal:** `DisplayFK` (src/displayfk.h)
- **Cores Padrão:** Definidas em displayfk.h
- **Exemplos:** examples/Embed_ESP32S3/Display_Test/
