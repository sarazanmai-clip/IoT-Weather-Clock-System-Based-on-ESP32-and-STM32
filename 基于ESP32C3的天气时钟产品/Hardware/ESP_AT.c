#include "stm32f10x.h" // Device header
#include <stdbool.h>
#include <stdlib.h>
#include <Delay.h>
#include "Serial.h"
#include "delay.h"
#include "ESP_AT.h"
#include "OLED.h"

#define Rx_BUFFER_SIZE 4096
#define AT_RESULT_OK   0
#define AT_RESULT_ERROR   1
#define AT_RESULT_FAIL   2


//发送命令和接收数据
bool ESP_AT_SendCommand(char cmd[],char rsp[],uint16_t *length,uint32_t timeout)
{
	Serial_SetRxReadyOn();		//打开接收
	Serial_SendString(cmd);		//串口发送指令
	
	while(timeout-- && Serial_GetRxData(rsp,length)!=1)		//等待字符串接收
	{
		Delay_ms(1);
	}
	
	
	if(timeout==0) 				//如果超时，则视为结束
		return false;
	
	return true;
}

//发送数据
bool ESP_AT_SendData(uint8_t data[],uint16_t length)
{
	Serial_SendArray(data,length);
	return true;
}

//ESP重置
bool ESP_AT_Reset(void)
{
	char RxTemp[128];
	uint16_t lenTemp;
	
	//恢复出厂设置
	ESP_AT_SendCommand("AT+RESTORE\r\n",RxTemp,&lenTemp,100000);
	if(RxTemp[lenTemp-3]!='K')
		return false;
	
	//关闭回显
	ESP_AT_SendCommand("ATE0\r\n",RxTemp,&lenTemp,100000);
	if(RxTemp[lenTemp-3]!='K')
		return false;
	
	//关闭存储
	ESP_AT_SendCommand("AT+SYSSTORE=0\r\n",RxTemp,&lenTemp,100000);
	if(RxTemp[lenTemp-3]!='K')
		return false;
	
	return true;
}

//ESP初始化
bool ESP_AT_Init(void)
{
	char RxTemp[128];
	uint16_t lenTemp;
    
    Serial_init();
	
	Serial_SetRxReadyOff();
   
    ESP_AT_SendCommand("AT+RESTORE\r\n",RxTemp,&lenTemp,100000);
	if(RxTemp[lenTemp-3]!='K')
		return false;
    
	return true;
}

//ESP_wifi初始化
bool ESP_AT_Wifi_Init(void)
{
	char RxTemp[128];
	uint16_t lenTemp;
	
	
	ESP_AT_SendCommand("AT+CWMODE=1\r\n",RxTemp,&lenTemp,10000000);
	
	if(RxTemp[lenTemp-3]!='K')
		return false;
	
	return true;
}

//ESP_Wifi连接，需要wifi名称和密码
bool ESP_AT_Wifi_Connect(const char *ssid,const char *pwd)
{
	char Cmd[64];
	
	char RxTemp[Rx_BUFFER_SIZE];
	uint16_t lenTemp;
	
	sprintf(Cmd,"AT+CWJAP=\"%s\",\"%s\"\r\n", ssid , pwd);

	ESP_AT_SendCommand(Cmd,RxTemp,&lenTemp,100000000);
	
	if(RxTemp[lenTemp-3]!='K')
		return false;
	return true;
}

//ESP获取http连接,得到数据
bool ESP_AT_HTTP_Get(const char *url,char rsp[],uint16_t *length)
{
	char Cmd[128];
	
	sprintf(Cmd,"AT+HTTPCGET=\"%s\"\r\n",url);
	
	ESP_AT_SendCommand(Cmd,rsp,length,10000);
	
	if(rsp[*length-3]!='K')
		return false;
	
	return true;
}

void ESP_AT_Weather_Get(char *url,char rspWether[],char rspTemperature[])
{
	char RxTemp[Rx_BUFFER_SIZE];
	uint16_t lenTemp;
	if(ESP_AT_HTTP_Get(url,RxTemp,&lenTemp)==false) OLED_ShowString(2,1,"failed");
	
	char* text=strstr(RxTemp,"text");
	text=text+6;
	uint8_t state=0;
	uint8_t index=0;
	
	
	while(state<2)
	{
		if(*text=='\"')
			state++;
		else if(state==1)
			rspWether[index++]=*text;
		text++;
	}
	rspWether[index]='\0';
	
	state=0;
	index=0;
	
	char* temperature=strstr(RxTemp,"temperature");
	temperature+=13;
	while(state<2)
	{
		if(*temperature=='\"')
			state++;
		else if(state==1)
			rspTemperature[index++]=*temperature;
		temperature++;
	}
	rspTemperature[index]='\0';

}   

//STNP初始化，设置的是中国时区
bool ESP_AT_Sntp_Init(void)
{
	char RxTemp[128];
	uint16_t lenTemp;
	
	ESP_AT_SendCommand("AT+CIPSNTPCFG=1,8,\"cn.ntp.org.cn\",\"ntp.sjtu.edu.cn\"\r\n",RxTemp,&lenTemp,100000);
	
	
	if(RxTemp[lenTemp-3]!='K')
		return false;
	
	return true;
}

//STNP初始化获取时间
char* ESP_AT_Sntp_Get(void)
{
	char RxTemp[Rx_BUFFER_SIZE];
	uint16_t lenTemp;
	ESP_AT_SendCommand("AT+CIPSNTPTIME?\r\n",RxTemp,&lenTemp,100000);
	
	RxTemp[lenTemp-6]='\0';
	lenTemp-=6;
	char* rat=RxTemp+30;
	return rat;
}

void ESP_AT_Time_Get(char rspDate[],char rspTime[])
{
	char* Sntp=ESP_AT_Sntp_Get();
	uint16_t len=strlen(Sntp);
	char year[5];
	char month_Eng[5];
	char date[5];  //表示年月日中的日
	char day[5];  //表示星期几
	char time[20];
	uint8_t Sntp_case=0;
	uint8_t index=0;
	for(uint8_t i=0;i<len;i++)
	{
		if(Sntp_case==0)
		{
			if(Sntp[i]==' ')
			{
				if(Sntp[i-1]==' ') continue;
				day[index]='\0';
				Sntp_case=1;
				index=0;
			}
			else
			{
				day[index++]=Sntp[i];
			}
		}
		else if(Sntp_case==1)
		{
			if(Sntp[i]==' ')
			{
				if(Sntp[i-1]==' ') continue;
				month_Eng[index]='\0';
				index=0;
				Sntp_case=2;
			}
			else
			{
				month_Eng[index++]=Sntp[i];
			}
		}
		else if(Sntp_case==2)
		{
			if(Sntp[i]==' ')
			{
				if(Sntp[i-1]==' ') continue;
				date[index]='\0';
				index=0;
				Sntp_case=3;
			}
			else
			{
				date[index++]=Sntp[i];
			}
		}
		else if(Sntp_case==3)
		{
			if(Sntp[i]==' ')
			{
				if(Sntp[i-1]==' ') continue;
				time[index]='\0';
				index=0;
				Sntp_case=4;
			}
			else
			{
				time[index++]=Sntp[i];
			}
		}
		else if(Sntp_case==4)
		{
			if(i==len-1)
			{
				year[index++]=Sntp[i];
				year[index]='\0';
				index=0;
				Sntp_case=4;
			}
			else
			{
				year[index++]=Sntp[i];
			}
		}
	}
	char* month_Num;
	if(month_Eng[0]=='J' && month_Eng[1]=='a' && month_Eng[2]=='n') { month_Num="01";}
	else if(month_Eng[0]=='F' && month_Eng[1]=='e' && month_Eng[2]=='b') { month_Num="02";}
	else if(month_Eng[0]=='M' && month_Eng[1]=='a' && month_Eng[2]=='r') { month_Num="03";}
	else if(month_Eng[0]=='A' && month_Eng[1]=='p' && month_Eng[2]=='r') { month_Num="04";}
	else if(month_Eng[0]=='M' && month_Eng[1]=='a' && month_Eng[2]=='y') { month_Num="05";}
	else if(month_Eng[0]=='J' && month_Eng[1]=='u' && month_Eng[2]=='n') { month_Num="06";}
	else if(month_Eng[0]=='J' && month_Eng[1]=='u' && month_Eng[2]=='l') { month_Num="07";}
	else if(month_Eng[0]=='A' && month_Eng[1]=='u' && month_Eng[2]=='g') { month_Num="08";}
	else if(month_Eng[0]=='S' && month_Eng[1]=='e' && month_Eng[2]=='p') { month_Num="09";}
	else if(month_Eng[0]=='O' && month_Eng[1]=='c' && month_Eng[2]=='t') { month_Num="10";}
	else if(month_Eng[0]=='N' && month_Eng[1]=='u' && month_Eng[2]=='v') { month_Num="11";}
	else if(month_Eng[0]=='D' && month_Eng[1]=='e' && month_Eng[2]=='c') { month_Num="12";}
	
	
	sprintf(rspDate,"%s/%s/%s %s",year,month_Num,date,day);
	
	sprintf(rspTime,"%s",time);

}

//STNP获取时间戳
char* ESP_AT_TimeStamp_Get(void)
{
	char RxTemp[Rx_BUFFER_SIZE];
	uint16_t lenTemp;
	ESP_AT_SendCommand("AT+SYSTIMESTAMP?\r\n",RxTemp,&lenTemp,100000);
	
	RxTemp[lenTemp-6]='\0';
	lenTemp-=6;
	char* rat=RxTemp+32;
	return rat;
}
