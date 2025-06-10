#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"
#include "ESP_AT.h"
#include "Timer.h"
#define BufferSize 4096

/*请输入你的wifi的id和密码*/
#define Wifi_id "Please_Input_Your_Wifi_id"
#define Wifi_pw "Please_Input_Your_Wifi_password"

/*url为网页API链接，此时location为上海，可以将location改为自己想要的*/
#define url "https://api.seniverse.com/v3/weather/now.json?key=Sw6MwWKTsVnrE38jX&location=shanghai&language=en&unit=c"
#define key "Sw6MwWKTsVnrE38jX"


char ReceiveData[BufferSize];
uint16_t ReceiveDatalength;

char ReceiveTime[32];					//接收时间数据
char ReceiveDate[32];					//接收日期数据
char ReceiveWeather[32];				//接收天气数据
char ReceiveTemperature[32];			//接收气温数据

int main(void)
{
    bool res=false;
	
	OLED_Init();		//OLED初始化
	Serial_init();
	res=ESP_AT_Init();	
    OLED_ShowString(1,1,"Init SUCCESS");
    Delay_ms(1000);
	OLED_Clear();
	res=ESP_AT_Wifi_Init();
    OLED_ShowString(1,1,"Wifi Init SUCCESS");
	Delay_ms(1000);
    OLED_Clear();
	
	res=ESP_AT_Wifi_Connect(Wifi_id,Wifi_pw);        	//wifi连接，其中wifi的名称和密码在预变量中定义
    OLED_ShowString(1,1,"Wifi Connect SUCCESS");
	Delay_ms(1000);
    OLED_Clear();
    
	res=ESP_AT_Sntp_Init();

	ESP_AT_Weather_Get(url,ReceiveWeather,ReceiveTemperature);//获取天气数据,获取天气的http链接在主函数的预定义中声明
	OLED_ShowString(1,1,"Weather Get SUCCESS");
	Delay_ms(1000);
    OLED_Clear();
    
    Timer_Init();
	OLED_ShowString(2,1,"Temperature:");
	
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);	//独立看门狗写使能
	IWDG_SetPrescaler(IWDG_Prescaler_16);			//设置预分频为16
	IWDG_SetReload(2499);							//设置重装值为2499，独立看门狗的超时时间为1000ms
	IWDG_ReloadCounter();							//重装计数器，喂狗
	IWDG_Enable();									//独立看门狗使能
	
	
	IWDG_ReloadCounter();							//重装计数器，喂狗
	
	while (1)
	{
		IWDG_ReloadCounter();						//重装计数器，喂狗

		ESP_AT_Time_Get(ReceiveDate,ReceiveTime);
		OLED_ShowString(1,1,ReceiveWeather);
		OLED_ShowString(2,13,ReceiveTemperature);
		OLED_ShowString(3,1,ReceiveDate);
		OLED_ShowString(4,1,ReceiveTime);
		
	}
}


//每半小时进入一次中断，更新天气数据
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{	
		ESP_AT_Weather_Get(url,ReceiveWeather,ReceiveTemperature);
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}
