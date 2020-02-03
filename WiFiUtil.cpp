/* 
 Copyright (C) 2016,2019,2020 hidenorly

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
#include "WiFiUtil.h"

#include <Ticker.h>
#include <WiFi.h>
#include <SPIFFS.h>

CALLBACK_FUNC WiFiUtil::mpConnectedCallback = NULL;
void* WiFiUtil::mpConnectedArg = NULL;
CALLBACK_FUNC WiFiUtil::mpDisconnectedCallback = NULL;
void* WiFiUtil::mpDisconnectedArg = NULL;

Ticker WiFiUtil::mWifiStatusTracker;
bool WiFiUtil::mbNetworkConnected = false;
int WiFiUtil::mbPreviousNetworkConnected = false;

// For WiFi AP setup
#define DEFAULT_IPADDR ip(192, 168, 10, 1)
#define DEFAULT_SUBMASK subnet(255, 255, 255, 0)

// retry

String WiFiUtil::getDefaultSSID(void)
{
  byte mac[6];
  WiFi.macAddress(mac);
  String ssid = "";
  for (int i = 0; i < 6; i++) {
    ssid += String(mac[i], HEX);
  }
  return ssid;
}

void WiFiUtil::setupWiFiAP(void)
{
  String ssid = getDefaultSSID();
  DEBUG_PRINTLN("SSID: " + ssid);
  DEBUG_PRINT("PASS: ");
  DEBUG_PRINTLN(WIFIAP_PASSWORD);

  WiFi.mode(WIFI_AP);
  delay(100);
  WiFi.softAP(ssid.c_str(), WIFIAP_PASSWORD);
  delay(100);
  const IPAddress DEFAULT_IPADDR;
  const IPAddress DEFAULT_SUBMASK;
  WiFi.softAPConfig(ip, ip, subnet);
  IPAddress myIP = WiFi.softAPIP();
  DEBUG_PRINT("AP IP address: ");
  DEBUG_PRINTLN(myIP);
}

void WiFiUtil::saveWiFiConfig(String ssid, String pass)
{
  if( ssid!="" && pass!="" ) {
    if ( SPIFFS.exists(WIFI_CONFIG) ) {
      SPIFFS.remove(WIFI_CONFIG);
    }

    File f = SPIFFS.open(WIFI_CONFIG, "w");
    f.println(ssid);
    f.println(pass);
    f.close();
  }
}

void WiFiUtil::loadWiFiConfig(String& ssid, String& pass)
{
  File f = SPIFFS.open(WIFI_CONFIG, "r");
  ssid = f.readStringUntil('\n');
  pass = f.readStringUntil('\n');
  f.close();

  ssid.trim();
  pass.trim();
}

void WiFiUtil::setupWiFiClient(void)
{
  bool bCurrentStatus = mbNetworkConnected;
  mbNetworkConnected = false;

  if(bCurrentStatus){
    DEBUG_PRINTLN("restart system due to wifi failure.");
    ESP.restart();
  } else {
    DEBUG_PRINTLN("Setup WiFi as Client");
    String ssid="";
    String pass="";
    loadWiFiConfig(ssid, pass);

    DEBUG_PRINTLN("SSID: " + ssid);
    DEBUG_PRINTLN("PASS: " + pass);

    delay(100);
    WiFi.begin(ssid.c_str(), pass.c_str());
    delay(100);
    WiFi.mode(WIFI_STA);
    setupWiFiStatusTracker();
  }
}

void WiFiUtil::setupWiFiStatusTracker(void)
{
  static int bInitialized = false;
  if( !bInitialized ) {
    mWifiStatusTracker.attach_ms<CTrackerParam*>(500, WiFiUtil::checkWiFiStatus, NULL);
    bInitialized = true;
  }
}

void WiFiUtil::handleWiFiClientStatus(void)
{
  if( mbPreviousNetworkConnected != mbNetworkConnected ){
    mbPreviousNetworkConnected = mbNetworkConnected;
    if ( mbNetworkConnected ) {
      // network is connected!
      if(mpConnectedCallback){
        mpConnectedCallback(mpConnectedArg);
      }
    } else {
      // network is disconnected
      if(mpDisconnectedCallback){
        mpDisconnectedCallback(mpDisconnectedArg);
      }
    }
  }
}

void WiFiUtil::setStatusCallback(CALLBACK_FUNC pConnectedFunc, void* pConnectedArg, CALLBACK_FUNC pDisconnectedFunc, void* pDisconnectedArg)
{
  mpConnectedCallback = pConnectedFunc;
  mpConnectedArg = pConnectedArg;
  mpDisconnectedCallback = pDisconnectedFunc;
  mpDisconnectedArg = pDisconnectedArg;
}

void WiFiUtil::clearStatusCallback(void)
{
  mpConnectedCallback = NULL;
  mpConnectedArg = NULL;
  mpDisconnectedCallback = NULL;
  mpDisconnectedArg = NULL;
}

void WiFiUtil::checkWiFiStatus(CTrackerParam* p)
{
  static int previousStatus = WL_DISCONNECTED; //WL_IDLE_STATUS;
  int curStatus = WiFi.status();
  if( previousStatus == curStatus) {
    return;
  } else {
    DEBUG_PRINT("WiFi.status=");
    DEBUG_PRINTLN(curStatus);
    previousStatus = curStatus;
    switch (curStatus) {
      case WL_CONNECTED:
        mbNetworkConnected = true;
        break;
      case WL_CONNECT_FAILED:
      case WL_CONNECTION_LOST:
      case WL_DISCONNECTED:
        setupWiFiClient();  // retry...
        break;
      default:;
        break;
    }
  }
}

bool WiFiUtil::isNetworkAvailable(void)
{
  return mbNetworkConnected;
}

