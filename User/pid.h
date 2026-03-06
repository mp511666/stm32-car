#ifndef __PID_H
#define __PID_H

typedef struct{
	float target;
	float actual;
	float out;
	
	float kp;
	float ki;
	float kd;
	
	float er0;
	float er1;
	float eri; //积分
	
	float outmax;
	float outmin;

}pid_t;

void pid_update(pid_t *p);


#endif
