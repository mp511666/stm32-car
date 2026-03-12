#include "stm32f10x.h"                  // Device header
#include <stdio.h>//对printf进行重定向的准备
#include <stdarg.h>//对sprintf进行包装的准备

//只存储发送或接收的载荷数据，包头包尾就不存了
char BlueTooth_RxPacket[100];
uint8_t BlueTooth_RxFlag;//受到数据包时可置标志位

//初始化
void Serial_Init(void)
{
	//开启时钟
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_USART1 , ENABLE);//USART1为APB2的外设
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA , ENABLE);
	//GPIO初始化
	GPIO_InitTypeDef GPIO_InitStructure; 
		//TX引脚，复用在PA9引脚，是USART外设控制的输出脚，要选复用推挽输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
		//RX引脚，复用在PA10引脚，是USART外设数据的输入脚，要选择输入模式，一般为浮空输入或上拉输入
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	//USART初始化
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;//波特率
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None ;//硬件流控制，可以防止处理慢而导致数据丢失的问题，通常不使用
		//CTRL+ALT+空格，可以联想代码
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;//串口模式，同时开启发送和接收部分
	USART_InitStructure.USART_Parity = USART_Parity_No ;//校验位，通常不需要，选No，Odd为奇校验，Even为偶校验
	USART_InitStructure.USART_StopBits = USART_StopBits_1 ;//停止位，通常选1
	USART_InitStructure.USART_WordLength = USART_WordLength_8b ;//字长，无需校验，则选8位
	USART_Init (USART1,&USART_InitStructure);
	//开启RXNE标志位到NVIC的输出
	USART_ITConfig (USART1,USART_IT_RXNE ,ENABLE);
	//配置NVIC
	NVIC_PriorityGroupConfig (NVIC_PriorityGroup_2);//那个优先级分组的，响应优先级和抢占优先级的那个占比
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//对应USART1的中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init (&NVIC_InitStructure);
	//启动USART
	USART_Cmd(USART1,ENABLE);
}

//发送函数
void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1,Byte);//Byte变量就写入到TDR（发送数据寄存器），写入时标志位会自动清零，所以无需手动清除
	while(USART_GetFlagStatus (USART1,USART_FLAG_TXE) == RESET);//等待TXE（发送数据寄存器空标志位）置1即SET，即等待TDR的数据转移到发送移位寄存器，防止数据覆盖

}

//串口封装函数

//发送一个数组
void Serial_SendArray(uint8_t *Array,uint16_t Length)
{
	uint16_t i;
	for(i=0;i<Length;i++)
	{
		Serial_SendByte (Array[i]);
	}
}

//发送字符串
void Serial_SendString(char *String)//字符串自带一个结束标志位，不需要再传递长度参数
{
	uint8_t i;
	for(i=0;String[i]!=0;i++)//String[i]!=0，0对应空字符，为字符串结束标志位。!=0就是还没结束，进行循环；等于0，就是结束了停止循环
	{
		Serial_SendByte (String[i]);
	}
}

//求一个数X的Y次方倍，为后面发送一个函数做准备
uint32_t Serial_Pow(uint32_t X,uint32_t Y)
{
	uint32_t Result=1;
	while(Y--)
	{
		Result *= X;
	}
	return Result;
}
	
//发送字符形式的数字
void Serial_SendNumber(uint32_t Number,uint8_t Length)
{
	uint8_t i;
	for(i=0;i<Length;i++)
	{
		//以十进制从高位到低位依次发送，最终要以字符的形式显示，结合ASCII码表，进行偏移，加上0x30或者是‘0’
		Serial_SendByte (Number / Serial_Pow(10,Length-i-1) % 10 + '0');
	}
}

//对printf进行重定向，将printf函数打印的东西输出到串口,但只能有一个，重定向到串口1了，那串口2再用就没有了
int fputc(int ch,FILE *f)//参数照着写就可以了，不用管那么多
{
	Serial_SendByte (ch);
	return ch;
	//fputc是printf函数的底层逻辑，把fputc函数重定义到串口，那printf自然就输出到串口了
}

//对sprintf可变参数的格式进行包装
void Serial_Printf(char *format,...)//第一个参数用来接收格式化字符串，...用来接收后面的可变参数列表，可以是任意数量、任意类型
{
	char String[100];//定义一个字符数组（缓冲区），用于存储格式化后地字符串
	va_list arg;//定义一个可变参数列表变量，va_list是一个类型，用于表示可变参数列表，用来依次获取...中地参数，arg就是用来遍历可变参数的变量
	va_start(arg,format);//va_start是一个带有参数的宏，用于初始化arg，让arg指向format后的第一个可变参数，即arg指向...部分地开始位置，也就是说，从format位置开始接收参数表，放在arg里面
	vsprintf(String,format,arg);//打印位置是String，格式化字符串是format，参数表是arg，这里会根据format字符串地格式说明符，从arg中依次取出对应的参数，并格式化写入String中。这里sprintf要改成vsprintf，因为sprintf只能接收直接写的参数，对于封装格式，要有vsprintf
	va_end(arg);//va_end也是一个宏，清理释放arg参数表。va_start和va_end必须成对出现，且va_end必须在函数返回前调用
	Serial_SendString(String);//把String发送出去
}

//接收数据包——中断
void USART1_IRQHandler(void)
{
	//用状态机来执行接收逻辑，RxState为状态变量
	static uint8_t RxState = 0;//静态变量，类似与全局变量，但只能在本函数使用，函数进入只会初始化一次0，函数退出后，数据仍然有效
	static uint8_t pRxPacket = 0;
	if(USART_GetFlagStatus (USART1,USART_IT_RXNE) == SET)
	{
		uint8_t RxData = USART_ReceiveData (USART1);
		//不能直接用连续的if，可能在状态转移时会出现问题，所以要使用else if，保证每次进来之后，只能选择执行其中一个状态的代码
		if(RxState==0)//进入等待包头的程序
		{
			if (RxData == '['&&BlueTooth_RxFlag == 0)//只有Flag == 0时才会继续接受下一个数据包，这样子写数据和读数据就是严格分开的，不会同时进行，就可以避免数据包错位的现象了，但是发送数据包的频率就不能太快，否则会丢弃部分数据包
			{
				RxState = 1;
				pRxPacket = 0;//清零，为后续接收数据做准备
			}
		}
		else if(RxState==1)//进入接收数据的程序
		{
			//因为载荷数量不确定，所以需要在每次接收之前，我们必须先判断是不是包尾
			if(RxData == ']')
			{
				RxState = 0;
				BlueTooth_RxPacket [pRxPacket] = '\0';//需要在这个字符数组的最后加个字符串结束标志位'\0',方便后续对字符串的处理，例如要ShowString，没有结束标志位，就不知道该字符串有多长
				BlueTooth_RxFlag = 1;//收到一个数据包了				
			}
			else
			{
				BlueTooth_RxPacket [pRxPacket] = RxData ;
				pRxPacket++;
			}
		}

		USART_ClearITPendingBit (USART1,USART_IT_RXNE);//若没读取DR，则需要手动清除标志位，若读取了，则不需要，这里干脆直接清除一下
	}
}
