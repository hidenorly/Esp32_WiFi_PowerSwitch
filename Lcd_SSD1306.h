/* 
 Copyright (C) 2020 hidenorly

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
*/

#ifndef __LCD_SSD1306_H__
#define __LCD_SSD1306_H__

#include "config.h"
#ifdef TARGET_BOARD_ESP32_DEV
#if ENABLE_LCD_SSD1306

#include "base.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define GPI_PIR_SENSOR 25
#define GPO_RELAY_CTRL 15
#define GPO_RELAY_CTRL_ACTIVE LOW


// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI  14
#define OLED_CLK   13
#define OLED_DC    21
#define OLED_CS    16
#define OLED_RESET 4
Adafruit_SSD1306 LCD(SCREEN_WIDTH, SCREEN_HEIGHT,
  OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

#define DISP_SIZE_MID 2
#define DISP_SIZE_LARGE 3
#define DISP_SIZE_XLARGE 4


void LCD_initialize(void)
{
	LCD.begin(SSD1306_SWITCHCAPVCC);
	LCD.clearDisplay();
	LCD.setRotation(2);
	LCD.fillScreen(SSD1306_BLACK);

	LCD.setTextSize(1);      // Normal 1:1 pixel scale
	LCD.setTextColor(SSD1306_WHITE); // Draw white text
	LCD.setCursor(0, 32);     // Start at top-left corner
	LCD.cp437(true);         // Use full 256 char 'Code Page 437' font
	LCD.display();
}

static unsigned long lastDisplayed = 0;

void LCD_update(void)
{
	unsigned long m = millis();	
	if( (m - lastDisplayed)> 100 ){
		LCD.display();
		lastDisplayed = m;
	}
}


#endif // ENABLE_LCD_SSD1306
#endif // TARGET_BOARD_ESP32_DEV
#endif // __LCD_SSD1306_H__
