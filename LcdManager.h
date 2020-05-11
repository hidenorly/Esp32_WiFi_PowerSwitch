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

#ifndef __LCD_MANAGER_H__
#define __LCD_MANAGER_H__

#include "config.h"

#ifdef TARGET_BOARD_M5STICK_C
 #include "Lcd_M5.h"
#endif // TARGET_BOARD_M5STICK_C

#ifdef TARGET_BOARD_ESP32_DEV
 #include "Lcd_SSD1306.h"
#endif // TARGET_BOARD_ESP32_DEV

#endif // __LCD_MANAGER_H__
