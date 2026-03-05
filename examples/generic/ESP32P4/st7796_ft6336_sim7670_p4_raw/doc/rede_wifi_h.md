# rede_wifi.h

Arquivo responsável por conectar ao Wi‑Fi, gerenciar o socket TCP (`WiFiClient`) e interpretar comandos vindos do servidor.

## Visão geral
- Conecta ao AP usando `ssid`/`password` (de `variaveis.h`).
- Mantém `WiFiClient client` para I/O em TCP.
- Envia/recebe linhas de texto no socket.
- Interpreta comandos e seta flags para integrações (Firebase).
- Mostra feedback com `showMessageStatus` e logs na Serial.

## Globais
- `precisaGravarNoFirebase`, `precisaAtualizarNoFirebase`: flags de ação.
- `WiFiClient client`: conexão TCP com `serverIP:serverPort`.

## Funções
- `conectarWiFi()`: conecta ao Wi‑Fi; aguarda `WL_CONNECTED`.
- `conectarSocket()`: conecta `client` ao servidor.
- `enviarDados(const String&)`: envia linha; reconecta se preciso.
- `lerDados()`: lê uma linha até `\n` se houver dados.
- `interpretarComando(const String&)`: processa "gravar"/"atualizar" e responde.

## Diagrama de chamadas (Mermaid)
```mermaid
flowchart TD
    %% Cadeia de conexao
    W0[conectarWiFi] --> W1[WiFi.begin]
    W1 --> W2[aguarda WiFi.status]
    W2 --> W3[showMessageStatus WiFi]
    W3 --> S0[conectarSocket]
    S0 --> S1[client.connect]
    S1 --> S2[showMessageStatus Socket]

    %% Cadeia de envio/recepcao
    E0[enviarDados] --> E1{client conectado}
    E1 -- sim --> E2[client.println]
    E1 -- nao --> S0
    E2 --> E3[showMessageStatus Enviando]

    R0[lerDados] --> R1{client.available}
    R1 -- sim --> R2[readStringUntil]
    R2 --> R3[trim]

    %% Cadeia de comandos
    C0[interpretarComando] --> C1{comando}
    C1 -- gravar --> C2[set precisaGravar]
    C1 -- atualizar --> C3[set precisaAtualizar]
    C1 -- outro --> C4[enviarDados eco]
```

## Fluxo típico
1. `conectarWiFi()` estabelece Wi‑Fi e mostra IP.
2. `conectarSocket()` conecta ao servidor TCP.
3. `enviarDados()` escreve; se cair, reconecta e envia.
4. `lerDados()` retorna linhas limpas.
5. `interpretarComando()` ajusta flags e envia confirmação.


