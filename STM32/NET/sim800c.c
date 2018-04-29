#include "sim800c.h"
#include "uart4.h"
#include "string.h"	
#include "FreeRTOS.h"
#include "task.h"

//��ʼ��SIM800C��Դ����
void power_800c(void)
{
	 GPIO_InitTypeDef  GPIO_InitStructure;
		
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	 //ʹ��PD�˿�ʱ��
		
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;				 			//PD.13 �˿�����
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 	//�������
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 	//IO���ٶ�Ϊ50MHz
	 GPIO_Init(GPIOD, &GPIO_InitStructure);					 				//�����趨������ʼ��GPIOD.13
	 GPIO_SetBits(GPIOD,GPIO_Pin_13);						 						//PD.13�����
}

//���ش��ڷ��ؽ���Ƿ�����ڴ����
u8* sim800c_check_cmd(char *str)
{
	char *strx=0;
	if(UART4_RX_STA&0X8000)		//���յ�һ��������
	{ 
		UART4_RX_BUF[UART4_RX_STA&0X7FFF]=0;//��ӽ�����
		strx=strstr((const char*)UART4_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}

//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,����ʧ��
u8 sim800c_send_cmd(char *cmd,char *ack)
{
	u8 res=0; 
	UART4_RX_STA=0;
	if((u32)cmd<=0XFF)
	{
		while((UART4->SR&0X40)==0);//�ȴ���һ�����ݷ������  
		UART4->DR=(u32)cmd;
	}else u4_printf("%s\r\n",cmd);//��������
	if(ack)		//��Ҫ�ȴ�Ӧ��
	{
			vTaskDelay(20);
			if(UART4_RX_STA&0X8000)//���յ��ڴ���Ӧ����
			{
				if(sim800c_check_cmd(ack))
				{
					printf("%s\r\n",UART4_RX_BUF);
					return 1;
				}//�õ���Ч���� 
				UART4_RX_STA=0;
			} 
	}
	return res;
}

//��ʼ������4������SIM800C��Դ
void init_sim800c(void)
{
		uart4_init(115200);
		power_800c();
		sim800_power=1;	 	
}

//ͨ��ATָ�����SIM800C�Ƿ�����������
u8 check_sim800c_ready(void)
{
	return sim800c_send_cmd("AT","OK");
}
