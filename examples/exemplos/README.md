# Exemplos DisplayFK — ESP32 / ESP32-S3

Este diretório reúne **exemplos prontos** de widgets da biblioteca **DisplayFK** para ESP32/ESP32-S3, com código `.ino`, documentação `.md`, projeto do **EdgeWX** (`.json`) e um **print da tela** (`screenshot.png`).  
Além dos exemplos, há imagens e arquivos de montagem para referência rápida.

---

## Estrutura da pasta

```
exemplos/
├─ README.md                     ← este arquivo
├─ montagem.png                  ← montagem padrão (ESP32)
├─ montagemS3.png                ← montagem para ESP32-S3
├─ ligacoes.png                  ← tabela de pinos (ESP32 e ESP32-S3)
├─ montagemFritizing-ESP32devkit.fzz
├─ montagemFritizing-ESP32s3.fzz
├─ <nome-exemplo-1>/
│  ├─ <nome-exemplo-1>.ino      ← código Arduino
│  ├─ <nome-exemplo-1>.md       ← descrição do exemplo
│  ├─ <nome-exemplo-1>.json     ← projeto EdgeWX
│  └─ screenshot.png            ← print da tela do widget
├─ <nome-exemplo-2>/
│  ├─ ...
└─ ...
```

> **Padrão de nomes nas imagens dos exemplos:**  
> Use sempre **`montagem`** (imagem de montagem) e **`screenshot`** (print da tela) nos `.md`.

---

## Como usar um exemplo

1. **Escolha a placa:**  
   - Para **ESP32 (DevKit)**, confira `montagem.png`.  
   - Para **ESP32-S3**, confira `montagemS3.png`.
2. **Verifique os pinos:**  
   Consulte **`ligacoes.png`** (tabela de pinos) e ajuste os `#define`/constantes do exemplo se necessário.
3. **Abra no Arduino IDE / PlatformIO:**  
   Carregue o arquivo `*.ino` do exemplo desejado, selecione a placa correta e **faça o upload**.
4. **(Opcional) Importar no EdgeWX:**  
   Abra o `*.json` correspondente para visualizar/editar a tela do widget e regenerar o código, se quiser.

---

## Convenções dentro dos `.md` dos exemplos

- A imagem de montagem é referenciada por **caminho relativo** a partir da pasta do exemplo:  
  ```md
  ![montagem](../montagem.png)
  ```
- O print da tela do próprio exemplo fica **ao lado do `.md`**:  
  ```md
  ![screenshot](screenshot.png)
  ```
- O código gerado automaticamente pelo EdgeWX pode vir anotado; trechos marcados como **(ESCRITO MANUAL)** indicam alterações manuais.

---

## Ligações típicas de hardware

### LED em GPIO16
```
ESP32 (GPIO16) ──► Resistor 220–330 Ω ──► Ânodo do LED
                                        Cátodo do LED ──► GND
```

### Potenciômetro (padrão adotado)
```
3V3 ──┬─────────┐           
      │         │               (terminal central - wiper)
      │      [ Potenciômetro ] ────► ESP32 (GPIO17)  // pino analógico
      │         │
GND ──┴─────────┘
```

> Ajuste os pinos conforme a **tabela de ligações** em `ligacoes.png`.

---

## Dicas rápidas

- **Touch:** os exemplos já chamam `checkCalibration()`; se necessário, recalcibre.
- **PWM (brilho de LED):** se sua instalação não suportar `analogWrite()` no ESP32, use LEDC (`ledcSetup`, `ledcAttachPin`, `ledcWrite`).
- **Caminhos relativos em Markdown:** use `../` para acessar arquivos que ficam na pasta `exemplos` (como `montagem.png`).

---

Boas práticas e contribuições são bem-vindas. Bons testes! 🚀
