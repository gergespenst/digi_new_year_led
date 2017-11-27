/* 
* ac_pwm.h
* ШИМ синхронизированный с сетью для изменения яркости ламп
* Работает на прерываниях от INT0 и TIMER1
* Created: 27.11.2017 10:20:30
* Author: USER
*/


#ifndef __AC_PWM_H__
#define __AC_PWM_H__

#include <avr/interrupt.h>
#include <avr/io.h>
#define PWM_PORT	PORTB
#define PWM_DDR		DDRB
#define PWM0_PIN	PB0 
#define PWM1_PIN	PB1
#define PWM2_PIN	PB4

#define INT0_DDR	DDRB
#define INT0_PORT	PORTB
#define INT0_PIN	PB2


void InitACPWM();

void SetACPWMPeriod(uint8_t pwm0_per,uint8_t pwm1_per);

#endif //__AC_PWM_H__
