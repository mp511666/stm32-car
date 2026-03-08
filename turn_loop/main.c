#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Motor.h"
#include "Key.h"
#include "Timer.h"
#include "Control.h"

//PWM限幅
#define PWM_MAX   1000
#define PWM_MIN  -1000

//获取平均PWM（由角度环+速度环产生）
extern float Get_Average_PWM(void);
//电机PWM设置函数（假设使用TIM1等）
extern void SetMotorPWM(int left, int right);

int main(void)
{
	//初始化转向环和定时器
    TurnControl_Init();
	Timer_Init();

	while(1)
	{
		//蓝牙，声光提示待定
	}
}

//定时中断（假设TIM2每10ms触发）
void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
        //获取差分PWM
        float turn = TurnControl_GetOutput();
        
        //获取平均PWM（pid数值上等价于PWM）
        float avg = Get_Average_PWM();
        
        //计算左右轮PWM——强制类型转换
        int left  = (int)(avg + turn/2);
		int right = (int)(avg - turn/2);
        
        //PWM限幅
        if (left  > PWM_MAX) 
			left  = PWM_MAX;
        if (left  < PWM_MIN) 
			left  = PWM_MIN;
        if (right > PWM_MAX) 
			right = PWM_MAX;
        if (right < PWM_MIN) 
			right = PWM_MIN;
		
        //输出
        SetMotorPWM(left, right);
		
		//清除中断标志位
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }
}
