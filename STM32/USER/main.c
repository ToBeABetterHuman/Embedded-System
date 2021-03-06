#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "adc.h"
#include "sim800c.h"
#include "FreeRTOS.h"
#include "task.h"
/************************************************

STM32F103 移植FreeRTOS
LFSSNH
2018.4.28
 
************************************************/

//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		128  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define LED0_TASK_PRIO		2
//任务堆栈大小	
#define LED0_STK_SIZE 		50  
//任务句柄
TaskHandle_t LED0Task_Handler;
//任务函数
void led0_task(void *pvParameters);

//任务优先级
#define LED1_TASK_PRIO		3
//任务堆栈大小	
#define LED1_STK_SIZE 		50  
//任务句柄
TaskHandle_t LED1Task_Handler;
//任务函数
void led1_task(void *pvParameters);

//任务优先级
#define ADC_TASK_PRIO			4
//任务堆栈大小	
#define ADC_STK_SIZE 		50  
//任务句柄
TaskHandle_t ADCTask_Handler;
//任务函数
void adc_task(void *pvParameters);

//任务优先级
#define CHECK_SIM800C_TASK_PRIO			5
//任务堆栈大小	
#define CHECK_SIM800C__STK_SIZE 		100  
//任务句柄
TaskHandle_t Check_Sim800c_Task_Handler;
//任务函数
void check_sim800c_task(void *pvParameters);


int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4	 
	delay_init();	    				//延时函数初始化	  
	uart_init(2048000);					//初始化串口
	LED_Init();		  					//初始化LED
	Adc_Init();		  					//ADC初始化
	printf("Running!\r\n");
	init_sim800c();
	//创建开始任务
  xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();          //开启任务调度
		
}

//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
    //创建LED0任务
    xTaskCreate((TaskFunction_t )led0_task,     	
                (const char*    )"led0_task",   	
                (uint16_t       )LED0_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )LED0_TASK_PRIO,	
                (TaskHandle_t*  )&LED0Task_Handler);   
    //创建LED1任务
    xTaskCreate((TaskFunction_t )led1_task,     
                (const char*    )"led1_task",   
                (uint16_t       )LED1_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )LED1_TASK_PRIO,
                (TaskHandle_t*  )&LED1Task_Handler);  
		//创建ADC采集任务
    xTaskCreate((TaskFunction_t )adc_task,     
                (const char*    )"adc_task",   
                (uint16_t       )ADC_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )ADC_TASK_PRIO,
                (TaskHandle_t*  )&ADCTask_Handler); 
		//创建ADC采集任务
    xTaskCreate((TaskFunction_t )check_sim800c_task,     
                (const char*    )"check_sim800c_task",   
                (uint16_t       )CHECK_SIM800C__STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )CHECK_SIM800C_TASK_PRIO,
                (TaskHandle_t*  )&Check_Sim800c_Task_Handler); 
								
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//LED0任务函数 
void led0_task(void *pvParameters)
{
    while(1)
    {
        LED0=~LED0;
        vTaskDelay(100);
    }
}

//LED0任务函数 
void led1_task(void *pvParameters)
{
    while(1)
    {
        LED1=~LED1;
        vTaskDelay(150);
				LED1=~LED1;
        vTaskDelay(350);
    }
}   

//PB1模拟量采集
void adc_task(void *pvParameters)
{
		uint16_t value;  
		while(1)
    {
				value=Get_Adc_Average(ADC_Channel_9,10);
				//printf("ADC Value:%d\r\n",value);
				vTaskDelay(500);			
    }
} 

// 检查SIM800C能否工作
void check_sim800c_task(void *pvParameters)
{
	while(1)
    {
       if(check_sim800c_ready())
				{
					vTaskDelete(Check_Sim800c_Task_Handler);
				}	
		}			 
}
