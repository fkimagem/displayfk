# Manual de Boas Práticas Modernas de C++ **para ESP32 (Arduino IDE)**  
*(para treinar um agente do Cursor IDE a detectar problemas no seu projeto/library)*

> Contexto: **ESP32 + Arduino core** (Xtensa/ESP-IDF por baixo), ambiente embarcado com **memória limitada**, possíveis restrições de build (**sem exceções e/ou sem RTTI** em muitos projetos), e uso frequente de **ISR**, **FreeRTOS**, Wi‑Fi/BLE etc.  
> Objetivo: regras acionáveis com exemplos “ruim vs bom” para o agente identificar **bugs, UB, fragmentação de heap, travamentos, watchdog resets, concorrência**, e problemas típicos do ecossistema Arduino/ESP32.

---

## Como usar no treinamento

Para cada regra o agente deve:
1. **Detectar** o padrão (heurística).
2. **Classificar severidade**: `erro`, `alto`, `médio`, `baixo`.
3. **Sugerir correção** mínima e segura.
4. **Explicar risco**: travamento, WDT reset, fragmentação de heap, corrida, UB, performance, consumo de energia, manutenção.

---

# 0) Premissas comuns em ESP32/Arduino

## 0.1 Exceções e RTTI podem estar desabilitados
**O que flaggar**
- `throw`, `try/catch`, `dynamic_cast`, `typeid` (podem falhar na build ou aumentar muito o binário).
- Bibliotecas que dependem de RTTI/exceções sem controle.

**Preferir**
- Tratamento de erro por retorno (`bool`, enum, `std::optional`-like) e logs.
- Polimorfismo por interfaces **sem RTTI** (virtual dispatch), ou `std::variant` (quando disponível), ou “tagged union”.

---

## 0.2 Heap é caro: evite fragmentação
**O que flaggar**
- Uso frequente de `new/delete`, `malloc/free`, `String` (Arduino) em loops e callbacks.
- Alocações em caminho quente (telemetria, loop, callbacks Wi‑Fi/BLE, ISR).

**Preferir**
- Buffers estáticos, `std::array`, `StaticJsonDocument` (se usar ArduinoJson), `reserve()` para strings, pools, `std::vector` só se controlado.

---

# 1) Layout, `struct`, padding e serialização (protocolos, BLE, I2C, UART)

## 1.1 Padding: reordene membros (se ABI interna)
**O que flaggar**
- `struct` com `char`/`bool` intercalados com `uint32_t`, `double`, ponteiros → padding grande.
- Estruturas em arrays grandes (impacto real de RAM/cache).

### Ruim
```cpp
struct Ruim {
  uint8_t  a;
  uint32_t b;
  uint8_t  c;
  uint32_t d;
};
```

### Bom (maiores primeiro)
```cpp
struct Bom {
  uint32_t b;
  uint32_t d;
  uint8_t  a;
  uint8_t  c;
};
```

**Heurística**
- Se `sizeof(struct)` for muito maior que a soma “óbvia”, sugerir reorder.

---

## 1.2 `packed` e “cast do buffer” para struct
**Risco**: alinhamento, endianness, UB, leitura desalinhada, crash em algumas CPUs.

### Ruim (muito comum em Arduino)
```cpp
struct __attribute__((packed)) Header {
  uint16_t type;
  uint32_t size;
};

auto* h = reinterpret_cast<const Header*>(buf); // assume alinhamento e endianness
```

### Bom (parse explícito)
```cpp
struct Header { uint16_t type; uint32_t size; };

uint16_t read_u16_le(const uint8_t* p) {
  return uint16_t(p[0]) | (uint16_t(p[1]) << 8);
}
uint32_t read_u32_le(const uint8_t* p) {
  return uint32_t(p[0]) | (uint32_t(p[1]) << 8) |
         (uint32_t(p[2]) << 16) | (uint32_t(p[3]) << 24);
}

Header parse_header(const uint8_t* buf, size_t n) {
  Header h{};
  if (n < 6) return h; // ou sinalize erro
  h.type = read_u16_le(buf + 0);
  h.size = read_u32_le(buf + 2);
  return h;
}
```

**O agente deve sugerir**
- Funções de leitura por bytes + endianness explícita.
- Evitar `reinterpret_cast` em buffers recebidos.

---

# 2) `union`, type punning e alternativas

## 2.1 `union` cru sem controle do membro ativo
**O que flaggar**
- Leitura de membro diferente do último escrito (UB).
- `union` usado para “variant” sem tag.

### Ruim
```cpp
union Value { int i; float f; };
Value v{};
v.f = 3.14f;
int x = v.i; // UB
```

### Bom (tag explícita ou `std::variant` se disponível)
```cpp
enum class ValueType : uint8_t { Int, Float };

struct Value {
  ValueType type{};
  union { int i; float f; };

  static Value make_int(int v)   { Value x; x.type = ValueType::Int;   x.i = v; return x; }
  static Value make_float(float v){ Value x; x.type = ValueType::Float; x.f = v; return x; }
};
```

---

## 2.2 “Olhar bits” de float (caso legítimo)
**Preferir** `std::memcpy` (evita aliasing) em vez de union punning.

### Bom
```cpp
uint32_t float_bits(float f) {
  uint32_t u{};
  static_assert(sizeof(u) == sizeof(f));
  std::memcpy(&u, &f, sizeof(u));
  return u;
}
```

---

# 3) Casts: regra prática para embarcados

## 3.1 Proibir C-style cast
**O que flaggar**
- `(T)x`, especialmente em ponteiros e buffers.
- “funciona no meu board” ≠ seguro.

### Ruim
```cpp
auto* p = (MyMsg*)buf;
```

### Bom
- Parse por bytes (seção 1.2), ou:
```cpp
auto* p = static_cast<MyMsg*>(ptr); // somente se a origem e alinhamento forem garantidos
```

---

## 3.2 `reinterpret_cast` é bug até prova em contrário
**O que flaggar**
- Converters de buffer → struct.
- Reinterpretação de tipos diferentes (strict aliasing).

**Sugestão**
- Isolar em módulo low-level + comentários + asserts de alinhamento.

---

## 3.3 `const_cast` em Arduino C APIs
**O que flaggar**
- Remover `const` para passar em API legado (`char*`) e depois escrever.

### Melhor
- Copiar para buffer mutável quando necessário:
```cpp
std::array<char, 64> tmp{};
strlcpy(tmp.data(), ssid, tmp.size());
wifi_set_ssid(tmp.data()); // API exige char*
```

---

# 4) RTTI e Polimorfismo (ESP32)

## 4.1 Evitar dependência de RTTI
**O que flaggar**
- `dynamic_cast`, `typeid` (pode não compilar se RTTI off; aumenta binário).

### Ruim
```cpp
if (auto* d = dynamic_cast<Derived*>(base)) { ... }
```

### Bom (virtual dispatch)
```cpp
struct Base {
  virtual ~Base() = default;
  virtual void tick() = 0;
};

struct Derived : Base {
  void tick() override { /* ... */ }
};
```

### Alternativa (quando quer “tipo” sem RTTI)
- `enum class Kind` + `switch` (simples e barato) ou `std::variant` (se disponível).

---

# 5) RAII e memória (o coração do embarcado)

## 5.1 Evitar `new/delete` em caminhos quentes
**O que flaggar**
- Alocação dentro de `loop()`, callbacks, handlers HTTP/MQTT, BLE events.

### Ruim
```cpp
void loop() {
  auto* pkt = new Packet();
  send(pkt);
  delete pkt;
}
```

### Bom (stack / estático / pool)
```cpp
void loop() {
  Packet pkt{};
  send(pkt);
}
```

Ou pré-alocar:
```cpp
static Packet pkt{};
```

---

## 5.2 `String` (Arduino) e fragmentação
**O que flaggar**
- Concatenação repetida (`s += ...`) em loop.
- Construção/destruição frequente de `String`.

### Ruim
```cpp
String s;
for (...) {
  s += value; // realoca várias vezes
}
```

### Melhor (reserve)
```cpp
String s;
s.reserve(128);
for (...) {
  s += value;
}
```

### Melhor ainda (C strings / buffer fixo)
```cpp
char buf[128];
snprintf(buf, sizeof(buf), "v=%d", value);
```

---

## 5.3 `std::string` e `std::vector` em Arduino
**O que flaggar**
- Uso sem `reserve()`.
- Uso em callbacks/ISR.
- Uso com realocações frequentes.

**Sugestão**
- `reserve()` e vida longa (alocar uma vez e reutilizar).
- Alternativas: `etl::string`, `etl::vector` (Embedded Template Library) se o projeto permitir.

---

# 6) ISR (Interrupções): regras obrigatórias

## 6.1 Não alocar nem logar dentro de ISR
**O que flaggar**
- `new`, `malloc`, `String`, `std::vector`, `Serial.print`, `delay`, `WiFi`, `BLE`, `xSemaphoreTake` bloqueante dentro de ISR.

### Ruim
```cpp
void IRAM_ATTR onInterrupt() {
  Serial.println("IRQ");  // ruim
  String s = "x";         // ruim
}
```

### Bom (marcar evento e sair rápido)
```cpp
volatile bool irq_flag = false;

void IRAM_ATTR onInterrupt() {
  irq_flag = true;
}

void loop() {
  if (irq_flag) {
    irq_flag = false;
    // tratar fora da ISR
  }
}
```

**Heurística**
- Qualquer função com `IRAM_ATTR` deve ser auditada: “curta, sem bloqueio, sem heap”.

---

# 7) Concorrência: FreeRTOS, callbacks e `volatile`

## 7.1 `volatile` não é sincronização
**O que flaggar**
- Uso de `volatile` como “thread-safety”.
- Acesso a variáveis compartilhadas sem mutex/atomic entre tasks/callbacks.

### Melhor
- Para flags simples: `std::atomic<bool>` (quando disponível) ou `portENTER_CRITICAL`.
- Para dados: `Semaphore`/`Mutex` do FreeRTOS.

```cpp
#include <atomic>
std::atomic<bool> ready{false};
```

---

## 7.2 Filas e buffers entre tasks
**O que flaggar**
- Passar ponteiro para buffer stack entre tasks.
- Escrever em buffer global sem proteção.

**Preferir**
- `xQueueSend` com cópia do payload pequeno.
- Ring buffer protegido.

---

# 8) Watchdog, delays e bloqueios

## 8.1 Bloqueio longo no `loop()`/task
**O que flaggar**
- `delay(5000)` em task crítica.
- Loops de busy-wait sem `vTaskDelay`/yield.
- Chamadas de rede sincrônicas sem timeout.

### Melhor
- Dividir em estados + timeouts.
- `vTaskDelay(pdMS_TO_TICKS(x))` em tasks FreeRTOS.

---

# 9) Logging e Strings: custo e flash (PROGMEM)

## 9.1 Evitar strings grandes na RAM
**O que flaggar**
- Literais grandes concatenados ou armazenados em `String` sem necessidade.

**Sugestão**
- Usar macros/funcionalidades para manter strings em flash quando aplicável (`F("...")` no Arduino) e logs condicionais.

```cpp
Serial.println(F("Mensagem em flash"));
```

*(Obs.: no ESP32, `F()` existe no Arduino core, mas o comportamento pode variar; ainda ajuda a sinalizar intenção.)*

---

# 10) Interfaces de library Arduino (design)

## 10.1 Evitar alocar no construtor se possível
**O que flaggar**
- Construtores que fazem I/O, Wi‑Fi, begin() pesado, alocação grande.
- Inicialização fora de controle do usuário.

**Preferir**
- Construtor leve + método `begin(...)` retornando status.

```cpp
class MyLib {
public:
  bool begin(/*cfg*/);
  void tick();
};
```

---

## 10.2 Evitar singletons globais com ordem de init
**O que flaggar**
- Objetos globais que dependem de outros globais (ordem de inicialização indefinida entre TUs).

**Preferir**
- “Meyers singleton” ou init explícito no `setup()`.

```cpp
MyLib& lib() {
  static MyLib instance;
  return instance;
}
```

---

# 11) Checklist rápido (para o agente)

## Erro / UB / travamento provável
- `reinterpret_cast` buffer→struct (alinhamento/endianness).
- `union` lido no membro errado.
- Alocação/Serial/IO em ISR.
- Base polimórfica sem destrutor virtual (se usar polimorfismo).
- Condição de corrida entre tasks/callbacks sem mutex/atomic.
- Busy-wait sem yield / bloqueio longo → WDT.

## Alto (instabilidade/performance)
- Uso intenso de `String` sem `reserve()` em loop.
- `new/delete` repetido em caminhos quentes.
- `dynamic_cast`/`typeid` em projeto onde RTTI pode estar off.

## Médio/Baixo (manutenção)
- C-style cast.
- Macros no lugar de `constexpr`.
- Padding exagerado (quando ABI permite reorder).

---

# 12) Heurísticas práticas de detecção

O agente pode procurar por:
- `IRAM_ATTR` → validar: sem heap/log/bloqueio.
- `new ` / `delete ` / `malloc` / `free` dentro de `loop`, callbacks (WiFi/BLE/MQTT/HTTP), ISRs.
- Uso de `String` em concat repetida, especialmente em loops.
- `reinterpret_cast<...*>(buf)` e casts C-style em buffers.
- `volatile` usado como “thread-safe” (sem mutex/atomic).
- `#pragma pack` / `__attribute__((packed))`.
- `dynamic_cast` / `typeid` (marcar como “pode exigir RTTI”).

---

## Apêndice A — Padrões de mensagem (templates)

- **Problema**: ISR executa `Serial.print`/alocação.  
  **Risco**: travamento/WDT/latência de interrupção.  
  **Sugestão**: marcar flag/colocar em fila e processar fora da ISR.

- **Problema**: parsing via `reinterpret_cast` assume layout/alinhamento/endianness.  
  **Risco**: UB/crash/leitura errada.  
  **Sugestão**: parse campo a campo com funções LE/BE e `memcpy`.

- **Problema**: uso intenso de `String` em loop sem `reserve()`.  
  **Risco**: fragmentação do heap e resets ao longo do tempo.  
  **Sugestão**: `reserve()` ou buffers fixos (`snprintf`) / reutilização.

---

**Fim.**
