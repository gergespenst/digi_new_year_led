/* 
* adc_keyboard.cpp
*
* Created: 22.11.2017 15:10:54
* Author: USER
*/


#include "adc_keyboard.h"

static PCALLFUNC g_press_func,g_long_press_func = 0;

void InitAdcKeyboard(PCALLFUNC pressFunc,PCALLFUNC longPressFunc){
	InitAdcKeyboard(pressFunc);
	g_long_press_func = longPressFunc;
}

void InitAdcKeyboard(PCALLFUNC func){
	//Init ADC
ADMUX =
(1 << ADLAR) |     // left shift result
(0 << REFS1) |     // Sets ref. voltage to VCC, bit 1
(0 << REFS0) |     // Sets ref. voltage to VCC, bit 0
(0 << MUX3)  |     // use ADC0 for input (PB4), MUX bit 3
(0 << MUX2)  |     // use ADC0 for input (PB4), MUX bit 2
(0 << MUX1)  |     // use ADC0 for input (PB4), MUX bit 1
(0 << MUX0);       // use ADC0 for input (PB4), MUX bit 0

ADCSRA =
(1 << ADEN)  |     // Enable ADC
(0 << ADPS2) |     // set prescaler to 64, bit 2
(0 << ADPS1) |     // set prescaler to 64, bit 1
(0 << ADPS0);      // set prescaler to 64, bit 0

	g_press_func = func;
}

#define MAX_ADC 0xFF
#define ANALOG_STEP (MAX_ADC/(NUM_OF_KEYS + 1))
#define ANALOG_DELTA (ANALOG_STEP/2)

void ScanKayboard(){
	static uint8_t pressed_key = NOP,press_counter;
	//��������� ���
	ADCSRA |= (1<<ADSC);
	while (ADCSRA & (1 << ADSC));
	//����� �������������� �������� ������� �� �����	
	uint8_t analogKeyVal = ADCH;
	//���������� ����� ������� ������, ���� ���������� ��� ������ ���������� NOP	
	uint8_t temp_key = 0;
	for (temp_key = 1; temp_key <= NUM_OF_KEYS + 1; temp_key++)
	{
		if ( (analogKeyVal >  (ANALOG_STEP*temp_key - ANALOG_DELTA))
		&& (analogKeyVal <  (ANALOG_STEP*temp_key + ANALOG_DELTA)) )
		{			
			break;
		}
	}
	
	if (temp_key == (pressed_key & 0x0F))//���� ������� ������� ��������� � ������� �����
	{
		if ((press_counter > LONG_PRESS) && ((pressed_key & 0x0F) != NOP))
		{//���� ������������ ������� ������ ��� ������� �������
			if(g_long_press_func) (*g_long_press_func)(pressed_key & 0x0F);	// ��������� ������� ���������� ����������� �������
			pressed_key |= 0x80;					// ������ ���� ����������� ������� � ���������� �������
			press_counter = 0;
		}else//����������� ������ ������������ �������
		{
			press_counter++;
		}
	}else
	{//���� ���������� � �������� �������� ������ �� ��������� �� � ������ ���� �� ������� ���� ����������� ������� ��������� ����������
		
		if (((pressed_key & 0x0F) != NOP) && ( (pressed_key & 0xF0) == 0))
		{
			(*g_press_func)(pressed_key & 0x0F);
		}
		// � ����� ������ ���������� ������������ ������� � ���������� �������
		press_counter = 0;
		pressed_key = temp_key;
		
	}
	
	
	
	
	
}