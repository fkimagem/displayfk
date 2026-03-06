# Correções recomendadas – Boas práticas C++ para ESP32 (DisplayFK)

Este documento lista as correções sugeridas com base no *Manual de Boas Práticas Modernas de C++ para ESP32 (Arduino IDE)*. Para cada item são indicados: **arquivo**, **linha(s)** (aproximadas), **problema**, **severidade** e **sugestão de correção**.

**Escopo:** arquivos em `src/`, **excluindo** `src/touch/`.

---

## Índice por severidade

1. [Erro / UB / risco alto](#1-erro--ub--risco-alto)
2. [Alto (estabilidade / desempenho)](#2-alto-estabilidade--desempenho)
3. [Médio (manutenção / portabilidade)](#3-médio-manutenção--portabilidade)

---

## 1. Erro / UB / risco alto

### 1.1 `reinterpret_cast` em callback de timer

| Campo | Valor |
|-------|--------|
| **Arquivo** | `src/displayfk.cpp` |
| **Linha** | ~3794 |
| **Severidade** | erro |
| **Risco** | Se o ID do timer não for o ponteiro `DisplayFK*` usado ao criar o timer, o cast é UB e pode causar crash. |

**Trecho atual:**
```cpp
void DisplayFK::timerCallback(TimerHandle_t xTimer) {
    DisplayFK *instance = reinterpret_cast<DisplayFK *>(pvTimerGetTimerID(xTimer));
    if (instance) {
        instance->m_simulateAutoClick = true;
    }
}
```

**Sugestão de correção:**
- Garantir que em todo `xTimerCreate` deste timer o `pvTimerID` seja sempre `this` (o `DisplayFK*` que criou o timer).
- Documentar no cabeçalho da função: *"pvTimerID deve ser o ponteiro DisplayFK* que criou este timer."*
- Opcional: criar um wrapper type-safe que guarde `DisplayFK*` (ex.: em union com `void*` apenas para a API do FreeRTOS) e use `static_cast` no retorno, em um único ponto.

---

### 1.2 `const_cast` ao desalocar string do pool

| Campo | Valor |
|-------|--------|
| **Arquivo** | `src/displayfk.cpp` |
| **Linha** | ~3811 |
| **Severidade** | erro |
| **Risco** | Remover `const` para passar a uma API que recebe `char*` pode esconder chamadas com string que não veio do pool (ex.: literal); escrita em memória read-only ou buffer inválido. |

**Trecho atual:**
```cpp
void DisplayFK::freeStringFromPool(const char* str) {
    if (str) {
        stringPool.deallocate(const_cast<char*>(str));
    }
}
```

**Sugestão de correção:**
- Opção A: Fazer `StringPool::deallocate(const char* ptr)` e, internamente, só fazer o cast para `char*` após verificar que o ponteiro está dentro do pool (por comparação de endereço com `pool[i]`). Assim a API pública não expõe `const_cast`.
- Opção B: Manter duas APIs: `freeStringFromPool(const char*)` que apenas verifica se o endereço pertence ao pool e chama uma função interna `deallocate(char*)` usada apenas com ponteiros já conhecidos como do pool. Documentar que `freeStringFromPool` não deve ser chamada com literais ou buffers que não tenham sido retornados por `allocateStringSafe`/`allocate`.

---

### 1.3 Buffer estático retornado – reentrância / concorrência

| Campo | Valor |
|-------|--------|
| **Arquivo** | `src/widgets/widgetbase.cpp` |
| **Linhas** | ~273–320 (`getLastLettersForSpace`), ~324–364 (`getFirstLettersForSpace`) |
| **Severidade** | alto |
| **Risco** | Se duas tasks (ou chamadas reentrantes) usarem o retorno ao mesmo tempo, há condição de corrida e dados incorretos. |

**Trecho atual (exemplo):**
```cpp
const char* WidgetBase::getLastLettersForSpace(const char* textoCompleto, uint16_t width, uint16_t height) {
  static char buffer[256];
  // ... preenche buffer ...
  return buffer;
}
```

**Sugestão de correção:**
- Preferir API em que o chamador fornece o buffer, por exemplo:
  ```cpp
  // Retorna número de caracteres escritos ou -1 em erro
  int getLastLettersForSpace(const char* textoCompleto, uint16_t width, uint16_t height,
                             char* out, size_t outSize);
  int getFirstLettersForSpace(const char* textoCompleto, uint16_t width, uint16_t height,
                              char* out, size_t outSize);
  ```
- Atualizar todos os chamadores para usar um buffer local (ex.: `char buf[256]`) ou membro da classe. Se o tamanho máximo for fixo e pequeno, pode-se usar `std::array<char, 256>` como buffer na stack do chamador.

---

### 1.4 Uso de `volatile` como sincronização em LineChart

| Campo | Valor |
|-------|--------|
| **Arquivo** | `src/widgets/linechart/wlinechart.h` |
| **Linhas** | 83, 88 |
| **Severidade** | alto |
| **Risco** | `volatile` não garante atomicidade nem ordem entre tarefas; leituras/escritas entre task de dados e task de draw podem ser rasgadas ou não visíveis. |

**Trecho atual:**
```cpp
volatile bool m_shouldRedraw;
volatile uint32_t m_dataVersion;
```

**Sugestão de correção:**
- Usar tipos atômicos (quando disponíveis no toolchain):
  ```cpp
  #include <atomic>
  std::atomic<bool> m_shouldRedraw{false};
  std::atomic<uint32_t> m_dataVersion{0};
  ```
- Se `std::atomic` não estiver disponível no ambiente, usar um mutex/semáforo ao acessar essas variáveis entre tasks, ou `portENTER_CRITICAL`/`portEXIT_CRITICAL` em trechos curtos, documentando o uso.

---

### 1.5 `malloc`/`free` e cast C-style em LineChart

| Campo | Valor |
|-------|--------|
| **Arquivo** | `src/widgets/linechart/wlinechart.cpp` |
| **Linhas** | 155–159 (free), 279–285 (malloc e free no alloc2D) |
| **Severidade** | alto |
| **Risco** | Múltiplos `malloc`/`free` fragmentam o heap; casts no estilo C escondem erros de tipo. |

**Trecho atual (exemplo):**
```cpp
ptr = (int**)malloc(sizeof(int*) * m_config.amountSeries);
// ...
ptr[i] = (int*)malloc(sizeof(int) * m_amountPoints);
// e free(ptr[i]); free(ptr);
```

**Sugestão de correção:**
- Preferir C++: `std::vector<std::vector<int>>` para `m_ringValues`, `m_snapshotValues`, `m_lastDrawnValues`, com reserva de tamanho em `start()`. Assim a alocação/desalocação é única e automática.
- Se precisar manter C puro: usar um único bloco contíguo, por exemplo:
  ```cpp
  // Um único malloc: ponteiros + dados
  size_t ptrsSize = sizeof(int*) * m_config.amountSeries;
  size_t dataSize = sizeof(int) * m_config.amountSeries * m_amountPoints;
  char* block = (char*)malloc(ptrsSize + dataSize);
  if (!block) return false;
  int** ptr = (int**)block;
  int* data = (int*)(block + ptrsSize);
  for (uint16_t i = 0; i < m_config.amountSeries; i++) {
    ptr[i] = data + i * m_amountPoints;
  }
  // free(block) uma vez no clear
  ```
- Substituir casts C-style por `static_cast<int**>(malloc(...))` e `static_cast<int*>(...)` onde for necessário manter malloc.

---

### 1.6 Uso de `String` (Arduino) em caminho de exibição

| Campo | Valor |
|-------|--------|
| **Arquivo** | `src/widgets/numberbox/wnumberbox.cpp` |
| **Linhas** | 12–14 |
| **Severidade** | alto |
| **Risco** | Se `convertoToString` for usada em `redraw()`/loop/callbacks, cada chamada aloca no heap e pode fragmentar. |

**Trecho atual:**
```cpp
String NumberBox::convertoToString(double f) {
  return String(f);
}
```

**Sugestão de correção:**
- Evitar retornar `String`. Preferir buffer fornecido pelo chamador:
  ```cpp
  // No .h: void convertoToString(double f, char* buf, size_t bufSize) const;
  void NumberBox::convertoToString(double f, char* buf, size_t bufSize) const {
    if (buf && bufSize > 0) {
      snprintf(buf, bufSize, "%.*f", (int)m_config.decimalPlaces, f);
    }
  }
  ```
- Ou usar `CharString` com buffer fixo interno e retornar `const char*` (ou `CharString` por valor com buffer pequeno), garantindo que não haja alocação dinâmica no caminho quente.

---

## 2. Alto (estabilidade / desempenho)

### 2.1 Vários `new`/`delete[]` em Label

| Campo | Valor |
|-------|--------|
| **Arquivo** | `src/widgets/label/wlabel.cpp` |
| **Linhas** | 119, 142, 179; 365, 373, 383 |
| **Severidade** | alto |
| **Risco** | Muitas chamadas a `setText`/`setPrefix`/`setSuffix` podem fragmentar o heap. |

**Sugestão de correção:**
- Para prefix/suffix/text com tamanho limitado: usar buffers fixos, por exemplo:
  ```cpp
  static constexpr size_t MAX_PREFIX = 32;
  static constexpr size_t MAX_SUFFIX = 32;
  static constexpr size_t MAX_TEXT   = 128;
  char m_prefix[MAX_PREFIX];
  char m_suffix[MAX_SUFFIX];
  char m_text[MAX_TEXT];
  ```
- Inicializar com `m_prefix[0] = '\0'` etc. e em `setPrefix`/`setSuffix`/`setText` usar `strncpy`/`snprintf` com limite, sem `new`/`delete`. Se precisar de tamanho variável maior, manter uma única alocação por membro e fazer `reserve`/realloc apenas quando o novo tamanho exceder o atual, para reduzir realocações.

---

### 2.2 `new` em GaugeSuper (título e arrays)

| Campo | Valor |
|-------|--------|
| **Arquivo** | `src/widgets/gauge/wgaugesuper.cpp` |
| **Linhas** | 583–590 (título), 606–620 (intervals/colors) |
| **Severidade** | alto |

**Sugestão de correção:**
- Título: usar buffer fixo `char m_title[MAX_TITLE_LENGTH+1]` (já existe `MAX_TITLE_LENGTH`) e copiar com `strncpy`/`snprintf`.
- Intervals e colors: se o máximo for limitado (ex.: `MAX_SERIES`), usar `std::array<int, MAX_SERIES>` e `std::array<uint16_t, MAX_SERIES>` (ou arrays C fixos) e um contador `m_amountIntervals`. Assim evita-se `new`/`delete` em `setConfig`.

---

### 2.3 `new` em Image (pixels, lineBuffer, mask)

| Campo | Valor |
|-------|--------|
| **Arquivo** | `src/widgets/image/wimage.cpp` |
| **Linhas** | 157, 175, 217 |
| **Severidade** | alto |

**Sugestão de correção:**
- `m_pixels`: manter alocação dinâmica se o tamanho da imagem for variável; garantir um único `delete[]` no destrutor/clear e evitar realocação desnecessária.
- `lineBuffer`: usar buffer na stack se `lineSize` for limitado (ex.: `uint8_t lineBuffer[LINE_BUFFER_MAX]`) ou um buffer membro reutilizável (ex.: `std::vector<uint8_t> m_lineBuffer` reservado uma vez) em vez de `new uint8_t[lineSize]` a cada carga.
- `m_maskAlpha`: idem: se o tamanho máximo for conhecido, considerar buffer membro fixo; caso contrário, manter um único `new[]`/`delete[]` e documentar ownership.

---

### 2.4 `new` em RadioGroup (array de botões)

| Campo | Valor |
|-------|--------|
| **Arquivo** | `src/widgets/radiobutton/wradiogroup.cpp` |
| **Linha** | 208 |
| **Severidade** | alto |

**Trecho atual:**
```cpp
m_buttons = new radio_t[config.amount];
```

**Sugestão de correção:**
- Se o número máximo de botões for limitado (ex.: 8 ou 16), usar array fixo: `radio_t m_buttons[MAX_RADIO_BUTTONS]` e `uint8_t m_buttonsCount`. Em `setConfig`, copiar até `min(config.amount, MAX_RADIO_BUTTONS)` e setar `m_buttonsCount`.
- Se o número for variável e precisar de heap, considerar `std::vector<radio_t>` para uma única região alocada e liberação automática.

---

### 2.5 Funções em `color.h` que retornam ponteiros para `new`

| Campo | Valor |
|-------|--------|
| **Arquivo** | `src/extras/color.h` |
| **Linhas** | 175, 198, 237 |
| **Severidade** | alto |
| **Risco** | Chamador deve fazer `delete[]`; uso em caminho quente aumenta fragmentação. |

**Sugestão de correção:**
- Preferir API em que o chamador fornece o buffer, por exemplo:
  ```cpp
  bool gerarDegradeHSV(float h, int numTons, uint16_t* out, int outSize);
  bool blendColors(uint16_t startColor, uint16_t endColor, int numTons, uint16_t* out, int outSize);
  bool blendColorsRGB(uint16_t c1, uint16_t c2, int numTons, uint16_t* out, int outSize);
  ```
- Documentar que `out` deve ter pelo menos `numTons` elementos. Se precisar manter a API que retorna ponteiro, documentar em destaque: *"O chamador deve liberar com delete[]"* e evitar chamar essas funções em loop/callbacks.

---

## 3. Médio (manutenção / portabilidade)

### 3.1 C-style cast para ponteiro nulo de fonte

| Campo | Valor |
|-------|--------|
| **Arquivos e linhas** | Ver tabela abaixo |
| **Severidade** | médio |

| Arquivo | Linha |
|---------|--------|
| `src/widgets/widgetbase.cpp` | 364 |
| `src/displayfk.cpp` | 3449, 3457 |
| `src/widgets/numberbox/numpad.cpp` | 540 |
| `src/widgets/label/wlabel.cpp` | 284 |
| `src/widgets/linechart/wlinechart.cpp` | 348 |

**Trecho atual:**
```cpp
WidgetBase::objTFT->setFont((GFXfont *)0);
```

**Sugestão de correção:**
- Se a API de display aceitar `nullptr`: usar `objTFT->setFont(nullptr)`.
- Caso contrário: usar `static_cast<GFXfont*>(nullptr)` em um único helper, por exemplo em `widgetbase` ou em um header de compatibilidade:
  ```cpp
  inline void setFontNull() {
    if (WidgetBase::objTFT) WidgetBase::objTFT->setFont(static_cast<GFXfont*>(nullptr));
  }
  ```
  e substituir todas as ocorrências de `(GFXfont *)0` por essa chamada.

---

### 3.2 Typo no parâmetro de `startTouchCST816`

| Campo | Valor |
|-------|--------|
| **Arquivo** | `src/displayfk.h` |
| **Linha** | 436 |
| **Severidade** | médio |
| **Problema** | Tipo inexistente `uintint8_t8_t` (typo). |

**Trecho atual:**
```cpp
void startTouchCST816(uint16_t w, uint16_t h, uint8_t _rotation, uintint8_t8_t pinSDA, int8_t pinSCL, int8_t pinINT, int8_t pinRST);
```

**Sugestão de correção:**
- Corrigir para o mesmo tipo usado nos outros `startTouch*`, por exemplo:
  ```cpp
  void startTouchCST816(uint16_t w, uint16_t h, uint8_t _rotation, int8_t pinSDA, int8_t pinSCL, int8_t pinINT, int8_t pinRST);
  ```

---

### 3.3 Estrutura `StringAllocation_t` – padding

| Campo | Valor |
|-------|--------|
| **Arquivo** | `src/displayfk.h` |
| **Linhas** | 171–175 |
| **Severidade** | baixo/médio |
| **Problema** | Entre `bool isFromPool` e `uint32_t timestamp` pode haver padding (ex.: +3 bytes) em algumas plataformas. |

**Trecho atual:**
```cpp
typedef struct {
    char* ptr;
    bool isFromPool;
    uint32_t timestamp;
    const char* caller;
} StringAllocation_t;
```

**Sugestão de correção (opcional):**
- Reordenar membros por tamanho decrescente para reduzir padding:
  ```cpp
  typedef struct {
      char* ptr;
      const char* caller;
      uint32_t timestamp;
      bool isFromPool;
  } StringAllocation_t;
  ```
- Só é relevante se houver muitos elementos em array ou preocupação com tamanho/cache.

---

## Resumo por arquivo

| Arquivo | Itens |
|---------|--------|
| `src/displayfk.cpp` | 1.1, 1.2, 3.1 |
| `src/displayfk.h` | 3.2, 3.3 |
| `src/widgets/widgetbase.cpp` | 1.3, 3.1 |
| `src/widgets/linechart/wlinechart.cpp` | 1.5, 3.1 |
| `src/widgets/linechart/wlinechart.h` | 1.4 |
| `src/widgets/label/wlabel.cpp` | 2.1, 3.1 |
| `src/widgets/numberbox/wnumberbox.cpp` | 1.6 |
| `src/widgets/gauge/wgaugesuper.cpp` | 2.2 |
| `src/widgets/image/wimage.cpp` | 2.3 |
| `src/widgets/radiobutton/wradiogroup.cpp` | 2.4 |
| `src/extras/color.h` | 2.5 |
| `src/widgets/numberbox/numpad.cpp` | 3.1 |

---

*Documento gerado com base no Manual de Boas Práticas Modernas de C++ para ESP32 (Arduino IDE) e na análise do código da library DisplayFK (excl. `src/touch/`).*
