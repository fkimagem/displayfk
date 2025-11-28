# Guia de Implementação: Recurso `onRelease` para Widgets

## Objetivo

Implementar um sistema que permite rastrear widgets que receberam touch e executar uma função `onRelease()` quando:
- O touch é solto (`TOUCH_UP`)
- O touch sai da área de reconhecimento do widget durante `TOUCH_HOLD`

## Requisitos

- Não deve afetar o funcionamento atual do sistema
- Deve suportar até 5 widgets simultaneamente com touch ativo
- Deve ser compatível com todos os tipos de widgets existentes
- Deve manter retrocompatibilidade (widgets sem `onRelease()` implementado continuam funcionando)
- **NOVO**: Adicionar variável `m_isPressed` em `WidgetBase` para rastrear estado de pressão
- **NOVO**: `m_isPressed` deve ser `true` quando `detectTouch()` retorna `true`
- **NOVO**: `m_isPressed` deve ser `false` quando `onRelease()` é chamado
- **NOVO**: Widgets que sobrescrevem `onRelease()` devem também definir `m_shouldRedraw = true`
- **NOVO**: Widgets podem usar `m_isPressed` durante `redraw()` para alterar aparência visual

---

## Análise da Arquitetura Atual

### Estado Atual do Sistema de Touch

1. **Estados de Touch** (`TouchEventType`):
   - `NONE`: Sem touch
   - `TOUCH_DOWN`: Touch iniciado
   - `TOUCH_HOLD`: Touch mantido
   - `TOUCH_UP`: Touch solto

2. **Fluxo Atual**:
   - `processTouchStatus()` gerencia estados de touch
   - `processTouchableWidgets()` detecta widgets tocados
   - Cada função `processXXXTouch()` retorna após encontrar o primeiro widget
   - Callbacks são adicionados à fila quando widget é detectado

3. **Limitação Identificada**:
   - Sistema não rastreia quais widgets estão com touch ativo
   - Não há mecanismo para detectar quando touch sai da área do widget
   - Não há notificação quando touch é solto
   - Não há variável para rastrear estado de pressão (`m_isPressed`)
   - Widgets não podem alterar aparência visual baseada em estado de pressão

---

## Fluxo de Funcionamento de `m_isPressed` e `m_shouldRedraw`

### Ciclo Completo de Touch

1. **Touch Iniciado (`TOUCH_DOWN`)**:
   - `processTouchableWidgets()` detecta widget tocado
   - `detectTouch()` retorna `true`
   - Widget chama `setPressed(true)` OU define `m_isPressed = true` diretamente
   - Widget pode definir `m_shouldRedraw = true` para feedback visual imediato
   - Widget é adicionado ao array `m_touchedWidgets[]`

2. **Touch Mantido (`TOUCH_HOLD`)**:
   - `checkWidgetsStillTouched()` verifica se touch ainda está dentro dos bounds
   - Se touch sair da área, `removeTouchedWidget()` é chamado
   - `onRelease()` é chamado, definindo `m_isPressed = false`
   - Se widget sobrescreveu `onRelease()`, `m_shouldRedraw = true` é definido
   - Widget é removido do array

3. **Touch Solto (`TOUCH_UP`)**:
   - `clearTouchedWidgets(true)` é chamado
   - `onRelease()` é chamado para todos os widgets no array
   - `m_isPressed = false` é definido para cada widget
   - Se widgets sobrescreveram `onRelease()`, `m_shouldRedraw = true` é definido
   - Array é limpo

4. **Atualização Visual (`updateWidgets()`)**:
   - `updateWidgets()` chama métodos `updateXXX()` para cada tipo de widget
   - Cada `updateXXX()` chama `redraw()` nos widgets
   - `redraw()` verifica `m_shouldRedraw` antes de desenhar
   - Se `m_shouldRedraw == true`, widget é redesenhado
   - Durante `redraw()`, widget pode verificar `m_isPressed` para alterar aparência
   - Após desenhar, `m_shouldRedraw = false` é definido

### Relação entre `m_isPressed` e `m_shouldRedraw`

- **`m_isPressed`**: Estado do widget (está sendo pressionado ou não)
  - `true` quando `detectTouch()` retorna `true`
  - `false` quando `onRelease()` é chamado
  
- **`m_shouldRedraw`**: Flag para indicar necessidade de redesenho
  - Pode ser `true` quando `detectTouch()` retorna `true` (feedback imediato)
  - **DEVE** ser `true` quando widget sobrescreve `onRelease()` (para atualizar aparência)
  - Verificado em `redraw()` antes de desenhar
  - Definido como `false` após redesenho

### Exemplo de Fluxo Completo

```
1. Usuário toca em RectButton
   → detectTouch() retorna true
   → setPressed(true) → m_isPressed = true
   → m_shouldRedraw = true (opcional, para feedback imediato)
   → Widget adicionado ao array

2. updateWidgets() é chamado
   → updateRectButton() chama redraw()
   → redraw() verifica m_shouldRedraw == true
   → redraw() verifica m_isPressed == true
   → Desenha botão com cor de "pressionado"
   → m_shouldRedraw = false

3. Usuário solta o touch
   → onRelease() é chamado
   → m_isPressed = false
   → m_shouldRedraw = true (se widget sobrescreveu onRelease)
   → Widget removido do array

4. updateWidgets() é chamado novamente
   → updateRectButton() chama redraw()
   → redraw() verifica m_shouldRedraw == true
   → redraw() verifica m_isPressed == false
   → Desenha botão com cor normal
   → m_shouldRedraw = false
```

---

## Passos de Implementação

### Passo 1: Adicionar Macro de Compilação Condicional

**Problema**: Precisa de forma fácil de habilitar/desabilitar o recurso.

**Solução**: Adicionar macro `ENABLE_ON_RELEASE` no início de `displayfk.h`.

**Localização**: `src/displayfk.h`, após includes, antes da classe `DisplayFK`

**Código a adicionar**:
```cpp
// Descomentar a linha abaixo para habilitar o recurso onRelease
// Quando comentado, todo o código de rastreamento de touch é desabilitado
// #define ENABLE_ON_RELEASE
```

**Considerações**:
- Macro deve ser definida apenas em `displayfk.h`
- Quando comentada, todo código de rastreamento não será compilado
- Permite zero overhead quando desabilitado

---

### Passo 2: Adicionar Estrutura de Rastreamento em `displayfk.h`

**Problema**: Não existe estrutura para armazenar widgets que estão recebendo touch.

**Solução**: Adicionar array de ponteiros e contador na seção privada da classe `DisplayFK`, envolvidos com `#ifdef ENABLE_ON_RELEASE`.

**Localização**: `src/displayfk.h`, seção `private:` (após linha ~580)

**Código a adicionar**:
```cpp
#ifdef ENABLE_ON_RELEASE
    // Touch tracking for onRelease functionality
    WidgetBase* m_touchedWidgets[5];      ///< Array of widgets currently being touched
    uint8_t m_touchedWidgetsCount;        ///< Number of widgets in the touched array
    bool m_trackingTouch;                 ///< Flag indicating if we're tracking touch
#endif
```

**Considerações**:
- Array de 5 posições para suportar múltiplos widgets
- Contador para rastrear quantos widgets estão no array
- Flag para indicar se estamos em modo de rastreamento
- Envolvido com `#ifdef` para permitir desabilitar o recurso

---

### Passo 3: Inicializar Variáveis no Construtor

**Problema**: Variáveis não inicializadas podem causar comportamento indefinido.

**Solução**: Inicializar array e contador no construtor de `DisplayFK`, envolvidos com `#ifdef ENABLE_ON_RELEASE`.

**Localização**: `src/displayfk.cpp`, construtor `DisplayFK::DisplayFK()`

**Código a adicionar**:
```cpp
#ifdef ENABLE_ON_RELEASE
    // Initialize touch tracking
    m_touchedWidgetsCount = 0;
    m_trackingTouch = false;
    for (uint8_t i = 0; i < 5; i++) {
        m_touchedWidgets[i] = nullptr;
    }
#endif
```

**Considerações**:
- Garantir que todos os ponteiros começam como `nullptr`
- Contador inicia em 0
- Envolvido com `#ifdef` para não compilar quando desabilitado

---

### Passo 3: Adicionar Variável `m_isPressed` em `WidgetBase`

**Problema**: Não existe forma de rastrear se um widget está atualmente sendo pressionado (com touch ativo).

**Solução**: Adicionar variável `bool m_isPressed` na seção `protected:` de `WidgetBase` para rastrear estado de pressão.

**Localização**: `src/widgets/widgetbase.h`, classe `WidgetBase`, seção `protected:` (após `m_shouldRedraw`, linha ~478)

**Código a adicionar**:
```cpp
  bool m_isPressed;          ///< True when widget is currently being touched/pressed
```

**Localização**: `src/widgets/widgetbase.cpp`, construtor `WidgetBase::WidgetBase()`

**Código a modificar** (adicionar inicialização):
```cpp
WidgetBase::WidgetBase(uint16_t _x, uint16_t _y, uint8_t _screen) 
    : m_visible(true)
    , m_xPos(_x)
    , m_yPos(_y)
    , m_screen(_screen)
    , m_loaded(false)
    , m_enabled(true)
    , m_initialized(false)
    , m_shouldRedraw(false)
    , m_isPressed(false)      // NEW: Initialize as not pressed
    , m_locked(false)
    , m_myTime(0)
    , m_callback(nullptr)
{
    ESP_LOGD(TAG, "WidgetBase created at (%d, %d) on screen %d", _x, _y, _screen);
}
```

**Localização**: `src/widgets/widgetbase.cpp`, destrutor `WidgetBase::~WidgetBase()`

**Código a modificar** (adicionar reset):
```cpp
WidgetBase::~WidgetBase()
{
    ESP_LOGD(TAG, "WidgetBase destroyed at (%d, %d)", m_xPos, m_yPos);
    
    // Limpa o callback se existir
    if (m_callback != nullptr) {
        m_callback = nullptr;
    }
    
    // Reseta o estado do widget
    m_loaded = false;
    m_initialized = false;
    m_shouldRedraw = false;
    m_isPressed = false;      // NEW: Reset pressed state
}
```

**Considerações**:
- Variável deve ser `protected:` para que widgets derivados possam acessá-la
- Inicializada como `false` no construtor
- Resetada no destrutor para garantir estado limpo
- Pode ser usada em `redraw()` para alterar aparência visual baseada no estado

---

### Passo 4: Modificar `detectTouch()` para Definir `m_isPressed = true`

**Problema**: Quando `detectTouch()` retorna `true`, o widget não marca que está sendo pressionado.

**Solução**: Modificar a implementação padrão de `detectTouch()` em `WidgetBase` (se existir) ou garantir que widgets derivados definam `m_isPressed = true` quando detectam touch.

**Abordagem Recomendada**: Como `detectTouch()` é um método virtual puro em `WidgetBase`, cada widget derivado implementa sua própria versão. A melhor abordagem é criar um método auxiliar em `WidgetBase` que pode ser chamado pelos widgets derivados, OU modificar cada implementação de `detectTouch()` nos widgets.

**Opção 1 - Método Auxiliar (Recomendado)**:

**Localização**: `src/widgets/widgetbase.h`, classe `WidgetBase`, seção `protected:`

**Código a adicionar**:
```cpp
  /**
   * @brief Marks widget as pressed
   * @details Called internally when touch is detected. Sets m_isPressed to true.
   */
  void setPressed(bool pressed);
```

**Localização**: `src/widgets/widgetbase.cpp`

**Código a adicionar**:
```cpp
/**
 * @brief Sets the pressed state of the widget
 * @param pressed True if widget is being pressed, false otherwise
 */
void WidgetBase::setPressed(bool pressed) {
    m_isPressed = pressed;
    if (pressed) {
        ESP_LOGD(TAG, "Widget marked as pressed at (%d, %d)", m_xPos, m_yPos);
    } else {
        ESP_LOGD(TAG, "Widget marked as not pressed at (%d, %d)", m_xPos, m_yPos);
    }
}
```

**Modificação em Widgets Derivados**: Cada widget que implementa `detectTouch()` deve chamar `setPressed(true)` quando detecta touch:

**Exemplo para `RectButton::detectTouch()`**:
```cpp
bool RectButton::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) {
  // ... validações existentes ...
  
  bool inBounds = POINT_IN_RECT(*_xTouch, *_yTouch, m_xPos, m_yPos, m_config.width, m_config.height);
  if(inBounds) {
    m_myTime = millis();
    setPressed(true);  // NEW: Mark as pressed
    changeState();
    m_shouldRedraw = true;
    return true;
  }
  return false;
}
```

**Opção 2 - Modificar Diretamente**: Modificar cada implementação de `detectTouch()` para definir `m_isPressed = true` diretamente quando retorna `true`.

**Considerações**:
- Método auxiliar `setPressed()` é preferível pois centraliza lógica e permite logging
- Todos os widgets que implementam `detectTouch()` precisam ser modificados
- Pode ser feito incrementalmente (widget por widget)
- Widgets que não modificarem continuarão funcionando, mas não terão `m_isPressed` atualizado

**Widgets que precisam ser modificados** (todos que implementam `detectTouch()`):
- `CheckBox`
- `CircleButton`
- `HSlider`
- `RadioGroup`
- `RectButton`
- `Image`
- `SpinBox`
- `ToggleButton`
- `TextButton`
- `TextBox`
- `NumberBox`
- `TouchArea`

---

### Passo 5: Adicionar Método Virtual `onRelease()` em `WidgetBase`

**Problema**: Não existe método para notificar widgets quando touch é perdido.

**Solução**: Adicionar método virtual em `WidgetBase` que define `m_isPressed = false` na implementação padrão.

**Localização**: `src/widgets/widgetbase.h`, classe `WidgetBase`, seção `public:` ou `protected:`

**Código a adicionar** (após `detectTouch`):
```cpp
  virtual void onRelease();  ///< Called when touch is released or leaves widget area
```

**Localização**: `src/widgets/widgetbase.cpp`

**Código a adicionar** (implementação padrão):
```cpp
/**
 * @brief Default implementation of onRelease
 * @details This method is called when touch is released or leaves the widget area.
 *          Default implementation sets m_isPressed to false.
 *          Derived widgets should call WidgetBase::onRelease() first if overriding,
 *          then set m_shouldRedraw = true if visual update is needed.
 */
void WidgetBase::onRelease() {
    // Default implementation: reset pressed state
    m_isPressed = false;
    ESP_LOGD(TAG, "Widget released at (%d, %d)", m_xPos, m_yPos);
}
```

**Considerações**:
- Implementação padrão define `m_isPressed = false`
- Widgets derivados podem sobrescrever para adicionar lógica customizada
- Widgets que sobrescrevem devem chamar `WidgetBase::onRelease()` primeiro OU definir `m_isPressed = false` manualmente
- Widgets que sobrescrevem devem também definir `m_shouldRedraw = true` para garantir redesenho

**Padrão Recomendado para Widgets que Sobrescrevem**:
```cpp
void MyWidget::onRelease() override {
    // Call base implementation to set m_isPressed = false
    WidgetBase::onRelease();
    
    // Custom logic here
    // ...
    
    // IMPORTANTE: Sempre definir m_shouldRedraw = true quando sobrescrever onRelease()
    // Isso garante que o widget será redesenhado no próximo updateWidgets()
    // para refletir a mudança de estado (não mais pressionado)
    m_shouldRedraw = true;
}
```

**OU** (se não chamar base):
```cpp
void MyWidget::onRelease() override {
    // Set pressed state to false
    m_isPressed = false;
    
    // Custom logic here
    // ...
    
    // IMPORTANTE: Sempre definir m_shouldRedraw = true quando sobrescrever onRelease()
    m_shouldRedraw = true;
}
```

**⚠️ Requisito Obrigatório**: 
Quando uma classe derivada sobrescreve `onRelease()`, **DEVE** definir `m_shouldRedraw = true` para garantir que o widget seja redesenhado durante o próximo `updateWidgets()`. Isso permite que o widget atualize sua aparência visual para refletir que não está mais sendo pressionado.

**Motivo**: 
- Quando `onRelease()` é chamado, `m_isPressed` muda de `true` para `false`
- Se o widget usa `m_isPressed` em `redraw()` para alterar aparência, precisa ser redesenhado
- `m_shouldRedraw = true` garante que `redraw()` será chamado no próximo ciclo de `updateWidgets()`

---

### Passo 7: Criar Funções Auxiliares para Gerenciar Array de Widgets

**Problema**: Precisa de funções para adicionar, remover e verificar widgets no array.

**Solução**: Criar funções auxiliares privadas em `DisplayFK`, envolvidas com `#ifdef ENABLE_ON_RELEASE`.

**Localização**: `src/displayfk.h`, seção `private:` (métodos privados)

**Código a adicionar** (após outros métodos privados):
```cpp
#ifdef ENABLE_ON_RELEASE
    // Touch tracking helper methods
    void addTouchedWidget(WidgetBase* widget);
    void removeTouchedWidget(WidgetBase* widget);
    bool isWidgetInTouchedArray(WidgetBase* widget) const;
    void checkWidgetsStillTouched(uint16_t xTouch, uint16_t yTouch);
    void clearTouchedWidgets(bool callOnRelease = true);
#endif
```

**Localização**: `src/displayfk.cpp`

**Todas as implementações devem ser envolvidas com `#ifdef ENABLE_ON_RELEASE`**:

**Implementação de `addTouchedWidget`**:
```cpp
#ifdef ENABLE_ON_RELEASE
/**
 * @brief Adds a widget to the touched widgets array
 * @param widget Pointer to the widget to add
 * @details Only adds if widget is not already in array and array has space
 */
void DisplayFK::addTouchedWidget(WidgetBase* widget) {
    if (!widget) return;
    
    // Check if widget is already in array
    if (isWidgetInTouchedArray(widget)) return;
    
    // Check if array has space
    if (m_touchedWidgetsCount >= 5) {
        ESP_LOGW(TAG, "Touched widgets array is full, cannot add more widgets");
        return;
    }
    
    // Add widget to array
    m_touchedWidgets[m_touchedWidgetsCount] = widget;
    m_touchedWidgetsCount++;
    ESP_LOGD(TAG, "Added widget to touched array, count: %u", m_touchedWidgetsCount);
}
#endif
```

**Implementação de `removeTouchedWidget`**:
```cpp
#ifdef ENABLE_ON_RELEASE
/**
 * @brief Removes a widget from the touched widgets array
 * @param widget Pointer to the widget to remove
 * @details Calls onRelease() before removing, which sets m_isPressed = false
 *          and m_shouldRedraw = true (if widget overrides onRelease)
 */
void DisplayFK::removeTouchedWidget(WidgetBase* widget) {
    if (!widget) return;
    
    // Find widget in array
    for (uint8_t i = 0; i < m_touchedWidgetsCount; i++) {
        if (m_touchedWidgets[i] == widget) {
            // Call onRelease before removing
            // This will set m_isPressed = false and m_shouldRedraw = true
            widget->onRelease();
            
            // Shift remaining widgets left
            for (uint8_t j = i; j < m_touchedWidgetsCount - 1; j++) {
                m_touchedWidgets[j] = m_touchedWidgets[j + 1];
            }
            
            // Clear last position and decrement count
            m_touchedWidgets[m_touchedWidgetsCount - 1] = nullptr;
            m_touchedWidgetsCount--;
            
            ESP_LOGD(TAG, "Removed widget from touched array, count: %u", m_touchedWidgetsCount);
            return;
        }
    }
}
#endif
```

**Implementação de `isWidgetInTouchedArray`**:
```cpp
#ifdef ENABLE_ON_RELEASE
/**
 * @brief Checks if a widget is in the touched widgets array
 * @param widget Pointer to the widget to check
 * @return true if widget is in array, false otherwise
 */
bool DisplayFK::isWidgetInTouchedArray(WidgetBase* widget) const {
    if (!widget) return false;
    
    for (uint8_t i = 0; i < m_touchedWidgetsCount; i++) {
        if (m_touchedWidgets[i] == widget) {
            return true;
        }
    }
    return false;
}
#endif
```

**Implementação de `checkWidgetsStillTouched`**:
```cpp
#ifdef ENABLE_ON_RELEASE
/**
 * @brief Checks if widgets in array are still being touched
 * @param xTouch Current touch X coordinate
 * @param yTouch Current touch Y coordinate
 * @details Removes widgets from array if touch is no longer within their bounds
 */
void DisplayFK::checkWidgetsStillTouched(uint16_t xTouch, uint16_t yTouch) {
    // Create a temporary array to avoid modifying while iterating
    WidgetBase* widgetsToCheck[5];
    uint8_t count = m_touchedWidgetsCount;
    
    // Copy current array
    for (uint8_t i = 0; i < count; i++) {
        widgetsToCheck[i] = m_touchedWidgets[i];
    }
    
    // Check each widget
    for (uint8_t i = 0; i < count; i++) {
        WidgetBase* widget = widgetsToCheck[i];
        if (!widget) continue;
        
        // Check if widget is still valid (visible, enabled, on current screen)
        if (!widget->showingMyScreen() || !widget->isEnabled()) {
            removeTouchedWidget(widget);
            continue;
        }
        
        // Check if touch is still within widget bounds
        // Note: This requires calling detectTouch with a flag that doesn't modify state
        // We need to check bounds without triggering touch detection
        // This might require a new method in WidgetBase: isPointInBounds(x, y)
        // For now, we'll use a workaround: try detectTouch and check if it would detect
        uint16_t testX = xTouch;
        uint16_t testY = yTouch;
        
        // Save current state to restore after check
        // Note: This is a simplified check - actual implementation may need
        // a dedicated method in WidgetBase for bounds checking
        bool stillTouched = widget->detectTouch(&testX, &testY);
        
        if (!stillTouched) {
            removeTouchedWidget(widget);
        }
    }
}
#endif
```

**Implementação de `clearTouchedWidgets`**:
```cpp
#ifdef ENABLE_ON_RELEASE
/**
 * @brief Clears all widgets from touched array
 * @param callOnRelease If true, calls onRelease() for each widget before clearing
 * @details When callOnRelease is true, onRelease() is called for each widget,
 *          which sets m_isPressed = false and m_shouldRedraw = true (if overridden)
 */
void DisplayFK::clearTouchedWidgets(bool callOnRelease) {
    if (callOnRelease) {
        // Call onRelease for all widgets
        // This will set m_isPressed = false and m_shouldRedraw = true for each
        for (uint8_t i = 0; i < m_touchedWidgetsCount; i++) {
            if (m_touchedWidgets[i]) {
                m_touchedWidgets[i]->onRelease();
            }
        }
    }
    
    // Clear array
    for (uint8_t i = 0; i < m_touchedWidgetsCount; i++) {
        m_touchedWidgets[i] = nullptr;
    }
    
    m_touchedWidgetsCount = 0;
    m_trackingTouch = false;
    ESP_LOGD(TAG, "Cleared touched widgets array");
}
#endif
```

**Considerações**:
- `checkWidgetsStillTouched` pode precisar de um método auxiliar em `WidgetBase` para verificar bounds sem modificar estado
- Considerar adicionar `isPointInBounds(uint16_t x, uint16_t y)` em `WidgetBase` para evitar efeitos colaterais de `detectTouch()`

---

### Passo 8: Modificar `processTouchableWidgets()` para Rastrear Múltiplos Widgets

**Problema**: Funções `processXXXTouch()` retornam após encontrar o primeiro widget, impedindo rastreamento de múltiplos widgets.

**Solução**: Modificar lógica para continuar processando mesmo após encontrar widgets, coletando todos os widgets tocados.

**Localização**: `src/displayfk.cpp`, função `processTouchableWidgets()`

**Modificação necessária**:
- Remover `return` prematuro das funções `processXXXTouch()` quando em modo de rastreamento
- OU criar versões alternativas que coletam widgets sem retornar
- OU modificar para aceitar um parâmetro indicando modo de coleta

**Abordagem Recomendada**: Criar versão alternativa que coleta widgets

**Código a modificar** (criar nova função ou modificar existente):
```cpp
/**
 * @brief Processes touchable widgets and collects all touched widgets
 * @param xTouch Touch X position
 * @param yTouch Touch Y position
 * @param collectMode If true, collects all widgets without early return
 */
void DisplayFK::processTouchableWidgets(uint16_t xTouch, uint16_t yTouch, bool collectMode = false) {
    ESP_LOGD(TAG, "Processing touchable widgets (collectMode: %s)", collectMode ? "true" : "false");
    
    // Process each type of touchable widget
    // Note: Each processXXXTouch function needs to be modified to support collectMode
    processCheckboxTouch(xTouch, yTouch, collectMode);
    processCircleButtonTouch(xTouch, yTouch, collectMode);
    processHSliderTouch(xTouch, yTouch, collectMode);
    processRadioGroupTouch(xTouch, yTouch, collectMode);
    processRectButtonTouch(xTouch, yTouch, collectMode);
    processImageTouch(xTouch, yTouch, collectMode);
    processSpinboxTouch(xTouch, yTouch, collectMode);
    processToggleTouch(xTouch, yTouch, collectMode);
    processTextButtonTouch(xTouch, yTouch, collectMode);
    processTextBoxTouch(xTouch, yTouch, collectMode);
    processNumberBoxTouch(xTouch, yTouch, collectMode);
    processEmptyAreaTouch(xTouch, yTouch, collectMode);
}
```

**Considerações**:
- Modificar todas as funções `processXXXTouch()` para aceitar parâmetro `collectMode`
- Quando `collectMode == false`, manter comportamento atual (retornar após primeiro match)
- Quando `collectMode == true`, continuar processando e adicionar widgets ao array

---

### Passo 9: Modificar Funções `processXXXTouch()` para Suportar Modo de Coleta

**Problema**: Cada função `processXXXTouch()` retorna após encontrar primeiro widget.

**Solução**: Adicionar parâmetro `collectMode` e modificar lógica para continuar quando em modo de coleta.

**Localização**: `src/displayfk.cpp`, todas as funções `processXXXTouch()`

**Exemplo de modificação** (usando `processCheckboxTouch` como template):

**Assinatura modificada**:
```cpp
void DisplayFK::processCheckboxTouch(uint16_t xTouch, uint16_t yTouch, bool collectMode = false)
```

**Código modificado**:
```cpp
void DisplayFK::processCheckboxTouch(uint16_t xTouch, uint16_t yTouch, bool collectMode) {
#ifdef DFK_CHECKBOX
    if(!m_checkboxConfigured || !arrayCheckbox){
        return;
    }
    
    for (uint32_t indice = 0; indice < qtdCheckbox; indice++) {
        // Skip widgets not on current screen
        if (!arrayCheckbox[indice]->showingMyScreen()) continue;
        
        if (arrayCheckbox[indice]->detectTouch(&xTouch, &yTouch)) {
            functionCB_t cb = arrayCheckbox[indice]->getCallbackFunc();
            WidgetBase::addCallback(cb, WidgetBase::CallbackOrigin::TOUCH);
            
#ifdef ENABLE_ON_RELEASE
            // Add to touched widgets array if in collect mode
            if (collectMode) {
                addTouchedWidget(arrayCheckbox[indice]);
            }
#endif
            
            // Return early only if not in collect mode
            if (!collectMode) {
                return;
            }
            // In collect mode, continue to find other widgets
        }
    }
#endif
}
```

**Funções a modificar**:
- `processCheckboxTouch()`
- `processCircleButtonTouch()`
- `processHSliderTouch()`
- `processRadioGroupTouch()`
- `processRectButtonTouch()`
- `processImageTouch()`
- `processSpinboxTouch()`
- `processToggleTouch()`
- `processTextButtonTouch()`
- `processTextBoxTouch()`
- `processNumberBoxTouch()`
- `processEmptyAreaTouch()`

**Considerações**:
- Manter comportamento padrão quando `collectMode == false` (retrocompatibilidade)
- Adicionar widget ao array quando `collectMode == true` e widget detectado
- Continuar processando quando em modo de coleta

---

### Passo 10: Modificar `processTouchStatus()` para Gerenciar Rastreamento

**Problema**: `processTouchStatus()` não gerencia o array de widgets tocados.

**Solução**: Integrar lógica de rastreamento nos diferentes estados de touch.

**Localização**: `src/displayfk.cpp`, função `processTouchStatus()`

**Modificações necessárias**:

**No estado `TOUCH_DOWN`**:
```cpp
if(m_lastTouchState == TouchEventType::TOUCH_DOWN){
    Serial.println("---------------------- APERTEI");
    pressPoint.x = xTouch;
    pressPoint.y = yTouch;
    releasePoint.x = xTouch;
    releasePoint.y = yTouch;
    
#ifdef ENABLE_ON_RELEASE
    // Clear previous touched widgets and start tracking
    clearTouchedWidgets(true);  // Call onRelease for previous widgets
    m_trackingTouch = true;
    // Process widgets in collect mode to populate array
    processTouchableWidgets(xTouch, yTouch, true);  // collectMode = true
#else
    // Comportamento antigo: processar sem modo de coleta
    processTouchableWidgets(xTouch, yTouch, false);
#endif
}
```

**No estado `TOUCH_HOLD`**:
```cpp
else if(m_lastTouchState == TouchEventType::TOUCH_HOLD){
    releasePoint.x = xTouch;
    releasePoint.y = yTouch;
    //Serial.println("---------------------- APERTANDO");
    
#ifdef ENABLE_ON_RELEASE
    // Check if widgets are still being touched
    if (m_trackingTouch && m_touchedWidgetsCount > 0) {
        checkWidgetsStillTouched(xTouch, yTouch);
    }
#endif
}
```

**No estado `TOUCH_UP`**:
```cpp
else if(m_lastTouchState == TouchEventType::TOUCH_UP){
    Serial.println("---------------------- SOLTEI");
    // ... existing swipe detection code ...
    
#ifdef ENABLE_ON_RELEASE
    // Call onRelease for all tracked widgets and clear array
    if (m_trackingTouch) {
        clearTouchedWidgets(true);  // Call onRelease for all widgets
    }
#endif
}
```

**No estado `NONE`**:
```cpp
else if(!hasTouch && m_lastTouchState == TouchEventType::NONE){
    m_lastTouchState = TouchEventType::NONE;
    
#ifdef ENABLE_ON_RELEASE
    // Ensure array is cleared when no touch
    if (m_touchedWidgetsCount > 0) {
        clearTouchedWidgets(false);  // Don't call onRelease, just clear
    }
#endif
}
```

**Considerações**:
- Limpar array anterior quando novo touch inicia
- Verificar continuamente durante `TOUCH_HOLD` se widgets ainda estão sendo tocados
- Chamar `onRelease()` quando touch é solto ou widgets perdem o touch

---

### Passo 11: Adicionar Método Auxiliar em `WidgetBase` para Verificação de Bounds (Opcional mas Recomendado)

**Problema**: `detectTouch()` pode ter efeitos colaterais (modifica timestamps, etc.), não sendo ideal para apenas verificar bounds.

**Solução**: Adicionar método dedicado para verificar se ponto está dentro dos bounds do widget.

**Localização**: `src/widgets/widgetbase.h`, classe `WidgetBase`, seção `protected:` ou `public:`

**Código a adicionar**:
```cpp
  /**
   * @brief Checks if a point is within widget bounds
   * @param x X coordinate to check
   * @param y Y coordinate to check
   * @return true if point is within bounds, false otherwise
   * @details This method only checks bounds without side effects.
   *          Derived widgets should override to provide accurate bounds checking.
   */
  virtual bool isPointInBounds(uint16_t x, uint16_t y) const;
```

**Localização**: `src/widgets/widgetbase.cpp`

**Implementação padrão** (precisa de informações de dimensão do widget):
```cpp
/**
 * @brief Default implementation checks basic bounds
 * @details Derived widgets should override with their specific bounds logic
 */
bool WidgetBase::isPointInBounds(uint16_t x, uint16_t y) const {
    // Default implementation - derived widgets should override
    // This is a placeholder that always returns false
    // Each widget type should implement its own bounds checking
    return false;
}
```

**Considerações**:
- Cada widget derivado precisa implementar este método
- Método deve verificar apenas bounds, sem efeitos colaterais
- Pode usar informações de dimensão específicas de cada widget

**Alternativa**: Se implementar `isPointInBounds()` em todos os widgets for muito trabalhoso, pode-se usar `detectTouch()` com cuidado, salvando e restaurando estado quando necessário.

---

### Passo 12: Implementar `isPointInBounds()` nos Widgets Derivados (Opcional)

**Problema**: Widgets precisam de método para verificar bounds sem efeitos colaterais.

**Solução**: Implementar `isPointInBounds()` em cada tipo de widget que precisa de rastreamento.

**Localização**: Cada arquivo de widget (ex: `src/widgets/checkbox/wcheckbox.h`, `wcheckbox.cpp`)

**Exemplo para CheckBox**:
```cpp
// Em wcheckbox.h
bool isPointInBounds(uint16_t x, uint16_t y) const override;

// Em wcheckbox.cpp
bool CheckBox::isPointInBounds(uint16_t x, uint16_t y) const {
    CHECK_VISIBLE_BOOL
    CHECK_CURRENTSCREEN_BOOL
    
    // Check if point is within checkbox bounds
    return POINT_IN_RECT(x, y, m_xPos, m_yPos, m_config.width, m_config.height);
}
```

**Considerações**:
- Implementar apenas nos widgets que realmente precisam de `onRelease()`
- Pode ser feito incrementalmente conforme necessidade
- Se não implementado, `checkWidgetsStillTouched()` pode usar `detectTouch()` com cuidado

---

### Passo 13: Atualizar `checkWidgetsStillTouched()` para Usar `isPointInBounds()` (Se Implementado)

**Problema**: `checkWidgetsStillTouched()` precisa verificar bounds sem efeitos colaterais.

**Solução**: Usar `isPointInBounds()` se disponível, caso contrário usar `detectTouch()` com cuidado.

**Localização**: `src/displayfk.cpp`, função `checkWidgetsStillTouched()`

**Código atualizado**:
```cpp
void DisplayFK::checkWidgetsStillTouched(uint16_t xTouch, uint16_t yTouch) {
    // Create a temporary array to avoid modifying while iterating
    WidgetBase* widgetsToCheck[5];
    uint8_t count = m_touchedWidgetsCount;
    
    // Copy current array
    for (uint8_t i = 0; i < count; i++) {
        widgetsToCheck[i] = m_touchedWidgets[i];
    }
    
    // Check each widget
    for (uint8_t i = 0; i < count; i++) {
        WidgetBase* widget = widgetsToCheck[i];
        if (!widget) continue;
        
        // Check if widget is still valid
        if (!widget->showingMyScreen() || !widget->isEnabled()) {
            removeTouchedWidget(widget);
            continue;
        }
        
        // Check if touch is still within widget bounds
        bool stillTouched = false;
        
        // Try to use isPointInBounds if available (preferred method)
        // Note: This requires checking if method is properly implemented
        // For now, we'll use a hybrid approach
        
        // Option 1: Use isPointInBounds if widget implements it properly
        // (This requires a way to check if method is overridden)
        // For simplicity, we'll try calling it and handle the result
        
        // Option 2: Use detectTouch with state preservation
        // This is more complex but works with current implementation
        
        // Simplified approach: Use detectTouch but be aware of side effects
        uint16_t testX = xTouch;
        uint16_t testY = yTouch;
        unsigned long savedTime = widget->m_myTime;  // Save state if accessible
        
        stillTouched = widget->detectTouch(&testX, &testY);
        
        // Restore state if needed (may require making m_myTime accessible)
        // widget->m_myTime = savedTime;  // Only if m_myTime is protected/public
        
        if (!stillTouched) {
            removeTouchedWidget(widget);
        }
    }
}
```

**Considerações**:
- Se `isPointInBounds()` estiver implementado, preferir seu uso
- Se não, usar `detectTouch()` com cuidado para preservar estado
- Pode ser necessário tornar `m_myTime` acessível ou criar método getter/setter

---

### Passo 14: Usar `m_isPressed` em Métodos `redraw()` dos Widgets (Opcional mas Recomendado)

**Problema**: Widgets não alteram aparência visual baseada no estado de pressão.

**Solução**: Widgets podem verificar `m_isPressed` durante `redraw()` para alterar aparência quando pressionados.

**Localização**: Cada widget que implementa `redraw()`, por exemplo `src/widgets/rectbutton/wrectbutton.cpp`

**Exemplo de Uso em `RectButton::redraw()`**:
```cpp
void RectButton::redraw() {
    // ... validações existentes ...
    
    // Draw button background
    uint16_t bgColor = m_isPressed ? m_config.colorPressed : m_config.colorBackground;
    WidgetBase::objTFT->fillRoundRect(m_xPos, m_yPos, m_config.width, m_config.height, 
                                      m_config.radius, bgColor);
    
    // Draw border
    uint16_t borderColor = m_isPressed ? m_config.colorBorderPressed : m_config.colorBorder;
    WidgetBase::objTFT->drawRoundRect(m_xPos, m_yPos, m_config.width, m_config.height, 
                                       m_config.radius, borderColor);
    
    // ... resto do código de desenho ...
    
    m_shouldRedraw = false;
}
```

**Considerações**:
- Widgets podem usar `m_isPressed` para alterar cores, sombras, posição, etc.
- Isso permite feedback visual imediato quando widget é pressionado
- Widgets que não implementarem continuam funcionando normalmente
- Pode ser feito incrementalmente conforme necessidade

**Widgets que podem se beneficiar**:
- Botões (RectButton, CircleButton, TextButton)
- Sliders (HSlider, VSlider)
- ToggleButton
- CheckBox
- Qualquer widget que precisa de feedback visual de pressão

---

### Passo 15: Testar Implementação

**Problema**: Precisa validar que implementação funciona corretamente e não quebra funcionalidade existente.

**Solução**: Criar testes para validar comportamento.

**Testes a realizar**:

1. **Teste de Retrocompatibilidade**:
   - Verificar que widgets sem `onRelease()` implementado continuam funcionando
   - Verificar que callbacks normais ainda são executados

2. **Teste de Rastreamento Básico**:
   - Tocar em um widget e verificar que é adicionado ao array
   - Soltar touch e verificar que `onRelease()` é chamado

3. **Teste de Múltiplos Widgets**:
   - Tocar em área onde múltiplos widgets se sobrepõem
   - Verificar que todos são adicionados ao array
   - Verificar que todos recebem `onRelease()` quando touch é solto

4. **Teste de Saída de Área**:
   - Tocar em widget e manter pressionado
   - Mover dedo para fora da área do widget
   - Verificar que `onRelease()` é chamado quando sai da área

5. **Teste de Limites**:
   - Tentar adicionar mais de 5 widgets
   - Verificar que sistema lida graciosamente com limite

6. **Teste de Estados**:
   - Tocar em widget e mudar de tela
   - Verificar que widget é removido do array
   - Verificar que `onRelease()` é chamado

---

## Ordem de Implementação Recomendada

1. **Fase 1 - Configuração e Estrutura Base** (Passos 1-5):
   - Adicionar macro `ENABLE_ON_RELEASE` em `displayfk.h`
   - Adicionar variável `m_isPressed` em `WidgetBase`
   - Adicionar método `setPressed()` auxiliar
   - Adicionar método `onRelease()` em `WidgetBase`
   - Inicializar `m_isPressed` no construtor
   - Adicionar variável `m_isPressed` em `WidgetBase`
   - Adicionar método `setPressed()` auxiliar
   - Adicionar método `onRelease()` em `WidgetBase`
   - Inicializar `m_isPressed` no construtor

2. **Fase 2 - Modificar detectTouch() nos Widgets** (Passo 4):
   - Modificar cada widget para chamar `setPressed(true)` quando `detectTouch()` retorna `true`
   - Pode ser feito incrementalmente (widget por widget)
   - Testar que `m_isPressed` é definido corretamente

3. **Fase 3 - Estrutura de Rastreamento em DisplayFK** (Passos 2-3):
   - Adicionar variáveis de rastreamento em `DisplayFK` (com `#ifdef`)
   - Inicializar variáveis no construtor (com `#ifdef`)

4. **Fase 4 - Funções Auxiliares** (Passo 7):
   - Implementar funções de gerenciamento do array
   - Testar funções isoladamente

5. **Fase 5 - Integração com Touch** (Passos 8-10):
   - Modificar `processTouchableWidgets()` e funções relacionadas
   - Integrar rastreamento em `processTouchStatus()`
   - Testar rastreamento básico e chamada de `onRelease()`

6. **Fase 6 - Detecção de Saída** (Passos 11-13):
   - Implementar `isPointInBounds()` (opcional)
   - Atualizar `checkWidgetsStillTouched()`
   - Testar detecção de saída de área

7. **Fase 7 - Uso de m_isPressed em redraw()** (Passo 14):
   - Modificar widgets para usar `m_isPressed` em `redraw()`
   - Testar feedback visual
   - Pode ser feito incrementalmente

8. **Fase 8 - Testes e Refinamento** (Passo 15):
   - Testes completos
   - Ajustes finais
   - Documentação

---

## Pontos de Atenção

### 1. Performance
- Verificar bounds de até 5 widgets a cada ciclo pode ter impacto
- Considerar otimizações se necessário
- Monitorar em sistemas com muitos widgets

### 2. Thread Safety
- Verificar se toque é processado em thread diferente
- Garantir que acesso ao array seja thread-safe se necessário

### 3. Widgets Destruídos
- Verificar se ponteiros no array ainda são válidos
- Adicionar validação antes de chamar métodos de widgets

### 4. Ordem de Processamento
- Definir ordem de chamada de `onRelease()` (ordem de detecção recomendada)

### 5. Estados do Widget
- Garantir que `onRelease()` não seja chamado se widget foi destruído
- Verificar visibilidade e habilitação antes de chamar

---

## Exemplo de Uso

### Exemplo 1: Widget que Sobrescreve `onRelease()`

Após implementação, widgets podem usar `onRelease()` assim:

```cpp
// Em um widget derivado (ex: CustomButton)
void CustomButton::onRelease() override {
    // Call base implementation to set m_isPressed = false
    WidgetBase::onRelease();
    
    // Resetar estado visual customizado
    m_customPressedState = false;
    
    // Marcar para redesenho (importante!)
    m_shouldRedraw = true;
    
    // Executar lógica customizada
    if (m_releaseCallback) {
        m_releaseCallback();
    }
}
```

### Exemplo 2: Widget que Usa `m_isPressed` em `redraw()`

```cpp
// Em RectButton::redraw()
void RectButton::redraw() {
    CHECK_TFT_VOID
    CHECK_VISIBLE_VOID
    CHECK_CURRENTSCREEN_VOID
    CHECK_LOADED_VOID
    CHECK_SHOULDREDRAW_VOID
    
    // Usar m_isPressed para alterar cores
    uint16_t bgColor = m_isPressed ? 
        m_config.colorPressed :      // Cor quando pressionado
        m_config.colorBackground;    // Cor normal
    
    uint16_t borderColor = m_isPressed ? 
        m_config.colorBorderPressed : 
        m_config.colorBorder;
    
    // Desenhar botão com cores baseadas no estado
    WidgetBase::objTFT->fillRoundRect(m_xPos, m_yPos, m_config.width, 
                                      m_config.height, m_config.radius, bgColor);
    WidgetBase::objTFT->drawRoundRect(m_xPos, m_yPos, m_config.width, 
                                     m_config.height, m_config.radius, borderColor);
    
    // ... resto do desenho ...
    
    m_shouldRedraw = false;
}
```

### Exemplo 3: Widget que Modifica `detectTouch()`

```cpp
// Em RectButton::detectTouch()
bool RectButton::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) {
    // ... validações existentes ...
    
    bool inBounds = POINT_IN_RECT(*_xTouch, *_yTouch, m_xPos, m_yPos, 
                                  m_config.width, m_config.height);
    if(inBounds) {
        m_myTime = millis();
        setPressed(true);        // NEW: Marcar como pressionado
        changeState();
        m_shouldRedraw = true;   // Marcar para redesenho imediato
        return true;
    }
    return false;
}
```

---

## Notas Finais

- Esta implementação é incremental e não quebra funcionalidade existente
- Widgets que não implementam `onRelease()` continuam funcionando normalmente
- Sistema pode ser expandido no futuro (ex: suporte para mais de 5 widgets)
- Documentação deve ser atualizada após implementação

---

## Checklist de Implementação

### Fase 1 - Configuração e WidgetBase
- [ ] Passo 1: Adicionar macro `ENABLE_ON_RELEASE` em `displayfk.h`
- [ ] Passo 3: Adicionar `m_isPressed` em `WidgetBase`
- [ ] Passo 3: Inicializar `m_isPressed` no construtor
- [ ] Passo 3: Resetar `m_isPressed` no destrutor
- [ ] Passo 4: Adicionar método `setPressed()` em `WidgetBase`
- [ ] Passo 5: Adicionar método `onRelease()` em `WidgetBase`
- [ ] Passo 5: Implementar `onRelease()` padrão que define `m_isPressed = false`

### Fase 2 - Modificar detectTouch() nos Widgets
- [ ] Passo 4: Modificar `CheckBox::detectTouch()` para chamar `setPressed(true)`
- [ ] Passo 4: Modificar `CircleButton::detectTouch()` para chamar `setPressed(true)`
- [ ] Passo 4: Modificar `HSlider::detectTouch()` para chamar `setPressed(true)`
- [ ] Passo 4: Modificar `RadioGroup::detectTouch()` para chamar `setPressed(true)`
- [ ] Passo 4: Modificar `RectButton::detectTouch()` para chamar `setPressed(true)`
- [ ] Passo 4: Modificar `Image::detectTouch()` para chamar `setPressed(true)`
- [ ] Passo 4: Modificar `SpinBox::detectTouch()` para chamar `setPressed(true)`
- [ ] Passo 4: Modificar `ToggleButton::detectTouch()` para chamar `setPressed(true)`
- [ ] Passo 4: Modificar `TextButton::detectTouch()` para chamar `setPressed(true)`
- [ ] Passo 4: Modificar `TextBox::detectTouch()` para chamar `setPressed(true)`
- [ ] Passo 4: Modificar `NumberBox::detectTouch()` para chamar `setPressed(true)`
- [ ] Passo 4: Modificar `TouchArea::detectTouch()` para chamar `setPressed(true)`

### Fase 3 - DisplayFK Rastreamento
- [ ] Passo 2: Adicionar estrutura de rastreamento em `DisplayFK` (com `#ifdef`)
- [ ] Passo 3: Inicializar variáveis no construtor (com `#ifdef`)

### Fase 4 - Funções Auxiliares
- [ ] Passo 7: Implementar `addTouchedWidget()` (com `#ifdef`)
- [ ] Passo 7: Implementar `removeTouchedWidget()` (com `#ifdef`)
- [ ] Passo 7: Implementar `isWidgetInTouchedArray()` (com `#ifdef`)
- [ ] Passo 7: Implementar `checkWidgetsStillTouched()` (com `#ifdef`)
- [ ] Passo 7: Implementar `clearTouchedWidgets()` (com `#ifdef`)

### Fase 5 - Integração com Touch
- [ ] Passo 8: Modificar `processTouchableWidgets()` para suportar modo de coleta
- [ ] Passo 9: Modificar todas as funções `processXXXTouch()` para suportar modo de coleta (com `#ifdef`)
- [ ] Passo 10: Modificar `processTouchStatus()` para gerenciar rastreamento (com `#ifdef`)

### Fase 6 - Detecção de Saída (Opcional)
- [ ] Passo 11: Adicionar `isPointInBounds()` em `WidgetBase` (opcional)
- [ ] Passo 12: Implementar `isPointInBounds()` nos widgets (opcional)
- [ ] Passo 13: Atualizar `checkWidgetsStillTouched()` para usar `isPointInBounds()`

### Fase 7 - Uso de m_isPressed (Opcional)
- [ ] Passo 14: Modificar widgets para usar `m_isPressed` em `redraw()`

### Fase 8 - Testes
- [ ] Passo 15: Teste de retrocompatibilidade
- [ ] Passo 15: Teste de rastreamento básico
- [ ] Passo 15: Teste de múltiplos widgets
- [ ] Passo 15: Teste de saída de área
- [ ] Passo 15: Teste de limites (mais de 5 widgets)
- [ ] Passo 15: Teste de estados (mudança de tela)
- [ ] Passo 15: Teste de `m_isPressed` em `redraw()`
- [ ] Passo 15: Teste com `ENABLE_ON_RELEASE` desabilitado (comentado)

---

---

## Como Desabilitar o Recurso (Compilação Condicional)

### Visão Geral

O recurso `onRelease` pode ser facilmente desabilitado usando compilação condicional com a macro `ENABLE_ON_RELEASE`. Quando desabilitado, o código relacionado ao rastreamento de touch e chamadas de `onRelease()` não será compilado, resultando em zero overhead.

### Configuração

**Localização**: `src/displayfk.h` (no início do arquivo, após includes)

**Código a adicionar**:
```cpp
// Descomentar a linha abaixo para habilitar o recurso onRelease
// Quando comentado, todo o código de rastreamento de touch é desabilitado
// #define ENABLE_ON_RELEASE
```

**Nota**: A macro deve ser definida apenas em `displayfk.h` e usada em `displayfk.cpp`. Não deve ser definida em `user_setup.h` ou outros arquivos.

### Modificações Necessárias

#### 1. Em `displayfk.h`

Adicionar a definição da macro no início do arquivo (após includes, antes da classe):

```cpp
#include "widgets/widgetsetup.h"
#include "../user_setup.h"
// ... outros includes ...

// Descomentar para habilitar rastreamento de touch e onRelease()
// #define ENABLE_ON_RELEASE

class DisplayFK {
    // ...
};
```

#### 2. Em `displayfk.cpp`

Envolver todas as chamadas relacionadas ao rastreamento com `#ifdef ENABLE_ON_RELEASE`:

**Exemplo 1 - Inicialização no construtor**:
```cpp
DisplayFK::DisplayFK() {
    // ... código existente ...
    
#ifdef ENABLE_ON_RELEASE
    // Initialize touch tracking
    m_touchedWidgetsCount = 0;
    m_trackingTouch = false;
    for (uint8_t i = 0; i < 5; i++) {
        m_touchedWidgets[i] = nullptr;
    }
#endif
}
```

**Exemplo 2 - Em `processTouchStatus()` - TOUCH_DOWN**:
```cpp
if(m_lastTouchState == TouchEventType::TOUCH_DOWN){
    Serial.println("---------------------- APERTEI");
    pressPoint.x = xTouch;
    pressPoint.y = yTouch;
    releasePoint.x = xTouch;
    releasePoint.y = yTouch;
    
#ifdef ENABLE_ON_RELEASE
    // Clear previous touched widgets and start tracking
    clearTouchedWidgets(true);
    m_trackingTouch = true;
    // Process widgets in collect mode to populate array
    processTouchableWidgets(xTouch, yTouch, true);
#else
    // Comportamento antigo: processar sem modo de coleta
    processTouchableWidgets(xTouch, yTouch, false);
#endif
}
```

**Exemplo 3 - Em `processTouchStatus()` - TOUCH_HOLD**:
```cpp
else if(m_lastTouchState == TouchEventType::TOUCH_HOLD){
    releasePoint.x = xTouch;
    releasePoint.y = yTouch;
    
#ifdef ENABLE_ON_RELEASE
    // Check if widgets are still being touched
    if (m_trackingTouch && m_touchedWidgetsCount > 0) {
        checkWidgetsStillTouched(xTouch, yTouch);
    }
#endif
}
```

**Exemplo 4 - Em `processTouchStatus()` - TOUCH_UP**:
```cpp
else if(m_lastTouchState == TouchEventType::TOUCH_UP){
    Serial.println("---------------------- SOLTEI");
    // ... existing swipe detection code ...
    
#ifdef ENABLE_ON_RELEASE
    // Call onRelease for all tracked widgets and clear array
    if (m_trackingTouch) {
        clearTouchedWidgets(true);
    }
#endif
}
```

**Exemplo 5 - Em `processTouchStatus()` - NONE**:
```cpp
else if(!hasTouch && m_lastTouchState == TouchEventType::NONE){
    m_lastTouchState = TouchEventType::NONE;
    
#ifdef ENABLE_ON_RELEASE
    // Ensure array is cleared when no touch
    if (m_touchedWidgetsCount > 0) {
        clearTouchedWidgets(false);
    }
#endif
}
```

**Exemplo 6 - Em `processXXXTouch()` - Adicionar widget ao array**:
```cpp
void DisplayFK::processCheckboxTouch(uint16_t xTouch, uint16_t yTouch, bool collectMode) {
#ifdef DFK_CHECKBOX
    // ... código existente ...
    
    if (arrayCheckbox[indice]->detectTouch(&xTouch, &yTouch)) {
        functionCB_t cb = arrayCheckbox[indice]->getCallbackFunc();
        WidgetBase::addCallback(cb, WidgetBase::CallbackOrigin::TOUCH);
        
#ifdef ENABLE_ON_RELEASE
        // Add to touched widgets array if in collect mode
        if (collectMode) {
            addTouchedWidget(arrayCheckbox[indice]);
        }
#endif
        
        if (!collectMode) {
            return;
        }
    }
#endif
}
```

**Exemplo 7 - Declarações de variáveis em `displayfk.h`**:
```cpp
private:
    // ... outras variáveis ...
    
#ifdef ENABLE_ON_RELEASE
    // Touch tracking for onRelease functionality
    WidgetBase* m_touchedWidgets[5];      ///< Array of widgets currently being touched
    uint8_t m_touchedWidgetsCount;        ///< Number of widgets in the touched array
    bool m_trackingTouch;                 ///< Flag indicating if we're tracking touch
#endif
```

**Exemplo 8 - Declarações de métodos em `displayfk.h`**:
```cpp
private:
    // ... outros métodos ...
    
#ifdef ENABLE_ON_RELEASE
    // Touch tracking helper methods
    void addTouchedWidget(WidgetBase* widget);
    void removeTouchedWidget(WidgetBase* widget);
    bool isWidgetInTouchedArray(WidgetBase* widget) const;
    void checkWidgetsStillTouched(uint16_t xTouch, uint16_t yTouch);
    void clearTouchedWidgets(bool callOnRelease = true);
#endif
```

**Exemplo 9 - Implementações de métodos em `displayfk.cpp`**:
```cpp
#ifdef ENABLE_ON_RELEASE
void DisplayFK::addTouchedWidget(WidgetBase* widget) {
    // ... implementação ...
}

void DisplayFK::removeTouchedWidget(WidgetBase* widget) {
    // ... implementação ...
}

// ... outras implementações ...
#endif
```

### Comportamento Quando Desabilitado

Quando `ENABLE_ON_RELEASE` não está definido:

1. **Variáveis de rastreamento**: Não são compiladas (zero overhead de memória)
2. **Métodos auxiliares**: Não são compilados (zero overhead de código)
3. **Chamadas de rastreamento**: Não são executadas
4. **`processTouchableWidgets()`**: Sempre chamado com `collectMode = false` (comportamento original)
5. **`m_isPressed`**: Ainda existe em `WidgetBase`, mas não é atualizado automaticamente
6. **`onRelease()`**: Não é chamado automaticamente, mas ainda pode ser chamado manualmente
7. **Funcionalidade básica**: Mantida (callbacks, detecção de touch funcionam normalmente)

### Vantagens da Abordagem

1. **Zero Overhead**: Quando desabilitado, código não é compilado
2. **Fácil de Habilitar/Desabilitar**: Apenas comentar/descomentar uma linha
3. **Sem Efeitos Colaterais**: Não deixa código inativo no binário
4. **Compatibilidade**: Widgets continuam funcionando normalmente
5. **Manutenibilidade**: Código relacionado fica claramente marcado

### Checklist para Desabilitar

- [ ] Comentar `#define ENABLE_ON_RELEASE` em `displayfk.h`
- [ ] Verificar que todas as chamadas de rastreamento estão envolvidas com `#ifdef`
- [ ] Verificar que variáveis de rastreamento estão envolvidas com `#ifdef`
- [ ] Verificar que métodos auxiliares estão envolvidos com `#ifdef`
- [ ] Compilar e testar que funcionalidade básica ainda funciona
- [ ] Verificar que não há erros de compilação

### Notas Importantes

1. **`m_isPressed` em WidgetBase**: Esta variável **NÃO** deve ser envolvida com `#ifdef`, pois pode ser usada manualmente pelos widgets mesmo quando o rastreamento automático está desabilitado.

2. **`onRelease()` em WidgetBase**: Este método **NÃO** deve ser envolvido com `#ifdef`, pois pode ser chamado manualmente pelos widgets.

3. **`setPressed()` em WidgetBase**: Este método **NÃO** deve ser envolvido com `#ifdef`, pois pode ser usado manualmente pelos widgets.

4. **Modificações em `detectTouch()`**: As chamadas a `setPressed(true)` nos widgets **NÃO** devem ser envolvidas com `#ifdef`, pois `m_isPressed` ainda existe e pode ser usado.

5. **Uso em `redraw()`**: Widgets podem usar `m_isPressed` em `redraw()` mesmo quando rastreamento automático está desabilitado, desde que definam `m_isPressed` manualmente.

### Exemplo de Uso Manual Quando Desabilitado

Mesmo com o rastreamento automático desabilitado, widgets podem usar `m_isPressed` manualmente:

```cpp
bool MyWidget::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) {
    // ... validações ...
    
    if (inBounds) {
        m_isPressed = true;  // Definir manualmente
        m_shouldRedraw = true;
        return true;
    }
    return false;
}

void MyWidget::redraw() {
    // ... validações ...
    
    // Usar m_isPressed mesmo sem rastreamento automático
    uint16_t color = m_isPressed ? COLOR_PRESSED : COLOR_NORMAL;
    // ... desenhar ...
    
    m_shouldRedraw = false;
}

// Em algum lugar do código (ex: em um callback ou timer)
void MyWidget::release() {
    m_isPressed = false;
    m_shouldRedraw = true;
    onRelease();  // Chamar manualmente se necessário
}
```

---

**Data de Criação**: 2024
**Última Atualização**: 2024
**Versão**: 1.1

