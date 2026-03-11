#ifndef __CONTROL_H
#define __CONTROL_H

#include "Grayscale_Sensor.h"
#include "pid.h"

//PID参数（待调试）
#define TURN_KP      15.0f
#define TURN_KI       0.0f
#define TURN_KD       0.8f
#define INTEGRAL_LIMIT 100.0f
#define OUTPUT_LIMIT   300.0f

//转向环初始化
void TurnControl_Init(void);
//计算转向环输出
float TurnControl_GetOutput(void);

#endif
