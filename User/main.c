//#include "stm32f10x.h"                  // Device header
//#include "Delay.h"
//#include "OLED.h"
//#include "LED.h"
//#include "Timer.h"
//#include "Key.h"
//#include "MPU6050.h"
//#include "Motor.h"
//#include "Encoder.h"
//#include "pid.h"
//#include "Beeper.h"
//#include "Grayscale_Sensor.h"
//#include "Point_Detection.h"
//#include "BlueTooth.h"
//#include "Point_Turn.h"
//#include <math.h>
//#include <string.h>
//#include <stdlib.h>


//#define ANGLE_MAX_LIMIT     45.0f   // 倒地自锁最大倾角
//#define STOP_TIME_COUNT     100     // 停车延时：100 * 10ms = 1s
//#define TARGET_TURN_ANGLE   37.5f   // 转向环目标角度
//#define GYRO_Z_OFFSET       0.0f    // Z轴陀螺仪零偏校准值（视实际情况调整）


//int16_t AX, AY, AZ, GX, GY, GZ;
//uint8_t TimerErrorFlag;
//uint16_t TimerCount;

//int16_t leftpwm,rightpwm;
//uint16_t avepwm,difpwm;      //全局变量定义输出的pwm（用于角度环）

//float leftspeed,rightspeed; //输出的平均速度和差分速度（用于速度环）
//float avespeed,difspeed;

//float AngleAcc,AngleGyro,Angle; //mpu6050读取角度
//float YawAngle = 0.0f;          // [AI修复] 新增偏航角变量，用于转向环

//uint8_t RunFlag;
//uint16_t Speed,PWM; //位置式pid

///*角度环*/
//pid_t anglepid={
//		.kp=3,
//		.ki=0.1,
//		.kd=3,
//	
//		.outmax=100,
//		.outmin=-100,

//};   //角度环结构体，最先调整

///*速度环*/
//pid_t speedpid={
//		.kp=2.0,
//		.ki=0.05,
//		.kd=0,
//	
//		.outmax=20,
//		.outmin=-20,

//}; 
///*位置环*/
//uint8_t movestate=0; //运动状态

//pid_t movepid={
//		.target=0,
//		.kp=0.5,
//		.ki=0,
//		.kd=0,
//		
//		.outmax=30,
//		.outmin=-30
//};
//#define slowdist 20.0f   //开始减速区域
//#define stopzone 2.0f    //视为到达
///*转向环*/
//pid_t turnpid={
//		.kp=2,
//		.ki=2,
//		.kd=2,
//	
//		.integral_limit=100,
//		.outmax=300,
//		.outmin=-300
//};
///*线路选择*/
//typedef struct
//{
//	float distance;
//	uint8_t circle;
//}Line;
//Line line;
//uint8_t linestate;
//uint8_t current_circle = 0; 

//void Blue_Process(void)
//{
//		if(BlueTooth_RxFlag == 1)
//		{
//			char* Tag=strtok(BlueTooth_RxPacket,",");
//			if(strcmp(Tag,"key")==0)
//			{
//				char* Name=strtok(NULL,",");
//				char* Distance=strtok(NULL,",");
//				char* Circle=strtok(NULL,",");
//				
//				current_circle = 0; 
//				YawAngle = 0.0f;   
//				
//				if(strcmp(Name,"Line1")==0&&strcmp(Distance,"100.0")==0&&strcmp(Circle,"1")==0)
//				{
//					line.distance = atof(Distance) ;
//					line.circle = atoi(Circle);
//					linestate =1;movestate=1;
//					printf("Line1,%f,%d\r\n",line.distance,line.circle);
//				}
//				else if(strcmp(Name,"Line2")==0&&strcmp(Distance,"451.2")==0&&strcmp(Circle,"1")==0)
//				{
//					line.distance = atof(Distance) ;
//					line.circle = atoi(Circle);
//					linestate=2;
//					printf("Line2,%f,%d\r\n",line.distance,line.circle);
//				}
//				else if(strcmp(Name,"Line3")==0&&strcmp(Distance,"507.2")==0&&strcmp(Circle,"1")==0)
//				{
//					line.distance = atof(Distance) ;
//					line.circle = atoi(Circle);
//					linestate=3;
//					printf("Line3,%f,%d\r\n",line.distance,line.circle);
//				}
//				else if(strcmp(Name,"Line4")==0&&strcmp(Distance,"2028.8")==0&&strcmp(Circle,"4")==0)
//				{
//					line.distance = atof(Distance) ;
//					line.circle = atoi(Circle);
//					linestate=4;
//					printf("Line4,%f,%d\r\n",line.distance,line.circle);
//				}
//			}

//			else if(strcmp(Tag,"slider")==0)
//			{
//				char* Name=strtok(NULL,",");
//				char* Value=strtok(NULL,",");
//				
//				if(strcmp(Name,"Angle_Kp")==0) { anglepid.kp = atof(Value); printf("Angle_Kp=%f\r\n",anglepid.kp); }
//				else if(strcmp(Name,"Angle_Ki")==0) { anglepid.ki = atof(Value); printf("Angle_Ki=%f\r\n",anglepid.ki); }
//				else if(strcmp(Name,"Angle_Kd")==0) { anglepid.kd = atof(Value); printf("Angle_Kd=%f\r\n",anglepid.kd); }
//				
//				else if(strcmp(Name,"Speed_Kp")==0) { speedpid.kp = atof(Value); printf("Speed_Kp=%f\r\n",speedpid.kp); }
//				else if(strcmp(Name,"Speed_Ki")==0) { speedpid.ki = atof(Value); printf("Speed_Ki=%f\r\n",speedpid.ki); }
//				else if(strcmp(Name,"Speed_Kd")==0) { speedpid.kd = atof(Value); printf("speedpid.kd=%f\r\n",speedpid.kd); }
//				
//				else if(strcmp(Name,"Move_Kp")==0) { movepid.kp = atof(Value); printf("Move_Kp=%f\r\n",movepid.kp); }
//				else if(strcmp(Name,"Move_Ki")==0) { movepid.ki = atof(Value); printf("Move_Ki=%f\r\n",movepid.ki); }
//				else if(strcmp(Name,"Move_Kd")==0) { movepid.kd = atof(Value); printf("Move_Kd=%f\r\n",movepid.kd); }
//				
//				else if(strcmp(Name,"Turn_Kp")==0) { turnpid.kp = atof(Value); printf("Turn_Kp=%f\r\n",turnpid.kp); }
//				else if(strcmp(Name,"Turn_Ki")==0) { turnpid.ki = atof(Value); printf("Turn_Ki=%f\r\n",turnpid.ki); }
//				else if(strcmp(Name,"Turn_Kd")==0) { turnpid.kd = atof(Value); printf("Turn_Kd=%f\r\n",turnpid.kd); }
//			}
//			BlueTooth_RxFlag = 0;
//		
//		/*OLED显示*/
//		OLED_Clear();
//		OLED_Printf(0, 0, OLED_6X8, "angle:");
//		OLED_Printf(0, 8, OLED_6X8, "P:%05.2f",anglepid .kp);
//		OLED_Printf(0, 16, OLED_6X8, "I:%05.2f", anglepid.ki );
//		OLED_Printf(0, 24, OLED_6X8, "D:%05.2f", anglepid .kd);
//		OLED_Printf(0, 32, OLED_6X8, "T:%+05.1f", anglepid .target);
//		OLED_Printf(0, 40, OLED_6X8, "A:%+05.1f", Angle);
//		OLED_Printf(0, 48, OLED_6X8, "O:%+05.0f", anglepid .out);
//		OLED_Printf(0, 56, OLED_6X8, "GY:%+05d", GY);
//		
//		OLED_Printf(50, 0,OLED_6X8, "speed");
//		OLED_Printf(50, 8, OLED_6X8, "%05.2f",speedpid .kp);
//		OLED_Printf(50, 16, OLED_6X8, "%05.2f", speedpid.ki );
//		OLED_Printf(50, 24, OLED_6X8, "%05.2f", speedpid .kd);
//		OLED_Printf(50, 32, OLED_6X8, "%+05.1f", speedpid .target);
//		OLED_Printf(50, 40, OLED_6X8, "%+05.1f", avespeed );
//		OLED_Printf(50, 48, OLED_6X8, "%+05.0f", speedpid .out);
//		OLED_Update();
//	}
//}

//int main(void)
//{
//	OLED_Init();
//	mpu6050_init();
//	LED_Init ();
//	Key_Init ();
//	Motor_Init ();
//	Encoder_Init ();
//	Timer_Init();
//	Beeper_init();
//	Grayscale_Sensor_Init();
//	PointDetection_Init();
//	pid_init(&anglepid);
//	pid_init(&speedpid);
//	pid_init (&movepid );
//	pid_init(&turnpid);
//	RunFlag=1;
//	movestate =0;
//	movepid.target = 0; 
//  speedpid.target = 0; 
//	
//	while (1)
//	{
//		if(RunFlag ){LED_ON() ;} else {LED_OFF ();}
//		Blue_Process();
//	}
//}
//		
//float TurnControl_GetOutput(void)
//{
//    float error = Grayscale_Sensor_GetError();         
//    return PID_Calc(&turnpid, error);
//}

//static uint16_t StopTimer = 0;   
//static uint8_t WayPoint=1;//ABCD点位

//void TIM1_UP_IRQHandler(void)
//{
//	static uint16_t Count0=0;
//	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
//	{
//	 	  TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
//	  	Beeper_Process();	
//			if(StopTimer > 0) StopTimer--;
//			
//			/*姿态解算*/
//			mpu6050_GetData(&AX, &AY, &AZ, &GX, &GY, &GZ);
//			GY-=16;   //GY存在零漂，手动给其偏移,后续需要自己调
//		
//			AngleAcc =-atan2(AX,AZ)/3.14159*180;				
//			AngleAcc+=0.5;     
//			AngleGyro =Angle +GY*32768.0*2000*0.01; 
//		
//			float Alpha=0.01 ;
//			Angle=Alpha*AngleAcc + (1-Alpha)*AngleGyro;

//        //积分计算偏航角 (Yaw)，16.4f 为 MPU6050 +/-2000deg/s 量程的灵敏度参数
//      YawAngle += (GZ - GYRO_Z_OFFSET) / 16.4f * 0.01f; 

//			/*倒地保护*/
//			if(Angle>ANGLE_MAX_LIMIT || Angle<-ANGLE_MAX_LIMIT) 
//			{
//				RunFlag=0;
//				movestate=0;
//				Motor_SetPWM (1,0);
//				Motor_SetPWM (2,0);
//			}
//			if(!RunFlag ) 
//			{
//				Motor_SetPWM (1,0);
//				Motor_SetPWM (2,0);
//				return; 
//			}
//			
//			/*通过高低电平检测是否达到停车点*/
//			if(PointDetection_Update())
//			{
//				WayPoint++;
//				movestate=2;
//				movepid.actual = 0; 
//			}
//			if(RunFlag)
//			{
//				if(linestate >= 1 && linestate <= 4 && movestate == 2)
//				{
//					anglepid.target = 0; 
//					difpwm = 0;

//        // 处理声光提示与延时
//					if(StopTimer == 0)
//					{
//            Beeper_start();
//            LED_ON();
//            StopTimer = 100; //1 秒延时
//					}
//					else if(StopTimer == 1) // 倒计时结束，准备重新起步
//					{	
//            LED_OFF();
//            movestate = 1;
//            movepid.actual = 0; 
//            speedpid.eri = 0;  
//					}	
//				}
//				else 
//				{	
//        // 正常行驶状态
//					if(movestate == 1) { anglepid.target = speedpid.out; }
//					else { anglepid.target = 0; }
//        
//					if(linestate == 1 || linestate == 2)
//					{
//            difpwm = TurnControl_GetOutput();
//					}
//					else if(linestate == 3 || linestate == 4)
//					{
//            if(WayPoint == 1 || WayPoint == 3)
//            {
//                pid_update(&turnpid);
//                difpwm = turnpid.out;
//            }
//            else if(WayPoint == 2 || WayPoint == 4)
//            {
//                difpwm = TurnControl_GetOutput();
//            }
//					}
//				}
//				anglepid.actual = Angle;
//				pid_update(&anglepid);
//				avepwm = -anglepid.out;    
//            
//				leftpwm = avepwm + difpwm / 2;
//				rightpwm = avepwm - difpwm / 2;       
//    
//    // 严格限幅
//				if(leftpwm > 100) { leftpwm = 100; } else if(leftpwm < -100) { leftpwm = -100; }
//				if(rightpwm > 100) { rightpwm = 100; } else if(rightpwm < -100) { rightpwm = -100; }
//            
//    // 时刻输出，维持动态平衡
//				Motor_SetPWM(1, leftpwm);
//				Motor_SetPWM(2, rightpwm);	
//			}
//			
//			
//	}
//	Count0++;
//	if(Count0 >=5)   //读取编码器的时间间隔：50ms
//	{
//			Count0=0;
//			leftspeed =Encoder_Get (1)/44.0/0.05/9.27666;
//			rightspeed =Encoder_Get (2)/44.0/0.05/9.27666; 
//			
//			avespeed=(leftspeed+rightspeed)/2.0;
//			difspeed =leftspeed -rightspeed ;
//			
//			if(RunFlag)
//			{
//				if(linestate==1)
//				{
//					if(movestate==1)
//					{
//						movepid.target=line.distance;	
//						movepid .actual  += avespeed * 0.05f;
//						
//						if(fabs(movepid .er0)<=stopzone ) 
//						{
//							movepid .out=0;
//							movestate=2;
//							Beeper_start ();
//							LED_ON ();
//							StopTimer = 100; 
//							LED_OFF ();
//						}
//						else
//						{
//							if(movepid .er0 >slowdist ) { movepid .out=movepid .outmax ; }
//							else 
//							{
//								movepid.kp =movepid.outmax /slowdist ;
//								pid_update(&movepid);
//								if (movepid.out  > movepid.outmax ) {movepid.out = movepid.outmax;}
//							}
//						}
//						speedpid.target=movepid .out;
//						speedpid .actual =avespeed;
//						pid_update(&speedpid );
//					}
//				}
//				
//				if(linestate==2)
//				{
//					if (movestate==1) 
//					{
//						movepid.target=line.distance;	
//						movepid .actual  += avespeed * 0.05f;
//                        
//						if(fabs(movepid .er0)<=stopzone ) 
//						{
//							movepid .out=0;
//							Beeper_start ();
//							LED_ON ();
//							StopTimer = STOP_TIME_COUNT; 
//							LED_OFF ();
//							movepid.actual=0;
//						}
//						else 
//						{
//							if(movepid .er0 >slowdist ) { movepid .out=movepid .outmax ; }
//							else 
//							{
//								movepid.kp =movepid.outmax /slowdist ;
//								pid_update(&movepid);
//								if (movepid.out  > movepid.outmax ) {movepid.out = movepid.outmax;}
//							}
//						}
//						speedpid.target=movepid .out;
//						speedpid .actual =avespeed;
//						pid_update(&speedpid );
//					}
//                    // 到达最后一站(A点)停车
//					if(movestate==2 && WayPoint>=5) 
//					{
//						RunFlag =0;
//					}
//				}

//				if(linestate==3)
//				{
//					if(WayPoint==1)
//					{
//						turnpid.target = TARGET_TURN_ANGLE;
//						turnpid.actual = YawAngle;          
//						movepid.target=line.distance;	
//						movepid .actual  += avespeed * 0.05f;
//						if(fabs(movepid .er0)<=stopzone ) { movepid .out=0; movepid.actual=0; }
//						else 
//						{
//							if(movepid .er0 >slowdist ) { movepid .out=movepid .outmax ; }
//							else {
//								movepid.kp =movepid.outmax /slowdist ; pid_update(&movepid);
//								if (movepid.out  > movepid.outmax ) {movepid.out = movepid.outmax;}
//							}
//						}
//						speedpid.target=movepid .out; speedpid .actual =avespeed; pid_update(&speedpid );
//					}
//					if(WayPoint==3)
//					{
//						turnpid.target = -TARGET_TURN_ANGLE; 
//            turnpid.actual = YawAngle;
//						movepid.target=line.distance;	
//						movepid .actual  += avespeed * 0.05f;
//						if(fabs(movepid .er0)<=stopzone ) { movepid .out=0; movepid.actual=0; }
//						else 
//						{
//							if(movepid .er0 >slowdist ) { movepid .out=movepid .outmax ; }
//							else {
//								movepid.kp =movepid.outmax /slowdist ; pid_update(&movepid);
//								if (movepid.out  > movepid.outmax ) {movepid.out = movepid.outmax;}
//							}
//						}
//						speedpid.target=movepid .out; 
//						speedpid .actual =avespeed; 
//						pid_update(&speedpid );
//					}
//					if(WayPoint>=5)
//					{
//						RunFlag=0;
//					}
//				}

//				if(linestate==4)
//				{
//					if(WayPoint==1)
//					{
//						turnpid.target = TARGET_TURN_ANGLE;
//            turnpid.actual = YawAngle; 
//						movepid.target=line.distance;	
//						movepid .actual  += avespeed * 0.05f;
//						if(fabs(movepid .er0)<=stopzone ) { movepid .out=0; movepid.actual=0; }
//						else 
//						{
//							if(movepid .er0 >slowdist ) { movepid .out=movepid .outmax ; }
//							else {
//								movepid.kp =movepid.outmax /slowdist ; pid_update(&movepid);
//								if (movepid.out  > movepid.outmax ) {movepid.out = movepid.outmax;}
//							}
//						}
//						speedpid.target=movepid .out; 
//						speedpid .actual =avespeed; 
//						pid_update(&speedpid );
//					}
//					if(WayPoint==3)
//					{
//						turnpid.target = -TARGET_TURN_ANGLE;
//            turnpid.actual = YawAngle;
//						movepid.target=line.distance;	
//						movepid .actual  += avespeed * 0.05f;
//						if(fabs(movepid .er0)<=stopzone ) { movepid .out=0; movepid.actual=0; }
//						else 
//						{
//							if(movepid .er0 >slowdist ) { movepid .out=movepid .outmax ; }
//							else 
//							{
//								movepid.kp =movepid.outmax /slowdist ; 
//								pid_update(&movepid);
//								if (movepid.out  > movepid.outmax ) {movepid.out = movepid.outmax;}
//							}
//						}
//						speedpid.target=movepid .out; 
//						speedpid .actual =avespeed; 
//						pid_update(&speedpid );
//					}
//					if(WayPoint >= 5)
//					{
//            WayPoint = 1;
//						current_circle++;
//            Beeper_start();
//            LED_ON();
//            StopTimer = STOP_TIME_COUNT;
//            LED_OFF();
//            if(current_circle >= line.circle) 
//            {
//							Motor_SetPWM(1, 0);
//              Motor_SetPWM(2, 0);
//              RunFlag = 0;
//            }
//					}
//				}
//			}
//		}
//}

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
#include "Point_Detection.h"
#include "BlueTooth.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define ANGLE_MAX_LIMIT     45.0f   // 倒地自锁最大倾角

int16_t AX, AY, AZ, GX, GY, GZ;
int16_t leftpwm, rightpwm;
int16_t avepwm, difpwm;     
float leftspeed, rightspeed, avespeed, difspeed;
float AngleAcc, AngleGyro, Angle; 

uint8_t RunFlag;
uint8_t OLED_Refresh_Flag = 0; 

// PID 结构体初始化
pid_t anglepid={ .kp=0, .ki=0, .kd=0.0, .outmax=100, .outmin=-100,.eri=0 };   
pid_t speedpid={ .kp=0, .ki=0, .kd=0, .outmax=10,  .outmin=-10,.target=0 }; 

// 蓝牙历史数据监控
char LastRxData[20] = "No Data"; 

// ==========================================
// 蓝牙与 OLED 处理函数
// ==========================================
void Blue_Process(void)
{
    if(BlueTooth_RxFlag == 1)
    {
        // 安全的字符串拷贝，防止乱码
        int i = 0;
        for(i = 0; i < 19 && BlueTooth_RxPacket[i] != '\0'; i++)
        {
            LastRxData[i] = BlueTooth_RxPacket[i];
        }
        LastRxData[i] = '\0';

        char* Tag = strtok(BlueTooth_RxPacket, ",");
        if(Tag != NULL && (strcmp(Tag, "slider") == 0 || strcmp(Tag, "[slider") == 0))
        {
            char* Name = strtok(NULL, ",");
            char* Value = strtok(NULL, ",]"); 
            
            if(Name != NULL && Value != NULL)
            {
                float val = atof(Value);
                if(strcmp(Name, "anglekp") == 0)      { anglepid.kp = val; }
                else if(strcmp(Name, "angleki") == 0) { anglepid.ki = val; }
                else if(strcmp(Name, "anglekd") == 0) { anglepid.kd = val; }
                else if(strcmp(Name, "speedkp") == 0) { speedpid.kp = val; }
                else if(strcmp(Name, "speedki") == 0) { speedpid.ki = val; }
                else if(strcmp(Name, "speedkd") == 0) { speedpid.kd = val; }
            }
        }
        BlueTooth_RxFlag = 0; 
    }
    
    // OLED 刷新
    if(OLED_Refresh_Flag == 1)
    {
        OLED_Refresh_Flag = 0;
        OLED_Clear();
        
        OLED_Printf(0, 0,  OLED_6X8, "Ang:%+05.1f", Angle); 
        OLED_Printf(0, 16, OLED_6X8, "Out:%+05.1f", anglepid.out); 
        
        // 极性诊断
        if (Angle > 2.0f && anglepid.out > 1.0f)        OLED_Printf(0, 32, OLED_6X8, "Pol: OK+");
        else if (Angle < -2.0f && anglepid.out < -1.0f) OLED_Printf(0, 32, OLED_6X8, "Pol: OK-");
        else if (fabs(Angle) > 5.0f)                    OLED_Printf(0, 32, OLED_6X8, "Pol: ERR!");
				OLED_Printf(0, 16, OLED_6X8, "Spd:%+05.1f", avespeed);
        OLED_Printf(64, 16, OLED_6X8, "P:%05.2f", anglepid.kp);
        OLED_Printf(0, 56, OLED_6X8, "Rx:%s", LastRxData);
        
        OLED_Update();
    }
}

// ==========================================
// 主函数 (大门)
// ==========================================
int main(void)
{
    // 1. 硬件基础初始化
    LED_Init();
    OLED_Init();
		LED_ON();
    
    // 2. 关键传感器与执行器初始化
    mpu6050_init();   
    Motor_Init();     
    Encoder_Init();   
    Serial_Init();    
    
    // 3. PID 结构体初始化
    pid_init(&anglepid);
    pid_init(&speedpid);
    
    // 4. 开启定时器中断
    Timer_Init(); 
    
    RunFlag = 1;
    speedpid.target = 0; 

    while (1)
    {
			Blue_Process();
    }
}
        
// ==========================================
// 10ms 核心控制中断
// ==========================================
void TIM1_UP_IRQHandler(void)
{
    static uint16_t Count0 = 0;
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
    {
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
        
        /* 1. 姿态解算 (已修正正负号与轴向) */
        mpu6050_GetData(&AX, &AY, &AZ, &GX, &GY, &GZ);
        
        AngleAcc = -atan2(AY, AZ) / 3.14159f * 180.0f;                
        AngleAcc -=5.0f; 
        
        AngleGyro = Angle - (GX / 16.4f) * 0.01f;
        
        float Alpha = 0.02f;
        Angle = Alpha * AngleAcc + (1.0f - Alpha) * AngleGyro;
        
        /* 2. 刷新标志位与速度获取 */
        Count0++;
        if(Count0 >= 5)   
        {
            Count0 = 0;
            OLED_Refresh_Flag = 1; 
            
            leftspeed = Encoder_Get(1) / 44.0 / 0.05 / 9.27666;
            rightspeed = Encoder_Get(2) / 44.0 / 0.05 / 9.27666; 
            avespeed = (leftspeed + rightspeed) / 2.0;
            speedpid.actual = avespeed;
            if(RunFlag) { pid_update(&speedpid); }
        }
        
        /* 3. 安全自锁与复活机制 */
        if(Angle > ANGLE_MAX_LIMIT || Angle < -ANGLE_MAX_LIMIT)
        {
            RunFlag = 0;
            Motor_SetPWM(1, 0);
            Motor_SetPWM(2, 0);
        }
        else
        {
            RunFlag = 1;  
        }
                
        if(!RunFlag) return; 
        
        /* 4. PID 计算 */
        anglepid.actual = Angle;
       // 直立环目标 = 速度环输出
				anglepid.target = 0; 
// 必须用这个专属公式！直接把陀螺仪数据 (GX/16.4) 喂给 Kd！
anglepid.out = anglepid.kp * (Angle - anglepid.target) + anglepid.kd * (GX / 16.4f);

avepwm = -(int16_t)anglepid.out; // 加上负号恢复动力输出！ 
        difpwm = 0; 

        leftpwm = avepwm + difpwm / 2;
        rightpwm = avepwm - difpwm / 2;      
        
        /* 5. 死区补偿 (解决电机低速不动) */
        if(leftpwm > 0) { leftpwm += 8; }
        else if(leftpwm < 0) { leftpwm -= 8; }
        
        if(rightpwm > 0) { rightpwm += 8; }
        else if(rightpwm < 0) { rightpwm -= 8; }
//                
        /* 6. 输出限幅与电机驱动 */
        if(leftpwm > 100) { leftpwm = 100; } else if(leftpwm < -100) { leftpwm = -100; }
        if(rightpwm > 100) { rightpwm = 100; } else if(rightpwm < -100) { rightpwm = -100; }
        
        Motor_SetPWM(1, leftpwm);
        Motor_SetPWM(2, rightpwm);
    }
}
