# Verificação das classes

Essa tarefa é repsonsável por verificar a estrutura da classe, se está de acordo com os requisitos descritos abaixo. Execute a lista de tarefas abaixo para a classe definida no prompt.

Aprenda as guias de práticas do arquivo 'RAG/CODE_GUIDELINES.md'.

## Task 1 - Renomear variaveis

- A variavel 'yPos' deve ser renomeada para 'm_yPos'
- A variavel 'xPos' deve ser renomeada para 'm_xPos'
- A variavel 'cb' deve ser renomeada para 'm_callback', exceto a relacionada à struct de config.
- A variavel 'loaded' deve ser renomeada para 'm_loaded'
- A variavel 'visible' deve ser renomeada para 'm_visible'

## Task 2 - Criar 'TAG' para log

Caso a classe não possua ainda, crie uma `const char* TAG = "CLASSNAME"` privada.

Substitua chamadas de Serial.print, Serial.println, log_* por `ESP_LOG*`

## Task 3 - Check de estrutura

- Cada classe que herda `WidgetBase` é considerada um widget.
- Essa classes estão dentro do diretorio 'src/widgets/[widget]'.
- Não deve-se alterar o funcionamento da classe.
- Não deve criar novos parametros na struct de confguração.
- Cada classe deve conter um membro privado chamada `m_config` que é uma struct de configuração, esse membro recebe o valor através do método setup que contem um parametro desse tipo de struct.
  - O metodo setup deve fazer a cópia dessa struct para a membro privado
  - Campos da struct que são ponteiro, arrays, ponteiros para ponteiros devem ser feitos '**deep-copy**', para isso, crie variaveis membro que recebem uma copia do valor no método 'setup', e depois atribuem ao campo em `m_config`, crie metodo de `cleanupMemory` para limpar a memoria antes da atriibuição do parametro de setup(struct) em m_config.
  - Evite ter duplicidade de campos entre `m_config` e variaveis membros, somente crie variaveis membro se o campo não existir em `m_config`
- As funções da classe devem usar preferencialmente os valores do `m_config`.
- Em cada classe, as variaveis membros devem ser iniciadas no construtor da classe, na mesma ordem que foram criadas no arquivo .h da classe.
- A variavel `TAG` deve ser `static const char* TAG` no arquivo  .h e ser iniciada no inicio do arquivo .cpp.
- O metodo `setup(p1,p1,p1....)` que contem varios parametros deve ser substituido pelo metodo setup que recebe a struct de configuração. Toda a verificação feita anters no setup com varios parametros, agora será realizada dentro do setup com um unico parametro.

## Task 4 - Uso de MACROS

No arquivo `widgetbase.h` foram definidos algumas macros para minimizar código escrito, essas macros estão na sessão `START SECTION - MACROS FOR VALIDATION`. Aprenda elas e aplique no código da classe onde já existe código representado por ela, exemplo:
Onde está

```cpp
if (WidgetBase::currentScreen != m_screen
```

troque por

`CHECK_CURRENTSCREEN_VOID` ou `CHECK_CURRENTSCREEN_VOID`, dependendo do tipo de retorno dentro do `if`

Em casos de `if` com multiplas verificações, exemplo:

```cpp
 if (WidgetBase::currentScreen != m_screen || WidgetBase::usingKeyboard == true || !m_shouldRedraw || !m_loaded)
  {
    return;
  }
```

troque por

```cpp
CHECK_CURRENTSCREEN_VOID
CHECK_USINGKEYBOARD_VOID
CHECK_LOADED_VOID
CHECK_SHOULDREDRAW_VOID
```

## Task 5 - Validação

Agora verifique novamente a classe pedida, procure por falhas de `memory leak`, possibilidade de `dangling pointer` e corrija.

# Observações

- Execute cada task de uma vez, aguarde confirmação no chat antes de prosseguir.
