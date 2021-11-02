#include "Nordic_HAL.h"

/**
 * @brief 
 * 
 * @param address 
 * @param cData 
 * @return ret_code_t 
 */
ret_code_t hal_i2c_write(uint8_t address, uint8_t *cData, uint16_t len)
{

    return nrf_drv_twi_tx(&m_twi, address, cData, len, false);
}

/**
 * @brief 
 * 
 * @param address 
 * @param cData 
 * @param len 
 * @return ret_code_t 
 */
ret_code_t hal_i2c_read(uint8_t address, uint8_t *cData, uint16_t len)
{

    return nrf_drv_twi_rx(&m_twi, address, cData, len);
}

/**
 * @brief 
 * 
 * @param x 
 * @param in_min 
 * @param in_max 
 * @param out_min 
 * @param out_max 
 * @return long 
 */
long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}