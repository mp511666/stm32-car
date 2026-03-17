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
#include "Beeper.h"
#include "Grayscale_Sensor.h"
#include "BlueTooth.h"
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

uint8_t RunFlag;
uint16_t Speed,PWM; //位置式pid

/*角度环*/
pid_t anglepid={
		.kp=3,
		.ki=0.1,
		.kd=3,
	
		.outmax=100,
		.outmin=-100,

};   //角度环结构体，最先调整

/*速度环*/
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
		.target=0,
		.kp=0.5,
		.ki=0,
		.kd=0,
		
		.outmax=30,
		.outmin=-30
};
#define slowdist 20.0f   //开始减速区域
#define stopzone 2.0f    //视为到达
/*转向环*/
pid_t turnpid={
		.kp=2,
		.ki=2,
		.kd=2,
	
		.integral_limit=100,
		.outmax=300,
		.outmin=-300
};
/*线路选择*/
typedef struct
{
	float distance;
	uint8_t circle;
}Line;
Line line;
uint8_t linestate;
void Blue_Process(void)
{
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
				char* Distance=strtok(NULL,",");
				char* Circle=strtok(NULL,",");
				
				if(strcmp(Name,"Line1")==0&&strcmp(Distance,"100.0")==0&&strcmp(Circle,"1")==0)
				{
					line.distance = atof(Distance) ;
					line.circle = atoi(Circle);
					linestate =1;movestate=1;
					printf("Line1,%f,%d\r\n",line.distance,line.circle);
				}
				else if(strcmp(Name,"Line2")==0&&strcmp(Distance,"451.2")==0&&strcmp(Circle,"1")==0)
				{
					line.distance = atof(Distance) ;
					line.circle = atoi(Circle);
					linestate=2;
					printf("Line2,%f,%d\r\n",line.distance,line.circle);
				}
				else if(strcmp(Name,"Line3")==0&&strcmp(Distance,"507.2")==0&&strcmp(Circle,"1")==0)
				{
					line.distance = atof(Distance) ;
					line.circle = atoi(Circle);
					linestate=3;
					printf("Line3,%f,%d\r\n",line.distance,line.circle);
				}
				else if(strcmp(Name,"Line4")==0&&strcmp(Distance,"2028.8")==0&&strcmp(Circle,"4")==0)
				{
					line.distance = atof(Distance) ;
					line.circle = atoi(Circle);
					linestate=4;
					printf("Line4,%f,%d\r\n",line.distance,line.circle);
				}
			}

			//滑杆——调节pid参数
			else if(strcmp(Tag,"slider")==0)
			{
				char* Name=strtok(NULL,",");
				char* Value=strtok(NULL,",");
				//角度环
				if(strcmp(Name,"Angle_Kp")==0)
				{
					anglepid.kp = atof(Value);//atof返回double类型，这里会进行隐式类型转换
					printf("Angle_Kp=%f\r\n",anglepid.kp);
				}
				else if(strcmp(Name,"Angle_Ki")==0)
				{
					anglepid.ki = atof(Value);//atof返回double类型，这里会进行隐式类型转换
					printf("Angle_Ki=%f\r\n",anglepid.ki);
				}
				else if(strcmp(Name,"Angle_Kd")==0)
				{
					anglepid.kd = atof(Value);//atof返回double类型，这里会进行隐式类型转换
					printf("Angle_Kd=%f\r\n",anglepid.kd);
				}
				//速度环
				else if(strcmp(Name,"Speed_Kp")==0)
				{
					speedpid.kp = atof(Value);//atof返回double类型，这里会进行隐式类型转换
					printf("Speed_Kp=%f\r\n",speedpid.kp);
				}
				else if(strcmp(Name,"Speed_Ki")==0)
				{
					speedpid.ki = atof(Value);//atof返回double类型，这里会进行隐式类型转换
					printf("Speed_Ki=%f\r\n",speedpid.ki);
				}
				else if(strcmp(Name,"Speed_Kd")==0)
				{
					speedpid.kd = atof(Value);//atof返回double类型，这里会进行隐式类型转换
					printf("speedpid.kd=%f\r\n",speedpid.kd);
				}
				//位置环
				else if(strcmp(Name,"Move_Kp")==0)
				{
					movepid.kp = atof(Value);//atof返回double类型，这里会进行隐式类型转换
					printf("Move_Kp=%f\r\n",movepid.kp);
				}
				else if(strcmp(Name,"Move_Ki")==0)
				{
					movepid.ki = atof(Value);//atof返回double类型，这里会进行隐式类型转换
					printf("Move_Ki=%f\r\n",movepid.ki);
				}
				else if(strcmp(Name,"Move_Kd")==0)
				{
					movepid.kd = atof(Value);//atof返回double类型，这里会进行隐式类型转换
					printf("Move_Kd=%f\r\n",movepid.kd);
				}
				//转向环
				else if(strcmp(Name,"Turn_Kp")==0)
				{
					turnpid.kp = atof(Value);//atof返回double类型，这里会进行隐式类型转换
					printf("Turn_Kp=%f\r\n",turnpid.kp);
				}
				else if(strcmp(Name,"Turn_Ki")==0)
				{
					turnpid.ki = atof(Value);//atof返回double类型，这里会进行隐式类型转换
					printf("Turn_Ki=%f\r\n",turnpid.ki);
				}
				else if(strcmp(Name,"Turn_Kd")==0)
				{
					turnpid.kd = atof(Value);//atof返回double类型，这里会进行隐式类型转换
					printf("Turn_Kd=%f\r\n",turnpid.kd);
				}
			}
			BlueTooth_RxFlag = 0;//置0，这个数据包接收完成，可以继续下一个了
		
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

int main(void)
{
	OLED_Init();
	mpu6050_init();
	LED_Init ();
	Key_Init ();
	Motor_Init ();
	Encoder_Init ();
	Timer_Init();
	Beeper_init();
	Grayscale_Sensor_Init();
	pid_init(&anglepid);
	pid_init(&speedpid);
	pid_init (&movepid );
	pid_init(&turnpid);
	RunFlag=1;
	movestate =0;
	movepid.target = 0; // 位置目标归零
  speedpid.target = 0; // 速度目标归零
	
	while (1)
	{
		if(RunFlag ){LED_ON() ;} else {LED_OFF ();}
		Blue_Process();
	}
}
		
float TurnControl_GetOutput(void)
{
    float error = Grayscale_Sensor_GetError();         
    return PID_Calc(&turnpid, error);//PID 输出（经过限幅后）和 PWM 值使用同一套“刻度尺”，所以它们数值上可以直接当作彼此使用，而这个等价是建立在“输出限幅范围与 PWM 范围匹配”的前提下的。       
}

static uint8_t last_level=0;
static uint16_t StopTimer = 0;   // 停车计时
//static uint16_t BeepTimer=0;
static uint8_t WayPoint=1; //ABCD点位
void TIM1_UP_IRQHandler(void)
{
	static uint16_t Count0=0;
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
	{
	 	  TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	  	Beeper_Process();	
			if(StopTimer > 0) StopTimer--;
			
			/*姿态解算*/
			mpu6050_GetData(&AX, &AY, &AZ, &GX, &GY, &GZ);
			GY-=16;   //GY存在零漂，手动给其偏移,后续需要自己调
		
			AngleAcc =-atan2(AX,AZ)/3.14159*180;				//返回弧度值，转为角度值
			AngleAcc+=0.5;     //校准中心角度  后续需要自己调
			AngleGyro =Angle +GY*32768.0*2000*0.01; //角度值累加
		
			float Alpha=0.01 ;
			Angle=Alpha*AngleAcc + (1-Alpha)*AngleGyro;
			/*倒地保护*/
			if(Angle>45||Angle<-45)
			{
				RunFlag=0;
				movestate=0;
				Motor_SetPWM (1,0);
				Motor_SetPWM (2,0);
			}
			if(!RunFlag ) 
			{
				Motor_SetPWM (1,0);
				Motor_SetPWM (2,0);
				return;
			}
			/*通过高低电平检测是否达到停车点*/
			uint8_t current_level=GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6);
			 
			if(current_level!=last_level)
			{
				WayPoint++;
				movestate=2;
				last_level=current_level;
			}
			
			if(RunFlag)
			{
				anglepid.actual =Angle;
				if(movestate ==1){anglepid.target=speedpid.out ;}
				else {anglepid .target =0;}
				
				difpwm = TurnControl_GetOutput();//获取差分pwm
				pid_update (&anglepid );
				avepwm=-anglepid .out;    //调极性
				
				leftpwm =avepwm +difpwm/2;
				rightpwm=avepwm-difpwm/2;       
				if(leftpwm>100){leftpwm =100;}else if(leftpwm<-100){leftpwm =-100;}
				if(rightpwm >100){rightpwm =100;}else if(rightpwm<-100){rightpwm =-100;}
				
				Motor_SetPWM (1,leftpwm);
				Motor_SetPWM (2,rightpwm);
			}
	}
		Count0++;
		if(Count0 >=5)   //读取编码器的时间间隔：50ms
		{
				Count0=0;
				leftspeed =Encoder_Get (1)/44.0/0.05/9.27666;
				rightspeed =Encoder_Get (2)/44.0/0.05/9.27666; //转换公式
			
				avespeed=(leftspeed+rightspeed)/2.0;
				difspeed =leftspeed -rightspeed ;
			
			if(RunFlag)
			{
					if(linestate==1)
				{
					if(movestate==1)
					{
							movepid.target=line.distance;	
							movepid .actual  += avespeed * 0.05f;
						
						if(fabs(movepid .er0)<=stopzone ) //准备跑过头
						{
							movepid .out=0;
							movestate=2;
							StopTimer = 200;    // 停 2 秒
              Beeper_start();
							RunFlag=0;
						}
						
						else//未跑过头
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
						speedpid.target=movepid .out;
						speedpid .actual =avespeed;
						pid_update(&speedpid );
					}
				}
				
				if(linestate==2)
				{
					if ((WayPoint==1||WayPoint ==3)&&movestate==1) 
					{
						movepid.target=line.distance;	
						movepid .actual  += avespeed * 0.05f;
						if(fabs(movepid .er0)<=stopzone ) //准备跑过头
						{
							movepid .out=0;
							Beeper_start ();
							StopTimer = 2; // 停 1 秒
//             BeepTimer = 30;
							movepid.actual=0;
						}
						else //未跑过头
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
						speedpid.target=movepid .out;
						speedpid .actual =avespeed;
						pid_update(&speedpid );
					}
					if(movestate==2&&(WayPoint==2||WayPoint ==4))
					{
						Beeper_start ();
						StopTimer = 100; 
						movestate=1;
						speedpid.target =2; //目标速度自己调
						speedpid.actual =avespeed ;
						pid_update (&speedpid);
					}
					
					if(movestate==2&&WayPoint==5)
					{
						Beeper_start ();
						RunFlag =0;
					}
				}
			}
			
	}
}
