#include "sys.h"
#include "usart.h"	
#include "string.h"

uint16_t BlueRxBuffer[7]={0};	
uint8_t Right_Speed = 0, Left_Speed = 0;		//作废
uint16_t x_num, y_num, colo_high, colo_low;

//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"					//FreeRTOS使用
#include "task.h"
#include "semphr.h"	  
#endif

//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 
 
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	  
  
void uart_init(u32 bound){
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
  
	//USART1_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
   
	//USART1_RX	  GPIOA.10初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=7 ;//抢占优先级7
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
	//USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART1, &USART_InitStructure); //初始化串口1
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
	USART_Cmd(USART1, ENABLE);                    //使能串口1 
}


void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res;
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res =USART_ReceiveData(USART1);	//读取接收到的数据
		
		if((USART_RX_STA&0x8000)==0)//接收未完成
			{
			if(USART_RX_STA&0x4000)//接收到了0x0d
				{
				if(Res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
				else USART_RX_STA|=0x8000;	//接收完成了 
				}
			else //还没收到0X0D
				{	
				if(Res==0x0d)USART_RX_STA|=0x4000;
				else
					{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
					}		 
				}
			}   		 
     } 
} 
#endif	





//************************************************另加的代码***************************************************************************
//初始化USART2接收openmv的数据(串口2和3在APB1)
void USART2_Init(u32 bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  
	//USART2_TX   GPIOA2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);
   
	//USART2_RX	  GPIOA3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure); 

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=7 ;//抢占优先级7
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART2, &USART_InitStructure); //初始化串口1
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接受中断
	USART_Cmd(USART2, ENABLE);                    //使能串口1 
}

void USART2_IRQHandler(void)      
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) 
	{
		USART_ClearITPendingBit(USART2,USART_IT_RXNE); 
		uint16_t com_data = USART_ReceiveData(USART2);
		
		Openmv_Receive(com_data);
	}
}


//接收openmv的数据
void Openmv_Receive(uint16_t com_data)
{
	uint8_t i;
	static uint8_t BlueRxCounter=0;//计数
	static uint8_t BlueRxState = 0, OpenmvRxState = 0;	//BlueRxState：介绍到参数，OpenmvRxState：未接受到参数原地旋转

//接收到了小球位置
	if(BlueRxState==0&&com_data==0x6A)  //0x6A帧头
	{			
		BlueRxState = 1;
		BlueRxBuffer[BlueRxCounter++]=com_data;  
	}
	else if(BlueRxState==1)
	{
		BlueRxBuffer[BlueRxCounter++] = com_data;
		if(BlueRxCounter>=7||com_data == 0x6D)		//0x6D帧尾
		{
			BlueRxState = 2;
			//取出数据
			x_num = BlueRxBuffer[BlueRxCounter-5];
			y_num = BlueRxBuffer[BlueRxCounter-4];
			colo_high = BlueRxBuffer[BlueRxCounter-3];
			colo_low = BlueRxBuffer[BlueRxCounter-2];
//			printf("%d, %d, %d, %d\r\n",x_num, y_num, colo_high, colo_low);
		}
	}
	else if(BlueRxState==2)		//检测是否接受到结束标志
	{
			if(BlueRxBuffer[BlueRxCounter] == 0x6A)    
			{
						BlueRxState = 0;
						BlueRxCounter = 0;				
			}
			else   //接收错误
			{
						BlueRxState  = 0;
						BlueRxCounter =0;
						for(i=0;i<7;i++)
						{
								BlueRxBuffer[i]=0x00;      //将存放数据数组清零
						}
			}
	} 
	
	
	//没有接收到小球位置
	else if(BlueRxState==0&&com_data==0x6B)  //0x6B帧头
	{
		OpenmvRxState = 1;
		BlueRxBuffer[BlueRxCounter++]=com_data;  
	}
	else if(OpenmvRxState==1)
	{
		BlueRxBuffer[BlueRxCounter++] = com_data;
		if(BlueRxCounter>=7||com_data == 0x6D)		//0x6D帧尾
		{
			OpenmvRxState = 2;

			x_num = 0;
			y_num = 0;
			colo_high = 0;
			colo_low = 0;	
		}
	}
	else if(OpenmvRxState==2)		//检测是否接受到结束标志
	{
			if(BlueRxBuffer[BlueRxCounter] == 0x6D)    
			{
						OpenmvRxState = 0;
						BlueRxCounter = 0;				
			}
			else   //接收错误
			{
						OpenmvRxState  = 0;
						BlueRxCounter =0;
						for(i=0;i<7;i++)
						{
								BlueRxBuffer[i]=0x00;      //将存放数据数组清零
						}
			}
	} 
 
}


//su-03t语音模块串口接收
//初始化USART3接收openmv的数据(串口2和3在APB1)
void USART3_Init(u32 bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
  
	//USART3_TX   GPIOB10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);
   
	//USART3_RX	  GPIOB11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOB, &GPIO_InitStructure); 

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=7 ;//抢占优先级7
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART3, &USART_InitStructure); //初始化串口1
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启串口接受中断
	USART_Cmd(USART3, ENABLE);                    //使能串口1 
}

void USART3_IRQHandler(void)      
{
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) 
	{
		USART_ClearITPendingBit(USART3,USART_IT_RXNE); 
		uint16_t com_data = USART_ReceiveData(USART3);
		
		Su03t_Receive_Data(com_data);
	}
}


uint8_t Su03tRxBuffer[7] = {0};
uint8_t Su03t_Data = 0;
//su03t接收函数
void Su03t_Receive_Data(uint8_t com_data)
{
	uint8_t i;
	static uint8_t BlueRxCounter=0;//计数
	static uint8_t BlueRxState = 0;	//接收状态

	if(BlueRxState==0&&com_data==0xAA)  //0x52帧头
	{			
		BlueRxState = 1;
		Su03tRxBuffer[BlueRxCounter++]=com_data;  
	}
	else if(BlueRxState==1&&com_data==0x55)  //0x21帧头
	{
		BlueRxState = 2;
		Su03tRxBuffer[BlueRxCounter++]=com_data;
	}	
	else if(BlueRxState==2)
	{	
		//存疑
		Su03tRxBuffer[BlueRxCounter++]=com_data;
		if(BlueRxCounter>=7||com_data == 0x55)    
		{
			BlueRxState = 3;       
			Su03t_Data =  Su03tRxBuffer[BlueRxCounter-2];
		}
	}

	else if(BlueRxState==3)		//检测是否接受到结束标志
	{
			if(Su03tRxBuffer[BlueRxCounter-1] == 0xAA)    
			{
						BlueRxState = 0;
						BlueRxCounter = 0;				
			}
			else   //接收错误
			{
						BlueRxState  = 0;
						BlueRxCounter =0;
						for(i=0;i<7;i++)
						{
								Su03tRxBuffer[i]=0x00;      //将存放数据数组清零
						}
			}
	} 
	else   //接收异常
	{
			BlueRxState = 0;
			BlueRxCounter = 0;
			for(i=0;i<7;i++)
			{
					Su03tRxBuffer[i]=0x00;      //将存放数据数组清零
			}
	}
}






























