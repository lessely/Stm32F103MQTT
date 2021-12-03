#include "sys.h"
#include "stm32f10x.h"
#include "common.h"
#include <stdio.h>
#include <stdbool.h>


#define    u8    unsigned char
#define    u16   unsigned short
#define    u32  unsigned int
	
u16 MqttConnectPacket(u8 *mqtt_message,char *client_id,char *username,char *password);
u8 mqtt_publish_message(u8* mqtt_message, char* topic, char* message, u8 qos);
u8 mqtt_puback_message(u8* mqtt_message);
u16 mqtt_subscribe_message(u8* mqtt_message, char* topic, u8 qos, u8 whether);
u8 mqtt_ping_message(u8* mqtt_message);
u8 mqtt_disconnect_message(u8* mqtt_message);
bool ESP_SendTcp(u8* data, u16 len);
bool mqtt_connect() ;
void  keepAlive();
void send_mqtt_data();
	
