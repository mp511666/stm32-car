#include "stm32f10x.h"                  // Device header
#include "Point_Turn.h"

static uint16_t turn_timer = 0;   // 转向剩余时间（ms），0表示未在转向
static uint8_t turn_direction = 0; // 1:右转，0:左转

//启动转向
void Turn_Start(uint16_t pc)
{
    // 只有偶数点（A、B）才转向
    if ((pc % 2) != 0) 
		return;

    // 根据点计数确定方向：pc=0/4/8... 在A点，右转；pc=2/6/10... 在B点，左转
    turn_direction = ((pc / 2) % 2 == 0) ? 1 : 0; // 1右转，0左转

    // 设置计时器
    turn_timer = TURN_TIME_MS;

    // 启动电机转向（注意电机极性可能需要调整符号）
    if (turn_direction==1)
    {
        // 右转
        //SetMotorPWM(TURN_PWM_DIFF, -TURN_PWM_DIFF);
    }
    else
    {
        // 左转
        //SetMotorPWM(-TURN_PWM_DIFF, ++TURN_PWM_DIFF);
    }
}

//定时减少控制转向时间
void Turn_Handler(void)
{
    if (turn_timer == 0) 
		return;

    turn_timer -= 10;   // 假设每10ms调用一次
    if (turn_timer <= 0)
    {
        // 转向完成，停车
        //SetMotorPWM(0, 0);
		//换标志位，前进
    }
}
