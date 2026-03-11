#include "stm32f10x.h"                  // Device header
#include "Pid.h"

//初始化
void PID_Init(PID *pid, float Kp, float Ki, float Kd, float ilimit, float olimit)
{
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->integral = 0;
    pid->last_error = 0;
    pid->integral_limit = ilimit;
    pid->output_limit = olimit;
}

//计算PID(误差已算好直接输入)
float PID_Calc(PID *pid, float error)
{
	//积分累加
	pid->integral += error;
	
	//积分限幅——防止积分项无限增长，避免系统因积分饱和而失控
	if (pid->integral > pid->integral_limit) 
		pid->integral = pid->integral_limit;
    if (pid->integral < -pid->integral_limit) 
		pid->integral = -pid->integral_limit;
	
	//计算PID总体输出
	float out = pid->Kp * error + pid->Ki * pid->integral + pid->Kd * (error - pid->last_error);
	pid->last_error = error;//误差回传，必须放在计算后面
	
	//输出限幅
	if (out > pid->output_limit) 
		out = pid->output_limit;
    if (out < -pid->output_limit) 
		out = -pid->output_limit;
	
	return out;
}
