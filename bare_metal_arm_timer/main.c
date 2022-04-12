#include "MKL46Z4.h"
#include "lcd.h"

// LED (RG)
// LED_GREEN = PTD5 (pin 98)
// LED_RED = PTE29 (pin 26)

// SWICHES
// RIGHT (SW1) = PTC3 (pin 73)
// LEFT (SW2) = PTC12 (pin 88)

//TPM0 en interrupt vector: 33

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

int estado; //estado 0 = parado, 1= contando 
int segundos;
int minutos;


void PORTDIntHandler(void){ //Handler para la interrupcion por puerto C o D, aunque solo ponga D
	
  if (estado == 0){//Si esta parado y se pulsa el boton 1, empieza a contar (se reinicia antes)
    if (sw1_check()){ //Empezamos a contar
      segundos = 0;
      minutos = 0;
      estado = 1;
    } 
  } else if (estado == 1){//Si esta contando y se pulsa el boton 2, deja de contar
    if (sw2_check()){ //Dejamos de contar
      estado = 0;
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

void clock_config(){
SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;//abre clock gate de tpm0
SIM->SOPT2 |= SIM_SOPT2_TPMSRC(3);//Escoge la fuente del reloj, en este caso MCGIRCLK
TPM0->SC = TPM_SC_CMOD(0);//Desactiva LPTPM
TPM0->SC |= TPM_SC_PS(6);//prescaler /32, 32Khz/32 = 1Khz

TPM0->MOD = TPM_MOD_MOD(100000); 
TPM0->SC |= TPM_SC_TOF_MASK; //Resetea bit de overflow
TPM0->SC = TPM_SC_CMOD(1);//Activa LPTPM


}

int main(void)
{
  NVIC_SetPriority(31, 0);
  irclk_ini(); // Enable internal ref clk to use by LCD

  lcd_ini();
  sw1_ini();
  sw2_ini();
  leds_ini();
  clock_config();

  
  estado = 0;
  segundos = 0;
  minutos = 0;
  lcd_display_time(0,0);
  NVIC_EnableIRQ(31); //Activa las interrupciones del canal 31 (botones)
  
  while(1){
    lcd_display_time(segundos, minutos);
    while((TPM0->SC & TPM_SC_TOF_MASK)!=TPM_SC_TOF_MASK);//Espera hasta que se produzca overflow (pasa el segundo)
    if(estado == 1){//Si esta contando
      segundos++;//Contamos +1 segundo
      if(segundos >= 60){ // Si pasa un minuto
        segundos = 0; //Reiniciamos los segundos
        minutos++; //Aumentamos los minutos
        if(minutos >= 99){ //Como no tenemos horas, contamos minutos hasta 99, y en caso de llegar hasta ahi paramos el cronometro
          estado = 0;
        }
      }
    }
    TPM0->SC |= TPM_SC_TOF_MASK; //Limpia el bit del overflow
    

  }

  

  return 0;
}
