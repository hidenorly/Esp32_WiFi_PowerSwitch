/* 
 Copyright (C) 2016, 2018, 2019, 2020 hidenorly

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
#include "LooperThreadTicker.h"

#include "GpioDetector.h"
#include "RemoteController.h"
#if ENABLE_IR_REMOTE_CONTROLLER
#include "RemoteController_Ir.h"
#endif // ENABLE_IR_REMOTE_CONTROLLER
#if ENABLE_SWITCH_BOT_REMOTE_CONTROLLER
#include "RemoteController_SwitchBot.h"
#include "BleUtil.h"
#endif // ENABLE_SWITCH_BOT_REMOTE_CONTROLLER
#include "PowerControl.h"
#include "PowerControlPoller.h"

#include "WiFiUtil.h"
#include "NtpUtil.h"
#include "WebConfig.h"
#include "WatchDog.h"
#include <SPIFFS.h>
#include <WiFi.h>

#include "LcdManager.h"

#define ENABLE_YMD 0

// --- mode changer
bool initializeProperMode(bool bSPIFFS){
//  M5.update();
  if( !bSPIFFS || (!SPIFFS.exists(WIFI_CONFIG)) /*|| M5.BtnA.isPressed()*/){
    // setup because WiFi AP mode is specified or WIFI_CONFIG is not found.
    WiFiUtil::setupWiFiAP();
    WebConfig::setup_httpd();
    return false;
  } else {
    WiFiUtil::setupWiFiClient();
  }
  return true;
}

#if ENABLE_YMD
  #define DEFAULT_DISP_SIZE DISP_SIZE_MID
#else
  #define DEFAULT_DISP_SIZE DISP_SIZE_LARGE
#endif

class DispManager
{
public:
  static void showTentativeText(const char* text, int sx=20, int sy=16, int textSize=DISP_SIZE_XLARGE);
  static void showDateClock(const char* text, int sx=4, int sy=20, int textSize=DEFAULT_DISP_SIZE);
};


// --- handler for WiFi client enabled
class MyNetHandler
{
  protected:
    static void onWiFiClientConnected(void* pArg){
      DEBUG_PRINTLN("WiFi connected.");
      DEBUG_PRINT("IP address: ");
      DEBUG_PRINTLN(WiFi.localIP());

//      WebConfig::setup_httpd();
      NtpUtil::sync();
      if( NtpUtil::isTimeValid() ){
        delay(100);
        WiFiUtil::disconnect();
      }
    }

  public:
    static void setup(void)
    {
      NtpUtil::loadConfig();
      WiFiUtil::setStatusCallback(onWiFiClientConnected, NULL);
    }
};

class TimePoller:public LooperThreadTicker
{
  protected:
    static unsigned long mPendingStartTime;
    const unsigned long PENDING_TIME = 1000 * 5; // 5sec

  public:
    TimePoller(int dutyMSec=0):LooperThreadTicker(NULL, NULL, dutyMSec)
    {
    }

    static void pendingShow(void)
    {
      mPendingStartTime = millis();
    }

  protected:
    const int NTP_SYNC_DURATION = 60*60;            // 1 hour
    const int NTP_SYNC_DURATION_NOT_SYNCED = 60*3;  // 3min

    void syncTime(void)
    {
      static int i=0;
      i++;
      if(i % (NtpUtil::isTimeValid() ? NTP_SYNC_DURATION : NTP_SYNC_DURATION_NOT_SYNCED ) == 0){
        if(WiFiUtil::getMode() == WIFI_OFF){
          WiFiUtil::setupWiFiClient();
        } else {
          NtpUtil::sync();
          if( NtpUtil::isTimeValid() ){
            WiFiUtil::disconnect();
          }
        }
      }
    }

    virtual void doCallback(void)
    {
      struct tm timeInfo;
      char s[20];
      static String lastMessage;

      getLocalTime(&timeInfo);

      // setup message for log
      sprintf(s, "%04d/%02d/%02d %02d:%02d:%02d",
              timeInfo.tm_year + 1900, timeInfo.tm_mon + 1, timeInfo.tm_mday,
              timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);
      DEBUG_PRINTLN(s);

      // setup message for LCD
#if ENABLE_YMD
      sprintf(s, " %02d/%02d\n %02d:%02d",
              timeInfo.tm_mon + 1, timeInfo.tm_mday,
              timeInfo.tm_hour, timeInfo.tm_min);
#else // ENABLE_YMD
      sprintf(s, "%02d:%02d",
              timeInfo.tm_hour, timeInfo.tm_min);
#endif // ENABLE_YMD

      if( (!mPendingStartTime && !lastMessage.equals(s)) || ( mPendingStartTime && ((millis()-mPendingStartTime) > PENDING_TIME)) ){
        lastMessage = s;
        mPendingStartTime = 0;
        DispManager::showDateClock(s);
      }

      syncTime();
  }
};

unsigned long TimePoller::mPendingStartTime = 0;

typedef void (*BTN_CALLBACK_FUNC)(void*);


class SwitchBtnPoller:public LooperThreadTicker
{
  public:
    typedef enum{
      BUTTON_A,
      BUTTON_B
    } BUTTONS;

  protected:
    BTN_CALLBACK_FUNC mpFunc;
    void* mpArg;
    BUTTONS mBtn;
    bool mLastPressed;

  public:
    SwitchBtnPoller(int dutyMSec=100, BTN_CALLBACK_FUNC pFunc=NULL, void* pArg=NULL, BUTTONS btn = BUTTON_A):LooperThreadTicker(NULL, NULL, dutyMSec),mpFunc(pFunc),mpArg(pArg),mBtn(btn)
    {
      mLastPressed = false;
    }
    virtual ~SwitchBtnPoller(){
    }

    void doCallback(void)
    {
#ifdef TARGET_BOARD_M5STICK_C
      M5.update();
      bool bPressed = (BUTTON_A == mBtn && M5.BtnA.isPressed()) || (BUTTON_B == mBtn && M5.BtnB.isPressed());
#else
      bool bPressed = false;
#endif
      if ( !mLastPressed && bPressed ) {
        if(mpFunc){
          mpFunc(mpArg);
        }
      }
      mLastPressed = bPressed;
    }
};

typedef struct buttonArg
{
  PowerControl* pPowerControl;
  PowerControlPoller* pPowerControllerPoller;
} BUTTON_ARG;

void doButtonPressedHandler(void* pArg)
{
  BUTTON_ARG* pButtonArg = reinterpret_cast<BUTTON_ARG*>(pArg);
  if(pButtonArg && pButtonArg->pPowerControl && pButtonArg->pPowerControllerPoller){
    pButtonArg->pPowerControl->setPower(!pButtonArg->pPowerControl->getPowerStatus());
    bool curPowerStatus = pButtonArg->pPowerControl->getPowerStatus();

    DispManager::showTentativeText(curPowerStatus ? "ON" : "OFF");
    TimePoller::pendingShow();

    pButtonArg->pPowerControllerPoller->notifyManualOperation(curPowerStatus);
  }
}

void doButtonPressedHandlerB(void* pArg)
{
  int mode = WiFiUtil::getMode();
  WiFiUtil::disconnect();

  switch( mode ){
    case WIFI_AP:
      WiFiUtil::setupWiFiClient();
      break;
    case WIFI_STA:
    case WIFI_OFF:
      WiFiUtil::setupWiFiAP();
      WebConfig::setup_httpd();
      break;
    default:;
  }
}

void DispManager::showTentativeText(const char* text, int sx, int sy, int textSize)
{
  LCD.fillScreen(BLACK);
  LCD.setCursor(sx, sy);
  LCD.setTextSize(textSize);
  LCD.print(text);

  TimePoller::pendingShow(); // TODO : the implementation will be moved from TimePoller
}

void DispManager::showDateClock(const char* text, int sx, int sy, int textSize)
{
  LCD.fillScreen(BLACK);
  LCD.setCursor(sx, sy);
  LCD.setTextSize(textSize);
  LCD.print(text);
}

// --- General setup() function
void setup() {
  // Initialize SerialPort
  Serial.begin(115200);

  // Initialize GPIO
  initializeGPIO();
  LCD_initialize();

  // Initialize SPI File System
  bool bSPIFFS = SPIFFS.begin();
  if(!bSPIFFS){
    SPIFFS.format();
  }

  // Check mode
  if(initializeProperMode(bSPIFFS)){
    static TimePoller* sPoll=new TimePoller(1000);
    g_LooperThreadManager.add(sPoll);
  }

  MyNetHandler::setup();

  static GpioDetector humanDetector(HUMAN_DETCTOR_PIN, true, HUMAN_UNDETECT_TIMEOUT);
#if ENABLE_IR_REMOTE_CONTROLLER
  static IrRemoteController remoteController(IR_SEND_PIN, KEYIrCodes);
#endif // ENABLE_IR_REMOTE_CONTROLLER
#if ENABLE_SWITCH_BOT_REMOTE_CONTROLLER
  SwitchBotUtil::loadConfig(); // get target mac Address from SPIFFS config file
  static SwitchBotRemoteController remoteController(0, SwitchBotUtil::getMode(0));
#endif // ENABLE_SWITCH_BOT_REMOTE_CONTROLLER
#if ENABLE_SWITCH_BOT_REMOTE_CONTROLLER || ENABLE_IR_REMOTE_CONTROLLER
  static PowerControl powerControl(&remoteController); // defined in config.cpp
  PowerControlPoller* pPowerControllerPoller = new PowerControlPoller(&powerControl, &humanDetector, HUMAN_UNDETECT_TIMEOUT, HUMAN_POLLING_PERIOD);
  if(pPowerControllerPoller){
    g_LooperThreadManager.add(pPowerControllerPoller);
  }
#endif

  static BUTTON_ARG arg;
#if ENABLE_SWITCH_BOT_REMOTE_CONTROLLER || ENABLE_IR_REMOTE_CONTROLLER
  arg.pPowerControl = &powerControl;
  arg.pPowerControllerPoller = pPowerControllerPoller;
#else
  arg.pPowerControl = NULL;
  arg.pPowerControllerPoller = NULL;
#endif

  SwitchBtnPoller* pSwitchBtnPoller = new SwitchBtnPoller(BTN_POLLING_PERIOD, doButtonPressedHandler, (void*)&arg);
  if(pSwitchBtnPoller){
    g_LooperThreadManager.add(pSwitchBtnPoller);
  }
  SwitchBtnPoller* pSwitchBtnPollerB = new SwitchBtnPoller(BTN_POLLING_PERIOD, doButtonPressedHandlerB, NULL, SwitchBtnPoller::BUTTON_B);
  if(pSwitchBtnPollerB){
    g_LooperThreadManager.add(pSwitchBtnPollerB);
  }

  WatchDog::enable(30000);
//  HeapWatchDog::enable(16384); // TODO: 16KB Is it possible to continue to work??
}

void loop() {
  // put your main code here, to run repeatedly
  WiFiUtil::handleWiFiClientStatus();
  WebConfig::handleWebServer();
#if ENABLE_SWITCH_BOT_REMOTE_CONTROLLER
  BleUtil::handleInLoop();
#endif // ENABLE_SWITCH_BOT_REMOTE_CONTROLLER
  g_LooperThreadManager.handleLooperThread();
  LCD_update();
  WatchDog::heartBeat();
  HeapWatchDog::check();
}
