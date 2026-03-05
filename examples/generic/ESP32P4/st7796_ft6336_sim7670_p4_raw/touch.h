#ifndef TOUCH_H
#define TOUCH_H

#include <Arduino.h>
#include <FT6336U.h>
#include "variaveis.h"

FT6336U *obj_touch = nullptr; ///< Ponteiro para instância do controlador de touch FT6336U.
FT6336U_TouchPointType tp; ///< Estrutura para armazenar o último estado de pontos de toque.

/* @brief Exibe, via Serial, parâmetros e registros do FT6336U para diagnóstico. */
void showSetup()
{
    if (!obj_touch)
    {
        Serial.println("Unable to start the capacitive touchscreen.");
    }
    Serial.print("FT6336 Device Mode: ");
    Serial.println(obj_touch->read_device_mode());
    Serial.print("FT6336 Threshold: 0x");
    Serial.println(obj_touch->read_touch_threshold(), HEX);
    Serial.print("FT6336 Filter Coefficient: 0x");
    Serial.println(obj_touch->read_filter_coefficient(), HEX);
    Serial.print("FT6336 Control Mode: 0x");
    Serial.println(obj_touch->read_ctrl_mode(), HEX);
    Serial.print("FT6336 Time Period for enter to Monitor Mode: 0x");
    Serial.println(obj_touch->read_time_period_enter_monitor(), HEX);
    Serial.print("FT6336 Active Rate: 0x");
    Serial.println(obj_touch->read_active_rate(), HEX);
    Serial.print("FT6336 Monitor Rate: 0x");
    Serial.println(obj_touch->read_monitor_rate(), HEX);

    Serial.print("FT6336 LIB Ver: 0x");
    Serial.println(obj_touch->read_library_version(), HEX);
    Serial.print("FT6336 Chip ID: 0x");
    Serial.println(obj_touch->read_chip_id(), HEX);
    Serial.print("FT6336 G Mode: 0x");
    Serial.println(obj_touch->read_g_mode(), HEX);
    Serial.print("FT6336 POWER Mode: 0x");
    Serial.println(obj_touch->read_pwrmode(), HEX);
    Serial.print("FT6336 Firm ID: 0x");
    Serial.println(obj_touch->read_firmware_id(), HEX);
    Serial.print("FT6336 Focal Tehc ID: 0x");
    Serial.println(obj_touch->read_focaltech_id(), HEX);
    Serial.print("FT6336 Release Code ID: 0x");
    Serial.println(obj_touch->read_release_code_id(), HEX);
    Serial.print("FT6336 State: 0x");
    Serial.println(obj_touch->read_state(), HEX);
}

/* @brief Inicializa o controlador de touch FT6336U e imprime informações de setup. */
void startTouch(){
    obj_touch = new FT6336U(TOUCH_SDA, TOUCH_SCL, TOUCH_RST, TOUCH_INT);
    obj_touch->begin();
    delay(2000);
    showSetup();
}

/* @brief Lê o estado do touch e retorna coordenadas mapeadas conforme configuração.
 * @param x Ponteiro para receber a coordenada X calculada.
 * @param y Ponteiro para receber a coordenada Y calculada.
 * @return true se há ao menos um toque detectado, false caso contrário.
 */
bool check_touch(int *x, int *y)
{
    if (!obj_touch)
    {
        Serial.println("Unable to start the capacitive touchscreen.");
    }

    tp = obj_touch->scan();
    bool touched = tp.touch_count > 0;
    if (touched)
    {
        TouchStatusEnum status0 = (tp.tp[0].status);
        TouchStatusEnum status1 = (tp.tp[1].status);

        uint16_t rx = 0; // raw X do FT6336
        uint16_t ry = 0; // raw Y do FT6336
        if (status0 == TouchStatusEnum::touch || status0 == TouchStatusEnum::stream)
        {
            rx = tp.tp[0].x;
            ry = tp.tp[0].y;
        }
        else if (status1 == TouchStatusEnum::touch || status1 == TouchStatusEnum::stream)
        {
            rx = tp.tp[1].x;
            ry = tp.tp[1].y;
        }

        if(TOUCH_INVERT_X){
            rx = TOUCH_MAP_X1 - rx;
        }

        if(TOUCH_INVERT_Y){
            ry = TOUCH_MAP_Y1 - ry;
        }

        if(TOUCH_SWAP_XY){
            *x = ry;
            *y = rx;
        }else{
            *x = rx;
            *y = ry;
        }
        

        Serial.printf("Touch [%d, %d]\r\n", rx, ry);
    }
    return touched;
}



#endif
