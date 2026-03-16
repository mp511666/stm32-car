#ifndef __POINT_DETECTION_H
#define __POINT_DETECTION_H

#include "Grayscale_Sensor.h"

// 变化确认阈值：连续多少次读到相反电平才认为颜色真的改变了，假设每10ms调用一次，CHANGE_THRESHOLD=10 表示需要连续100ms的稳定变化才触发
#define CHANGE_THRESHOLD  10
// 当前稳定的电平（0对应白色,或1对应黑色）
static uint8_t stable_level;     
// 连续读到相反电平的次数
static uint8_t change_cnt;   

void PointDetection_Init(void);
void PointDetection_Update(void);

#endif
