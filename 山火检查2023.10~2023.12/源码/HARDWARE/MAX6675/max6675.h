#ifndef __MAX6675_H
#define __MAX6675_H
#include "stm32f10x.h"

#define  MAX6675_SO_PIN   				GPIO_Pin_12     //								
#define  MAX6675_CS_PIN    				GPIO_Pin_13			// 
#define  MAX6675_SCK_PIN    			GPIO_Pin_14			// 



#define  MAX6675_SO_H   			GPIO_SetBits(GPIOB,MAX6675_SO_PIN)
#define  MAX6675_SO_L   			GPIO_ResetBits(GPIOB,MAX6675_SO_PIN)

#define  MAX6675_CS_H   			GPIO_SetBits(GPIOB,MAX6675_CS_PIN)
#define  MAX6675_CS_L   			GPIO_ResetBits(GPIOB,MAX6675_CS_PIN)

#define  MAX6675_SCK_H   			GPIO_SetBits(GPIOB,MAX6675_SCK_PIN)
#define  MAX6675_SCK_L   			GPIO_ResetBits(GPIOB,MAX6675_SCK_PIN)

#define  MAX6675_SO_Read      GPIO_ReadInputDataBit(GPIOB, MAX6675_SO_PIN)



void MAX6675_GPIO_Init(void);
void SO_Pin_IN(void);
void SO_Pin_Output(void);
uint16_t MAX6675_ReadReg(void);
void dat_dis(void);

int MAX6675_Get_Temperature(void);

#endif
