/* Project-Suction Pump
   
   Hardware-AMS5812-1000D(Pressure sensor),ATmega328p(micro controller),ILI9225 display lcd display controller ic.
   
   Author: Rajat mishra
*/


/*************Dependencies*************/

#include <Arduino.h>
#include "AMS5812.h"
#include "SPI.h"
#include "TFT_22_ILI9225.h"
#include <math.h>
#include "FreeSerif18pt7b.h"
#include "FreeSerif12pt7b.h"

/************************************/

#define TFT_RST 8   //Reset 
#define TFT_RS  9   //Register select
#define TFT_CS  10  // SS
#define TFT_SDI 11  // MOSI
#define TFT_CLK 13  // SCK
#define TFT_LED 3   // 0 if wired to +5V directly

/*************************************/

#define TFT_BRIGHTNESS 255  //Set tft brightness
#define WHITE COLOR_WHITE
#define BLACK COLOR_BLACK
#define MAXAREA 4500       // Maximum area of rectangle
#define rectangle_length 150 //length of rectangle
#define rectangle_width  30  //width of rectangle
#define primary_color COLOR_WHITE
#define secondry_color COLOR_BLACK

/*****************Variables*************************/

float pressure = 0;
float Previous_Pressure = 0;
float Current_Pressure = 0;
int Previous_Area = 0;
int Current_Area = 0;
int w = 10;
int h = 15;
String stringTwo;
char first_digit = 0;
char second_digit = 0;
char third_digit = 0;
bool mode = false;


/************************************************/

TFT_22_ILI9225 tft = TFT_22_ILI9225(TFT_RST, TFT_RS, TFT_CS, TFT_LED, TFT_BRIGHTNESS);
AMS5812 dPress(Wire,0x10,AMS5812::AMS5812_1000_D);
void fill_area(int area,uint16_t colour);
void draw_Mpa_scale();
void draw_mmHg_scale();
void AgVa_logo();
void main_screen();

/***************************************************************************************/

void setup() {
  Serial.begin(9600);
  tft.begin();  //Tft initialize 
  tft.setOrientation(3);  //Set tft Orientation
  tft.setBacklight(HIGH); //Set back light high
  tft.setFont(Terminal6x8);
  //Printing AgVa logo on screen
  AgVa_logo();
  main_screen();
  while(!Serial){}
  if (dPress.begin() < 0) 
  {
    Serial.println("Error communicating with sensor, check wiring and I2C address");
    while(1){}
  }
}

/*********************************************************************************/
void loop() {
  
  float final_pressure = 0;
  uint8_t i = 200;
  
  //Taking 2oo pressure value samples 
  while(i)
  {  dPress.readSensor();
	  pressure = dPress.getPressure_Pa(); //Pressure in pascal
	  if(pressure >= 1700000)
	  {
		  pressure=0;
	  }
   if(mode)  
   {
   float kPa = (pressure/1000); //pressure in Kilo  pascal
   final_pressure = final_pressure+kPa;
   }
   else
   {
	 float mmHg = (pressure/133.322);
	 final_pressure = final_pressure+mmHg;
   }
    i--;
   delay(1);
  }
   final_pressure = final_pressure/200;
  
   Serial.print("Pressure =\t");
   Serial.print(final_pressure,4); 
   Serial.print("kPa");
   //Serial.print(Angle);
   Serial.print("\n");
  if(mode)
  {
   Current_Area = map(final_pressure,0,100,0,4500); //Maping between Pressure and Area
  }
  else
  {Current_Area = map(final_pressure,0,750,0,4500); //Maping between Pressure and Area
  }
   Current_Pressure = final_pressure; 
  if(mode)
  {
   stringTwo =  String(Current_Pressure/1000,3);
  }
  else
  {
	stringTwo =  String(Current_Pressure, 0);  
  }
   Serial.println(stringTwo); 
 	
	 if((Current_Pressure-Previous_Pressure> 1)||(Previous_Pressure-Current_Pressure > 1))   //Printing Pressure Value Screen
  { 
	  if(third_digit!=stringTwo[2])
	  {
		  
	   tft.fillRectangle(94, 45, 120, 75,COLOR_BLACK); // Rectangle  to delete Third digit	  
	   third_digit = stringTwo[2];
	 
	  }
	  
	  if(second_digit != stringTwo[1])
	  {
		  tft.fillRectangle(76, 45, 94, 75, COLOR_BLACK); // Rectangle  to delete second digit
		  second_digit = stringTwo[1];
	  }
	 
	  if(first_digit != stringTwo[0])
	  {
		  
		tft.fillRectangle(50, 45, 75, 75, COLOR_BLACK); // Rectangle  to delete first digit
		first_digit = stringTwo[0];   
	  
	  }
       
	    tft.drawGFXText(60,70, stringTwo,primary_color);  //Writing new value from screen
      
   }
   
   
      if((Current_Pressure-Previous_Pressure > 1))   //When Pressure is increasing.
   {
  
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

   
    else if(Previous_Pressure-Current_Pressure > 1)  //Pressure is decreasing.
{
    float Previous_length = Previous_Area/rectangle_width;
  
    float Current_length =  Current_Area/rectangle_width;
  
    tft.fillRectangle(35+Current_length, 105, 35+Previous_length, 135,COLOR_BLACK);
  
   if (Current_Area<50)
  {
	 fill_area(Current_Area,COLOR_BLACK); 
  }
  
  else if(Current_Area >= 50 && Current_Area < 900)
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

/*******************************************************************************/

void fill_area(int area,uint16_t colour)
{
  
   int length = area/rectangle_width;
  
   tft.fillRectangle(35, 105, 35+length, 135, colour);
  
}


/******************************************************************************/
void draw_Mpa_scale()
{
	 tft.drawLine(30,140,30,155,primary_color);
	 tft.drawText(27,158,"0",primary_color);
	 tft.drawLine(50,140,50,150,primary_color);
	 tft.drawLine(70,140,70,150,primary_color);
	 tft.drawLine(90,140,90,150,primary_color);
	 
	 tft.drawLine(110,140,110,155,primary_color);
	 tft.drawText(97,158,"0.05",primary_color);
	 tft.drawLine(130,140,130,150,primary_color);
	 tft.drawLine(150,140,150,150,primary_color);
	 tft.drawLine(170,140,170,150,primary_color);
	 tft.drawLine(190,140,190,155,primary_color);
	 tft.drawText(180,156,"0.1",primary_color);
}
/***************************************************************************/

/******************************************************************************/
void draw_mmHg_scale()
{
	tft.drawLine(30,140,30,155,primary_color);
	tft.drawText(27,158,"0",primary_color);
	tft.drawLine(50,140,50,150,primary_color);
	tft.drawLine(70,140,70,150,primary_color);
	tft.drawLine(90,140,90,150,primary_color);
	
	tft.drawLine(110,140,110,155,primary_color);
	tft.drawText(97,158,"375",primary_color);
	tft.drawLine(130,140,130,150,primary_color);
	tft.drawLine(150,140,150,150,primary_color);
	tft.drawLine(170,140,170,150,primary_color);
	tft.drawLine(190,140,190,155,primary_color);
	tft.drawText(180,158,"750",primary_color);
}
/***********************/

void AgVa_logo()
{    String s2;
	 tft.fillRectangle(0,0,219,176,COLOR_WHITE);
	 tft.setGFXFont(&FreeSerif12pt7b);    //Set gfx font free serif
	 s2="AgVa";
	 tft.drawGFXText(20,90, s2,COLOR_DEEP_PINK);
	 s2="Healthcare";
	 tft.drawGFXText(90,90, s2,COLOR_BLACK);
	 delay(1000);
	 tft.clear();
}

/************************************************************************/

void main_screen()
{
    tft.drawRectangle(0, 0, tft.maxX() - 1, tft.maxY() - 1, primary_color);  //Drawing rectangle at boundary of tft screen.
    tft.drawRectangle(1, 1, tft.maxX() - 2, tft.maxY() - 2, primary_color);
    tft.drawRectangle(2, 2, tft.maxX() - 3, tft.maxY() - 3, primary_color);

    tft.drawRectangle(30, 100, 190, 140,primary_color);  //Draw rectangle to show pressure display
    tft.drawRectangle(28, 98, 192, 142, primary_color);
    tft.drawRectangle(27, 97, 193, 143, primary_color);	
    tft.setGFXFont(&FreeSerif18pt7b);
 
    //String s2="0.000";
	String s2="0000";
   // tft.drawGFXText(30,70, s2,primary_color); //Initially  show pressure zero
    tft.setGFXFont(&FreeSerif12pt7b);
    s2="mmHg";
    tft.drawGFXText(130,65, s2,primary_color); //Draw MPa text on screen
    tft.setGFXFont(&FreeSerif18pt7b);
   //tft.drawBitmap(170, 10, batt3, 30, 20, primary_color); //Battery icon
    draw_mmHg_scale();
}

/**********************************************************************/
