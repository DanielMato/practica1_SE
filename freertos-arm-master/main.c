#include "MKL46Z4.h"
#include "FreeRTOS.h"
#include "task.h"


void button_init()
{
  SIM->COPC = 0; //COP = watchdog. SIM = modulo del sistema. Deshabilita o watchdog
  SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK; //SCGC = registros para dar reloj al puerto cuya máscara uses
  PORTC->PCR[3] |= PORT_PCR_MUX(1)|PORT_PCR_PE_MASK|PORT_PCR_PS_MASK; //(Usar puerto 1 del MUX del pin 3 del puerto C, y activar el pullup)
  GPIOC->PDDR &= ~(1 << 3); //GPIO = General Purpose IO  //PDDR= Port Data Direction Register. 1 para output, 0 para input, ponerlo en el pin correcto (en este caso pin 3 que es el del botón)
}
int check_pressed(){
 return !(GPIOC->PDIR & (1 << 3));
}

void led_green_init()
{
	SIM_COPC = 0;
	SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;
	PORTD_PCR5 = PORT_PCR_MUX(1);
	GPIOD_PDDR |= (1 << 5);
	GPIOD_PSOR = (1 << 5);
}

void led_green_toggle()
{
	GPIOD_PTOR = (1 << 5);
}

void led_red_init()
{
	SIM_COPC = 0;
	SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;
	PORTE_PCR29 = PORT_PCR_MUX(1);
	GPIOE_PDDR |= (1 << 29);
	GPIOE_PSOR = (1 << 29);
}

void led_red_toggle(void)
{
	GPIOE_PTOR = (1 << 29);
}


static TaskHandle_t notifyRed = NULL;
static TaskHandle_t notifyGreen = NULL;


void taskLedGreen(void *pvParameters)
{
    notifyGreen = xTaskGetCurrentTaskHandle();
    for (;;) {
	xTaskNotifyWait(0x00, INT8_MAX, NULL, portMAX_DELAY);
        led_green_toggle();
	
    }
}

void taskLedRed(void *pvParameters)
{
    notifyRed = xTaskGetCurrentTaskHandle();
    for (;;) {
	xTaskNotifyWait(0x00, INT8_MAX, NULL, portMAX_DELAY);
        led_red_toggle();
	
    }
}

void taskButton(void *pvParameters)
{
    for (;;) {
	if (check_pressed()){
		xTaskNotifyGive(notifyGreen);
		xTaskNotifyGive(notifyRed);
		while (check_pressed()){};
	}
    }
}



int main(void)
{
	led_green_init();
	led_red_init();
	button_init();
	led_green_toggle();
	
	/* create green led task */
	xTaskCreate(taskLedGreen, (signed char *)"TaskLedGreen", 
		configMINIMAL_STACK_SIZE, (void *)NULL, 1, NULL);

	/* create red led task */
	xTaskCreate(taskLedRed, (signed char *)"TaskLedRed", 
		configMINIMAL_STACK_SIZE, (void *)NULL, 1, NULL);

	/* create button task */
	xTaskCreate(taskButton, (signed char *)"TaskButton", 
		configMINIMAL_STACK_SIZE, (void *)NULL, 1, NULL);
	
	/* start the scheduler */
	vTaskStartScheduler();

	/* should never reach here! */
	for (;;);

	return 0;
}

