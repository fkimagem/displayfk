# Documentação da Classe Numpad

## Visão Geral

A classe `Numpad` é um widget interativo que exibe um teclado numérico na tela para entrada de números. Ele herda de `WidgetBase` e trabalha em conjunto com `NumberBox` para fornecer uma interface de entrada numérica completa.

### Características Principais

- 🔢 Teclado numérico 4x4 (16 teclas)
- 📱 Teclas para dígitos 0-9 e ponto decimal
- ➕➖ Operações matemáticas (incremento, decremento, inversão)
- ⌫ Controles de edição (Delete, OK)
- 📺 Área de preview do valor digitado
- 🎨 Cores e fontes personalizáveis
- 💾 Integrado com NumberBox
- 🔒 Proteção de entrada com validação
- 👁️ Pode ser mostrado/ocultado dinamicamente
- 🔗 Integra-se automaticamente com o sistema DisplayFK

---

## 📋 Estruturas de Configuração

### NumpadConfig

Estrutura que contém todos os parâmetros de configuração do teclado (ordem conforme `numpad.h`):

```cpp
struct NumpadConfig {
  #if defined(USING_GRAPHIC_LIB)
  const GFXfont* fontKeys;     // Fonte para as teclas.
  const GFXfont* fontPreview;   // Fonte para a área de preview.
  #endif
  uint16_t backgroundColor;    // Cor de fundo do Numpad.
  uint16_t letterColor;        // Cor do texto das teclas.
  uint16_t keyColor;           // Cor de fundo das teclas.
};
```

### Layout do Teclado

O teclado numérico tem o seguinte layout (4x4):

```
┌─────┬─────┬─────┬─────┐
│  7  │  8  │  9  │ +/- │
├─────┼─────┼─────┼─────┤
│  4  │  5  │  6  │ Del │
├─────┼─────┼─────┼─────┤
│  1  │  2  │  3  │  .  │
├─────┼─────┼─────┼─────┤
│  -- │  0  │  ++ │ OK  │
└─────┴─────┴─────┴─────┘
```

**Funções das teclas:**
- **0-9:** Inserir dígitos
- **.:** Inserir ponto decimal
- **+/-:** Inverter sinal do valor
- **Del:** Apagar último caractere
- **--:** Decrementar valor em 1
- **++:** Incrementar valor em 1
- **OK:** Confirmar e fechar teclado

---

## 🔧 Métodos Públicos

### Construtor

```cpp
Numpad(uint16_t _x, uint16_t _y, uint8_t _screen)
Numpad() // Construtor padrão (x=0, y=0, screen=0)
```

Cria um novo teclado numérico.

**Parâmetros:**
- `_x`: Coordenada X inicial
- `_y`: Coordenada Y inicial
- `_screen`: Identificador da tela (0 = primeira tela)

**Nota:** Após criar o objeto, chame `setup()` antes de usá-lo.

### Destrutor

```cpp
~Numpad()
```

Libera os recursos do Numpad.

### setup()

```cpp
bool setup()
void setup(const NumpadConfig& config)
```

Configura o teclado. A versão sem parâmetros usa configuração padrão e retorna sucesso/falha.

**Parâmetros:**
- `config`: Estrutura `NumpadConfig` com as configurações

### open()

```cpp
void open(NumberBox *_field)
```

Abre o teclado numérico para editar um NumberBox específico.

**Parâmetros:**
- `_field`: Ponteiro para o NumberBox que será editado

### close()

```cpp
void close()
```

Fecha o teclado e aplica o valor digitado ao NumberBox.

### show()

```cpp
void show()
```

Torna o teclado visível.

### hide()

```cpp
void hide()
```

Oculta o teclado.

---

## 🔒 Métodos Privados (Apenas para Referência)

Estes métodos existem na classe e são usados internamente:

- `detectTouch()`: Detecta toque nas teclas
- `detectTouch(..., PressedKeyType*)`: Versão usada para identificar tipo de tecla pressionada
- `drawKeys()`: Desenha o teclado
- `redraw()`: Redesenha o teclado
- `forceUpdate()`: Força atualização
- `addLetter()`: Adiciona caractere
- `removeLetter()`: Remove último caractere
- `cleanupMemory()`: Limpa memória

---

## 💻 Como Usar no Arduino IDE

### 📚 Passo 1: Incluir as Bibliotecas

```cpp
#include <displayfk.h>
// Incluir outras bibliotecas necessárias
```

### 🏗️ Passo 2: Configurar Cores Globais (Opcional)

```cpp
DisplayFK myDisplay;

// Configurar cores globais do Numpad
Numpad::m_backgroundColor = CFK_GREY3;
Numpad::m_letterColor = CFK_BLACK;
Numpad::m_keyColor = CFK_GREY13;
```

### 📝 Passo 3: Configurar NumberBox

```cpp
// Criar NumberBox
NumberBox numberbox(110, 240, 0);
const uint8_t qtdNumberbox = 1;
NumberBox *arrayNumberbox[qtdNumberbox] = {&numberbox};
```

### ⚙️ Passo 4: Configurar Display (setup)

```cpp
void setup() {
    // ... configuração do display ...
    myDisplay.setDrawObject(tft);
    
    loadWidgets();
    myDisplay.loadScreen(screen0);
    myDisplay.createTask(false, 3);
}
```

### 🔧 Passo 5: Configurar o Numpad

```cpp
void loadWidgets() {
    // Configurar NumberBox (associado ao Numpad)
    NumberBoxConfig configNumberBox = {
        .width = 101,
        .height = 25,
        .letterColor = CFK_COLOR01,
        .backgroundColor = CFK_WHITE,
        .startValue = 0.0,
        .font = &RobotoRegular10pt7b,
        .funcPtr = screen0,
        .callback = numberbox_cb
    };
    numberbox.setup(configNumberBox);
    
    // Registrar NumberBox
    myDisplay.setNumberbox(arrayNumberbox, qtdNumberbox);
    
    // Numpad é configurado automaticamente pelo sistema
}
```

### 🔔 Passo 6: Função Callback do NumberBox

```cpp
void numberbox_cb() {
    float valor = numberbox.getValue();
    Serial.print("Novo valor: ");
    Serial.println(valor);
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
    // O Numpad abre automaticamente quando o usuário toca no NumberBox
    // Não é necessário código adicional no loop
}
```

---

## 📝 Exemplo Completo

```cpp
#include <displayfk.h>

DisplayFK myDisplay;
Arduino_DataBus *bus = nullptr;
Arduino_GFX *gfx = nullptr;

// Configurar cores do Numpad
Numpad::m_backgroundColor = CFK_GREY3;
Numpad::m_letterColor = CFK_BLACK;
Numpad::m_keyColor = CFK_GREY13;

// Criar NumberBox
NumberBox meuNumero(100, 100, 0);

const uint8_t qtdNumberbox = 1;
NumberBox *arrayNumberbox[qtdNumberbox] = {&meuNumero};

void setup() {
    Serial.begin(115200);
    
    // Inicializar display
    bus = new Arduino_ESP32SPI(/* ... */);
    gfx = new Arduino_ST7789(/* ... */);
    gfx->begin();
    
    myDisplay.setDrawObject(gfx);
    
    // Configurar NumberBox
    NumberBoxConfig config = {
        .width = 150,
        .height = 30,
        .letterColor = CFK_COLOR01,
        .backgroundColor = CFK_WHITE,
        .startValue = 0.0,
        .font = &RobotoRegular10pt7b,
        .funcPtr = minhaTela,
        .callback = numero_callback
    };
    meuNumero.setup(config);
    
    myDisplay.setNumberbox(arrayNumberbox, qtdNumberbox);
    myDisplay.loadScreen(minhaTela);
    myDisplay.createTask(false, 3);
}

void loop() {
    // Seu código aqui
    // Numpad abre automaticamente ao tocar no NumberBox
}

void minhaTela() {
    gfx->fillScreen(CFK_WHITE);
    WidgetBase::backgroundColor = CFK_WHITE;
    myDisplay.drawWidgetsOnScreen(0);
}

void numero_callback() {
    Serial.print("Valor digitado: ");
    Serial.println(meuNumero.getValue());
}
```

---

## 📏 Dicas e Boas Práticas

### 📐 Tamanhos
- O Numpad ocupa 90% da largura da tela
- Ocupa 75% da altura da tela
- Teclas são dimensionadas automaticamente
- Layout responsivo baseado no tamanho da tela

### 🎨 Cores
- Use cores contrastantes para teclas e texto
- Fundo escuro com teclas claras é comum
- Considere o tema claro/escuro da interface
- Cores estáticas globais para consistência

### 🔔 Integração
- Numpad trabalha automaticamente com NumberBox
- Abre quando o usuário toca em um NumberBox
- Fecha quando o usuário pressiona OK
- Valor é aplicado automaticamente ao NumberBox
- Tecla `ESC` restaura o conteúdo anterior antes de fechar

### ⚡ Performance
- Teclado é desenhado apenas quando aberto
- Preview atualizado em tempo real
- Eficiente para entrada de dados
- Redesenho apenas do necessário

### 👥 Usabilidade
- Teclas grandes facilitam o toque
- Preview mostra valor em tempo real
- Operações matemáticas rápidas
- Feedback visual ao tocar em teclas
- Layout familiar (calculadora)

### 🎨 Funcionalidades Especiais
- **+/-:** Inverte sinal rapidamente
- **--/++:** Decrementa/incrementa em 1
- **Del:** Apaga caractere a caractere
- **OK:** Confirma entrada
- Preview em tempo real
- Validação de entrada numérica

---

## 🔗 Herança de WidgetBase

A classe `Numpad` herda métodos de `WidgetBase`:

- `isEnabled()` / `setEnabled()`: Habilitar/desabilitar widget
- `isInitialized()`: Verificar se foi configurado
- `m_visible`: Controla visibilidade interna
- `m_shouldRedraw`: Flag para redesenho automático

---

## 🔗 Integração com DisplayFK

O `Numpad` integra-se com o sistema DisplayFK:

1. **Abertura Automática:** Quando o usuário toca em um NumberBox
2. **Bloqueio:** Outros widgets ficam bloqueados quando o Numpad está aberto
3. **Gerenciamento:** Controlado pelo loop principal do DisplayFK
4. **Aplicação:** Valor aplicado automaticamente ao NumberBox ao fechar
5. **Proteção:** usingKeyboard ativa durante uso

---

## 🎨 Detalhes de Renderização

O Numpad renderiza em camadas:

1. **Fundo:**
   - Tela preenchida com cor de fundo
   - Área principal do teclado

2. **Preview:**
   - Área retangular no topo
   - Exibe valor atual sendo digitado
   - Atualizado em tempo real

3. **Teclas:**
   - Grade 4x4 de teclas
   - Cada tecla com cor de fundo
   - Texto centralizado em cada tecla
   - Espaçamento automático

---

## 🔧 Solução de Problemas

### Numpad não abre
- Verifique se o NumberBox está configurado
- Confirme que o usuário tocou no NumberBox
- Verifique se funcPtr foi configurado
- Certifique-se que DisplayFK foi inicializado

### Teclas não respondem
- Verifique se o touch foi inicializado
- Confirme que o teclado está aberto (usingKeyboard)
- Verifique debounce settings
- Teste em área que não seja das teclas

### Valor não é aplicado
- Verifique se OK foi pressionado
- Confirme que o NumberBox tem callback configurado
- Verifique log para erros de conversão
- Teste diferentes valores de entrada
- Verifique se o `Numpad` está associado a um `NumberBox` válido (`open(field)`)

### Preview não mostra valor
- Verifique configuração de fonte do preview
- Confirme que m_content está sendo atualizado
- Verifique se m_fontPreview foi configurado
- Teste com valores simples primeiro

---

## 📚 Referências

- **Classe Base:** `WidgetBase` (src/widgets/widgetbase.h)
- **NumberBox:** `NumberBox` (src/widgets/numberbox/wnumberbox.h)
- **DisplayFK Principal:** `DisplayFK` (src/displayfk.h)
- **Cores Padrão:** CFK_GREY3, CFK_BLACK, CFK_GREY13

