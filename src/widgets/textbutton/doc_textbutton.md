# Documentação da Classe TextButton

## Visão Geral

A classe `TextButton` é um widget interativo que exibe um botão com texto personalizável e cantos arredondados. Ele herda de `WidgetBase` e fornece funcionalidade completa para criar botões com texto, cores e callbacks configuráveis.

### Características Principais

- 🔘 Botão com texto personalizável
- 🎨 Cores de fundo e texto configuráveis
- 📐 Dimensões e raio de bordas arredondadas configuráveis
- 🔤 Fontes personalizáveis (GFXfont)
- 🔔 Callback para ações de clique
- 👆 Suporte a toque para ativação
- 🔒 Pode ser habilitado/desabilitado
- 👁️ Pode ser mostrado/ocultado dinamicamente
- 🌓 Suporte a modo claro/escuro
- 🔗 Integra-se automaticamente com o sistema DisplayFK

---

## 📋 Estruturas de Configuração

### TextButtonConfig

Estrutura que contém todos os parâmetros de configuração:

```cpp
struct TextButtonConfig {
  uint16_t width;         // Largura do botão em pixels
  uint16_t height;        // Altura do botão em pixels
  uint16_t radius;        // Raio para cantos arredondados
  uint16_t backgroundColor; // Cor de fundo do botão
  uint16_t textColor;     // Cor do texto
  const char* text;       // Texto a exibir
  const GFXfont* fontFamily; // Ponteiro para a fonte usada para o texto
  functionCB_t callback;  // Função callback
};
```

---

## 🔧 Métodos Públicos

### Construtor

```cpp
TextButton(uint16_t _x, uint16_t _y, uint8_t _screen)
```

Cria um novo botão de texto.

**Parâmetros:**
- `_x`: Coordenada X do canto superior esquerdo
- `_y`: Coordenada Y do canto superior esquerdo
- `_screen`: Identificador da tela (0 = primeira tela)

**Nota:** Após criar o objeto, chame `setup()` antes de usá-lo.

### Destrutor

```cpp
~TextButton()
```

Libera os recursos do TextButton.

### setup()

```cpp
void setup(const TextButtonConfig& config)
```

Configura o botão de texto. **Este método deve ser chamado após a criação do objeto.**

**Parâmetros:**
- `config`: Estrutura `TextButtonConfig` com as configurações

### onClick()

```cpp
void onClick()
```

Simula um clique no botão programaticamente (executa o callback).

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
TextButton botao1(50, 100, 0);
TextButton botao2(200, 100, 0);

const uint8_t qtdTextbutton = 2;
TextButton *arrayTextbutton[qtdTextbutton] = {&botao1, &botao2};
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
    TextButtonConfig configBotao1 = {
        .width = 100,
        .height = 50,
        .radius = 10,
        .backgroundColor = CFK_COLOR01,
        .textColor = CFK_WHITE,
        .text = "OK",
        .fontFamily = &RobotoBold10pt7b,
        .callback = botao1_cb
    };
    botao1.setup(configBotao1);
    
    // Configurar Botão 2
    TextButtonConfig configBotao2 = {
        .width = 120,
        .height = 60,
        .radius = 15,
        .backgroundColor = CFK_COLOR02,
        .textColor = CFK_BLACK,
        .text = "Cancelar",
        .fontFamily = &RobotoBold12pt7b,
        .callback = botao2_cb
    };
    botao2.setup(configBotao2);
    
    // Registrar no DisplayFK
    myDisplay.setTextButton(arrayTextbutton, qtdTextbutton);
}
```

### 🔔 Passo 5: Criar Funções de Callback

```cpp
void botao1_cb() {
    Serial.println("Botão OK pressionado!");
}

void botao2_cb() {
    Serial.println("Botão Cancelar pressionado!");
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
    // Simular clique programaticamente
    botao1.onClick();
    
    // Verificar estado
    bool habilitado = botao1.getEnabled();
    
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
TextButton botaoSalvar(50, 100, 0);
TextButton botaoExcluir(200, 100, 0);

const uint8_t qtdTextbutton = 2;
TextButton *arrayTextbutton[qtdTextbutton] = {&botaoSalvar, &botaoExcluir};

void setup() {
    Serial.begin(115200);
    
    // Inicializar display
    bus = new Arduino_ESP32SPI(/* ... */);
    gfx = new Arduino_ST7789(/* ... */);
    gfx->begin();
    
    myDisplay.setDrawObject(gfx);
    
    // Configurar botão Salvar
    TextButtonConfig configSalvar = {
        .width = 120,
        .height = 50,
        .radius = 10,
        .backgroundColor = CFK_GREEN,
        .textColor = CFK_WHITE,
        .text = "Salvar",
        .fontFamily = &RobotoBold10pt7b,
        .callback = salvar_callback
    };
    botaoSalvar.setup(configSalvar);
    
    // Configurar botão Excluir
    TextButtonConfig configExcluir = {
        .width = 120,
        .height = 50,
        .radius = 10,
        .backgroundColor = CFK_RED,
        .textColor = CFK_WHITE,
        .text = "Excluir",
        .fontFamily = &RobotoBold10pt7b,
        .callback = excluir_callback
    };
    botaoExcluir.setup(configExcluir);
    
    myDisplay.setTextButton(arrayTextbutton, qtdTextbutton);
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

void salvar_callback() {
    Serial.println("Salvando dados...");
}

void excluir_callback() {
    Serial.println("Excluindo dados...");
}
```

---

## 📏 Dicas e Boas Práticas

### 📐 Tamanhos Recomendados
- **Pequenos:** 60x30 pixels
- **Médios:** 100x50 pixels
- **Grandes:** 150x70 pixels

### 🎨 Cores
- Use cores contrastantes para fundo e texto
- Verde para ações positivas (OK, Salvar)
- Vermelho para ações negativas (Cancelar, Excluir)
- Azul para ações neutras
- Considere o tema claro/escuro

### 🔔 Textos e Fontes
- Use textos curtos e claros
- Evite textos muito longos
- Considere o espaço disponível
- Use fontes apropriadas para o tamanho do botão
- Configure `fontFamily` com ponteiro para fonte GFXfont (ex: `&RobotoBold10pt7b`)
- Fontes maiores podem não caber em botões pequenos

### ⚡ Performance
- Redesenho apenas quando necessário
- Detecção de toque eficiente
- Callbacks executados apenas no clique
- Estados internos otimizados

### 👥 Usabilidade
- Botões grandes o suficiente para toque
- Textos legíveis e claros
- Cores intuitivas para diferentes ações
- Posições lógicas na interface

### 🎨 Visual
- Bordas arredondadas para aparência moderna
- Cores adaptadas ao modo claro/escuro
- Texto centralizado automaticamente
- Fonte configurável através de fontFamily

### 🔒 Controle
- Pode ser habilitado/desabilitado
- Clique programático disponível
- Integração com sistema de callbacks
- Estados persistentes

---

## 🔗 Herança de WidgetBase

A classe `TextButton` herda métodos de `WidgetBase`:

- `isEnabled()` / `setEnabled()`: Habilitar/desabilitar
- `isInitialized()`: Verificar configuração
- `m_visible`: Controla visibilidade
- `m_shouldRedraw`: Flag para redesenho

---

## 🔗 Integração com DisplayFK

O `TextButton` integra-se com o sistema DisplayFK:

1. **Renderização:** Automática com `drawWidgetsOnScreen()`
2. **Callbacks:** Executados quando botão é clicado
3. **Gerenciamento:** Controlado pelo loop principal
4. **Performance:** Redesenho eficiente
5. **Estados:** Persistência entre ciclos

---

## 🎨 Detalhes de Renderização

O TextButton renderiza assim:

1. **Fundo:**
   - Retângulo arredondado com backgroundColor
   - Borda externa com cor adaptada ao tema

2. **Texto:**
   - Texto centralizado no botão
   - Cor definida por textColor
   - Fonte definida por fontFamily (ponteiro para GFXfont)

3. **Adaptação ao Tema:**
   - Cores ajustadas automaticamente
   - Modo claro/escuro suportado
   - Contraste adequado

---

## 🔧 Solução de Problemas

### Botão não aparece
- Verifique se chamou `setup()` após criar
- Confirme valores de width e height > 0
- Verifique se está na tela correta
- Chame `myDisplay.setTextButton()`

### Toque não funciona
- Verifique se o usuário tocou dentro da área
- Confirme que o botão está habilitado
- Verifique logs para erros
- Teste com botão maior

### Callback não executa
- Verifique se callback foi configurado corretamente
- Confirme que o botão foi realmente clicado
- Verifique logs para erros
- Teste com onClick() programático

### Texto não aparece
- Verifique se o texto foi configurado
- Confirme textColor contrasta com backgroundColor
- Verifique se fontFamily foi configurado corretamente
- Verifique dimensões adequadas para a fonte escolhida
- Teste com texto mais simples
- Certifique-se de que a fonte está incluída no projeto

### Visual incorreto
- Verifique cores configuradas
- Confirme radius está adequado
- Verifique dimensões apropriadas
- Teste em modo claro/escuro

---

## 📚 Referências

- **Classe Base:** `WidgetBase` (src/widgets/widgetbase.h)
- **DisplayFK Principal:** `DisplayFK` (src/displayfk.h)
- **Cores Padrão:** Definidas em displayfk.h
- **Exemplos:** examples/Embed_ESP32S3/Display_Test/

