#include "max6675.h"
#include "sys.h"

/*
	接线
K型热电偶MAX6675模块--------------------STM32F103C8T6
VCC------------------------------------5V
GND------------------------------------GND
SO-------------------------------------PB12
CS-------------------------------------PB13
SCK------------------------------------PB14
*/

void MAX6675_GPIO_Init(void)	
{

	GPIO_InitTypeDef  GPIO_InitStructure;  

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
 

	GPIO_InitStructure.GPIO_Pin =  MAX6675_SO_PIN | MAX6675_CS_PIN | MAX6675_SCK_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//
	GPIO_Init(GPIOB, &GPIO_InitStructure);

}

void SO_Pin_IN(void)  //SDA输入
{
	GPIO_InitTypeDef  GPIO_InitStructure;  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
 
	GPIO_InitStructure.GPIO_Pin =  MAX6675_SO_PIN ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void SO_Pin_Output(void) //
{
	GPIO_InitTypeDef  GPIO_InitStructure;  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
 
	GPIO_InitStructure.GPIO_Pin =  MAX6675_SO_PIN ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}


uint16_t MAX6675_ReadReg(void)
{ 
	uint8_t i=0;   
	uint16_t dat=0;
	 
	
	MAX6675_CS_L; 
	MAX6675_SCK_L;     
	SO_Pin_Output();
	for(i=0; i<16; i++)		//get D15-D0 from 6675  
	{      
		MAX6675_SCK_H;     
		dat = dat<<1;  
		if(MAX6675_SO_Read)   
		{
			dat = dat|0x01;  
		}
		MAX6675_SCK_L;    
	} 
	MAX6675_CS_H;   
	SO_Pin_Output();  
	return dat;   
} 

uint16_t Temp = 0,Temp_Dat = 0;	//温度和湿度
uint8_t Flag_connect;
unsigned char T_DatBuf[10]={0};

void dat_dis(void)//温湿度数据处理转为字符串
{
    T_DatBuf[0]='T';
    T_DatBuf[1]=':';
		T_DatBuf[2] = Temp/1000 + 0x30;		//十位
		T_DatBuf[3] = (Temp%1000)/100 + 0x30;			//个位
		T_DatBuf[4] = '.';		
		T_DatBuf[5] = (Temp%100)/10 + 0x30;
		T_DatBuf[6] = Temp%10 + 0x30;	
    T_DatBuf[7]='C';    
    T_DatBuf[8]='\0';
    
}

int MAX6675_Get_Temperature(void)
{
	Temp_Dat = MAX6675_ReadReg();
	Flag_connect = Temp_Dat & 0x04;		////读出数据的D2位是热电偶掉线标志位，该位为1表示掉线，该位为0表示连接
	Flag_connect=Flag_connect>>2; 	//MAX6675是否在线
	
	Temp_Dat = Temp_Dat<<1;					   	//读出来的数据的D3~D14是温度值
	Temp_Dat = Temp_Dat>>4;
	Temp=Temp_Dat*100/4;
	delay_ms(1000);
	
//	dat_dis();
	Temp = (Temp/1000)*10 + (Temp%1000)/100;
	
	return Temp;
}






















