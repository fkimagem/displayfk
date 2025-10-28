# Documentação da Classe NumberBox

## Visão Geral

A classe `NumberBox` é um widget interativo que exibe e permite edição de valores numéricos (float) através de um teclado numérico virtual. Ele herda de `WidgetBase` e trabalha em conjunto com `Numpad` para fornecer uma interface completa de entrada numérica.

### Características Principais

- 🔢 Exibe valores numéricos (float)
- 📝 Permite edição via teclado virtual (Numpad)
- 🎨 Cores e fontes personalizáveis
- 📐 Dimensões configuráveis
- 🔔 Callback para mudanças de valor
- 👆 Suporte a toque para abrir teclado
- 💾 Valor armazenado como CharString
- 👁️ Pode ser mostrado/ocultado dinamicamente
- 🔗 Integra-se com Numpad automaticamente
- 🌓 Suporte a modo claro/escuro

---

## 📋 Estruturas de Configuração

### NumberBoxConfig

Estrutura que contém todos os parâmetros de configuração:

```cpp
struct NumberBoxConfig {
  uint16_t width;               // Largura da caixa em pixels
  uint16_t height;              // Altura da caixa em pixels
  uint16_t letterColor;         // Cor do texto
  uint16_t backgroundColor;     // Cor de fundo
  float startValue;             // Valor numérico inicial
  const GFXfont* font;          // Fonte para o texto
  functionLoadScreen_t funcPtr; // Ponteiro para função da tela pai
  functionCB_t callback;        // Função callback
};
```

---

## 🔧 Métodos Públicos

### Construtor

```cpp
NumberBox(uint16_t _x, uint16_t _y, uint8_t _screen)
NumberBox() // Construtor padrão
```

Cria um novo NumberBox.

**Parâmetros:**
- `_x`: Coordenada X do canto superior esquerdo
- `_y`: Coordenada Y do canto superior esquerdo
- `_screen`: Identificador da tela (0 = primeira tela)

**Nota:** Após criar o objeto, chame `setup()` antes de usá-lo.

### Destrutor

```cpp
~NumberBox()
```

Libera os recursos do NumberBox.

### setup()

```cpp
void setup(const NumberBoxConfig& config)
```

Configura o NumberBox. **Este método deve ser chamado após a criação do objeto.**

**Parâmetros:**
- `config`: Estrutura `NumberBoxConfig` com as configurações

### getValue()

```cpp
float getValue()
```

Retorna o valor numérico atual como float.

### getValueChar()

```cpp
const char* getValueChar()
```

Retorna o valor como string (const char*).

### setValue()

```cpp
void setValue(float str)
```

Define o valor numérico.

**Parâmetros:**
- `str`: Novo valor numérico

**Características:**
- Converte float para string internamente
- Marca para redesenho
- Registra no log do ESP32

### show()

```cpp
void show()
```

Torna o NumberBox visível.

### hide()

```cpp
void hide()
```

Oculta o NumberBox.

---

## 🔒 Métodos Privados (Apenas para Referência)

Estes métodos são chamados internamente:

- `detectTouch()`: Detecta toque e abre Numpad
- `redraw()`: Redesenha o NumberBox
- `forceUpdate()`: Força atualização
- `getCallbackFunc()`: Retorna callback
- `cleanupMemory()`: Limpa memória
- `convertoToString()`: Converte float para string

---

## 💻 Como Usar no Arduino IDE

### 📚 Passo 1: Incluir as Bibliotecas

```cpp
#include <displayfk.h>
// Incluir outras bibliotecas necessárias
```

### 🏗️ Passo 2: Configurar Cores Globais do Numpad

```cpp
DisplayFK myDisplay;

// Configurar cores do Numpad (usado pelo NumberBox)
Numpad::m_backgroundColor = CFK_GREY3;
Numpad::m_letterColor = CFK_BLACK;
Numpad::m_keyColor = CFK_GREY13;
```

### 📝 Passo 3: Declarar Objetos Globais

```cpp
// Criar NumberBox
NumberBox numberbox(110, 240, 0);

const uint8_t qtdNumberbox = 1;
NumberBox *arrayNumberbox[qtdNumberbox] = {&numberbox};
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

### 🔧 Passo 5: Configurar o NumberBox

```cpp
void loadWidgets() {
    // Configurar NumberBox
    NumberBoxConfig configNumberBox = {
        .width = 101,
        .height = 25,
        .letterColor = CFK_COLOR01,
        .backgroundColor = CFK_WHITE,
        .startValue = 1234.56,
        .font = &RobotoRegular10pt7b,
        .funcPtr = screen0,
        .callback = numberbox_cb
    };
    numberbox.setup(configNumberBox);
    
    // Registrar no DisplayFK
    myDisplay.setNumberbox(arrayNumberbox, qtdNumberbox);
}
```

### 🔔 Passo 6: Criar Funções de Callback

```cpp
void numberbox_cb() {
    float valor = numberbox.getValue();
    Serial.print("Novo valor digitado: ");
    Serial.println(valor);
    
    // Faça algo com o novo valor
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
    // O NumberBox funciona automaticamente
    // Quando o usuário toca nele, o Numpad abre
    
    // Você pode ler o valor atual:
    float valorAtual = numberbox.getValue();
    
    // Ou definir um valor programaticamente:
    numberbox.setValue(99.99);
    
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
        .width = 200,
        .height = 40,
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

### 📐 Tamanhos Recomendados
- **Pequenos:** 80x25 pixels
- **Médios:** 120x35 pixels
- **Grandes:** 180x45 pixels

### 🎨 Cores
- Use cores contrastantes para texto e fundo
- Fundo branco com texto escuro é comum
- Considere o tema claro/escuro
- Borda usa letterColor

### 🔔 Integração com Numpad
- Numpad abre automaticamente ao tocar no NumberBox
- Configure cores globais do Numpad antes de usar NumberBox
- O Numpad aplica o valor automaticamente ao fechar
- funcPtr deve apontar para função da tela pai

### ⚡ Performance
- Armazenamento interno eficiente com CharString
- Conversão automática entre float e string
- Redesenho apenas quando necessário
- Integração otimizada com Numpad

### 👥 Usabilidade
- Tamanho adequado para fácil toque
- Valor exibido com formatação inteligente
- Preview no Numpad mostra valor em tempo real
- Callback executa quando valor é confirmado

### 🎨 Display
- Retângulo com borda simples
- Texto centralizado verticalmente
- Formatação automática do texto
- Padding de 3 pixels por padrão

### 🔢 Valores Numéricos
- Suporta floats com casas decimais
- Pode exibir valores positivos e negativos
- Conversão automática para string
- Validação pelo Numpad

---

## 🔗 Herança de WidgetBase

A classe `NumberBox` herda métodos de `WidgetBase`:

- `isEnabled()` / `setEnabled()`: Habilitar/desabilitar
- `isInitialized()`: Verificar configuração
- `m_visible`: Controla visibilidade
- `m_shouldRedraw`: Flag para redesenho

---

## 🔗 Integração com DisplayFK

O `NumberBox` integra-se com o sistema DisplayFK:

1. **Numpad:** Abre automaticamente ao tocar
2. **Callback:** Executado quando valor é confirmado
3. **Screen:** FuncPtr aponta para função da tela pai
4. **Gerenciamento:** Controlado pelo loop principal
5. **Bloqueio:** Outros widgets ficam bloqueados durante edição

---

## 🎨 Detalhes de Renderização

O NumberBox renderiza assim:

1. **Fundo:**
   - Retângulo preenchido com backgroundColor
   - Borda desenhada com letterColor

2. **Texto:**
   - Valor formatado para caber no espaço
   - Centralizado verticalmente
   - Usa fonte configurada
   - Cor definida por letterColor

3. **Otimizações:**
   - Formatação inteligente do texto
   - Redesenho apenas quando necessário
   - Conversão otimizada float vá string

---

## 🔧 Solução de Problemas

### NumberBox não aparece
- Verifique se chamou `setup()` após criar
- Confirme valores de width e height > 0
- Verifique se está na tela correta
- Chame `myDisplay.setNumberbox()`

### Numpad não abre
- Verifique se o usuário tocou no NumberBox
- Confirme que funcPtr foi configurado corretamente
- Verifique logs para erros
- Teste com touch inicializado

### Valor não é aplicado
- Verifique se usuário pressionou OK no Numpad
- Confirme que callback está configurado
- Verifique logs para conversão
- Teste com valores simples

### Texto não mostra corretamente
- Verifique configuração de fonte
- Confirme letterColor contrasta com backgroundColor
- Teste com valores menores
- Verifique width e height adequados

### Problemas de formatação
- Valores grandes podem ser truncados
- Use tamanho adequado para o valor
- Considere font size
- Teste diferentes valores

---

## 📚 Referências

- **Classe Base:** `WidgetBase` (src/widgets/widgetbase.h)
- **Numpad:** `Numpad` (src/widgets/numberbox/numpad.h)
- **DisplayFK Principal:** `DisplayFK` (src/displayfk.h)
- **Cores Padrão:** Definidas em displayfk.h
- **Fontes:** src/fonts/RobotoRegular/

