/*!
 *  @file Adafruit_LC709203F.cpp
 *
 *  @mainpage Adafruit LC709203F Battery Monitor library
 *
 *  @section intro_sec Introduction
 *
 * 	I2C Driver for the Adafruit LC709203F Battery Monitor library
 *
 * 	This is a library for the Adafruit LC709203F breakout:
 * 	https://www.adafruit.com/product/4712
 *
 * 	Adafruit invests time and resources providing this open source code,
 *  please support Adafruit and open-source hardware by purchasing products from
 * 	Adafruit!
 *
 *  @section dependencies Dependencies
 *  This library depends on the Adafruit BusIO library
 *
 *  @section author Author
 *
 *  Limor Fried (Adafruit Industries)
 *
 * 	@section license License
 *
 * 	BSD (see license.txt)
 *
 * 	@section  HISTORY
 *
 *     v1.0 - First release
 */

#include "LC709203F.h"
#include "Nordic_HAL.h"
#include "string.h"
/*!
 *    @brief  Instantiates a new LC709203F class
 */
Adafruit_LC709203F::Adafruit_LC709203F(void) {}

Adafruit_LC709203F::~Adafruit_LC709203F(void) {}

/*!
 *    @brief  Sets up the hardware and initializes I2C
 *    @param  wire
 *            The Wire object to be used for I2C connections.
 *    @return True if initialization was successful, otherwise false.
 */
bool Adafruit_LC709203F::begin(void)
{

  if (!setPowerMode(LC709203F_POWER_OPERATE))
  {
    printf("1 fail\r\n");
    return false;
  }


  if (!setPackSize(LC709203F_APA_500MAH))
  {
    printf("2 fail\r\n");
    return false;
  }


  
  uint16_t param ;
  bool status = readWord(LC709203F_CMD_PARAMETER, &param);
  printf("status %d\r\n", status);
  printf("Profile Param: %02X\r\n", param);

  // use 4.2V profile
  if (!setBattProfile(0x1))
  {
    printf("3 fail\r\n");
    return false;
  }
    

  if (!setTemperatureMode(LC709203F_TEMPERATURE_THERMISTOR))
  {
    printf("4 fail\r\n");
    return false;
  }
    

  return true;
}

/*!
 *    @brief  Get IC LSI version
 *    @return 16-bit value read from LC709203F_CMD_ICVERSION register
 */
uint16_t Adafruit_LC709203F::getICversion(void)
{
  uint16_t vers = 0;
  readWord(LC709203F_CMD_ICVERSION, &vers);
  return vers;
}

/*!
 *    @brief  Initialize the RSOC algorithm
 *    @return True on I2C command success
 */
bool Adafruit_LC709203F::initRSOC(void)
{
  return writeWord(LC709203F_CMD_INITRSOC, 0xAA55);
}

/*!
 *    @brief  Get battery voltage
 *    @return Floating point value read in Volts
 */
float Adafruit_LC709203F::cellVoltage(void)
{
  uint16_t voltage = 0;
  readWord(LC709203F_CMD_CELLVOLTAGE, &voltage);
  return voltage / 1000.0;
}

/*!
 *    @brief  Get battery state in percent (0-100%)
 *    @return Floating point value from 0 to 100.0
 */
float Adafruit_LC709203F::cellPercent(void)
{
  uint16_t percent = 0;
  readWord(LC709203F_CMD_CELLITE, &percent);
  return percent / 10.0;
}

/*!
 *    @brief  Get battery thermistor temperature
 *    @return Floating point value from -20 to 60 *C
 */
float Adafruit_LC709203F::getCellTemperature(void)
{
  uint16_t temp = 0;
  readWord(LC709203F_CMD_CELLTEMPERATURE, &temp);
  float tempf = map(temp, 0x9E4, 0xD04, -200, 600);
  return tempf / 10.0;
}

/*!
 *    @brief  Set the temperature mode (external or internal)
 *    @param t The desired mode: LC709203F_TEMPERATURE_I2C or
 * LC709203F_TEMPERATURE_THERMISTOR
 *    @return True on successful I2C write
 */
bool Adafruit_LC709203F::setTemperatureMode(lc709203_tempmode_t t)
{
  return writeWord(LC709203F_CMD_STATUSBIT, (uint16_t)t);
}

/*!
 *    @brief  Set the approximate pack size, helps RSOC calculation
 *    @param apa The lc709203_adjustment_t enumerated approximate cell size
 *    @return True on successful I2C write
 */
bool Adafruit_LC709203F::setPackSize(lc709203_adjustment_t apa)
{
  return writeWord(LC709203F_CMD_APA, (uint16_t)apa);
}

/*!
 *    @brief  Set the alarm pin to respond to an RSOC percentage level
 *    @param percent The threshold value, set to 0 to disable alarm
 *    @return True on successful I2C write
 */
bool Adafruit_LC709203F::setAlarmRSOC(uint8_t percent)
{
  return writeWord(LC709203F_CMD_ALARMRSOC, percent);
}

/*!
 *    @brief  Set the alarm pin to respond to a battery voltage level
 *    @param voltage The threshold value, set to 0 to disable alarm
 *    @return True on successful I2C write
 */
bool Adafruit_LC709203F::setAlarmVoltage(float voltage)
{
  return writeWord(LC709203F_CMD_ALARMVOLT, voltage * 1000);
}

/*!
 *    @brief  Set the power mode, LC709203F_POWER_OPERATE or
 * LC709203F_POWER_SLEEP
 *    @param t The power mode desired
 *    @return True on successful I2C write
 */
bool Adafruit_LC709203F::setPowerMode(lc709203_powermode_t t)
{
  return writeWord(LC709203F_CMD_POWERMODE, (uint16_t)t);
}

/*!
 *    @brief  Get the thermistor B value (e.g. 3950)
 *    @return The uint16_t B value
 */
uint16_t Adafruit_LC709203F::getThermistorB(void)
{
  uint16_t val = 0;
  readWord(LC709203F_CMD_THERMISTORB, &val);
  return val;
}

/*!
 *    @brief  Set the thermistor B value (e.g. 3950)
 *    @param b The value to set it to
 *    @return True on successful I2C write
 */
bool Adafruit_LC709203F::setThermistorB(uint16_t b)
{
  return writeWord(LC709203F_CMD_THERMISTORB, b);
}

/*!
 *    @brief  Get the battery profile parameter
 *    @return The uint16_t profile value (0 or 1)
 */
uint16_t Adafruit_LC709203F::getBattProfile(void)
{
  uint16_t val = 0;
  readWord(LC709203F_CMD_BATTPROF, &val);
  return val;
}

/*!
 *    @brief  Set the battery profile parameter
 *    @param b The value to set it to (0 or 1)
 *    @return True on successful I2C write
 */
bool Adafruit_LC709203F::setBattProfile(uint16_t b)
{
  return writeWord(LC709203F_CMD_BATTPROF, b);
}

/*!
 *    @brief  Helper that reads 16 bits of CRC data from the chip. Note
 *            this function performs a CRC on data that includes the I2C
 *            write address, command, read address and 2 bytes of response
 *    @param command The I2C register/command
 *    @param data Pointer to uint16_t value we will store response
 *    @return True on successful I2C read
 */
bool Adafruit_LC709203F::readWord(uint8_t command, uint16_t *data)
{

  ret_code_t err_code = 0;
  uint8_t reply[6];
  reply[0] = LC709203F_I2CADDR_DEFAULT * 2; // write byte
  reply[1] = command;                       // command / register
  reply[2] = reply[0] | 0x1;                // read byte

  err_code = hal_i2c_write(LC709203F_I2CADDR_DEFAULT, &command, 1);

  if (err_code != NRF_SUCCESS)
  {
    return false;
  }

  err_code = hal_i2c_read(LC709203F_I2CADDR_DEFAULT, reply + 3, 3);
  printf("err_code %ld\r\n", err_code);

  for(uint8_t i = 0; i < 6; i ++)
  {
    printf("%02X ", reply[i]);
  }
  printf("\r\n");
  uint8_t crc = crc8(reply, 5);
  // CRC failure?
  if (crc != reply[5])
    return false;

  *data = reply[4];
  *data <<= 8;
  *data |= reply[3];

  return true;
}

/*!
 *    @brief  Helper that writes 16 bits of CRC data to the chip. Note
 *            this function performs a CRC on data that includes the I2C
 *            write address, command, and 2 bytes of response
 *    @param command The I2C register/command
 *    @param data Pointer to uint16_t value we will write to register
 *    @return True on successful I2C write
 */
bool Adafruit_LC709203F::writeWord(uint8_t command, uint16_t data)
{
  bool status = false;
  ret_code_t err_code = 0;
  uint8_t send[4];
  send[0] = command; // command / register
  send[1] = data & 0xFF;
  send[2] = data >> 8;
  send[3] = crc8(send, 4);

  err_code = hal_i2c_write(LC709203F_I2CADDR_DEFAULT, send, sizeof(send));

  if (err_code == NRF_SUCCESS)
  {
    status = true;
  }
  else
  {
    status = false;
  }
  return status;
}

/**
 * Performs a CRC8 calculation on the supplied values.
 *
 * @param data  Pointer to the data to use when calculating the CRC8.
 * @param len   The number of bytes in 'data'.
 *
 * @return The computed CRC8 value.
 */
uint8_t crc8(uint8_t *data, int len)
{
  uint8_t copyData[64] = {0};
  memcpy(copyData, data, len);
  const uint8_t POLYNOMIAL = 0x07;
  uint8_t crc = 0x00, k = 0;

  for (int j = len; j; --j)
  {
    crc ^= copyData[k++];

    for (int i = 8; i; --i)
    {
      crc = (crc & 0x80) ? (crc << 1) ^ POLYNOMIAL : (crc << 1);
    }
  }

  if (POLYNOMIAL == 0 || crc == 0)
  {
  }
  return crc;
}
