#include "stm32f10x.h"                  // Device header
#include "Timer.h"

uint32_t timer_over_float = 0;//记录溢出值for  Timer_Speed_Check_Start()

void Timer_Init_Raw(TIM_TypeDef* TIMx,uint32_t psc,uint32_t arr){
	if(TIMx==TIM2)RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	else if(TIMx==TIM3)RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	else if(TIMx==TIM4)RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	else if(TIMx==TIM5)RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	TIM_InternalClockConfig(TIMx);//use ineternal Clock

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;//do not division
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;//CountMode:UP
	TIM_TimeBaseInitStructure.TIM_Period = arr;
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseInitStructure);
	TIM_Cmd(TIMx, ENABLE);
}

void Timer_IT_Init_Raw(TIM_TypeDef* TIMx,uint32_t psc,uint32_t arr){
	if(TIMx==TIM2)RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	else if(TIMx==TIM3)RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	else if(TIMx==TIM4)RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	else if(TIMx==TIM5)RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	TIM_InternalClockConfig(TIMx);//use ineternal Clock

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;//do not division
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;//CountMode:UP
	TIM_TimeBaseInitStructure.TIM_Period = arr;
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseInitStructure);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//Set NVIC
	NVIC_InitTypeDef NVIC_InitStructure;
	if(TIMx==TIM2)NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	else if(TIMx==TIM3)NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	else if(TIMx==TIM4)NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_Init(&NVIC_InitStructure);

	TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);
	TIM_ClearFlag(TIMx, TIM_FLAG_Update);
	
	TIM_Cmd(TIMx, ENABLE);
}

void Timer_IT_Init_Hz(TIM_TypeDef* TIMx,uint32_t Hz){
	if(TIMx==TIM2)RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	else if(TIMx==TIM3)RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	else if(TIMx==TIM4)RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	else if(TIMx==TIM5)RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	TIM_InternalClockConfig(TIMx);//use ineternal Clock

	//below use externalClock(which TIM  ,  Witch Division  ,  Trigger Mode:NO  ,  filter)
	//TIM_ETRClockMode2Config(TIM2, TIM_ExtTRGPSC_OFF, TIM_ExtTRGPolarity_NonInverted, 0x0F);
	//Set TIMwdjskwdskjk

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;//do not division
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;//CountMode:UP
	if(Hz>100000){
		TIM_TimeBaseInitStructure.TIM_Period = (72000000- 1)/Hz;//72M
		TIM_TimeBaseInitStructure.TIM_Prescaler = 0;
	}
	else {
		TIM_TimeBaseInitStructure.TIM_Period = (1000000- 1)/Hz;
		TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;
	}

	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseInitStructure);

	//Set NVIC
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//Set NVIC
	NVIC_InitTypeDef NVIC_InitStructure;
	if(TIMx==TIM2)NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	else if(TIMx==TIM3)NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	else if(TIMx==TIM4)NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);

	TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);
	TIM_ClearFlag(TIMx, TIM_FLAG_Update);
	TIM_Cmd(TIMx, ENABLE);
}
/*例子:TImer_IT_init_ms(TIM2,10)这个指的是你打开了一个10ms溢出的定时器，而不是打开了一个计数频率是10ms的定时器
TIMx   选择定时器
ms     设定多少ms的定时器
#注意这里的NVIC都是同一个组，注意避免冲突
*/
void TImer_IT_init_ms(TIM_TypeDef* TIMx,uint32_t ms){
	if(TIMx==TIM2)RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	else if(TIMx==TIM3)RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	else if(TIMx==TIM4)RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	else if(TIMx==TIM5)RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	RCC_HSEConfig(RCC_HSE_ON);
	//TIM_InternalClockConfig(TIMx);//use ineternal Clock

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;//do not division
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;//CountMode:UP
	TIM_TimeBaseInitStructure.TIM_Period = 10*ms - 1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7200 - 1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseInitStructure);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//Set NVIC
	NVIC_InitTypeDef NVIC_InitStructure;
	if(TIMx==TIM2)NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	else if(TIMx==TIM3)NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	else if(TIMx==TIM4)NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_ITConfig(TIMx,TIM_IT_Update,ENABLE);
	TIM_ClearFlag(TIMx, TIM_FLAG_Update);
	TIM_ClearITPendingBit(TIMx,TIM_IT_Update);

	TIM_Cmd(TIMx, ENABLE);
}

void Timer_Speed_Check_Start(TIM_TypeDef* TIMx){//精度1ms
	static uint8_t Init_flag = 0;
	
	if(Init_flag==0){//只允许程序执行一次 it means that we only run this code once
		if(TIMx==TIM2)RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
		else if(TIMx==TIM3)RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
		else if(TIMx==TIM4)RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
		else if(TIMx==TIM5)RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
		TIM_InternalClockConfig(TIMx);//use ineternal Clock

		//初始化定时器
		TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
		TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;//do not division
		TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;//CountMode:UP
		TIM_TimeBaseInitStructure.TIM_Period = 65535;
		TIM_TimeBaseInitStructure.TIM_Prescaler = 7200;//1M -> 100us = 0.1ms
		TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
		TIM_TimeBaseInit(TIMx, &TIM_TimeBaseInitStructure);

		//配置NVIC
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//Set NVIC
		NVIC_InitTypeDef NVIC_InitStructure;
		if(TIMx==TIM2)NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
		else if(TIMx==TIM3)NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
		else if(TIMx==TIM4)NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
		NVIC_Init(&NVIC_InitStructure);

		TIM_ClearITPendingBit(TIMx,TIM_IT_Update);
		TIM_ClearFlag(TIMx, TIM_FLAG_Update);
		TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);
		
		Init_flag=1;
	}
	TIM_Cmd(TIMx, ENABLE);
}

void Timer_Speed_Check_Stop(TIM_TypeDef* TIMx,float* speed){
	TIM_Cmd(TIMx, DISABLE);//关闭定时器
	*speed = ((float)timer_over_float*65535 + (float)TIM_GetCounter(TIMx))/10;//将速度写入
	TIM_SetCounter(TIMx,0);
	timer_over_float=0;
}

/*例子:IC_Init_Internal(TIM2,1)//TIM2的CH1通道作为输入捕获
TIMx:选择你的输入捕获寄存器(TIM2~3)
CH:选择你的通道(1~4)
##输入捕获使用内部时钟常用于测周法,在STM32F103C8T6中CH3和CH4不能使用
*/
void IC_Init_Internal(TIM_TypeDef* TIMx,uint8_t CH)
{
	if(TIMx==TIM3)RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	else if(TIMx==TIM2)RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	if(TIMx==TIM3){
		switch (CH)
		{
			case 1:GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;break;
			case 2:GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;break;
			case 3:GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;break;
			case 4:GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;break;
		default:
			break;
		}
	}
	else if(TIMx==TIM2){
		switch (CH)
		{
			case 1:GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;break;
			case 2:GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;break;
			case 3:GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;break;
			case 4:GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;break;
		default:
			break;
		}
	}
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;//no division
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 65536 - 1;		//ARR最大记录66535个数
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;		//PSC 1M的速度计数 1us
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseInitStructure);

	//输入捕获的结构体
	TIM_ICInitTypeDef TIM_ICInitStructure;
	switch (CH)
	{
		case 1:TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;break;//CH1输入捕获通道
		case 2:TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;break;//CH2输入捕获通道
		case 3:TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;break;//CH3输入捕获通道
		case 4:TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;break;//CH4输入捕获通道
	default:
		break;
	}
	TIM_ICInitStructure.TIM_ICFilter = 0x0;//无滤波器
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;//上升沿捕获
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;//对输入信号不分频
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;//直连通道不走交叉通道
	TIM_ICInit(TIMx,&TIM_ICInitStructure);
	TIM_PWMIConfig(TIMx,&TIM_ICInitStructure);//PWMI模式，把另一个下降沿的数据传到第二个输入捕获寄存器
	switch (CH)
	{
		case 1:TIM_SelectInputTrigger(TIMx,TIM_TS_TI1FP1);break;//选择触发源break;//CH1输入捕获通道
		case 2:TIM_SelectInputTrigger(TIMx,TIM_TS_TI2FP2);break;//CH2输入捕获通道
		//STM32F103C8T6的配置有限,以下不能使用
		//case 3:TIM_SelectInputTrigger(TIMx,TIM_TS_TI3FP3);break;//CH3输入捕获通道
		//case 4:TIM_SelectInputTrigger(TIMx,TIM_TS_TI4FP4);break;//CH4输入捕获通道
	default:
		break;
	}
	TIM_SelectSlaveMode(TIMx,TIM_SlaveMode_Reset);//设置定时器从模式,设置为清0
	
	TIM_Cmd(TIMx,ENABLE);
}

/*例子:IC_Init_External(TIM3,TIM_Channel_1);
TIMx:选择你的定时器
TIM_Channel_x:选择你的定时器通道
##注意:用于外部触发定时器计数，常用于测频法,STM32F103C8T6的配置有限，TIM_Channel_3和TIM_Channel_4不能正常使用
*/
void IC_Init_External(TIM_TypeDef* TIMx,uint16_t TIM_Channel_x)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	if(TIMx==TIM2){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		if(TIM_Channel_x == TIM_Channel_1)GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
		else if(TIM_Channel_x == TIM_Channel_2)GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
		else if(TIM_Channel_x == TIM_Channel_3)GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		else if(TIM_Channel_x == TIM_Channel_4)GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	}
	else if(TIMx==TIM3){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);
		if(TIM_Channel_x == TIM_Channel_1){GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;GPIO_Init(GPIOA, &GPIO_InitStructure);}
		else if(TIM_Channel_x == TIM_Channel_2){GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;GPIO_Init(GPIOA, &GPIO_InitStructure);}
		else if(TIM_Channel_x == TIM_Channel_3){GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;GPIO_Init(GPIOB, &GPIO_InitStructure);}
		else if(TIM_Channel_x == TIM_Channel_4){GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;GPIO_Init(GPIOB, &GPIO_InitStructure);}
	}
	else if(TIMx==TIM4){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
		if(TIM_Channel_x == TIM_Channel_1)GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
		else if(TIM_Channel_x == TIM_Channel_2)GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
		else if(TIM_Channel_x == TIM_Channel_3)GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
		else if(TIM_Channel_x == TIM_Channel_4)GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
		GPIO_Init(GPIOD, &GPIO_InitStructure);
	}
	/*
	if(TIMx==TIM5){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		if(TIM_Channel_x == TIM_Channel_1)GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
		else if(TIM_Channel_x == TIM_Channel_2)GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
		else if(TIM_Channel_x == TIM_Channel_3)GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		else if(TIM_Channel_x == TIM_Channel_4)GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	}*/
	
	//配置定时器
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 65535 ;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 1-1;//将外部输入信号/10可以减少出中断的次数，但是会导致精度下降
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseInitStructure);
	//配置中断
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStructure;
	if(TIMx ==TIM2)NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	else if(TIMx ==TIM3)NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	else if(TIMx ==TIM4)NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);

	if(TIM_Channel_x == TIM_Channel_1)
		TIM_TIxExternalClockConfig(TIMx,TIM_TIxExternalCLK1Source_TI1,TIM_ICPolarity_Rising,0);//外部信号上升沿来到内部计数值+1
	else if(TIM_Channel_x == TIM_Channel_2)
		TIM_TIxExternalClockConfig(TIMx,TIM_TIxExternalCLK1Source_TI2,TIM_ICPolarity_Rising,0);//外部信号上升沿来到内部计数值+1
	/*STM32F103C8T6的配置有限，以下配置不能使用
	else if(TIM_Channel_x == TIM_Channel_3)
		TIM_TIxExternalClockConfig(TIMx,TIM_TIxExternalCLK1Source_TI3,TIM_ICPolarity_Rising,0);//外部信号上升沿来到内部计数值+1
	else if(TIM_Channel_x == TIM_Channel_4)
		TIM_TIxExternalClockConfig(TIMx,TIM_TIxExternalCLK1Source_TI4,TIM_ICPolarity_Rising,0);//外部信号上升沿来到内部计数值+1
	*/
	
	//TIM_ClearFlag(TIMx, TIM_FLAG_Update);//清除中断标志位
	TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);//开启定时器中断
	
	TIM_Cmd(TIMx, ENABLE);
}

//返回频率
//TIMx:选择你的定时器(TIM1~2)
//CH:选择你的通道(1~4)
uint32_t IC_GetFreq_Cycle(TIM_TypeDef* TIMx,uint8_t CH){//return freq
	uint32_t temp = 0;
	switch (CH)
	{
	case 1: temp = (1000000 / (TIM_GetCapture1(TIMx) + 1));break;
	case 2: temp = (1000000 / (TIM_GetCapture2(TIMx) + 1));break;
	case 3: temp = (1000000 / (TIM_GetCapture3(TIMx) + 1));break;
	case 4: temp = (1000000 / (TIM_GetCapture4(TIMx) + 1));break;
	default: temp = 6666 ;break;//错误代码
	}
	return temp;
}
//返回占空比
//TIMx:选择你的定时器(TIM1~2)
//CH:选择你的通道(1~4)
uint32_t IC_GetDuty(TIM_TypeDef* TIMx,uint8_t CH){
	uint32_t temp = 0;
	switch (CH)
	{
	case 1:temp =  (TIM_GetCapture2(TIMx) + 1) * 100 / (TIM_GetCapture1(TIMx) + 1);break;
	case 2:temp = (TIM_GetCapture1(TIMx) + 1) * 100 / (TIM_GetCapture2(TIMx) + 1);break;
	case 3:temp = (TIM_GetCapture4(TIMx) + 1) * 100 / (TIM_GetCapture3(TIMx) + 1);break;
	case 4:temp = (TIM_GetCapture3(TIMx) + 1) * 100 / (TIM_GetCapture4(TIMx) + 1);break;
	default:temp = 6666 ;break;//错误代码
	}
	return temp;
}

/*
void TIM3_IRQHandler(void){
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET){
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);//clear flag
		timer_over_float++;
	}
	if (TIM_GetITStatus(TIM3, TIM_IT_CC1) == SET){
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);	
    }
}

//定时器测频法模板,请修改timer_over_float不要和Timer_Speed_Check_Start()冲突
uint32_t return_value = 0;
void TIM4_IRQHandler(void){
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) == SET){
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);//clear flag
			return_value = TIM_GetCounter(TIM3) + timer_over_float*65536;//读取外部定时器的值
			TIM_SetCounter(TIM3, 0);//复位定时器3计数值
			timer_over_float = 0;//复位溢出值
	}
}
*/
