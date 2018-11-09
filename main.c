#define F_CPU 1000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

#include <util/delay.h>

volatile int timer_overflow_count=0;
unsigned int count=0;
unsigned int lastADCval=0;
unsigned int ADCval=0;


ISR(TIM0_COMPA_vect){
	if (++timer_overflow_count < count) PORTB |= (1 << DDB2);
	else PORTB &= ~(1 << DDB2);											//Software PWM Algo
	if (timer_overflow_count>127) timer_overflow_count=0;
}

int main (void) {

	DDRB=0b0100;
	DIDR0 |=(1 <<ADC1D);
	PORTB=0b0100;
	PRR &= ~(1<<PRADC);
	ADMUX = 1<<MUX0;;
	ADCSRA = 1<<ADEN | 3<<ADPS0; 
	TIMSK0 |= (1<<OCIE0A);
	TCCR0A =0x00;
	TCCR0B |= (1<<WGM02);
	OCR0AH =0x00;
	OCR0AL = 1;
	sei();
	ADCSRA = ADCSRA | 1<<ADSC;    // Start
	while (ADCSRA & 1<<ADSC);     // Wait while conversion in progress
	ADCval=ADCL;
	lastADCval=ADCval;
	count = 127-(ADCval>>1);
	
	
	TCCR0B|= (1<<CS00);
	while (1){
		ADCSRA = ADCSRA | 1<<ADSC;    // Start
		while (ADCSRA & 1<<ADSC);     // Wait while conversion in progress
		ADCval=ADCL;
		if ((lastADCval - ADCval)>5 || (lastADCval- ADCval) < -5 ){
			count = 127-(ADCL>>1);
			lastADCval=ADCval;
		}
		
	}
}