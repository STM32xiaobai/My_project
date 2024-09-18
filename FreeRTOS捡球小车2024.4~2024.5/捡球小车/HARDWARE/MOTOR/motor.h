#ifndef __MOTOR_H
#define __MOTOR_H
#include "stdio.h"	
#include "sys.h" 

void Motor_PWM_Init(u16 arr,u16 psc);
void Car_Run(int Left_Speed, int Right_Speed);
void Car_Turn(void);
void Left_Motor(int PWM);
void Right_Motor(int PWM);

//夹子闭合
void Clamp_Close(void);
//夹子打开
void Clamp_Open(void);
//提升舵机提起
void Elevator_Up(void);
//提升舵机放下
void Elevator_Down(void);

#endif
