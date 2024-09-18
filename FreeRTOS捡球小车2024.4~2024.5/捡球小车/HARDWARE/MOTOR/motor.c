#include "motor.h"
#include "sys.h"


//电机PWM初始化TIM3_CH1、2， CH3、4		（PA6、PA7、PB0、PB1）
void Motor_PWM_Init(u16 arr,u16 psc)  //Prescaler=0, Period=7200-1
{		 		
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);// 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);  //使能GPIO外设时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);  //使能GPIO外设时钟使能
	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  不分频
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_Pulse = 0;                            //设置待装入捕获比较寄存器的脉冲值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     //输出极性:TIM输出比较极性高
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx

  TIM_CtrlPWMOutputs(TIM3,ENABLE);	//MOE 主输出使能	

	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  //CH4预装载使能	 
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //CH4预装载使能	
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);  //CH4预装载使能	 
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);  //CH4预装载使能	
	
	TIM_ARRPreloadConfig(TIM3, ENABLE); //使能TIMx在ARR上的预装载寄存器
	
	TIM_Cmd(TIM3, ENABLE);  //使能TIM1
} 


//接收openmv的数据控制车轮运动
void Car_Run(int Left_Speed, int Right_Speed)
{
	Left_Motor(Left_Speed);
	Right_Motor(Right_Speed);
	
	Left_Speed = 0;
	Left_Speed = 0;
}


//没有接收到小球，车辆旋转
void Car_Turn(void)
{
	Left_Motor(50);
	Right_Motor(-50);
}


//左轮运动
void Left_Motor(int PWM)
{
	//限幅
	if(PWM > 80){ PWM = 80; }
	if(PWM < 80){ PWM = -80; }
	
	if(PWM > 0)
	{
		TIM_SetCompare1(TIM3, PWM);
		TIM_SetCompare2(TIM3, 0);
	}
	if(PWM <= 0)
	{
		TIM_SetCompare1(TIM3, 0);
		TIM_SetCompare2(TIM3, PWM);
	}
}

//左轮运动
void Right_Motor(int PWM)
{
	//限幅
	if(PWM > 80){ PWM = 80; }
	if(PWM < 80){ PWM = -80; }
	
	if(PWM > 0)
	{
		TIM_SetCompare3(TIM3, PWM);
		TIM_SetCompare4(TIM3, 0);
	}
	if(PWM <= 0)
	{
		TIM_SetCompare4(TIM3, 0);
		TIM_SetCompare3(TIM3, PWM);
	}
}

//******************************************SG90舵机部分*******************************************************
//TIM4_CH1、2、3          PB6、7、8
//通道1：夹取乒乓球的夹子			通道2、3：提升夹子的舵机
void SG90_Init(void)
{
		// 结构体定义
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// 开启时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	// 初始化GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   // 复用推挽输出
	GPIO_Init(GPIOB,&GPIO_InitStructure);
//	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3,ENABLE);   // 改变指定管脚的映射	
	
	// 初始化定时器参数
	TIM_TimeBaseInitStructure.TIM_Period = 200;   // 自动装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7200-1;   // 分频系数
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;   // 设置向上计数模式
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);	
	
	// 初始化PWM参数
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;   // 比较输出模式
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;   // 输出极性
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   // 输出使能
	TIM_OC1Init(TIM4,&TIM_OCInitStructure);   // 输出比较通道1初始化
	
	TIM_OC1PreloadConfig(TIM4,TIM_OCPreload_Enable);   // 使能TIMx在 CCR1 上的预装载寄存器
	TIM_ARRPreloadConfig(TIM4,ENABLE);   // 使能预装载寄存器
	
	TIM_Cmd(TIM4,ENABLE);   

}

//夹子闭合
void Clamp_Close(void)
{
	TIM_SetCompare1(TIM4,195);   // 旋转到0°
}
//夹子打开
void Clamp_Open(void)
{
	TIM_SetCompare1(TIM4,185);   // 旋转到90°
}

//提升舵机提起
void Elevator_Up(void)
{
	TIM_SetCompare2(TIM4,180);   // 旋转到135°
	TIM_SetCompare3(TIM4,180);   // 旋转到135°
}

//提升舵机放下
void Elevator_Down(void)
{
	TIM_SetCompare2(TIM4,195);   // 旋转到0°
	TIM_SetCompare3(TIM4,195);   // 旋转到0°
}




















