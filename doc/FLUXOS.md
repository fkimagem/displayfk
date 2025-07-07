[TOC]


## Task
```mermaid
flowchart TD
    Start(["Início da loopTask()"])
    CheckScreen{"WidgetBase::loadScreen está definida?"}
    CallScreen["Chama WidgetBase::loadScreen()"]
    NullScreen["Zera WidgetBase::loadScreen (nullptr)"]
    Delay5ms1["vTaskDelay(5ms)"]
    ProcessLog["processLogQueue: Processa fila de logs (SD/logs)"]
    ProcessCallback["processCallback: Executa callbacks pendentes"]
    TouchEnabled{"Touch está habilitado? (HAS_TOUCH)"}
    GetTouch["Lê touch: xTouch, yTouch, zPressure, gesture"]
    TouchDetected{"Touch detectado? (hasTouch)"}
    DebugTouch{"Debug Touch ativado? (m_debugTouch)"}
    DrawCircle["Desenha círculo rosa no ponto de toque"]
    CallProcessTouch["processTouchEvent: Processa evento de toque"]
    UpdateWidgets["updateWidgets: Atualiza todos os widgets"]
    UpdateCircularBar["updateCircularBar"]
    UpdateGauge["updateGauge"]
    UpdateLabel["updateLabel"]
    UpdateLed["updateLed"]
    UpdateLineChart["updateLineChart"]
    UpdateVBar["updateVBar"]
    UpdateVAnalog["updateVAnalog"]
    UpdateCheckbox["updateCheckbox"]
    UpdateCircleButton["updateCircleButton"]
    UpdateHSlider["updateHSlider"]
    UpdateRadioGroup["updateRadioGroup"]
    UpdateRectButton["updateRectButton"]
    UpdateToggle["updateToggle"]
    UpdateTextButton["updateTextButton"]
    UpdateSpinbox["updateSpinbox"]
    UpdateNumberBox["updateNumberBox"]
    UpdateTextBox["updateTextBox"]
    CalcTime["Calcula tempo de execução"]
    Delay5ms2["vTaskDelay(5ms)"]
    End(["Fim da iteração da task"])

    Start --> CheckScreen
    CheckScreen -- "Sim" --> CallScreen
    CallScreen --> NullScreen
    NullScreen --> Delay5ms1
    Delay5ms1 --> ProcessLog
    CheckScreen -- "Não" --> ProcessLog
    ProcessLog --> ProcessCallback
    ProcessCallback --> TouchEnabled
    TouchEnabled -- "Sim" --> GetTouch
    GetTouch --> TouchDetected
    TouchDetected -- "Sim" --> DebugTouch
    DebugTouch -- "Sim" --> DrawCircle
    DrawCircle --> CallProcessTouch
    DebugTouch -- "Não" --> CallProcessTouch
    CallProcessTouch --> UpdateWidgets
    TouchDetected -- "Não" --> UpdateWidgets
    TouchEnabled -- "Não" --> UpdateWidgets
    UpdateWidgets --> UpdateCircularBar
    UpdateCircularBar --> UpdateGauge
    UpdateGauge --> UpdateLabel
    UpdateLabel --> UpdateLed
    UpdateLed --> UpdateLineChart
    UpdateLineChart --> UpdateVBar
    UpdateVBar --> UpdateVAnalog
    UpdateVAnalog --> UpdateCheckbox
    UpdateCheckbox --> UpdateCircleButton
    UpdateCircleButton --> UpdateHSlider
    UpdateHSlider --> UpdateRadioGroup
    UpdateRadioGroup --> UpdateRectButton
    UpdateRectButton --> UpdateToggle
    UpdateToggle --> UpdateTextButton
    UpdateTextButton --> UpdateSpinbox
    UpdateSpinbox --> UpdateNumberBox
    UpdateNumberBox --> UpdateTextBox
    UpdateTextBox --> CalcTime
    CalcTime --> Delay5ms2
    Delay5ms2 --> End
```

## Fluxo detalhado: processTouchEvent
```mermaid
flowchart TD
    Start(["Início de processTouchEvent(xTouch, yTouch, zPressure, gesture)"])
    CheckKeyboard{"WidgetBase::usingKeyboard está true?"}
    ReturnKeyboard["Retorna (não processa touch)"]
    Next(["Processa widgets touchable"])
    CallCheckbox["processCheckboxTouch"]
    CallCircleButton["processCircleButtonTouch"]
    CallHSlider["processHSliderTouch"]
    CallRadioGroup["processRadioGroupTouch"]
    CallRectButton["processRectButtonTouch"]
    CallImage["processImageTouch"]
    CallSpinbox["processSpinboxTouch"]
    CallToggle["processToggleTouch"]
    CallTextButton["processTextButtonTouch"]
    CallTextBox["processTextBoxTouch"]
    CallNumberBox["processNumberBoxTouch"]
    CallEmptyArea["processEmptyAreaTouch"]
    End(["Fim do processamento"])

    Start --> CheckKeyboard
    CheckKeyboard -- "Sim" --> ReturnKeyboard
    CheckKeyboard -- "Não" --> Next
    Next --> CallCheckbox
    CallCheckbox --> CallCircleButton
    CallCircleButton --> CallHSlider
    CallHSlider --> CallRadioGroup
    CallRadioGroup --> CallRectButton
    CallRectButton --> CallImage
    CallImage --> CallSpinbox
    CallSpinbox --> CallToggle
    CallToggle --> CallTextButton
    CallTextButton --> CallTextBox
    CallTextBox --> CallNumberBox
    CallNumberBox --> CallEmptyArea
    CallEmptyArea --> End
```

**Descrição:**
- Se o teclado virtual está ativo (`usingKeyboard`), a função retorna imediatamente e não processa nenhum widget.
- Caso contrário, a função chama sequencialmente os métodos de processamento de toque para cada tipo de widget interativo (Checkbox, CircleButton, HSlider, etc.), passando as coordenadas do toque.
- Cada função verifica se o toque ocorreu em algum widget daquele tipo e, se sim, agenda o callback correspondente.
- O fluxo termina após tentar processar todos os tipos de widgets touchable.




## Fluxo detalhado: processNumberBoxTouch
```mermaid
flowchart TD
    StartNB(["Início de processNumberBoxTouch(xTouch, yTouch)"])
    CheckConfigNB{"NumberBox está configurado?"}
    ForEachNB["Para cada NumberBox na tela atual"]
    DetectTouchNB{"detectTouch retorna true?"}
    OpenNumpad["Abre o Numpad para o NumberBox"]
    NumpadLoop["Enquanto WidgetBase::usingKeyboard"]
    GetTouchNB["Lê touch (getTouch)"]
    DetectNumpadTouch{"detectTouch do Numpad retorna true?"}
    PressedReturnNB{"PressedKeyType == RETURN?"}
    SetScreenNB["WidgetBase::loadScreen = parentScreen"]
    CloseNumpad["Fecha o Numpad"]
    CallCallbackNB["Agenda callback do NumberBox"]
    EndNB(["Fim do processamento"])
    NextNB["Próximo NumberBox"]

    StartNB --> CheckConfigNB
    CheckConfigNB -- "Sim" --> ForEachNB
    CheckConfigNB -- "Não" --> EndNB
    ForEachNB --> DetectTouchNB
    DetectTouchNB -- "Sim" --> OpenNumpad
    DetectTouchNB -- "Não" --> NextNB
    NextNB --> ForEachNB
    OpenNumpad --> NumpadLoop
    NumpadLoop --> GetTouchNB
    GetTouchNB --> DetectNumpadTouch
    DetectNumpadTouch -- "Sim" --> PressedReturnNB
    DetectNumpadTouch -- "Não" --> NumpadLoop
    PressedReturnNB -- "Sim" --> SetScreenNB
    SetScreenNB --> CloseNumpad
    CloseNumpad --> CallCallbackNB
    CallCallbackNB --> EndNB
    PressedReturnNB -- "Não" --> NumpadLoop
```

**Descrição:**
- Para cada NumberBox configurado, verifica se houve toque.
- Se sim, abre o Numpad e entra em um loop aguardando interação.
- Se o usuário pressionar RETURN no Numpad, fecha o teclado, retorna para a tela original e agenda o callback do NumberBox.

---

## Fluxo detalhado: processTextBoxTouch
```mermaid
flowchart TD
    StartTB(["Início de processTextBoxTouch(xTouch, yTouch)"])
    CheckConfigTB{"TextBox está configurado?"}
    ForEachTB["Para cada TextBox na tela atual"]
    DetectTouchTB{"detectTouch retorna true?"}
    OpenKeyboard["Abre o teclado virtual para o TextBox"]
    KeyboardLoop["Enquanto WidgetBase::usingKeyboard"]
    GetTouchTB["Lê touch (getTouch)"]
    DetectKeyboardTouch{"detectTouch do teclado retorna true?"}
    PressedReturnTB{"PressedKeyType == RETURN?"}
    SetScreenTB["WidgetBase::loadScreen = parentScreen"]
    CloseKeyboard["Fecha o teclado virtual"]
    CallCallbackTB["Agenda callback do TextBox"]
    EndTB(["Fim do processamento"])
    NextTB["Próximo TextBox"]

    StartTB --> CheckConfigTB
    CheckConfigTB -- "Sim" --> ForEachTB
    CheckConfigTB -- "Não" --> EndTB
    ForEachTB --> DetectTouchTB
    DetectTouchTB -- "Sim" --> OpenKeyboard
    DetectTouchTB -- "Não" --> NextTB
    NextTB --> ForEachTB
    OpenKeyboard --> KeyboardLoop
    KeyboardLoop --> GetTouchTB
    GetTouchTB --> DetectKeyboardTouch
    DetectKeyboardTouch -- "Sim" --> PressedReturnTB
    DetectKeyboardTouch -- "Não" --> KeyboardLoop
    PressedReturnTB -- "Sim" --> SetScreenTB
    SetScreenTB --> CloseKeyboard
    CloseKeyboard --> CallCallbackTB
    CallCallbackTB --> EndTB
    PressedReturnTB -- "Não" --> KeyboardLoop
```

**Descrição:**
- Para cada TextBox configurado, verifica se houve toque.
- Se sim, abre o teclado virtual e entra em um loop aguardando interação.
- Se o usuário pressionar RETURN no teclado, fecha o teclado, retorna para a tela original e agenda o callback do TextBox.

## Fluxo detalhado: processCheckboxTouch
```mermaid
flowchart TD
    StartCB(["Início de processCheckboxTouch(xTouch, yTouch)"])
    CheckConfigCB{"Checkbox está configurado?"}
    ForEachCB["Para cada Checkbox na tela atual"]
    DetectTouchCB{"detectTouch retorna true?"}
    GetCallbackCB["Obtém callback do Checkbox"]
    AddCallbackCB["Agenda callback (WidgetBase::addCallback)"]
    EndCB(["Fim do processamento"])
    NextCB["Próximo Checkbox"]

    StartCB --> CheckConfigCB
    CheckConfigCB -- "Sim" --> ForEachCB
    CheckConfigCB -- "Não" --> EndCB
    ForEachCB --> DetectTouchCB
    DetectTouchCB -- "Sim" --> GetCallbackCB
    GetCallbackCB --> AddCallbackCB
    AddCallbackCB --> EndCB
    DetectTouchCB -- "Não" --> NextCB
    NextCB --> ForEachCB
```

---

## Fluxo detalhado: processCircleButtonTouch
```mermaid
flowchart TD
    StartCCB(["Início de processCircleButtonTouch(xTouch, yTouch)"])
    CheckConfigCCB{"CircleButton está configurado?"}
    ForEachCCB["Para cada CircleButton na tela atual"]
    DetectTouchCCB{"detectTouch retorna true?"}
    GetCallbackCCB["Obtém callback do CircleButton"]
    AddCallbackCCB["Agenda callback"]
    EndCCB(["Fim do processamento"])
    NextCCB["Próximo CircleButton"]

    StartCCB --> CheckConfigCCB
    CheckConfigCCB -- "Sim" --> ForEachCCB
    CheckConfigCCB -- "Não" --> EndCCB
    ForEachCCB --> DetectTouchCCB
    DetectTouchCCB -- "Sim" --> GetCallbackCCB
    GetCallbackCCB --> AddCallbackCCB
    AddCallbackCCB --> EndCCB
    DetectTouchCCB -- "Não" --> NextCCB
    NextCCB --> ForEachCCB
```

---

## Fluxo detalhado: processHSliderTouch
```mermaid
flowchart TD
    StartHS(["Início de processHSliderTouch(xTouch, yTouch)"])
    CheckConfigHS{"HSlider está configurado?"}
    ForEachHS["Para cada HSlider na tela atual"]
    DetectTouchHS{"detectTouch retorna true?"}
    GetCallbackHS["Obtém callback do HSlider"]
    AddCallbackHS["Agenda callback"]
    EndHS(["Fim do processamento"])
    NextHS["Próximo HSlider"]

    StartHS --> CheckConfigHS
    CheckConfigHS -- "Sim" --> ForEachHS
    CheckConfigHS -- "Não" --> EndHS
    ForEachHS --> DetectTouchHS
    DetectTouchHS -- "Sim" --> GetCallbackHS
    GetCallbackHS --> AddCallbackHS
    AddCallbackHS --> EndHS
    DetectTouchHS -- "Não" --> NextHS
    NextHS --> ForEachHS
```

---

## Fluxo detalhado: processRadioGroupTouch
```mermaid
flowchart TD
    StartRG(["Início de processRadioGroupTouch(xTouch, yTouch)"])
    CheckConfigRG{"RadioGroup está configurado?"}
    ForEachRG["Para cada RadioGroup na tela atual"]
    DetectTouchRG{"detectTouch retorna true?"}
    GetCallbackRG["Obtém callback do RadioGroup"]
    AddCallbackRG["Agenda callback"]
    EndRG(["Fim do processamento"])
    NextRG["Próximo RadioGroup"]

    StartRG --> CheckConfigRG
    CheckConfigRG -- "Sim" --> ForEachRG
    CheckConfigRG -- "Não" --> EndRG
    ForEachRG --> DetectTouchRG
    DetectTouchRG -- "Sim" --> GetCallbackRG
    GetCallbackRG --> AddCallbackRG
    AddCallbackRG --> EndRG
    DetectTouchRG -- "Não" --> NextRG
    NextRG --> ForEachRG
```

---

## Fluxo detalhado: processRectButtonTouch
```mermaid
flowchart TD
    StartRB(["Início de processRectButtonTouch(xTouch, yTouch)"])
    CheckConfigRB{"RectButton está configurado?"}
    ForEachRB["Para cada RectButton na tela atual"]
    DetectTouchRB{"detectTouch retorna true?"}
    GetCallbackRB["Obtém callback do RectButton"]
    AddCallbackRB["Agenda callback"]
    EndRB(["Fim do processamento"])
    NextRB["Próximo RectButton"]

    StartRB --> CheckConfigRB
    CheckConfigRB -- "Sim" --> ForEachRB
    CheckConfigRB -- "Não" --> EndRB
    ForEachRB --> DetectTouchRB
    DetectTouchRB -- "Sim" --> GetCallbackRB
    GetCallbackRB --> AddCallbackRB
    AddCallbackRB --> EndRB
    DetectTouchRB -- "Não" --> NextRB
    NextRB --> ForEachRB
```

---

## Fluxo detalhado: processImageTouch
```mermaid
flowchart TD
    StartIM(["Início de processImageTouch(xTouch, yTouch)"])
    CheckConfigIM{"Image está configurado?"}
    ForEachIM["Para cada Image na tela atual"]
    DetectTouchIM{"detectTouch retorna true?"}
    GetCallbackIM["Obtém callback do Image"]
    AddCallbackIM["Agenda callback"]
    EndIM(["Fim do processamento"])
    NextIM["Próxima Image"]

    StartIM --> CheckConfigIM
    CheckConfigIM -- "Sim" --> ForEachIM
    CheckConfigIM -- "Não" --> EndIM
    ForEachIM --> DetectTouchIM
    DetectTouchIM -- "Sim" --> GetCallbackIM
    GetCallbackIM --> AddCallbackIM
    AddCallbackIM --> EndIM
    DetectTouchIM -- "Não" --> NextIM
    NextIM --> ForEachIM
```

---

## Fluxo detalhado: processSpinboxTouch
```mermaid
flowchart TD
    StartSB(["Início de processSpinboxTouch(xTouch, yTouch)"])
    CheckConfigSB{"SpinBox está configurado?"}
    ForEachSB["Para cada SpinBox na tela atual"]
    DetectTouchSB{"detectTouch retorna true?"}
    GetCallbackSB["Obtém callback do SpinBox"]
    AddCallbackSB["Agenda callback"]
    EndSB(["Fim do processamento"])
    NextSB["Próximo SpinBox"]

    StartSB --> CheckConfigSB
    CheckConfigSB -- "Sim" --> ForEachSB
    CheckConfigSB -- "Não" --> EndSB
    ForEachSB --> DetectTouchSB
    DetectTouchSB -- "Sim" --> GetCallbackSB
    GetCallbackSB --> AddCallbackSB
    AddCallbackSB --> EndSB
    DetectTouchSB -- "Não" --> NextSB
    NextSB --> ForEachSB
```

---

## Fluxo detalhado: processToggleTouch
```mermaid
flowchart TD
    StartTG(["Início de processToggleTouch(xTouch, yTouch)"])
    CheckConfigTG{"Toggle está configurado?"}
    ForEachTG["Para cada Toggle na tela atual"]
    DetectTouchTG{"detectTouch retorna true?"}
    GetCallbackTG["Obtém callback do Toggle"]
    AddCallbackTG["Agenda callback"]
    EndTG(["Fim do processamento"])
    NextTG["Próximo Toggle"]

    StartTG --> CheckConfigTG
    CheckConfigTG -- "Sim" --> ForEachTG
    CheckConfigTG -- "Não" --> EndTG
    ForEachTG --> DetectTouchTG
    DetectTouchTG -- "Sim" --> GetCallbackTG
    GetCallbackTG --> AddCallbackTG
    AddCallbackTG --> EndTG
    DetectTouchTG -- "Não" --> NextTG
    NextTG --> ForEachTG
```

---

## Fluxo detalhado: processTextButtonTouch
```mermaid
flowchart TD
    StartTBt(["Início de processTextButtonTouch(xTouch, yTouch)"])
    CheckConfigTBt{"TextButton está configurado?"}
    ForEachTBt["Para cada TextButton na tela atual"]
    DetectTouchTBt{"detectTouch retorna true?"}
    GetCallbackTBt["Obtém callback do TextButton"]
    AddCallbackTBt["Agenda callback"]
    EndTBt(["Fim do processamento"])
    NextTBt["Próximo TextButton"]

    StartTBt --> CheckConfigTBt
    CheckConfigTBt -- "Sim" --> ForEachTBt
    CheckConfigTBt -- "Não" --> EndTBt
    ForEachTBt --> DetectTouchTBt
    DetectTouchTBt -- "Sim" --> GetCallbackTBt
    GetCallbackTBt --> AddCallbackTBt
    AddCallbackTBt --> EndTBt
    DetectTouchTBt -- "Não" --> NextTBt
    NextTBt --> ForEachTBt
```

---

## Fluxo detalhado: processEmptyAreaTouch
```mermaid
flowchart TD
    StartEA(["Início de processEmptyAreaTouch(xTouch, yTouch)"])
    CheckConfigEA{"TouchArea está configurado?"}
    ForEachEA["Para cada TouchArea na tela atual"]
    DetectTouchEA{"detectTouch retorna true?"}
    GetCallbackEA["Obtém callback do TouchArea"]
    AddCallbackEA["Agenda callback"]
    EndEA(["Fim do processamento"])
    NextEA["Próxima TouchArea"]

    StartEA --> CheckConfigEA
    CheckConfigEA -- "Sim" --> ForEachEA
    CheckConfigEA -- "Não" --> EndEA
    ForEachEA --> DetectTouchEA
    DetectTouchEA -- "Sim" --> GetCallbackEA
    GetCallbackEA --> AddCallbackEA
    AddCallbackEA --> EndEA
    DetectTouchEA -- "Não" --> NextEA
    NextEA --> ForEachEA
```

## Fluxo detalhado: updateCircularBar
```mermaid
flowchart TD
    StartCBAR(["Início de updateCircularBar"])
    CheckConfigCBAR{"CircularBar está configurado?"}
    ForEachCBAR["Para cada CircularBar"]
    RedrawCBAR["Redesenha (redraw)"]
    EndCBAR(["Fim"])
    NextCBAR["Próximo CircularBar"]

    StartCBAR --> CheckConfigCBAR
    CheckConfigCBAR -- "Sim" --> ForEachCBAR
    CheckConfigCBAR -- "Não" --> EndCBAR
    ForEachCBAR --> RedrawCBAR
    RedrawCBAR --> NextCBAR
    NextCBAR --> ForEachCBAR
    ForEachCBAR -- "Todos processados" --> EndCBAR
```

---

## Fluxo detalhado: updateGauge
```mermaid
flowchart TD
    StartG(["Início de updateGauge"])
    CheckConfigG{"Gauge está configurado?"}
    ForEachG["Para cada Gauge"]
    RedrawG["Redesenha (redraw)"]
    EndG(["Fim"])
    NextG["Próximo Gauge"]

    StartG --> CheckConfigG
    CheckConfigG -- "Sim" --> ForEachG
    CheckConfigG -- "Não" --> EndG
    ForEachG --> RedrawG
    RedrawG --> NextG
    NextG --> ForEachG
    ForEachG -- "Todos processados" --> EndG
```

---

## Fluxo detalhado: updateLabel
```mermaid
flowchart TD
    StartL(["Início de updateLabel"])
    CheckConfigL{"Label está configurado?"}
    ForEachL["Para cada Label"]
    RedrawL["Redesenha (redraw)"]
    EndL(["Fim"])
    NextL["Próximo Label"]

    StartL --> CheckConfigL
    CheckConfigL -- "Sim" --> ForEachL
    CheckConfigL -- "Não" --> EndL
    ForEachL --> RedrawL
    RedrawL --> NextL
    NextL --> ForEachL
    ForEachL -- "Todos processados" --> EndL
```

---

## Fluxo detalhado: updateLed
```mermaid
flowchart TD
    StartLED(["Início de updateLed"])
    CheckConfigLED{"Led está configurado?"}
    ForEachLED["Para cada Led"]
    RedrawLED["Redesenha (redraw)"]
    EndLED(["Fim"])
    NextLED["Próximo Led"]

    StartLED --> CheckConfigLED
    CheckConfigLED -- "Sim" --> ForEachLED
    CheckConfigLED -- "Não" --> EndLED
    ForEachLED --> RedrawLED
    RedrawLED --> NextLED
    NextLED --> ForEachLED
    ForEachLED -- "Todos processados" --> EndLED
```

---

## Fluxo detalhado: updateLineChart
```mermaid
flowchart TD
    StartLC(["Início de updateLineChart"])
    CheckConfigLC{"LineChart está configurado?"}
    ForEachLC["Para cada LineChart"]
    RedrawLC["Redesenha (redraw)"]
    EndLC(["Fim"])
    NextLC["Próximo LineChart"]

    StartLC --> CheckConfigLC
    CheckConfigLC -- "Sim" --> ForEachLC
    CheckConfigLC -- "Não" --> EndLC
    ForEachLC --> RedrawLC
    RedrawLC --> NextLC
    NextLC --> ForEachLC
    ForEachLC -- "Todos processados" --> EndLC
```

---

## Fluxo detalhado: updateVBar
```mermaid
flowchart TD
    StartVB(["Início de updateVBar"])
    CheckConfigVB{"VBar está configurado?"}
    ForEachVB["Para cada VBar"]
    RedrawVB["Redesenha (redraw)"]
    EndVB(["Fim"])
    NextVB["Próximo VBar"]

    StartVB --> CheckConfigVB
    CheckConfigVB -- "Sim" --> ForEachVB
    CheckConfigVB -- "Não" --> EndVB
    ForEachVB --> RedrawVB
    RedrawVB --> NextVB
    NextVB --> ForEachVB
    ForEachVB -- "Todos processados" --> EndVB
```

---

## Fluxo detalhado: updateVAnalog
```mermaid
flowchart TD
    StartVA(["Início de updateVAnalog"])
    CheckConfigVA{"VAnalog está configurado?"}
    ForEachVA["Para cada VAnalog"]
    RedrawVA["Redesenha (redraw)"]
    EndVA(["Fim"])
    NextVA["Próximo VAnalog"]

    StartVA --> CheckConfigVA
    CheckConfigVA -- "Sim" --> ForEachVA
    CheckConfigVA -- "Não" --> EndVA
    ForEachVA --> RedrawVA
    RedrawVA --> NextVA
    NextVA --> ForEachVA
    ForEachVA -- "Todos processados" --> EndVA
```

---

## Fluxo detalhado: updateCheckbox
```mermaid
flowchart TD
    StartCBX(["Início de updateCheckbox"])
    CheckConfigCBX{"Checkbox está configurado?"}
    ForEachCBX["Para cada Checkbox"]
    RedrawCBX["Redesenha (redraw)"]
    EndCBX(["Fim"])
    NextCBX["Próximo Checkbox"]

    StartCBX --> CheckConfigCBX
    CheckConfigCBX -- "Sim" --> ForEachCBX
    CheckConfigCBX -- "Não" --> EndCBX
    ForEachCBX --> RedrawCBX
    RedrawCBX --> NextCBX
    NextCBX --> ForEachCBX
    ForEachCBX -- "Todos processados" --> EndCBX
```

---

## Fluxo detalhado: updateCircleButton
```mermaid
flowchart TD
    StartCCB(["Início de updateCircleButton"])
    CheckConfigCCB{"CircleButton está configurado?"}
    ForEachCCB["Para cada CircleButton"]
    RedrawCCB["Redesenha (redraw)"]
    EndCCB(["Fim"])
    NextCCB["Próximo CircleButton"]

    StartCCB --> CheckConfigCCB
    CheckConfigCCB -- "Sim" --> ForEachCCB
    CheckConfigCCB -- "Não" --> EndCCB
    ForEachCCB --> RedrawCCB
    RedrawCCB --> NextCCB
    NextCCB --> ForEachCCB
    ForEachCCB -- "Todos processados" --> EndCCB
```

---

## Fluxo detalhado: updateHSlider
```mermaid
flowchart TD
    StartHS(["Início de updateHSlider"])
    CheckConfigHS{"HSlider está configurado?"}
    ForEachHS["Para cada HSlider"]
    RedrawHS["Redesenha (redraw)"]
    EndHS(["Fim"])
    NextHS["Próximo HSlider"]

    StartHS --> CheckConfigHS
    CheckConfigHS -- "Sim" --> ForEachHS
    CheckConfigHS -- "Não" --> EndHS
    ForEachHS --> RedrawHS
    RedrawHS --> NextHS
    NextHS --> ForEachHS
    ForEachHS -- "Todos processados" --> EndHS
```

---

## Fluxo detalhado: updateRadioGroup
```mermaid
flowchart TD
    StartRG(["Início de updateRadioGroup"])
    CheckConfigRG{"RadioGroup está configurado?"}
    ForEachRG["Para cada RadioGroup"]
    RedrawRG["Redesenha (redraw)"]
    EndRG(["Fim"])
    NextRG["Próximo RadioGroup"]

    StartRG --> CheckConfigRG
    CheckConfigRG -- "Sim" --> ForEachRG
    CheckConfigRG -- "Não" --> EndRG
    ForEachRG --> RedrawRG
    RedrawRG --> NextRG
    NextRG --> ForEachRG
    ForEachRG -- "Todos processados" --> EndRG
```

---

## Fluxo detalhado: updateRectButton
```mermaid
flowchart TD
    StartRB(["Início de updateRectButton"])
    CheckConfigRB{"RectButton está configurado?"}
    ForEachRB["Para cada RectButton"]
    RedrawRB["Redesenha (redraw)"]
    EndRB(["Fim"])
    NextRB["Próximo RectButton"]

    StartRB --> CheckConfigRB
    CheckConfigRB -- "Sim" --> ForEachRB
    CheckConfigRB -- "Não" --> EndRB
    ForEachRB --> RedrawRB
    RedrawRB --> NextRB
    NextRB --> ForEachRB
    ForEachRB -- "Todos processados" --> EndRB
```

---

## Fluxo detalhado: updateToggle
```mermaid
flowchart TD
    StartTG(["Início de updateToggle"])
    CheckConfigTG{"Toggle está configurado?"}
    ForEachTG["Para cada Toggle"]
    RedrawTG["Redesenha (redraw)"]
    EndTG(["Fim"])
    NextTG["Próximo Toggle"]

    StartTG --> CheckConfigTG
    CheckConfigTG -- "Sim" --> ForEachTG
    CheckConfigTG -- "Não" --> EndTG
    ForEachTG --> RedrawTG
    RedrawTG --> NextTG
    NextTG --> ForEachTG
    ForEachTG -- "Todos processados" --> EndTG
```

---

## Fluxo detalhado: updateTextButton
```mermaid
flowchart TD
    StartTBt(["Início de updateTextButton"])
    CheckConfigTBt{"TextButton está configurado?"}
    ForEachTBt["Para cada TextButton"]
    RedrawTBt["Redesenha (redraw)"]
    EndTBt(["Fim"])
    NextTBt["Próximo TextButton"]

    StartTBt --> CheckConfigTBt
    CheckConfigTBt -- "Sim" --> ForEachTBt
    CheckConfigTBt -- "Não" --> EndTBt
    ForEachTBt --> RedrawTBt
    RedrawTBt --> NextTBt
    NextTBt --> ForEachTBt
    ForEachTBt -- "Todos processados" --> EndTBt
```

---

## Fluxo detalhado: updateSpinbox
```mermaid
flowchart TD
    StartSB(["Início de updateSpinbox"])
    CheckConfigSB{"SpinBox está configurado?"}
    ForEachSB["Para cada SpinBox"]
    RedrawSB["Redesenha (redraw)"]
    EndSB(["Fim"])
    NextSB["Próximo SpinBox"]

    StartSB --> CheckConfigSB
    CheckConfigSB -- "Sim" --> ForEachSB
    CheckConfigSB -- "Não" --> EndSB
    ForEachSB --> RedrawSB
    RedrawSB --> NextSB
    NextSB --> ForEachSB
    ForEachSB -- "Todos processados" --> EndSB
```

---

## Fluxo detalhado: updateNumberBox
```mermaid
flowchart TD
    StartNB(["Início de updateNumberBox"])
    CheckConfigNB{"NumberBox está configurado?"}
    ForEachNB["Para cada NumberBox"]
    RedrawNB["Redesenha (redraw)"]
    EndNB(["Fim"])
    NextNB["Próximo NumberBox"]

    StartNB --> CheckConfigNB
    CheckConfigNB -- "Sim" --> ForEachNB
    CheckConfigNB -- "Não" --> EndNB
    ForEachNB --> RedrawNB
    RedrawNB --> NextNB
    NextNB --> ForEachNB
    ForEachNB -- "Todos processados" --> EndNB
```

---

## Fluxo detalhado: updateTextBox
```mermaid
flowchart TD
    StartTB(["Início de updateTextBox"])
    CheckConfigTB{"TextBox está configurado?"}
    ForEachTB["Para cada TextBox"]
    RedrawTB["Redesenha (redraw)"]
    EndTB(["Fim"])
    NextTB["Próximo TextBox"]

    StartTB --> CheckConfigTB
    CheckConfigTB -- "Sim" --> ForEachTB
    CheckConfigTB -- "Não" --> EndTB
    ForEachTB --> RedrawTB
    RedrawTB --> NextTB
    NextTB --> ForEachTB
    ForEachTB -- "Todos processados" --> EndTB
```


