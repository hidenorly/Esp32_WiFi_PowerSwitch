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
#include "PowerControl.h"
#include "PowerControlPoller.h"

#include "WiFiUtil.h"
#include "NtpUtil.h"
#include "WebConfig.h"
#include "WatchDog.h"
#include <SPIFFS.h>
#include <WiFi.h>

#define ENABLE_YMD 0

#define ENABLE_EXPERIMENTAL_HEAP_CHECKER 1
#define EXPERIMENTAL_HEAP_CHECKER_THRESHOLD 128000


// --- mode changer
bool initializeProperMode(bool bSPIFFS){
  M5.update();

  if( !bSPIFFS || M5.BtnA.wasPressed() || (!SPIFFS.exists(WIFI_CONFIG))){
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
  #define DEFAULT_DISP_SIZE 4
#else
  #define DEFAULT_DISP_SIZE 5
#endif

class DispManager
{
public:
  static void showTentativeText(const char* text, int sx=20, int sy=16, int textSize=7);
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

      WebConfig::setup_httpd();
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
      if(i % (NtpUtil::isTimeValid() ? NTP_SYNC_DURATION_NOT_SYNCED : NTP_SYNC_DURATION) == 0){
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
#if ENABLE_EXPERIMENTAL_HEAP_CHECKER
      DEBUG_PRINT("heap:");
      DEBUG_PRINTLN(xPortGetFreeHeapSize());
      if(xPortGetFreeHeapSize() < EXPERIMENTAL_HEAP_CHECKER_THRESHOLD){
        ESP.restart();
      }
#endif // ENABLE_EXPERIMENTAL_HEAP_CHECKER

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
  protected:
    BTN_CALLBACK_FUNC mpFunc;
    void* mpArg;

  public:
    SwitchBtnPoller(int dutyMSec=100, BTN_CALLBACK_FUNC pFunc=NULL, void* pArg=NULL):LooperThreadTicker(NULL, NULL, dutyMSec),mpFunc(pFunc),mpArg(pArg)
    {
    }
    virtual ~SwitchBtnPoller(){
    }

    void doCallback(void)
    {
      M5.update();
      if ( M5.BtnA.wasPressed() ) {
        if(mpFunc){
          mpFunc(mpArg);
        }
      }
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

void DispManager::showTentativeText(const char* text, int sx, int sy, int textSize)
{
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(sx, sy);
  M5.Lcd.setTextSize(textSize);
  M5.Lcd.print(text);

  TimePoller::pendingShow(); // TODO : the implementation will be moved from TimePoller
}

void DispManager::showDateClock(const char* text, int sx, int sy, int textSize)
{
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(sx, sy);
  M5.Lcd.setTextSize(textSize);
  M5.Lcd.print(text);
}

// --- General setup() function
void setup() {
  // Initialize SerialPort
  Serial.begin(115200);

  // Initialize GPIO
  initializeGPIO();

  // Initialize LCD
  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLACK);

  // Initialize SPI File System
  bool bSPIFFS = SPIFFS.begin();
  if(!bSPIFFS){
    SPIFFS.format();
  }

  // Check mode
  delay(1000);
  if(initializeProperMode(bSPIFFS)){
    static TimePoller* sPoll=new TimePoller(1000);
    g_LooperThreadManager.add(sPoll);
  }

  MyNetHandler::setup();

  static GpioDetector humanDetector(HUMAN_DETCTOR_PIN, true, HUMAN_UNDETECT_TIMEOUT);
  static IrRemoteController remoteController(IR_SEND_PIN, KEYIrCodes);
  static PowerControl powerControl(&remoteController); // defined in config.cpp
  PowerControlPoller* pPowerControllerPoller = new PowerControlPoller(&powerControl, &humanDetector, HUMAN_UNDETECT_TIMEOUT, HUMAN_POLLING_PERIOD);
  if(pPowerControllerPoller){
    g_LooperThreadManager.add(pPowerControllerPoller);
  }

  static BUTTON_ARG arg;
  arg.pPowerControl = &powerControl;
  arg.pPowerControllerPoller = pPowerControllerPoller;

  SwitchBtnPoller* pSwitchBtnPoller = new SwitchBtnPoller(BTN_POLLING_PERIOD, doButtonPressedHandler, (void*)&arg);
  if(pSwitchBtnPoller){
    g_LooperThreadManager.add(pSwitchBtnPoller);
  }

  WatchDog::enable();
}

void loop() {
  // put your main code here, to run repeatedly
  WiFiUtil::handleWiFiClientStatus();
  WebConfig::handleWebServer();
  g_LooperThreadManager.handleLooperThread();
  WatchDog::heartBeat();
}
