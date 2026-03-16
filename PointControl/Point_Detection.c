#include "stm32f10x.h"                  // Device header
#include "Point_Detection.h"

//点检测初始化
void PointDetection_Init(void)
{
    stable_level = WHITE_LEVEL;     // 设定当前稳定色为起点颜色（白色）
    change_cnt = 0;                 // 清零变化计数器
}

//点检测更新函数：每10ms调用一次
void PointDetection_Update(void)
{
    // 读取中间传感器的当前电平（0或1）
    uint8_t cur = Grayscale_Sensor_ReadMid();

    // 判断当前电平是否与稳定电平不同
    if (cur != stable_level) // 不同：说明可能进入了新的颜色区域
	{
        change_cnt++;

        // 如果连续多次都读到相反电平，则认为颜色确实稳定改变了
        if (change_cnt >= CHANGE_THRESHOLD) 
		{
            // 到达了一个点！调用路线管理函数处理点事
			// 标志位Flag=2，车停，灯闪，鸣叫

            // 更新稳定电平为新的颜色
            stable_level = cur;

            // 清零变化计数器，准备下一次检测
            change_cnt = 0;
        }
    } 
	else // 与稳定电平相同：说明之前的异动可能是噪声，清零计数器
	{
        
        change_cnt = 0;
    }
}
