# WidgetBase Class Reference - RAG Document

## 📋 Overview

**WidgetBase** is an abstract base class that serves as the foundation for all widget implementations in the DisplayFK library. This class cannot be instantiated directly and must be inherited by concrete widget classes.

## 🏗️ Class Architecture

### Class Type

- **Abstract Base Class** - Pure virtual methods prevent direct instantiation
- **Template Pattern** - Defines common interface for all widgets
- **State Management** - Provides robust state control and validation

### Inheritance Hierarchy

```
WidgetBase (Abstract)
├── CheckBox
├── TextButton
├── CircleButton
├── RectButton
├── ToggleButton
├── TouchArea
├── NumberBox
├── TextBox
├── SpinBox
├── Gauge
├── CircularBar
├── LineChart
├── Thermometer
├── LED
└── [Other Widgets...]
```

## 🔧 Core Design Principles

### 1. **RAG Compliance**

- ✅ All member variables use `m_` prefix
- ✅ Doxygen documentation for all public/protected members
- ✅ ESP_LOG instead of Serial.printf for logging
- ✅ Static TAG for logging identification
- ✅ Robust error handling and validation
- ✅ **Configuration Management**: Each widget must contain an `m_config` attribute that represents the widget's configuration struct
- ✅ **Attribute Consolidation**: Class attributes that serve the same function as struct config attributes must be removed and replaced with `m_config` usage in the .cpp file
- ✅ **Memory Management**: Use allocation tracking flags and centralized cleanup methods
- ✅ **Input Validation**: Comprehensive validation of all inputs and configurations
- ✅ **Error Handling**: Robust error handling with graceful degradation
- ✅ **Performance Optimization**: Use CHECK_* macros and caching for optimal performance

### 2. **Abstract Interface**

```cpp
// Pure virtual methods - MUST be implemented by derived classes
virtual bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) = 0;
virtual functionCB_t getCallbackFunc() = 0;
virtual void show() = 0;
virtual void hide() = 0;
virtual void forceUpdate() = 0;
virtual void redraw() = 0;
```

### 3. **State Management**

```cpp
// Core state flags
bool m_visible = true;        ///< Widget visibility
bool m_loaded = false;        ///< Initialization status
bool m_enabled = true;        ///< Widget enabled state
bool m_initialized = false;   ///< Proper initialization flag
bool m_shouldRedraw = true;   ///< Redraw flag for optimization
unsigned long m_myTime = 0;   ///< Timestamp for timing functions (debounce, etc.)
```

## 📚 Public Interface

### Static Members

```cpp
// System-wide configuration
static QueueHandle_t xFilaCallback;    ///< Callback queue
static bool usingKeyboard;             ///< Virtual keyboard state
static uint8_t currentScreen;         ///< Current screen index
static bool showingLog;               ///< Log display state
static bool lightMode;                ///< Light/dark mode
static uint16_t backgroundColor;       ///< Background color

// Display objects (conditional compilation)
static Arduino_GFX *objTFT;           ///< Main display object
static uint16_t screenWidth;          ///< Screen width
static uint16_t screenHeight;         ///< Screen height
```

### Core Methods

```cpp
// Constructor
WidgetBase(uint16_t _x, uint16_t _y, uint8_t _screen);

// Pure virtual methods (must be implemented)
virtual bool detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) = 0;
virtual functionCB_t getCallbackFunc() = 0;
virtual void show() = 0;
virtual void hide() = 0;
virtual void forceUpdate() = 0;
virtual void redraw() = 0;

// State management
bool isInitialized() const;
bool isEnabled() const;
void setEnabled(bool enabled);
bool isValidState() const;
bool validateParameters() const;
bool showingMyScreen();

// Lock management
void lock();
void unlock();
bool isLocked() const;
```

### Utility Methods

```cpp
// Color manipulation
static uint16_t lightenColor565(unsigned short color, float factor);
static uint16_t lightenToWhite565(uint16_t color, float factor);
static uint16_t darkenColor565(unsigned short color, float factor);
static uint16_t blendColors(uint16_t color1, uint16_t color2, float factor);

// Callback management
static void addCallback(functionCB_t callback, CallbackOrigin origin);
```

## 🛡️ Protected Members

### Core Attributes

```cpp
bool m_visible = true;        ///< True if the widget is visible
uint16_t m_xPos;              ///< X position of the widget
uint16_t m_yPos;              ///< Y position of the widget
uint8_t m_screen;             ///< Screen index for the widget
bool m_loaded = false;        ///< True if the widget has been initialized
bool m_enabled = true;        ///< True if the widget is enabled
bool m_initialized = false;   ///< True if the widget is properly initialized
bool m_shouldRedraw = true;   ///< Flag to indicate if the widget should be redrawn
bool m_locked = false;        ///< True if the widget is locked (prevents interaction)
unsigned long m_myTime = 0;   ///< Timestamp for handling timing-related functions (debounce, etc.)
functionCB_t m_callback = nullptr; ///< Callback function to execute when the widget is clicked
```

### Utility Methods

```cpp
// Text handling
void printText(const char* _texto, uint16_t _x, uint16_t _y, uint8_t _datum);
void printText(const char* _texto, uint16_t _x, uint16_t _y, uint8_t _datum, TextBound_t &lastTextBoud, uint16_t _colorPadding);
TextBound_t getTextBounds(const char* str, int16_t x, int16_t y);
static void recalculateTextPosition(const char* _texto, uint16_t *_x, uint16_t *_y, uint8_t _datum);

// Font management
const GFXfont* getBestRobotoBold(uint16_t availableWidth, uint16_t availableHeight, const char* texto);
const GFXfont* getBestFontForArea(const char* text, uint16_t width, uint16_t height, const GFXfont* const fonts[], size_t fontCount);
const char* getFirstLettersForSpace(const char* textoCompleto, uint16_t width, uint16_t height);
const char* getLastLettersForSpace(const char* textoCompleto, uint16_t width, uint16_t height);
void updateFont(FontType _f);

// Graphics utilities
void drawRotatedImageOptimized(uint16_t *image, int16_t width, int16_t height, float angle, int16_t pivotX, int16_t pivotY, int16_t drawX, int16_t drawY);
void showOrigin(uint16_t color);
```

## 🎯 Implementation Guidelines

### 1. **Constructor Pattern**

```cpp
MyWidget::MyWidget(uint16_t _x, uint16_t _y, uint8_t _screen)
    : WidgetBase(_x, _y, _screen), m_myTime(0), m_status(false), m_shouldRedraw(true) {
    // Initialize widget-specific members
    ESP_LOGD(TAG, "MyWidget created at (%d, %d)", _x, _y);
}
```

### 2. **Touch Detection Pattern**

```cpp
bool MyWidget::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) {
    // Early validation checks
    if (!m_visible || !m_initialized || !m_loaded) {
        return false;
    }
  
    if (WidgetBase::usingKeyboard || WidgetBase::currentScreen != m_screen) {
        return false;
    }

    if (!m_enabled) {
        ESP_LOGW(TAG, "Widget is disabled");
        return false;
    }

    // Validate touch coordinates
    if (_xTouch == nullptr || _yTouch == nullptr) {
        ESP_LOGW(TAG, "Touch coordinates are null");
        return false;
    }

    // Widget-specific touch logic
    // ... implementation ...
  
    return true; // or false based on touch detection
}
```

### 3. **State Management Pattern**

```cpp
void MyWidget::setup(const MyWidgetConfig& config) {
    if (!WidgetBase::objTFT) {
        ESP_LOGW(TAG, "TFT not defined on WidgetBase");
        return;
    }
  
    if (m_loaded) {
        ESP_LOGW(TAG, "Widget already configured");
        return;
    }

    // Configure widget-specific settings
    m_config = config;
    m_callback = config.callback;
    m_initialized = true;
    m_loaded = true;
  
    ESP_LOGD(TAG, "Widget configured successfully");
}
```

### 4. **Timing Management Pattern**

```cpp
// Update timestamp for debounce timing
m_myTime = millis();

// Check debounce timing in macros
CHECK_DEBOUNCE_CLICK_BOOL    // Uses m_myTime with TIMEOUT_CLICK
CHECK_DEBOUNCE_REDRAW_BOOL   // Uses m_myTime with TIMEOUT_REDRAW

// Manual debounce check
if (millis() - m_myTime < TIMEOUT_CLICK) {
    return false; // Too soon for another click
}
```

### 5. **Lock Management Pattern**

```cpp
// Lock widget to prevent interaction
myWidget->lock();
if (myWidget->isLocked()) {
    ESP_LOGD(TAG, "Widget is locked, no interaction allowed");
}

// Unlock widget to allow interaction
myWidget->unlock();
if (!myWidget->isLocked()) {
    ESP_LOGD(TAG, "Widget is unlocked, interaction allowed");
}
```

### 6. **Redraw Pattern**

```cpp
void MyWidget::redraw() {
    CHECK_TFT_VOID
    if (!m_visible || !m_initialized) {
        return;
    }
  
    if (WidgetBase::currentScreen != m_screen || !m_loaded || !m_shouldRedraw) {
        return;
    }
  
    m_shouldRedraw = false;
  
    // Widget-specific drawing logic
    // ... implementation ...
}
```

## 🔍 Validation Patterns

### 1. **Early Return Validation**

```cpp
// Always check these conditions first
if (!m_visible || !m_initialized || !m_loaded) {
    return false; // or appropriate return value
}

if (WidgetBase::usingKeyboard || WidgetBase::currentScreen != m_screen) {
    return false;
}

if (!m_enabled) {
    ESP_LOGW(TAG, "Widget is disabled");
    return false;
}

if (m_locked) {
    ESP_LOGW(TAG, "Widget is locked");
    return false;
}
```

### 2. **Parameter Validation**

```cpp
// Validate input parameters
if (_xTouch == nullptr || _yTouch == nullptr) {
    ESP_LOGW(TAG, "Invalid parameters provided");
    return false;
}

// Validate widget state
if (!isValidState()) {
    ESP_LOGW(TAG, "Widget in invalid state");
    return false;
}
```

## 📝 Logging Standards

### 1. **Log Levels**

```cpp
ESP_LOGD(TAG, "Debug information: %s", debugInfo);     // Debug
ESP_LOGI(TAG, "Information: %d", value);              // Info
ESP_LOGW(TAG, "Warning: %s", warningMessage);         // Warning
ESP_LOGE(TAG, "Error: %s", errorMessage);            // Error
```

### 2. **Common Log Patterns**

```cpp
// Widget creation/destruction
ESP_LOGD(TAG, "Widget created at (%d, %d)", m_xPos, m_yPos);
ESP_LOGD(TAG, "Widget destroyed at (%d, %d)", m_xPos, m_yPos);

// State changes
ESP_LOGD(TAG, "Widget %s at (%d, %d)", enabled ? "enabled" : "disabled", m_xPos, m_yPos);
ESP_LOGD(TAG, "Widget shown at (%d, %d)", m_xPos, m_yPos);
ESP_LOGD(TAG, "Widget hidden at (%d, %d)", m_xPos, m_yPos);

// Touch events
ESP_LOGD(TAG, "Widget touched at (%d, %d)", x, y);

// Configuration
ESP_LOGD(TAG, "Widget configured: enabled=%s, visible=%s", 
         m_enabled ? "true" : "false", 
         m_visible ? "true" : "false");
```

## 🚨 Error Handling

### 1. **Common Error Scenarios**

```cpp
// TFT not available
if (!WidgetBase::objTFT) {
    ESP_LOGW(TAG, "TFT not defined on WidgetBase");
    return;
}

// Widget already configured
if (m_loaded) {
    ESP_LOGW(TAG, "Widget already configured");
    return;
}

// Invalid parameters
if (config.size < 10 || config.size > 100) {
    ESP_LOGW(TAG, "Invalid size %d, using default", config.size);
    // Apply default or clamp value
}
```

### 2. **Callback Management**

```cpp
// Add callback with error handling
if (m_callback != nullptr) {
    WidgetBase::addCallback(m_callback, WidgetBase::CallbackOrigin::SELF);
} else {
    ESP_LOGW(TAG, "No callback function set");
}
```

## 🔧 Configuration Management

### Widget Configuration Pattern

```cpp
// Each widget must have a configuration struct
struct WidgetConfig {
    // Widget-specific configuration parameters
    uint16_t param1;
    uint16_t param2;
    functionCB_t callback;
};

// Widget class must contain m_config attribute
class MyWidget : public WidgetBase {
private:
    static const char* TAG;
    MyWidgetConfig m_config;  // Configuration for the widget
  
    // Remove individual attributes that duplicate config functionality
    // ❌ uint16_t m_param1;     // Remove - use m_config.param1
    // ❌ uint16_t m_param2;     // Remove - use m_config.param2
    // ❌ functionCB_t m_callback; // Remove - use m_config.callback
  
public:
    void setup(const MyWidgetConfig& config);
    // ... other methods
};
```

### Configuration Usage Pattern

```cpp
// Constructor - Initialize m_config with default values
MyWidget::MyWidget(uint16_t _x, uint16_t _y, uint8_t _screen)
    : WidgetBase(_x, _y, _screen), m_otherAttribute(0)
{
    m_config = {.param1 = 0, .param2 = 0, .callback = nullptr};
    ESP_LOGD(TAG, "MyWidget created at (%d, %d) on screen %d", _x, _y, _screen);
}

// Setup method - Use m_config
void MyWidget::setup(const MyWidgetConfig& config) {
    m_config = config;
    // Use m_config.param1, m_config.param2, m_config.callback
}

// Methods - Use m_config instead of individual attributes
void MyWidget::redraw() {
    // Use m_config.param1 instead of m_param1
    WidgetBase::objTFT->drawSomething(m_xPos, m_yPos, m_config.param1);
}

functionCB_t MyWidget::getCallbackFunc() { 
    return m_config.callback;  // Use m_config.callback instead of m_callback
}
```

### Configuration Benefits

- **Centralized Configuration**: All widget parameters in one place
- **Reduced Memory**: Eliminates duplicate attributes
- **Consistent Interface**: All widgets follow the same pattern
- **Easy Maintenance**: Changes to configuration affect all usage
- **Type Safety**: Configuration struct ensures parameter consistency

## 🔧 Validation Macros

### Extended Validation Macros

```cpp
// TFT validation macros
CHECK_TFT_BOOL           // Returns false if TFT not available
CHECK_TFT_VOID           // Returns early if TFT not available

// Visibility validation macros
CHECK_VISIBLE_BOOL       // Returns false if not visible
CHECK_VISIBLE_VOID       // Returns early if not visible

// Debounce validation macros (different timeouts)
CHECK_DEBOUNCE_CLICK_BOOL    // Returns false if click debounce time not met
CHECK_DEBOUNCE_CLICK_VOID    // Returns early if click debounce time not met
CHECK_DEBOUNCE_REDRAW_BOOL   // Returns false if redraw debounce time not met
CHECK_DEBOUNCE_REDRAW_VOID   // Returns early if redraw debounce time not met

// State validation macros
CHECK_ENABLED_BOOL       // Returns false if disabled (with logging)
CHECK_ENABLED_VOID       // Returns early if disabled (with logging)
CHECK_LOCKED_BOOL        // Returns false if locked (with logging)
CHECK_LOCKED_VOID        // Returns early if locked (with logging)
CHECK_INITIALIZED_BOOL   // Returns false if not initialized
CHECK_INITIALIZED_VOID   // Returns early if not initialized

// System state validation macros
CHECK_LOADED_BOOL        // Returns false if not loaded
CHECK_LOADED_VOID        // Returns early if not loaded
CHECK_USINGKEYBOARD_BOOL // Returns false if keyboard is active
CHECK_USINGKEYBOARD_VOID // Returns early if keyboard is active
CHECK_CURRENTSCREEN_BOOL // Returns false if not on current screen
CHECK_CURRENTSCREEN_VOID // Returns early if not on current screen
```

### Debounce Constants

```cpp
#define TIMEOUT_CLICK 500   // Debounce timeout for touch/click interactions (in milliseconds)
#define TIMEOUT_REDRAW 50   // Debounce timeout for redraw operations (in milliseconds)
```

### Debounce Macro Guidelines

```cpp
// Use CLICK debounce for touch/interaction functions
bool MyWidget::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) {
    CHECK_DEBOUNCE_CLICK_BOOL  // Use click debounce for touch detection (500ms)
    // ... touch logic
}

void MyWidget::onTouch() {
    CHECK_DEBOUNCE_CLICK_VOID  // Use click debounce for touch handlers
    // ... touch handler logic
}

// Use REDRAW debounce for display/rendering functions
void MyWidget::redraw() {
    CHECK_DEBOUNCE_REDRAW_VOID  // Use redraw debounce for display functions
    // ... redraw logic
}

void MyWidget::update() {
    CHECK_DEBOUNCE_REDRAW_VOID  // Use redraw debounce for update functions
    // ... update logic
}

// Use specific debounce based on function purpose
bool MyWidget::canClick() {
    CHECK_DEBOUNCE_CLICK_BOOL   // Click-related validation
    return true;
}

bool MyWidget::canRedraw() {
    CHECK_DEBOUNCE_REDRAW_BOOL  // Redraw-related validation
    return true;
}
```

### Macro Selection Guide

```cpp
// Use _BOOL macros for functions that return bool
bool MyWidget::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) {
    CHECK_VISIBLE_BOOL         // Returns false if not visible
    CHECK_INITIALIZED_BOOL    // Returns false if not initialized
    CHECK_LOADED_BOOL         // Returns false if not loaded
    CHECK_USINGKEYBOARD_BOOL  // Returns false if keyboard active
    CHECK_CURRENTSCREEN_BOOL  // Returns false if not current screen
    CHECK_DEBOUNCE_CLICK_BOOL // Returns false if click debounce not met
    CHECK_ENABLED_BOOL        // Returns false if disabled
    CHECK_LOCKED_BOOL         // Returns false if locked
    // ... rest of validation
}

// Use _VOID macros for functions that return void
void MyWidget::redraw() {
    CHECK_TFT_VOID            // Returns early if TFT not available
    CHECK_VISIBLE_VOID        // Returns early if not visible
    CHECK_INITIALIZED_VOID   // Returns early if not initialized
    CHECK_LOADED_VOID        // Returns early if not loaded
    CHECK_USINGKEYBOARD_VOID // Returns early if keyboard active
    CHECK_CURRENTSCREEN_VOID // Returns early if not current screen
    CHECK_DEBOUNCE_REDRAW_VOID // Returns early if redraw debounce not met
    // ... rest of validation
}


// Use _BOOL macros for boolean functions
bool MyWidget::isValid() {
    CHECK_TFT_BOOL           // Returns false if TFT not available
    CHECK_VISIBLE_BOOL       // Returns false if not visible
    CHECK_INITIALIZED_BOOL   // Returns false if not initialized
    CHECK_ENABLED_BOOL       // Returns false if disabled
    CHECK_LOCKED_BOOL        // Returns false if locked
    return true;
}

// Use specific debounce macros based on context
void MyWidget::update() {
    CHECK_DEBOUNCE_REDRAW_VOID // Use redraw debounce for update functions
    // ... update logic
}

bool MyWidget::canClick() {
    CHECK_DEBOUNCE_CLICK_BOOL  // Use click debounce for touch functions
    return true;
}
```

### Example Usage in detectTouch()

```cpp
bool MyWidget::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) {
    // Early validation using macros
    CHECK_VISIBLE_BOOL
    CHECK_INITIALIZED_BOOL
    CHECK_LOADED_BOOL
    CHECK_USINGKEYBOARD_BOOL
    CHECK_CURRENTSCREEN_BOOL
    CHECK_DEBOUNCE_CLICK_BOOL
    CHECK_ENABLED_BOOL
    CHECK_LOCKED_BOOL
  
    // Validate coordinates
    if (_xTouch == nullptr || _yTouch == nullptr) {
        ESP_LOGW(TAG, "Touch coordinates are null");
        return false;
    }
  
    // Touch detection logic...
}
```

### Example Usage in void Functions

```cpp
void MyWidget::redraw() {
    CHECK_TFT_VOID
    CHECK_VISIBLE_VOID
    CHECK_INITIALIZED_VOID
    CHECK_LOADED_VOID
    CHECK_CURRENTSCREEN_VOID
    CHECK_DEBOUNCE_REDRAW_VOID
  
    // Redraw logic...
}

void MyWidget::update() {
    CHECK_TFT_VOID
    CHECK_INITIALIZED_VOID
    CHECK_ENABLED_VOID
    CHECK_LOCKED_VOID
    CHECK_DEBOUNCE_REDRAW_VOID
  
    // Update logic...
}
```

### Example Usage in boolean Functions

```cpp
bool MyWidget::isValid() {
    CHECK_TFT_BOOL
    CHECK_VISIBLE_BOOL
    CHECK_INITIALIZED_BOOL
    CHECK_LOADED_BOOL
    CHECK_ENABLED_BOOL
    CHECK_LOCKED_BOOL
  
    // Validation logic...
    return true;
}

bool MyWidget::canClick() {
    CHECK_DEBOUNCE_CLICK_BOOL
    CHECK_USINGKEYBOARD_BOOL
  
    // Click check logic...
    return true;
}

bool MyWidget::canRedraw() {
    CHECK_DEBOUNCE_REDRAW_BOOL
    CHECK_LOADED_BOOL
  
    // Redraw check logic...
    return true;
}

bool MyWidget::isOnCurrentScreen() {
    CHECK_CURRENTSCREEN_BOOL
  
    // Screen check logic...
    return true;
}
```

## 🎨 Best Practices

### 1. **Memory Management**

- Use stack allocation when possible
- Avoid dynamic allocation in constructors
- Clean up resources in destructors
- Use const references for large parameters

### 2. **Performance Optimization**

- Use early returns to avoid unnecessary processing
- Implement `m_shouldRedraw` flag for efficient redraws
- Cache frequently used values
- Minimize calculations in hot paths

### 3. **Code Organization**

- Keep public interface minimal
- Use protected for internal methods
- Group related functionality together
- Follow single responsibility principle

## 📋 Checklist for New Widgets

### ✅ **Required Implementation**

- [ ] Inherit from WidgetBase
- [ ] Implement all pure virtual methods
- [ ] Add widget-specific TAG constant
- [ ] Implement proper constructor
- [ ] Add configuration struct
- [ ] **Add m_config attribute** - Configuration for the widget
- [ ] **Remove duplicate attributes** - Remove class attributes that duplicate config functionality
- [ ] **Use m_myTime for timing** - Inherited from WidgetBase, no need to declare separately
- [ ] Implement setup() method with configuration struct
- [ ] Add proper logging throughout

### ✅ **Configuration Management**

- [ ] **Create configuration struct** - Define WidgetConfig with all parameters
- [ ] **Add m_config attribute** - WidgetConfig m_config in private section
- [ ] **Remove duplicate attributes** - Remove individual attributes that duplicate config
- [ ] **Update constructor** - Initialize m_config with default values
- [ ] **Update setup method** - Use m_config = config pattern
- [ ] **Update all methods** - Use m_config.param instead of m_param
- [ ] **Update getCallbackFunc** - Return m_config.callback
- [ ] **Update destructor** - Clean m_config.callback

### ✅ **Timing Management**

- [ ] **Use inherited m_myTime** - No need to declare separately, inherited from WidgetBase
- [ ] **Update timestamp on interaction** - Set m_myTime = millis() in detectTouch()
- [ ] **Use debounce macros** - Use CHECK_DEBOUNCE_CLICK_* and CHECK_DEBOUNCE_REDRAW_*
- [ ] **Remove duplicate timing variables** - Remove any private unsigned long m_myTime declarations

### ✅ **State Management**

- [ ] Use m_ prefix for all member variables
- [ ] Implement proper initialization
- [ ] Add enabled/disabled state
- [ ] Implement visibility control
- [ ] Add redraw optimization
- [ ] Implement lock management

### ✅ **Error Handling**

- [ ] Validate all input parameters
- [ ] Check widget state before operations
- [ ] Add appropriate error logging
- [ ] Handle edge cases gracefully

### ✅ **Documentation**

- [ ] Add Doxygen comments for all public methods
- [ ] Document all member variables
- [ ] Include usage examples
- [ ] Document configuration options

## 🔗 Related Files

- `src/widgets/widgetbase.h` - Header file
- `src/widgets/widgetbase.cpp` - Implementation
- `src/widgets/widgetsetup.h` - Widget setup utilities
- `src/extras/baseTypes.h` - Base type definitions
- `src/extras/wutils.h` - Widget utilities

## 🎯 **Advanced Patterns from Improved Widgets**

Based on analysis of the improved CheckBox, CircleButton, CircularBar, and GaugeSuper classes, the following advanced patterns have been identified and should be applied to all widgets:

### **Configuration Consolidation Pattern**

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

### **Memory Management Pattern**

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

### **Input Validation Pattern**

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

### **Error Handling Pattern**

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

### **Performance Optimization Pattern**

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

- [RAG Code Guidelines](RAG/CODE_GUIDELINES.md)
- [Widget Development Patterns](RAG/WIDGET_DEVELOPMENT_PATTERNS.md)
- [ESP-IDF Logging](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/log.html)
- [Doxygen Documentation](https://www.doxygen.nl/manual/docblocks.html)

---

**Last Updated:** 2024-01-XX
**Version:** 2.0
**Maintainer:** DisplayFK Team
