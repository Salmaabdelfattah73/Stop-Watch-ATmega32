/*
 * pro.c
 *
 *  Created on: Sep 13, 2023
 *      Author: Salma Essam
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
unsigned char g_tick = 0;//count the number of timer ticks
unsigned char count_second_flag = 0; //flag will be set when the timer count 1 second
unsigned char second_count = 0;
unsigned char minute_count = 0;
unsigned char hour_count = 0;
ISR(TIMER1_COMPA_vect){
	count_second_flag = 1;
}
void timer1_init_CTC_mode(int tick){
	TCNT1 = 0;
	OCR1A  = tick;
	TIMSK |= (1<<OCIE1A); //enable compare interrupt
	TCCR1A = (1<<FOC1A) | (1<<FOC1B);
	TCCR1B = (1<<WGM12) | (1<<CS10) | (1<<CS12);
}
ISR(INT0_vect){
	second_count = 0;
	minute_count = 0;
	hour_count = 0;
}
ISR(INT1_vect){
	TCCR1B &= 0xF8;
}
ISR(INT2_vect){
	TCCR1B |= (1<<CS10) | (1<<CS12);
}
void INT0_Init(void){
	GICR  |= (1<<INT0);
	MCUCR &= ~(1<<ISC00);
	MCUCR |= (1<<ISC01);
}
void INT1_Init(void){
	MCUCR |= (1<<ISC10);
	MCUCR |= (1<<ISC11);
	GICR  |= (1<<INT1);
}
void INT2_Init(void){
	MCUCSR &= !(1<<ISC2);
	GICR   |= (1<<INT2);
}
int main(void){
	DDRD  &= (~(1<<PD2));
	PORTD |= (1<<PD2);
	DDRD  &= (~(1<<PD3));
	DDRB   &= (~(1<<PB2));
	PORTB |= (1<<PB2);
	DDRA |= 0x3F;
	DDRC |= 0x0F;
	PORTA |= 0x3F;
	PORTC &= 0xF0;
	SREG  |= (1<<7);
	timer1_init_CTC_mode(1000);
	INT0_Init();
	INT1_Init();
	INT2_Init();
    while(1){
    	if(count_second_flag == 1)
    	{second_count++;
    		if(second_count == 60){
    			second_count = 0;
    			minute_count++;
    		}
    		if(minute_count == 60){
    			minute_count = 0;
    			hour_count++;
    		}
    		if(hour_count == 24){
    			hour_count = 0;
    		}
    	    count_second_flag = 0;
    	}
    	else{
    		PORTA = (PORTA & 0xC0) | 0x01;// out the number of seconds
    		PORTC = (PORTC & 0xF0) | (second_count%10);
    		_delay_ms(1);
    		PORTA = (PORTA & 0xC0) | 0x02;
    		PORTC = (PORTC & 0xF0) | (second_count/10);
    		_delay_ms(1);
    		PORTA = (PORTA & 0xC0) | 0x04;// out the number of minutes
    		PORTC = (PORTC & 0xF0) | (minute_count%10);
    		_delay_ms(1);
    		PORTA = (PORTA & 0xC0) | 0x08;
    	    PORTC = (PORTC & 0xF0) | (minute_count/10);
    	    _delay_ms(1);
    	    PORTA = (PORTA & 0xC0) | 0x10;// out the number of hours
    	    PORTC = (PORTC & 0xF0) | (hour_count%10);
    	    _delay_ms(1);
    	    PORTA = (PORTA & 0xC0) | 0x20;
    	    PORTC = (PORTC & 0xF0) | (hour_count/10);
    	    _delay_ms(1);
    	}
    }
}
