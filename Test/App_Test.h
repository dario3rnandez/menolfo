#ifndef AAP_TEST_H
#define AAP_TEST_H

#include "stdint.h"
void sendBluetoothData(uint8_t * pDataSend, uint16_t lenSend);
void test_handler(void);
void testLeds(void);
void testLEC709(void);
void test_twi(void);
void testSGP40(void);
void testLeds3(void);
void testLeds2(void);
void setvalueBluetooth(char value);
char getvalueBluetooth(void);
char * getStringData(void);
uint8_t getReadyToSend(void);
#endif /* AAP_TEST_H */
