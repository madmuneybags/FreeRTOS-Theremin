#include "stm32l476xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "FreeRTOSConfig.h"
#include "math.h"

#include "uart.h"
#include "timer_manager.h"

// Constants for the the DAC sine wav
#define SCALE 2.5
#define OFFSET 200
#define STEPS 32

QueueHandle_t led_queue, probe_queue, typed_queue, note_queue, vol_queue;

const uint16_t SINE_WAVE[STEPS] = {
	25, 30, 35, 39, 43, 46, 48, 50,
	50, 50, 48, 46, 43, 39, 35, 30,
	25, 20, 15, 11, 7, 4, 2, 0,
	0, 0, 2, 4, 7, 11, 15, 20,                                                                
};

/* sin wave calculation

f = 440Hz 
T = 1/f = 2.28ms
overflow frequency = T / n (n = 64 per lab document)
	= 35.5 us
We need the timer to interupt every 35.5 us, triggering the next step in the sine wave array

*/

// Initializes the board's hardware
void prvSetupHardware( void ) {
	// Setup the clock and wait for it be ready
	RCC->CR |= RCC_CR_HSION;
	while((RCC->CR & RCC_CR_HSIRDY) == 0);
	RCC->CFGR |= RCC_CFGR_SW_HSI;
	
	// Manage GPIOs A & C
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOCEN | RCC_AHB2ENR_GPIOBEN;
	
	// Set (User LD2) PA5 to output mode
	GPIOA->MODER &= 0xFFFFF3FF;
	GPIOA->MODER |= 0x00000400; // Set P5 to output (01)
	
	// Set (DAC1) PA4 to analog mode
	GPIOA->MODER &= 0xFFFFFCFF;
	GPIOA->MODER |= 0x00000300; // Set P4 to analog (11)
	
	// Set (B1 User) PC13 to input mode
	GPIOC->MODER &= 0xF3FFFFFF;
	GPIOC->MODER |= 0x00000000; // Set P13 to input (00)
	
	// Set (B1 User) PC13 to pull-down
	GPIOC->PUPDR &= 0xF3FFFFFF;
	GPIOC->PUPDR |= 0x08000000; // Set P13 to pull-down (10)
}

void prvDacSetup(void){
	// Disable DAC before config
	RCC->APB1ENR1 |= RCC_APB1ENR1_DAC1EN;
	DAC1->CR &= ~DAC_CR_EN1;


	// Software trigger
//	DAC1->CR &= ~DAC_CR_TSEL1;
//	DAC1->CR |= DAC_CR_TEN1;
	
	// Enable the DAC's output buffer
	DAC1->MCR &= ~DAC_MCR_MODE1_Msk;

	// Disable triangle
	//DAC1->CR &= ~(DAC_CR_WAVE1);

	// Enable DAC channel
	DAC1->CR |= DAC_CR_EN1;
}

void TIM4_IRQHandler(void) {
	// checks if interupt flag is set
	if (TIM4->SR & TIM_SR_UIF){
		// Resets interupt flag
		TIM4->SR &= ~TIM_SR_UIF;
		static uint16_t idx = 0;
	
		// Peek at queue
		char led = 0;
		if (xQueueIsQueueFullFromISR(led_queue)) {
			xQueuePeekFromISR(led_queue, &led);
	}
		// Make sound if LED is on
		if (led) {
			idx = (idx + 1) % STEPS;
			
			int volume = 0;
			xQueuePeekFromISR(vol_queue, &volume);
			volume *= SCALE;
			
			DAC1->DHR12R1 = (SINE_WAVE[idx] * volume) + OFFSET;
		}
	}
}

char USART_IRQHandler(USART_TypeDef* USARTx){
	 // Received data
	if(USARTx->ISR & USART_ISR_RXNE) {
		return USARTx->RDR;
	}
	
	return '\0';
}

void USART1_IRQHandler(void) {
	volatile char received = USART_IRQHandler(USART1);
	BaseType_t task_woken = pdFALSE;
	
	xQueueSendFromISR(probe_queue, &received, &task_woken);
}

void USART3_IRQHandler(void){
	volatile char received = USART_IRQHandler(USART3);
	BaseType_t task_woken = pdFALSE;
	xQueueSendFromISR(note_queue, &received, &task_woken);
}

void USART2_IRQHandler(void) {
//	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//	volatile char received = USART_IRQHandler(USART2);
//	char temp_char = 0x50;
//	char dist_char = 0x55;
//	
//	switch (received) {
//		case 'a': vTimerManagerSetT4(STEPS, A3); break;
//		case 'b': vTimerManagerSetT4(STEPS, B3); break;
//		case 'c': vTimerManagerSetT4(STEPS, C3); break;
//		case 'd': vTimerManagerSetT4(STEPS, D3); break;
//		case 'e': vTimerManagerSetT4(STEPS, E3); break;
//		case 'f': vTimerManagerSetT4(STEPS, F3); break;
//		case 'g': vTimerManagerSetT4(STEPS, G3); break;
//		case 'h': vTimerManagerSetT4(STEPS, A3); break;
//		case 't': USART_Write(USART1, &temp_char, 1); 
//							xQueueOverwriteFromISR(typed_queue, &received, &xHigherPriorityTaskWoken);
//							break;
//		case 'p': USART_Write(USART1, &dist_char, 1); // distance 
//							xQueueOverwriteFromISR(typed_queue, &received, &xHigherPriorityTaskWoken);
//							break;
//		default: vTimerManagerSetT4(STEPS, 1); break;
//	}
}

// Task declaration for the LED's behavior
void vMainLedTask( void* pvParams ) {
	for ( ;; ) {
		// Write to ODR
		char led;
		xQueuePeek(led_queue, &led, portMAX_DELAY);
		
		if (led) {
			GPIOA->ODR |= 0x00000020;
		} else {
			GPIOA->ODR &= 0xFFFFFFDF;
		}
	}
}


// Task declaration for the button's behavior
void vMainBtnTask( void* pvParams ) {
	const int BTN_DELAY = 1000;
	static char current_led = 0;
	
	xQueueOverwrite(led_queue, &current_led);
	
	for ( ;; ) {
		// Read from IDR
		
		if (((GPIOC->IDR >> 13) & 1) == 0) {
			for (volatile int i = 0; i < BTN_DELAY; i++); 
			if (((GPIOC->IDR >> 13) & 1) == 0) {
				// Toggle the led value
				current_led = ~current_led;
				xQueueOverwrite(led_queue, &current_led);
				
				// Wait if being held down
				while(((GPIOC->IDR >> 13) & 1) == 0);
			}
		}
	}
}

//void vMainProbeTask( void* pvParams ) {
//	char* new_line = "\r\n";
//	for ( ;; ) {
//		char slot1, slot3 = '\0';
//		char dist[2];
//		char buffer[10] = {'\0'};
//		
//		xQueuePeek(typed_queue, &slot3, portMAX_DELAY);
//		
//		if (slot3 == 'p'){
//					
//					xQueueReceive(probe_queue, &dist[0], portMAX_DELAY);
//					xQueueReceive(probe_queue, &dist[1], portMAX_DELAY);
//					int combined = dist[0] + dist[1];
//					int inches = combined; //conversion has been removed. it would have gone here. 
//					char new_buffer[10] = {'\0'};
//					//sprintf(buffer, "%d", dist[0] + dist[1]);
//					snprintf(new_buffer, sizeof(new_buffer), "%d", inches);
//					
//					USART_Write(USART2, "Distannce (in.): ", 15);
//					USART_Write(USART2, new_buffer, 10);
//					USART_Write(USART2, new_line, 2);
//		} else if (slot3 == 't') {
//				xQueueReceive(probe_queue, &slot1, portMAX_DELAY);
//				int new_temp = slot1 + 4;
//				sprintf(buffer, "%d", new_temp);
//				USART_Write(USART2, "Temperature (F): ", 17);
//				USART_Write(USART2, buffer, 10);
//				USART_Write(USART2, new_line, 2);
//		}
//	}
//}

void vMainProbeTask( void* pvParams ) {
	char* new_line = "\r\n";
	char dist_char = 0x55;
	uint8_t raw_vol[2];
	uint8_t raw_note[2];
	
	int current_volume = 0;
	int current_note = 0;
	
	int volume_count = 0;
	int note_count = 0;
	int vol_avg = 0;
	int note_avg = 0;
	
	for ( ;; ) {
//		TickType_t lastWakeTime = xTaskGetTickCount();
//		vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(100));
		
		// write to sensor to get distance
		USART_Write(USART1, &dist_char, 1); 
		
		// read the distance
		xQueueReceive(probe_queue, &raw_vol[0], portMAX_DELAY);
		xQueueReceive(probe_queue, &raw_vol[1], portMAX_DELAY);
		// combine into int
		int vol = ((uint16_t)raw_vol[0] << 8) | raw_vol[1];
		
		//int vol = raw_vol[1];
		vol = (vol / 70);		// force 7 steps
		if (vol > 7) vol = 7;
		
		if (vol != current_volume) {
			current_volume = vol;
			
			xQueueOverwrite(vol_queue, &current_volume);
			
			char buffer[10] = { '\0' };
			snprintf(buffer, sizeof(buffer), "%d", vol);
			
			USART_Write(USART2, "Volume: ", 8);
			USART_Write(USART2, buffer, 10);
			USART_Write(USART2, new_line, 2);
		}
		
		// write to second sensor to get distance
		USART_Write(USART3, &dist_char, 1); 
		
		// read the distance
		xQueueReceive(note_queue, &raw_note[0], portMAX_DELAY);
		xQueueReceive(note_queue, &raw_note[1], portMAX_DELAY);
		// combine into int
		uint16_t note = ((uint16_t)raw_note[0] << 8) | raw_note[1];
		
		int new_note_index = (note + 50) / 24;
		if (new_note_index >= 24) new_note_index = 24;
		int new_note = NOTES[new_note_index];
		if (current_note != new_note) {
				char* note_name = NOTE_NAMES[new_note_index];
				current_note = new_note;
			
				vTimerManagerSetT4(STEPS, new_note);
			
				char buffer2[10] = { '\0' };
				snprintf(buffer2, sizeof(buffer2), "%u", note);
				
				USART_Write(USART2, note_name, 3);
				USART_Write(USART2, new_line, 2);
		}
	}
}

int main( void )
{
	// Create the queues
	led_queue = xQueueCreate(1, sizeof(char));
	probe_queue = xQueueCreate(2, sizeof(char));
	note_queue = xQueueCreate(2, sizeof(char));
	typed_queue = xQueueCreate(1, sizeof(char));
	vol_queue = xQueueCreate(1, sizeof(int));
	
	//fill queue since it will be a mailbox
	char empty = 0;
	xQueueSend(led_queue, (void *) &empty, (TickType_t) 10);
	xQueueSend(typed_queue, (void *) &empty, (TickType_t) 10);
	
	int init_volume = 1;
	xQueueSend(vol_queue, (void *) &init_volume, (TickType_t) 10);
	
	// Setup hardware
	prvSetupHardware();
	vTimerManagerInitT4(STEPS, A3);
	prvDacSetup();
	UART1_init();
	UART2_init();
	UART3_init();
	
	NVIC_SetPriority(TIM4_IRQn, 10);
	NVIC_SetPriority(USART1_IRQn, configMAX_SYSCALL_INTERRUPT_PRIORITY - 1);
	NVIC_SetPriority(USART2_IRQn, configMAX_SYSCALL_INTERRUPT_PRIORITY - 1);
	NVIC_SetPriority(USART3_IRQn, configMAX_SYSCALL_INTERRUPT_PRIORITY - 1);
	
	// Initialize tasks
	TaskHandle_t xLedHandle = NULL;
	TaskHandle_t xBtnHandle = NULL;
	TaskHandle_t xProbeHandle = NULL;
	
	// Create the led task
	BaseType_t xLedRtn = xTaskCreate(vMainLedTask, 
		"ledTask", 
		configMINIMAL_STACK_SIZE, 
		(void*)NULL,
		1,
		&xLedHandle);
	
	// Create the button task
	BaseType_t xBtnRtn = xTaskCreate(vMainBtnTask, 
		"btnTask", 
		configMINIMAL_STACK_SIZE, 
		(void*)NULL,
		1,
		&xBtnHandle);
		
	// Create the probe task
	BaseType_t xProbeRtn = xTaskCreate(vMainProbeTask,
		"probeTask", 
		configMINIMAL_STACK_SIZE, 
		(void*)NULL,
		1,
		&xProbeHandle);
		
	// Start FreeRTOS scheduling
	vTaskStartScheduler();
	
	// Forever loop so the program never closes
	for( ;; );
	return 0;
}