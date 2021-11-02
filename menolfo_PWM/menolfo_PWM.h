#ifndef MENOLFO_PWM_H
#define MENOLFO_PWM_H
#include "app_pwm.h"

#ifdef __cplusplus
extern "C" {
#endif

void menolfo_pwm_init(void);

ret_code_t setPWM(app_pwm_duty_t duty);
void pulseRountine(uint16_t value);
#ifdef __cplusplus
}
#endif


#endif /* MENOLFO_PWM_H */
