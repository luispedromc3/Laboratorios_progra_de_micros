/*
 * prelab5.c
 *
 * Created: 4/9/2025 11:28:47 AM
 * Author: Luis Pedro Monterroso
 * Description: controlo dos servos y el brillo de un led. 
 */
/****************************************/
// Encabezado (Libraries)
#include <avr/io.h>
#include <avr/interrupt.h>
#include "PWM1/PWM1.h"
#include "PWM2/PWM2.h"
#include "PWM_MANUAL/PWM_MANUAL.h"
uint8_t contador;
uint8_t contador_pwm;
uint8_t VALOR_ADC_LED;
/****************************************/
// Function prototypes
void setup();
void initADC();
void PMW1CONFIG(uint16_t top, uint16_t prescaler);
void PWM2CONFiG();
void initTIMER0();
/****************************************/
// Main Function
int main(void)
{
	setup();
	PMW1CONFIG(312,64);
	PWM2CONFIG();
	initTIMER0();
	while (1)
	{
	
	}
}
/****************************************/
// NON-Interrupt subroutines
void setup() {
	cli();
	
	//CONFIGURAR presecaler general del CLK A 1MHz
	CLKPR = (1 << CLKPCE);
	CLKPR = (1 << CLKPS2);
	
	//adc
	initADC();
	//SALIDAS DE LOS DOS PWM
	DDRB |= (1 << DDB1);
	DDRB |= (1 << PORTB3);
	DDRB |= (1 << PORTB2); //PWM MANUAL
	PORTB &= ~(1 << PORTB2); //INICIO EL PWM INICIALMENTE APAGADO
	//INICIALIZAR VARIABLES
	contador_pwm = 0;
	contador = 0;
	VALOR_ADC_LED = 0;
	
	sei();
}
void initADC(){
	ADMUX = 0;
	ADMUX |= (1 << REFS0) ; //VCC COMO COMPARADOR
	ADMUX |= (1 << ADLAR) ; //JUSTIFICADO A LA IZQUIERDA USAR HIGH
	ADMUX |= (1 << MUX2)|(1 << MUX1); //ADC6
	ADCSRA = 0;
	ADCSRA |= (1 << ADPS1) | (1 << ADPS0); //PRESCALER DE 128
	ADCSRA |= (1 << ADEN);
	ADCSRA |= (1 << ADIE); //ACTIVAR INTERRUPCIONES
	ADCSRA |= (1 << ADSC); //EMPEZAR EL ADC
}

/****************************************/
// Interrupt routines
ISR(ADC_vect){
//En esta interrupcion, leo y despues cambio al siguiente adc a leer.
	switch(contador){
		case 1:
			OCR1A = ((ADCH*30/255) +7);
			ADMUX = 0;
			ADMUX |= (1 << REFS0) ; //VCC COMO COMPARADOR
			ADMUX |= (1 << ADLAR) ; //JUSTIFICADO A LA IZQUIERDA USAR HIGH
			ADMUX |= (1 << MUX2)|(1 << MUX1)|(1 << MUX0); //ADC7
			break;
		case 2:
			OCR2A	= ((ADCH*30/255) +7) ;
			ADMUX = 0;
			ADMUX |= (1 << REFS0) ; //VCC COMO COMPARADOR
			ADMUX |= (1 << ADLAR) ; //JUSTIFICADO A LA IZQUIERDA USAR HIGH
			ADMUX |= (1 << MUX2)|(1 << MUX0); //ADC5
			break;
		case 3:
			VALOR_ADC_LED = ADCH ; //  / 255
			if (TCNT0 >= VALOR_ADC_LED){
				PORTB &= ~(1 << PORTB2);
			}
			break;
		default:
			ADMUX = 0;
			ADMUX |= (1 << REFS0) ; //VCC COMO COMPARADOR
			ADMUX |= (1 << ADLAR) ; //JUSTIFICADO A LA IZQUIERDA USAR HIGH
			ADMUX |= (1 << MUX2)|(1 << MUX1); //ADC6
			contador = 0;
			break;
	}
	contador++;
	
	ADCSRA |= (1 << ADSC); //EMPEZAR EL ADC
}

ISR(TIMER0_OVF_vect){
//pongo en high el pwm manualmente, este se apagara en la interrupcion del adc cuando el valor sea igual o mayor al de TCNT0
	TCNT0 = 50; 
	PORTB |= (1 << PORTB2);
	if (VALOR_ADC_LED <= 20){
		PORTB &= ~(1 << PORTB2); //si me acerco a un valor cercano a 0 apago el led para evitar que parpadee o que no se apague completamente
	}

}