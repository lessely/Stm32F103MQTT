#include "mqtt.h"
#include "bsp_esp8266.h"
#include "common.h"
#include <stdio.h>  
#include <string.h>  
#include <stdbool.h>
#include "bsp_SysTick.h"

#define    pub  "stm32"//����������
#define    des  "mydata"  //���������
u8 mqtt_msg[200] = { 0 };
/***************************************MQTT�ͻ���*************************************/


/*************MQTT���Ӱ�װ*******************/
u16 MqttConnectPacket(u8 *mqtt_message,char *client_id,char *username,char *password)
{
	u16 client_id_length = strlen(client_id);
	u16 username_length = strlen(username);
	u16 password_length = strlen(password);
	u16 packetLen;
	u16 i,baseIndex;
	
	packetLen = 12 + 2 + client_id_length;
	if(username_length > 0)
		packetLen = packetLen + 2 + username_length;
	if(password_length > 0)
		packetLen = packetLen+ 2 + password_length;
	
	mqtt_message[0] = 16;				//0x10 // MQTT Message Type CONNECT
	mqtt_message[1] = packetLen - 2;	//ʣ�೤�ȣ��������̶�ͷ
	baseIndex = 2;
	if(packetLen > 127)
	{
		mqtt_message[2] = 1;			//packetLen/127;    
		baseIndex = 3;
	}
	mqtt_message[baseIndex++] = 0;		// Protocol Name Length MSB    
	mqtt_message[baseIndex++] = 4;		// Protocol Name Length LSB    
	mqtt_message[baseIndex++] = 77;		// ASCII Code for M    
	mqtt_message[baseIndex++] = 81;		// ASCII Code for Q    
	mqtt_message[baseIndex++] = 84;		// ASCII Code for T    
	mqtt_message[baseIndex++] = 84;		// ASCII Code for T    
	mqtt_message[baseIndex++] = 4;		// MQTT Protocol version = 4    
	mqtt_message[baseIndex++] = 0xC2;		// conn flags ��Ҫ�û�����������֤
	mqtt_message[baseIndex++] = 0;		// Keep-alive Time Length MSB    
	mqtt_message[baseIndex++] = 60;		// Keep-alive Time Length LSB    
	mqtt_message[baseIndex++] = (0xff00&client_id_length)>>8;// Client ID length MSB    
	mqtt_message[baseIndex++] = 0xff&client_id_length;	// Client ID length LSB    
	
	// Client ID
	for(i = 0; i < client_id_length; i++)
	{
		mqtt_message[baseIndex + i] = client_id[i];    
	}
	baseIndex = baseIndex+client_id_length;
		
	if(username_length > 0)
	{
		//username    
		mqtt_message[baseIndex++] = (0xff00&username_length)>>8;//username length MSB    
		mqtt_message[baseIndex++] = 0xff&username_length;	//username length LSB    
		for(i = 0; i < username_length ; i++)
		{
			mqtt_message[baseIndex + i] = username[i];    
		}
		baseIndex = baseIndex + username_length;
	}
		
	if(password_length > 0)
	{
		//password    
		mqtt_message[baseIndex++] = (0xff00&password_length)>>8;//password length MSB    
		mqtt_message[baseIndex++] = 0xff&password_length;	//password length LSB    
		for(i = 0; i < password_length ; i++)
		{
			mqtt_message[baseIndex + i] = password[i];    
		}
		baseIndex += password_length; 
	}	
	
	return baseIndex;    
}

/*********MQTT������Ϣ��װ******************************/
u8 mqtt_publish_message(u8* mqtt_message, char* topic, char* message, u8 qos)
{
    u16 topic_length = strlen(topic);
    u16 message_length = strlen(message);
    u16 i, index = 0;
    static u16 id = 0;

    mqtt_message[index++] = 48; //0x30 // MQTT Message Type PUBLISH    
    if (qos)
        mqtt_message[index++] = 2 + topic_length + 2 + message_length;
    else
        mqtt_message[index++] = 2 + topic_length + message_length;   // Remaining length    
    mqtt_message[index++] = (0xff00 & topic_length) >> 8;
    mqtt_message[index++] = 0xff & topic_length;

    // Topic    
    for (i = 0; i < topic_length; i++)
    {
        mqtt_message[index + i] = topic[i];
    }
    index += topic_length;

    if (qos)
    {
        mqtt_message[index++] = (0xff00 & id) >> 8;
        mqtt_message[index++] = 0xff & id;
        id++;
    }

    // Message
    for (i = 0; i < message_length; i++)
    {
        mqtt_message[index + i] = message[i];
    }
    index += message_length;
    return index;
 
}

/*****************����ȷ�ϰ�****************************/
u8 mqtt_puback_message(u8* mqtt_message)
{
    static u16 id = 0;
    mqtt_message[0] = 64;       //0x40 //��Ϣ���ͺͱ�־ PUBACK
    mqtt_message[1] = 2;        //ʣ�೤��(�������̶�ͷ��)
    mqtt_message[2] = (0xff00 & id) >> 8; //��Ϣ��ʶ��
    mqtt_message[3] = 0xff & id;      //��Ϣ��ʶ��
    id++;
    return 4;
}

/*********�������������װ*********/
u16 mqtt_subscribe_message(u8* mqtt_message, char* topic, u8 qos, u8 whether)
{
    u16 topic_len = strlen(topic);
    u16 i, index = 0;
    static u16 id = 0;

    id++;
    if (whether)
        mqtt_message[index++] = 130;
    else
        mqtt_message[index++] = 162;
    mqtt_message[index++] = topic_len + 5;
    mqtt_message[index++] = (0xff00 & id) >> 8;
    mqtt_message[index++] = 0xff & id;
    mqtt_message[index++] = (0xff00 & topic_len) >> 8;
    mqtt_message[index++] = 0xff & topic_len;
    for (i = 0; i < topic_len; i++)
    {
        mqtt_message[index + i] = topic[i];
    }
    index += topic_len;

    if (whether)
    {
        mqtt_message[index] = qos;//QoS����
        index++;
    }
    return index;
}

/***********����MQTT����PING��********************/
u8 mqtt_ping_message(u8* mqtt_message)
{
    mqtt_message[0] = 192;  //0xC0 //��Ϣ���ͺͱ�־ PING
    mqtt_message[1] = 0;  //ʣ�೤��(�������̶�ͷ��)
    return 2;
}

/************�����Ͽ���******************/
u8 mqtt_disconnect_message(u8* mqtt_message)
{
    mqtt_message[0] = 224;  //0xE0 //��Ϣ���ͺͱ�־ DISCONNECT
    mqtt_message[1] = 0;  //ʣ�೤��(�������̶�ͷ��)
    return 2;
}

/***************��·���ӷ�������****************/
bool ESP_SendTcp(u8* data, u16 len)
{
    char cmd[100];//��������ָ��
		bool bRet = false;
		int i=0;

		sprintf ( cmd, "AT+CIPSEND=%d",len);
		ESP8266_Cmd(cmd,">",0,500);	
		
		for(i=0;i<len;i++)
		{
			USART3_Send_Byte(data[i]);
		}
		Delay_ms(100);
}

//����MQTT������
bool mqtt_connect() {
    u16 len;
		u16 i=0;
    char cmd[120]="AT+CIPSTART=\"TCP\",\"192.168.5.195\",1883";
		if(ESP8266_Cmd (cmd, "OK","ALREAY CONNECT", 500))
			//printf("\n����MQTT�������ɹ�\n");
		
		//printf("\n��֤�û�������\n");
    len = MqttConnectPacket(mqtt_msg, "stm32", "lessely", "haohaoyaya");//��װ
		ESP_SendTcp(mqtt_msg, len);


    printf("\n��ʼ����...\n");
    len = mqtt_subscribe_message(mqtt_msg, des, 0, 1);//���İ�װ
    ESP_SendTcp(mqtt_msg, len);
        printf("\n���ĳɹ�\n");

    return true;
}

void  keepAlive() {
    u8 ping[2];
    ping[0] = 0xc0;
    ping[1] = 0x00;
    ESP_SendTcp(ping, 2);//��������
}

void send_mqtt_data(){
		u16 len;
		char *a={"Hello"};
		char mg[100];
		strcpy(mg, a);
		len = mqtt_publish_message(mqtt_msg, pub, mg, 0);
		ESP_SendTcp(mqtt_msg, len);
}