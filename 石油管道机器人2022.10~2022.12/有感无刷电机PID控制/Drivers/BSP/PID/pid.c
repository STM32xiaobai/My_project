#include "./BSP/PID/pid.h"
#include "./BSP/BLDC/bldc.h"

PID_TypeDef  g_current_pid;        /* 位置PID参数结构体 */
PID_TypeDef  g_speed_pid;           /* 速度PID参数结构体 */
/**
 * @brief       初始化PID
 * @param       无
 * @retval      无
 */
void pid_init(void)
{
    /* 设定电流目标1500mA(空载最小电流400mA左右)较高的转速对应的电流较大，力矩较大，可适应较高的转速调节*/
    /* 【注意】如设置的转速对应的电流超过了电流设定值，将导致PID转至电流环调节，转速将无法继续提升 */
    g_current_pid.SetPoint = 1000.0;
    g_current_pid.ActualValue = 0.0;    /* 设定目标Desired Value */
    g_current_pid.LastError = 0.0;      /* Error[1] */
    g_current_pid.LastError = 0.0;      /* Error[-1] */
    g_current_pid.PrevError = 0.0;      /* Error[-2] */
    g_current_pid.Proportion = C_KP;    /* 比例常数 Proportional Const */
    g_current_pid.Integral = C_KI;      /* 积分常数 Integral Const */
    g_current_pid.Derivative = C_KD;    /* 微分常数 Derivative Const */
    g_current_pid.IngMax = 9000;
    g_current_pid.IngMin = 600;
    g_current_pid.OutMin = 600;
    g_current_pid.OutMax = 9000;
    
    g_speed_pid.SetPoint = 0;           /* 设定目标Desired Value */
    g_speed_pid.ActualValue = 0.0;      /* 设定目标Desired Value */
    g_speed_pid.Ui = 0.0;
    g_speed_pid.Up = 0.0;
    g_speed_pid.Ud = 0.0;
    g_speed_pid.Error = 0.0;            /* Error[1] */
    g_speed_pid.LastError = 0.0;        /* Error[-1] */
    g_speed_pid.PrevError = 0.0;        /* Error[-2] */
    g_speed_pid.Proportion = S_KP;      /* 比例常数 Proportional Const */
    g_speed_pid.Integral = S_KI;        /* 积分常数 Integral Const */
    g_speed_pid.Derivative = S_KD;      /* 微分常数 Derivative Const */ 
    g_speed_pid.IngMax = 9000;
    g_speed_pid.IngMin = -9000;
    g_speed_pid.OutMax = 9000;          /* 输出限制 */
    g_speed_pid.OutMin = -9000;
}


/**
 * @brief       位置式PID算法
 * @param       *PID：PID结构体句柄所对应的目标值
 * @param       Feedback_value ： 实际值
 * @retval      目标控制量
 */
int32_t increment_pid_ctrl(PID_TypeDef *PID,float Feedback_value)
{
    PID->Error = (float)(PID->SetPoint - Feedback_value);   /* 目标值与实际值的偏差值 */
    PID->Up = PID->Proportion * PID->Error;
    PID->Ui += (PID->Error * PID->Integral);
    LIMIT_OUT(PID->Ui,PID->IngMax,PID->IngMin);             /* 积分限制 */
    PID->Ud = PID->Derivative * (PID->Error - PID->LastError);
    PID->ActualValue = PID->Up + PID->Ui + PID->Ud;
    LIMIT_OUT(PID->ActualValue,PID->OutMax,PID->OutMin);    /* 输出限制 */
    PID->LastError = PID->Error;                            /* 存储上次误差，以便下次计算使用 */
    return ((int32_t)(PID->ActualValue));                   /* 返回实际控制数值 */
}

///**
// * @brief       闭环PID控制算法设计
// * @note        通过宏 INCR_LOCT_SELECT 选择使用位置式算法/增量式算法
// * @param       *PID：PID结构体句柄所对应的目标值
// * @param       Feedback_value ： 实际值
// * @retval      目标控制量
// */
//int32_t increment_pid_ctrl(PID_TypeDef *PID,float Feedback_value)
//{
//    PID->Error = (float)(PID->SetPoint - Feedback_value);                   /* 速度档位偏差*/
//#if  INCR_LOCT_SELECT
//    PID->ActualValue += (PID->Proportion * (PID->Error - PID->LastError))   /* E[k]项*/
//                        + (PID->Integral * PID->Error)                      /* E[k-1]项*/
//                        + (PID->Derivative * (PID->Error - 2 * PID->LastError + PID->PrevError)); /*E[k-2]项*/
//    PID->PrevError = PID->LastError;                                        /* 存储误差，用于下次计算*/
//    PID->LastError = PID->Error;
//#else
//    PID->SumError += PID->Error;
//    PID->ActualValue = (PID->Proportion * PID->Error)                       /* E[k]项*/
//                       + (PID->Integral * PID->SumError)                    /* E[k-1]项*/
//                       + (PID->Derivative * (PID->Error - PID->LastError)); /* E[k-2]项*/
//    PID->LastError = PID->Error;
//#endif
//    return ((int32_t)(PID->ActualValue));                                   /* 返回实际控制数值*/
//}
