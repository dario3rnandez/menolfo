#include "App_Test.h"

#include "Nordic_HAL.h"
#include "stdio.h"
#include "LC709203F.h"
#include "string.h"
#include "menolfo_RGB.h"
#include "SGP40.h"
#include "menolfo_PWM.h"

volatile char _valueBluetooth = '0';

void setvalueBluetooth(char valueBluetooth)
{
    _valueBluetooth = valueBluetooth;
}

char getvalueBluetooth(void)
{
    return _valueBluetooth;
}

uint8_t PWM_Gamma64[64] =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0b, 0x0d, 0x0f, 0x11, 0x13, 0x16,
        0x1a, 0x1c, 0x1d, 0x1f, 0x22, 0x25, 0x28, 0x2e,
        0x34, 0x38, 0x3c, 0x40, 0x44, 0x48, 0x4b, 0x4f,
        0x55, 0x5a, 0x5f, 0x64, 0x69, 0x6d, 0x72, 0x77,
        0x7d, 0x80, 0x88, 0x8d, 0x94, 0x9a, 0xa0, 0xa7,
        0xac, 0xb0, 0xb9, 0xbf, 0xc6, 0xcb, 0xcf, 0xd6,
        0xe1, 0xe9, 0xed, 0xf1, 0xf6, 0xfa, 0xfe, 0xff};

//Special customized functions *********************************************************

static void mode1(void) // Taken from ISSI Arduino code
{
    int8_t i = 0;
    int8_t j = 0;

    for (i = 1; i <= 12; i++) // First third
    {
        RGB(1, 0, 0, 0xFF); //set  PWM
        halDelayMs(80);     //100ms
    }
    halDelayMs(500); //keep 0.5s

    for (i = 1; i <= 12; i++) // Second third
    {
        RGB(1, 0, 0xFF, 0); //set  PWM
        halDelayMs(80);     //100ms
    }
    halDelayMs(500); //keep 0.5s

    for (i = 1; i <= 12; i++) // Last third
    {
        RGB(1, 0xFF, 0, 0); //set  PWM
        halDelayMs(80);     //100ms
    }
    halDelayMs(500); //keep 0.5s

    for (j = 63; j >= 0; j--) //all LED breath falling
    {
        RGBAllLeds(PWM_Gamma64[j], PWM_Gamma64[j], PWM_Gamma64[j]);
        halDelayMs(30); //20ms
    }
    halDelayMs(500); //keep 0.5s
}

static void allLEDFadeOn(uint8_t speed) // fade speed in milliseconds
{
    for (int8_t j = 0; j <= 63; j++) // all LED breath rising
    {
        RGBAllLeds(PWM_Gamma64[j], PWM_Gamma64[j], PWM_Gamma64[j]);
        halDelayMs(speed);
    }
    halDelayMs(500); //keep 0.5s
}

static void allLEDFadeOff(uint8_t speed) // fade speed in milliseconds
{
    for (int8_t j = 63; j >= 0; j--) // all LED breath falling
    {
        RGBAllLeds(PWM_Gamma64[j], PWM_Gamma64[j], PWM_Gamma64[j]);
        halDelayMs(speed);
    }
    halDelayMs(500); //keep 0.5s
}

void testLeds(void)
{
    initRGB();

    while (1)
    {
        // Turn on leds slowly from 1 to 36 one at a time at 1/4 intensity
        for (uint8_t ii = 0x01; ii <= 12; ii++)
        {
            RGB(ii, 255, 0, 0);
            halDelayMs(100);
            RGB(ii, 255, 255, 0);
            halDelayMs(100);
            RGB(ii, 255, 255, 255);
            halDelayMs(100);
        }
        halDelayMs(1000);
        printf("End of Test 1!\r\n");

        // Turn off leds slowly from 1 to 36 one at a time
        for (uint8_t ii = 0x01; ii <= 12; ii++)
        {
            RGB(ii, 255, 255, 0);
            halDelayMs(100);
            RGB(ii, 255, 0, 0);
            halDelayMs(100);
            RGB(ii, 0, 0, 0);
            halDelayMs(100);
        }
        halDelayMs(1000);
        printf("End of Test 2!\r\n");

        // Turn on leds quickly from 1 to 36 one at a time at 1/2 intensity
        for (uint8_t ii = 0x01; ii <= 12; ii++)
        {
            RGB(ii, 128, 0, 0);
            halDelayMs(50);
            RGB(ii, 128, 128, 0);
            halDelayMs(50);
            RGB(ii, 128, 128, 128);
            halDelayMs(50);
        }
        halDelayMs(1000);

        // Turn off leds quickly from 1 to 36 one at a time
        for (uint8_t ii = 0x01; ii <= 12; ii++)
        {
            RGB(ii, 128, 128, 0);
            halDelayMs(50);
            RGB(ii, 128, 0, 0);
            halDelayMs(50);
            RGB(ii, 0, 0, 0);
            halDelayMs(50);
        }
        halDelayMs(1000);
        printf("End of Test 3!\r\n");

        // Turn on leds very quickly from 1 to 36 one at a time at full intensity
        for (uint8_t ii = 0x01; ii <= 12; ii++)
        {
            RGB(ii, 255, 0, 0);
            halDelayMs(25);
            RGB(ii, 255, 255, 0);
            halDelayMs(25);
            RGB(ii, 255, 255, 255);
            halDelayMs(25);
        }
        halDelayMs(1000);

        // Turn off leds very quickly from 1 to 36 one at a time
        for (uint8_t ii = 0x01; ii <= 12; ii++)
        {
            RGB(ii, 255, 255, 0);
            halDelayMs(50);
            RGB(ii, 255, 0, 0);
            halDelayMs(50);
            RGB(ii, 0, 0, 0);
            halDelayMs(50);
        }
        halDelayMs(1000);
        printf("End of Test 4!\r\n");

        // Breath all leds slowly
        allLEDFadeOn(100);
        allLEDFadeOff(100);
        // then faster
        allLEDFadeOn(50);
        allLEDFadeOff(50);
        //then faster still
        allLEDFadeOn(10);
        allLEDFadeOff(10);
        //then smooth blink
        allLEDFadeOn(5);
        allLEDFadeOff(5);
        allLEDFadeOn(5);
        allLEDFadeOff(5);
        allLEDFadeOn(5);
        allLEDFadeOff(5);
        halDelayMs(1000);
        printf("End of Test 5!\r\n");

        // Fade on and fade off leds 1 to 36 one at a time
        for (uint8_t ii = 0x01; ii <= 12; ii++)
        {
            for (int8_t j = 0; j <= 63; j++)
            { // one LED breath rising
                RGB(ii, PWM_Gamma64[j], PWM_Gamma64[j], PWM_Gamma64[j]);
                halDelayMs(1);
            }
            for (int8_t j = 63; j >= 0; j--)
            { // all LED breath falling
                RGB(ii, PWM_Gamma64[j], PWM_Gamma64[j], PWM_Gamma64[j]);
                halDelayMs(3);
            }
        }
        halDelayMs(1000);
        printf("End of Test 6!\r\note text");

        mode1(); // ISSI demo
        halDelayMs(1000);
        printf("End of Test 7!\r\n");
    }
}

/**
 * @brief 
 * 
 */
void testLeds2(void)
{
    initRGB();
    uint16_t a = 0;
    while (1)
    {
        setArColor(a++);
        halDelayMs(20);

        // RGBAllLeds(a++, 255, 0);
        if (a == 500)
        {
            a = 0;
        }
        halDelayMs(100);
    }
}
/**
 * @brief 
 * 
 */

void testLeds3(void)
{
    initRGB();

    halDelayMs(10000);
    while (1)
    {

        for (int verde = 0; verde <= 255; verde++)
        {
            RGBAllLeds(255, verde, 0);
            halDelayMs(1);
        }
        //Se mantiene el verde al maximo mientras baja el rojo
        for (int rojo = 255; rojo >= 0; rojo--)
        {
            RGBAllLeds(rojo, 255, 0);
            halDelayMs(1);
        }

        //Se mantiene el verde al maximo mientras sube el azul
        for (int azul = 0; azul <= 255; azul++)
        {
            RGBAllLeds(0, 255, azul);
            halDelayMs(1);
        }
        //Se mantiene el azul al maximo mientras baja el verde
        for (int verde = 255; verde >= 0; verde--)
        {
            RGBAllLeds(0, verde, 255);
            halDelayMs(1);
        }
    }
}

char StringData[8] = {0};

char * getStringData(void)
{
    return StringData;
}
/**
 * @brief 
 * 
 */
void testPrintFloat(void)
{
    float a = 3.14;
    while (1)
    {
        printf("%.2f\r\n", a);
        halDelayMs(10);
    }
}

Adafruit_LC709203F lc;

/**
 * @brief 
 * 
 */
void testLEC709(void)
{
    bool status = true;
    if (status == true)
        status = !status;
    if (!lc.begin())
    {
        printf("Couldnt find Adafruit LC709203F?\r\nMake sure a battery is plugged in!\r\n");
        while (1)
            halDelayMs(10);
    }
    printf("Found LC709203F\r\n");
    printf("Version: 0x%02X\r\n", lc.getICversion());

    status = lc.setThermistorB(3950);
    // printf("status %d\r\n", status);
    printf("Thermistor B = %d\r\n", lc.getThermistorB());

    status = lc.setPackSize(LC709203F_APA_100MAH);
    // printf("status %d\r\n", status);
    status = lc.setAlarmVoltage(3.8);
    // printf("status %d\r\n", status);

    while (1)
    {
        printf("Batt Voltage: %.3f\r\n", lc.cellVoltage());
        printf("Batt Percent: %.1f\r\n", lc.cellPercent());
        printf("Batt Temp: %.1f\r\n", lc.getCellTemperature());

        halDelayMs(2000); // dont query too often!
    }
}

/**
 * @brief 
 * 
 */
void test_twi(void)
{
    uint8_t command[2] = {0x28, 0x0E};
    bool detected_device = false;
    int err_code = 0;
    for (uint8_t i = 0X59; i < 0X60; i++)
    {
        err_code = hal_i2c_write(i, command, sizeof(command));
        if (err_code == NRF_SUCCESS)
        {
            detected_device = true;
            printf("TWI device detected at address 0x%x.\r\n", i);
        }
    }
    if (!detected_device)
    {
        printf("No device was found. \r\n");
    }
    // nrf_delay_ms(500);
}

uint8_t readValue[3] = {0x00};
uint8_t readyToSend = 0;

uint8_t getReadyToSend(void)
{
    return readyToSend;
}
SGP40 mySensor; //create an object of the SGP40 class

/**
 * @brief 
 * 
 */
void testSGP40(void)
{
    //
    //Initialize sensor
    if (mySensor.begin() == false)
    {
        printf("SGP40 not detected. Check connections. Freezing...\r\n");
        while (1)
            ; // Do nothing more
    }
    halDelayMs(10);
    initRGB();
    while (1)
    {
        readyToSend = 0;
        int32_t value = mySensor.getVOCindex();
        memset(StringData, 0, sizeof(StringData));
        sprintf(StringData, "%ld\r\n", value);
        sendBluetoothData((uint8_t *)StringData, strlen(StringData));
        printf("%ld\r\n", value); //Get the VOC Index using the default RH (50%) and T (25C)
        readyToSend = 1;
        halDelayMs(10);
        if (_valueBluetooth == '0')
        {
            setArColor((uint16_t)value);
            pulseRountine(value);
        }
        else
        {
            if (value >= 150)
            {
                setArColor((uint16_t)value);
                pulseRountine(value);
            }
            else
            {
                RGBAllLeds(0, 0, 0);
            }
        }

        halDelayMs(10); //Wait 5 seconds
    }
}

void testLibrary(void)
{
    while (1)
    {
        printf("%ld\r\n", F16(0.5));
        halDelayMs(10);
    }
}

void testPWM(void)
{
    while (1)
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
}

/**
 * @brief 
 * 
 */
void test_handler(void)
{
    // testLeds();
    // testLeds3();
    testLeds2();
    // testPrintFloat();
    // test_twi();
    // testLEC709();
    // testSGP40();
    // testLibrary();
    // testPWM();
}