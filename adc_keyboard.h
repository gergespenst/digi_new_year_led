/* 
* adc_keyboard.h
*
* Created: 22.11.2017 15:10:54
* Author: USER
*/


#ifndef __ADC_KEYBOARD_H__
#define __ADC_KEYBOARD_H__

#include "task_query.h"
#define NUM_OF_KEYS 4
#define KEY0 1
#define KEY1 2
#define KEY2 3
#define KEY3 4
#define NOP  5
#define LONG_PRESS 10 //������������ ������� ������� � �������� ������� ��������� ������������ ������

typedef void (*PCALLFUNC)(uint8_t);

inline void InitAdcKeyboard(PCALLFUNC func);
void InitAdcKeyboard(PCALLFUNC pressFunc,PCALLFUNC longPressFunc);
void ScanKayboard();

#endif //__ADC_KEYBOARD_H__
