#include "MKL46Z4.h"

// LED (RG)
// LED_GREEN = PTD5
// LED_RED = PTE29
// BUTTON = PTC3 (?)

int estado;
//ESTADOS:
//0: Ambas puertas cerradas
//1: Puerta 1 abierta, puerta 2 cerrada
//2: Puerta 2 abierta, puerta 1 cerrada
//3: Ambas puertas abiertas

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
void button_init_1()
{
  SIM->COPC = 0; //COP = watchdog. SIM = modulo del sistema. Deshabilita o watchdog
  SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK; //SCGC = registros para dar reloj al puerto cuya máscara uses
  PORTC->PCR[3] |= PORT_PCR_MUX(1)|PORT_PCR_PE_MASK|PORT_PCR_PS_MASK; //(Usar puerto 1 del MUX del pin 3 del puerto C, y activar el pullup)
  //GPIOC->PDDR &= ~(1 << 3); //GPIO = General Purpose IO  //PDDR= Port Data Direction Register. 1 para output, 0 para input, ponerlo en el pin correcto (en este caso pin 3 que es el del botón)
}

void button_init_2() //AJUSTAR A SWITCH 3
{
  SIM->COPC = 0; //COP = watchdog. SIM = modulo del sistema. Deshabilita o watchdog
  SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK; //SCGC = registros para dar reloj al puerto cuya máscara uses
  PORTC->PCR[12] |= PORT_PCR_MUX(1)|PORT_PCR_PE_MASK|PORT_PCR_PS_MASK; //(Usar puerto 1 del MUX del pin 3 del puerto C, y activar el pullup)
  GPIOC->PDDR &= ~((1 << 3)||(1 << 12)); //GPIO = General Purpose IO  //PDDR= Port Data Direction Register. 1 para output, 0 para input, ponerlo en el pin correcto (en este caso pin 3 que es el del botón). En este caso configuramos aquí ambos pines, porque CREO que haciendolo por separado en cada uno se aplastan el uno al otro
}

//No PCR, para os de entrada hay que configurar o pullup (campo PE a 1) (port_pcr_pe)
//Enquisa, preguntar ao rexistro correcto con GPIOx_PDIR
//Cuando se pulse, pasa de un color al otro

void led_green_on()
{
  
  GPIOD->PCOR = (1 << 5);//Por algun motivo parece que hay que usar clear para encenderlo y set para apagarlo???
}
void led_green_off()
{
  GPIOD->PSOR = (1 << 5);
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

void led_red_on()
{

  GPIOE->PCOR = (1 << 29);
}
void led_red_off()
{
  GPIOE->PSOR = (1 << 29);
  
}
void delay_custom(int ms){ //funcion para esperar x milisegundos. Al final no se usa porque check_pressed es mejor
	int i;
	for (i=ms*1500; i>0; i--){}
}
int check_pressed_1(){
 return !(GPIOC->PDIR & (1 << 3));
}

int check_pressed_2(){//AJUSTAR A SWITCH 3
 return !(GPIOC->PDIR & (1 << 12));
}

int main(void)
{
  estado = 0; //inicializamos el estado a 0

  led_green_init();
  led_red_init();
  button_init_1();
  button_init_2();


  while (1) {
    switch(estado){
      case 0:
        led_green_off();
        led_red_off();
        if (check_pressed_1()){
          estado = 1;
          while (check_pressed_1()){}
        } else if (check_pressed_2()){
          estado = 2;
          while (check_pressed_2()){}
        }
      break;
      case 1:
        led_green_on();
        led_red_off();
        if (check_pressed_1()){
          estado = 0;
          while (check_pressed_1()){}
        } else if (check_pressed_2()){
          estado = 3;
          while (check_pressed_2()){}
        }
      break;
      case 2:
        led_green_off();
        led_red_on();
        if (check_pressed_1()){
          estado = 3;
          while (check_pressed_1()){}
        } else if (check_pressed_2()){
          estado = 0;
          while (check_pressed_2()){}
        }
      break;
      case 3:
        led_green_on();
        led_red_on();
        if (check_pressed_1()){
          estado = 2;
          while (check_pressed_1()){}
        } else if (check_pressed_2()){
          estado = 1;
          while (check_pressed_2()){}
        }
      break;
    }
  }

  return 0;
}
