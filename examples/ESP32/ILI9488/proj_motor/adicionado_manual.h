/*
As funções abaixo foram criadas automaticamente pelo edge (exceto cb_LerSensor). O que mudei foi apenas o conteudo delas. 
Essas variaveis globais abaixo, criei para logica de 'ler sensor a cada 1 segundo' utilizando Timer do esp32.
A função 'cb_LerSensor' é disparada a cada 1 segundo, que muda o estado da variavel 'podeLerSensor' para true.
No setup, eu crio o TImer e inicia ele imediatamente.
No loop, quando a variavel paa ler o sensor é true, faço as leituras e atualizo o display. Tambem ja faço a verificação dos limites de temperatura e aplico
a logica que força o desligamento do motor ou da bomba, e tambem bloqueio o botão para ligar novamente em caso de temperatura alta.
Nas funções de callback ligo/desligo os leds e relés, e também aplico  logica que faz o acionamento simultaneos de relés que precisam ser ligados/desligados juntos.
Nas callbacks das imagtens, coloquei comandos para trocar de tela;


*/

TimerHandle_t xTimerLeitura;            // Handle de um timer (FreeRTOS) usado para agendar leituras periódicas dos sensores
volatile bool podeLerSensor = false;    // Flag controlada pelo timer para indicar que é hora de ler os sensores (usada dentro de ISR ou tarefas)


// Função de callback quando estoura o timer
void cb_LerSensor(TimerHandle_t xTimer)
{
    podeLerSensor = true;
}

void setup()
{
    // Cria um timer com período de 1000ms (1 segundo), repetitivo (pdTRUE), e função de callback `cb_LerSensor`
    xTimerLeitura = xTimerCreate("TIMER1", pdMS_TO_TICKS(1000), pdTRUE, 0, cb_LerSensor);

    // Inicia o timer imediatamente
    xTimerStart(xTimerLeitura, 0);
}

void loop()
{
    // Se puder ler o sensor, lê o sensor
    if (podeLerSensor)
    {
        podeLerSensor = false; // Reseta a variável auxiliar para a leitura do sensor

        // Inicio a transação, como vamos fazer varias atualizações que afetam o display, primeiro vamos mudar todos os valores e depois desenhar tudo de uma vez
        myDisplay.startTransaction();

        // Finalizo a transacao para indicar para a task de desenho que pode desenhar
        myDisplay.finishTransaction();

        bool temperaturaAltaMotor = (temperatureMotor >= maxTempMotor);    // Verifica se a temperatura do sensor é maior ou igual ao valor maximo permitido
        bool temperaturaAltaComp = (temperatureComp >= maxTempCompressor); // Verifica se a temperatura do sensor é maior ou igual ao valor maximo permitido

        // Se a temperatura do primeiro sensor for maior ou igual ao valor maximo permitido, desativa o relé 1
        if (temperaturaAltaMotor)
        {
            btnmotor.setStatus(false);       // Desativa o toggle que controla o relé 1
            Serial.println("Motor Quente!"); // Imprime uma mensagem no monitor serial
        }
        if (temperaturaAltaComp)
        {
            btncompressor.setStatus(false);       // Desativa o toggle que controla o relé 1
            Serial.println("Compressor Quente!"); // Imprime uma mensagem no monitor serial
        }

        btnmotor.setEnabled(!temperaturaAltaMotor);     // Desativa/Ativa o controle manual (por touch) do toggle que controla o relé 1
        btncompressor.setEnabled(!temperaturaAltaComp); // Desativa/Ativa o controle manual (por touch) do toggle que controla o relé 1
    }

    delay(10); // Aguarda 10ms para evitar que o loop seja executado muito rapidamente
}

// Quando clica no botão leda, ativa/desativa o rele do ledA
void btnleda_cb()
{
    bool myValue = btnleda.getStatus();
    digitalWrite(pinLed_1, myValue); // Ativa/Desativa o LED A
}

// Quando clica no botão ledb, ativa/desativa o rele do ledB
void btnledb_cb()
{
    bool myValue = btnledb.getStatus();
    digitalWrite(pinLed_2, myValue); // Ativa/Desativa o LED B
}

// Quando clica no botão ledc, ativa/desativa o rele do ledC
void btnledc_cb()
{
    bool myValue = btnledc.getStatus();
    digitalWrite(pinLed_3, myValue); // Ativa/Desativa o LED C
}

// Quando clica no botão motor, ativa/desativa o rele do motor
void btnmotor_cb()
{
    bool myStatus = btnmotor.getStatus();

    bool statusRele = !myStatus; // O rele ativa com LOW, entao aqui inverto a logica para facilitar o entendimento das linhas abaixo.
    // statusRele = true -> rele ativado (passando energia)
    // statusRele = false -> rele desativado

    digitalWrite(pinMotor, statusRele); // Ativa/Desativa o relé 1

    // Se ligou togglebutton do rele do motor
    if (myStatus)
    {
        // Força a bomba para ligar
        btncompressor.setStatus(true);      // Ativa o togglebutton da bomba
        digitalWrite(pinBomba, statusRele); // Coloca o pin da bomba no mesmo nivel do pin do rele (digitalWrite acima desse 'if')
    }

    Serial.print("O motor esta em nivel: ");
    Serial.println(statusRele); // Imprime o valor do toggleButton no monitor serial
}

// Quando clica no botão compressor, ativa/desativa o rele da bomba
void btncompressor_cb()
{
    bool myStatus = btncompressor.getStatus();

    bool statusRele = !myStatus; // O rele ativa com LOW, entao aqui inverto a logica para facilitar o entendimento das linhas abaixo.

    digitalWrite(pinBomba, statusRele); // Ativa/Desativa o relé 2

    // Se desligou o togglebutton do rele da bomba
    if (!myStatus)
    {
        // Força o motor para desligar
        btnmotor.setStatus(false);          // Desativa o togglebutton de controle do motor
        digitalWrite(pinMotor, statusRele); ////Coloca o pin do motor no mesmo nivel do pin do rele (digitalWrite acima desse 'if')
    }

    Serial.print("A bomba esta em nivel: ");
    Serial.println(statusRele); // Imprime o valor do toggleButton no monitor serial
}

// Quando clica no botão right, carrega a tela de configuração
void rightpng_cb()
{
    WidgetBase::loadScreen = screen1; // Carrega a tela de configuração
}

// Quando clica no botão home, carrega a tela de controle
void homepng_cb()
{
    WidgetBase::loadScreen = screen0; // Carrega a tela de configuração
}