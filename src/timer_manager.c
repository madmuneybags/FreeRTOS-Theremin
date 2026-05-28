
#include "timer_manager.h"

int calc_arr(int steps, int freq) {
	int timer_freq = steps * freq;
	int arr = CLK / (timer_freq * PRESCALE);
	
	return arr - 1;
}

void vTimerManagerSetT4(int steps, int freq) {
	TIM4->ARR = calc_arr(steps, freq);;	// Auto Reload value
}

void vTimerManagerInitT4(int steps, int freq) {
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM4EN;	// Enable Timer 4
  TIM4->CR1 &= ~TIM_CR1_CEN; 	// Disable timer before config
	TIM4->CR1 &= ~TIM_CR1_DIR;	// Set to upcount
	
	TIM4->PSC = PRESCALE - 1;	// Timer Prescaler;
	TIM4->ARR = calc_arr(steps, freq);	// Auto Reload value
	
  TIM4->EGR = TIM_EGR_UG;	// Generate update event to load PSC/ARR
  TIM4->DIER |= TIM_DIER_UIE;	// Enable update interrupt
  TIM4->SR &= ~TIM_SR_UIF;	// Clear any pending update flag
	NVIC_EnableIRQ(TIM4_IRQn);	// enables timer 4's interupt
	
  TIM4->CR1 |= TIM_CR1_CEN; 	// enable timer
}