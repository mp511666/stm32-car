#ifndef __PID_H
#define __PID_H

typedef struct
{
	float Kp, Ki, Kd;
    float integral;
    float last_error;
    float integral_limit;   // 积分限幅
    float output_limit;     // 输出限幅
}PID;

//初始化PID
void PID_Init(PID *pid, float Kp, float Ki, float Kd, float ilimit, float olimit);
//计算PID
float PID_Calc(PID *pid, float error);
#endif
