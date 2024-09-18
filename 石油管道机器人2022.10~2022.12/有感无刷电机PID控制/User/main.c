#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/TIMER/bldc_tim.h"
#include "./BSP/KEY/key.h"
#include "./BSP/BLDC/bldc.h"
#include "pid.h"
#include "./DEBUG/debug.h"
#include "./BSP/ADC/adc.h"


extern float*user_setpoint;
extern float debug_data_temp;
extern int32_t motor_pwm_s;
extern int32_t temp_pwm1;
extern int16_t adc_amp_un[3];
extern float  adc_amp_bus;
extern uint16_t g_adc_value[ADC_CH_NUM * ADC_COLL];
extern uint8_t clc;
void bldc_speed_stop(void);                  /* 清除电机状态并关闭电机 */

int main(void)
{
    uint8_t debug_cmd = 0;                   /* 存放上位机指令 */
    float current_lpf[4] = {0.0f};           /* 存放三相电流以及母线电流 */
    uint8_t key,t;
    char buf[32];
    float current[3] = {0.0f};
    int16_t speed_diplay = 0;
    float user_setpoint_temp  = 0.0;

    HAL_Init();                              /* 初始化HAL库 */
		sys_stm32_clock_init(RCC_PLL_MUL9);         /* 设置时钟, 72Mhz */
    delay_init(168);                         /* 延时初始化 */
    usart_init(115200);                      /* 串口初始化为115200 */
    led_init();                              /* 初始化LED */
    key_init();                              /* 初始化按键 */
    bldc_init(72000/18-1,0);
    bldc_ctrl(MOTOR_1,CCW,0);                /* 初始无刷电机接口1速度 */
    pid_init();
   
    adc_nch_dma_init();
		

    
#if DEBUG_ENABLE                                                /* 开启调试*/
    debug_init();                                               /* PID调试初始化*/
    debug_send_motorcode(BLDC_MOTOR );                          /* 直流无刷电机 */
    debug_send_motorstate(IDLE_STATE);                          /* 电机空闲 */
    /* 初始化同步数据（选择第x组PIDX,目标速度地址,P,I,D参数）到上位机 */
    debug_send_initdata(TYPE_PID1,(float*)(&g_speed_pid.SetPoint),C_KP,C_KI,C_KD);    /* 电流环PID参数（PID1）*/
    debug_send_initdata(TYPE_PID2,(float*)(&g_speed_pid.SetPoint),S_KP,S_KI,S_KD);    /* 速度环PID参数（PID2）*/
#endif

    while (1)
    {
        t++;
			if(t % 60 == 0)
        {
            printf("PWM_Duty:%.1f%%",(float)((g_bldc_motor1.pwm_duty/MAX_PWM_DUTY)*100));/* 显示控制PWM占空比 */
  
            printf("SetSpeed:%4d   ",(int16_t)*user_setpoint);     /* 显示设置速度 */
  
            printf("M1 Speed:%4d   ",(int16_t)g_bldc_motor1.speed); /* 显示测量速度 */
            
            printf("M1 pos:%4d",g_bldc_motor1.pos);                /* 显示位置变化 */
            
            printf("Power:%.3fV ",g_adc_value[0]*ADC2VBUS);				/* 显示电源电压 */
            
            printf("Temp:%.1fC ",get_temp(g_adc_value[1]));				/* 显示温度 */
           
            LED0_TOGGLE();                                              /* LED0(红灯) 翻转 */

            current[0] = adc_amp_un[0] * ADC2CURT; /* U */
            current[1] = adc_amp_un[1] * ADC2CURT; /* V */
            current[2] = adc_amp_un[2] * ADC2CURT; /* W */

            /* 一阶数字滤波 滤波系数0.1 用于显示 */
            FirstOrderRC_LPF(current_lpf[0],current[0],0.1f);
            FirstOrderRC_LPF(current_lpf[1],current[1],0.1f);
            FirstOrderRC_LPF(current_lpf[2],current[2],0.1f);
            FirstOrderRC_LPF(current_lpf[3],adc_amp_bus,0.1f);
            if(g_bldc_motor1.run_flag == STOP)                          /* 停机的电流显示 */
            {
                current_lpf[0]=0;
                current_lpf[1]=0;
                current_lpf[2]=0;
                current_lpf[3]=0;
            }
           
        }
				
				
				
        key = key_scan(0);
        if(key == KEY0_PRES)                                    /* 按下KEY0目标速度值++ */
        {
            g_bldc_motor1.run_flag = RUN;                       /* 开启运行 */
            start_motor1();                                     /* 开启运行 */

            if(g_bldc_motor1.dir == CCW && *user_setpoint == 0) /* 切换方向条件*/
            {
                g_bldc_motor1.dir = CW; 
            }
            *user_setpoint += 400;                              /* 逆时针旋转下递增 */
            if(*user_setpoint >= 3000)                          /* 最高不超过3000PRM */
                *user_setpoint = 3000;
            if(*user_setpoint == 0)
            {
                pid_init();                                     /* 初始化PID */
                g_bldc_motor1.run_flag = STOP;                  /* 标记停机 */
                stop_motor1();                                  /* 停机 */
                g_bldc_motor1.speed = 0;
                motor_pwm_s = 0;
                g_bldc_motor1.pwm_duty = 0;
            }
            debug_data_temp = *user_setpoint;
        }
        else if(key == KEY1_PRES)                               /* 按下KEY1目标速度值-- */
        {
            g_bldc_motor1.run_flag = RUN;                       /* 开启运行 */
            start_motor1();                                     /* 运行电机 */
            /* 切换方向条件 */
            if(g_bldc_motor1.dir == CW && *user_setpoint == 0)
            {
                g_bldc_motor1.dir = CCW;
            }
            *user_setpoint -= 400;                              /* 逆时针旋转下递增 */
            if(*user_setpoint <= -3000)                         /* 最高不超过300PRM */
                *user_setpoint = -3000;
            if(*user_setpoint == 0)
            {
                pid_init();                                     /* 初始化PID */
                g_bldc_motor1.run_flag = STOP;                  /* 标记停机 */
                stop_motor1();                                  /* 停机 */
                g_bldc_motor1.speed = 0;
                motor_pwm_s = 0;
                g_bldc_motor1.pwm_duty = 0;
            }

            debug_data_temp = *user_setpoint;
        }
        else if(key == KEY2_PRES)                               /* 按下KEY2关闭电机 */
        {
            bldc_speed_stop();
        }
        
#if DEBUG_ENABLE
        /* Debug发送部分 */
        /* 主要显示参数 */
        debug_send_valtage(g_adc_value[0] * ADC2VBUS);              /* 发送电压 */
        debug_send_speed(g_bldc_motor1.speed);                      /* 发送速度 */
        debug_send_temp(50,get_temp(g_adc_value[1]));               /* 发送电机温度、驱动板温度 */
        debug_send_current((float)(current_lpf[0]/1000),(float)(current_lpf[0]/1000),(float)(current_lpf[0]/1000)); /* 发送电流 */
        /* 电流波形和速度波形 */
        debug_send_wave_data(1,(int16_t)g_bldc_motor1.speed);           /* 选择通道1 发送实际速度 */
        debug_send_wave_data(2,(int16_t)*user_setpoint);                /* 选择通道2 发送目标速度 */ 
        debug_send_wave_data(3,current_lpf[0]);                         /* 选择通道3 发送实际电流U */
        debug_send_wave_data(4,current_lpf[1]);                         /* 选择通道4 发送实际电流V */
        debug_send_wave_data(5,current_lpf[2]);                         /* 选择通道5 发送实际电流W */
        debug_send_wave_data(7,current_lpf[3]);                         /* 选择通道7 发送实际电流 */
        debug_send_wave_data(8,g_current_pid.SetPoint);                 /* 选择通道8 发送目标电流 */
        /* Debug接收部分 */
       debug_receive_pid(TYPE_PID1,(float*)&g_current_pid.Proportion,  /* 查询接收PID助手的PID1参数*/
                          (float*)&g_current_pid.Integral,(float*)&g_current_pid.Derivative);
			 
       debug_cmd = debug_receive_ctrl_code();                      /* 读取命令 */
				
				
        if(debug_cmd == HALT_CODE)                                  /* 停机 */
        {
            pid_init();                                             /* 重新初始化PID，防止积分过大失控 */
            g_bldc_motor1.run_flag = STOP;                          /* 标记停机 */
            stop_motor1();                                          /* 停机 */
            g_bldc_motor1.speed = 0;
            motor_pwm_s = 0;
            g_bldc_motor1.pwm_duty = 0;
        }
        else if(debug_cmd == RUN_CODE)                              /* 运行 */
        {
            g_bldc_motor1.dir = CW;
            g_bldc_motor1.run_flag = RUN;                           /* 运行标记 */
            *user_setpoint = 400;                                   /* 自动设置目标 */
            start_motor1();                                         /* 启动电机 */
            debug_send_motorstate(RUN_STATE);                       /* 电机运行 */
        }
        else if (debug_cmd == BREAKED)                              /* 刹车（电机停止 点击电机运行才可解除） */
        {
            *user_setpoint = 0;                                     /* 减速直至0 */
            debug_send_motorstate(BREAKED_STATE);                   /* 电机刹车 */
        }
#endif
       delay_ms(10); 
    }
}
/**
 * @brief       清除电机状态并关闭电机
 * @param       无
 * @retval      无
 */
void bldc_speed_stop(void)
{
    pid_init();                     /* 重新初始化PID，防止积分过大失控 */
    g_bldc_motor1.run_flag = STOP;  /* 标记停机 */
    stop_motor1();                  /* 停机 */
    g_bldc_motor1.speed = 0;
    motor_pwm_s = 0;
    g_bldc_motor1.pwm_duty = 0;
}


