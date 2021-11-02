#include "menolfo.h"
#include "nrf_drv_twi.h"
#include "nrf_delay.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
///////////////////////////////////////////////////////////////
#include "Nordic_HAL.h"
///////////////////////////////////////////////////////////////
/* Buffer for samples read from temperature sensor. */
// static uint8_t m_sample;

/* Indicates if operation on TWI has ended. */
static volatile bool m_xfer_done = false;

/* TWI instance ID. */
/* TWI instance ID. */
#if TWI0_ENABLED
#define TWI_INSTANCE_ID     0
#elif TWI1_ENABLED
#define TWI_INSTANCE_ID     1
#endif

/* TWI instance. */
nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);

/**
 * @brief Function for handling data from I2C Sensors
 *
 * @param[in] temp          Temperature in Celsius degrees read from sensor.
 */
__STATIC_INLINE void data_handler(uint8_t temp)
{
    // NRF_LOG_INFO("Temperature: %d Celsius degrees.", temp);
}

/**
 * @brief TWI events handler.
 */
// void twi_handler(nrf_drv_twi_evt_t const *p_event, void *p_context)
// {
//     switch (p_event->type)
//     {
//     case NRF_DRV_TWI_EVT_DONE:
//         if (p_event->xfer_desc.type == NRF_DRV_TWI_XFER_RX)
//         {
//             data_handler(m_sample);
//         }
//         m_xfer_done = true;
//         break;
//     default:
//         break;
//     }
// }

/**
 * @brief TWI initialization.
 */
void twi_init(void)
{
    ret_code_t err_code;

    const nrf_drv_twi_config_t twi_press_sensor_config =
        {
            .scl = MENOLFO_SCL,
            .sda = MENOLFO_SDA,
            .frequency = NRF_DRV_TWI_FREQ_100K,
            .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
            .clear_bus_init = false};

    err_code = nrf_drv_twi_init(&m_twi, &twi_press_sensor_config, NULL,
                                NULL);
    APP_ERROR_CHECK(err_code);

    nrf_drv_twi_enable(&m_twi);
}
