#include "led_ws2812.h"


static T_PIXEL	temp_pix[NUM_OF_LEDS]  ;//пиксели дл€ вывода с примененной €ркостью
static T_PIXEL  out_data_set[NUM_OF_LEDS] ;
//= {
			//{.red=0xFF,.green=0x00,.blue=0x00},
			//{.red=0xFF,.green=0x7F,.blue=0x00},
			//{.red=0xFF,.green=0xFF,.blue=0x00},
			//{.red=0x00,.green=0xFF,.blue=0x00},
			//{.red=0x00,.green=0x00,.blue=0xFF}
//};


#define DELAY_04US()	asm("nop");\
						asm("nop");\
						//asm("nop");
						
#define DELAY_08US()	DELAY_04US();\
						DELAY_04US();\
						DELAY_04US();\
						asm("nop");
						
#define SEND_ZERO()		LED_PIN_HIGH();\
						asm("nop");\
						asm("nop");\
						LED_PIN_LOW();\
						asm("nop");\
						asm("nop");\
						asm("nop");\
						asm("nop");\
						asm("nop");\
						asm("nop");\
						asm("nop");\
						asm("nop");\
						asm("nop");\
						asm("nop");
						
						
						
						
#define SEND_ONE()		LED_PIN_HIGH();\
						asm("nop");\
						asm("nop");\
						asm("nop");\
						asm("nop");\
						asm("nop");\
						asm("nop");\
						asm("nop");\
						asm("nop");\
						asm("nop");\
						asm("nop");\
						LED_PIN_LOW();\
						asm("nop");\
						asm("nop");

#pragma optimize=z none
void SendOnePixel(T_PIXEL* pixel){
	//volatile uint16_t x = 0;
	volatile uint8_t out_data;
	for(uint8_t j = 0; j < 3; j++){
		out_data = (((uint8_t*)pixel)[j]);
		if(out_data & 0x80){
			SEND_ONE();
		}else{
			SEND_ZERO();
		}
		if(out_data & 0x40){
			SEND_ONE();
		}else{

			SEND_ZERO();
		}
		if(out_data & 0x20){
			SEND_ONE();
		}else{

			SEND_ZERO();
		}
		if(out_data & 0x10){
			SEND_ONE();
		}else{

			SEND_ZERO();
		}
		if(out_data & 0x08){
			SEND_ONE();
		}else{
			SEND_ZERO();
		}
		if(out_data & 0x04){
			SEND_ONE();
		}else{
			SEND_ZERO();
		}
		if(out_data & 0x02){
			SEND_ONE();
		}else{
			SEND_ZERO();
		}
		if(out_data & 0x01){
			SEND_ONE();
		}else{
			SEND_ZERO();
		}
	}
}


uint8_t SetPixelColor(uint8_t pixnum,uint8_t r,uint8_t g,uint8_t b,uint16_t brightness){
	if(pixnum > NUM_OF_LEDS - 1) return 1;

#ifdef P9823
		out_data_set[pixnum].red = 		g;
		out_data_set[pixnum].green = 	r;
#else
		out_data_set[pixnum].red = 		r;
		out_data_set[pixnum].green = 	g;
#endif 
	out_data_set[pixnum].blue = 	b;

	out_data_set[pixnum].br = brightness;

	return 0;
}

uint8_t SetPixColor(uint8_t pixnum,T_PIXEL color,uint8_t br){

	if(pixnum > NUM_OF_LEDS - 1) return 1;

#ifdef P9823
	out_data_set[pixnum].red = 		(color.green );
	out_data_set[pixnum].green = 	(color.red );
#else	
	out_data_set[pixnum].red = 		(color.red );
	out_data_set[pixnum].green = 	(color.green );
#endif
	out_data_set[pixnum].blue = 	(color.blue);

	out_data_set[pixnum].br = br;


	return 0;
}

__attribute__((optimize("Os")))
void SendAllPixels(){
	
	 for(uint8_t i = 0; i < NUM_OF_LEDS; i++){//ѕримен€ем €ркости
		 temp_pix[i].blue = (out_data_set[i].blue	 * 	out_data_set[i].br) >> 8;
		 temp_pix[i].red  = (out_data_set[i].red	 * 	out_data_set[i].br) >> 8;
		 temp_pix[i].green = (out_data_set[i].green * out_data_set[i].br) >> 8;
		 
	 }
	 //отправл€ем пиксели
	 for(uint8_t i = 0; i < NUM_OF_LEDS; i++) SendOnePixel(&(temp_pix[i]));
}



uint8_t SetPixBrightness(uint8_t pixnum,uint8_t br){
	if(pixnum > NUM_OF_LEDS - 1) return 1;
	out_data_set[pixnum].br = br;
	return 0;
}

T_PIXEL GetPix(uint8_t pixnum){
	return out_data_set[pixnum];
}

uint8_t GetPixBr(uint8_t pixnum){
	return out_data_set[pixnum].br;
}