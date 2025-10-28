# Documentação da Classe TextBox

## Visão Geral

A classe `TextBox` é um widget interativo que exibe e permite edição de texto através de um teclado virtual. Ele herda de `WidgetBase` e trabalha em conjunto com `Keyboard` para fornecer uma interface completa de entrada de texto.

### Características Principais

- 📝 Exibe e edita texto (string)
- ⌨️ Permite edição via teclado virtual (Keyboard)
- 🎨 Cores e fontes personalizáveis
- 📐 Dimensões configuráveis
- 🔔 Callback para mudanças de texto
- 👆 Suporte a toque para abrir teclado
- 💾 Texto armazenado como CharString
- 👁️ Pode ser mostrado/ocultado dinamicamente
- 🔗 Integra-se com Keyboard automaticamente
- 🌓 Suporte a modo claro/escuro

---

## 📋 Estruturas de Configuração

### TextBoxConfig

Estrutura que contém todos os parâmetros de configuração:

```cpp
struct TextBoxConfig {
  uint16_t width;            // Largura da caixa de texto
  uint16_t height;           // Altura da caixa de texto
  uint16_t letterColor;      // Cor do texto
  uint16_t backgroundColor;  // Cor de fundo
  const char* startValue;    // Texto inicial
  const GFXfont* font;       // Fonte para o texto
  functionLoadScreen_t funcPtr; // Ponteiro para função da tela pai
  functionCB_t cb;           // Função callback
};
```

---

## 🔧 Métodos Públicos

### Construtor

```cpp
TextBox(uint16_t _x, uint16_t _y, uint8_t _screen)
TextBox() // Construtor padrão
```

Cria um novo TextBox.

**Parâmetros:**
- `_x`: Coordenada X do canto superior esquerdo
- `_y`: Coordenada Y do canto superior esquerdo
- `_screen`: Identificador da tela (0 = primeira tela)

**Nota:** Após criar o objeto, chame `setup()` antes de usá-lo.

### Destrutor

```cpp
~TextBox()
```

Libera os recursos do TextBox.

### setup()

```cpp
void setup(const TextBoxConfig& config)
```

Configura o TextBox. **Este método deve ser chamado após a criação do objeto.**

**Parâmetros:**
- `config`: Estrutura `TextBoxConfig` com as configurações

### getValue()

```cpp
const char* getValue()
```

Retorna o texto atual como string (const char*).

### setValue()

```cpp
void setValue(const char* str)
```

Define o texto exibido.

**Parâmetros:**
- `str`: Novo texto

**Características:**
- Texto é armazenado internamente como CharString
- Marca para redesenho
- Registra no log do ESP32

### show()

```cpp
void show()
```

Torna o TextBox visível.

### hide()

```cpp
void hide()
```

Oculta o TextBox.

---

## 🔒 Métodos Privados (Apenas para Referência)

Estes métodos são chamados internamente:

- `detectTouch()`: Detecta toque e abre Keyboard
- `redraw()`: Redesenha o TextBox
- `forceUpdate()`: Força atualização
- `getCallbackFunc()`: Retorna callback

---

## 💻 Como Usar no Arduino IDE

### 📚 Passo 1: Incluir as Bibliotecas

```cpp
#include <displayfk.h>
// Incluir outras bibliotecas necessárias
```

### 🏗️ Passo 2: Configurar Cores Globais do Keyboard

```cpp
DisplayFK myDisplay;

// Configurar cores do Keyboard (usado pelo TextBox)
Keyboard::m_backgroundColor = CFK_GREY3;
Keyboard::m_letterColor = CFK_BLACK;
Keyboard::m_keyColor = CFK_GREY13;
```

### 📝 Passo 3: Declarar Objetos Globais

```cpp
// Criar TextBox
TextBox textbox(110, 240, 0);

const uint8_t qtdTextbox = 1;
TextBox *arrayTextbox[qtdTextbox] = {&textbox};
```

### ⚙️ Passo 4: Configurar Display (setup)

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

### 🔧 Passo 5: Configurar o TextBox

```cpp
void loadWidgets() {
    // Configurar TextBox
    TextBoxConfig configTextBox = {
        .width = 101,
        .height = 25,
        .letterColor = CFK_COLOR01,
        .backgroundColor = CFK_WHITE,
        .startValue = "Digite aqui",
        .font = &RobotoRegular10pt7b,
        .funcPtr = screen0,
        .cb = textbox_cb
    };
    textbox.setup(configTextBox);
    
    // Registrar no DisplayFK
    myDisplay.setTextbox(arrayTextbox, qtdTextbox);
}
```

### 🔔 Passo 6: Criar Função de Callback

```cpp
void textbox_cb() {
    const char* texto = textbox.getValue();
    Serial.print("Novo texto: ");
    Serial.println(texto);
}
```

### 🖥️ Passo 7: Função da Tela

```cpp
void screen0() {
    tft->fillScreen(CFK_WHITE);
    WidgetBase::backgroundColor = CFK_WHITE;
    
    myDisplay.drawWidgetsOnScreen(0);
}
```

### 🔄 Passo 8: Exemplo de Uso

```cpp
void loop() {
    // Ler texto atual
    const char* textoAtual = textbox.getValue();
    
    // Alterar texto programaticamente
    textbox.setValue("Novo texto");
    
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

// Configurar cores do Keyboard
Keyboard::m_backgroundColor = CFK_GREY3;
Keyboard::m_letterColor = CFK_BLACK;
Keyboard::m_keyColor = CFK_GREY13;

// Criar TextBox
TextBox meuTexto(100, 100, 0);

const uint8_t qtdTextbox = 1;
TextBox *arrayTextbox[qtdTextbox] = {&meuTexto};

void setup() {
    Serial.begin(115200);
    
    // Inicializar display
    bus = new Arduino_ESP32SPI(/* ... */);
    gfx = new Arduino_ST7789(/* ... */);
    gfx->begin();
    
    myDisplay.setDrawObject(gfx);
    
    // Configurar TextBox
    TextBoxConfig config = {
        .width = 200,
        .height = 40,
        .letterColor = CFK_COLOR01,
        .backgroundColor = CFK_WHITE,
        .startValue = "Digite seu nome",
        .font = &RobotoRegular10pt7b,
        .funcPtr = minhaTela,
        .cb = texto_callback
    };
    meuTexto.setup(config);
    
    myDisplay.setTextbox(arrayTextbox, qtdTextbox);
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

void texto_callback() {
    Serial.print("Texto digitado: ");
    Serial.println(meuTexto.getValue());
}
```

---

## 📏 Dicas e Boas Práticas

### 📐 Tamanhos Recomendados
- **Pequenos:** 80x25 pixels
- **Médios:** 120x35 pixels
- **Grandes:** 180x45 pixels

### 🎨 Cores
- Use cores contrastantes para texto e fundo
- Fundo branco com texto escuro é comum
- Considere o tema claro/escuro
- Borda usa letterColor

### 🔔 Integração com Keyboard
- Keyboard abre automaticamente ao tocar no TextBox
- Configure cores globais do Keyboard antes de usar TextBox
- O Keyboard aplica o texto automaticamente ao fechar
- funcPtr deve apontar para função da tela pai

### ⚡ Performance
- Armazenamento interno eficiente com CharString
- Redesenho apenas quando necessário
- Integração otimizada com Keyboard
- Formatação inteligente do texto

### 👥 Usabilidade
- Tamanho adequado para fácil toque
- Texto exibido com formatação inteligente
- Preview no Keyboard mostra texto em tempo real
- Callback executa quando texto é confirmado

### 🎨 Display
- Retângulo com borda simples
- Texto centralizado verticalmente
- Formatação automática do texto
- Padding de 3 pixels por padrão

### 📝 Texto
- Suporta strings de qualquer tamanho
- Formatação automática para caber no espaço
- Conversão interna para CharString
- Validação pelo Keyboard

---

## 🔗 Herança de WidgetBase

A classe `TextBox` herda métodos de `WidgetBase`:

- `isEnabled()` / `setEnabled()`: Habilitar/desabilitar
- `isInitialized()`: Verificar configuração
- `m_visible`: Controla visibilidade
- `m_shouldRedraw`: Flag para redesenho

---

## 🔗 Integração com DisplayFK

O `TextBox` integra-se com o sistema DisplayFK:

1. **Keyboard:** Abre automaticamente ao tocar
2. **Callback:** Executado quando texto é confirmado
3. **Screen:** FuncPtr aponta para função da tela pai
4. **Gerenciamento:** Controlado pelo loop principal
5. **Bloqueio:** Outros widgets ficam bloqueados durante edição

---

## 🎨 Detalhes de Renderização

O TextBox renderiza assim:

1. **Fundo:**
   - Retângulo preenchido com backgroundColor
   - Borda desenhada com letterColor

2. **Texto:**
   - Texto formatado para caber no espaço
   - Centralizado verticalmente
   - Usa fonte configurada
   - Cor definida por letterColor

3. **Otimizações:**
   - Formatação inteligente do texto
   - Redesenho apenas quando necessário
   - Conversão otimizada para CharString

---

## 🔧 Solução de Problemas

### TextBox não aparece
- Verifique se chamou `setup()` após criar
- Confirme valores de width e height > 0
- Verifique se está na tela correta
- Chame `myDisplay.setTextbox()`

### Keyboard não abre
- Verifique se o usuário tocou no TextBox
- Confirme que funcPtr foi configurado corretamente
- Verifique logs para erros
- Teste com touch inicializado

### Texto não é aplicado
- Verifique se usuário pressionou OK no Keyboard
- Confirme que callback está configurado
- Verifique logs para erros
- Teste com texto simples

### Texto não mostra corretamente
- Verifique configuração de fonte
- Confirme letterColor contrasta com backgroundColor
- Teste com texto menor
- Verifique width e height adequados

### Problemas de formatação
- Textos longos podem ser truncados
- Use tamanho adequado para o texto
- Considere font size
- Teste diferentes textos

---

## 📚 Referências

- **Classe Base:** `WidgetBase` (src/widgets/widgetbase.h)
- **Keyboard:** `Keyboard` (src/widgets/textbox/wkeyboard.h)
- **DisplayFK Principal:** `DisplayFK` (src/displayfk.h)
- **Cores Padrão:** Definidas em displayfk.h
- **Fontes:** src/fonts/RobotoRegular/

