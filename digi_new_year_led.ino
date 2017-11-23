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

void InitSysTimer(){
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


void InitBlink(){
		
		DDRB |= 1<<PB1;
		PORTB |= 1<<PB1;
}
T_PIXEL tempPix = {0,0x00,0xff,0x00};
	

void Blink(){
		PORTB ^= 1<< PB1;

}



#pragma inline=forced
static void InitWS2110(){
	INIT_LED_GPIO();
	InitLedColors();
}

void callbac(uint8_t data){
	switch (data)
	{
	case KEY0:{
	NextColorEffect();
	}
		break;
	case KEY1:
	NextBrightnessEffect();
		break;
	case KEY2:
	SetColorEffect(2,5);
	break;
	case KEY3:
	SetColorEffect(3,5);
	break;
	
	}
}
void long_press(uint8_t key){
	NextBrightnessEffect();
}

int main(){
	
	InitBlink();
	InitSysTimer();	
	InitWS2110();
	InitAdcKeyboard(callbac,long_press);
	sei();
	
	AddTask(Blink,0,1000);
	//AddTask(UpdateColor,0,0x50);
	AddTask(SendAllPixels,0,10);
	AddTask(ScanKayboard,0,100);
	while (1)
	{
		Dispatcher();
	}
	return 0;
}

