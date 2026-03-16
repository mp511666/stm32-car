#include "stm32f10x.h"                  // Device header
#include "Grayscale_Sensor.h"

//引脚初始化
void Grayscale_Sensor_Init(void)
{
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA ,ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入——为灰度传感器（开漏型传感器模块）提供内部上拉
	GPIO_InitStructure.GPIO_Pin = GRAYSCALE_SENSOR_ALL_PINS;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
}

//读取5路灰度传感器的值——黑1白0
void Grayscale_Sensor_Read(uint8_t *data)
{
	uint16_t val = GPIO_ReadInputData (GRAYSCALE_SENSOR_PORT);//一次性读取整个端口,得到一个uint16_t的数值，它二进制上的每一位，对应每一个GPIOA口的电平状态，0为低电平，1为高电平
	
	//val & GRAYSCALE_SENSORX_PIN能得到对应位的数值，再通过条件运算符将非零值转换为0，零值转换为0。相比于连续调用5次 GPIO_ReadInputDataBit更快更紧凑，适用于实现高频控制循环
	data[0] = (val & GRAYSCALE_SENSOR1_PIN) ? 1 : 0;
	data[1] = (val & GRAYSCALE_SENSOR2_PIN) ? 1 : 0;
	data[2] = (val & GRAYSCALE_SENSOR3_PIN) ? 1 : 0;
	data[3] = (val & GRAYSCALE_SENSOR4_PIN) ? 1 : 0;
	data[4] = (val & GRAYSCALE_SENSOR5_PIN) ? 1 : 0;
}

//读取中间传感器的高低电平——借电平变化来实现确定指定点
uint8_t Grayscale_Sensor_ReadMid(void)
{
	return GPIO_ReadInputDataBit (GRAYSCALE_SENSOR_PORT,GRAYSCALE_SENSOR3_PIN);
}

//计算误差（由于远近不同，所以使用加权平均更合理）
float Grayscale_Sensor_GetError(void)
{
	uint8_t data[5];
	Grayscale_Sensor_Read(data);
	
	const int8_t weight[5]={-2,-1,0,1,2};//固定权重：左到右 -2 -1 0 1 2
	int32_t sum = 0;//32是处于安全和可扩展性
	uint8_t cnt = 0,i= 0;
	for(i=0;i<5;i++)
	{
		if(data[i] == BLACK_LEVEL)//检测到黑线
		{
			sum += weight[i];
			cnt++;
		
		}
	}
	
	float error;
	if(cnt == 0)
		error = 0;
	else
		error = (float)sum/cnt;//强制类型转换为浮点型

	return error;
}
