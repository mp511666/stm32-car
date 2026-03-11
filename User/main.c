#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "LED.h"
#include "Timer.h"
#include "Key.h"
#include "MPU6050.h"
#include "Motor.h"
#include "Encoder.h"
#include "pid.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>

int16_t AX, AY, AZ, GX, GY, GZ;
uint8_t TimerErrorFlag;
uint16_t TimerCount;

int16_t leftpwm,rightpwm;
uint16_t avepwm,difpwm;      //全局变量定义输出的pwm（用于角度环）

float leftspeed,rightspeed; //输出的平均速度和差分速度（用于速度环）
float avespeed,difspeed;

float AngleAcc,AngleGyro,Angle; //mpu6050读取角度

uint8_t KeyNum,RunFlag;
uint16_t Speed,PWM; //位置式pid

pid_t anglepid={
		.kp=3,
		.ki=0.1,
		.kd=3,
	
		.outmax=100,
		.outmin=-100,

};   //角度环结构体，最先调整

pid_t speedpid={
		.kp=2.0,
		.ki=0.05,
		.kd=0,
	
		.outmax=20,
		.outmin=-20,

}; 

/*位置环*/

uint8_t movestate=0; //运动状态

pid_t movepid={
		.target=100,
		.kp=0.5,
		.ki=0,
		.kd=0,
		
		.outmax=30,
		.outmin=-30
};
#define slowdist 20.0f   //开始减速区域
#define stopzone 2.0f    //视为到达


int main(void)
{
	OLED_Init();
	mpu6050_init();
	LED_Init ();
	Key_Init ();
	Motor_Init ();
	Encoder_Init ();
	Timer_Init();
	
	OLED_Printf (0,0,OLED_8X16,"Speed Control");	
	while (1)
	{
		if(RunFlag ){LED_ON() ;} else {LED_OFF ();}
		
		KeyNum =Key_GetNum ();
		if(KeyNum ==1)
		{
			/*这是什么？*/
			if(RunFlag==0)
			{
					pid_init(&anglepid);
					pid_init(&speedpid);
					pid_init (&movepid );
					RunFlag=1;
					movestate =1;
			}
			else
			{
					RunFlag=0;
			}
		}
		/*OLED显示*/
		OLED_Clear();
		/*显示angle环*/
		OLED_Printf(0, 0, OLED_6X8, "angle:");
		OLED_Printf(0, 8, OLED_6X8, "P:%05.2f",anglepid .kp);
		OLED_Printf(0, 16, OLED_6X8, "I:%05.2f", anglepid.ki );
		OLED_Printf(0, 24, OLED_6X8, "D:%05.2f", anglepid .kd);
		OLED_Printf(0, 32, OLED_6X8, "T:%+05.1f", anglepid .target);
		OLED_Printf(0, 40, OLED_6X8, "A:%+05.1f", Angle);
		OLED_Printf(0, 48, OLED_6X8, "O:%+05.0f", anglepid .out);
		OLED_Printf(0, 56, OLED_6X8, "GY:%+05d", GY);
		/*显示speed环*/
		OLED_Printf(50, 0,OLED_6X8, "speed");
		OLED_Printf(50, 8, OLED_6X8, "%05.2f",speedpid .kp);
		OLED_Printf(50, 16, OLED_6X8, "%05.2f", speedpid.ki );
		OLED_Printf(50, 24, OLED_6X8, "%05.2f", speedpid .kd);
		OLED_Printf(50, 32, OLED_6X8, "%+05.1f", speedpid .target);
		OLED_Printf(50, 40, OLED_6X8, "%+05.1f", avespeed );
		OLED_Printf(50, 48, OLED_6X8, "%+05.0f", speedpid .out);
		OLED_Update();
		}	
}
		


void TIM1_UP_IRQHandler(void)
{
	static uint16_t Count,Count0;
	
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
		
		Key_Tick ();
		Count++;
		if(Count >=10)
		{
			Count=0;
			
			mpu6050_GetData(&AX, &AY, &AZ, &GX, &GY, &GZ);
			GY-=16;   //GY存在零漂，手动给其偏移,后续需要自己调
		
			AngleAcc =-atan2(AX,AZ)/3.14159*180;				//返回弧度值，转为角度值
			AngleAcc+=0.5;     //校准中心角度  后续需要自己调
			
			
			AngleGyro =Angle +GY*32768.0*2000*0.01; //角度值累加
		
			float Alpha=0.01 ;
			Angle=Alpha*AngleAcc + (1-Alpha)*AngleGyro;
		
			if(Angle>45||Angle<-45)
			{
				RunFlag=0;
			}
			
			if(RunFlag)
			{
				anglepid.actual =Angle;
				
				if(movestate ==1||movestate==2){anglepid.target=speedpid.out ;}
				else {anglepid .target =0;}
				
				pid_update (&anglepid );
				avepwm=-anglepid .out;    //调极性
				
				leftpwm =avepwm +difpwm/2;
				rightpwm=avepwm-difpwm/2;
				
				
				if(leftpwm>100){leftpwm =100;}else if(leftpwm<-100){leftpwm =-100;}
				if(rightpwm >100){rightpwm =100;}else if(rightpwm<-100){rightpwm =-100;}
				
				Motor_SetPWM (1,leftpwm);
				Motor_SetPWM (2,rightpwm);
			
			}
			else 
			{
				Motor_SetPWM (1,0);
				Motor_SetPWM (2,0);
			}
		}
		
		Count0 ++;
		if(Count0 >=50)   //读取编码器的时间间隔：50ms
		{
				Count0=0;
				leftspeed =Encoder_Get (1)/44.0/0.05/9.27666;
				rightspeed =Encoder_Get (2)/44.0/0.05/9.27666; //转换公式
			
				avespeed=(leftspeed+rightspeed)/2.0;
				difspeed =leftspeed -rightspeed ;
				
				if (RunFlag && (movestate== 1 ||movestate == 2)) 
			{
					movepid .actual  += avespeed * 0.05f; 
			}
				if(RunFlag ==1&&movestate==1)
				{
					if(fabs(movepid .er0)<=stopzone ) //到达减速区
					{
							movepid .out=0;
							movestate=2;
					}
					else if(fabs(movepid .er0)>stopzone ) //未跑过头
					{
							if(movepid .er0 >slowdist )
							{
								movepid .out=movepid .outmax ;
							}
							else 
							{
								movepid.kp =movepid.outmax /slowdist ;
								pid_update(&movepid);
								if (movepid.out  > movepid.outmax ) {movepid.out = movepid.outmax;}
							}
					}
					else
					{
							movepid.out=0;
							movestate=2;
					}
					
					speedpid.target=movepid .out;
				}
				if(RunFlag)
				{
						speedpid .actual =avespeed;
						pid_update(&speedpid );
						
				}
		}
		
		
		if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
		{
			TimerErrorFlag = 1;
			TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
		}
		TimerCount = TIM_GetCounter(TIM1);
	}
}

