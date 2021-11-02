#ifndef MENOLFO_RGB_H
#define MENOLFO_RGB_H

#include "stdint.h"

void RGB(uint8_t led, uint8_t R, uint8_t G, uint8_t B);
void RGBAllLeds(uint8_t R, uint8_t G, uint8_t B);
void initRGB(void);
void setArColor(uint16_t value);
#endif /* MENOLFO_RGB_H */
