/*
  This is a library written for the SPG40
  By Paul Clark @ SparkFun Electronics, December 5th, 2020


  https://github.com/sparkfun/SparkFun_SGP40_Arduino_Library

  Development environment specifics:
  Arduino IDE 1.8.13

  SparkFun labored with love to create this code. Feel like supporting open
  source hardware? Buy a board from SparkFun!
  https://www.sparkfun.com/products/17729


  VOC Index Algorithm provided by Sensirion:
  https://github.com/Sensirion/embedded-sgp


  CRC lookup table from Bastian Molkenthin  http://www.sunshine2k.de/coding/javascript/crc/crc_js.html

  Copyright (c) 2015 Bastian Molkenthin

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include "SGP40.h"
#include "Nordic_HAL.h"

//SGP40 I2C Commands
uint8_t sgp40_measure_raw[2] = {0x26, 0x0F};
uint8_t sgp40_measure_test[2] = {0x28, 0x0E};
uint8_t sgp40_heater_off[2] = {0x36, 0x15};
uint8_t sgp40_soft_reset[2] = {0x00, 0x06};
//That's all folks!

//SGP40 Measure Test Results
uint16_t sgp40_measure_test_pass = 0xD400;
uint16_t sgp40_measure_test_fail = 0x4B00;

//Constructor
SGP40::SGP40()
{
}

//Start I2C communication using specified port
//Returns true if successful or false if no sensor detected
bool SGP40::begin(void)
{

  SGP40ERR result = measureTest();
  return (result == SGP40_SUCCESS);
}

//Calling this function with nothing sets the debug port to Serial
//You can also call it with other streams like Serial1, SerialUSB, etc.
void SGP40::enableDebugging(void)
{
  _printDebug = true;
}

//Sensor runs on chip self test
//Returns SUCCESS (0) if successful or other error code if unsuccessful
SGP40ERR SGP40::measureTest(void)
{
  ret_code_t i2cResult;
  uint8_t readValues[3] = {0};
  i2cResult = hal_i2c_write(_SGP40Address, sgp40_measure_test, 2);

  if (i2cResult != 0)
  {
    if (_printDebug == true)
    {
      printf("measureTest: endTransmission returned: %ld\r\n", i2cResult);
    }
    return SGP40_ERR_I2C_ERROR;
  }

  //Hang out while measurement is taken. datasheet says 250ms
  halDelayMs(250);

  //Comes back in 3 bytes, data(MSB) / data(LSB) / Checksum
  i2cResult = hal_i2c_read(_SGP40Address, readValues, 3);

  if (i2cResult != 0)
  {
    if (_printDebug == true)
    {
      printf("measureTest: requestFrom returned: %ld\r\n", i2cResult);
    }
    return SGP40_ERR_I2C_ERROR; //Error out
  }

  uint16_t results = readValues[0] << 8; //store MSB in results
  results |= readValues[1];              //store LSB in results

  uint8_t checkSum = readValues[2]; //verify checksum

  if (checkSum != _CRC8(results))
  {
    if (_printDebug == true)
    {
      printf("measureTest: checksum failed! Expected: 0x%02X Received: 0x%02X\r\n",
             _CRC8(results), checkSum);
    }
    return SGP40_ERR_BAD_CRC; //checksum failed
  }

  if (results != sgp40_measure_test_pass)
  {
    if (_printDebug == true)
    {
      printf("measureTest: unexpected test results! Received: 0x%02X\r\n", results);
    }
    return SGP40_MEASURE_TEST_FAIL; //self test results incorrect
  }

  return SGP40_SUCCESS;
}

//Perform a soft reset
//Returns SUCCESS (0) if successful
SGP40ERR SGP40::softReset(void)
{
  ret_code_t i2cResult = 0;
  i2cResult = hal_i2c_write(_SGP40Address, (uint8_t *)sgp40_soft_reset, 2);

  if (i2cResult != 0)
  {
    if (_printDebug == true)
    {
      printf("softReset: endTransmission returned: %ld\r\n", i2cResult);
    }
    return SGP40_ERR_I2C_ERROR;
  }

  return SGP40_SUCCESS;
}

//Turn the heater off
//Returns SUCCESS (0) if successful
SGP40ERR SGP40::heaterOff(void)
{
  ret_code_t i2cResult = 0;
  i2cResult = hal_i2c_write(_SGP40Address, (uint8_t *)sgp40_heater_off, 2);

  halDelayMs(1); // From datasheet

  if (i2cResult != 0)
  {
    if (_printDebug == true)
    {
      printf("heaterOff: endTransmission returned: %ld\r\n", i2cResult);
    }
    return SGP40_ERR_I2C_ERROR;
  }

  return SGP40_SUCCESS;
}

//Measure Raw Signal
//Returns SUCCESS (0) if successful
//The raw signal is returned in SRAW_ticks
//The user can provide Relative Humidity and Temperature parameters if desired:
//RH_ticks and T_ticks are 16 bit
//RH_ticks = %RH * 65535 / 100
//T_ticks = (DegC + 45) * 65535 / 175
//%RH: Minimum=0 (RH_ticks = 0x0000)  Maximum=100 (RH_ticks = 0xFFFF)
//DegC: Minimum=-45 (T_ticks = 0x0000)  Minimum=130 (T_ticks = 0xFFFF)
//Default %RH = 50 (RH_ticks = 0x8000)
//Default DegC = 25 (T_ticks = 0x6666)
//See the SGP40 datasheet for more details
SGP40ERR SGP40::measureRaw(uint16_t *SRAW_ticks, float RH, float T)
{
  ret_code_t i2cResult = 0;
  uint8_t readValues[3] = {0};
  uint16_t RH_ticks, T_ticks;

  // Check RH and T are within bounds (probably redundant, but may prevent unexpected wrap-around errors)
  if (RH < 0)
  {
    if (_printDebug == true)
      printf("measureRaw: RH too low! Correcting...");
    RH = 0;
  }
  if (RH > 100)
  {
    if (_printDebug == true)
      printf("measureRaw: RH too high! Correcting...");
    RH = 100;
  }
  if (T < -45)
  {
    if (_printDebug == true)
      printf("measureRaw: T too low! Correcting...");
    T = -45;
  }
  if (T > 130)
  {
    if (_printDebug == true)
      printf("measureRaw: T too high! Correcting...");
    T = 130;
  }

  RH_ticks = (uint16_t)(RH * 65535 / 100);      // Convert RH from %RH to ticks
  T_ticks = (uint16_t)((T + 45) * 65535 / 175); // Convert T from DegC to ticks
  if (_printDebug == true)
  {
    printf("measureRaw: RH_ticks: 0x%02X T_ticks: 0x%02X\r\n", RH_ticks, T_ticks);
  }
  uint8_t dataToSend[8] = {0};
  dataToSend[0] = sgp40_measure_raw[0];
  dataToSend[1] = sgp40_measure_raw[1];
  dataToSend[2] = (uint8_t)(RH_ticks >> 8);
  dataToSend[3] = (uint8_t)(RH_ticks & 0xFF);
  dataToSend[4] = _CRC8(RH_ticks);
  dataToSend[5] = (uint8_t)(T_ticks >> 8);
  dataToSend[6] = (uint8_t)(T_ticks & 0xFF);
  dataToSend[7] = _CRC8(T_ticks);

  i2cResult = hal_i2c_write(_SGP40Address, dataToSend, 8);

  if (i2cResult != 0)
  {
    if (_printDebug == true)
    {
      printf("measureRaw: endTransmission returned: %ld\r\n", i2cResult);
    }
    return SGP40_ERR_I2C_ERROR;
  }

  //Hang out while measurement is taken. datasheet says 30ms
  halDelayMs(30);

  //Comes back in 3 bytes, data(MSB) / data(LSB) / Checksum
  i2cResult = hal_i2c_read(_SGP40Address, readValues, 3);

  if (i2cResult != 0)
  {
    if (_printDebug == true)
    {
      printf("measureRaw: requestFrom returned: %ld\r\n", i2cResult);
    }
    return SGP40_ERR_I2C_ERROR; //Error out
  }

  uint16_t results = readValues[0] << 8; //store MSB in results
  results |= readValues[1];              //store LSB in results

  uint8_t checkSum = readValues[2]; //verify checksum

  if (checkSum != _CRC8(results))
  {
    if (_printDebug == true)
    {
      printf("measureRaw: checksum failed! Expected: 0x%02X Received: 0x%02X\r\n",
             _CRC8(results), checkSum);
    }
    return SGP40_ERR_BAD_CRC; //checksum failed
  }

  *SRAW_ticks = results; //Pass the results

  return SGP40_SUCCESS;
}

// #define SIMULATE_RAW_DATA
//Get VOC Index
//Returns -100 if an error occurs
//The user can provide Relative Humidity and Temperature parameters if desired
int32_t SGP40::getVOCindex(float RH, float T)
{
  int32_t vocIndex = 0;

#ifndef SIMULATE_RAW_DATA
  uint16_t SRAW_ticks = 0;
  SGP40ERR result = measureRaw(&SRAW_ticks, RH, T);
#else
  static uint16_t SRAW_ticks = 47000;
  SGP40ERR result = SGP40_SUCCESS;
  SRAW_ticks++;
#endif
  printf("%d,", SRAW_ticks);
  if (result != SGP40_SUCCESS)
  {
    if (_printDebug == true)
    {
      printf("getVOCindex: measureRaw returned error: %d\r\n", result);
    }
    return -100; //fail...
  }

  VocAlgorithm_process(&vocAlgorithmParameters, SRAW_ticks, &vocIndex);

  return vocIndex;
}

#ifndef SGP40_LOOKUP_TABLE
//Given an array and a number of bytes, this calculate CRC8 for those bytes
//CRC is only calc'd on the data portion (two bytes) of the four bytes being sent
//From: http://www.sunshine2k.de/articles/coding/crc/understanding_crc.html
//Tested with: http://www.sunshine2k.de/coding/javascript/crc/crc_js.html
//x^8+x^5+x^4+1 = 0x31
uint8_t SGP40::_CRC8(uint16_t data)
{
  uint8_t crc = 0xFF; //Init with 0xFF

  crc ^= (data >> 8); // XOR-in the first input byte

  for (uint8_t i = 0; i < 8; i++)
  {
    if ((crc & 0x80) != 0)
      crc = (uint8_t)((crc << 1) ^ 0x31);
    else
      crc <<= 1;
  }

  crc ^= (uint8_t)data; // XOR-in the last input byte

  for (uint8_t i = 0; i < 8; i++)
  {
    if ((crc & 0x80) != 0)
      crc = (uint8_t)((crc << 1) ^ 0x31);
    else
      crc <<= 1;
  }

  return crc; //No output reflection
}
#else
//Generates CRC8 for SGP40 from lookup table
uint8_t SGP40::_CRC8(uint16_t data)
{
  uint8_t CRC = 0xFF;                          //inital value
  CRC ^= (uint8_t)(data >> 8);                 //start with MSB
  CRC = _CRC8LookupTable[CRC >> 4][CRC & 0xF]; //look up table [MSnibble][LSnibble]
  CRC ^= (uint8_t)data;                        //use LSB
  CRC = _CRC8LookupTable[CRC >> 4][CRC & 0xF]; //look up table [MSnibble][LSnibble]
  return CRC;
}
#endif
