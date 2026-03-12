# Documentação da Classe TouchArea

## Visão Geral

A classe `TouchArea` é um widget invisível que define uma área sensível ao toque na tela. Ele herda de `WidgetBase` e fornece funcionalidade para criar regiões retangulares que detectam toques e executam callbacks, mas não renderizam nenhuma aparência visual.

### Características Principais

- 👆 Área sensível ao toque invisível
- 🔔 Callback para ações de toque
- 📐 Dimensões configuráveis
- 🔒 Pode ser habilitado/desabilitado
- 👁️ Pode ser mostrado/ocultado dinamicamente
- 🎯 Detecção precisa de coordenadas
- 🔗 Integra-se automaticamente com o sistema DisplayFK

---

## 📋 Estruturas de Configuração

### TouchAreaConfig

Estrutura que contém todos os parâmetros de configuração (ordem conforme `wtoucharea.h`):

```cpp
struct TouchAreaConfig {
  functionCB_t callback;  // Callback executada quando a área é tocada.
  uint16_t width;         // Largura da área de toque em pixels.
  uint16_t height;        // Altura da área de toque em pixels.
};
```

---

## 🔧 Métodos Públicos

### Construtor

```cpp
TouchArea(uint16_t _x, uint16_t _y, uint8_t _screen)
```

Cria uma nova área de toque.

**Parâmetros:**
- `_x`: Coordenada X do canto superior esquerdo
- `_y`: Coordenada Y do canto superior esquerdo
- `_screen`: Identificador da tela (0 = primeira tela)

**Nota:** Após criar o objeto, chame `setup()` antes de usá-lo.

### Destrutor

```cpp
~TouchArea()
```

Libera os recursos do TouchArea.

### setup()

```cpp
void setup(const TouchAreaConfig& config)
```

Configura a área de toque. **Este método deve ser chamado após a criação do objeto.**

**Parâmetros:**
- `config`: Estrutura `TouchAreaConfig` com as configurações

### onClick()

```cpp
void onClick()
```

Simula um toque na área programaticamente (executa o callback).

### getStatus()

```cpp
bool getStatus()
```

Retorna o status atual da área de toque.

### show()

```cpp
void show()
```

Torna a área de toque ativa.

### hide()

```cpp
void hide()
```

Desativa a área de toque.

---

## 🔒 Métodos Privados (Apenas para Referência)

Estes métodos são chamados internamente:

- `detectTouch()`: Detecta toque na área
- `redraw()`: Não desenha nada (área invisível)
- `forceUpdate()`: Força atualização
- `getCallbackFunc()`: Retorna callback
- `cleanupMemory()`: Limpa memória
- `changeState()`: Não implementado

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

// Criar áreas de toque
TouchArea area1(50, 100, 0);
TouchArea area2(200, 100, 0);

const uint8_t qtdToucharea = 2;
TouchArea *arrayToucharea[qtdToucharea] = {&area1, &area2};
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

### 🔧 Passo 4: Configurar as Áreas de Toque

```cpp
void loadWidgets() {
    // Configurar Área 1
    TouchAreaConfig configArea1 = {
        .callback = area1_cb,
        .width = 100,
        .height = 80
    };
    area1.setup(configArea1);
    
    // Configurar Área 2
    TouchAreaConfig configArea2 = {
        .callback = area2_cb,
        .width = 120,
        .height = 100
    };
    area2.setup(configArea2);
    
    // Registrar no DisplayFK
    myDisplay.setToucharea(arrayToucharea, qtdToucharea);
}
```

### 🔔 Passo 5: Criar Funções de Callback

```cpp
void area1_cb() {
    Serial.println("Área 1 tocada!");
}

void area2_cb() {
    Serial.println("Área 2 tocada!");
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
    // Simular toque programaticamente
    area1.onClick();
    
    // Verificar status
    bool status = area1.getStatus();
    
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

// Criar áreas de toque
TouchArea areaEsquerda(0, 0, 0);
TouchArea areaDireita(200, 0, 0);
TouchArea areaCentral(100, 100, 0);

const uint8_t qtdToucharea = 3;
TouchArea *arrayToucharea[qtdToucharea] = {&areaEsquerda, &areaDireita, &areaCentral};

void setup() {
    Serial.begin(115200);
    
    // Inicializar display
    bus = new Arduino_ESP32SPI(/* ... */);
    gfx = new Arduino_ST7789(/* ... */);
    gfx->begin();
    
    myDisplay.setDrawObject(gfx);
    
    // Configurar Área Esquerda
    TouchAreaConfig configEsquerda = {
        .callback = area_esquerda_callback,
        .width = 200,
        .height = 240
    };
    areaEsquerda.setup(configEsquerda);
    
    // Configurar Área Direita
    TouchAreaConfig configDireita = {
        .callback = area_direita_callback,
        .width = 200,
        .height = 240
    };
    areaDireita.setup(configDireita);
    
    // Configurar Área Central
    TouchAreaConfig configCentral = {
        .callback = area_central_callback,
        .width = 200,
        .height = 100
    };
    areaCentral.setup(configCentral);
    
    myDisplay.setToucharea(arrayToucharea, qtdToucharea);
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

void area_esquerda_callback() {
    Serial.println("Lado esquerdo da tela tocado!");
}

void area_direita_callback() {
    Serial.println("Lado direito da tela tocado!");
}

void area_central_callback() {
    Serial.println("Área central tocada!");
}
```

---

## 📏 Dicas e Boas Práticas

### 📐 Tamanhos Recomendados
- **Pequenos:** 50x50 pixels
- **Médios:** 100x100 pixels
- **Grandes:** 200x200 pixels
- **Tela inteira:** 320x240 pixels (exemplo)

### 🎯 Posicionamento
- Use para criar botões invisíveis
- Defina áreas de navegação
- Crie zonas de toque para diferentes funções
- Posicione sobre outros elementos visuais

### 🔔 Callbacks
- Execute ações específicas por área
- Use para navegação entre telas
- Implemente gestos personalizados
- Crie interfaces invisíveis

### ⚡ Performance
- Detecção de toque eficiente
- Sem renderização visual
- Callbacks executados apenas no toque
- Estados internos otimizados

### 👥 Usabilidade
- Áreas grandes o suficiente para toque
- Posições lógicas na interface
- Feedback através de callbacks
- Integração com outros widgets

### 🎨 Visual
- Área completamente invisível
- Não interfere com outros elementos
- Pode ser posicionada sobre imagens
- Ideal para interfaces minimalistas

### 🔒 Controle
- Pode ser habilitado/desabilitado
- Toque programático disponível
- Integração com sistema de callbacks
- Estados persistentes

---

## 🔗 Herança de WidgetBase

A classe `TouchArea` herda métodos de `WidgetBase`:

- `isEnabled()` / `setEnabled()`: Habilitar/desabilitar
- `isInitialized()`: Verificar configuração
- `m_visible`: Controla visibilidade
- `m_shouldRedraw`: Flag para redesenho

---

## 🔗 Integração com DisplayFK

O `TouchArea` integra-se com o sistema DisplayFK:

1. **Detecção:** Automática com `detectTouch()`
2. **Callbacks:** Executados quando área é tocada
3. **Gerenciamento:** Controlado pelo loop principal
4. **Performance:** Detecção eficiente
5. **Estados:** Persistência entre ciclos

---

## 🎨 Detalhes de Renderização

O TouchArea **não renderiza nada visualmente**:

1. **Invisível:** Não desenha elementos na tela
2. **Transparente:** Permite que outros elementos sejam visíveis
3. **Área de Detecção:** Apenas define região retangular
4. **Sem Interferência:** Não afeta aparência visual

---

## 🔧 Solução de Problemas

### Área não detecta toque
- Verifique se chamou `setup()` após criar
- Confirme valores de width e height > 0
- Verifique se está na tela correta
- Chame `myDisplay.setToucharea()`

### Callback não executa
- Verifique se callback foi configurado corretamente
- Confirme que a área foi realmente tocada
- Verifique logs para erros
- Teste com `onClick()` programático

### Área muito pequena
- Aumente width e height
- Verifique posicionamento
- Teste com área maior
- Confirme coordenadas corretas

### Conflito com outros widgets
- Verifique sobreposição de áreas
- Use áreas menores se necessário
- Posicione estrategicamente
- Teste ordem de detecção

### Problemas de posicionamento
- Verifique coordenadas X e Y
- Confirme dimensões da tela
- Teste com valores conhecidos
- Verifique se está na tela correta

---

## 📚 Referências

- **Classe Base:** `WidgetBase` (src/widgets/widgetbase.h)
- **DisplayFK Principal:** `DisplayFK` (src/displayfk.h)
- **Cores Padrão:** Definidas em displayfk.h
- **Exemplos:** examples/Embed_ESP32S3/Display_Test/
