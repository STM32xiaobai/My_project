#ifndef __BLDC_TIM_H
#define __BLDC_TIM_H

#include "./SYSTEM/sys/sys.h"

/******************************************************************************************/
/* 高级定时器 定义 */

/* TIM_CHx通道 上桥臂IO定义 */
#define ATIM_TIMX_PWM_CH1_GPIO_PORT            GPIOE
#define ATIM_TIMX_PWM_CH1_GPIO_PIN             GPIO_PIN_9
#define ATIM_TIMX_PWM_CH1_GPIO_CLK_ENABLE()    do{  __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)    /* PA口时钟使能 */

#define ATIM_TIMX_PWM_CH2_GPIO_PORT            GPIOE
#define ATIM_TIMX_PWM_CH2_GPIO_PIN             GPIO_PIN_11
#define ATIM_TIMX_PWM_CH2_GPIO_CLK_ENABLE()    do{  __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)    /* PA口时钟使能 */

#define ATIM_TIMX_PWM_CH3_GPIO_PORT            GPIOE
#define ATIM_TIMX_PWM_CH3_GPIO_PIN             GPIO_PIN_13
#define ATIM_TIMX_PWM_CH3_GPIO_CLK_ENABLE()    do{  __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)    /* PA口时钟使能 */

/* 下桥臂IO定义 */
#define M1_LOW_SIDE_U_PORT                      GPIOE										//注意
#define M1_LOW_SIDE_U_PIN                       GPIO_PIN_8
#define M1_LOW_SIDE_U_GPIO_CLK_ENABLE()         do{  __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)    /* PB口时钟使能 */、、

#define M1_LOW_SIDE_V_PORT                      GPIOE
#define M1_LOW_SIDE_V_PIN                       GPIO_PIN_10
#define M1_LOW_SIDE_V_GPIO_CLK_ENABLE()         do{  __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)    /* PB口时钟使能 */

#define M1_LOW_SIDE_W_PORT                      GPIOE
#define M1_LOW_SIDE_W_PIN                       GPIO_PIN_12
#define M1_LOW_SIDE_W_GPIO_CLK_ENABLE()         do{  __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)    /* PB口时钟使能 */

/* 复用到TIM1 */
#define ATIM_TIMX_PWM_CHY_GPIO_AF              GPIO_AF1_TIM1

#define ATIM_TIMX_PWM                          TIM1
#define ATIM_TIMX_PWM_IRQn                     TIM1_UP_TIM10_IRQn
#define ATIM_TIMX_PWM_IRQHandler               TIM1_UP_TIM10_IRQHandler
#define ATIM_TIMX_PWM_CH1                      TIM_CHANNEL_1                               /* 通道1 */
#define ATIM_TIMX_PWM_CH2                      TIM_CHANNEL_2                               /* 通道2 */
#define ATIM_TIMX_PWM_CH3                      TIM_CHANNEL_3                               /* 通道3 */
#define ATIM_TIMX_PWM_CHY_CLK_ENABLE()         do{ __HAL_RCC_TIM1_CLK_ENABLE(); }while(0)  /* TIM1 时钟使能 */


extern TIM_HandleTypeDef g_atimx_handle;                                                    /* 定时器x句柄 */
/******************************************************************************************/

/******************************* 基本定时器 定义 *************************************/

#define BTIM_TIMX_INT                           TIM2
#define BTIM_TIMX_INT_IRQn                      TIM2_IRQn
#define BTIM_TIMX_INT_IRQHandler                TIM2_IRQHandler
#define BTIM_TIMX_INT_CLK_ENABLE()              do{ __HAL_RCC_TIM2_CLK_ENABLE(); }while(0)  /* TIM6 时钟使能 */

//extern TIM_HandleTypeDef g_atimx_handle;                    /* 定时器x句柄 */
/******************************************************************************************/

void atim_timx_oc_chy_init(uint16_t arr, uint16_t psc);     /* 高级定时器 PWM初始化函数 */
void btim_timx_int_init(uint16_t arr, uint16_t psc);        /* 基本定时器中断初始化 */

#endif

















