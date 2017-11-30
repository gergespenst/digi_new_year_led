/* 
* ac_pwm_effects.h
*
* Created: 30.11.2017 10:40:03
* Author: USER
*/


#ifndef __AC_PWM_EFFECTS_H__
#define __AC_PWM_EFFECTS_H__
#include "ac_pwm.h"
#include "task_query.h"
#include <avr/eeprom.h>
#include <stdlib.h>

void InitACEffects();
void NextACEffect();

void ToggleACRandomEffects();

#endif //__AC_PWM_EFFECTS_H__
