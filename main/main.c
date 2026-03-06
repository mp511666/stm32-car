#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "LED.h"
#include "Timer.h"
#include "Key.h"
#include "MPU6050.h"
#include "Motor.h"
#include "Encoder.h"
#include "Serial.h"
#include "BlueSerial.h"
#include "pid.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>

/*MPU6050测试*/
int16_t AX, AY, AZ, GX, GY, GZ;
uint8_t TimerErrorFlag;
uint16_t TimerCount;

int16_t leftpwm,rightpwm;
uint16_t avepwm,difpwm;      //全局变量定义输出的pwm（用于角度环）

float leftspeed,rightspeed; //输出的平均速度和差分速度（用于速度环）
float avespeed,difspeed;

float AngleAcc;
float AngleGyro;
float Angle;

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

int main(void)
{
	OLED_Init();
	MPU6050_Init();
	BlueSerial_Init ();
	LED_Init ();
	Key_Init ();
	Motor_Init ();
	Encoder_Init ();
	Serial_Init ();
	
	Timer_Init();
	
	OLED_Printf (0,0,OLED_8X16,"Speed Control");	
	while (1)
	{
		if(RunFlag ){LED_ON() ;} else {LED_OFF ();}
		
		KeyNum =Key_GetNum ();
		if(KeyNum ==1)
		{
			
			if(RunFlag==0)
			{
					pid_init(&anglepid);
					pid_init(&speedpid);
					RunFlag=1;
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
		
		/*蓝牙串口数据，不理解*/
		if(BlueSerial_RxFlag ==1)
		{
			char *Tag = strtok(BlueSerial_RxPacket, ",");	//提取数据1，定义为标签Tag
			if (strcmp(Tag, "key") == 0)					//Tag为key，收到按键数据包
			{
				char *Name = strtok(NULL, ",");				//提取数据2，定义为按键名称
				char *Action = strtok(NULL, ",");			//提取数据3，定义为按键动作
				
				/*此处可执行按键操作，目前程序暂时没用到按键，这段何意味。*/
			}
			else if (strcmp(Tag, "slider") == 0)			//Tag为slider，收到滑杆数据包
			{
				char *Name = strtok(NULL, ",");				//提取数据2，定义为滑杆名称
				char *Value = strtok(NULL, ",");			//提取数据3，定义为滑杆值
				
				/*执行滑杆操作*/
				/*调整angle环比例系数*/
				if (strcmp(Name, "anglekp") == 0)			//如果滑杆名称是AngleKp
				{
					anglepid .kp = atof(Value);				//则把滑杆值赋值给角度环Kp
				}
				else if (strcmp(Name, "angleki") == 0)		//如果滑杆名称是AngleKi
				{
					anglepid .ki = atof(Value);				//则把滑杆值赋值给角度环Ki
				}
				else if (strcmp(Name, "anglekd") == 0)		//如果滑杆名称是AngleKd
				{
					anglepid .kd = atof(Value);				//则把滑杆值赋值给角度环Kd
				}
				/*调整speed环比例系数*/
				if (strcmp(Name, "speedkp") == 0)			
				{
					speedpid  .kp = atof(Value);				
				}
				else if (strcmp(Name, "speedki") == 0)		
				{
					speedpid  .ki = atof(Value);				
				}
				else if (strcmp(Name, "speedkd") == 0)		
				{
					speedpid  .kd = atof(Value);				
				}
			}
			else if (strcmp(Tag, "joystick") == 0)			//Tag为joystick，收到摇杆数据包
			{
				int8_t LH = atoi(strtok(NULL, ","));		//提取数据2，定义为摇杆值LH
				int8_t LV = atoi(strtok(NULL, ","));		//提取数据3，定义为摇杆值LV
				int8_t RH = atoi(strtok(NULL, ","));		//提取数据4，定义为摇杆值RH
				int8_t RV = atoi(strtok(NULL, ","));		//提取数据5，定义为摇杆值RV
				
				/*执行摇杆操作*/
				speedpid .target  = LV / 25.0;		//摇杆值LV缩放后，控制角度环目标值，前后行进控制
				difpwm  = RH / 2;				//摇杆值RH缩放后，控制差分PWM，左右转弯控制
			}
			
			BlueSerial_RxFlag = 0;				//处理完成后，标志位置0，允许接收下一个数据包
		}
		
		/*蓝牙串口打印波形，需配合蓝牙串口小程序实现波形绘制*/
		BlueSerial_Printf("[plot,%f,%f]", anglepid .target, Angle);	//绘制角度环的波形(随调整的环进行改变)
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
			
			MPU6050_GetData(&AX, &AY, &AZ, &GX, &GY, &GZ);
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
				pid_update (&anglepid );
				avepwm=-anglepid .out;
				
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
				rightspeed =Encoder_Get (1)/44.0/0.05/9.27666; //转换公式
			
				avespeed=(leftspeed+rightspeed)/2.0;
				difspeed =leftspeed -rightspeed ;
			
				if(RunFlag)
				{
						speedpid .actual =avespeed;
						pid_update(&speedpid );
						anglepid.target=speedpid.out;
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

