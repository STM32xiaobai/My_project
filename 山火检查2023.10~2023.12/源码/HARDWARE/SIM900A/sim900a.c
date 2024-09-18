 #include "sys.h"
#include "delay.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>
#include "sim900a.h"

extern u8 Usart2_buff[128];     /*static和extern的区别*/
char dispbuf[64];


/**************************************************************************/
//函数作用：字符串匹对函数
//函数名称：Find_char(char *a,char *b) ;
//内部参数：数组a   数组b
//修改日期：2022年1月26日  凌晨2：40
//作者：       (CSDN搜)大屁桃
/**************************************************************************/
u8 Find_char(char *a,char *b)  //b为子串
{ 
  if(strstr(a,b)!=NULL)
	    return 0;
	else
			return 1;
}

/**************************************************************************/
//函数作用：发送英文短信函数
//函数名称：sim900a_send_English_message(char *message,char *phonenumber)(uint8_t number);
//内部参数：message phonenumber
//修改日期：2022年1月26日  凌晨2：40、
//作者：       (CSDN搜)大屁桃
/**************************************************************************/
void sim900a_send_English_message(char *message,char *phonenumber)
{

	Usart_SendString2(USART2,"AT\r\n");                            //SIM900A是否与单片机来连接成功
	delay_ms(200);	
	while(Find_char((char*)Usart2_buff,"OK"));                      
	printf("English_message_OK1\r\n");
	
	Usart_SendString2(USART2,"AT&F\r\n");                           //SIM900A复位
	delay_ms(200);	
	while(Find_char((char*)Usart2_buff,"OK"));                     //字符串匹对函数   
	printf("English_message_OK2\r\n");	
	
	Usart_SendString2(USART2,"AT+CSCS=\"GSM\"\r\n");               //英文短信指令1
	delay_ms(200);	
	while(Find_char((char*)Usart2_buff,"OK"));                     
	printf("English_message_OK3\r\n");
	
	
	Usart_SendString2(USART2,"AT+CMGF=1\r\n");                     //英文短信指令2
	delay_ms(200);	
	while(Find_char((char*)Usart2_buff,"OK"));  
	printf("English_message_OK4\r\n");
	
	sprintf(dispbuf,"AT+CMGS=\"%s\"\r\n",phonenumber);
	Usart_SendString2(USART2,dispbuf);                             //英文短信指令3
	delay_ms(200);
	while(Find_char((char*)Usart2_buff,"OK")); 
	printf("English_message_OK5\r\n");
	
	Usart_SendString2(USART2,message);                              //英文短信指令4
	delay_ms(200);
	while(Find_char((char*)Usart2_buff,"OK"));  
	printf("English_message_OK6\r\n");
	
	Usart_SendHalfWord(USART2,0x1a);                                //结束指令
	delay_ms(2000);  //延时两秒
	while(Find_char((char*)Usart2_buff,"OK"));  
	printf("English_message_OK7\r\n");
} 

/**************************************************************************/
//函数作用：发送中文短信函数
//函数名称：sim900a_send_Chinese_message(char *message,char *phonenumber)(uint8_t number);
//内部参数：message phonenumber
//修改日期：2022年1月26日  凌晨2：40
//作者：       (CSDN搜)大屁桃
/**************************************************************************/
void sim900a_send_Chinese_message(char *message,char *phonenumber)
{ 
	
	Usart_SendString2(USART2,"AT\r\n");                           //SIM900A是否与单片机来连接成功
	delay_ms(200);	
	while(Find_char((char*)Usart2_buff,"OK"));                     //字符串匹对函数   
	printf("Chinese_message_OK1\r\n");
	
	Usart_SendString2(USART2,"AT&F\r\n");                          //SIM900A复位
	delay_ms(200);	
	while(Find_char((char*)Usart2_buff,"OK"));                    
	printf("Chinese_message_OK2\r\n");	
	
	Usart_SendString2(USART2,"AT+CSCS=\"UCS2\"\r\n");               //中文短信指令1
	delay_ms(200);	
	while(Find_char((char*)Usart2_buff,"OK"));                       
	printf("Chinese_message_OK3\r\n");
	
	
	Usart_SendString2(USART2,"AT+CMGF=1\r\n");                     //中文短信指令2
	delay_ms(200);	
	while(Find_char((char*)Usart2_buff,"OK"));  
	printf("Chinese_message_OK4\r\n");
	
	Usart_SendString2(USART2,"AT+CSMP=17,167,0,8\r\n");            //中文短信指令2
	delay_ms(200);	
	while(Find_char((char*)Usart2_buff,"OK"));  
	printf("Chinese_message_OK5\r\n");
	
	sprintf(dispbuf,"AT+CMGS=\"%s\"\r\n",phonenumber);
	Usart_SendString2(USART2,dispbuf);                             //中文短信指令3
	delay_ms(200);
	while(Find_char((char*)Usart2_buff,"OK")); 
	printf("Chinese_message_OK6\r\n");
	
	Usart_SendString2(USART2,message);                              //中文短信指令4
	delay_ms(200);
	while(Find_char((char*)Usart2_buff,"OK"));  
	printf("Chinese_message_OK7\r\n");
	
	Usart_SendHalfWord(USART2,0x1a);                                //中文结束指令
	delay_ms(2000);  //延时两秒
	while(Find_char((char*)Usart2_buff,"OK"));  
	printf("Chinese_message_OK8\r\n");
} 

extern float longitude, latitude, altitude;		//精度、维度、海拔

//sim900A发送数据
void sim900a_send(void)
{
	//发送短信
	char English_message[100];
	sprintf(English_message, "Axis is %.5fE %.5fN  %.1fM on fire", longitude, latitude, altitude);		//英文短信内容
	char phonenumber[]={"17328967102"};                             //接受短信的号码
	
	Usart2_Init(115200);		//初始化串口2
	
	printf("Usart2_Init\r\n");
	
	//可以点个灯表示一下发送
	LED0 = 0;
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	sim900a_send_English_message(English_message, phonenumber);
	LED0 = 1;

}





