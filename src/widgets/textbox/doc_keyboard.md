# Documentação da Classe Keyboard

## Visão Geral

A classe `Keyboard` (também conhecida como `WKeyboard`) é um widget interativo que exibe um teclado virtual completo (QWERTY) na tela para entrada de texto. Ele herda de `WidgetBase` e trabalha em conjunto com `TextBox` para fornecer uma interface de entrada de texto completa.

### Características Principais

- ⌨️ Teclado QWERTY completo 5x10 (50 teclas)
- 🔤 Letras minúsculas e maiúsculas
- 🔢 Números e símbolos
- 🔒 Caps Lock para alternar maiúsculas/minúsculas
- ⌫ Delete para apagar caracteres
- 🗑️ Clear para limpar todo o texto
- ✅ OK para confirmar e fechar
- 📺 Área de preview do texto digitado
- 🎨 Cores personalizáveis
- 💾 Integrado com TextBox
- 🔒 Proteção com validação de entrada
- 👁️ Pode ser mostrado/ocultado dinamicamente
- 🔗 Integra-se automaticamente com o sistema DisplayFK

---

## 📋 Estruturas e Enums

### KeyboardType Enum

Enum para especificar tipos de entrada do teclado:

```cpp
enum class KeyboardType {
  NONE = 0,         // Nenhum tipo específico
  ALPHANUMERIC = 1, // Entrada alfanumérica
  INTEGER = 2,      // Entrada inteira
  DECIMAL = 3       // Entrada decimal
};
```

### Layout do Teclado

O teclado tem dois layouts (minúsculas e maiúsculas), com 5 linhas e 10 colunas:

**Layout Minúsculas:**
```
┌───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┐
│   1   │   2   │   3   │   4   │   5   │   6   │   7   │   8   │   9   │   0   │
├───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┤
│   q   │   w   │   e   │   r   │   t   │   y   │   u   │   i   │   o   │   p   │
├───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┤
│   a   │   s   │   d   │   f   │   g   │   h   │   j   │   k   │   l   │  DEL  │
├───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┤
│   z   │   x   │   c   │   v   │   b   │   n   │   m   │       │   ø   │  CAP  │
├───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┤
│   :   │   ,   │   +   │   -   │   *   │   /   │   =   │   .   │  CLR  │  OK   │
└───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┘
```

**Layout Maiúsculas:**
```
┌───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┐
│   !   │   @   │   #   │   $   │   %   │   ^   │   &   │   ?   │   (   │   )   │
├───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┤
│   Q   │   W   │   E   │   R   │   T   │   Y   │   U   │   I   │   O   │   P   │
├───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┤
│   A   │   S   │   D   │   F   │   G   │   H   │   J   │   K   │   L   │  DEL  │
├───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┤
│   Z   │   X   │   C   │   V   │   B   │   N   │   M   │   {   │   }   │  CAP  │
├───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┤
│   _   │   '   │   :   │   <   │   >   │   ;   │   ~   │   [   │   ]   │  OK   │
└───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┘
```

**Funções das teclas:**
- **Letras (a-z/A-Z):** Inserir letras
- **Números (0-9):** Inserir dígitos
- **Símbolos:** Inserir caracteres especiais
- **CAP:** Alternar Caps Lock (maiúsculas/minúsculas)
- **DEL:** Apagar último caractere
- **CLR:** Limpar todo o texto
- **OK:** Confirmar e fechar teclado

---

## 🔧 Métodos Públicos

### Construtor

```cpp
Keyboard(uint16_t _x, uint16_t _y, uint8_t _screen)
Keyboard() // Construtor padrão
```

Cria um novo teclado virtual.

**Parâmetros:**
- `_x`: Coordenada X inicial (geralmente 0)
- `_y`: Coordenada Y inicial (geralmente 0)
- `_screen`: Identificador da tela (0 = primeira tela)

**Nota:** Após criar o objeto, chame `setup()` antes de usá-lo.

### Destrutor

```cpp
~Keyboard()
```

Libera os recursos do teclado.

### setup()

```cpp
bool setup()
```

Configura o teclado com valores padrão. **Este método deve ser chamado após a criação do objeto.**

### open()

```cpp
void open(TextBox *_field)
```

Abre o teclado para editar um TextBox específico.

**Parâmetros:**
- `_field`: Ponteiro para o TextBox que será editado

### close()

```cpp
void close()
```

Fecha o teclado e aplica o texto digitado ao TextBox.

### clear()

```cpp
void clear()
```

Limpa todo o conteúdo digitado.

### insertChar()

```cpp
void insertChar(char c)
```

Insere um caractere no texto atual.

**Parâmetros:**
- `c`: Caractere a inserir

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

Estes métodos são chamados internamente:

- `detectTouch()`: Detecta toque nas teclas
- `drawKeys()`: Desenha o teclado
- `redraw()`: Redesenha o teclado
- `forceUpdate()`: Força atualização
- `addLetter()`: Adiciona caractere ao texto
- `removeLetter()`: Remove último caractere

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

// Configurar cores globais do Keyboard
Keyboard::m_backgroundColor = CFK_GREY3;
Keyboard::m_letterColor = CFK_BLACK;
Keyboard::m_keyColor = CFK_GREY13;
```

### 📝 Passo 3: Configurar TextBox

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

### 🔧 Passo 5: Configurar o Keyboard

```cpp
void loadWidgets() {
    // Configurar TextBox (associado ao Keyboard)
    TextBoxConfig configTextBox = {
        .width = 101,
        .height = 25,
        .letterColor = CFK_COLOR01,
        .backgroundColor = CFK_WHITE,
        .font = &RobotoRegular10pt7b,
        .funcPtr = screen0,
        .callback = textbox_cb
    };
    textbox.setup(configTextBox);
    
    // Registrar TextBox
    myDisplay.setTextbox(arrayTextbox, qtdTextbox);
    
    // Keyboard é configurado automaticamente pelo sistema
}
```

### 🔔 Passo 6: Função Callback do TextBox

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
    // O Keyboard abre automaticamente quando o usuário toca no TextBox
    // Não é necessário código adicional no loop
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
        .font = &RobotoRegular10pt7b,
        .funcPtr = minhaTela,
        .callback = texto_callback
    };
    meuTexto.setup(config);
    
    myDisplay.setTextbox(arrayTextbox, qtdTextbox);
    myDisplay.loadScreen(minhaTela);
    myDisplay.createTask(false, 3);
}

void loop() {
    // Seu código aqui
    // Keyboard abre automaticamente ao tocar no TextBox
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

### 📐 Tamanhos
- O Keyboard ocupa toda a tela quando aberto
- Teclas são dimensionadas automaticamente
- Layout responsivo baseado no tamanho da tela
- Preview no topo mostra texto digitado

### 🎨 Cores
- Use cores contrastantes para teclas e texto
- Fundo escuro com teclas claras é comum
- Considere o tema claro/escuro da interface
- Cores estáticas globais para consistência

### 🔔 Integração
- Keyboard trabalha automaticamente com TextBox
- Abre quando o usuário toca em um TextBox
- Fecha quando o usuário pressiona OK
- Texto é aplicado automaticamente ao TextBox

### ⚡ Performance
- Teclado é desenhado apenas quando aberto
- Preview atualizado em tempo real
- Redesenho otimizado do layout
- Eficiente para entrada de texto

### 👥 Usabilidade
- Teclas grandes facilitam o toque
- Preview mostra texto em tempo real
- Caps Lock para alternar maiúsculas/minúsculas
- CLR limpa tudo rapidamente
- DEL apaga um caractere por vez
- Feedback visual ao tocar em teclas
- Layout QWERTY familiar

### 🎨 Funcionalidades Especiais
- **CAP:** Toggle caps lock para maiúsculas
- **DEL:** Apaga último caractere
- **CLR:** Limpa todo o texto
- **OK:** Confirma entrada
- Preview em tempo real
- Layout minúsculas/maiúsculas completo

---

## 🔗 Herança de WidgetBase

A classe `Keyboard` herda métodos de `WidgetBase`:

- `isEnabled()` / `setEnabled()`: Habilitar/desabilitar widget
- `isInitialized()`: Verificar se foi configurado
- `m_visible`: Controla visibilidade interna
- `m_shouldRedraw`: Flag para redesenho automático

---

## 🔗 Integração com DisplayFK

O `Keyboard` integra-se com o sistema DisplayFK:

1. **Abertura Automática:** Quando o usuário toca em um TextBox
2. **Bloqueio:** Outros widgets ficam bloqueados quando o Keyboard está aberto
3. **Gerenciamento:** Controlado pelo loop principal do DisplayFK
4. **Aplicação:** Texto aplicado automaticamente ao TextBox ao fechar
5. **Proteção:** usingKeyboard ativa durante uso

---

## 🎨 Detalhes de Renderização

O Keyboard renderiza em camadas:

1. **Fundo:**
   - Tela preenchida com cor de fundo
   - Área principal do teclado

2. **Preview:**
   - Área retangular no topo
   - Exibe texto atual sendo digitado
   - Atualizado em tempo real

3. **Teclas:**
   - Grade 5x10 de teclas
   - Cada tecla com cor de fundo
   - Texto centralizado em cada tecla
   - Espaçamento automático
   - Layout muda com Caps Lock

---

## 🔧 Solução de Problemas

### Keyboard não abre
- Verifique se o TextBox está configurado
- Confirme que o usuário tocou no TextBox
- Verifique se funcPtr foi configurado
- Certifique-se que DisplayFK foi inicializado

### Teclas não respondem
- Verifique se o touch foi inicializado
- Confirme que o teclado está aberto (usingKeyboard)
- Verifique debounce settings
- Teste em área que não seja das teclas

### Texto não é aplicado
- Verifique se OK foi pressionado
- Confirme que o TextBox tem callback configurado
- Verifique log para erros
- Teste com texto simples primeiro

### Preview não mostra texto
- Verifique configuração de fonte do preview
- Confirme que m_content está sendo atualizado
- Verifique se m_fontPreview foi configurado
- Teste com caracteres simples primeiro

### Caps Lock não funciona
- Verifique se a tecla CAP foi tocada corretamente
- Confirme que m_capsLock está alternando
- Verifique logs para erros
- Teste o layout maiúsculas/minúsculas

---

## 📚 Referências

- **Classe Base:** `WidgetBase` (src/widgets/widgetbase.h)
- **TextBox:** `TextBox` (src/widgets/textbox/wtextbox.h)
- **DisplayFK Principal:** `DisplayFK` (src/displayfk.h)
- **Cores Padrão:** CFK_GREY3, CFK_BLACK, CFK_GREY13

