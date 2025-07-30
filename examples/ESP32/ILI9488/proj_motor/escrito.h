TimerHandle_t xTimerLeitura;        
volatile bool podeLerSensor = false;
void cb_LerSensor(TimerHandle_t xTimer)
{
    podeLerSensor = true;
}
xTimerLeitura = xTimerCreate("TIMER1", pdMS_TO_TICKS(1000), pdTRUE, 0, cb_LerSensor);
xTimerStart(xTimerLeitura, 0);
if (podeLerSensor)
{
    podeLerSensor = false;
    myDisplay.startTransaction();
    myDisplay.finishTransaction();
    bool temperaturaAltaMotor = (temperatureMotor >= maxTempMotor);
    bool temperaturaAltaComp = (temperatureComp >= maxTempCompressor);
    if (temperaturaAltaMotor)
    {
        btnmotor.setStatus(false);  
        Serial.println("Motor Quente!");
    }
    if (temperaturaAltaComp)
    {
        btncompressor.setStatus(false);       
        Serial.println("Compressor Quente!"); 
    }
    btnmotor.setEnabled(!temperaturaAltaMotor);    
    btncompressor.setEnabled(!temperaturaAltaComp);
}
bool statusRele = !myStatus;
digitalWrite(pinMotor, statusRele);
if (myStatus)
{
    btncompressor.setStatus(true);     
    digitalWrite(pinBomba, statusRele);
}
Serial.print("O motor esta em nivel: ");
Serial.println(statusRele); 
bool statusRele = !myStatus; 
digitalWrite(pinBomba, statusRele); 
if (!myStatus)
{
    btnmotor.setStatus(false);      
    digitalWrite(pinMotor, statusRele);
}
Serial.print("A bomba esta em nivel: ");
Serial.println(statusRele);
WidgetBase::loadScreen = screen1;
WidgetBase::loadScreen = screen0;