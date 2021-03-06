/* 
 Copyright (C) 2016,2018,2019,2020 hidenorly

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

#include "base.h"
#include "config.h"

#include "GpioDetector.h"
#include "RemoteController.h"
#include "RemoteController_Ir.h"

// --- config: WIFI
const char* WIFI_CONFIG = "/wifi_config";
const char* WIFIAP_PASSWORD = "esp32wifi"; // you can see WiFi Mac Address's SSID and this is password for setup SSID/Password from web.

// --- config: NTP
const char* NTP_CONFIG = "/ntp_config";

#if ENABLE_SWITCH_BOT_REMOTE_CONTROLLER
// --- config: SwitchBot
const char* SWITCHBOT_CONFIG = "/ble_config";
const char* SWITCHBOT_CONFIG_REVERSE = "/swb_config_reverse";
#endif // ENABLE_SWITCH_BOT_REMOTE_CONTROLLER

// --- config: httpd
int HTTP_SERVER_PORT = 80;
const char* HTML_HEAD = "<html><head><title>hidenorly's M5StickC</title></head><body>";


// --- config
const int HUMAN_DETCTOR_PIN = GpioDetector::GPIO_G36;
const int HUMAN_UNDETECT_TIMEOUT=1000*60*3; // 3min
const int HUMAN_POLLING_PERIOD=1000; // polling per 1 sec

const int BTN_POLLING_PERIOD=100; // polling per 100msec

#if ENABLE_IR_REMOTE_CONTROLLER
const int IR_SEND_PIN = 32; // Use 9 if you want to use M5StickC's Ir LED
KEYIRCODE KEYIrCodes[] = {
  {IRemoteController::KEY_POWER_ON, 0x41B6659AUL, 3, 50},
  {IRemoteController::KEY_POWER_OFF, 0x41B67D82UL, 3, 50},
  {NULL, (uint64_t)NULL, (uint8_t)NULL}
};
#endif // ENABLE_IR_REMOTE_CONTROLLER

// --- GPIO config
void initializeGPIO(void) {
  // General config
#ifdef TARGET_BOARD_M5STICK_C
  M5.begin();     // Initialze M5 stack C
  DEBUG_PRINTLN("M5.begin()");
#endif

  // If pin is NC, we should set {output / High} or {input / pull up enabled} on the pin.
  // Project related config
  pinMode(HUMAN_DETCTOR_PIN, INPUT_PULLUP);   // Optical Detector Input (High: detected)
}
