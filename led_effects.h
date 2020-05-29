/*
* led_effects.h
*
* Created: 28.10.2015 21:04:35
* Author: Nik
*/


#ifndef __LED_EFFECTS_H__
#define __LED_EFFECTS_H__
#include "led_ws2812.h"
#include "task_query.h"
#include "avr/pgmspace.h"
#include "avr/eeprom.h"
#include <stdlib.h>
#define NUM_OF_COLOR_EFFECTS 5
#define NUM_OF_BR_EFFECTS 4
typedef enum {RAINBOW,COLOR_RING,FLAME,COLD,WARM,COLD_WHITE,WARM_WHITE,NO_COLOR} T_COLOR_PALETTE;
typedef enum {FALSE,TRUE} T_BOOL;

void SetBrightnessEffect(uint8_t effect,uint8_t speed);
void NextBrightnessEffect();
void NextPalette();
void InitLedColors();
void UpdateAll();
void RandomAllEffect();
void NextEffect();
void InitWS2110();
void InitTestWS2110();
void SetColorEffect(uint8_t effect,uint8_t speed);
void NextColorEffect();
void UpdateBrightness();
void RandomColor();
void UpdateColor();




T_PIXEL ColorFromPalette(T_COLOR_PALETTE palette,uint8_t colorInd);

#endif
