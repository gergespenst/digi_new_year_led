#include "led_effects.h"

static uint8_t 	g_colorEffect = 0, g_colorSpeed = 0x05,
				g_brEffect = 0, g_brSpeed = 0x05,
				g_allEffect = 0, g_allSpeed = 0x05;
T_COLOR_PALETTE g_currentPalette = COLOR_RING;
EEMEM T_COLOR_PALETTE g_eepromPalette = COLOR_RING;
EEMEM uint8_t g_eepromAllEffect = 0;
#define RED {0x00,0xFF,0x00,0xFF}
#define YELLOW {0xFF,0xFF,0x00,0xFF}
#define BLACK{0x00,0x00,0x00,0x00}
	
PROGMEM const T_PIXEL g_rainbow[7] = {
		{0x00,0xFF,0x00,0xFF},//red
		{0x7F,0xFF,0x00,0xFF},//orange
		{0xFF,0xFF,0x00,0xFF},//yellow
		{0xFF,0x00,0x00,0xFF},//green
		{0xaa,0x42,0xff,0xFF},//light b
		{0x00,0x00,0xFF,0xFF},//blue
		{0x00,0x8b,0xFF,0xFF}//violet	


};


#define BRIGHT_STEPS 16
uint8_t linBr[BRIGHT_STEPS] = {0 , 1 , 2 , 3 , 5 ,
					 8 , 13, 21, 34, 55,
					 89, 144, 233, 238, 243,
					 255};

#define IND_MAX_BRIGHT (BRIGHT_STEPS - 1)
#define MAX_BRIGHT	linBr[IND_MAX_BRIGHT]
#define LIN_BRIGH(x) linBr[(x%BRIGHT_STEPS)]
#define INV_LINBRIGH(x) linBr[IND_MAX_BRIGHT - ((x)%BRIGHT_STEPS)]

T_PIXEL Wheel(uint8_t WheelPos) {//ѕалитра типа цветового круга r - g - b -r
	T_PIXEL temp_pix;
  if(WheelPos < 85) {
	  temp_pix.red = 255 - WheelPos * 3;
	  temp_pix.green = WheelPos * 3;
	  temp_pix.blue = 0;
	  temp_pix.br = 0xff;
  } else if(WheelPos < 170) {
   WheelPos -= 85;
	  temp_pix.red = 0;
	  temp_pix.green = 255 - WheelPos * 3;
	  temp_pix.blue = WheelPos * 3;
	  temp_pix.br = 0xff;
  } else {
   WheelPos -= 170;
	  temp_pix.red = WheelPos * 3;
	  temp_pix.green = 0;
	  temp_pix.blue = 255 - WheelPos * 3;
	  temp_pix.br = 0xff;
  }
  return temp_pix;
}
//‘ункци€ выбирает цвета из палитры. ÷вета от 0 до 255
T_PIXEL ColorFromPalette(T_COLOR_PALETTE palette,uint8_t colorInd){
	T_PIXEL tempPixel;
	switch (palette) {
		case RAINBOW:{
			memcpy_P((void*)&tempPixel,(const void*)&(g_rainbow[colorInd/36]),sizeof(T_PIXEL));
		//	tempPixel = g_rainbow[colorInd%7];
					}break;
		case COLOR_RING:{
			tempPixel = Wheel(colorInd);
					}break;
		case FLAME:{
			colorInd = ((uint16_t)colorInd*(uint16_t)84)/255;
			if(colorInd%84 < 42)
                            tempPixel = Wheel( colorInd%42);
			else
                            tempPixel = Wheel(42 - colorInd%42);
					}break;
		case COLD:{
			if(colorInd >= 128)
				colorInd = 127 - colorInd%128;

			if(colorInd <= 128){
			tempPixel.red = 0;
			tempPixel.green = 0xFF - colorInd*2;
			tempPixel.blue = colorInd*2;
			}
					}break;
		case WARM:{
			if(colorInd >= 128)
				colorInd = 127 - colorInd%128;

			if(colorInd <= 128){
			tempPixel.red =  0xFF - colorInd*2;
			tempPixel.green = 0;
			tempPixel.blue = colorInd*2;
			}

		}break;
		case COLD_WHITE:{
			tempPixel.red = 0xf0;
			tempPixel.green = 0xf8;
			tempPixel.blue = 0xff;
					}break;
		case WARM_WHITE:{
			tempPixel.red = 0xff;
			tempPixel.green = 0xcf;
			tempPixel.blue = 0x48;
					}break;
		case NO_COLOR:{
			tempPixel.red =		0;
			tempPixel.green =	0;
			tempPixel.blue =	0;			
		}
		default:
			break;
	}
	tempPixel.br = 0xFF;
	return tempPixel;
}


void LinUpDown(){
	static uint16_t duty = 1,sign;
	for(uint8_t i = 0; i < NUM_OF_LEDS; i++){
			SetPixBrightness(i,LIN_BRIGH(duty));
	}
	(sign)?(duty = duty - 1):( duty = duty + 1);
		if(duty == 15) sign = TRUE;
		if(duty == 0) sign = FALSE;
}

void LinUpDownChet(){
	static uint16_t duty = 1,sign;
	for(uint8_t i = 0; i < NUM_OF_LEDS; i++){
		if(i%2 == 0)
			SetPixBrightness(i,LIN_BRIGH(duty));
		else
			SetPixBrightness(i,INV_LINBRIGH(duty));
	}
	(sign)?(duty = duty - 1):( duty = duty + 1);
		if(duty == 15) sign = TRUE;
		if(duty == 0) sign = FALSE;
}

#define FIRE_LENGT NUM_OF_LEDS
void RunFireBr(){
	static uint8_t indOfFire = 0;

	for(uint8_t i = 0; i < FIRE_LENGT; i++){
		SetPixBrightness( (indOfFire - i)%(NUM_OF_LEDS + 1 ),
							INV_LINBRIGH( (15*i)/(FIRE_LENGT-1) )
						);
	}
	indOfFire++;
}

void OneRandomBlink(){
	//пробегаем по €ркости от 0 до 31
	//если €ркость от 16 до 31 - €ркость увеличиваетс€
	//если €ркость от 0 до 15 - €ркость уменьшаетс€
	//если €ркость равна 0 - значит элемент снова погашен, мен€ем индекс
	static uint8_t index = 0,
				   br = IND_MAX_BRIGHT*2 + 1;

	if(0 == br){//€ркость вернулась на максимум, мен€ем индекс и ставим минимум €ркости
		SetPixBrightness(index,INV_LINBRIGH(br));
		index = (rand()%255)%NUM_OF_LEDS;
		br = IND_MAX_BRIGHT*2 + 1;
	}
	if(br == 16) SetPixColor(index,ColorFromPalette(g_currentPalette,rand()%255),0x00);
	if(br > 15)
		SetPixBrightness(index,LIN_BRIGH(br));
	else
		SetPixBrightness(index,INV_LINBRIGH(br));
	br--;

}

void UpdateBrightness(){
	if(g_brEffect == 0)	RunFireBr();
	if(g_brEffect == 1)	LinUpDown();
	if(g_brEffect == 2)	LinUpDownChet();
	if(g_brEffect == 3) OneRandomBlink();
}

////////////////////////////////////////////
void Rotate(){
	static	uint8_t shift = 0;
		for(uint8_t i = 0; i < NUM_OF_LEDS; i++){
			SetPixColor(i,ColorFromPalette(g_currentPalette,i*255/NUM_OF_LEDS + shift),GetPixBr(i));
		}
		shift++;
}

void StackColor(){
static T_PIXEL tempPixel;
static uint8_t curPixInd = 0;
static uint8_t curEndOfLine = NUM_OF_LEDS -1;

	if(curPixInd < curEndOfLine){
		tempPixel = GetPix(curPixInd);
		SetPixColor(curPixInd,GetPix(curPixInd + 1),MAX_BRIGHT);
		SetPixColor(curPixInd + 1,tempPixel,MAX_BRIGHT);

		curPixInd++;
	}else{
		curPixInd = 0;
		curEndOfLine--;
		if(curEndOfLine == 1) {
			curEndOfLine = NUM_OF_LEDS - 1;
		//	SetPixColor(0,ColorFromPalette(WARM_WHITE,rand()%255),MAX_BRIGHT);
		}

	}

}

void StackComet(){
	//Ќулевой огонек начинает движение к концу линии
	//стира€ за собой пиксели
	//как только дойдет до конца линии то в нулевой позиции по€вл€етс€
	//случайный пиксель и начинает бежать до второй с конца позиции и тд
	static T_PIXEL tempPixel;
	static uint8_t curPixInd = 0;
	static uint8_t curEndOfLine = NUM_OF_LEDS ;

	if(curPixInd == 0){

		SetPixColor(curPixInd ,tempPixel,MAX_BRIGHT);
		curPixInd++;

	}else
		if(curPixInd < curEndOfLine){

		SetPixBrightness(curPixInd - 1,LIN_BRIGH(0));
		SetPixColor(curPixInd ,tempPixel,MAX_BRIGHT);

		curPixInd++;
	}else{
		curPixInd = 0;
		curEndOfLine--;
		tempPixel = ColorFromPalette(g_currentPalette,rand()%255);
		if(curEndOfLine == 0) {
			curEndOfLine = NUM_OF_LEDS ;

		}

	}
}

void RandomColor(){
		SetPixColor(rand()%NUM_OF_LEDS,
				ColorFromPalette(g_currentPalette,rand()%255),
				MAX_BRIGHT);
}



void UpdateColor(){
	if(g_colorEffect == 0) ;
	if(g_colorEffect == 1) Rotate();
	if(g_colorEffect == 2) StackColor();
	if(g_colorEffect == 3) RandomColor();
	if (g_colorEffect == 4)StackComet();
}



void SetColorEffect(uint8_t effect,uint8_t speed){
	g_colorEffect = effect;
	g_colorSpeed = speed;

	AddTask(UpdateColor,0,10*speed);
	if(speed == 0) DeleteTask(UpdateColor);
}

void NextColorEffect()
{
	g_colorEffect++;
	g_colorEffect = g_colorEffect%NUM_OF_COLOR_EFFECTS;
	T_COLOR_PALETTE tempPalette = g_currentPalette;
	g_currentPalette = NO_COLOR;
	InitLedColors();
	SetPixColor(g_colorEffect,RED,MAX_BRIGHT);
	g_currentPalette = tempPalette;
	AddTask(InitLedColors,1000,0);
	AddTask(UpdateColor,1500,10*g_colorSpeed);

}

 void SetBrightnessEffect(uint8_t effect,uint8_t speed){
	 g_brSpeed = speed;
	 g_brEffect = effect;
	 AddTask(UpdateBrightness,0,10*speed);
	 if(speed == 0) DeleteTask(UpdateBrightness);
 }

void NextBrightnessEffect()
{
	g_brEffect++;
	g_brEffect = g_brEffect%NUM_OF_BR_EFFECTS;
	AddTask(UpdateBrightness,0,10*g_brSpeed);

}

void NextPalette()
{
	switch (g_currentPalette)
	{
		case RAINBOW		:g_currentPalette = COLOR_RING	; break;
		case COLOR_RING		:g_currentPalette = FLAME 		; break;
		case FLAME 			:g_currentPalette = COLD 		; break;
		case COLD 			:g_currentPalette = WARM 		; break;
		case WARM 			:g_currentPalette = COLD_WHITE	; break;
		case COLD_WHITE 	:g_currentPalette = WARM_WHITE	; break;
		case WARM_WHITE		:g_currentPalette = RAINBOW		; break;
		default				:g_currentPalette = COLOR_RING	; break;
	}
	eeprom_update_byte((uint8_t*)&g_eepromPalette,(uint8_t)g_currentPalette);
	InitLedColors();
}

void InitLedColors(){
	g_currentPalette = (T_COLOR_PALETTE)eeprom_read_byte((const uint8_t*)&g_eepromPalette);
	for(uint8_t i = 0; i < NUM_OF_LEDS; i++){
		SetPixColor(i,ColorFromPalette(g_currentPalette,i*255/NUM_OF_LEDS),0xFF);
	}

}
#define NUM_OF_ALL_EFFECTS 6
#define EFFECT_MASK			0x80
#define NON_EFFECT			0
#define COLOR_WAWE			1
#define RUN_FIRE			2
#define RANDOM_SLOW_BLINK	3
#define STACK_COMET			4
#define CHET_BLINK			5

void UpdateAll(){
	switch (g_allEffect & ~EFFECT_MASK)
	{
	case NON_EFFECT			:					;break;
	case COLOR_WAWE			:Rotate()			;break;
	case RUN_FIRE			:{RunFireBr();	Rotate();}	;break;
	case RANDOM_SLOW_BLINK	:OneRandomBlink()	;break;
	case STACK_COMET		:StackComet()		;break;
	case CHET_BLINK			:{LinUpDownChet();Rotate();}; break;
	default					:Rotate()			;break;
	}
}

void RandomAllEffect(){
	if (g_allEffect & EFFECT_MASK)
	{
		g_allEffect = rand()%NUM_OF_ALL_EFFECTS;
		g_allEffect |= EFFECT_MASK;
	}
	
	InitLedColors();
}

void NextEffect(){
		g_allEffect++;						
		g_allEffect = (g_allEffect & ~EFFECT_MASK) %(NUM_OF_ALL_EFFECTS + 1);
		for(uint8_t i = 0; i < NUM_OF_LEDS; i++){
				SetPixColor(i,BLACK,0);
			}
		if( (g_allEffect & ~EFFECT_MASK) == NUM_OF_ALL_EFFECTS) {
			g_allEffect |= EFFECT_MASK;
			//AddTask(RandomAllEffect,0xffff,0xffff);
			SetPixColor(0,RED,MAX_BRIGHT);
			}
			else{
				g_allEffect &= ~EFFECT_MASK;
				//DeleteTask(RandomAllEffect);
				SetPixColor(g_allEffect,YELLOW,MAX_BRIGHT);
			}
		eeprom_update_byte(&g_eepromAllEffect,g_allEffect);
		AddTask(InitLedColors,1000,0);
		AddTask(UpdateAll,1500,10*g_allSpeed);
}

void InitWS2110(){
	INIT_LED_GPIO();
	InitLedColors();
	g_allEffect = eeprom_read_byte(&g_eepromAllEffect);
	AddTask(RandomAllEffect,0,0xffff);
	AddTask(SendAllPixels,0,10);
	AddTask(UpdateAll,0,10*g_allSpeed);
	
}