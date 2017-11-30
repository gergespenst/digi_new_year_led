/* 
* ac_pwm.cpp
*
* Created: 27.11.2017 10:20:29
* Author: USER
*/


#include "ac_pwm.h"
static uint8_t g_netPeriod = 0xFF;
static uint8_t g_pwmPeriod[3] = {0xff,0xff,0xff};
	
inline void InitNetSync(){
	//Первый запуск устанавливаем от спадающего фронта
	MCUCR = (0 << ISC00) | (1 << ISC01);
	//Разрешаем INT0
	GIMSK |= (1 << 6);
	GIFR  |= (1 << 6);
	//Настраиваем вход INT0 на вход
	INT0_DDR &= ~(1 << INT0_PIN);
	INT0_PORT |= (1 << INT0_PIN);
	//Настраиваем выходы ШИМ
	PWM_DDR |= ((1 << PWM0_PIN) | (1 << PWM1_PIN) | (1 << PWM2_PIN));
	PWM_PORT &= ~((1 << PWM0_PIN) | (1 << PWM1_PIN) | (1 << PWM2_PIN));
}

void InitACPWM(){
	InitNetSync();
	TCCR1 = 0;
	GTCCR = 0;
	TCNT1 = 0;
	OCR1A = 0 ;
	OCR1B = 0;
	OCR1C = 0;
	TIMSK |=  (1 << OCIE1A)| (1 << TOIE1) | (1 << OCIE1B) ;
}

void SetACPWMDuty(uint8_t pwm0_per,uint8_t pwm1_per){
	g_pwmPeriod[0] = pwm0_per;
	g_pwmPeriod[1] = pwm1_per;
}





ISR(TIM1_COMPA_vect){
	PWM_PORT |= (1<< PWM0_PIN);
}

ISR(TIM1_COMPB_vect){
	PWM_PORT |= (1<< PWM1_PIN);
	
}



ISR(TIM1_OVF_vect){
	TCCR1 &= ~0x0F;
	TCNT1 = 0;
	PWM_PORT &= ~((1 << PWM0_PIN) | (1 << PWM1_PIN) | (1 << PWM2_PIN));
}

ISR(INT0_vect){
	//после первого же срабатывания переключаемся на срабатывание по обоим фронтам
	// сделано для того чтоб мерять длительность между нулями
	
	
	MCUCR = (1 << ISC00) | (0 << ISC01);
	if ((TCCR1 & 0x0F) && (TCNT1 > (g_netPeriod >> 4)) && (PINB & (1 << PB2)))
	{
		g_netPeriod = TCNT1;
		TCCR1 &= ~0x0F;
		TCNT1 = 0;
		PWM_PORT &= ~((1 << PWM0_PIN) | (1 << PWM1_PIN) | (1 << PWM2_PIN));
	}else
	{
		TCCR1 = (1 << CS13) | (0 << CS12) | (1 << CS11) | (1 << CS10);
		OCR1A = (g_netPeriod*g_pwmPeriod[0])>>8;
		OCR1B = (g_netPeriod*g_pwmPeriod[1])>>8;
	}
	
}
