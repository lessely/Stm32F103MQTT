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
 * @brief  ������
 * @param  ��
 * @retval ��
 */
int main( void )
{
	/* ��ʼ�� */
	USARTx_Config();                                        /* ��ʼ������1 */
	SysTick_Init();                                         /* ���� SysTick Ϊ 1ms �ж�һ�� */
	ESP8266_Init();                                         /* ��ʼ��WiFiģ��ʹ�õĽӿں����� */
	delay_init();                                           /* ��ʱ������ʼ�� */
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_2 );       /* ����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ� */
	LED_Init();

	printf( "\r\n�������� ESP8266 ......\r\n" );
	macESP8266_CH_ENABLE();//��ʼ��io
	//printf("\n���ͣ�AT\n");
	ESP8266_AT_Test();//����AT�ȴ�OK
	//printf("\n���ù���ģʽ\n");
	ESP8266_Net_Mode_Choose( STA );//����STAģʽ
	//printf("\n����wifi��...\n");
	while ( !ESP8266_JoinAP( macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd ) );//����wifi
	//printf("\n���õ�����ģʽ\n");
	ESP8266_Enable_MultipleId( DISABLE );//���õ�����
	//printf("\n��ʼ����MQTT������\n");
	mqtt_connect();
	printf( "\r\n���� ESP8266 ���\r\n" );
	LED = 1;
	while ( 1 )
	{
		keepAlive();
		send_mqtt_data();

	}
}


