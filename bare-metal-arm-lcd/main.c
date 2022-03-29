#include "MKL46Z4.h"
#include "lcd.h"

// LED (RG)
// LED_GREEN = PTD5 (pin 98)
// LED_RED = PTE29 (pin 26)

// SWICHES
// RIGHT (SW1) = PTC3 (pin 73)
// LEFT (SW2) = PTC12 (pin 88)

// Enable IRCLK (Internal Reference Clock)
// see Chapter 24 in MCU doc
void irclk_ini()
{
  MCG->C1 = MCG_C1_IRCLKEN(1) | MCG_C1_IREFSTEN(1);
  MCG->C2 = MCG_C2_IRCS(0); //0 32KHZ internal reference clock; 1= 4MHz irc
}

void delay(void)
{
  volatile int i;

  for (i = 0; i < 1000000; i++);
}

// RIGHT_SWITCH (SW1) = PTC3
void sw1_ini()
{
  SIM->COPC = 0;
  SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
  PORTC->PCR[3] |= PORT_PCR_MUX(1) | PORT_PCR_PE(1)|PORT_PCR_IRQC(10);
  GPIOC->PDDR &= ~(1 << 3);
}

// LEFT_SWITCH (SW2) = PTC12
void sw2_ini()
{
  SIM->COPC = 0;
  SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
  PORTC->PCR[12] |= PORT_PCR_MUX(1) | PORT_PCR_PE(1)|PORT_PCR_IRQC(10);
  GPIOC->PDDR &= ~(1 << 12);
}

int sw1_check()
{
  return( !(GPIOC->PDIR & (1 << 3)) );
}

int sw2_check()
{
  return( !(GPIOC->PDIR & (1 << 12)) );
}

// RIGHT_SWITCH (SW1) = PTC3
// LEFT_SWITCH (SW2) = PTC12
void sws_ini()
{
  SIM->COPC = 0;
  SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
  PORTC->PCR[3] |= PORT_PCR_MUX(1) | PORT_PCR_PE(1);
  PORTC->PCR[12] |= PORT_PCR_MUX(1) | PORT_PCR_PE(1);
  GPIOC->PDDR &= ~(1 << 3 | 1 << 12);
}

// LED_GREEN = PTD5
void led_green_ini()
{
  SIM->COPC = 0;
  SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
  PORTD->PCR[5] = PORT_PCR_MUX(1);
  GPIOD->PDDR |= (1 << 5);
  GPIOD->PSOR = (1 << 5);
}

void led_green_toggle()
{
  GPIOD->PTOR = (1 << 5);
}

// LED_RED = PTE29
void led_red_ini()
{
  SIM->COPC = 0;
  SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
  PORTE->PCR[29] = PORT_PCR_MUX(1);
  GPIOE->PDDR |= (1 << 29);
  GPIOE->PSOR = (1 << 29);
}

void led_red_toggle(void)
{
  GPIOE->PTOR = (1 << 29);
}

// LED_RED = PTE29
// LED_GREEN = PTD5
void leds_ini()
{
  SIM->COPC = 0;
  SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK;
  PORTD->PCR[5] = PORT_PCR_MUX(1);
  PORTE->PCR[29] = PORT_PCR_MUX(1);
  GPIOD->PDDR |= (1 << 5);
  GPIOE->PDDR |= (1 << 29);
  // both LEDS off after init
  GPIOD->PSOR = (1 << 5);
  GPIOE->PSOR = (1 << 29);
}

// Hit condition: (else, it is a miss)
// - Left switch matches red light
// - Right switch matches green light

int estado; //estado 0 = inicio, 1= led verde, sin respuesta, 2 = respuesta correcta, 3= respuesta incorrecta, 4= led rojo, sin respuesta
int aciertos;
int fallos;

void PORTDIntHandler(void){ //Handler para la interrupcion por puerto C o D, aunque solo ponga D
	
  if (estado == 1){//Si deberiamos pulsar el switch 1
    if (sw1_check()){ //Si el boton pulsado es el 1, se da por bueno. Si no, es un fallo
      estado = 2;
    } else {
      estado = 3;
    }
  } else if (estado == 4){//Si deberiamos pulsar el switch 2
    if (sw2_check()){ //Si el boton pulsado es el 2, se da por bueno. Si no, es un fallo
      estado = 2;
    } else {
      estado = 3;
    }
  }
  PORTC->PCR[3] |= PORT_PCR_ISF_MASK;
  PORTC->PCR[12] |= PORT_PCR_ISF_MASK;
	
	

}

void lcd_clear(){
  LCD->WF8B[LCD_FRONTPLANE0] = LCD_CLEAR;
  LCD->WF8B[LCD_FRONTPLANE1] = LCD_CLEAR;
  LCD->WF8B[LCD_FRONTPLANE2] = LCD_CLEAR;
  LCD->WF8B[LCD_FRONTPLANE3] = LCD_CLEAR;
  LCD->WF8B[LCD_FRONTPLANE4] = LCD_CLEAR;
  LCD->WF8B[LCD_FRONTPLANE5] = LCD_CLEAR;
  LCD->WF8B[LCD_FRONTPLANE6] = LCD_CLEAR;
  LCD->WF8B[LCD_FRONTPLANE7] = LCD_CLEAR;
}

int main(void)
{
  NVIC_SetPriority(31, 0);
  irclk_ini(); // Enable internal ref clk to use by LCD

  lcd_ini();
  sw1_ini();
  sw2_ini();
  leds_ini();

  // 'Random' sequence :-)
  volatile unsigned int sequence = 0x32B14D98,
    index = 0;

  
  estado = 0;
  aciertos = 0;
  fallos = 0;
  lcd_display_time(0,0);
  NVIC_EnableIRQ(31);
  while (index < 32) {
    if (sequence & (1 << index)) { //odd
      //
      // Switch on green led
      led_green_toggle();
      estado = 1;
      // [...]
      //
    } else { //even
      //
      // Switch on red led
      led_red_toggle();
      estado = 4;
      // [...]
      //
    }
    index++;
    delay();
    GPIOD->PSOR = (1 << 5); //Apagamos ambos leds
    GPIOE->PSOR = (1 << 29);
    switch (estado){
      case 1://Si no se pulsa un boton o se pulsa el incorrecto, da fallo. Si no, acierto
      case 3:
      case 4:
        fallos++;
        break;
      case 2:
        aciertos++;
        break;
    }
    estado = 0;//reiniciamos el estado
    lcd_display_time(aciertos,fallos);
    delay();
  }

  while (1) {
    lcd_display_time(aciertos,fallos);
    delay();
    lcd_clear();
    delay();
  }

  return 0;
}
