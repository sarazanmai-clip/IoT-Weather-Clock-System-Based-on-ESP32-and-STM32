# [ESP32-STM32 物联网天气时钟系统](https://github.com/sarazanmai-clip/IoT-Weather-Clock-System-Based-on-ESP32-and-STM32.git)

> 硬件设计 | UART/I2C通信

## 项目概述
通过STM32主控协调ESP32获取网络API天气数据，驱动OLED实时显示天气/温度/日期/时间。
## 硬件准备
**主微控制器**：STM32F103C8T6  
**连接微控制器**：ESP32-C3开发板  
**OLED显示屏**：SSD1306（128x64分辨率）(IIC接口)  
**ST-Link v2**： 用于STM32调试和编程  
**一根USB线(连接ESP32与电脑接口)、杜邦线若干**

  
**硬件架构**：`STM32F103C8T6`(主控) ↔ `UART` ↔ `ESP32-C3开发板`(WiFi) ↔ `I2C` ↔ `OLED` ↔ `IWDG独立看门狗`

## 硬件连接
| 设备                     | 引脚连接           |
|--------------------------|-------------------|
| STM32 USART1_TX(GPIOA9)  | ESP32 RX (GPIO5)  |  
| STM32 USART1_RX(GPIOA10) | ESP32 TX (GPIO4)  |  
| STM32 I2C1_SCL(GPIOB8)   | OLED SCL          |  
| STM32 I2C1_SDA(GPIOB7)   | OLED SDA          |  

## 使用软件
**STM32**：使用keil和标准库作为模板  
**ESP32C3**：根据./ESP32-C3-MINI-1-AT-V3.3.0.0/[ESP32C3-AT][v3.3.0.0]用户指南.pdf文件中的1.3.2部分进行固件的烧录：  
开始烧录之前，请下载 Flash 下载工具(这里已经下载好，在./flash_download_tool文件夹里)  
• 打开 Flash 下载工具(flash_download_tool_3.9.8_5.exe)；  
• 选择芯片类型；（此处，我们选择 ESP32-C3）  
• 根据您的需求选择一种工作模式；（此处，我们选择 develop)  
• 根据您的需求选择一种下载接口；（此处，我们选择 uart)  
• 将 AT 固件烧录至设备：  
– 直接下载打包好的量产固件至 0x0 地址：勾选“DoNotChgBin”，使用量产固件的默认配置； 
![c260318cb4777f063c1e422f0ccfa31](https://github.com/user-attachments/assets/a741e90f-c9da-4c61-a6a5-e71701fd2cbb)  
![070df23edcbe194897937478f61d6dd](https://github.com/user-attachments/assets/443402a8-4640-455b-bfa3-20e5059fb2a7)


## 效果展示
  ![a4a4034cd46b058b0b45aa59cba809f](https://github.com/user-attachments/assets/98f36b0a-c386-4646-9be5-a195b08e90f4)  
  ![1394fdb735fa5fbc528bb7e1b94c201](https://github.com/user-attachments/assets/8f64fc7a-5d6c-454e-abef-18d24e1a3f33)  

## 快速上手
将项目克隆到本地,烧录好ESP32-C3并且将硬件连接后，打开./基于 ESP32 和 STM32 的物联网天气时钟系统/基于ESP32C3的天气时钟产品/Project.uvprojx，在main.c文件夹的Wifi_id和Wifi_pw输入你的wifi账号密码，然后点击左上角的运行按钮


