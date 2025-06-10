#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdio.h>
#include <stdarg.h>
#include <Serial.h>
#include <String.h>
typedef void (* Serial_Receive_Callback_t)(uint8_t data);

void Serial_init(void);
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *Array, uint16_t Length);
void Serial_SendString(char *String);
void Serial_SendNumber(uint32_t Number, uint8_t Length);

void Serial_SetRxReadyOn(void);
void Serial_SetRxReadyOff(void);
uint8_t Serial_GetRxData(char RSP[],uint16_t *length);


#endif
