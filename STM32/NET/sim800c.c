#include "sim800c.h"
#include "uart4.h"
#include "string.h"	
#include "FreeRTOS.h"
#include "task.h"

//初始化SIM800C电源引脚
void power_800c(void)
{
	 GPIO_InitTypeDef  GPIO_InitStructure;
		
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	 //使能PD端口时钟
		
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;				 			//PD.13 端口配置
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 	//推挽输出
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 	//IO口速度为50MHz
	 GPIO_Init(GPIOD, &GPIO_InitStructure);					 				//根据设定参数初始化GPIOD.13
	 GPIO_SetBits(GPIOD,GPIO_Pin_13);						 						//PD.13输出高
}

//返回串口返回结果是否包含期待结果
u8* sim800c_check_cmd(char *str)
{
	char *strx=0;
	if(UART4_RX_STA&0X8000)		//接收到一次数据了
	{ 
		UART4_RX_BUF[UART4_RX_STA&0X7FFF]=0;//添加结束符
		strx=strstr((const char*)UART4_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}

//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
u8 sim800c_send_cmd(char *cmd,char *ack)
{
	u8 res=0; 
	UART4_RX_STA=0;
	if((u32)cmd<=0XFF)
	{
		while((UART4->SR&0X40)==0);//等待上一次数据发送完成  
		UART4->DR=(u32)cmd;
	}else u4_printf("%s\r\n",cmd);//发送命令
	if(ack)		//需要等待应答
	{
			vTaskDelay(20);
			if(UART4_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(sim800c_check_cmd(ack))
				{
					printf("%s\r\n",UART4_RX_BUF);
					return 1;
				}//得到有效数据 
				UART4_RX_STA=0;
			} 
	}
	return res;
}

//初始化串口4，并打开SIM800C电源
void init_sim800c(void)
{
		uart4_init(115200);
		power_800c();
		sim800_power=1;	 	
}

//通过AT指令测试SIM800C是否能正常工作
u8 check_sim800c_ready(void)
{
	return sim800c_send_cmd("AT","OK");
}
