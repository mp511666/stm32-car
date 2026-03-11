#include "stm32f10x.h"                  // Device header
#include "Delay.h"
void myiic_W_SCL(uint8_t BitValue)
{
	GPIO_WriteBit (GPIOB,GPIO_Pin_10,(BitAction )BitValue); //强制类型转换是为了把数字当作枚举值处理
	Delay_us (10);
}

void myiic_W_SDA(uint8_t BitValue)
{
	GPIO_WriteBit (GPIOB,GPIO_Pin_11,(BitAction )BitValue); //强制类型转换是为了把数字当作枚举值处理
	Delay_us (10);
}

uint8_t myiic_R_SDA(void )
{
	uint8_t BitValue;
	BitValue=GPIO_ReadInputDataBit (GPIOB ,GPIO_Pin_11); 
	Delay_us (10);
	return BitValue;
}
void myiic_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB,GPIO_Pin_10|GPIO_Pin_11);
}
/*起始*/
void myiic_start(void)
{
		myiic_W_SDA(1);
		myiic_W_SCL(1);
		myiic_W_SDA(0);
		myiic_W_SCL(0);
}
/*终止*/
void myiic_stop(void)
{
		myiic_W_SDA(0);
		myiic_W_SCL(1);
		myiic_W_SDA(1);
}
/*发送一个字节*/
void myiic_send(uint8_t Byte)
{
	uint8_t i;
	for(i=0;i<8;i++)
	{
		myiic_W_SDA(Byte&(0x80>>i)); //&的作用是除了第x位，其他位写入不受&影响（高位先行）
		myiic_W_SCL(1);
		myiic_W_SCL(0);
	}
}
/*接收一个字节*/
uint8_t myiic_receive(void)
{
	uint8_t i,Byte=0x00;
	myiic_W_SDA(1);
	for(i=0;i<8;i++)
	{
		myiic_W_SCL(1);
		if(myiic_R_SDA()==1){Byte |=(0x80>>i);}
		myiic_W_SCL(0);
	}
	return Byte;
}	
/*发送应答*/
void myiic_sendack(uint8_t Ackbit)
{
		myiic_W_SDA(Ackbit); //&的作用是除了第x位，其他位写入不受&影响（高位先行）
		myiic_W_SCL(1);
		myiic_W_SCL(0);
	
}
/*接收应答*/
uint8_t myiic_receiveack(void)
{
	uint8_t Ackbit;
	myiic_W_SDA(1);
	
	myiic_W_SCL(1);
	Ackbit=myiic_R_SDA();
	myiic_W_SCL(0);
		
	return  Ackbit;
}	

