/*
 * digi_new_year_led.ino
 *
 * Created: 11/22/2017 11:10:23 AM
 * Author: USER
 */ 
#include "task_query.h"
#include "led_ws2812.h"
#include "led_effects.h"
#include "adc_keyboard.h"
#include <avr/io.h>
#include <avr/interrupt.h>

inline void InitSysTimer(){
	//Системнй таймер с переполнением раз в 1 мс
	PLLCSR=0x00;
	// Timer/Counter 0 initialization
	// Clock source: System Clock
	// Clock value: 250,000 kHz
	// Mode: Normal top=0xFF
	// Start value: 0x05
	// -> 1 kHz overflow
	// OC0A output: Disconnected
	// OC0B output: Disconnected
	TCCR0A=0x00;
	TCCR0B=0x03;
	TCNT0=0x00;
	OCR0A=0x00;
	OCR0B=0x00;
	// Timer(s)/Counter(s) Interrupt(s) initialization
	TIMSK=0x02;

}



ISR(TIM0_OVF_vect){
	TCNT0=0x00;
	TimerProcess();	
}

inline void InitNetSync(){
	//Первый запуск устанавливаем от спадающего фронта
	MCUCR = (0 << ISC00) | (1 << ISC01);
	//Разрешаем INT0
	GIMSK |= (1 << 6);
	GIFR  |= (1 << 6);
	//Настраиваем вход INT0 yf вход
	DDRB &= ~(1 << PB2);
	PORTB |= (1 << PB2);
}

inline void InitPWMTimer(){
	//TCCR1 = (1 << CS13) | (0 << CS12) | (1 << CS11) | (1 << CS10);
	TCCR1 = 0;
	GTCCR = 0;
	TCNT1 = 0;
	OCR1A = 0 ;
	OCR1B = 0;
	OCR1C = 0;
	TIMSK |=  (1 << OCIE1A)| (1 << TOIE1) | (1 << OCIE1B);
}

void InitBlink(){
		
		DDRB |= 1<<PB1;
		PORTB |= 1<<PB1;
}


void Blink(){
		//PORTB ^= 1<< PB1;

}

static uint8_t period = 0xFF;

ISR(TIM1_COMPA_vect){
	PORTB ^= 1<< PB1;
}

ISR(TIM1_COMPB_vect){
	PORTB ^= 1<< PB1;
}

ISR(TIM1_OVF_vect){
	TCCR1 &= ~0x0F;
	TCNT1 = 0;
}

ISR(INT0_vect){
	//после первого же срабатывания переключаемся на срабатывание по обоим фронтам
	// сделано для того чтоб мерять длительность между нулями	
	MCUCR = (1 << ISC00) | (0 << ISC01);
	if ((TCCR1 & 0x0F) && (TCNT1 > (period >> 4)))
	{
		period = TCNT1;
		TCCR1 &= ~0x0F;
		TCNT1 = 0;
	}else
	{
		TCCR1 = (1 << CS13) | (0 << CS12) | (1 << CS11) | (1 << CS10);
		OCR1A = period >> 2;
		OCR1B = OCR1A + period >> 6 ;
		
	}
	//PORTB ^= 1<< PB1;	
}




void callbac(uint8_t data){
	switch (data)
	{
	case KEY0:{
		NextEffect();
	}
		break;
	case KEY1:
	
		break;
	case KEY2:
	
	break;
	case KEY3:
	
	break;
	
	}
}
void long_press(uint8_t key){
		switch (key)
		{
			case KEY0:{
				NextPalette();
			}
			break;
			case KEY1:
		
			break;
			case KEY2:
		
			break;
			case KEY3:
		
			break;
			
		}
}

__ATTR_NORETURN__ int main(){
	
	InitBlink();
	InitSysTimer();	
	InitWS2110();
	InitAdcKeyboard(callbac,long_press);
	InitNetSync();
	InitPWMTimer();
	sei();
	
	AddTask(Blink,0,1000);

	AddTask(ScanKayboard,0,100);
	while (1)
	{
		Dispatcher();
	}
//	return 0;
}

