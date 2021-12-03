#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsp_SysTick.h"
#include "bsp_esp8266.h"
#include "test.h"
#include "delay.h"
#include "led.h"
#include "mqtt.h"
#include "string.h"

char *LED_Result;
char *LED_OPEN={"open"};
char *LED_CLOSE={"close"};


/**
 * @brief  主函数
 * @param  无
 * @retval 无
 */
int main( void )
{
	/* 初始化 */
	USARTx_Config();                                        /* 初始化串口1 */
	SysTick_Init();                                         /* 配置 SysTick 为 1ms 中断一次 */
	ESP8266_Init();                                         /* 初始化WiFi模块使用的接口和外设 */
	delay_init();                                           /* 延时函数初始化 */
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_2 );       /* 设置NVIC中断分组2:2位抢占优先级，2位响应优先级 */
	LED_Init();

	printf( "\r\n正在配置 ESP8266 ......\r\n" );
	macESP8266_CH_ENABLE();//初始化io
	//printf("\n发送：AT\n");
	ESP8266_AT_Test();//发送AT等待OK
	//printf("\n设置工作模式\n");
	ESP8266_Net_Mode_Choose( STA );//设置STA模式
	//printf("\n连接wifi中...\n");
	while ( !ESP8266_JoinAP( macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd ) );//连接wifi
	//printf("\n设置单连接模式\n");
	ESP8266_Enable_MultipleId( DISABLE );//设置单连接
	//printf("\n开始连接MQTT服务器\n");
	mqtt_connect();
	printf( "\r\n配置 ESP8266 完毕\r\n" );
	LED = 1;
	while ( 1 )
	{
		keepAlive();
		send_mqtt_data();

	}
}


