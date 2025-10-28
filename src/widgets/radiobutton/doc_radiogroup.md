# Documentação da Classe RadioGroup

## Visão Geral

A classe `RadioGroup` é um widget interativo que exibe um grupo de botões de rádio onde apenas um pode estar selecionado por vez. Ele herda de `WidgetBase` e fornece funcionalidade completa para criar e gerenciar grupos de seleção única na tela.

### Características Principais

- 🔘 Grupo de botões de rádio circulares
- ✅ Seleção única (apenas um botão ativo por vez)
- 🎨 Cores personalizáveis para cada botão
- 📐 Raio configurável para todos os botões
- 🔔 Callback para mudanças de seleção
- 👆 Suporte a toque para seleção
- 💾 Gerenciamento automático de memória
- 👁️ Pode ser mostrado/ocultado dinamicamente
- 🌓 Suporte a modo claro/escuro
- 🔗 Integra-se automaticamente com o sistema DisplayFK

---

## 📋 Estruturas de Configuração

### radio_t

Estrutura que representa um botão de rádio individual:

```cpp
struct radio_t {
  uint16_t x;     // Coordenada X do botão
  uint16_t y;     // Coordenada Y do botão
  uint8_t id;     // ID único do botão no grupo
  uint16_t color; // Cor do botão quando selecionado
};
```

### RadioGroupConfig

Estrutura que contém todos os parâmetros de configuração do grupo:

```cpp
struct RadioGroupConfig {
  uint8_t group;               // ID do grupo
  uint16_t radius;             // Raio de todos os botões
  uint8_t amount;              // Número de botões no grupo
  const radio_t* buttons;      // Array de botões
  uint8_t defaultClickedId;    // ID do botão selecionado por padrão
  functionCB_t callback;       // Função callback
};
```

---

## 🔧 Métodos Públicos

### Construtor

```cpp
RadioGroup(uint8_t _screen)
```

Cria um novo grupo de botões de rádio.

**Parâmetros:**
- `_screen`: Identificador da tela (0 = primeira tela)

**Nota:** Após criar o objeto, chame `setup()` antes de usá-lo.

### Destrutor

```cpp
~RadioGroup()
```

Libera automaticamente a memória alocada para os botões.

### setup()

```cpp
void setup(const RadioGroupConfig& config)
```

Configura o grupo de botões de rádio. **Este método deve ser chamado após a criação do objeto.**

**Parâmetros:**
- `config`: Estrutura `RadioGroupConfig` com as configurações

### getSelected()

```cpp
uint16_t getSelected()
```

Retorna o ID do botão atualmente selecionado.

### setSelected()

```cpp
void setSelected(uint16_t clickedId)
```

Define qual botão está selecionado programaticamente.

**Parâmetros:**
- `clickedId`: ID do botão a selecionar

### getGroupId()

```cpp
uint16_t getGroupId()
```

Retorna o ID do grupo.

### show()

```cpp
void show()
```

Torna o grupo de botões visível.

### hide()

```cpp
void hide()
```

Oculta o grupo de botões.

---

## 🔒 Métodos Privados (Apenas para Referência)

Estes métodos são chamados internamente:

- `detectTouch()`: Detecta toque nos botões
- `redraw()`: Redesenha os botões
- `forceUpdate()`: Força atualização
- `getCallbackFunc()`: Retorna callback
- `cleanupMemory()`: Limpa memória alocada

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

// Definir botões do grupo
radio_t opcoes[] = {
    {50, 100, 1, CFK_COLOR01},  // Opção 1
    {50, 150, 2, CFK_COLOR02},  // Opção 2
    {50, 200, 3, CFK_COLOR03}   // Opção 3
};

// Criar RadioGroup
RadioGroup radiogroup(0);

const uint8_t qtdRadiogroup = 1;
RadioGroup *arrayRadiogroup[qtdRadiogroup] = {&radiogroup};
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

### 🔧 Passo 4: Configurar o RadioGroup

```cpp
void loadWidgets() {
    // Configurar RadioGroup
    RadioGroupConfig configRadioGroup = {
        .group = 1,
        .radius = 15,
        .amount = 3,
        .buttons = opcoes,
        .defaultClickedId = 1,
        .callback = radiogroup_cb
    };
    radiogroup.setup(configRadioGroup);
    
    // Registrar no DisplayFK
    myDisplay.setRadiogroup(arrayRadiogroup, qtdRadiogroup);
}
```

### 🔔 Passo 5: Criar Função de Callback

```cpp
void radiogroup_cb() {
    uint16_t selecionado = radiogroup.getSelected();
    Serial.print("Opção selecionada: ");
    Serial.println(selecionado);
    
    // Faça algo baseado na seleção
    switch(selecionado) {
        case 1:
            Serial.println("Opção 1 selecionada");
            break;
        case 2:
            Serial.println("Opção 2 selecionada");
            break;
        case 3:
            Serial.println("Opção 3 selecionada");
            break;
    }
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
    // Ler seleção atual
    uint16_t opcaoAtual = radiogroup.getSelected();
    
    // Alterar seleção programaticamente
    radiogroup.setSelected(2);
    
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

// Definir opções do grupo
radio_t opcoesTemperatura[] = {
    {80, 100, 1, CFK_RED},      // Quente
    {80, 150, 2, CFK_YELLOW},   // Morno
    {80, 200, 3, CFK_BLUE}      // Frio
};

// Criar RadioGroup
RadioGroup grupoTemperatura(0);

const uint8_t qtdRadiogroup = 1;
RadioGroup *arrayRadiogroup[qtdRadiogroup] = {&grupoTemperatura};

void setup() {
    Serial.begin(115200);
    
    // Inicializar display
    bus = new Arduino_ESP32SPI(/* ... */);
    gfx = new Arduino_ST7789(/* ... */);
    gfx->begin();
    
    myDisplay.setDrawObject(gfx);
    
    // Configurar RadioGroup
    RadioGroupConfig config = {
        .group = 1,
        .radius = 20,
        .amount = 3,
        .buttons = opcoesTemperatura,
        .defaultClickedId = 2,
        .callback = temperatura_callback
    };
    grupoTemperatura.setup(config);
    
    myDisplay.setRadiogroup(arrayRadiogroup, qtdRadiogroup);
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

void temperatura_callback() {
    uint16_t temp = grupoTemperatura.getSelected();
    Serial.print("Temperatura selecionada: ");
    Serial.println(temp);
}
```

---

## 📏 Dicas e Boas Práticas

### 📐 Tamanhos Recomendados
- **Pequenos:** Raio 10-15 pixels
- **Médios:** Raio 15-25 pixels
- **Grandes:** Raio 25-35 pixels

### 🎨 Cores
- Use cores diferentes para cada botão
- Considere o tema claro/escuro
- Botão selecionado usa a cor definida
- Botões não selecionados usam cor de fundo

### 🔔 Layout
- Espaçamento adequado entre botões
- Alinhamento vertical ou horizontal
- Posições claras e organizadas
- Fácil acesso por toque

### ⚡ Performance
- Gerenciamento eficiente de memória
- Redesenho apenas quando necessário
- Detecção de toque otimizada
- Callback executado apenas na mudança

### 👥 Usabilidade
- Botões grandes o suficiente para toque
- Feedback visual claro da seleção
- Cores distintivas para cada opção
- Posições intuitivas

### 🎨 Visual
- Círculos com bordas definidas
- Botão selecionado com círculo interno
- Cores adaptadas ao modo claro/escuro
- Renderização suave e clara

### 🔢 IDs e Grupos
- IDs únicos dentro do grupo
- Grupos separados para diferentes funcionalidades
- ID padrão sensato
- Validação de IDs válidos

---

## 🔗 Herança de WidgetBase

A classe `RadioGroup` herda métodos de `WidgetBase`:

- `isEnabled()` / `setEnabled()`: Habilitar/desabilitar
- `isInitialized()`: Verificar configuração
- `m_visible`: Controla visibilidade
- `m_shouldRedraw`: Flag para redesenho

---

## 🔗 Integração com DisplayFK

O `RadioGroup` integra-se com o sistema DisplayFK:

1. **Renderização:** Automática com `drawWidgetsOnScreen()`
2. **Callbacks:** Executados quando seleção muda
3. **Gerenciamento:** Controlado pelo loop principal
4. **Performance:** Redesenho eficiente
5. **Grupos:** Suporte a múltiplos grupos independentes

---

## 🎨 Detalhes de Renderização

O RadioGroup renderiza assim:

1. **Botões Não Selecionados:**
   - Círculo de fundo com cor de fundo
   - Borda com cor de contraste
   - Sem círculo interno

2. **Botão Selecionado:**
   - Círculo de fundo com cor de fundo
   - Borda com cor de contraste
   - Círculo interno com cor específica do botão
   - Círculo interno com borda

3. **Adaptação ao Tema:**
   - Cores ajustadas automaticamente
   - Modo claro/escuro suportado
   - Contraste adequado

---

## 🔧 Solução de Problemas

### Botões não aparecem
- Verifique se chamou `setup()` após criar
- Confirme que amount > 0
- Verifique se array de botões foi definido
- Chame `myDisplay.setRadiogroup()`

### Toque não funciona
- Verifique se o usuário tocou dentro do raio
- Confirme que o RadioGroup está visível
- Verifique logs para erros
- Teste com botões maiores

### Callback não executa
- Verifique se callback foi configurado
- Confirme que seleção realmente mudou
- Verifique logs para erros
- Teste com seleção programática

### Botões não mudam visualmente
- Verifique se m_shouldRedraw está sendo definido
- Confirme que redraw() está sendo chamado
- Verifique cores configuradas
- Teste com cores mais contrastantes

### Problemas de memória
- Verifique se cleanupMemory() está sendo chamado
- Confirme que array foi alocado corretamente
- Verifique logs para erros de alocação
- Teste com menos botões primeiro

---

## 📚 Referências

- **Classe Base:** `WidgetBase` (src/widgets/widgetbase.h)
- **DisplayFK Principal:** `DisplayFK` (src/displayfk.h)
- **Cores Padrão:** Definidas em displayfk.h
- **Exemplos:** examples/Embed_ESP32S3/Display_Test/

