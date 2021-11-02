#include "menolfo_PWM.h"
#include "nrf_gpio.h"
#include "Nordic_HAL.h"

APP_PWM_INSTANCE(PWM1, 1); // Create the instance "PWM1" using TIMER1.

static volatile bool ready_flag; // A flag indicating PWM status.

void pwm_ready_callback(uint32_t pwm_id) // PWM callback function
{
    ready_flag = true;
}

void menolfo_pwm_init(void)
{
    ret_code_t err_code;
    app_pwm_config_t pwm1_cfg = APP_PWM_DEFAULT_CONFIG_1CH(500L, NRF_GPIO_PIN_MAP(1, 13));
    pwm1_cfg.pin_polarity[1] = APP_PWM_POLARITY_ACTIVE_LOW;
    err_code = app_pwm_init(&PWM1, &pwm1_cfg, pwm_ready_callback);
    APP_ERROR_CHECK(err_code);
    app_pwm_enable(&PWM1);
    setPWM(67);
}

ret_code_t setPWM(app_pwm_duty_t duty)
{
    return app_pwm_channel_duty_set(&PWM1, 0, duty);
}

void pulseRountine(uint16_t value)
{

    if (value >= 150)
    {
        /* Set the duty cycle - keep trying until PWM is ready... */
        while (setPWM(30) == NRF_ERROR_BUSY)
            ;

        /* ... or wait for callback. */
        halDelayMs(200);
        while (setPWM(67) == NRF_ERROR_BUSY)
            ;
        halDelayMs(200);
    }
    else
    {
        while (setPWM(67) == NRF_ERROR_BUSY)
            ;
    }
}