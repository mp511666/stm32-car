#ifndef __GRAYSCALE_SENSOR_H
#define __GRAYSCALE_SENSOR_H

//灰度传感器引脚的宏定义
#define GRAYSCALE_SENSOR1_PIN        GPIO_Pin_0
#define GRAYSCALE_SENSOR2_PIN        GPIO_Pin_1
#define GRAYSCALE_SENSOR3_PIN        GPIO_Pin_2
#define GRAYSCALE_SENSOR4_PIN        GPIO_Pin_3
#define GRAYSCALE_SENSOR5_PIN        GPIO_Pin_4
#define GRAYSCALE_SENSOR_ALL_PINS    (GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4)
#define GRAYSCALE_SENSOR_PORT        GPIOA

//电平定义（检测到黑线为1，高电平；白线为0，低电平）
#define BLACK_LEVEL 1
#define WHITE_LEVEL 0

//初始化
void Grayscale_Sensor_Init(void);
//读取数据
void Grayscale_Sensor_Read(uint8_t *data);
//读取中间传感器的高低电平
uint8_t Grayscale_Sensor_ReadMid(void);
//计算误差
float Grayscale_Sensor_GetError(void);

#endif
