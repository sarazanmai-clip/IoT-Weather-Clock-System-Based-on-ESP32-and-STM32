#ifndef __ESP_AT_H
#define __ESP_AT_H
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

bool ESP_AT_Init(void);

bool ESP_AT_SendCommand(char cmd[],char rsp[],uint16_t *length,uint32_t timeout);
bool ESP_AT_SendData(uint8_t data[],uint16_t length);

bool ESP_AT_Reset(void);

bool ESP_AT_Wifi_Init(void);

bool ESP_AT_Wifi_Connect(const char *ssid,const char *pwd);

bool ESP_AT_HTTP_Get(const char *url,char rsp[],uint16_t *length);

void ESP_AT_Weather_Get(char *url, char rspWether[], char rspTemperature[]);

bool ESP_AT_Sntp_Init(void);

char* ESP_AT_Sntp_Get(void);

void ESP_AT_Time_Get(char rspDate[],char rspTime[]);

char* ESP_AT_TimeStamp_Get(void);

#endif
