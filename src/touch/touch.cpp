#include "touch.h"

#if defined(TOUCH_FT6236U)
//TouchScreen *TouchScreen::m_instance = nullptr;
#endif

TouchScreen::TouchScreen()
{
  
}

#if defined(TOUCH_XPT2046)
void TouchScreen::startAsXPT2046(uint16_t w, uint16_t h, uint8_t _rotation, uint8_t pinSclk,uint8_t pinMosi, uint8_t pinMiso, uint8_t pinCS, SPIClass *_sharedSPI, Arduino_GFX *_objTFT){
  m_ts = new XPT2046(pinSclk, pinMiso, pinMosi, pinCS);
  this->setDimension(w, h, _rotation);
  m_objTFT = _objTFT;
  m_spitoque = _sharedSPI;
  m_pinCS = pinCS;
  if(m_pinCS){
    pinMode(m_pinCS, OUTPUT);
    digitalWrite(m_pinCS, HIGH);
  }
  touch_init();
}
#elif defined(TOUCH_FT6236U)
void TouchScreen::startAsFT6236U(uint16_t w, uint16_t h, uint8_t _rotation, uint8_t pinSDA, uint8_t pinSCL, uint8_t pinINT, uint8_t pinRST){
  m_ts = new FT6236();
  this->setDimension(w, h, _rotation);
  m_pinSCL = pinSCL;
  m_pinSDA = pinSDA;
  m_pinINT = pinINT;
  m_pinRST = pinRST;
  touch_init();
}
#elif defined(TOUCH_FT6336)
void TouchScreen::startAsFT6336(uint16_t w, uint16_t h, uint8_t _rotation, uint8_t pinSDA, uint8_t pinSCL, uint8_t pinINT, uint8_t pinRST){
  m_ts = new FT6336U(pinSDA, pinSCL, pinRST, pinINT);
  this->setDimension(w, h, _rotation);
  m_pinSCL = pinSCL;
  m_pinSDA = pinSDA;
  m_pinINT = pinINT;
  m_pinRST = pinRST;
  touch_init();
}
#elif defined(TOUCH_CST816)
void TouchScreen::startAsCST816(uint16_t w, uint16_t h, uint8_t _rotation, uint8_t pinSDA, uint8_t pinSCL, uint8_t pinINT, uint8_t pinRST){
 m_ts = new CST816S(pinSDA, pinSCL, pinRST, pinINT);
 this->setDimension(w, h, _rotation);
 m_pinSCL = pinSCL;
m_pinSDA = pinSDA;
m_pinINT = pinINT;
m_pinRST = pinRST;
 touch_init();
}
#elif defined(TOUCH_GT911)
void TouchScreen::startAsGT911(uint16_t w, uint16_t h, uint8_t _rotation, uint8_t pinSDA, uint8_t pinSCL, uint8_t pinINT, uint8_t pinRST){
//m_ts = new TAMC_GT911(pinSDA, pinSCL, pinINT, pinRST, max(TOUCH_MAP_X1, TOUCH_MAP_X2), max(TOUCH_MAP_Y1, TOUCH_MAP_Y2));
this->setDimension(w, h, _rotation);
m_ts = new TAMC_GT911(pinSDA, pinSCL, pinINT, pinRST, m_widthScreen, m_heightScreen);
m_pinSCL = pinSCL;
m_pinSDA = pinSDA;
m_pinINT = pinINT;
m_pinRST = pinRST;
touch_init();
}
#endif

uint16_t TouchScreen::getWidthScreen()
{
#if HAS_TOUCH
  return m_widthScreen;
#else
  return 0;
#endif
}

uint16_t TouchScreen::getHeightScreen()
{
#if HAS_TOUCH
  return m_heightScreen;
#else
  return 0;
#endif
}

/**
 * @brief Destructor for the TouchScreen class.
 */
TouchScreen::~TouchScreen()
{
  if(m_ts){
    delete m_ts;
  }
}



#if defined(TOUCH_FT6236)
TouchScreen *TouchScreen::m_instance = nullptr;

/**
 * @brief Handles touch events.
 * @param p Touch point.
 * @param e Touch event type.
 */
void TouchScreen::touch(TPoint p, TEvent e)
{
  if (e != TEvent::Tap && e != TEvent::DragStart && e != TEvent::DragMove && e != TEvent::DragEnd)
  {
    return;
  }
  // translation logic depends on screen rotation
#if defined(TOUCH_SWAP_XY)
  m_touch_last_x = map(p.y, TOUCH_MAP_X1, TOUCH_MAP_X2, 0, m_widthScreen);
  m_touch_last_y = map(p.x, TOUCH_MAP_Y1, TOUCH_MAP_Y2, 0, m_heightScreen);
#else
  m_touch_last_x = map(p.x, TOUCH_MAP_X1, TOUCH_MAP_X2, 0, m_widthScreen);
  m_touch_last_y = map(p.y, TOUCH_MAP_Y1, TOUCH_MAP_Y2, 0, m_heightScreen);
#endif
  switch (e)
  {
  case TEvent::Tap:
    Serial.println("Tap");
    m_touch_touched_flag = true;
    m_touch_released_flag = true;
    break;
  case TEvent::DragStart:
    Serial.println("DragStart");
    m_touch_touched_flag = true;
    break;
  case TEvent::DragMove:
    Serial.println("DragMove");
    m_touch_touched_flag = true;
    break;
  case TEvent::DragEnd:
    Serial.println("DragEnd");
    m_touch_released_flag = true;
    break;
  default:
    Serial.println("UNKNOWN");
    break;
  }
}
#endif

/**
 * @brief Initializes the touch screen.
 */
void TouchScreen::touch_init()
{
#if defined(TOUCH_GT911)
  m_ts->begin();
  m_ts->setRotation(m_rotation);

#elif defined(TOUCH_XPT2046)

#if defined (DISP_CS)
  digitalWrite(DISP_CS, HIGH);
  delayMicroseconds(5);
#endif

  
  m_ts->begin(m_spitoque, TOUCH_FREQUENCY, DISP_FREQUENCY);

  
#if defined (DISP_CS)
  delayMicroseconds(5);
  digitalWrite(DISP_CS, LOW);
  #endif

#elif defined(TOUCH_FT6236U)
  if (!m_ts->begin(40, m_pinSDA, m_pinSCL)) // 40 in this case represents the sensitivity. Try higer or lower for better response.
  {
    Serial.println("Unable to start the capacitive touchscreen.");
  }
  else
  {
    Serial.println("Touch initialized.");
  }
#elif defined(TOUCH_CST816)
  m_ts->begin();
#elif defined(TOUCH_FT6336U)
  m_ts->begin();
  showSetup();
#endif
}

#if defined(TOUCH_FT6336U)
void TouchScreen::showSetup()
{
  Serial.print("FT6336U Device Mode: ");
  Serial.println(m_ts->read_device_mode());
  Serial.print("FT6336U Threshold: 0x");
  Serial.println(m_ts->read_touch_threshold(), HEX);
  Serial.print("FT6336U Filter Coefficient: 0x");
  Serial.println(m_ts->read_filter_coefficient(), HEX);
  Serial.print("FT6336U Control Mode: 0x");
  Serial.println(m_ts->read_ctrl_mode(), HEX);
  Serial.print("FT6336U Time Period for enter to Monitor Mode: 0x");
  Serial.println(m_ts->read_time_period_enter_monitor(), HEX);
  Serial.print("FT6336U Active Rate: 0x");
  Serial.println(m_ts->read_active_rate(), HEX);
  Serial.print("FT6336U Monitor Rate: 0x");
  Serial.println(m_ts->read_monitor_rate(), HEX);

  Serial.print("FT6336U LIB Ver: 0x");
  Serial.println(m_ts->read_library_version(), HEX);
  Serial.print("FT6336U Chip ID: 0x");
  Serial.println(m_ts->read_chip_id(), HEX);
  Serial.print("FT6336U G Mode: 0x");
  Serial.println(m_ts->read_g_mode(), HEX);
  Serial.print("FT6336U POWER Mode: 0x");
  Serial.println(m_ts->read_pwrmode(), HEX);
  Serial.print("FT6336U Firm ID: 0x");
  Serial.println(m_ts->read_firmware_id(), HEX);
  Serial.print("FT6336U Focal Tehc ID: 0x");
  Serial.println(m_ts->read_focaltech_id(), HEX);
  Serial.print("FT6336U Release Code ID: 0x");
  Serial.println(m_ts->read_release_code_id(), HEX);
  Serial.print("FT6336U State: 0x");
  Serial.println(m_ts->read_state(), HEX);
}
#endif


void TouchScreen::setDimension(uint16_t _widthScreen, uint16_t _heightScreen, uint8_t _rotation)
{
  m_widthScreen = _widthScreen;
  m_heightScreen = _heightScreen;
  m_rotation = _rotation;
  m_startedTouch = true;
}

/**
 * @brief Checks if the touch screen has a signal.
 * @return True if the touch screen has a signal, false otherwise.
 */
bool TouchScreen::touch_has_signal()
{
#if defined(TOUCH_FT6236)
  m_ts->loop();
  return m_touch_touched_flag || m_touch_released_flag;

#elif defined(TOUCH_GT911)
  return true;

#elif defined(TOUCH_XPT2046)
  return true;
#elif defined(TOUCH_FT6236U)
  return true;
#elif defined(TOUCH_CST816)
  return true;
#elif defined(TOUCH_FT6336U)
  tp = m_ts->scan();
  return tp.touch_count > 0;
#else
  return false;
#endif
}

/**
 * @brief Checks if the touch screen has been touched.
 * @return True if the touch screen has been touched, false otherwise.
 */
bool TouchScreen::touch_touched()
{
#if defined(TOUCH_FT6236) && HAS_TOUCH
  if (m_touch_touched_flag)
  {
    m_touch_touched_flag = false;
    // touch_last_x = 0;
    // touch_last_y = 0;
    Serial.printf("Clicou %i x %i\n", m_touch_last_x, m_touch_last_y);
    return true;
  }
  else
  {
    return false;
  }

#elif defined(TOUCH_GT911) && HAS_TOUCH
  m_ts->read();
  if (m_ts->isTouched)
  {
#if defined(TOUCH_SWAP_XY)
    m_touch_last_x = map(m_ts->points[0].y, TOUCH_MAP_X1, TOUCH_MAP_X2, 0, m_widthScreen - 1);
    m_touch_last_y = map(m_ts->points[0].x, TOUCH_MAP_Y1, TOUCH_MAP_Y2, 0, m_heightScreen - 1);
    m_touch_last_z = -1;
#else
    // Raw touch coordinates
    Serial.printf("Raw touch coordinates: %i, %i\n", m_ts->points[0].x, m_ts->points[0].y);
    m_touch_last_x = map(m_ts->points[0].x, TOUCH_MAP_X1, TOUCH_MAP_X2, 0, m_widthScreen - 1);
    m_touch_last_y = map(m_ts->points[0].y, TOUCH_MAP_Y1, TOUCH_MAP_Y2, 0, m_heightScreen - 1);
    m_touch_last_z = -1;
#endif

    return true;
  }
  else
  {
    return false;
  }

#elif defined(TOUCH_XPT2046) && HAS_TOUCH

  if (!m_calibMatrix)
  {
    Serial.println("Calibration not defined");
    return false;
  }


  int16_t xTouch = 0, yTouch = 0, zTouch = 0;
  int16_t touchAmount = 8;
  uint8_t touchDetect = touchAmount;
  // bool validTouch = false;

  while (m_ts->getInputBodmer() && touchDetect > 0)
  {
    xTouch += m_ts->x;
    yTouch += m_ts->y;
    zTouch += m_ts->z;
    touchDetect--;
  }
  xTouch /= touchAmount;
  yTouch /= touchAmount;
  zTouch /= touchAmount;

  bool hasTouch = (touchDetect == 0);

#if defined(DISP_CS)
  // delayMicroseconds(5);
  // digitalWrite(DISP_CS, LOW);
  // log_d("Pino %i modo LOW", DISP_CS);
#endif

  if (hasTouch)
  {

    Serial.printf("Raw xyz[%i\t%i\t%i]\n", xTouch, yTouch, zTouch);
    ScreenPoint_t toque = getScreenPosition(xTouch, yTouch); // Raw XY touch converted to XY screen
                                                             // Serial.printf("Raw xyz[%i\t%i\t%i]\n", xTouch, yTouch, zTouch);
                                                             // Serial.printf("Scr xyz[%i\t%i\t%i]\n", toque.x, toque.y, zTouch);

#if defined(TOUCH_SWAP_XY)

    m_ts->x = constrain(xTouch, m_calibMatrix[2], m_calibMatrix[3]);
    m_ts->y = constrain(yTouch, m_calibMatrix[0], m_calibMatrix[1]);

    m_touch_last_x = map(m_ts->y, m_calibMatrix[0], m_calibMatrix[1], 0, m_widthScreen - 1);
    m_touch_last_y = map(m_ts->x, m_calibMatrix[2], m_calibMatrix[3], 0, m_heightScreen - 1);
#else
    m_touch_last_x = toque.x;
    m_touch_last_y = toque.y;
    m_touch_last_z = zTouch;
#endif

    return true;
  }
  else
  {
    return false;
  }

#elif defined(TOUCH_FT6236U) && HAS_TOUCH
  if (m_ts->touched())
  {
    // Retrieve a point
    TS_Point p = m_ts->getPoint();

    // Print coordinates to the serial output
    // Serial.printf("X: %i, Y: %i\n", p.x, p.y);
#if defined(TOUCH_SWAP_XY)
    m_touch_last_x = map(p.y, TOUCH_MAP_X1, TOUCH_MAP_X2, 0, m_widthScreen - 1);
    m_touch_last_y = map(p.x, TOUCH_MAP_Y1, TOUCH_MAP_Y2, 0, m_heightScreen - 1);
    m_touch_last_z = 0;
#else
    m_touch_last_x = map(p.x, TOUCH_MAP_X1, TOUCH_MAP_X2, 0, m_widthScreen - 1);
    m_touch_last_y = map(p.y, TOUCH_MAP_Y1, TOUCH_MAP_Y2, 0, m_heightScreen - 1);
    m_touch_last_z = 0;
#endif

    return true;
  }
  else
  {
    return false;
  }

#elif defined(TOUCH_CST816) && HAS_TOUCH
  bool touched;
  uint8_t gesture;
  uint16_t touchX, touchY;
  touched = m_ts->available();
  if (touched)
  {
    m_touch_last_x = m_ts->data.x;
    m_touch_last_y = m_ts->data.y;
    m_gesture = m_ts->data.gestureID;
    m_touch_last_z = -1;
  }

  return touched;
#elif defined(TOUCH_FT6336U) && HAS_TOUCH
  bool touched = tp.touch_count > 0;
  if (!touched)
    return false;

  uint16_t rx = tp.tp[0].x; // raw X do FT6336U
  uint16_t ry = tp.tp[0].y; // raw Y do FT6336U
  uint16_t sx, sy;          // coordenadas já convertidas

  switch (m_rotation & 3) // 0,1,2,3
  {
  case 0: // Portrait   (0°)
    sx = map(rx, TOUCH_MAP_X1, TOUCH_MAP_X0, 0, m_widthScreen - 1);
    sy = map(ry, TOUCH_MAP_Y0, TOUCH_MAP_Y1, 0, m_heightScreen - 1);
    break;

  case 1: // Landscape  (90°)
    sx = map(ry, TOUCH_MAP_Y0, TOUCH_MAP_Y1, 0, m_widthScreen - 1);
    sy = map(rx, TOUCH_MAP_X0, TOUCH_MAP_X1, 0, m_heightScreen - 1);
    break;

  case 2: // Portrait   (180°)
    sx = map(rx, TOUCH_MAP_X0, TOUCH_MAP_X1, 0, m_widthScreen - 1);
    sy = map(ry, TOUCH_MAP_Y1, TOUCH_MAP_Y0, 0, m_heightScreen - 1);
    break;

  default: // Landscape  (270°)
    sx = map(ry, TOUCH_MAP_Y1, TOUCH_MAP_Y0, 0, m_widthScreen - 1);
    sy = map(rx, TOUCH_MAP_X1, TOUCH_MAP_X0, 0, m_heightScreen - 1);
    break;
  }

  /* Garante que nada extrapole os limites físicos do display */
  m_touch_last_x = constrain(sx, 0, m_widthScreen - 1);
  m_touch_last_y = constrain(sy, 0, m_heightScreen - 1);
  m_touch_last_z = -1; // pressão fixa ou não usada

  return true;
#else
  return false;
#endif
}

/**
 * @brief Checks if the touch screen has been released.
 * @return True if the touch screen has been released, false otherwise.
 */
bool TouchScreen::touch_released()
{
#if defined(TOUCH_FT6236)
  if (m_touch_released_flag)
  {
    m_touch_released_flag = false;
    return true;
  }
  else
  {
    return false;
  }

#elif defined(TOUCH_GT911)
  return true;

#elif defined(TOUCH_XPT2046)
  return true;
#elif defined(TOUCH_FT6236U)
  return true;

#else
  return false;
#endif
}

#if defined(TOUCH_FT6236)
/**
 * @brief Handles touch events.
 * @param p Touch point.
 * @param e Touch event type.
 */
void TouchScreen::touchHandler(TPoint p, TEvent e)
{
  if (m_instance)
  {
    m_instance->touch(p, e); // Redireciona para o método de instância
  }
}

#endif

/**
 * @brief Gets the rotation of the touch screen.
 * @return Rotation of the touch screen.
 */
uint8_t TouchScreen::getRotation()
{
  return m_rotation;
}

/**
 * @brief Sets the calibration of the touch screen.
 * @param array Array of calibration points.
 */
#if HAS_TOUCH && defined(TOUCH_XPT2046)
void TouchScreen::setCalibration(CalibrationPoint_t *array)
{
  delete[] m_calibMatrix;
  if (m_calibMatrix)
  {
    delete m_calibMatrix;
  }
  m_calibMatrix = new CalibrationPoint_t[4];
  memcpy(m_calibMatrix, array, sizeof(array) * sizeof(CalibrationPoint_t));
  // Serial.printf("Tamanho array %u", sizeof(array) * sizeof(CalibrationPoint_t));

  for (size_t i = 0; i < 5; i++)
  {
    // calibMatrix[i] = array[i];
  }

  /*Serial.println("Array base");
  for (uint8_t i = 0; i < 4; i++)
  {
    Serial.printf("Tela: %i x %i = Touch: %i x %i\n", array[i].xScreen, array[i].yScreen, array[i].xTouch, array[i].yTouch);
  }

  Serial.println("Set Calibration data");
  for (uint8_t i = 0; i < 4; i++)
  {
    Serial.printf("Tela: %i x %i = Touch: %i x %i\n", calibMatrix[i].xScreen, calibMatrix[i].yScreen, calibMatrix[i].xTouch, calibMatrix[i].yTouch);
  }*/
}
#endif
/**
 * @brief Calibrates the touch screen using 9 points.
 * @param parameters Array of calibration parameters.
 * @param color_fg Color of the foreground.
 * @param color_bg Color of the background.
 * @param size Size of the calibration points.
 */
#if HAS_TOUCH && defined(TOUCH_XPT2046)
void TouchScreen::calibrateTouch9Points(uint16_t *parameters, uint32_t color_fg, uint32_t color_bg, uint8_t size)
{
  int16_t values[18] = {0}; // 9 pontos (x, y) = 18 valores

  if (!m_objTFT)
  {
    Serial.println("Display not defined");
    return;
  }
  else
  {
    Serial.println("Obj display definido");
    m_objTFT->fillScreen(0xffff);
  }

  // Posições dos pontos de calibração: 9 pontos
  int points[9][2] = {
      {0, 0},                                  // canto superior esquerdo
      {m_widthScreen - 1, 0},                  // canto superior direito
      {0, m_heightScreen - 1},                 // canto inferior esquerdo
      {m_widthScreen - 1, m_heightScreen - 1}, // canto inferior direito
      {m_widthScreen / 2, 0},                  // meio superior
      {m_widthScreen / 2, m_heightScreen - 1}, // meio inferior
      {0, m_heightScreen / 2},                 // meio esquerdo
      {m_widthScreen - 1, m_heightScreen / 2}, // meio direito
      {m_widthScreen / 2, m_heightScreen / 2}  // centro
  };

  for (uint8_t i = 0; i < 9; i++)
  {
    for (int j = 0; j < 9; j++)
    {
      int x = points[j][0];
      int y = points[j][1];

      m_objTFT->fillCircle(x, y, size / 2, color_bg);
      Serial.printf("Ponto %i: %i x %i\n", j, x, y);
    }

    // Desenhe o quadrado vermelho com linhas cruzadas no ponto de calibração
    int x = points[i][0];
    int y = points[i][1];
    m_objTFT->fillCircle(x, y, size / 3, color_fg);

    // Aguarde o usuário tocar no ponto
    Serial.printf("Calibrando ponto %d\n", i);
    for (uint8_t j = 0; j < 8; j++) // Média de 8 leituras para cada ponto
    {
      while (!m_ts->getInputBodmer())
        ; // Aguarde até detectar o toque
      values[i * 2] += m_ts->x;
      values[i * 2 + 1] += m_ts->y;
      Serial.printf("{%d\t%d\t%d}\n", m_ts->x, m_ts->y, m_ts->z);
    }

    values[i * 2] /= 8;
    values[i * 2 + 1] /= 8;
    Serial.printf("Calibrado ponto %d => %d, %d\n", i, values[i * 2], values[i * 2 + 1]);

    m_objTFT->fillCircle(points[i][0], points[i][1], size, color_bg); // Apaga o ponto de toque com a cor de fundo
    delay(2000);                                                      // Aguarda antes do próximo ponto
  }

  // Determina se há rotação entre os eixos de toque e da tela
  m_touchCalibration_rotate = false;
  if (abs(values[0] - values[1]) > abs(values[2] - values[3]))
  {
    m_touchCalibration_rotate = true;
  }

  // Determina os valores de x e y mínimos e máximos para o ajuste da calibração
  m_touchCalibration_x0 = min(values[0], min(values[2], min(values[4], min(values[6], values[8]))));
  m_touchCalibration_x1 = max(values[0], max(values[2], max(values[4], max(values[6], values[8]))));

  m_touchCalibration_y0 = min(values[1], min(values[3], min(values[5], min(values[7], values[9]))));
  m_touchCalibration_y1 = max(values[1], max(values[3], max(values[5], max(values[7], values[9]))));

  // Detecta e ajusta a inversão dos eixos
  m_touchCalibration_invert_x = m_touchCalibration_x0 > m_touchCalibration_x1;
  m_touchCalibration_invert_y = m_touchCalibration_y0 > m_touchCalibration_y1;

  if (m_touchCalibration_invert_x)
  {
    int temp = m_touchCalibration_x0;
    m_touchCalibration_x0 = m_touchCalibration_x1;
    m_touchCalibration_x1 = temp;
  }

  if (m_touchCalibration_invert_y)
  {
    int temp = m_touchCalibration_y0;
    m_touchCalibration_y0 = m_touchCalibration_y1;
    m_touchCalibration_y1 = temp;
  }

  // Exporta os parâmetros se o ponteiro for válido
  if (parameters != NULL)
  {
    parameters[0] = m_touchCalibration_x0;
    parameters[1] = m_touchCalibration_x1;
    parameters[2] = m_touchCalibration_y0;
    parameters[3] = m_touchCalibration_y1;
    parameters[4] = m_touchCalibration_rotate | (m_touchCalibration_invert_x << 1) | (m_touchCalibration_invert_y << 2);
  }

  Serial.println("Calibração concluída:");
  Serial.printf("x0: %d, x1: %d, y0: %d, y1: %d\n", m_touchCalibration_x0, m_touchCalibration_x1, m_touchCalibration_y0, m_touchCalibration_y1);
}
#endif
/**
 * @brief Calibrates the touch screen using a structure.
 * @param points Array of calibration points.
 * @param length Length of the array of calibration points.
 * @param rectScreen Rectangle of the screen.
 * @param color_fg Color of the foreground.
 * @param color_bg Color of the background.
 * @param sizeMarker Size of the marker.
 */
#if HAS_TOUCH && defined(TOUCH_XPT2046)
void TouchScreen::calibrateTouchEstrutura(CalibrationPoint_t *points, uint8_t length, Rect_t *rectScreen, uint32_t color_fg, uint32_t color_bg, uint8_t sizeMarker)
{
  // CalibrationPoint_t points[4];

  if (!m_objTFT)
  {
    Serial.println("Display not defined");
    return;
  }
  else
  {
    Serial.println("Obj display definido");
    m_objTFT->fillScreen(0xffff);
    Serial.println(m_objTFT->width());
  }
  // uint16_t x_tmp, y_tmp;

  Serial.printf("Width: %i, Height: %i\n", m_widthScreen, m_heightScreen);

  points[0].xScreen = rectScreen->x;
  points[0].yScreen = rectScreen->y;
  points[0].xTouch = 0;
  points[0].yTouch = 0;

  points[1].xScreen = rectScreen->x + rectScreen->width;
  points[1].yScreen = rectScreen->y;
  points[1].xTouch = 0;
  points[1].yTouch = 0;

  points[2].xScreen = rectScreen->x + rectScreen->width;
  points[2].yScreen = rectScreen->y + rectScreen->height;
  points[2].xTouch = 0;
  points[2].yTouch = 0;

  points[3].xScreen = rectScreen->x;
  points[3].yScreen = rectScreen->y + rectScreen->height;
  points[3].xTouch = 0;
  points[3].yTouch = 0;

  for (auto i = 0; i < length; i++)
  {
    // objTFT->fillCircle(points[0].xScreen, points[0].yScreen, size / 2, color_bg);
    // objTFT->fillCircle(points[1].xScreen, points[1].yScreen, size / 2, color_bg);
    // objTFT->fillCircle(points[2].xScreen, points[2].yScreen, size / 2, color_bg);
    // objTFT->fillCircle(points[3].xScreen, points[3].yScreen, size / 2, color_bg);

    m_objTFT->fillCircle(points[i].xScreen, points[i].yScreen, sizeMarker, color_fg);

    // drawStar(points[i].xScreen, points[i].yScreen, sizeMarker, color_bg);

    delay(1000);

    // objTFT->fillCircle(widthScreen / 2, heightScreen / 2, 20, 0x07c0);
    Serial.printf("Calibration point %i: %i, %i\n", i, points[i].xScreen, points[i].yScreen);
    const uint16_t margin = 50;

    m_objTFT->fillRect(rectScreen->x + margin, rectScreen->y + margin, rectScreen->width - 2 * margin, rectScreen->height - 2 * margin, 0xffff);
    m_objTFT->setCursor(rectScreen->width / 2, rectScreen->height / 2);
    m_objTFT->setTextColor(0x0);
    m_objTFT->print("Waiting");

    const uint8_t captureCount = 8;

    for (uint8_t j = 0; j < captureCount; j++)
    {
      while (!m_ts->getInputBodmer())
        ;
      Serial.printf("%i, %i\n", m_ts->x, m_ts->y);
      points[i].xTouch += m_ts->x;
      points[i].yTouch += m_ts->y;
      Serial.printf("{%i\t%i\t%i}\n", m_ts->x, m_ts->y, m_ts->z);
    }
    points[i].xTouch /= captureCount;
    points[i].yTouch /= captureCount;

    m_objTFT->fillCircle(points[i].xScreen, points[i].yScreen, sizeMarker, color_bg);
    Serial.printf("Calibrado %i => %i, %i\n", i, points[i].xTouch, points[i].yTouch);
    m_objTFT->setCursor(rectScreen->width / 2, rectScreen->height / 2);
    m_objTFT->print("OK");

    delay(2000);
  }

  uint16_t mediaX = 0, mediaMinX = 0, mediaMaxX = 0;
  uint16_t mediaY = 0, mediaMinY = 0, mediaMaxY = 0;

  // Somar o X de todos os pontos, e somar todos os Y tambem
  for (uint8_t i = 0; i < length; i++)
  {
    Serial.printf("Tela: %i x %i = Touch: %i x %i\n", points[i].xScreen, points[i].yScreen, points[i].xTouch, points[i].yTouch);
    mediaX += points[i].xTouch;
    mediaY += points[i].yTouch;
  }

  // Tirar a media para depois encontrar os dois menores e dois maiores
  mediaX /= length;
  mediaY /= length;

  // Para cada ponto, verificar se X é menor ou maior que a media e somar na varivel para depois tirar a media entre os dois menores e dois maiores
  for (uint8_t i = 0; i < length; i++)
  {
    if (points[i].xTouch < mediaX)
    {
      mediaMinX += points[i].xTouch;
    }
    else
    {
      mediaMaxX += points[i].xTouch;
    }

    if (points[i].yTouch < mediaY)
    {
      mediaMinY += points[i].yTouch;
    }
    else
    {
      mediaMaxY += points[i].yTouch;
    }
  }

  // media dos pontos menores e maiores
  mediaMinX /= (length / 2);
  mediaMaxX /= (length / 2);
  mediaMinY /= (length / 2);
  mediaMaxY /= (length / 2);

  // Normatiza os pontos para formar um retangulo medio do touch
  for (uint8_t i = 0; i < length; i++)
  {
    points[i].xTouch = points[i].xTouch < mediaX ? mediaMinX : mediaMaxX;
    points[i].yTouch = points[i].yTouch < mediaY ? mediaMinY : mediaMaxY;
  }

  // Mostra o retangulo touch normatizado
  for (uint8_t i = 0; i < length; i++)
  {
    Serial.printf("Normatized Tela: %i x %i = Touch: %i x %i\n", points[i].xScreen, points[i].yScreen, points[i].xTouch, points[i].yTouch);
  }

  Serial.println("Terminou");
}
#endif
/**
 * @brief Draws a star on the screen.
 * @param xPos X position of the star.
 * @param yPos Y position of the star.
 * @param size Size of the star.
 * @param color Color of the star.
 */
#if defined(TOUCH_XPT2046) && HAS_TOUCH

CalibrationPoint_t TouchScreen::getMinPoint(CalibrationPoint_t pontos[4]) {
  CalibrationPoint_t minPoint = pontos[0];

  for (int i = 1; i < 4; ++i) {
    if (pontos[i].xTouch <= minPoint.xTouch && pontos[i].yTouch <= minPoint.yTouch) {
      minPoint = pontos[i];
    }
  }

  return minPoint;
}

// Retorna o ponto com o maior xTouch e maior yTouch
CalibrationPoint_t TouchScreen::getMaxPoint(CalibrationPoint_t pontos[4]) {
  CalibrationPoint_t maxPoint = pontos[0];

  for (int i = 1; i < 4; ++i) {
    if (pontos[i].xTouch >= maxPoint.xTouch && pontos[i].yTouch >= maxPoint.yTouch) {
      maxPoint = pontos[i];
    }
  }

  return maxPoint;
}

void TouchScreen::drawStar(int16_t xPos, int16_t yPos, uint8_t size, uint16_t color)
{
  if (m_objTFT)
  {
  
    m_objTFT->fillCircle(xPos, yPos, size, color);
  }
}
#endif
/**
 * @brief Gets the screen position of the touch screen.
 * @param xTouch X position of the touch.
 * @param yTouch Y position of the touch.
 * @return Screen position of the touch.
 */
#if defined(TOUCH_XPT2046) && HAS_TOUCH
ScreenPoint_t TouchScreen::getScreenPosition(int16_t xTouch, int16_t yTouch)
{
  ScreenPoint_t screenPos = {0, 0};

  if (!m_calibMatrix)
    return screenPos;

  Serial.printf("Get screen position for xTouch: %i and yTouch: %i\n", xTouch, yTouch);

  CalibrationPoint_t cornerMin = getMinPoint(m_calibMatrix);
  CalibrationPoint_t cornerMax = getMaxPoint(m_calibMatrix);

  Serial.printf("Ponto minimo: %i x %i e ponto maximo: %i x %i\n", cornerMin.xTouch, cornerMin.yTouch, cornerMax.xTouch, cornerMax.yTouch);

  xTouch = constrain(xTouch, cornerMin.xTouch, cornerMax.xTouch);
  yTouch = constrain(yTouch, cornerMin.yTouch, cornerMax.yTouch);

  int16_t xMapFrom, xMapTo, yMapFrom, yMapTo;
  int16_t xMapVal, yMapVal;

  switch (m_rotation)
  {
    case 0:
      xMapVal = xTouch;
      yMapVal = yTouch;

      xMapFrom = TOUCH_INVERT_X ? cornerMax.xTouch : cornerMin.xTouch;
      xMapTo   = TOUCH_INVERT_X ? cornerMin.xTouch : cornerMax.xTouch;

      yMapFrom = TOUCH_INVERT_Y ? cornerMin.yTouch : cornerMax.yTouch;
      yMapTo   = TOUCH_INVERT_Y ? cornerMax.yTouch : cornerMin.yTouch;
      break;

    case 1:
      xMapVal = yTouch;
      yMapVal = xTouch;

      xMapFrom = TOUCH_INVERT_X ? cornerMin.yTouch : cornerMax.yTouch;
      xMapTo   = TOUCH_INVERT_X ? cornerMax.yTouch : cornerMin.yTouch;

      yMapFrom = TOUCH_INVERT_Y ? cornerMin.xTouch : cornerMax.xTouch;
      yMapTo   = TOUCH_INVERT_Y ? cornerMax.xTouch : cornerMin.xTouch;
      break;

    case 2:
      xMapVal = xTouch;
      yMapVal = yTouch;

      xMapFrom = TOUCH_INVERT_X ? cornerMin.xTouch : cornerMax.xTouch;
      xMapTo   = TOUCH_INVERT_X ? cornerMax.xTouch : cornerMin.xTouch;

      yMapFrom = TOUCH_INVERT_Y ? cornerMax.yTouch : cornerMin.yTouch;
      yMapTo   = TOUCH_INVERT_Y ? cornerMin.yTouch : cornerMax.yTouch;
      break;

    case 3:
      xMapVal = yTouch;
      yMapVal = xTouch;

      xMapFrom = TOUCH_INVERT_X ? cornerMax.yTouch : cornerMin.yTouch;
      xMapTo   = TOUCH_INVERT_X ? cornerMin.yTouch : cornerMax.yTouch;

      yMapFrom = TOUCH_INVERT_Y ? cornerMax.xTouch : cornerMin.xTouch;
      yMapTo   = TOUCH_INVERT_Y ? cornerMin.xTouch : cornerMax.xTouch;
      break;

    default:
      return screenPos;
  }

  screenPos.x = map(xMapVal, xMapFrom, xMapTo, 0, m_widthScreen);
  screenPos.y = map(yMapVal, yMapFrom, yMapTo, 0, m_heightScreen);

  Serial.printf("Mapped x: %i from [%i → %i] to [0 → %i] = %i\n", xMapVal, xMapFrom, xMapTo, m_widthScreen, screenPos.x);
  Serial.printf("Mapped y: %i from [%i → %i] to [0 → %i] = %i\n", yMapVal, yMapFrom, yMapTo, m_heightScreen, screenPos.y);

  return screenPos;
}
#endif
/**
 * @brief Calibrates the touch screen.
 * @param parameters Array of calibration parameters.
 * @param color_fg Color of the foreground.
 * @param color_bg Color of the background.
 * @param size Size of the calibration points.
 */
#if defined(TOUCH_XPT2046) && HAS_TOUCH
void TouchScreen::calibrateTouch(uint16_t *parameters, uint32_t color_fg, uint32_t color_bg, uint8_t size)
{
  int16_t values[] = {0, 0, 0, 0, 0, 0, 0, 0};

  if (!m_objTFT)
  {
    Serial.println("Display not defined");
    return;
  }
  else
  {
    Serial.println("Obj display definido");
    m_objTFT->fillScreen(0xffff);
    Serial.println(m_objTFT->width());
  }
  // uint16_t x_tmp, y_tmp;

  for (uint8_t i = 0; i < 4; i++)
  {
    m_objTFT->fillRect(0, 0, size + 1, size + 1, color_bg);
    m_objTFT->fillRect(0, m_heightScreen - size - 1, size + 1, size + 1, color_bg);
    m_objTFT->fillRect(m_widthScreen - size - 1, 0, size + 1, size + 1, color_bg);
    m_objTFT->fillRect(m_widthScreen - size - 1, m_heightScreen - size - 1, size + 1, size + 1, color_bg);

    if (i == 5)
      break; // used to clear the arrows

    switch (i)
    {
    case 0: // up left
      m_objTFT->drawLine(0, 0, 0, size, color_fg);
      m_objTFT->drawLine(0, 0, size, 0, color_fg);
      m_objTFT->drawLine(0, 0, size, size, color_fg);
      break;
    case 1: // bot left
      m_objTFT->drawLine(0, m_heightScreen - size - 1, 0, m_heightScreen - 1, color_fg);
      m_objTFT->drawLine(0, m_heightScreen - 1, size, m_heightScreen - 1, color_fg);
      m_objTFT->drawLine(size, m_heightScreen - size - 1, 0, m_heightScreen - 1, color_fg);
      break;
    case 2: // up right
      m_objTFT->drawLine(m_widthScreen - size - 1, 0, m_widthScreen - 1, 0, color_fg);
      m_objTFT->drawLine(m_widthScreen - size - 1, size, m_widthScreen - 1, 0, color_fg);
      m_objTFT->drawLine(m_widthScreen - 1, size, m_widthScreen - 1, 0, color_fg);
      break;
    case 3: // bot right
      m_objTFT->drawLine(m_widthScreen - size - 1, m_heightScreen - size - 1, m_widthScreen - 1, m_heightScreen - 1, color_fg);
      m_objTFT->drawLine(m_widthScreen - 1, m_heightScreen - 1 - size, m_widthScreen - 1, m_heightScreen - 1, color_fg);
      m_objTFT->drawLine(m_widthScreen - 1 - size, m_heightScreen - 1, m_widthScreen - 1, m_heightScreen - 1, color_fg);
      break;
    }

    // user has to get the chance to release
    if (i > 0)
      delay(1000);

    m_objTFT->fillCircle(m_widthScreen / 2, m_heightScreen / 2, 20, 0x07c0);
    Serial.printf("Calibrando %i\n", i);
    for (uint8_t j = 0; j < 8; j++)
    {
      // Use a lower detect threshold as corners tend to be less sensitive
      while (!m_ts->getInputBodmer())
        ;
      Serial.printf("%i, %i\n", m_ts->x, m_ts->y);
      values[i * 2] += m_ts->x;
      values[i * 2 + 1] += m_ts->y;
      Serial.printf("{%i\t%i\t%i}\n", m_ts->x, m_ts->y, m_ts->z);
    }
    values[i * 2] /= 8;
    values[i * 2 + 1] /= 8;
    Serial.printf("Calibrado %i => %i, %i\n", i, values[i * 2], values[i * 2 + 1]);
    m_objTFT->fillCircle(m_widthScreen / 2, m_heightScreen / 2, 20, 0xf800);
    delay(2000);
  }

  // from case 0 to case 1, the y value changed.
  // If the measured delta of the touch x axis is bigger than the delta of the y axis, the touch and TFT axes are switched.
  m_touchCalibration_rotate = false;
  if (abs(values[0] - values[2]) > abs(values[1] - values[3]))
  {
    m_touchCalibration_rotate = true;
    m_touchCalibration_x0 = (values[1] + values[3]) / 2; // calc min x
    m_touchCalibration_x1 = (values[5] + values[7]) / 2; // calc max x
    m_touchCalibration_y0 = (values[0] + values[4]) / 2; // calc min y
    m_touchCalibration_y1 = (values[2] + values[6]) / 2; // calc max y
  }
  else
  {
    m_touchCalibration_x0 = (values[0] + values[2]) / 2; // calc min x
    m_touchCalibration_x1 = (values[4] + values[6]) / 2; // calc max x
    m_touchCalibration_y0 = (values[1] + values[5]) / 2; // calc min y
    m_touchCalibration_y1 = (values[3] + values[7]) / 2; // calc max y
  }

  for (auto i = 0; i < 8; ++i)
  {
    Serial.print(values[i]);
    Serial.print("\t");
  }

  /*Serial.print("\n");
  Serial.println("Calibrated values before");
  Serial.println(touchCalibration_x0);
  Serial.println(touchCalibration_x1);
  Serial.println(touchCalibration_y0);
  Serial.println(touchCalibration_y1);
  Serial.println("-----------------");*/

  // in addition, the touch screen axis could be in the opposite direction of the TFT axis
  m_touchCalibration_invert_x = false;
  if (m_touchCalibration_x0 > m_touchCalibration_x1)
  {
    values[0] = m_touchCalibration_x0;
    m_touchCalibration_x0 = m_touchCalibration_x1;
    m_touchCalibration_x1 = values[0];
    m_touchCalibration_invert_x = true;
  }
  m_touchCalibration_invert_y = false;
  if (m_touchCalibration_y0 > m_touchCalibration_y1)
  {
    values[0] = m_touchCalibration_y0;
    m_touchCalibration_y0 = m_touchCalibration_y1;
    m_touchCalibration_y1 = values[0];
    m_touchCalibration_invert_y = true;
  }

  // pre calculate
  // touchCalibration_x1 -= touchCalibration_x0;
  // touchCalibration_y1 -= touchCalibration_y0;

  if (m_touchCalibration_x0 == 0)
    m_touchCalibration_x0 = 1;
  if (m_touchCalibration_x1 == 0)
    m_touchCalibration_x1 = 1;
  if (m_touchCalibration_y0 == 0)
    m_touchCalibration_y0 = 1;
  if (m_touchCalibration_y1 == 0)
    m_touchCalibration_y1 = 1;

  Serial.println("Calibrated values");
  Serial.println(m_touchCalibration_x0);
  Serial.println(m_touchCalibration_x1);
  Serial.println(m_touchCalibration_y0);
  Serial.println(m_touchCalibration_y1);
  Serial.println("-----------------");

  // export parameters, if pointer valid
  if (parameters != NULL)
  {
    parameters[0] = m_touchCalibration_x0;
    parameters[1] = m_touchCalibration_x1;
    parameters[2] = m_touchCalibration_y0;
    parameters[3] = m_touchCalibration_y1;
    parameters[4] = m_touchCalibration_rotate | (m_touchCalibration_invert_x << 1) | (m_touchCalibration_invert_y << 2);
    /*Serial.println("Calibrated parameter values");
    Serial.println(parameters[0]);
    Serial.println(parameters[1]);
    Serial.println(parameters[2]);
    Serial.println(parameters[3]);
    Serial.println(parameters[4]);
    Serial.println("-----------------");*/
  }
  else
  {
    Serial.println("No parameter to calibrate");
  }
}

#endif

/**
 * @brief Gets the gesture of the touch screen.
 * @return Gesture of the touch screen.
 */
uint8_t TouchScreen::getGesture()
{
  return m_gesture;
}

/**
 * @brief Gets the touch of the touch screen.
 * @param xTouch X position of the touch.
 * @param yTouch Y position of the touch.
 * @param zPressure Z position of the touch.
 * @return True if the touch screen has been touched, false otherwise.
 */
bool TouchScreen::getTouch(uint16_t *xTouch, uint16_t *yTouch, int *zPressure)
{

  if (!m_startedTouch)
  {
    return false;
  }

  if (touch_has_signal())
  {
    if (touch_touched())
    {
      (*xTouch) = m_touch_last_x;
      (*yTouch) = m_touch_last_y;
      (*zPressure) = m_touch_last_z;
      // Serial.println("Return true");
      return true;
    }
  }
  return false;
}