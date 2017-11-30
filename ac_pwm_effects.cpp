/* 
* ac_pwm_effects.cpp
*
* Created: 30.11.2017 10:40:03
* Author: USER
*/


#include "ac_pwm_effects.h"
uint8_t linBr0[16] = {	15,60,80,100,		//Числа для микроламп
						110,120,130,140,
						150,160,170,180,
						190,200,210,230};

#define LIN_AC_BRIGH(x) linBr0[15 - ((x)%16)]
#define INV_LIN_AC_BRIGH(x) linBr0[x%16]
#define MAX_AC_BRIGH	linBr0[0]
#define MIN_AC_BRIGH	linBr0[15]

#ifndef TRUE
	#define TRUE 1
#endif

#ifndef FALSE
	#define FALSE 0
#endif

static uint8_t  duty = 1,sign,
				g_acPwmSpeed = 100,g_acPwmMode = 2;

EEMEM uint8_t g_eeprom_acPwmMode = 0;
#define NUM_OF_AC_EFFECTS 7
#define NO_EFFECT			0
#define UPDOWNSYNC			1
#define UPDOWNANTISYNC		2
#define WAWE				3
#define FAST_UPDOWNSYNC		4
#define FAST_UPDOWNANTISYNC 5
#define SERIAL_UP_DOWN		6

void LinUpDownACSynphase(){	
	SetACPWMDuty(LIN_AC_BRIGH(duty),LIN_AC_BRIGH(duty));	
	(sign)?(duty = duty - 1):( duty = duty + 1);
	if(duty >= 15) sign = TRUE;
	if(duty == 0) sign = FALSE;
}

void LinUpDownACAntiphase(){
	SetACPWMDuty(INV_LIN_AC_BRIGH(duty),LIN_AC_BRIGH(duty));
	(sign)?(duty = duty - 1):( duty = duty + 1);
	if(duty >= 15) sign = TRUE;
	if(duty == 0) sign = FALSE;
}

void WaweAC(){
	if (sign)
	{
		SetACPWMDuty(MAX_AC_BRIGH,MIN_AC_BRIGH);
		sign = FALSE;
	}else{
		SetACPWMDuty(MIN_AC_BRIGH,MAX_AC_BRIGH);
		sign = TRUE;
	}	
}

void SerialUpDownAC(){//Плавно разгорается и тухнет один цвет, после него плавно разгорается и тухнет другой
	if(sign & 0x80)	SetACPWMDuty(MIN_AC_BRIGH,LIN_AC_BRIGH(duty));
		else SetACPWMDuty(LIN_AC_BRIGH(duty),MIN_AC_BRIGH);
		
		(sign & 0x0F)?(duty = duty - 1):( duty = duty + 1);
		if(duty >= 15) sign = (sign & 0xF0) | TRUE;
		if(duty == 0) {
			sign =  (sign & 0xF0) | FALSE;
			sign ^= 0x80;
			};
}

void UpdateACPwm(){
	switch (g_acPwmMode & ~0x80)
	{
	case NO_EFFECT:{
		SetACPWMDuty(MAX_AC_BRIGH,MAX_AC_BRIGH);
		g_acPwmSpeed = 100;
		}
		break;
	case UPDOWNSYNC:{
		LinUpDownACSynphase();
		g_acPwmSpeed = 100;
		}
		break;
	case UPDOWNANTISYNC:{
		LinUpDownACAntiphase();
		g_acPwmSpeed = 100;
		}
		break;
	case WAWE:{
		WaweAC();
		g_acPwmSpeed = 200;
		}
		break;
	case FAST_UPDOWNSYNC:{
		LinUpDownACSynphase();
		g_acPwmSpeed = 25;
		}break;
		
	case FAST_UPDOWNANTISYNC:{
		LinUpDownACAntiphase();
		g_acPwmSpeed = 25;
		}break;
	case SERIAL_UP_DOWN:{
		SerialUpDownAC();
		g_acPwmSpeed = 100;
		}break;
	}
	AddTask(UpdateACPwm,0,g_acPwmSpeed);
}

void RandomChangeACEffect(){
	if ((g_acPwmMode & 0x80) == 0x80)
	{
		g_acPwmMode = rand()%NUM_OF_AC_EFFECTS;
		g_acPwmMode |= 0x80;
	}
}

void InitACEffects(){
	InitACPWM();
	SetACPWMDuty(MIN_AC_BRIGH,MAX_AC_BRIGH);
	g_acPwmMode = eeprom_read_byte(&g_eeprom_acPwmMode);
	AddTask(UpdateACPwm,0,g_acPwmSpeed);
	AddTask(RandomChangeACEffect,0,3000);
	
}

void NextACEffect(){
	
	g_acPwmMode = (g_acPwmMode & 0x80) | (( g_acPwmMode & ~0x80) + 1 )%NUM_OF_AC_EFFECTS;
	eeprom_update_byte(&g_eeprom_acPwmMode,g_acPwmMode);
	 UpdateACPwm();	
	};

void ToggleACRandomEffects(){
	if ((g_acPwmMode & 0x80) == 0x80)
	{
		g_acPwmMode &= ~0x80;
		eeprom_update_byte(&g_eeprom_acPwmMode,g_acPwmMode);
		SetACPWMDuty(MIN_AC_BRIGH,MAX_AC_BRIGH);
	}else{
		g_acPwmMode |= 0x80;
		eeprom_update_byte(&g_eeprom_acPwmMode,g_acPwmMode);	
		SetACPWMDuty(MAX_AC_BRIGH,MIN_AC_BRIGH);
		AddTask(RandomChangeACEffect,0,3000);
	}
	AddTask(UpdateACPwm,5000,g_acPwmSpeed);
	
}