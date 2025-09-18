#ifndef __TIMER_H
#define __TIMER_H

/*//TIM_CH GPIO_Pin praphic定时器引脚图
Universal Timer//////////////////////////////
TIM2_default:  	TIM2_remap:	|	TIM3_default:	TIM3_reamp:	 |	TIM4_reamp:		|	TIM5_default:
TIM2:CH1->A0				|	TIM3:CH1->A6	C6,B4		 |	TIM4:CH1->D12	|	IM5:CH1->A0
	 CH2->A1		B3		|		CH2->A7	    C7,B5		 |		CH2->D13	|		CH2->A1
	 CH3->A2		B10		|		CH3->B0	    C8			 |		CH3->D14	|		CH3->A2
	 CH4->A3		B11		|		CH4->B1	    C9			 |		CH4->D15	|		CH4->A3

Advance Timer////////////////////////////////
TIM1_default:	 TIM1_remap:  |	 TIM8_default:
TIM1:BKIN->B12	  A6,E15	  |	 TIM8:ETR->A0	
	CH1N->B13	  A7,E8		  |	 	 BKIN->A6
	CH2N->B14	  B0,E10	  |	 	 CH1N->A7
	CH3N->B15	  B1,E12	  |		 CH2N->B0
	CH1->A8		  E9		  |		 CH3N->B1
	CH2->A9		  E11		  |		 CH1->C6
	CH3->A10	  E13		  |	     CH2->C7
	CH4->A11	  E14		  |		 CH3->C8
	ETR->A12	  E7		  |		 CH4->C9
*/

extern uint32_t timer_over_float;//记录定时器溢出值用于，定时器测速防止溢出
//基本定时器
void Timer_Init_Raw(TIM_TypeDef* TIMx,uint32_t psc,uint32_t arr);
void Timer_IT_Init_Raw(TIM_TypeDef* TIMx,uint32_t psc,uint32_t arr);
void Timer_IT_Init_Hz(TIM_TypeDef* TIMx,uint32_t Hz);
void TImer_IT_init_ms(TIM_TypeDef* TIMx,uint32_t ms);
void Timer_Speed_Check_Start(TIM_TypeDef* TIMx);//测速函数要求使用72M主频
void Timer_Speed_Check_Stop(TIM_TypeDef* TIMx,float* speed);//测速函数要求使用72M主频

void IC_Init_Internal(TIM_TypeDef* TIMx,uint8_t CH);//测周法,测频率
void IC_Init_External(TIM_TypeDef* TIMx,uint16_t TIM_Channel_x);//测频法,测频率
uint32_t IC_GetFreq_Cycle(TIM_TypeDef* TIMx,uint8_t CH);
uint32_t IC_GetDuty(TIM_TypeDef* TIMx,uint8_t CH);

#endif
