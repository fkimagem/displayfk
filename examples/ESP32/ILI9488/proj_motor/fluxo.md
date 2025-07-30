# Fluxograma Detalhado - Sistema de Controle de Motor e Compressor

## Visão Geral do Sistema
Este sistema Arduino controla um motor e compressor com interface gráfica touch, monitoramento de temperatura e controle automático de segurança.

## Fluxograma Principal

```mermaid
flowchart TD
    A["Início do Programa"] --> B["Definições e Includes"]
    B --> C["Declaração de Variáveis Globais"]
    C --> D["Declaração de Objetos de Interface"]
    D --> E["Declaração de Sensores de Temperatura"]
    E --> F["Declaração de Pinos de Saída"]
    
    F --> G["MANUAL - INICIO<br/>Declaração de Variáveis de Controle"]
    G --> H["Declaração de Timer e Flags"]
    H --> I["Função cb_LerSensor"]
    I --> J["MANUAL - FIM"]
    
    J --> K["Função setup"]
    K --> L["Inicialização Serial"]
    L --> M["Configuração de Pinos de Saída"]
    M --> N["Inicialização de LEDs e Relés"]
    N --> O["Inicialização do Display SPI"]
    O --> P["Inicialização do Touch Screen"]
    P --> Q["Inicialização dos Sensores de Temperatura"]
    Q --> R["Carregamento de Widgets"]
    R --> S["Configuração da Tela Inicial"]
    S --> T["Criação da Tarefa de Interface"]
    
    T --> U["MANUAL - INICIO<br/>Criação do Timer de Leitura"]
    U --> V["Inicialização do Timer"]
    V --> W["MANUAL - FIM"]
    
    W --> X["Função loop"]
    X --> Y{"podeLerSensor == true?"}
    Y -->|"Não"| Z["delay 10ms"]
    Y -->|"Sim"| AA["Reset da Flag podeLerSensor"]
    AA --> CC["Requisição de Temperatura dos Sensores"]
    CC --> DD["Delay de 750ms"]
    DD --> EE["Leitura da Temperatura do Motor"]
    EE --> FF["Leitura da Temperatura do Compressor"]
    FF --> GG["Início da Transação de Display"]
    GG --> HH["Atualização do Gráfico com Dados"]
    HH --> II["Atualização do Gauge"]
    II --> JJ["Finalização da Transação"]
    JJ --> KK["Verificação de Temperatura Alta"]
    KK --> LL{"Temperatura Motor >= Limite?"}
    LL -->|"Sim"| MM["Desativa Motor"]
    LL -->|"Não"| NN["Ativa Controle Manual do Motor"]
    MM --> PP
    NN --> PP
    PP{"Temperatura Compressor >= Limite?"}
    PP -->|"Sim"| QQ["Desativa Compressor"]
    PP -->|"Não"| RR["Ativa Controle Manual do Compressor"]
    QQ --> Z
    RR --> Z
    Z --> X
```

## Fluxograma Detalhado das Funções de Callback

```mermaid
flowchart TD
    A["Evento de Touch"] --> B{"Qual Callback?"}
    
    B -->|"btnleda_cb"| C["MANUAL - INICIO<br/>Obtém Status do Botão"]
    C --> D["digitalWrite pinLed_1"]
    D --> E["MANUAL - FIM"]
    
    B -->|"btnledb_cb"| F["MANUAL - INICIO<br/>Obtém Status do Botão"]
    F --> G["digitalWrite pinLed_2"]
    G --> H["MANUAL - FIM"]
    
    B -->|"btnledc_cb"| I["MANUAL - INICIO<br/>Obtém Status do Botão"]
    I --> J["digitalWrite pinLed_3"]
    J --> K["MANUAL - FIM"]
    
    B -->|"btnmotor_cb"| L["Obtém Status do Botão"]
    L --> M["MANUAL - INICIO<br/>Inverte Lógica do Relé"]
    M --> N["digitalWrite pinMotor"]
    N --> O{"Botão Ligado?"}
    O -->|"Sim"| P["Força Bomba Ligar"]
    O -->|"Não"| Q["Continua"]
    P --> R["digitalWrite pinBomba"]
    R --> S["MANUAL - FIM"]
    Q --> S
    S --> T["Log Serial"]
    
    B -->|"btncompressor_cb"| U["Obtém Status do Botão"]
    U --> V["MANUAL - INICIO<br/>Inverte Lógica do Relé"]
    V --> W["digitalWrite pinBomba"]
    W --> X{"Botão Desligado?"}
    X -->|"Sim"| Y["Força Motor Desligar"]
    X -->|"Não"| Z["Continua"]
    Y --> AA["digitalWrite pinMotor"]
    AA --> BB["MANUAL - FIM"]
    Z --> BB
    BB --> CC["Log Serial"]
    
    B -->|"spinmaxmotor_cb"| DD["Atualiza maxTempMotor"]
    B -->|"spinmaxcomp_cb"| EE["Atualiza maxTempCompressor"]
    
    B -->|"rightpng_cb"| FF["MANUAL - INICIO<br/>Carrega Tela de Configuração"]
    FF --> GG["MANUAL - FIM"]
    
    B -->|"homepng_cb"| HH["MANUAL - INICIO<br/>Carrega Tela de Controle"]
    HH --> II["MANUAL - FIM"]
```

## Fluxograma de Configuração dos Widgets

```mermaid
flowchart TD
    A["loadWidgets"] --> B["Configuração do Gauge"]
    B --> C["Configuração dos Labels"]
    C --> D["Configuração do Gráfico de Linha"]
    D --> E["Configuração dos SpinBoxes"]
    E --> F["Configuração dos ToggleButtons"]
    F --> G["Configuração das Imagens"]
    G --> H["Registro dos Widgets na Biblioteca"]
```

## Fluxograma das Telas

```mermaid
flowchart TD
    A["screen0 - Tela de Controle"] --> B["Preenche Fundo Cinza"]
    B --> C["Desenha Título 'Tela de controle'"]
    C --> D["Desenha Área dos LEDs"]
    D --> E["Desenha Área do Motor/Compressor"]
    E --> F["Desenha Todos os Widgets da Tela 0"]
    
    G["screen1 - Tela de Configuração"] --> H["Preenche Fundo Cinza"]
    H --> I["Desenha Título 'Config do motor'"]
    I --> J["Desenha Labels dos Limites"]
    J --> K["Desenha Todos os Widgets da Tela 1"]
```

## Fluxograma do Timer e Leitura de Sensores

```mermaid
flowchart TD
    A["Timer FreeRTOS"] --> B["Callback cb_LerSensor"]
    B --> C["MANUAL - INICIO<br/>Define podeLerSensor = true"]
    C --> D["MANUAL - FIM"]
    D --> E["Loop Principal Detecta Flag"]
    E --> F["Reset da Flag"]
    F --> H["Leitura dos Sensores"]
    H --> I["Atualização da Interface"]
    I --> J["Verificação de Segurança"]
    J --> K["Controle Automático"]
```

## Detalhamento dos Blocos MANUAL - INICIO/FIM

### 1. Declaração de Variáveis de Controle (Linhas 108-115)
```mermaid
flowchart LR
    B --> C["xTimerLeitura"]
    C --> D["podeLerSensor = false"]
```

### 2. Função de Callback do Timer (Linhas 117-121)
```mermaid
flowchart LR
    A["MANUAL - INICIO"] --> B["cb_LerSensor"]
    B --> C["podeLerSensor = true"]
    C --> D["MANUAL - FIM"]
```

### 3. Criação do Timer no Setup (Linhas 175-178)
```mermaid
flowchart LR
    A["MANUAL - INICIO"] --> B["xTimerCreate"]
    B --> C["xTimerStart"]
    C --> D["MANUAL - FIM"]
```

### 4. Loop Principal de Leitura (Linhas 183-230)
```mermaid
flowchart TD
    A["MANUAL - INICIO"] --> B{"podeLerSensor?"}
    B -->|"Sim"| E["Reset Flag"]
    E --> F["Request Temperaturas"]
    F --> G["Delay 750ms"]
    G --> H["Leitura Temperaturas"]
    H --> I["Start Transaction"]
    I --> J["Update Gráfico"]
    J --> K["Update Gauge"]
    K --> L["Finish Transaction"]
    L --> M["Verifica Limites"]
    M --> N["Controle Automático"]
    N --> O["delay 10ms"]
    O --> P["MANUAL - FIM"]
    B -->|"Não"| O
```

### 5. Callbacks dos LEDs (Linhas 520-540)
```mermaid
flowchart LR
    A["MANUAL - INICIO"] --> B["Obtém Status"]
    B --> C["digitalWrite"]
    C --> D["MANUAL - FIM"]
```

### 6. Callback do Motor (Linhas 542-565)
```mermaid
flowchart TD
    A["MANUAL - INICIO"] --> B["Obtém Status"]
    B --> C["Inverte Lógica"]
    C --> D["digitalWrite Motor"]
    D --> E{"Botão Ligado?"}
    E -->|"Sim"| F["Força Bomba"]
    E -->|"Não"| G["Continua"]
    F --> H["digitalWrite Bomba"]
    H --> I["MANUAL - FIM"]
    G --> I
```

### 7. Callback do Compressor (Linhas 567-590)
```mermaid
flowchart TD
    A["MANUAL - INICIO"] --> B["Obtém Status"]
    B --> C["Inverte Lógica"]
    C --> D["digitalWrite Bomba"]
    D --> E{"Botão Desligado?"}
    E -->|"Sim"| F["Força Motor Desligar"]
    E -->|"Não"| G["Continua"]
    F --> H["digitalWrite Motor"]
    H --> I["MANUAL - FIM"]
    G --> I
```

### 8. Callbacks de Navegação (Linhas 592-600)
```mermaid
flowchart LR
    A["MANUAL - INICIO"] --> B["Carrega Tela"]
    B --> C["MANUAL - FIM"]
```

## Resumo dos Blocos MANUAL

Os blocos **MANUAL - INICIO** e **MANUAL - FIM** contêm:

1. **Variáveis de Controle**: Timer e flags para controle do sistema
2. **Timer FreeRTOS**: Sistema de agendamento para leitura periódica dos sensores
3. **Loop Principal**: Lógica de leitura de sensores, atualização da interface e controle automático
4. **Callbacks de Hardware**: Controle direto dos pinos de saída (LEDs, motor, compressor)
5. **Navegação**: Controle de mudança entre telas

Estes blocos representam a lógica de controle específica do sistema, separada da interface gráfica e configuração dos widgets. 