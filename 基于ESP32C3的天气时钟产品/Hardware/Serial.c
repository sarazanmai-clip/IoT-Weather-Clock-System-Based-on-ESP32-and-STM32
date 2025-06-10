#include "stm32f10x.h"                  // Device header

#include "OLED.h"
#include <stdio.h>
#include <stdarg.h>
#include <Serial.h>
#include <String.h>
#define BufferSize 4096

char RxData[BufferSize];

uint8_t RxFlag;
uint8_t Rxlen;
uint8_t RxReady;


void Serial_init(void)
{
	//开启时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	//GPIO初始化
	GPIO_InitTypeDef GPIO_InitTypeDefStructure; 
	
	
	
	GPIO_InitTypeDefStructure.GPIO_Mode=GPIO_Mode_AF_PP;  //PA9=TX,PA10=RX
	GPIO_InitTypeDefStructure.GPIO_Pin=GPIO_Pin_9;
	GPIO_InitTypeDefStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitTypeDefStructure);
	
	GPIO_InitTypeDefStructure.GPIO_Mode=GPIO_Mode_IPU;  //PA9=TX,PA10=RX
	GPIO_InitTypeDefStructure.GPIO_Pin=GPIO_Pin_10;
	GPIO_InitTypeDefStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitTypeDefStructure);
	
	//USART初始化
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate=115200;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode=USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity=USART_Parity_No;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStructure);
	
	
	//中断配置
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
//	USART_ITConfig(USART1, USART_IT_ORE, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel=USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=1;
	NVIC_Init(&NVIC_InitStruct);
	
	//使能
	USART_Cmd(USART1,ENABLE);

}

void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1, Byte);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Array[i]);
	}
}

void Serial_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		Serial_SendByte(String[i]);
	}
}

uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y --)
	{
		Result *= X;
	}
	return Result;
}

void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
	}
}

//要在接收数据前必须先设置RxReady为开
void Serial_SetRxReadyOn(void)
{
	RxReady=1;
}
//接受完数据并进行数据处理时要将RxReady设置为关
void Serial_SetRxReadyOff(void)
{
	RxReady=0;
}

//检查数据是否被接收，若接收则获取接收的数据
uint8_t Serial_GetRxData(char RSP[],uint16_t *length)
{
	if(RxFlag==1)
	{
		Serial_SetRxReadyOff();
		strcpy(RSP,RxData);
		*length=Rxlen;
		Rxlen=0;
		RxFlag=0;
		return 1;
	}
	return 0;
}


void USART1_IRQHandler(void)
{
									
	
	//中断标志位RXNE
	if(USART_GetITStatus(USART1,USART_IT_RXNE)==SET)
	{	
		uint8_t data=USART_ReceiveData(USART1);
		
		//清除标志位
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		
		//检查RxReady位，若位0则直接return
		if(RxReady==0) return;
		
		//若收到的数据为‘\n’且之前三个字符为‘K’,'O','\r',则视为字符串结束标志，并将RxFLAG置为1
		if (
			((char)data == '\n' && RxData[Rxlen-1]=='\r') 
			&&
			(
				(RxData[Rxlen-2] == 'K' && RxData[Rxlen-3]=='O')
				||
				(RxData[Rxlen-2] && RxData[Rxlen-1]=='O'&& RxData[Rxlen-2]=='R'
								&& RxData[Rxlen-3]=='R'&& RxData[Rxlen-4]=='E')
			)
		   )

		{
			//给RxData字符串置结束标志‘\0’
			RxData[Rxlen++] = data;
			RxData[Rxlen] = '\0';
			RxFlag = 1;
		}
		else
		{
			RxData[Rxlen] = data;
			Rxlen ++;
		}
		
	}
}


