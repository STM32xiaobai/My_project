#include "sys.h"
#include "delay.h"
#include "usart.h" 
#include "led.h" 		 	 
#include "lcd.h" 	 
#include "gps.h"	
#include "usart3.h" 	
#include "key.h" 	 
#include "string.h"	


u8 USART1_TX_BUF[USART3_MAX_RECV_LEN]; 					//串口1,发送缓存区
nmea_msg gpsx; 											//GPS信息
__align(4) u8 dtbuf[50];   								//打印缓存器
const u8*fixmode_tbl[4]={"Fail","Fail"," 2D "," 3D "};	//fix mode字符串 
	   

int main(void)
{	 
	u16 i,rxlen;
	u8 key=0XFF;
   
	Power_switch();		//开启总电源引脚
	Power_ON();
	
	delay_init();	    	 //延时函数初始化	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);		//开启PWR的时钟
	
	OLED_Init();
	MyRTC_Init();
	
	MAX6675_GPIO_Init();	//初始化热电偶
	uart_init(115200);	 	//串口初始化为115200
 	LED_Init();		  			//初始化与LED连接的硬件接口
	KEY_Init();					//初始化按键 
	usart3_init(38400);		//初始化串口3 
	Usart2_Init(115200);
	
	uint32_t Alqrm = RTC_GetCounter() + 60*60;			//闹钟设定1小时触发一次
	RTC_SetAlarm(Alqrm);
 
	Test_GPS();		//设置定位信息更新速度为5Hz,顺便判断GPS模块是否在位. 
	
	while(1) 
	{
		
		int Temperature = MAX6675_Get_Temperature();
		printf("%d\r\n", Temperature);

		if(Temperature >= 60)		//温度过高
		{
			retry:
			delay_ms(1);
			if(USART3_RX_STA&0X8000)		//接收到一次数据了
			{
				rxlen=USART3_RX_STA&0X7FFF;	//得到数据长度
				for(i=0;i<rxlen;i++)USART1_TX_BUF[i]=USART3_RX_BUF[i];	   
				USART3_RX_STA=0;		   	//启动下一次接收
				USART1_TX_BUF[i]=0;			//自动添加结束符
				GPS_Analysis(&gpsx,(u8*)USART1_TX_BUF);//分析字符串	
				Gps_Msg_Show();				//显示信息	
				
				if(gpsx.fixmode <= 1)		//如果没有定位
				{
					goto retry;
				}
			}
			
			sim900a_send();
		}
		
	OLED_ShowString(0, 1, "Longitude:", OLED_6X8);		//经度
	OLED_Update();
		OLED_ShowData();
		
		OLED_Clear();
		Power_OFF();
		PWR_EnterSTANDBYMode();		//STM32进入停止模式，并等待指定的唤醒事件（WKUP上升沿或RTC闹钟）
	}
		
}


















