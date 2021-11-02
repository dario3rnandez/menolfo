#include "menolfo_RGB.h"
#include "IS31FL3236A.h"
#include "stddef.h"
uint8_t freq = 22;         // set PWM frequency, either 3 kHz (default) or 22 kHz allowed
uint8_t outCurrent = 0x00; // choices are 0x00 (IMax = 16 mA), 0x01 (IMax/2), 0x02 (IMax/3), and 0x03 (IMax/4)

IS31FL3236A IS31FL3236A; // instantiate IS31FL3236A class

void initRGB(void)
{
    IS31FL3236A.init(freq, outCurrent); // set PWM frequency and output current for all leds
    IS31FL3236A.setRunMode();
}

void RGB(uint8_t led, uint8_t R, uint8_t G, uint8_t B)
{
    if (led == 0 || led > 12)
    {
        return;
    }
    IS31FL3236A.setPWM(3 * (led - 1) + 1, G);
    IS31FL3236A.setPWM(3 * (led - 1) + 2, R);
    IS31FL3236A.setPWM(3 * (led - 1) + 3, B);

    IS31FL3236A.PWMUpdate();
}

void RGBAllLeds(uint8_t R, uint8_t G, uint8_t B)
{
    for (size_t i = 1; i < 13; i++)
    {
        if (i % 2 == 0)
        {
            RGB(i, R, G, B);
        }
    }
}

void setArColor(uint16_t value)
{

    if (value >= 0 && value < 100)
    {
        if (value >= 0 && value < 50)
        {
            RGBAllLeds(0, (uint8_t)((255 * value) / 50), 255);
        }
        else
        {
            RGBAllLeds(0, 255, (uint8_t)(255 - 255 * (value - 50) / 50));
        }
    }
    else if (value >= 100 && value < 200)
    {

        RGBAllLeds((uint8_t)((255 * (value - 100)) / 100), 255, 0);
    }
    else if (value >= 200 && value < 300)
    {
        RGBAllLeds(255, (uint8_t)(255 - (255 * (value - 200)) / 200), 0);
    }
    else if (value >= 300 && value < 500)
    {
        RGBAllLeds(255, (uint8_t)(127 - (127 * (value - 300)) / 200), 0);
    }
    else
    {
        return;
    }
}