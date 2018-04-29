#include "delay.h"
#include "uart4.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	 
#include "timer.h"   

//串口接收缓存区 	
u8 	UART4_RX_BUF[UART4_MAX_RECV_LEN]; 				//接收缓冲,最大UART4_MAX_RECV_LEN个字节.
u8  UART4_TX_BUF[UART4_MAX_SEND_LEN]; 			//发送缓冲,最大UART4_MAX_SEND_LEN字节

//通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
//任何数据,则表示此次接收完毕.
//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度
vu16 UART4_RX_STA=0;   	


void UART4_IRQHandler(void)
{
	u8 res;	      
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)//接收到数据
	{	 
		res =USART_ReceiveData(UART4);		 
		if((UART4_RX_STA&(1<<15))==0)//接收完的一批数据,还没有被处理,则不再接收其他数据
		{ 
			if(UART4_RX_STA<UART4_MAX_RECV_LEN)	//还可以接收数据
			{
				TIM_SetCounter(TIM7,0);//计数器清空          				//计数器清空
				if(UART4_RX_STA==0) 				//使能定时器7的中断 
				{
					TIM_Cmd(TIM7,ENABLE);//使能定时器7
				}
				UART4_RX_BUF[UART4_RX_STA++]=res;	//记录接收到的值	 
			}else 
			{
				UART4_RX_STA|=1<<15;				//强制标记接收完成
			} 
		}
	}  				 											 
}   


//初始化IO 串口4
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率	  
void uart4_init(u32 bound)
{  

	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	// GPIOB时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE); //串口4时钟使能

 	USART_DeInit(UART4);  //复位串口4
		 //UART4_TX   PC10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PC10
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOC, &GPIO_InitStructure); //初始化PC10
   
    //UART4_RX	  PC11
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOC, &GPIO_InitStructure);  //初始化PC11
	
	USART_InitStructure.USART_BaudRate = bound;//波特率一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  
	USART_Init(UART4, &USART_InitStructure); //初始化串口	4
  

	USART_Cmd(UART4, ENABLE);                    //使能串口 
	
	//使能接收中断
  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//开启中断   
	
	//设置中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
	
	TIM7_Int_Init(99,7199);		//10ms中断
	UART4_RX_STA=0;		//清零
	TIM_Cmd(TIM7,DISABLE);			//关闭定时器7

}

//串口4,printf 函数
//确保一次发送数据不超过UART4_MAX_SEND_LEN字节
void u4_printf(char* fmt,...)  
{  
	u16 i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)UART4_TX_BUF,fmt,ap);
	i=strlen((const char*)UART4_TX_BUF);		//此次发送数据的长度
	for(j=0;j<i;j++)							//循环发送数据
	{
	  while(USART_GetFlagStatus(UART4,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
		USART_SendData(UART4,UART4_TX_BUF[j]); 
	} 
}

 



















