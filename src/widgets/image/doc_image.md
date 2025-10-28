# Documentação da Classe Image

## Visão Geral

A classe `Image` é um widget visual que exibe imagens na tela. Ele herda de `WidgetBase` e fornece funcionalidade completa para criar e gerenciar imagens a partir de diferentes fontes: arquivos (SD, SPIFFS, FATFS) ou dados de pixels embutidos no código.

### Características Principais

- 🖼️ Exibe imagens de múltiplas fontes
- 📁 Suporte a arquivos (SD, SPIFFS, FATFS)
- 💾 Suporte a imagens embutidas (embedded)
- 👆 Suporte a toque com callbacks
- 🎨 Suporte a máscaras de transparência
- 🔄 Suporte a rotação de imagens
- ⚡ Métricas de desempenho integradas
- 💾 Gerenciamento inteligente de memória
- 🎨 Formatos RGB565 e monocromático
- 👁️ Pode ser mostrado/ocultado dinamicamente
- 🔗 Integra-se automaticamente com o sistema DisplayFK

---

## 📋 Estruturas de Configuração

### ImageFromFileConfig

Estrutura para carregar imagens de arquivos:

```cpp
struct ImageFromFileConfig {
  SourceFile source;           // Fonte: SD, SPIFFS ou FATFS
  const char *path;           // Caminho do arquivo
  functionCB_t cb;            // Função callback para toque
  uint16_t backgroundColor;    // Cor de fundo RGB565
};
```

### ImageFromPixelsConfig

Estrutura para carregar imagens de dados de pixels:

```cpp
struct ImageFromPixelsConfig {
  const pixel_t *pixels;      // Dados de pixels (formato depende do display)
  uint16_t width;             // Largura da imagem (> 0)
  uint16_t height;            // Altura da imagem (> 0)
  const uint8_t *maskAlpha;   // Máscara de transparência (opcional)
  functionCB_t cb;            // Função callback para toque
  uint16_t backgroundColor;   // Cor de fundo RGB565
  float angle;                // Ângulo de rotação em graus
};
```

### SourceFile (enum)

Enum para especificar a origem do arquivo:

```cpp
enum class SourceFile {
  SD = 0,     // Cartão SD
  SPIFFS = 1, // Sistema SPIFFS
  EMBED = 2,  // Dados embutidos
  FATFS = 3   // Sistema de arquivos FAT
};
```

### PerformanceMetrics_t

Estrutura que armazena métricas de desempenho:

```cpp
struct PerformanceMetrics_t {
  uint32_t drawCount;              // Número de chamadas a draw()
  uint32_t fileLoadCount;          // Número de carregamentos de arquivo
  uint32_t rotationDrawCount;      // Número de desenhos com rotação
  // ... tempos médios e máximos ...
};
```

---

## 🔧 Métodos Públicos

### Construtor

```cpp
Image(uint16_t _x, uint16_t _y, uint8_t _screen)
```

Cria uma nova imagem na posição especificada.

**Parâmetros:**
- `_x`: Coordenada X do canto superior esquerdo
- `_y`: Coordenada Y do canto superior esquerdo
- `_screen`: Identificador da tela (0 = primeira tela)

**Nota:** Após criar o objeto, é obrigatório chamar `setupFromFile()` ou `setupFromPixels()` antes de usá-lo.

### Destrutor

```cpp
~Image()
```

Libera automaticamente a memória alocada para imagens.

### setupFromFile()

```cpp
void setupFromFile(ImageFromFileConfig &config)
```

Configura a imagem para carregar de um arquivo.

**Parâmetros:**
- `config`: Estrutura `ImageFromFileConfig` com as configurações

**Características:**
- Carrega arquivo do sistema de arquivos (SD, SPIFFS ou FATFS)
- Lê dimensões do arquivo automaticamente
- Aloca memória dinamicamente para pixels
- Suporta máscaras de transparência

### setupFromPixels()

```cpp
void setupFromPixels(ImageFromPixelsConfig &config)
```

Configura a imagem a partir de dados de pixels embutidos.

**Parâmetros:**
- `config`: Estrutura `ImageFromPixelsConfig` com as configurações

**Características:**
- Usa dados de pixels fornecidos (não aloca memória nova)
- Valida dimensões automaticamente
- Suporta rotação de imagem
- Suporta máscaras de transparência

### show()

```cpp
void show()
```

Torna a imagem visível na tela.

### hide()

```cpp
void hide()
```

Oculta a imagem da tela.

### draw()

```cpp
void draw()
```

Desenha a imagem na tela.

### drawBackground()

```cpp
void drawBackground()
```

Desenha o fundo da imagem.

### getMetrics()

```cpp
const PerformanceMetrics_t& getMetrics() const
```

Retorna métricas de desempenho da imagem.

### resetMetrics()

```cpp
void resetMetrics()
```

Reseta as métricas de desempenho.

### printMetrics()

```cpp
void printMetrics() const
```

Imprime métricas de desempenho no Serial.

---

## 🔒 Métodos Privados (Apenas para Referência)

Estes métodos são chamados internamente:

- `detectTouch()`: Detecta toque na imagem
- `redraw()`: Redesenha a imagem na tela
- `forceUpdate()`: Força atualização
- `getCallbackFunc()`: Retorna função callback
- `readFileFromDisk()`: Lê arquivo do disco
- `defineFileSystem()`: Define sistema de arquivos
- `drawRotatedImage()`: Desenha imagem rotacionada
- `validateConfig()`: Valida configuração
- `clearBuffers()`: Limpa buffers de memória

---

## 💻 Como Usar no Arduino IDE

### 📚 Passo 1: Incluir as Bibliotecas

```cpp
#include <displayfk.h>
// Para imagens de arquivos
#include <SD.h>  // ou SPIFFS.h, FFat.h dependendo da fonte
// Incluir outras bibliotecas necessárias
```

### 🏗️ Passo 2: Declarar Objetos Globais

```cpp
DisplayFK myDisplay;

// Criar imagem: Image(canto_x, canto_y, tela)
Image homepng(190, 400, 0);
Image img3dgraphpng(135, 400, 0);

// Criar array de ponteiros
const uint8_t qtdImagem = 2;
Image *arrayImagem[qtdImagem] = {&img3dgraphpng, &homepng};
```

### 📝 Passo 3: Prototipar Funções de Callback

```cpp
void homepng_cb();
void image_callback();
```

### ⚙️ Passo 4: Configurar Display (setup)

```cpp
void setup() {
    Serial.begin(115200);
    
    // Inicializar display
    myDisplay.setDrawObject(tft);
    myDisplay.startTouchGT911(/* ... parâmetros ... */);
    
    // Inicializar sistema de arquivos (se usar arquivos)
    // SD.begin(/* ... */);
    // ou SPIFFS.begin();
    
    loadWidgets();  // Configurar widgets
    myDisplay.loadScreen(screen0);
    myDisplay.createTask(false, 3);
}
```

### 🔧 Passo 5: Configurar Imagens - De Dados Embutidos

```cpp
void loadWidgets() {
    // Configurar imagem de dados embutidos
    ImageFromPixelsConfig configImage = {
        .pixels = img3dgraphpngPixels,
        .width = img3dgraphpngW,
        .height = img3dgraphpngH,
        .maskAlpha = img3dgraphpngMask,
        .cb = nullptr,
        .backgroundColor = CFK_WHITE
    };
    img3dgraphpng.setupFromPixels(configImage);
    
    // Outra imagem com callback
    ImageFromPixelsConfig configImage2 = {
        .pixels = HomepngPixels,
        .width = HomepngW,
        .height = HomepngH,
        .maskAlpha = HomepngMask,
        .cb = homepng_cb,
        .backgroundColor = CFK_WHITE
    };
    homepng.setupFromPixels(configImage2);
    
    // Registrar no DisplayFK
    myDisplay.setImage(arrayImagem, qtdImagem);
}
```

### 🔧 Configurar Imagens - De Arquivos

```cpp
void loadWidgets() {
    // Configurar imagem de arquivo SD
    ImageFromFileConfig configFileImage = {
        .source = SourceFile::SD,
        .path = "/image.bin",
        .cb = image_callback,
        .backgroundColor = CFK_WHITE
    };
    
    Image arquivoImage(100, 100, 0);
    arquivoImage.setupFromFile(configFileImage);
    
    // Registrar no DisplayFK
    Image *arrayImagem[1] = {&arquivoImage};
    myDisplay.setImage(arrayImagem, 1);
}
```

### 🔔 Passo 6: Criar Funções de Callback

```cpp
void homepng_cb() {
    Serial.println("Imagem Home tocada!");
    // Faça algo quando a imagem for tocada
}

void image_callback() {
    Serial.println("Imagem tocada!");
}
```

### 🖥️ Passo 7: Função da Tela

```cpp
void screen0() {
    tft->fillScreen(CFK_WHITE);
    WidgetBase::backgroundColor = CFK_WHITE;
    
    // Desenhar todos os widgets na tela 0
    myDisplay.drawWidgetsOnScreen(0);
}
```

### 🔄 Passo 8: Exemplo Completo

```cpp
void loop() {
    // Mostrar/ocultar imagem dinamicamente
    homepng.show();
    homepng.hide();
    
    // Obter métricas de desempenho
    const auto& metrics = homepng.getMetrics();
    Serial.print("Desenhos: ");
    Serial.println(metrics.drawCount);
    
    // Imprimir todas as métricas
    homepng.printMetrics();
    
    delay(1000);
}
```

---

## 📝 Exemplo Completo

### Exemplo com Dados Embutidos

```cpp
#include <displayfk.h>

DisplayFK myDisplay;
Arduino_DataBus *bus = nullptr;
Arduino_GFX *gfx = nullptr;

// Dados de pixel embutidos (gerados por conversor)
extern const uint16_t myimagePixels[];
extern const uint8_t myimageMask[];
#define MYIMAGE_W 100
#define MYIMAGE_H 100

// Criar imagem
Image minhaImagem(200, 150, 0);

const uint8_t qtdImage = 1;
Image *arrayImage[qtdImage] = {&minhaImagem};

void setup() {
    Serial.begin(115200);
    
    // Inicializar display
    bus = new Arduino_ESP32SPI(/* ... */);
    gfx = new Arduino_ST7789(/* ... */);
    gfx->begin();
    
    myDisplay.setDrawObject(gfx);
    myDisplay.startTouchGT911(/* ... */);
    
    // Configurar imagem
    ImageFromPixelsConfig config = {
        .pixels = myimagePixels,
        .width = MYIMAGE_W,
        .height = MYIMAGE_H,
        .maskAlpha = myimageMask,
        .cb = image_callback,
        .backgroundColor = CFK_WHITE,
        .angle = 0.0f
    };
    minhaImagem.setupFromPixels(config);
    
    myDisplay.setImage(arrayImage, qtdImage);
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

void image_callback() {
    Serial.println("Imagem tocada!");
}
```

---

## 📏 Dicas e Boas Práticas

### 📐 Formatos de Arquivo
- Formato binário proprietário
- Layout: 2 bytes largura, 2 bytes altura, dados pixels
- Para RGB565: 2 bytes por pixel
- Para monocromático: 1 byte por pixel
- Máscara de transparência opcional após pixels

### 🎨 Otimização de Memória
- Imagens embutidas: memória estática (não alocada dinamicamente)
- Imagens de arquivos: alocação dinâmica na leitura
- Imagens grandes podem consumir muita RAM
- Considere converter para formato comprimido se necessário

### 🔔 Callbacks
- Callback é executado quando a imagem é tocada
- Toque detectado dentro da área retangular da imagem
- Mantenha callbacks curtas e rápidas
- Não bloqueie a execução dentro do callback

### ⚡ Performance
- Uso de métricas de desempenho para otimização
- Carregamento de arquivos pode ser lento
- Imagens grandes afetam performance de renderização
- Evite redesenhos desnecessários
- Considere cache de imagens frequentes

### 👥 Usabilidade
- Use imagens com resolução adequada ao display
- Considere a densidade de pixels do display
- Imagens muito pequenas podem ser difíceis de tocar
- Use máscaras de transparência para fundos complexos

### 🎨 Design Visual
- Imagens são renderizadas como retângulos de pixels
- Suporte a máscaras de transparência (alpha channel)
- Fundo usa backgroundColor configurado
- Rotação disponível apenas para imagens embutidas
- Display RGB565: 16 bits por pixel
- Display monocromático: 8 bits por pixel

### 📊 Gerenciamento de Memória
- Imagens de arquivos: memória alocada dinamicamente
- Imagens embutidas: referência a dados estáticos
- Flag m_ownsMemory controla propriedade da memória
- Destrutor libera memória automaticamente se necessário
- Use clearBuffers() para liberar memória manualmente

### 🔧 Métricas de Desempenho
- Rastreia chamadas de desenho e carregamento
- Mede tempos de operação
- Calcula médias e máximos automaticamente
- Use printMetrics() para debug
- Use resetMetrics() para reiniciar contadores

---

## 🔗 Herança de WidgetBase

A classe `Image` herda todos os métodos de `WidgetBase`:

- `isEnabled()` / `setEnabled()`: Habilitar/desabilitar widget
- `isInitialized()`: Verificar se foi configurado
- `m_visible`: Controla visibilidade interna
- `m_shouldRedraw`: Flag para redesenho automático

---

## 🔗 Integração com DisplayFK

O `Image` integra-se automaticamente com o sistema DisplayFK:

1. **Renderização:** Automática quando usando `drawWidgetsOnScreen()`
2. **Detecção de toque:** Automática quando touch é inicializado
3. **Gerenciamento:** Controlado pelo loop principal do DisplayFK
4. **Callbacks:** Executados de forma segura e não-bloqueante
5. **Fontes:** Múltiplas fontes de dados suportadas
6. **Formato:** Adapta-se automaticamente ao tipo de display

---

## 🎨 Detalhes de Renderização

O `Image` é renderizado diretamente:

1. **Dados de Pixel:**
   - RGB565 para displays coloridos (16 bits/pixel)
   - Monocromático para displays mono (8 bits/pixel)
   - Máscara de transparência opcional

2. **Renderização:**
   - Desenha pixels diretamente no buffer da tela
   - Aplica máscara de transparência se disponível
   - Usa backgroundColor para áreas transparentes
   - Suporta rotação de imagem (apenas embutidas)

3. **Otimizações:**
   - Leitura de arquivo por linhas completas
   - Cache de dimensões
   - Métricas de desempenho integradas
   - Gerenciamento inteligente de memória

---

## 🔧 Solução de Problemas

### Imagem não aparece na tela
- Verifique se chamou `setupFromFile()` ou `setupFromPixels()`
- Confirme que as dimensões estão corretas
- Verifique se está na tela correta
- Certifique-se de chamar `myDisplay.setImage()`
- Confirme que dados de pixels não são null

### Imagem de arquivo não carrega
- Verifique se o sistema de arquivos foi inicializado
- Confirme o caminho do arquivo está correto
- Verifique se o arquivo existe e não está corrompido
- Confirme que há memória suficiente para carregar
- Verifique permissões de leitura do arquivo

### Imagem embutida não aparece
- Verifique se os dados de pixels foram incluídos
- Confirme que width e height estão corretos
- Verifique se o ponteiro pixels não é null
- Certifique-se de que o formato está correto

### Problemas de memória
- Imagens grandes podem consumir muita RAM
- Use imagens menores ou comprimidas se necessário
- Libere memória com clearBuffers() se disponível
- Considere usar imagens embutidas ao invés de arquivos

### Performance lenta
- Use métricas para identificar gargalos
- Evite redesenhos desnecessários
- Considere reduzir tamanho da imagem
- Use cache de imagens frequentes
- Evite carregar mesmo arquivo múltiplas vezes

### Toque não detectado
- Verifique se o touch foi inicializado corretamente
- Confirme que o callback foi configurado
- Verifique se a imagem está visível
- Certifique-se de que o widget está habilitado

---

## 📚 Referências

- **Classe Base:** `WidgetBase` (src/widgets/widgetbase.h)
- **DisplayFK Principal:** `DisplayFK` (src/displayfk.h)
- **Exemplos:** examples/Embed_ESP32S3/Display_Test/
- **Cores Padrão:** Definidas em displayfk.h (CFK_COLOR01-CFK_COLOR28, CFK_GREY*, CFK_WHITE, CFK_BLACK)
- **Conversor:** Use ferramentas para converter imagens para formato binário

