#include "freertos_demo.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "malloc.h"
#include <stdlib.h>

//freertos配置
#include "FreeRTOS.h"
#include "event_groups.h"
#include "task.h"
#include "semphr.h"

extern uint16_t x_num, y_num, colo_high, colo_low;
extern uint8_t Right_Speed, Left_Speed;		//获取openmv发送的数据
extern uint8_t Su03t_Data;

//开始任务
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_TASK_STACK_SIZE 		128  
//任务句柄
TaskHandle_t start_task_handler;
//任务函数
void start_task(void *pvParameters);

//task1
#define TASK1_TASK_PRIO		2
//任务堆栈大小	
#define TASK1_STK_SIZE 		128  
//任务句柄
TaskHandle_t task1_handler;
//任务函数
void task1(void *pvParameters);

//task2
#define TASK2_TASK_PRIO		3
#define TASK2_STK_SIZE 		128  
TaskHandle_t task2_handler;
void task2(void *pvParameters);

//task3
#define TASK3_TASK_PRIO		4
#define TASK3_STK_SIZE 		128  
TaskHandle_t task3_handler;
void task3(void *pvParameters);

void timer_50ms_callback(TimerHandle_t pxTimer);
void timer_100ms_callback(TimerHandle_t pxTimer);

//*************************事件标志位****************************************
EventGroupHandle_t eventgroup_handle;	//事件标志位句柄
#define CONTROL_MOTOR (1 << 0)		//控制驱动电机
#define CONTROL_SG90	(1 << 1)		//控制SG90舵机
#define OLED_SHOW			(1 << 2)		//OLED显示
#define USART_SU03T		(1 << 3)		//接收语音模块的数据
#define EVEVT_ALL			(CONTROL_MOTOR | CONTROL_SG90 | OLED_SHOW | USART_SU03T)
//******************************************************************************

void freertos_demo(void)
{    
    xTaskCreate((TaskFunction_t         )   start_task,
                (char *                 )   "start_task",
                (uint16_t								)   START_TASK_STACK_SIZE,	//堆栈大小
                (void *                 )   NULL,
                (UBaseType_t            )   START_TASK_PRIO,			//优先级
                (TaskHandle_t *         )   &start_task_handler );
    vTaskStartScheduler();
}


TimerHandle_t timer_50ms_handle = 0;	//OLED刷新定时器
TimerHandle_t timer_100ms_handle = 0;	//处理串口发送的数据电机控制
QueueHandle_t semphore_handle;			

//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
	
		semphore_handle = xSemaphoreCreateBinary();		//创建二值信号量
		if(semphore_handle != NULL){ printf("二值信号量创建成功!!!\r\n"); }
	
		//创建周期定时器（OLED显示）
		timer_50ms_handle = xTimerCreate( "50ms_timer", 
																	50,				//延时时间
																	pdTRUE,
																	(void *)1,
																	timer_50ms_callback );
																	
		//创建周期定时器（处理接收到的数据）
		timer_100ms_handle = xTimerCreate( "100ms_timer", 
																	100,				//延时时间
																	pdTRUE,
																	(void *)2,
																	timer_100ms_callback );
																	
		//创建事件标志位														
		eventgroup_handle = xEventGroupCreate();
		if(eventgroup_handle != NULL){ printf("事件标志位创建成功！\r\n"); }
	
    //创建TASK1任务
    xTaskCreate((TaskFunction_t )task1,             
                (const char*    )"task1",           
                (uint16_t       )TASK1_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )TASK1_TASK_PRIO,        
                (TaskHandle_t*  )&task1_handler);   
    //创建TASK2任务
    xTaskCreate((TaskFunction_t )task2,     
                (const char*    )"task2",   
                (uint16_t       )TASK2_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK2_TASK_PRIO,
                (TaskHandle_t*  )&task2_handler); 
		//创建TASK3任务
    xTaskCreate((TaskFunction_t )task3,     
                (const char*    )"task3",   
                (uint16_t       )TASK3_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK3_TASK_PRIO,
                (TaskHandle_t*  )&task3_handler); 
								
    vTaskDelete(NULL); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//任务1：做控制工作
void task1(void *pvParameters)
{
	//开启定软件时器
	xTimerStart(timer_50ms_handle, portMAX_DELAY);
	xTimerStart(timer_100ms_handle, portMAX_DELAY);
	
	EventBits_t event_bit = 0;
	
	while(1)
	{
		event_bit = xEventGroupWaitBits(eventgroup_handle,			/* 事件标志组句柄 */
																			EVEVT_ALL,						 /* 等待事件标志组的bit0和bit1位 */
																			pdTRUE,								/* 成功等待到事件标志位后，清除事件标志组中的bit0和bit1位 */
																			pdFALSE,							 /* pdFALSE 或 ; pdTRUE 与 即所有标志位满足才行*/
																			0);	

//		printf("等待到的事件标志位值为：%#x\r\n",event_bit);
		
		//有问题需要改
		if(event_bit & CONTROL_MOTOR)
		{
			float distance = Get_Distance();		//超声波测距
			
			if(distance > 10)	
			{	
				if(colo_low < 28)		//色块小，也就是距离远
				{
					if(x_num>1 && x_num<75)		//球在车的左边，小车右转
					{
						Right_Motor(50);
//						Right_Motor(abs(95-x_num));
					}
					else if(x_num>115 && x_num<190)	//球在车的右边，小车左转
					{
						Left_Motor(50);
//						Left_Motor(abs(95-x_num));
					}
					else if(x_num>=75 && x_num<=115)
					{
						Car_Run(50, 50);		//直行
					}
					else
					{
						Car_Run(0, 0);
					}
				}
				else if(colo_low > 50)	//距离太近后退
				{
					Car_Run(-50, -50);
				}
				else if(x_num==0 && y_num==0 && colo_high==0 && colo_low==0)		//没有目标小车旋转
				{
					Car_Turn();
				}	
			}
			else if(distance <= 3)		//撞墙了倒车
			{
				Left_Motor(-50);
				Right_Motor(-50);
			}
//			printf("任务1启动！！\r\n");
			xSemaphoreGive(semphore_handle);		//释放二值信号量
		}	
//		printf("任务1启动！！\r\n");
		vTaskDelay(10);
	}
}


//任务2：OLED显示工作
void task2(void *pvParameters)
{
	BaseType_t err;
	while(1)
	{
		//用二值信号看量进行OLED显示
		err = xSemaphoreTake(semphore_handle, portMAX_DELAY);		//获取二值信号量
		if(err == pdTRUE)
		{
			printf("二值信号量获取成功!!!\r\n");
			OLED_Show();
		}
		else
		{
			printf("已超时!!\r\n");
		}	
		printf("任务2启动！！\r\n");
		vTaskDelay(10);
	}
}

//挂起和恢复任务1和2
void task3(void *pvParameters)
{
	uint8_t key = 0;
	while(1)
	{
		key =  KEY_Scan(0);
		
		if(Su03t_Data == 1 || key == KEY0_PRES)
		{
			//挂起task1
			printf("挂起task1!!!\r\n");
			vTaskSuspend(task1_handler);
			vTaskSuspend(task2_handler);
		}
		if(Su03t_Data == 2 || key == KEY1_PRES)
		{
			//恢复task1
			printf("恢复task1!!!\r\n");
			vTaskResume(task1_handler);
			vTaskResume(task2_handler);
		}
		vTaskDelay(10);
	}
}

//**********************************************************************************************************
//***********************************************************************************************************

//创建周期定时器（OLED显示）
void timer_50ms_callback(TimerHandle_t pxTimer)
{	
//	printf("定时器1开启\r\n");
}
//创建周期定时器（处理接收到的数据）
void timer_100ms_callback(TimerHandle_t pxTimer)
{
	xEventGroupSetBits(eventgroup_handle, CONTROL_MOTOR);
//	printf("定时器2开启\r\n");
}

//夹取小球并放到框里
void Clamp_Ball(void)
{
	Clamp_Close();
	vTaskDelay(1000);
	Elevator_Up();
	vTaskDelay(1000);
	Clamp_Open();
}

//小球入库后收回夹子
void Put_Down_Clamp(void)
{
	Elevator_Down();
}

//OLED显示内容
void OLED_Show(void)
{
	
}







