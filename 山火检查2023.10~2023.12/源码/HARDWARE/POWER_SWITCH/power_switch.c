#include "sys.h"
#include "power_switch.h"

//初始化电源总控制引脚
void Power_switch(void)		//PA6
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void Power_ON(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_6);
}

void Power_OFF(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_6);
}
