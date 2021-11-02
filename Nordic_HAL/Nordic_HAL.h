#ifndef NORDIC_HAL_H
#define NORDIC_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nrf_drv_twi.h"
#include "nrf_delay.h"

extern nrf_drv_twi_t m_twi;

#define halDelayMs  nrf_delay_ms
ret_code_t hal_i2c_write(uint8_t address, uint8_t * cData, uint16_t len);
ret_code_t hal_i2c_read(uint8_t address, uint8_t * cData, uint16_t len);
long map(long x, long in_min, long in_max, long out_min, long out_max);
#ifdef __cplusplus
}
#endif

#endif /* NORDIC_HAL_H */
