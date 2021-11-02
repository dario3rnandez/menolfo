#ifndef MENOLFO_H
#define MENOLFO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nrf_gpio.h"

///////////////////////////////////////////////////////////////
///User defines
#define MENOLFO_SDA      NRF_GPIO_PIN_MAP(1,15)
#define MENOLFO_SCL      NRF_GPIO_PIN_MAP(0,2)

/**
 * @brief TWI initialization.
 */
void twi_init(void);




#ifdef __cplusplus
}
#endif

#endif /* MENOLFO_H */
