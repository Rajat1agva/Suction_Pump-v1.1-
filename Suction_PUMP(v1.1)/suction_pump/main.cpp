/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>

/*End of auto generated code by Atmel studio */

/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>

/*End of auto generated code by Atmel studio */

/************************************/
#include "AMS5812.h"
#include "SPI.h"
#include "TFT_22_ILI9225.h"
#include <math.h>
#include "FreeSerif9pt7b.h"
#include "icon.h"
//Beginning of Auto generated function prototypes by Atmel Studio
//End of Auto generated function prototypes by Atmel Studio


//Beginning of Auto generated function prototypes by Atmel Studio
//End of Auto generated function prototypes by Atmel Studio


/************************************/
#define TFT_RST 8
#define TFT_RS  9
#define TFT_CS  10  // SS
#define TFT_SDI 11  // MOSI
#define TFT_CLK 13  // SCK
#define TFT_LED 3   // 0 if wired to +5V directly

/*************************************/
#define TFT_BRIGHTNESS 200
#define WHITE COLOR_WHITE
#define BLACK COLOR_BLACK
#define MAXAREA 4500
#define rectangle_length 150
#define rectangle_width  30
/******************************************/

float pressure = 0;
float Previous_Pressure = 0;
float Current_Pressure = 0;
int Previous_Area = 0;
int Current_Area = 0;
int w = 10;
int h = 15;
String stringTwo;
/******************************************/


/***************************************************************************************/
TFT_22_ILI9225 tft = TFT_22_ILI9225(TFT_RST, TFT_RS, TFT_CS, TFT_LED, TFT_BRIGHTNESS);
AMS5812 dPress(Wire,0x10,AMS5812::AMS5812_1000_D);
void fill_area(int area,uint16_t colour);
/**************************************************************************************/
void setup() {
	Serial.begin(9600);
	tft.begin();
	tft.clear();
	tft.setOrientation(3);
	tft.setBacklight(HIGH);
	tft.drawRectangle(0, 0, tft.maxX() - 1, tft.maxY() - 1, COLOR_WHITE);
	tft.drawRectangle(1, 1, tft.maxX() - 2, tft.maxY() - 2, COLOR_WHITE);
	tft.drawRectangle(2, 2, tft.maxX() - 3, tft.maxY() - 3, COLOR_WHITE);
	tft.drawRectangle(30, 100, 190, 140, COLOR_WHITE);
	tft.drawRectangle(29, 99, 191, 141, COLOR_WHITE);
	tft.drawRectangle(28, 98, 192, 142, COLOR_WHITE);
	tft.drawRectangle(27, 97, 193, 143, COLOR_WHITE);
	tft.setFont(Terminal12x16);
	String s2;
	tft.setGFXFont(&FreeSerif9pt7b);
	s2="Pressure ";
	tft.drawGFXText(15,70, s2,COLOR_WHITE);
	s2="0.000";
	tft.drawGFXText(110,70, s2,COLOR_WHITE);
	s2="MPa";
	tft.drawGFXText(160,70, s2,COLOR_WHITE);
	tft.drawBitmap(170, 10, batt3, 30, 20, COLOR_WHITE);
	while(!Serial){}
	// starting communication with the
	// static pressure transducer
	if (dPress.begin() < 0) {
		Serial.println("Error communicating with sensor, check wiring and I2C address");
		while(1){}
	}
}


void loop() {
	
	dPress.readSensor();
	pressure = dPress.getPressure_Pa(); //Pressure in pascal
	if(pressure >= 1700000)
	{
		pressure=0;
	}
	
	float kPa = (pressure/1000); //pressure in Kilo  pascal
	float final_pressure = 0;
	int i = 200;
	
	
	while(i)
	{
		final_pressure = final_pressure+kPa;
		i--;
		delay(1);
	}
	final_pressure = final_pressure/200;

	
	Serial.print("Pressure =\t");
	Serial.print(final_pressure,4);
	Serial.print("kPa");
	//Serial.print(Angle);
	Serial.print("\n");

	Current_Area = map(final_pressure,0,100,0,4500);
	Current_Pressure = final_pressure;
	stringTwo =  String(Current_Pressure/1000, 3);
	if((Current_Pressure-Previous_Pressure > 1)||(Previous_Pressure-Current_Pressure > 1))
	{
		tft.fillRectangle(110, 50, 155, 80, COLOR_BLACK);
		tft.drawGFXText(110,70, stringTwo,COLOR_WHITE);
	}
	if((Current_Pressure-Previous_Pressure > 1))   //When Pressure is increasing.
	{
		
		if(Current_Area >= 0 && Current_Area < 900)
		{fill_area(Current_Area,COLOR_AZUR);
		}
		else if(Current_Area >=900 && Current_Area <3600)
		{
			fill_area(Current_Area,COLOR_GREEN);
		}
		else if(Current_Area >= 3600 )
		{
			fill_area(Current_Area,COLOR_RED);
		}
		Previous_Pressure = Current_Pressure;
		Previous_Area = Current_Area;
	}
	else if(Previous_Pressure-Current_Pressure > 1)
	{
		float Previous_length = Previous_Area/rectangle_width;
		float Current_length =  Current_Area/rectangle_width;
		tft.fillRectangle(35+Current_length, 105, 35+Previous_length, 135,COLOR_BLACK);
		if(Current_Area >= 0 && Current_Area < 900)
		{
			fill_area(Current_Area,COLOR_GOLD);
		}
		else if(Current_Area >=900 && Current_Area <3600)
		{
			fill_area(Current_Area,COLOR_GREEN);
		}
		else if(Current_Area >= 3600 )
		{
			fill_area(Current_Area,COLOR_RED);
		}
		Previous_Pressure = Current_Pressure;
		Previous_Area = Current_Area;
	}
}
void fill_area(int area,uint16_t colour)
{
	
	int length = area/rectangle_width;
	tft.fillRectangle(35, 105, 35+length, 135, colour);
	
}
