/*
 * digi_new_year_led.ino
 *
 * Created: 11/22/2017 11:10:23 AM
 * Author: USER
 * --- PB0 - PWM0
 * --- PB1 - PWM1
 * --- PB2 - INT0
 * --- PB3 - WS2812
 * --- PB4 - ----
 * --- PB5 - ADC_Keyboard
 */ 
#include "task_query.h"
#include "led_ws2812.h"
#include "led_effects.h"
#include "adc_keyboard.h"
#include "ac_pwm.h"
#include "ac_pwm_effects.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>

inline void InitSysTimer(){
	//�������� ������ � ������������� ��� � 1 ��
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



void InitBlink(){
		
		DDRB |= 1<<PB1;
		PORTB |= 1<<PB1;
}


void Blink(){
	//	PORTB ^= 1<< PB1;

}




void callbac(uint8_t data){
	switch (data)
	{
	case KEY0:{
		NextEffect();
	}
		break;
	case KEY1:
		{
			NextACEffect();
}
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
			case KEY1:{
				ToggleACRandomEffects();
			}
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
	//InitTestWS2110();
	InitAdcKeyboard(callbac,long_press);
	InitACEffects();

	sei();
	AddTask(ScanKayboard,0,100);
	
	
	while (1)
	{
		Dispatcher();
	}
//	return 0;
}

