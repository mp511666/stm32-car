#include "stm32f10x.h"                  // Device header
#include "pid.h"

void pid_update(pid_t *p)
{
		p->er1 =p->er0 ;
		p->er0 =p->target -p->actual ;
		
		if(p->ki!=0){p->eri +=p->er0;} 
		else {p->eri=0;}
		p->out =p->kp *p->er0
						+p->ki*p->eri
						+p->kd *(p->er0-p->er1);
		
		if(p->out >p->outmax )
		{
			p->out =p->outmax;
		
		}
		if(p->out<p->outmin)
		{
			p->out=p->outmin;
		}

}



