# touch.h

Integra o controlador capacitivo FT6336U: inicializacao, leitura de pontos de toque e impressao de diagnostico.

## Visao geral

- Aloca e inicializa `FT6336U` com pinos de `variaveis.h`.
- `showSetup()` imprime registros/versoes para debug.
- `check_touch(int*, int*)` le o estado, aplica mapeamento/inversoes e retorna coordenadas.

## Globais

- `FT6336U* obj_touch`: instancia do driver de touch.
- `FT6336U_TouchPointType tp`: ultimo snapshot lido com `scan()`.

## Funcoes

- `startTouch()`: cria `FT6336U`, chama `begin()` e `showSetup()`.
- `showSetup()`: le e imprime parametros/IDs do FT6336U.
- `check_touch(int* x, int* y)`: retorna se houve toque; preenche `x` e `y` mapeados.

## Mapeamento de coordenadas

- Inverte eixos conforme `TOUCH_INVERT_X`/`TOUCH_INVERT_Y`.
- Troca eixos se `TOUCH_SWAP_XY`.
- Usa `TOUCH_MAP_X1` e `TOUCH_MAP_Y1` para refletir a origem desejada.

## Diagrama de chamadas (Mermaid)

```mermaid
flowchart TD
    T0[startTouch] --> T1[new FT6336U]
    T1 --> T2[begin]
    T2 --> T3[showSetup]

    C0[check_touch] --> C1[scan]
    C1 --> C2{tem toque}
    C2 -- nao --> C9[return false]
    C2 -- sim --> C3[pega coordenadas]
    C3 --> C4{inverter X}
    C4 -- sim --> C5[inverte rx]
    C4 -- nao --> C6[mantem rx]
    C5 --> C7{inverter Y}
    C6 --> C7
    C7 -- sim --> C8[inverte ry]
    C7 -- nao --> C10[mantem ry]
    C8 --> C11{trocar XY}
    C10 --> C11
    C11 -- sim --> C12[troca x e y]
    C11 -- nao --> C13[mantem x e y]
    C12 --> C14[log Serial]
    C13 --> C14
    C14 --> C15[return true]
```

## Fluxo típico

1. `startTouch()` inicializa o controlador e imprime diagnóstico.
2. `check_touch()` lê pontos, converte para coordenadas de UI e informa se houve toque.
