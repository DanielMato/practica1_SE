#include "MKL46Z4.h"

// LED (RG)
// LED_GREEN = PTD5
// LED_RED = PTE29
// BUTTON = PTC3 (?)

void delay(void)
{
  volatile int i;

  for (i = 0; i < 1000000; i++);
}

// LED_GREEN = PTD5
void led_green_init()
{
  SIM->COPC = 0; //COP = watchdog. SIM = modulo del sistema. Deshabilita o watchdog
  SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK; //SCGC = registros para dar reloj al puerto cuya máscara uses
  PORTD->PCR[5] = PORT_PCR_MUX(1); //(Usar puerto 1 del MUX del pin 5 del puerto D)
  GPIOD->PDDR |= (1 << 5); //GPIO = General Purpose IO  //PDDR= Port Data Direction Register. 1 para output, 0 para input, ponerlo en el pin correcto (en este caso pin 5 que es el del led)
  GPIOD->PSOR = (1 << 5); //PSOR: Port Set Output Register 
}

void button_init()
{
  SIM->COPC = 0; //COP = watchdog. SIM = modulo del sistema. Deshabilita o watchdog
  SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK; //SCGC = registros para dar reloj al puerto cuya máscara uses
  PORTC->PCR[3] |= PORT_PCR_MUX(1)|PORT_PCR_PE_MASK|PORT_PCR_PS_MASK|PORT_PCR_IRQC(10); //(Usar puerto 1 del MUX del pin 3 del puerto C, y activar el pullup. Configurar interrupt a falling edge)
  GPIOC->PDDR &= ~(1 << 3); //GPIO = General Purpose IO  //PDDR= Port Data Direction Register. 1 para output, 0 para input, ponerlo en el pin correcto (en este caso pin 3 que es el del botón)
}

//No PCR, para os de entrada hay que configurar o pullup (campo PE a 1) (port_pcr_pe)
//Enquisa, preguntar ao rexistro correcto con GPIOx_PDIR
//Cuando se pulse, pasa de un color al otro

void led_green_toggle()
{
  GPIOD->PTOR = (1 << 5);
}

// LED_RED = PTE29
void led_red_init()
{
  SIM->COPC = 0;
  SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
  PORTE->PCR[29] = PORT_PCR_MUX(1);
  GPIOE->PDDR |= (1 << 29);
  GPIOE->PSOR = (1 << 29);

}

void led_red_toggle()
{
  GPIOE->PTOR = (1 << 29);
}
void delay_custom(int ms){ //funcion para esperar x milisegundos. Al final no se usa porque check_pressed es mejor
	int i;
	for (i=ms*1500; i>0; i--){}
}

void PORTDIntHandler(void){ //Handler para la interrupcion por puerto C o D, aunque solo ponga D
	
	led_green_toggle();
	led_red_toggle();
	PORTC->PCR[3] |= PORT_PCR_ISF_MASK;
	
	

}

int main(void)
{
  NVIC_SetPriority(31, 0);


  led_green_init();
  led_red_init();
  button_init();
  led_green_toggle();

  NVIC_EnableIRQ(31);//Activar interrupcions portos C e D, no noso caso interesanos o porto C
  while (1) {//Bucle infinito, esperamos a la interrupcion
  }

  return 0;
}
