#include "wimage.h"
#include <esp_log.h>

const char* Image::TAG = "Image";

/**
 * @brief Construtor da classe Image.
 * @param _x Posição X da imagem.
 * @param _y Posição Y da imagem.
 * @param _screen Número da tela.
 * @details Inicializa o widget com valores padrão e configuração vazia.
 *          A imagem não será funcional até que setupFromFile() ou setupFromPixels() seja chamado.
 */
Image::Image(uint16_t _x, uint16_t _y, uint8_t _screen)
    : WidgetBase(_x, _y, _screen),
#if defined(USING_GRAPHIC_LIB)
      m_pixels(nullptr),
#endif
      m_maskAlpha(nullptr), m_fs(nullptr), m_path(nullptr) {

        //initialize m_config with default values
        m_config = {
          .pixels = nullptr,
          .width = 0,
          .height = 0,
          .maskAlpha = nullptr,
          .cb = nullptr,
          .backgroundColor = 0,
          .angle = 0.0f
        };
}

/**
 * @brief Destrutor da classe Image.
 * @details Limpa todos os buffers e libera memória alocada.
 */
Image::~Image() { clearBuffers(); }

/**
 * @brief Detecta se a Image foi tocada.
 * @param _xTouch Ponteiro para a coordenada X do toque na tela.
 * @param _yTouch Ponteiro para a coordenada Y do toque na tela.
 * @return True se o toque está dentro da área da imagem, False caso contrário.
 * @details Este método realiza múltiplas validações antes de processar o toque:
 *          - Verifica se o widget está visível, inicializado, carregado e habilitado
 *          - Valida que o teclado virtual não está ativo
 *          - Verifica que a imagem está na tela atual
 *          - Aplica debounce para evitar múltiplos cliques
 *          - Verifica se o widget não está bloqueado
 *          - Verifica se o toque está dentro dos limites da imagem
 *          Se todas as validações passarem, marca o widget para redesenho.
 */
bool Image::detectTouch(uint16_t *_xTouch, uint16_t *_yTouch) {
  // Early validation checks using macros
  CHECK_VISIBLE_BOOL
  CHECK_LOADED_BOOL
  CHECK_USINGKEYBOARD_BOOL
  CHECK_CURRENTSCREEN_BOOL
  CHECK_POINTER_TOUCH_NULL_BOOL
  CHECK_LOCKED_BOOL
  CHECK_INITIALIZED_BOOL
  CHECK_DEBOUNCE_CLICK_BOOL
  CHECK_ENABLED_BOOL

  


  bool detected = POINT_IN_RECT(*_xTouch, *_yTouch, m_xPos, m_yPos, m_config.width, m_config.height);
if(detected) {
  m_myTime = millis();
  setPressed(true);  // Mark widget as pressed
}
  

  return detected;
}

/**
 * @brief Recupera a função callback associada à imagem.
 * @return Ponteiro para a função callback.
 * @details Retorna o ponteiro para a função que será executada quando a imagem for tocada.
 */
functionCB_t Image::getCallbackFunc() { return m_callback; }

/**
 * @brief Lê o arquivo de imagem do disco (SD, SPIFFS ou FATFS).
 * @return True se o arquivo foi lido com sucesso, False caso contrário.
 * @details Este método lê um arquivo de imagem do sistema de arquivos:
 *          - Abre o arquivo e valida suas dimensões
 *          - Lê dados de pixels no formato apropriado (RGB565 ou monocromático)
 *          - Lê a máscara de transparência se disponível
 *          - Aloca memória dinamicamente para os dados
 *          - Registra métricas de desempenho para otimização
 *          - Suporta leitura otimizada por linhas completas
 */
bool Image::readFileFromDisk() {
  if (m_source == SourceFile::EMBED) {
    ESP_LOGE(TAG, "Cannot read from embedded source");
    return false;
  }

  if (!m_fs) {
    ESP_LOGE(TAG, "No source defined to find image");
    return false;
  }

  ESP_LOGD(TAG, "Looking for file: %s", m_path);

  // Start file load performance timing
  uint32_t fileLoadStartTime = micros();
  m_metrics.fileLoadCount++;

  fs::File file = m_fs->open(m_path, "r");
  if (!file) {
    ESP_LOGE(TAG, "Cant open file");
    return false;
  }

  if (file.isDirectory()) {
    ESP_LOGE(TAG, "Path is a directory");
    file.close();
    return false;
  }

  if (!file.available()) {
    ESP_LOGE(TAG, "File is empty");
    file.close();
    return false;
  }

  size_t size = file.size();
  if (size < 4) {
    ESP_LOGE(TAG, "File is too small");
    file.close();
    return false;
  }

  // Get width
  uint16_t arqWidth = ((file.read()) << 8) | file.read();
  // Get height
  uint16_t arqHeight = ((file.read()) << 8) | file.read();

  if (arqWidth == 0 || arqHeight == 0) {
    ESP_LOGE(TAG, "Invalid image size");
    file.close();
    return false;
  }

  ESP_LOGD(TAG, "Image size: %d x %d", arqWidth, arqHeight);

  // Store dimensions in m_config
  m_config.width = arqWidth;
  m_config.height = arqHeight;

  uint32_t bytesOfColor = arqWidth * arqHeight;

  m_pixels = new pixel_t[bytesOfColor];

  if (!m_pixels) {
    ESP_LOGE(TAG, "Failed to allocate memory for image pixels");
    file.close();
    return false;
  }

  memset(m_pixels, 0, bytesOfColor * sizeof(uint16_t));

#if defined(DISP_DEFAULT)
  const uint8_t read_pixels = 2;
#elif defined(DISP_PCD8544) || defined(DISP_SSD1306) || defined(DISP_U8G2)
  const uint8_t read_pixels = 1;
#endif

  // Optimized reading: read entire lines at once
  const uint32_t lineSize = arqWidth * read_pixels;
  uint8_t *lineBuffer = new uint8_t[lineSize];
  
  if (!lineBuffer) {
    ESP_LOGE(TAG, "Failed to allocate line buffer");
    file.close();
    clearBuffers();
    return false;
  }

  for (int y = 0; y < arqHeight; y++) {
    // Read entire line at once
    if (file.read(lineBuffer, lineSize) != lineSize) {
      ESP_LOGE(TAG, "Error reading line %d", y);
      delete[] lineBuffer;
      file.close();
      clearBuffers();
      return false;
    }
    
    // Process line pixels
    for (int x = 0; x < arqWidth; x++) {
#if defined(DISP_DEFAULT)
      uint16_t color = (lineBuffer[x * 2] << 8) | lineBuffer[x * 2 + 1];
      m_pixels[y * arqWidth + x] = color;
#elif defined(DISP_PCD8544) || defined(DISP_SSD1306) || defined(DISP_U8G2)
      uint8_t color = lineBuffer[x];
      m_pixels[y * arqWidth + x] = color;
#endif
    }
  }
  
  delete[] lineBuffer;

  uint16_t maskLen = ((file.read()) << 8) | file.read();

  if (maskLen == 0) {
    ESP_LOGE(TAG, "Invalid mask length");
    file.close();
    clearBuffers();
    return false;
  }

  m_maskAlpha = new uint8_t[maskLen];

  if (!m_maskAlpha) {
    ESP_LOGE(TAG, "Failed to allocate memory for image mask");
    file.close();
    return false;
  }

  memset(m_maskAlpha, 0, maskLen);

  for (int i = 0; i < maskLen; i++) {
    m_maskAlpha[i] = file.read();
  }

  file.close();
  
  // End file load performance timing
  uint32_t fileLoadEndTime = micros();
  m_metrics.lastFileLoadTime = fileLoadEndTime - fileLoadStartTime;
  m_metrics.totalFileLoadTime += m_metrics.lastFileLoadTime;
  if (m_metrics.lastFileLoadTime > m_metrics.maxFileLoadTime) {
    m_metrics.maxFileLoadTime = m_metrics.lastFileLoadTime;
  }
  
  return true;
}

/**
 * @brief Desenha o fundo do widget de imagem.
 * @details Preenche a área da imagem com a cor de fundo. Apenas desenha se o widget
 *          está na tela atual e precisa de atualização.
 *          Valida a configuração usando o método centralizado validateConfig().
 */
void Image::drawBackground() {
  CHECK_TFT_VOID
  CHECK_VISIBLE_VOID
  CHECK_CURRENTSCREEN_VOID
  CHECK_USINGKEYBOARD_VOID
  CHECK_LOADED_VOID
  CHECK_SHOULDREDRAW_VOID
  
  // Validate configuration using centralized method
  if (!validateConfig()) {
    ESP_LOGW(TAG, "Invalid configuration for background drawing");
    return;
  }
  
  WidgetBase::objTFT->fillRect(m_xPos, m_yPos, m_config.width, m_config.height, m_config.backgroundColor);
  ESP_LOGD(TAG, "Background drawn: %dx%d at (%d,%d)", m_config.width, m_config.height, m_xPos, m_yPos);
}

/**
 * @brief Desenha a imagem na tela.
 * @details Carrega e renderiza a imagem de sua fonte (arquivo ou pixels) com rotação opcional.
 *          Apenas desenha se o widget está na tela atual e precisa de atualização:
 *          - Aplica rotação se o ângulo não for zero usando drawRotatedImage()
 *          - Usa draw16bitRGBBitmapWithMask() para formatos RGB565
 *          - Usa drawBitmap() para displays monocromáticos
 *          - Registra métricas de desempenho para otimização
 *          - Desenha fundo se o widget não estiver visível
 */
void Image::draw() {
  CHECK_TFT_VOID
  CHECK_VISIBLE_VOID
  CHECK_CURRENTSCREEN_VOID
  CHECK_USINGKEYBOARD_VOID
  CHECK_LOADED_VOID
  CHECK_SHOULDREDRAW_VOID

#if defined(USING_GRAPHIC_LIB) || defined(DISP_U8G2)

  m_shouldRedraw = false;

  // If not visible, draw background only
  if (!m_visible) {
    // Validate configuration before drawing background
    if (validateConfig()) {
      WidgetBase::objTFT->fillRect(m_xPos, m_yPos, m_config.width, m_config.height, m_config.backgroundColor);
      ESP_LOGD(TAG, "Image hidden - background drawn: %dx%d", m_config.width, m_config.height);
    } else {
      ESP_LOGW(TAG, "Cannot draw background - invalid configuration");
    }
    return;
  }

  // Validate configuration before drawing
  if (!validateConfig()) {
    ESP_LOGW(TAG, "Cannot draw image - invalid configuration");
    return;
  }

  // Start performance timing
  uint32_t startTime = micros();
  m_metrics.drawCount++;

  ESP_LOGD(TAG, "Redraw image: %d x %d (angle: %.1f°)", m_config.width, m_config.height, m_config.angle);

  // Apply rotation if angle is not zero
  if (m_config.angle != 0.0f) {
    drawRotatedImage();
  } else {
    // Draw without rotation
#if defined(DISP_DEFAULT)
    WidgetBase::objTFT->draw16bitRGBBitmapWithMask(
        m_xPos, m_yPos, m_config.pixels, m_config.maskAlpha, m_config.width, m_config.height);
#elif defined(DISP_PCD8544) || defined(DISP_SSD1306)
    WidgetBase::objTFT->drawBitmap(m_xPos, m_yPos, m_config.pixels, m_config.width, m_config.height,
                                   CFK_BLACK);
#elif defined(DISP_U8G2)
    WidgetBase::objTFT->drawXBMP(m_xPos, m_yPos, m_config.width, m_config.height, m_config.pixels);
#endif
  }
  
  // End performance timing
  uint32_t endTime = micros();
  m_metrics.lastDrawTime = endTime - startTime;
  m_metrics.totalDrawTime += m_metrics.lastDrawTime;
  if (m_metrics.lastDrawTime > m_metrics.maxDrawTime) {
    m_metrics.maxDrawTime = m_metrics.lastDrawTime;
  }
#endif
}

/**
 * @brief Redesenha a imagem na tela.
 * @details Chama simplesmente o método draw() para renderizar a imagem.
 *          Toda a validação é tratada pelo próprio método draw().
 */
void Image::redraw() {
  draw();
}

/**
 * @brief Força a imagem a atualizar.
 * @details Define a flag de atualização para disparar um redesenho no próximo ciclo.
 */
void Image::forceUpdate() { 
  m_shouldRedraw = true; 
  ESP_LOGD(TAG, "Image force update requested");
}


/**
 * @brief Configura o widget Image com uma fonte de arquivo.
 * @param config Estrutura @ref ImageFromFileConfig contendo fonte de arquivo, caminho, callback e ângulo de rotação.
 * @details Configura a imagem com uma fonte de arquivo, caminho, função callback e ângulo de rotação:
 *          - Limpa buffers existentes usando clearBuffers()
 *          - Define sistema de arquivos usando defineFileSystem()
 *          - Carrega arquivo do disco usando readFileFromDisk()
 *          - Mapeia dados carregados para configuração unificada
 *          - Valida configuração usando validateConfig()
 *          - Marca widget como carregado e inicializado
 *          A imagem não será exibida corretamente até que este método seja chamado.
 */
void Image::setupFromFile(ImageFromFileConfig &config) {
  if (!WidgetBase::objTFT) {
    ESP_LOGW(TAG, "TFT not defined on WidgetBase");
    return;
  }

  if (m_loaded) {
    ESP_LOGW(TAG, "Image widget already configured");
    return;
  }

  // Validate file configuration
  if (config.path == nullptr) {
    ESP_LOGE(TAG, "File path is null");
    return;
  }

  if (strlen(config.path) == 0) {
    ESP_LOGE(TAG, "File path is empty");
    return;
  }

  // Clean up any existing memory
  clearBuffers();

  // Set up file system and load file
  m_source = config.source;
  m_path = config.path;
  m_callback = config.cb;
  //m_backgroundColor = config.backgroundColor;
  //m_angle = 0.0f; // File rotation not implemented yet
  m_ownsMemory = true; // We will own the memory for file images

  // Define file system
  defineFileSystem(config.source);

  // Load file from disk
  if (readFileFromDisk()) {
    // Map loaded data to unified configuration
    m_config.pixels = m_pixels;
    m_config.maskAlpha = m_maskAlpha;
    m_config.cb = config.cb;
    m_config.backgroundColor = config.backgroundColor;
    m_config.angle = 0.0;

    // Validate loaded configuration
    if (!validateConfig()) {
      ESP_LOGE(TAG, "Loaded image configuration is invalid");
      clearBuffers();
      return;
    }

    m_loaded = true;
    m_shouldRedraw = true;
    m_initialized = true;
    
    ESP_LOGD(TAG, "Image setup from file completed at (%d, %d) - %dx%d from %s", 
             m_xPos, m_yPos, m_config.width, m_config.height, config.path);
  } else {
    ESP_LOGE(TAG, "Failed to load image from file: %s", config.path);
  }
}


/**
 * @brief Configura o widget Image com dados de pixels.
 * @param config Estrutura @ref ImageFromPixelsConfig contendo dados de pixels, dimensões, máscara alfa e callback.
 * @details Configura a imagem com dados de pixels, dimensões, máscara de transparência e função callback:
 *          - Limpa buffers existentes usando clearBuffers()
 *          - Armazena configuração para validação
 *          - Valida usando validateConfig() método centralizado
 *          - Usa referência direta para evitar overhead de memória para imagens embutidas
 *          - Mapeia para variáveis legacy para compatibilidade
 *          - Marca widget como carregado e inicializado
 *          Ideal para imagens embutidas no código como arrays constantes.
 */
void Image::setupFromPixels(ImageFromPixelsConfig &config) {
  if (!WidgetBase::objTFT) {
    ESP_LOGW(TAG, "TFT not defined on WidgetBase");
    return;
  }

  if (m_loaded) {
    ESP_LOGW(TAG, "Image widget already configured");
    return;
  }

  // Clean up any existing memory
  clearBuffers();

  // Store configuration first for validation
  m_config = config;
  
  // Validate configuration using centralized method
  if (!validateConfig()) {
    ESP_LOGE(TAG, "Invalid pixel configuration provided");
    m_config = {}; // Reset to default
    return;
  }

  
  m_ownsMemory = false; // We don't own the memory for embedded images
  
  // Map to legacy variables for compatibility
  m_pixels = const_cast<pixel_t*>(config.pixels);
  m_maskAlpha = const_cast<uint8_t*>(config.maskAlpha);
  m_callback = config.cb;
  //m_backgroundColor = config.backgroundColor;
  m_source = SourceFile::EMBED;

  m_loaded = true;
  m_shouldRedraw = true;
  m_initialized = true;
  
  ESP_LOGD(TAG, "Image setup from pixels completed at (%d, %d) - %dx%d", 
           m_xPos, m_yPos, m_config.width, m_config.height);
}

/**
 * @brief Define o sistema de arquivos para a imagem.
 * @param source Fonte do arquivo de imagem (@ref SourceFile).
 * @details Configura o ponteiro do sistema de arquivos baseado na fonte:
 *          - SD: usa WidgetBase::mySD se disponível
 *          - SPIFFS: monta SPIFFS se necessário
 *          - FATFS: monta FFat se necessário
 *          - EMBED: retorna erro (embutido não precisa de sistema de arquivos)
 */
void Image::defineFileSystem(SourceFile source) {
  if (source == SourceFile::SD) {
    if (!WidgetBase::mySD) {
      ESP_LOGE(TAG, "SD not configured");
      return;
    }
    m_fs = WidgetBase::mySD;
  } else if (source == SourceFile::SPIFFS) {
    #if defined(USE_SPIFFS)
    if (!SPIFFS.begin(false)) {
      ESP_LOGE(TAG, "SPIFFS Mount Failed");
      return;
    }
    m_fs = &SPIFFS;
    #else
    ESP_LOGE(TAG, "USE_SPIFFS is not defined in widgetsetup.h");
    return;
    #endif
  } else if (source == SourceFile::FATFS) {
    #if defined(USE_FATFS)
    if (!FFat.begin(false)) {
      ESP_LOGE(TAG, "FAT Mount Failed");
        return;
      }
      m_fs = &FFat;
      #else
      ESP_LOGE(TAG, "USE_FATFS is not defined in widgetsetup.h");
      return;
    #endif
  } else {
    ESP_LOGE(TAG, "Invalid source");
    return;
  }
}

/**
 * @brief Imprime métricas de desempenho no log.
 * @details Exibe estatísticas completas de desempenho incluindo tempos de desenho,
 *          tempos de carregamento de arquivos, tempos de rotação e suas médias:
 *          - Número de operações de desenho, carregamento e rotação
 *          - Tempo último, médio e máximo para cada operação
 *          - Tempo total acumulado
 */
void Image::printMetrics() const {
  ESP_LOGI(TAG, "=== Image Performance Metrics ===");
  ESP_LOGI(TAG, "Draw Operations: %u calls", m_metrics.drawCount);
  ESP_LOGI(TAG, "File Loads: %u calls", m_metrics.fileLoadCount);
  ESP_LOGI(TAG, "Rotation Draws: %u calls", m_metrics.rotationDrawCount);
  ESP_LOGI(TAG, "");
  ESP_LOGI(TAG, "Draw Times (μs):");
  ESP_LOGI(TAG, "  Last: %u", m_metrics.lastDrawTime);
  ESP_LOGI(TAG, "  Average: %.2f", m_metrics.getAverageDrawTime());
  ESP_LOGI(TAG, "  Maximum: %u", m_metrics.maxDrawTime);
  ESP_LOGI(TAG, "  Total: %u", m_metrics.totalDrawTime);
  ESP_LOGI(TAG, "");
  ESP_LOGI(TAG, "File Load Times (μs):");
  ESP_LOGI(TAG, "  Last: %u", m_metrics.lastFileLoadTime);
  ESP_LOGI(TAG, "  Average: %.2f", m_metrics.getAverageFileLoadTime());
  ESP_LOGI(TAG, "  Maximum: %u", m_metrics.maxFileLoadTime);
  ESP_LOGI(TAG, "  Total: %u", m_metrics.totalFileLoadTime);
  ESP_LOGI(TAG, "");
  ESP_LOGI(TAG, "Rotation Times (μs):");
  ESP_LOGI(TAG, "  Last: %u", m_metrics.lastRotationTime);
  ESP_LOGI(TAG, "  Average: %.2f", m_metrics.getAverageRotationTime());
  ESP_LOGI(TAG, "  Maximum: %u", m_metrics.maxRotationTime);
  ESP_LOGI(TAG, "  Total: %u", m_metrics.totalRotationTime);
  ESP_LOGI(TAG, "");
  // print m_loaded
  ESP_LOGI(TAG, "Loaded: %d", m_loaded);
  // print m_shouldRedraw
  ESP_LOGI(TAG, "Should Redraw: %d", m_shouldRedraw);
  // print m_visible
  ESP_LOGI(TAG, "Visible: %d", m_visible);
  // print m_initialized
  ESP_LOGI(TAG, "Initialized: %d", m_initialized);
  ESP_LOGI(TAG, "=====================================");
}

/**
 * @brief Desenha a imagem com rotação aplicada.
 * @details Implementa rotação pixel por pixel usando cálculos trigonométricos:
 *          - Converte ângulo para radianos e calcula seno/cosseno
 *          - Calcula dimensões da caixa delimitadora rotacionada
 *          - Para cada pixel na área rotacionada, aplica rotação inversa para obter coordenadas originais
 *          - Desenha apenas se coordenadas originais estão dentro dos limites
 *          - Suporta máscara de transparência
 *          - Registra métricas de desempenho para otimização
 *          Rotação ao redor do centro da imagem.
 */
void Image::drawRotatedImage() {
  // Start rotation performance timing
  uint32_t rotationStartTime = micros();
  m_metrics.rotationDrawCount++;
  
  // Convert angle to radians
  float angleRad = m_config.angle * PI / 180.0f;
  float cosAngle = cos(angleRad);
  float sinAngle = sin(angleRad);
  
  // Calculate center point
  float centerX = m_config.width / 2.0f;
  float centerY = m_config.height / 2.0f;
  
  // Calculate rotated dimensions (bounding box)
  float cosAbs = fabs(cosAngle);
  float sinAbs = fabs(sinAngle);
  int rotatedWidth = (int)(m_config.width * cosAbs + m_config.height * sinAbs);
  int rotatedHeight = (int)(m_config.width * sinAbs + m_config.height * cosAbs);
  
  ESP_LOGD(TAG, "Drawing rotated image: %dx%d -> %dx%d (%.1f°)", 
           m_config.width, m_config.height, rotatedWidth, rotatedHeight, m_config.angle);
  
  // Draw rotated image pixel by pixel
  for (int y = 0; y < rotatedHeight; y++) {
    for (int x = 0; x < rotatedWidth; x++) {
      // Calculate position relative to rotated center
      float relX = x - rotatedWidth / 2.0f;
      float relY = y - rotatedHeight / 2.0f;
      
      // Apply inverse rotation to get original coordinates
      float origX = relX * cosAngle + relY * sinAngle + centerX;
      float origY = -relX * sinAngle + relY * cosAngle + centerY;
      
      // Check if original coordinates are within bounds
      if (origX >= 0 && origX < m_config.width && origY >= 0 && origY < m_config.height) {
        int origXInt = (int)origX;
        int origYInt = (int)origY;
        int pixelIndex = origYInt * m_config.width + origXInt;
        
        // Draw pixel with rotation
        int screenX = m_xPos + x - rotatedWidth / 2 + m_config.width / 2;
        int screenY = m_yPos + y - rotatedHeight / 2 + m_config.height / 2;
        
        if (screenX >= 0 && screenX < WidgetBase::objTFT->width() && 
            screenY >= 0 && screenY < WidgetBase::objTFT->height()) {
          
#if defined(DISP_DEFAULT)
          uint16_t color = m_config.pixels[pixelIndex];
          // Apply alpha mask if available
          if (m_config.maskAlpha && m_config.maskAlpha[pixelIndex] < 255) {
            // Skip transparent pixels
            continue;
          }
          WidgetBase::objTFT->drawPixel(screenX, screenY, color);
#elif defined(DISP_PCD8544) || defined(DISP_SSD1306) || defined(DISP_U8G2)
          uint8_t color = m_config.pixels[pixelIndex];
          if (color != 0) { // Only draw non-transparent pixels
            WidgetBase::objTFT->drawPixel(screenX, screenY, color);
          }
#endif
        }
      }
    }
  }
  
  // End rotation performance timing
  uint32_t rotationEndTime = micros();
  m_metrics.lastRotationTime = rotationEndTime - rotationStartTime;
  m_metrics.totalRotationTime += m_metrics.lastRotationTime;
  if (m_metrics.lastRotationTime > m_metrics.maxRotationTime) {
    m_metrics.maxRotationTime = m_metrics.lastRotationTime;
  }
}

/**
 * @brief Valida a configuração atual da imagem.
 * @return True se a configuração é válida, False caso contrário.
 * @details Realiza validação abrangente da configuração da imagem incluindo
 *          dimensões, dados de pixels e compatibilidade com display. Centraliza toda
 *          a lógica de validação para garantir consistência em todo o widget:
 *          - Verifica se pixels não são nulos
 *          - Valida dimensões não-zero e dentro de limites razoáveis (max 4096x4096)
 *          - Valida contagem de pixels não-overflow
 *          - Normaliza ângulo para faixa 0-360°
 *          - Verifica se display está inicializado
 *          - Valida posição dentro dos limites do display
 *          - Considera bounding box para imagens rotacionadas
 */
bool Image::validateConfig() {
  // Validate basic configuration
  if (m_config.pixels == nullptr) {
    ESP_LOGE(TAG, "Image pixels are null");
    return false;
  }
  
  if (m_config.width == 0 || m_config.height == 0) {
    ESP_LOGE(TAG, "Invalid image dimensions: %dx%d", m_config.width, m_config.height);
    return false;
  }
  
  // Validate reasonable size limits
  if (m_config.width > 4096 || m_config.height > 4096) {
    ESP_LOGE(TAG, "Image dimensions too large: %dx%d (max: 4096x4096)", 
             m_config.width, m_config.height);
    return false;
  }
  
  // Validate pixel count doesn't overflow
  uint32_t pixelCount = static_cast<uint32_t>(m_config.width) * static_cast<uint32_t>(m_config.height);
  if (pixelCount == 0 || pixelCount > 16777216) { // 16M pixels max
    ESP_LOGE(TAG, "Invalid pixel count: %u", pixelCount);
    return false;
  }
  
  // Validate angle is reasonable
  if (m_config.angle < -360.0f || m_config.angle > 360.0f) {
    ESP_LOGW(TAG, "Angle out of range: %.1f° (normalizing to 0-360°)", m_config.angle);
    // Normalize angle to 0-360 range
    while (m_config.angle < 0.0f) m_config.angle += 360.0f;
    while (m_config.angle >= 360.0f) m_config.angle -= 360.0f;
  }
  
  // Validate display compatibility
  if (!WidgetBase::objTFT) {
    ESP_LOGE(TAG, "TFT display not initialized");
    return false;
  }
  
  // Validate position is within display bounds
  if (m_xPos < 0 || m_yPos < 0) {
    ESP_LOGW(TAG, "Image position is negative: (%d, %d)", m_xPos, m_yPos);
    return false;
  }
  
  // Check if image fits within display (with rotation consideration)
  int maxWidth = WidgetBase::objTFT->width();
  int maxHeight = WidgetBase::objTFT->height();
  
  if (m_config.angle == 0.0f) {
    // Simple bounds check for non-rotated images
    if (m_xPos + m_config.width > maxWidth || m_yPos + m_config.height > maxHeight) {
      ESP_LOGW(TAG, "Image extends beyond display bounds: (%d+%d, %d+%d) > (%d, %d)", 
               m_xPos, m_config.width, m_yPos, m_config.height, maxWidth, maxHeight);
      return false;
    }
  } else {
    // For rotated images, check bounding box
    float angleRad = m_config.angle * PI / 180.0f;
    float cosAbs = fabs(cos(angleRad));
    float sinAbs = fabs(sin(angleRad));
    int rotatedWidth = (int)(m_config.width * cosAbs + m_config.height * sinAbs);
    int rotatedHeight = (int)(m_config.width * sinAbs + m_config.height * cosAbs);
    
    if (m_xPos + rotatedWidth > maxWidth || m_yPos + rotatedHeight > maxHeight) {
      ESP_LOGW(TAG, "Rotated image extends beyond display bounds: (%d+%d, %d+%d) > (%d, %d)", 
               m_xPos, rotatedWidth, m_yPos, rotatedHeight, maxWidth, maxHeight);
      return false;
    }
  }
  
  ESP_LOGD(TAG, "Configuration validation passed: %dx%d at (%d,%d) angle=%.1f°", 
           m_config.width, m_config.height, m_xPos, m_yPos, m_config.angle);
  return true;
}

/**
 * @brief Limpa todos os buffers e redefine gerenciamento de memória.
 * @details Gerencia limpeza de memória baseada se a imagem é embutida ou carregada de arquivo:
 *          - Limpa configuração unificada
 *          - Para imagens embutidas: apenas limpa referências
 *          - Para imagens de arquivo: desaloca memória se m_ownsMemory é true
 *          - Limpa referências de sistema de arquivos
 *          - Reseta flags de estado (m_ownsMemory, m_loaded, m_shouldRedraw)
 *          Previne vazamentos de memória através de gerenciamento inteligente de propriedade.
 */
void Image::clearBuffers() {
  // Clear unified configuration
  m_config.pixels = nullptr;
  m_config.maskAlpha = nullptr;
  m_config.width = 0;
  m_config.height = 0;
  m_config.cb = nullptr;
  m_config.backgroundColor = 0x0000;
  m_config.angle = 0.0f;
  
  // Clear legacy variables
  if (m_pixels) {
    // Only delete if we own the memory
    if (m_ownsMemory) {
      delete[] m_pixels;
    }
    m_pixels = nullptr;
  }
  
  if (m_maskAlpha) {
    // Only delete if we own the memory
    if (m_ownsMemory) {
      delete[] m_maskAlpha;
    }
    m_maskAlpha = nullptr;
  }
  
  // Clear file system references
  if (m_fs) {
    m_fs = nullptr;
  }
  if (m_path) {
    m_path = nullptr;
  }
  
  
  // Reset state
  m_ownsMemory = false;
  m_loaded = false;
  m_shouldRedraw = false;
}


/**
 * @brief Torna o widget de imagem visível.
 * @details Torna a imagem visível e marca para redesenho.
 *          Funciona apenas se o widget estiver adequadamente carregado.
 */
void Image::show() {
  if (!m_loaded) {
    ESP_LOGW(TAG, "Cannot show unloaded image widget");
    return;
  }
  
  m_visible = true;
  m_shouldRedraw = true;
  ESP_LOGD(TAG, "Image shown at (%d, %d)", m_xPos, m_yPos);
}

/**
 * @brief Oculta o widget de imagem.
 * @details Torna a imagem invisível e marca para redesenho.
 *          Funciona apenas se o widget estiver adequadamente carregado.
 */
void Image::hide() {
  if (!m_loaded) {
    ESP_LOGW(TAG, "Cannot hide unloaded image widget");
    return;
  }
  
  m_visible = false;
  m_shouldRedraw = true;
  ESP_LOGD(TAG, "Image hidden at (%d, %d)", m_xPos, m_yPos);
}