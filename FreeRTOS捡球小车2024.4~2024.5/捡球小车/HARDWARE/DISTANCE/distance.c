#include "sys.h"
#include "distance.h"
#include "delay.h"

//初始化超声波模块TIM2_CH2(PA1)
void Distance_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);  //使能GPIO外设时钟使能
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //开启TIM2时钟	
	
	//Trig引脚		PA4
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//100MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_4); 	//开始置低电
	
	//ECHO输入	PA1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	TIM_TimeBaseInitStructure.TIM_Period = 65535;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 71;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);
}

//超声波测距第一个启发脉冲
void frist_maichong(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_4);
	delay_ms(30);
	GPIO_ResetBits(GPIOA, GPIO_Pin_4);
}

float Get_Distance(void)
{
	while (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1) == 1);//等到ECHO为低电平，即返回的值成功后，才启动超声波。
	frist_maichong();
	while (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1) == 0);//等到ECHO接收返回数据时，计时。
  TIM_SetCounter(TIM2,0); //清零计数器
	TIM_Cmd(TIM2, ENABLE);  //使能定时器2,开始计数
  while (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1) == 1);//等到ECHO为低电平才停止计时
	TIM_Cmd(TIM2, DISABLE);	//失能定时器2,截止计数	
	
	return 	(TIM_GetCounter(TIM2))/1000000.0*340/2 *100;//用时间计时单位cm
}
