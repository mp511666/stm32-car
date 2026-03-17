#ifndef __POINT_TURN_H
#define __POINT_TURN_H

#define TURN_PWM_DIFF   200     // 转向时左右轮差速PWM值
#define TURN_TIME_MS    500      // 转向持续时间（ms）

// 启动转向，pc为当前点计数（偶数点时调用）
void Turn_Start(uint16_t pc);
// 转向计时处理函数，需每10ms调用一次
void Turn_Handler(void);

#endif
