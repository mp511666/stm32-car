#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Motor.h"
#include "Key.h"
#include "Timer.h"
#include "Control.h"
#include "BlueTooth.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>


//PWM限幅
#define PWM_MAX   1000
#define PWM_MIN  -1000

//获取平均PWM（由角度环+速度环产生）
extern float Get_Average_PWM(void);
//电机PWM设置函数（假设使用TIM1等）
extern void SetMotorPWM(int left, int right);
//获取角度环和速度环的PID的系数
extern float ANGLE_KP,ANGLE_KI,ANGLE_KD,SPEED_KP,SPEED_KI,SPEED_KD;

int main(void)
{
	//初始化转向环和定时器
    TurnControl_Init();
	Timer_Init();

	while(1)
	{
		//蓝牙，声光提示待定
		
		//如果连续发送数据包，程序处理不及时，可能导致数据包错位，利用RxFlag实现，不采取读取Flag之后立刻清除的策略
		if(BlueTooth_RxFlag == 1)
		{
			//strtok：可实现用指定字符分割字符串，第一个参数给待分割的字符串，第二个参数指定用什么字符来分割这个字符串（注意要用“”），返回值用char*接收，只需在第一次分割时，传入待分割的字符串，后续分割时，写NULL,表示这是前面的后续分割，而不是开启一个新的分割
			//strcmp：可以必对两个字符串是否相等，相等返回0
			//atoi/atof：可以把字符串形式的数值转化为整型/浮点型
			
			char* Tag=strtok(BlueTooth_RxPacket,",");
			//按键——选择路线
			if(strcmp(Tag,"key")==0)
			{
				char* Name=strtok(NULL,",");
				
				if(strcmp(Name,"Line1")==0)
				{
					printf("Line1\r\n");
				}
				else if(strcmp(Name,"Line2")==0)
				{
					printf("Line2\r\n");
				}
				else if(strcmp(Name,"Line3")==0)
				{
					printf("Line3\r\n");
				}
				else if(strcmp(Name,"Line4")==0)
				{
					printf("Line4\r\n");
				}
			}
			//滑杆——调节pid参数
			else if(strcmp(Tag,"slider")==0)
			{
				char* Name=strtok(NULL,",");
				char* Value=strtok(NULL,",");
				
				if(strcmp(Name,"ANGLE_KP")==0)
				{
					float FloatValue=atof(Value);//atof返回double类型，这里会进行隐式类型转换
					printf("ANGLE_KP=%f\r\n",FloatValue);
				}
				else if(strcmp(Name,"ANGLE_KI")==0)
				{
					float FloatValue=atof(Value);//atof返回double类型，这里会进行隐式类型转换
					printf("ANGLE_KI=%f\r\n",FloatValue);
				}
				else if(strcmp(Name,"ANGLE_KD")==0)
				{
					float FloatValue=atof(Value);//atof返回double类型，这里会进行隐式类型转换
					printf("ANGLE_KD=%f\r\n",FloatValue);
				}
				else if(strcmp(Name,"SPEED_KP")==0)
				{
					float FloatValue=atof(Value);//atof返回double类型，这里会进行隐式类型转换
					printf("SPEED_KP=%f\r\n",FloatValue);
				}
				else if(strcmp(Name,"SPEED_KI")==0)
				{
					float FloatValue=atof(Value);//atof返回double类型，这里会进行隐式类型转换
					printf("SPEED_KI=%f\r\n",FloatValue);
				}
				else if(strcmp(Name,"SPEED_KD")==0)
				{
					float FloatValue=atof(Value);//atof返回double类型，这里会进行隐式类型转换
					printf("SPEED_KD=%f\r\n",FloatValue);
				}
				else if(strcmp(Name,"TURN_KP")==0)
				{
					float FloatValue=atof(Value);//atof返回double类型，这里会进行隐式类型转换
					printf("TURN_KP=%f\r\n",FloatValue);
				}
				else if(strcmp(Name,"TURN_KI")==0)
				{
					float FloatValue=atof(Value);//atof返回double类型，这里会进行隐式类型转换
					printf("TURN_KI=%f\r\n",FloatValue);
				}
				else if(strcmp(Name,"TURN_KD")==0)
				{
					float FloatValue=atof(Value);//atof返回double类型，这里会进行隐式类型转换
					printf("TURN_KD=%f\r\n",FloatValue);
				}
			}
			BlueTooth_RxFlag = 0;//置0，这个数据包接收完成，可以继续下一个了
		}
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
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}
