/* 
 Copyright (C) 2016,2018,2019,2020, 2024 hidenorly

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

#ifndef __CONFIG_H__
#define __CONFIG_H__

//#define TARGET_BOARD_ESP32_DEV
//#define ENABLE_LCD_SSD1306 1
#define  TARGET_BOARD_M5STICK_C

// -- config
#define KEEP_WIFI_CONNECT_AFTER_NTPSYNCED

#define ENABLE_IR_REMOTE_CONTROLLER 0
#define ENABLE_SWITCH_BOT_REMOTE_CONTROLLER 0
#define ENABLE_LOCAL_POWER_CONTROLLER 1

// --- config: GPIO in ENABLE_LOCAL_POWER_CONTROLLER
#if ENABLE_LOCAL_POWER_CONTROLLER
     #define LOCAL_POWER_CONTROLLER_GPO 26 // G26 on M5StickC (this config is optional)
     #define LOCAL_POWER_CONTROLLER_MODE 1 // 0:ON/OFF mode, 1:High Pulse mode, 2:Low Pulse mode
     #define LOCAL_POWER_CONTROLLER_PULL_DURATION 100 // pulse duration msec for 1/2:Pulse Mode
     #if LOCAL_POWER_CONTROLLER_MODE==1
          #define LOCAL_POWER_CONTROLLER_PULSE_ACTIVE 1
          #define LOCAL_POWER_CONTROLLER_PULSE_INACTIVE 0
     #else
          #define LOCAL_POWER_CONTROLLER_PULSE_ACTIVE 0
          #define LOCAL_POWER_CONTROLLER_PULSE_INACTIVE 1
     #endif
#endif // ENABLE_LOCAL_POWER_CONTROLLER

// --- config: WIFI
extern const char* WIFI_CONFIG;
extern const char* WIFIAP_PASSWORD;

// --- config: NTP
extern const char* NTP_CONFIG;
#define NTP_DEFAULT_SERVER "ntp.nict.jp"
#define NTP_DEFAULT_TIMEZONE_OFFSET 9

#if ENABLE_SWITCH_BOT_REMOTE_CONTROLLER
// --- config: SWITCHBOT
extern const char* SWITCHBOT_CONFIG;
extern const char* SWITCHBOT_CONFIG_REVERSE;
#endif // ENABLE_SWITCH_BOT_REMOTE_CONTROLLER

// --- config: httpd
extern int HTTP_SERVER_PORT;
extern const char* HTML_HEAD;

// --- GPIO config
extern const int HUMAN_DETCTOR_PIN;
#if ENABLE_IR_REMOTE_CONTROLLER
extern const int IR_SEND_PIN;
#endif // ENABLE_IR_REMOTE_CONTROLLER

// -- params
extern const int HUMAN_UNDETECT_TIMEOUT;
extern const int HUMAN_POLLING_PERIOD;
extern const int BTN_POLLING_PERIOD;


#include "RemoteController.h"
#if ENABLE_IR_REMOTE_CONTROLLER
#include "RemoteController_Ir.h"
extern KEYIRCODE KEYIrCodes[];
#endif // ENABLE_IR_REMOTE_CONTROLLER

void initializeGPIO(void);

#define MANUAL_OPERATION_TIMEOUT (1000*15)	// 15sec


#endif // __CONFIG_H__
