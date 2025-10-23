# ESP32 C/C++ Library Code Guidelines

This document defines the **coding standards and best practices** for writing, organizing, and reviewing code in ESP32-based C/C++ libraries. These guidelines are optimized for embedded systems development and adapted for use by AI assistants or RAG agents during code analysis.

---

## 📁 Project Type: ESP32 Library

- This project is structured as a **C++ library** built for the **ESP32 platform** using **ESP-IDF** or similar frameworks.
- The codebase is designed for **modularity**, **portability**, and **low resource usage**.
- The RAG agent should analyze the **file structure**, respecting separation between `src`, `include`, and `components` directories where applicable.

---

## 📐 Naming Conventions

### 📦 Files and Folders
- Use **snake_case** for file and directory names.
  - Example: `line_chart.cpp`, `display_driver.h`

### 🔤 Identifiers

| Element             | Style         | Example                |
|---------------------|---------------|------------------------|
| Class names         | PascalCase    | `LineChart`            |
| Struct names        | PascalCase    | `ChartConfig`          |
| Enum names          | PascalCase    | `ChartType`            |
| Member variables    | `m_` prefix + camelCase | `m_chartWidth`     |
| Constants / Macros  | UPPER_CASE    | `MAX_SERIES`           |
| Local variables     | camelCase     | `dataBuffer`           |
| Functions / Methods | camelCase     | `setupConfig()`        |

---

## 🧠 Language Standards

- Use **C++17** or newer if supported by toolchain.
- Code should avoid:
  - **RTTI**
  - **Exceptions**
  - **Heavy STL containers** (`std::map`, `std::vector`, etc.) unless explicitly permitted.
- Favor:
  - `constexpr` over `#define`
  - Strongly-typed `enum class` over raw `enum`

---

## 🧹 Memory Management

- Avoid **heap allocations** where possible.
- Prefer **stack-based or statically allocated arrays**.
- If dynamic memory is necessary:
  - Check for `nullptr` after allocations.
  - Manage ownership and avoid memory leaks.
- No use of `new`/`delete` in loops or ISRs.
- Deep copies must be manually handled for pointer-based config structs.

---

## 🧱 Structs and Configs

- Configurations should be passed by `const&`.
- Use **fixed-size arrays** inside classes (e.g., `colorsSeries[MAX_SERIES]`) for pointer fields.
- Struct comments must follow **Doxygen**:
  - `///<` for field-level comments
  - `/* @brief */` for struct-level or function-level descriptions

### Example

```cpp
struct LineChartConfig {
    uint16_t width;                ///< Width of the chart
    uint16_t height;               ///< Height of the chart
    uint8_t amountSeries;          ///< Number of series
    uint16_t* colorsSeries;        ///< Pointer to color array
};
```

---

## 📄 Class Structure and Attributes

- All class attributes must be **prefixed with `m_`**.
- Keep private attributes encapsulated.
- Comments must appear in the `.h` file using Doxygen format.

### Example

```cpp
class LineChart {
public:
    /**
     * @brief Setup the line chart with given configuration.
     * @param config Configs for object.
     */
    void setup(const LineChartConfig& config);

private:
    LineChartConfig m_config;             ///< Internal config copy
    uint16_t m_colorsSeries[MAX_SERIES];  ///< Buffer for series colors
};
```

Class organization should be:
1. public attributes
2. public methods
3. private attributes
4. private methods
---

## 🗂 File Organization

- Follow a structured layout:

```
/include/
  line_chart.h         <-- Public headers (API)
/src/
  line_chart.cpp       <-- Implementation
/components/
  display/
    display_driver.h/.cpp
/tests/
  test_line_chart.cpp
```

- Header files must include `#ifndef _CLASSNAME \ #define _CLASSNAME`.
- Each `.cpp` file must only include headers it directly needs.

---

## ⚙️ Performance Guidelines

- Prefer integer math over floating point when possible.
- Avoid blocking calls in main loop or tasks.
- Use FreeRTOS primitives for concurrency:
  - `xQueueSend`, `xSemaphoreTake`, etc.
- Use `xTaskCreateStatic()` for tasks (no dynamic allocs).
- Minimize work inside ISRs; defer using queues or task notifications.

---

## 🧪 Testing and Logging

- Use `ESP_LOGI`, `ESP_LOGE`, `ESP_LOGW`, etc. with proper tags.
- Configure log levels via `menuconfig`.
- For performance debugging, toggle GPIO pins and monitor with a logic analyzer.

---

## ✅ Summary Checklist for RAG Agent

- [ ] Are class attributes prefixed with `m_`?
- [ ] Are all comments in `.h` files using Doxygen (`///<`, `@brief`)?
- [ ] Are raw pointers safely copied or replaced by internal buffers?
- [ ] Is dynamic memory avoided or tightly controlled?
- [ ] Are file names and identifiers using consistent naming?
- [ ] Is the configuration passed by `const&` and copied defensively?
- [ ] Are tasks, memory, and logging FreeRTOS/ESP-IDF compliant?
- [ ] Are STL or C++ features limited to lightweight, ESP32-safe constructs?
- [ ] **NEW**: Is configuration consolidated in dedicated struct with `m_config` attribute?
- [ ] **NEW**: Are redundant class attributes removed in favor of config struct?
- [ ] **NEW**: Is memory management using allocation tracking flags?
- [ ] **NEW**: Is input validation comprehensive and robust?
- [ ] **NEW**: Is error handling graceful with proper logging?
- [ ] **NEW**: Are CHECK_* macros used for performance optimization?

---

## 🎯 **Advanced Patterns from Improved Widgets**

Based on analysis of the improved CheckBox, CircleButton, CircularBar, and GaugeSuper classes, the following advanced patterns have been identified and should be applied to all widgets:

### **Configuration Consolidation**

```cpp
// ✅ CORRECT: Use dedicated config struct
struct WidgetConfig {
  uint16_t size;
  uint16_t color;
  functionCB_t callback;
  // ... other parameters
};

class Widget : public WidgetBase {
private:
  WidgetConfig m_config;  // Single source of truth
  // Remove redundant attributes that duplicate config
};
```

### **Memory Management with Tracking**

```cpp
// ✅ CORRECT: Track allocations and clean up safely
class Widget {
private:
  bool m_*Allocated = false;  // Allocation tracking
  char* m_* = nullptr;        // Dynamic arrays
  
  void cleanupMemory();       // Centralized cleanup
};

void Widget::cleanupMemory() {
  if (m_*Allocated && m_* != nullptr) {
    delete[] m_*;
    m_* = nullptr;
    m_*Allocated = false;
  }
}
```

### **Constructor Initialization Order**

```cpp
// ✅ CORRECT: Initialization order matches declaration order in .h file
class Widget : public WidgetBase {
private:
  uint8_t m_dotRadius = 2;           // Declared 1st
  bool m_shouldRedraw = false;       // Declared 2nd
  bool m_blocked;                    // Declared 3rd
  bool m_valuesAllocated = false;    // Declared 4th
};

Widget::Widget(uint16_t _x, uint16_t _y, uint8_t _screen) 
    : WidgetBase(_x, _y, _screen),
      m_shouldRedraw(true),            // Initialize in declaration order
      m_blocked(false),
      m_valuesAllocated(false)
{
  ESP_LOGD(TAG, "Widget created at (%d, %d) on screen %d", _x, _y, _screen);
}

// ❌ INCORRECT: Wrong initialization order causes warnings
Widget::Widget(uint16_t _x, uint16_t _y, uint8_t _screen) 
    : WidgetBase(_x, _y, _screen),
      m_valuesAllocated(false),        // Wrong order!
      m_blocked(false),
      m_shouldRedraw(true)
{
}
```

**CRITICAL**: Member initialization list MUST follow the exact declaration order in the .h file to avoid compiler warnings and undefined behavior.

### **Input Validation**

```cpp
// ✅ CORRECT: Comprehensive validation
bool Widget::validateConfig(const WidgetConfig& config) {
  if (config.size <= 0) {
    ESP_LOGE(TAG, "Invalid size: %d", config.size);
    return false;
  }
  // ... more validations
  return true;
}
```

### **Error Handling**

```cpp
// ✅ CORRECT: Robust error handling
void Widget::setup(const WidgetConfig& config) {
  if (!validateConfig(config)) {
    ESP_LOGE(TAG, "Invalid configuration");
    return;
  }
  
  // Clean up existing memory
  cleanupMemory();
  
  // Handle allocation failures
  m_* = new char[size];
  if (m_* == nullptr) {
    ESP_LOGE(TAG, "Memory allocation failed");
    return;
  }
}
```

### **Performance Optimization**

```cpp
// ✅ CORRECT: Use CHECK_* macros for early returns
void Widget::redraw() {
  CHECK_TFT_VOID
  CHECK_VISIBLE_VOID
  CHECK_INITIALIZED_VOID
  CHECK_LOADED_VOID
  CHECK_DEBOUNCE_REDRAW_VOID
  
  if (!m_shouldRedraw) return;
  
  // Drawing logic using m_config
  m_shouldRedraw = false;
}
```

## 📚 References

- [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/)
- [Modern C++ Best Practices](https://github.com/seisenmann/Modern-Cpp-Best-Practices)
- [Doxygen Style Guide](https://www.doxygen.nl/manual/docblocks.html)
- [Widget Development Patterns](RAG/WIDGET_DEVELOPMENT_PATTERNS.md)
- [WidgetBase Reference](RAG/WIDGETBASE_REFERENCE.md)
