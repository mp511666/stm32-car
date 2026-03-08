#include "stm32f10x.h"                  // Device header
#include "Control.h"

//转向环pid——static提高封装性和安全性
static PID turn_pid;

//转向环初始化
void TurnControl_Init(void)
{
    Grayscale_Sensor_Init();
    PID_Init(&turn_pid, TURN_KP, TURN_KI, TURN_KD, INTEGRAL_LIMIT, OUTPUT_LIMIT);
}

//转向环pid输出(差分PWM)
float TurnControl_GetOutput(void)
{
    float error = Grayscale_Sensor_GetError();         
    return PID_Calc(&turn_pid, error);//PID 输出（经过限幅后）和 PWM 值使用同一套“刻度尺”，所以它们数值上可以直接当作彼此使用，而这个等价是建立在“输出限幅范围与 PWM 范围匹配”的前提下的。       
}
