#include "stm32f10x.h"                  // Device header

//初始化
void Timer_Init(void)
{
	//开启时钟
	RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM2 ,ENABLE);
	//选择时基单元的时钟
	TIM_InternalClockConfig(TIM2);//调用该函数，则通用定时器TIM2的时基单元就由内部时钟来驱动
	//配置时基单元
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;//时钟分频，目的是为了滤波，与时基单元关系不大，随便配一个
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数
	//CK_CNT（计数器计数频率） = CK_PSC（72MHz）/(PSC + 1)
	//CK_CNT_OV（定时频率/计数器溢出频率） = CK_CNT /(ARR + 1) = CK_PSC（72MHz）/(PSC + 1)/(ARR + 1)
	//ARR和PSC都是161位，所以需在0-65535
	//此处定时10ms，即为100Hz
	TIM_TimeBaseInitStructure.TIM_Period = 7200-1;//ARR自动重装器的值
	TIM_TimeBaseInitStructure.TIM_Prescaler = 100-1;//PSC预分频器的值
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;//重复计数器的值，高级定时器才有，通用计时器没有
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);
	//手动把更新中断标志位清除一下，避免刚初始化完就进入中断
	TIM_ClearFlag (TIM2, TIM_IT_Update );
	//使能更新中断
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	//NVIC
	NVIC_PriorityGroupConfig (NVIC_PriorityGroup_2 );
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;//找到TIM2的中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;//响应优先级
	NVIC_Init (&NVIC_InitStructure);
	//启动定时器
	TIM_Cmd(TIM2,ENABLE);
}

