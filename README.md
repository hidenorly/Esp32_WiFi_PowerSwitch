# M5StickC_NTPSwitch_SwitchBot

This is PIR Human detection based switch for M5StickC with WiFi AP & NTP,  PIR Sensor for M5StickC, Infrared LED and SwitchBot.

This supports
 * Turn ON through Ir Remote Transmiter/SwitchBot while human is detected.
 * Turn Off if human is not detected for a while.
 * The Ir Code is configurable.
 * Show On/Off status through LCD
 * Manual operations
   * Button A: Manually turn On/Off by M5 Button A
   * Button B: Switch between WiFi AP mode and WiFi Client by M5 Button B
     * You can configure this in WiFi AP mode with http://192.168.10.1
 * For improving availability, sending Ir few times (the count is configurable)
 * Show NTP synced time
 * WebConfig for SSID/Password, NTP, TimeZone Offset
 * SwitchBot
   * SwitchBot can support various switching modes
     1. pushing SW (button type or putting SwitchBot at the edge of the lage seesaw switch)
        * You should choose ON_ON, ON_OFF, OFF_ON, OFF_OFF.
     2. pushing SW / pulling SW (small seesaw switch)
        * You should choose ONOFF_ON_OFF, ONOFF_OFF_ON.
   * Meaning of X_Y:
     * X means trigger condition (e.g. ON: human detected, OFF: not detected, ONOFF: both)
     * Y means SwitchBot swithing mode (e.g. ON: Switch ON mode, OFF: Switch OFF mode)

# HW Config

| Infrared LED Unit | M5StickC |
| :---: | :---: |
| IN | G33 |
| OUT | G32 |
| +5V | +5V |
| GND | GND |

| PIR Unit | M5StickC |
| :---: | :---: |
| OUT | G36 |
| +3.3V | +3.3V |
| GND | GND |
| NC | +5V |
| NC | G0 |
| NC | G26 |
| NC | BAT|

# Config

```config.h
// -- config
#define ENABLE_IR_REMOTE_CONTROLLER 0
#define ENABLE_SWITCH_BOT_REMOTE_CONTROLLER 1
```

```config.cpp
// --- config
const int HUMAN_DETCTOR_PIN = GpioDetector::GPIO_G36;
const int HUMAN_UNDETECT_TIMEOUT=1000*60*3; // 3min
const int HUMAN_POLLING_PERIOD=1000; // polling per 1 sec

const int BTN_POLLING_PERIOD=100; // polling per 100msec

const int IR_SEND_PIN = 32; // Use 9 if you want to use M5StickC's Ir LED
KEYIRCODE KEYIrCodes[] = {
  {IRemoteController::KEY_POWER_ON, 0x41B6659AUL, 3, 50},
  {IRemoteController::KEY_POWER_OFF, 0x41B67D82UL, 3, 50},
  {NULL, (uint64_t)NULL, (uint8_t)NULL}
};
```

You can use the IrCode by dumping result of IRrecvDumpV2.

# For building

You need to choose "M5StickC" and "Minimum SPIFFS" or "No OTA" to include program area from "default".

## Dependent libraries

You can install the following libraries from Manage Library in Arduino IDE.

* M5StickC library
* Esp8266IrRemote library (https://github.com/crankyoldgit/IRremoteESP8266)
* ESP32_BLE_Arduino (https://github.com/nkolban/ESP32_BLE_Arduino)

# Configure SSID/Password

Please access to M5StickC's Mac Address with "esp32-wifi" and open browser at 192.168.10.1 to configure them.
