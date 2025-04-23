/*
 * prelab6.c
 *
 * Created: 4/22/2025 5:05:45 PM
 * Author: Luis Pedro Monterroso
 * Description: comunicacion usart
 */
/****************************************/
// Encabezado (Libraries)
#include <avr/io.h>
#include <avr/interrupt.h>
/****************************************/
// Function prototypes
void setup();
void initUART();
void writeChar(char caracter);
/****************************************/
// Main Function
int main(void)
{
	setup();
	writeChar('H');
	writeChar('o');
	writeChar('l');
	writeChar('a');
	writeChar('.');
	while (1)
	{
	}
}
/****************************************/
// NON-Interrupt subroutines
void setup(){
	cli();
	initUART();
	//portb como salida de los leds INICIALMENTE APAGADOS
	DDRB |= (1 << PORTB0)|(1 << PORTB1)|(1 << PORTB2)|(1 << PORTB3)|(1 << PORTB4)|(1 << PORTB5);
	PORTB &= ~( (1 << PORTB0)|(1 << PORTB1)|(1 << PORTB2)|(1 << PORTB3)|(1 << PORTB4)|(1 << PORTB5) );
	DDRC |= (1 << PORTC0)|(1 << PORTC1);
	PORTC &= ~((1 << PORTC0) | (1 << PORTC1));

	sei();
}
void initUART(){
	//SALIDA Y ENTRADA DEL RX Y TX
	DDRD |= (1 << DDD1);
	DDRD &= ~(1 << DDD0);
	//conf de banderas
	UCSR0A = 0;
	//configurar UCSR0B
	UCSR0B |= (1 << RXCIE0)|(1 << RXEN0)|(1 << TXEN0);
	//8 bits de datos, sin polaridad, 1 STOP BIT y asincrono
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);
	//configurar BAUDRATE, es decir velocidad de transmision. UBRR0 = 103 -> 9600 @ 16MHz
	UBRR0 = 103;
}
void writeChar(char caracter){
	//verificamos que se haya enviado cada dato, sino no enviamos
	uint8_t temporal = UCSR0A & (1 << UDRE0);
	while(temporal == 0){
		//esperamos a que este se lea
		temporal = UCSR0A & (1 << UDRE0);
	}
	
	UDR0 = caracter;
	
}
/****************************************/
// Interrupt routines
ISR(USART0_RX_vect){
	//aca es cuando recibe algo
	char temporal = UDR0;
	writeChar(temporal);
	
	// 1. Escribir bits 0-5 en PORTB0-PORTB5
	PORTB = (PORTB & 0b11000000) | (temporal & 0b00111111);
	// 2. Escribir bits 6-7 en PORTC0-PORTC1
	PORTC = (PORTC & 0b11111100) | ((temporal >> 6) & (0b00000011));
	
	/*
	PORTB = temporal;
	PORTC = (temporal >> 6);
	*/
}



