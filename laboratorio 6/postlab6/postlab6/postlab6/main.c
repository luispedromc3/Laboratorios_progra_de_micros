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
#include <stdlib.h> // para itoa()
uint8_t bandera = 0;
/****************************************/
// Function prototypes
void setup();
void initUART();
void writeChar(char caracter);
void writeString(char* string);
void mostrarMenu();
void initADC();
void writeNumber8(uint8_t number);
uint8_t valor_adc = 0;
/****************************************/
// Main Function
int main(void)
{
	setup();
	mostrarMenu();
	while (1)
	{
	}
}
/****************************************/
// NON-Interrupt subroutines
void setup(){
	cli();
	initUART();
	initADC();
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
void writeString(char* string){
	for (uint16_t i=0; *(string+i) != '\0' ; i++){
		writeChar(*(string+i));
	}
}
void mostrarMenu(){
		writeString("----------------------------------------------\n");
		writeString("---------------elija una opcion---------------\n");
		writeString("----------------------------------------------\n");
		writeString("    1.) leer potenciometro\n");
		writeString("    2.) mostrar codigo ASCII\n");
		writeString("----------------------------------------------\n");
		writeString("----------------------------------------------\n");
}
void initADC(){
	ADMUX = 0;
	ADMUX |= (1 << REFS0) ; //VCC COMO COMPARADOR
	ADMUX |= (1 << ADLAR) ; //JUSTIFICADO A LA IZQUIERDA USAR HIGH
	ADMUX |= (1 << MUX2)|(1 << MUX1)|(1 << MUX0); //ADC7
	ADCSRA = 0;
	ADCSRA |= (1 << ADPS1) | (1 << ADPS0); //PRESCALER DE 128
	ADCSRA |= (1 << ADEN);
	ADCSRA |= (1 << ADIE); //ACTIVAR INTERRUPCIONES
	ADCSRA |= (1 << ADSC); //EMPEZAR EL ADC
}
void writeNumber8(uint8_t number) {
	char buffer[4]; // Máx: "255\0"
	itoa(number, buffer, 10); // Convierte número a string en base 10
	writeString(buffer);
}

/****************************************/
// Interrupt routines
ISR(USART0_RX_vect){
	char temporal = UDR0;
	if(temporal == '1'){
		if(bandera == 1){
			// aca solo entra si queremos mostrar el codigo del numero 1
			writeString("\n");
			writeString("\n");
			writeString("ASCII EN LOS LEDS \n");
			writeString("\n");
			// 1. Escribir bits 0-5 en PORTB0-PORTB5
			PORTB = (PORTB & 0b11000000) | (temporal & 0b00111111);
			// 2. Escribir bits 6-7 en PORTC0-PORTC1
			PORTC = (PORTC & 0b11111100) | ((temporal >> 6) & (0b00000011));
			mostrarMenu();
			bandera = 0;
		}else{
			//eligio la opcion 1
			//potenciometro
			writeString("\n");
			writeString("El valor del potenciometro es: ");
			writeNumber8(valor_adc);
			writeString("\n");
			// 1. Escribir bits 0-5 en PORTB0-PORTB5
			PORTB = (PORTB & 0b11000000) | (valor_adc & 0b00111111);
			// 2. Escribir bits 6-7 en PORTC0-PORTC1
			PORTC = (PORTC & 0b11111100) | ((valor_adc >> 6) & (0b00000011));
			mostrarMenu();
		}
		
	}else if(temporal == '2'){
		//ascii
		
		writeString("\n");
		writeString("INGRESE UN CARACTER: ");
		
		PORTB = 0;
		PORTC = 0;
		bandera = 1;

	}else{
		if(bandera == 1){
			writeString("\n");
			writeString("\n");
			writeString("ASCII EN LOS LEDS \n");
			writeString("\n");
			// 1. Escribir bits 0-5 en PORTB0-PORTB5
			PORTB = (PORTB & 0b11000000) | (temporal & 0b00111111);
			// 2. Escribir bits 6-7 en PORTC0-PORTC1
			PORTC = (PORTC & 0b11111100) | ((temporal >> 6) & (0b00000011));
			mostrarMenu();
			bandera = 0;
		}else{
			PORTC = 0;
			PORTB = 0;
			writeString("\n");
			writeString("ERROR!!!!!!!!!!!!! VALOR INVALIDO !!!!!!!!!!!!! \n");
			writeString("\n");
			mostrarMenu();
		}
		
	}
}

ISR(ADC_vect){
	valor_adc = ADCH;
	ADCSRA |= (1 << ADSC); //EMPEZAR EL ADC
}

