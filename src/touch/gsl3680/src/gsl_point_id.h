#ifndef _GSL_POINT_ID_H
#define _GSL_POINT_ID_H

#if defined(ESP_ARDUINO_VERSION_OK)

#if defined(ESP_ARDUINO_VERSION_MAJOR) && defined(ESP_ARDUINO_VERSION_MINOR)
    #if ESP_ARDUINO_VERSION_MAJOR < 3 || (ESP_ARDUINO_VERSION_MAJOR == 3 && ESP_ARDUINO_VERSION_MINOR < 3)
        #error "GSL3680_touch requer ESP Arduino Core versao 3.3.0 ou superior. Versao atual incompativel."
    #endif
#endif

struct gsl_touch_info
{
    int x[10];
    int y[10];
    int id[10];
    int finger_num;
};

unsigned int gsl_mask_tiaoping(void);
unsigned int gsl_version_id(void);
void gsl_alg_id_main(struct gsl_touch_info *cinfo);
void gsl_DataInit(unsigned int *conf_in);

#endif

#endif

