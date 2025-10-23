# Widget Development Patterns - RAG Document

## 📋 Overview

This document defines the **standardized patterns and best practices** for developing widgets in the DisplayFK library, based on analysis of the improved CheckBox, CircleButton, CircularBar, and GaugeSuper classes.

## 🎯 Core Development Patterns

### 1. **Configuration Structure Pattern**

Every widget MUST have a dedicated configuration structure that consolidates all configurable parameters:

```cpp
/// @brief Configuration structure for [WidgetName]
struct [WidgetName]Config {
  // Core parameters
  uint16_t size;                    ///< Primary size parameter
  uint16_t color;                   ///< Primary color
  functionCB_t callback;            ///< Callback function
  
  // Optional parameters with defaults
  uint16_t backgroundColor = 0;     ///< Background color (0 = auto)
  uint16_t borderColor = 0;         ///< Border color (0 = auto)
  uint8_t borderWidth = 0;          ///< Border width (0 = no border)
  bool showLabels = false;         ///< Show labels flag
  
  // Complex parameters
  const char* title = nullptr;      ///< Title text (nullptr = no title)
  const int* intervals = nullptr; ///< Interval array (nullptr = no intervals)
  const uint16_t* colors = nullptr; ///< Color array (nullptr = no colors)
  uint8_t amountIntervals = 0;     ///< Number of intervals
};
```

### 2. **Class Structure Pattern**

Standard class structure for all widgets:

```cpp
class [WidgetName] : public WidgetBase
{
private:
  static const char* TAG;           ///< Logging tag
  static constexpr uint8_t MAX_* = 10; ///< Maximum limits
  
  // Configuration
  [WidgetName]Config m_config;     ///< Configuration for the widget
  
  // Internal state (minimal, avoid redundancy)
  bool m_status;                    ///< Current widget state
  bool m_shouldRedraw;              ///< Redraw flag
  bool m_initialized;               ///< Initialization flag
  
  // Memory management (if needed)
  bool m_*Allocated = false;        ///< Allocation tracking flags
  char* m_* = nullptr;              ///< Dynamic arrays
  
  // Helper methods
  void cleanupMemory();             ///< Centralized memory cleanup
  bool validateConfig(const [WidgetName]Config& config); ///< Config validation
  bool is*Visible() const;          ///< Visibility helpers
  
public:
  // Standard interface
  [WidgetName](uint16_t _x, uint16_t _y, uint8_t _screen);
  virtual ~[WidgetName]();
  
  // Required overrides
  bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) override;
  functionCB_t getCallbackFunc() override;
  void redraw() override;
  void forceUpdate() override;
  void show() override;
  void hide() override;
  
  // Configuration
  void setup(const [WidgetName]Config& config);
  
  // Widget-specific methods
  void setValue(int newValue);
  bool getStatus() const;
  void setStatus(bool status);
};
```

### 3. **Constructor Pattern**

Standard constructor with proper initialization:

```cpp
[WidgetName]::[WidgetName](uint16_t _x, uint16_t _y, uint8_t _screen) 
    : WidgetBase(_x, _y, _screen), 
      m_status(false), m_shouldRedraw(true), m_initialized(false),
      m_*Allocated(false), m_*Allocated(false)
{
  // Initialize with default config
  m_config = {.size = 0, .color = 0, .callback = nullptr, /* other defaults */};
  
  // Initialize dynamic arrays to nullptr
  m_* = nullptr;
  m_* = nullptr;
  
  ESP_LOGD(TAG, "[WidgetName] created at (%d, %d) on screen %d", _x, _y, _screen);
}
```

**IMPORTANT**: Member initialization list order MUST match the declaration order in the .h file to avoid compiler warnings and undefined behavior. Initialize members in the exact same order they are declared in the class definition.

```cpp
// ✅ CORRECT: Initialization order matches declaration order in .h
class Widget : public WidgetBase {
private:
  uint8_t m_dotRadius = 2;           // Declared 1st
  uint8_t m_minSpace = 10;           // Declared 2nd
  bool m_shouldRedraw = false;       // Declared 3rd
  bool m_blocked;                    // Declared 4th
  bool m_valuesAllocated = false;    // Declared 5th
};

Widget::Widget(uint16_t _x, uint16_t _y, uint8_t _screen) 
    : WidgetBase(_x, _y, _screen),
      m_shouldRedraw(true),            // Initialize in declaration order
      m_blocked(false),
      m_valuesAllocated(false)
{
  // m_dotRadius and m_minSpace are already initialized with default values in declaration
}

// ❌ INCORRECT: Wrong order causes compiler warnings
Widget::Widget(uint16_t _x, uint16_t _y, uint8_t _screen) 
    : WidgetBase(_x, _y, _screen),
      m_valuesAllocated(false),        // Wrong order!
      m_blocked(false),
      m_shouldRedraw(true)
{
}
```

### 4. **Destructor Pattern**

Safe cleanup with proper memory management:

```cpp
[WidgetName]::~[WidgetName]()
{
  ESP_LOGD(TAG, "[WidgetName] destroyed at (%d, %d)", m_xPos, m_yPos);
  
  // Use centralized cleanup method
  cleanupMemory();
  
  // Clear callback
  if (m_callback != nullptr) {
    m_callback = nullptr;
  }
  
  // Reset state
  m_loaded = false;
  m_initialized = false;
  m_shouldRedraw = false;
}
```

### 5. **Touch Detection Pattern**

Robust touch detection with comprehensive validation:

```cpp
bool [WidgetName]::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) {
  // Early validation checks using macros
  CHECK_VISIBLE_BOOL
  CHECK_INITIALIZED_BOOL
  CHECK_LOADED_BOOL
  CHECK_USINGKEYBOARD_BOOL
  CHECK_CURRENTSCREEN_BOOL
  CHECK_DEBOUNCE_CLICK_BOOL
  CHECK_ENABLED_BOOL
  CHECK_LOCKED_BOOL

  // Validate touch coordinates
  if (_xTouch == nullptr || _yTouch == nullptr) {
    ESP_LOGW(TAG, "Touch coordinates are null");
    return false;
  }

  // Widget-specific touch logic
  bool withinBounds = /* touch detection logic */;
  
  if (withinBounds) {
    m_myTime = millis();
    // Handle touch
    m_shouldRedraw = true;
    ESP_LOGD(TAG, "[WidgetName] touched at (%d, %d)", *_xTouch, *_yTouch);
    return true;
  }
  
  return false;
}
```

### 6. **Setup Method Pattern**

Robust configuration with validation and memory management:

```cpp
void [WidgetName]::setup(const [WidgetName]Config& config) {
  if (!WidgetBase::objTFT) {
    ESP_LOGW(TAG, "TFT not defined on WidgetBase");
    return;
  }
  
  if (m_loaded) {
    ESP_LOGD(TAG, "[WidgetName] widget already configured");
    return;
  }

  // Validate configuration first
  if (!validateConfig(config)) {
    ESP_LOGE(TAG, "Invalid configuration provided");
    return;
  }

  // Clean up any existing memory
  cleanupMemory();

  // Assign the configuration structure
  m_config = config;
  
  // Handle dynamic memory allocation with proper cleanup
  if (config.title != nullptr) {
    // Clean up existing title
    if (m_titleAllocated && m_title != nullptr) {
      delete[] m_title;
      m_title = nullptr;
      m_titleAllocated = false;
    }
    
    // Allocate new title
    size_t titleLen = strlen(config.title);
    if (titleLen > MAX_TITLE_LENGTH) {
      ESP_LOGW(TAG, "Title length (%d) exceeds maximum limit (%d)", titleLen, MAX_TITLE_LENGTH);
      titleLen = MAX_TITLE_LENGTH;
    }
    
    m_title = new char[titleLen + 1];
    if (m_title != nullptr) {
      strncpy(m_title, config.title, titleLen);
      m_title[titleLen] = '\0';
      m_titleAllocated = true;
    } else {
      ESP_LOGE(TAG, "Failed to allocate memory for title");
      return;
    }
  }
  
  // Initialize widget
  start(); // or equivalent initialization method
  
  m_loaded = true;
  m_initialized = true;
  
  ESP_LOGD(TAG, "[WidgetName] setup completed at (%d, %d)", m_xPos, m_yPos);
}
```

### 7. **Memory Management Pattern**

Centralized and safe memory management:

```cpp
void [WidgetName]::cleanupMemory() {
  // Clean up arrays with allocation tracking
  if (m_intervalsAllocated && m_intervals != nullptr) {
    delete[] m_intervals;
    m_intervals = nullptr;
    m_intervalsAllocated = false;
  }

  if (m_colorsAllocated && m_colors != nullptr) {
    delete[] m_colors;
    m_colors = nullptr;
    m_colorsAllocated = false;
  }

  if (m_titleAllocated && m_title != nullptr) {
    delete[] m_title;
    m_title = nullptr;
    m_titleAllocated = false;
  }
  
  // Reset all pointers to nullptr for safety
  m_intervals = nullptr;
  m_colors = nullptr;
  m_title = nullptr;
}
```

### 8. **Configuration Validation Pattern**

Comprehensive validation of configuration parameters:

```cpp
bool [WidgetName]::validateConfig(const [WidgetName]Config& config) {
  // Validate basic parameters
  if (config.size <= 0) {
    ESP_LOGE(TAG, "Invalid size: %d", config.size);
    return false;
  }
  
  if (config.minValue >= config.maxValue) {
    ESP_LOGE(TAG, "Invalid range: min=%d, max=%d", config.minValue, config.maxValue);
    return false;
  }
  
  if (config.amountIntervals > MAX_SERIES) {
    ESP_LOGW(TAG, "Too many intervals: %d (max: %d)", config.amountIntervals, MAX_SERIES);
    return false;
  }
  
  // Validate arrays if needed
  if (config.amountIntervals > 0) {
    if (config.intervals == nullptr || config.colors == nullptr) {
      ESP_LOGE(TAG, "Intervals or colors array is null");
      return false;
    }
  }
  
  return true;
}
```

### 9. **Redraw Pattern**

Optimized redraw with proper state management:

```cpp
void [WidgetName]::redraw() {
  CHECK_TFT_VOID
  CHECK_VISIBLE_VOID
  CHECK_INITIALIZED_VOID
  CHECK_LOADED_VOID
  CHECK_USINGKEYBOARD_VOID
  CHECK_CURRENTSCREEN_VOID
  CHECK_DEBOUNCE_REDRAW_VOID

  if (!m_shouldRedraw) {
    return;
  }

  ESP_LOGD(TAG, "Redrawing [WidgetName]");
  
  // Widget-specific drawing logic
  // Use m_config for all configuration values
  
  m_shouldRedraw = false;
}
```

### 10. **Helper Methods Pattern**

Utility methods for common operations:

```cpp
// Visibility helpers
bool [WidgetName]::isTitleVisible() const {
  return (m_title != nullptr && strlen(m_title) > 0);
}

bool [WidgetName]::isLabelsVisible() const {
  return m_config.showLabels;
}

// Color helpers
uint16_t [WidgetName]::getUncheckedColor() {
  if (m_config.uncheckedColor != 0) {
    return m_config.uncheckedColor;
  }
  // Auto-calculate based on light/dark mode
  return WidgetBase::lightMode ? CFK_GREY11 : CFK_GREY3;
}
```

## 🔧 Implementation Guidelines

### **Memory Management Rules**

1. **Always use allocation tracking flags** for dynamic memory
2. **Clean up existing memory** before allocating new
3. **Handle allocation failures** gracefully
4. **Reset pointers to nullptr** after cleanup
5. **Use centralized cleanup methods**

### **Configuration Management Rules**

1. **Store configuration in m_config** attribute
2. **Remove redundant class attributes** that duplicate config
3. **Access config values via m_config** in implementation
4. **Validate configuration** before use
5. **Provide sensible defaults** in config struct

### **Error Handling Rules**

1. **Use ESP_LOG macros** for all logging
2. **Validate inputs** at method boundaries
3. **Handle null pointers** gracefully
4. **Provide meaningful error messages**
5. **Use early returns** for error conditions

### **Performance Optimization Rules**

1. **Use CHECK_* macros** for early validation
2. **Cache frequently used values** in m_config
3. **Minimize redundant calculations**
4. **Use const methods** where appropriate
5. **Optimize redraw logic** with m_shouldRedraw flag

## 📊 Quality Metrics

### **Code Quality Checklist**

- [ ] All member variables use `m_` prefix
- [ ] Configuration stored in dedicated struct
- [ ] No redundant attributes (use m_config instead)
- [ ] Proper memory management with tracking flags
- [ ] Comprehensive input validation
- [ ] ESP_LOG used for all logging
- [ ] Static TAG for logging identification
- [ ] Doxygen documentation for all public methods
- [ ] Helper methods for common operations
- [ ] Centralized cleanup methods

### **Memory Safety Checklist**

- [ ] No memory leaks in any scenario
- [ ] Proper cleanup in destructor
- [ ] Allocation failure handling
- [ ] Double-delete prevention
- [ ] Null pointer validation
- [ ] Resource tracking with flags

### **Stability Checklist**

- [ ] Robust error handling
- [ ] Input validation
- [ ] State consistency
- [ ] Graceful degradation
- [ ] Comprehensive logging

## 🎯 Best Practices Summary

1. **Configuration First**: Always use dedicated config structs
2. **Memory Safety**: Track allocations and clean up properly
3. **Validation**: Validate all inputs and configurations
4. **Logging**: Use ESP_LOG with proper tags
5. **Error Handling**: Handle failures gracefully
6. **Performance**: Optimize with early returns and caching
7. **Documentation**: Document all public interfaces
8. **Consistency**: Follow established patterns across all widgets

This document serves as the definitive guide for developing widgets in the DisplayFK library, ensuring consistency, safety, and maintainability across all implementations.
