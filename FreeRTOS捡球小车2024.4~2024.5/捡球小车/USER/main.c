#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "timer.h"
#include "lcd.h"
#include "key.h"
#include "beep.h"
#include "malloc.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "freertos_demo.h"


extern uint8_t Su03t_Data;

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4	 
	delay_init();	    				//延时函数初始化	 
	uart_init(115200);					//初始化串口
	LED_Init();		  					//初始化LED
	KEY_Init();							//初始化按键
	BEEP_Init();						//初始化蜂鸣器
	my_mem_init(SRAMIN);            	//初始化内部内存池
	
	//必须要接入超声波模块，才能运行，因为二值信号是创建到超声波任务里的
	Distance_Init();		
	USART2_Init(115200);
	USART3_Init(115200);
	Motor_PWM_Init(100-1, 7200-1);
	OLED_Init();
	SG90_Init();
	
	
	while(1)
	{
		uint8_t key = KEY_Scan(0);
		if(Su03t_Data == 1 || key == WKUP_PRES) {break;}
	}
	
	
	freertos_demo();
}




