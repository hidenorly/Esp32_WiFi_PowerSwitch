/* 
 Copyright (C) 2016, 2018, 2019, 2020, 2024 hidenorly

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
#if ENABLE_SWITCH_BOT_REMOTE_CONTROLLER
#include "RemoteController_SwitchBot.h"
#endif // ENABLE_SWITCH_BOT_REMOTE_CONTROLLER
#include "NtpUtil.h"
#include <WiFi.h>
#include <WebServer.h>
#include <FS.h>
#include "WebConfig.h"
#include <SPIFFS.h>

// --- HTTP server related
WebServer* WebConfig::mpHttpd = NULL; // http server for WiFi AP Mode
const char* const WebConfig::HTML_TAIL = "</body></html>";


WebServer* WebConfig::setup_httpd(WebServer* pWebServer)
{
  if( mpHttpd == NULL ){
    mpHttpd = pWebServer;
    if( mpHttpd == NULL ){
      mpHttpd = new WebServer(HTTP_SERVER_PORT);
    }
  }
  mpHttpd->on("/", HTTP_GET, httpd_handleRootGet);
  mpHttpd->on("/", HTTP_POST, httpd_handleRootPost);
  mpHttpd->begin();
  DEBUG_PRINTLN("HTTP server started.");

  return mpHttpd;
}

void WebConfig::httpd_handleRootGet(void)
{
  DEBUG_PRINTLN("Receive: GET /");
  if (mpHttpd != NULL) {
    String html = HTML_HEAD;
    html += "<h1>WiFi/Power Control Settings</h1>";
    html += "<form method='post'>";
    html += "  <input type='text' name='ssid' placeholder='ssid'><br>";
    html += "  <input type='text' name='pass' placeholder='pass'><br>";
#if ENABLE_SWITCH_BOT_REMOTE_CONTROLLER
    html += "  <input type='text' name='bleaddr' placeholder='bleaddr' value='";
    html += SwitchBotUtil::getTargetBleAddr();
    html += "''><br>";
    html += "  <input type='text' name='reverse' placeholder='reverse' value='";
    html += SwitchBotUtil::getModeString();
    html += "''><br>";
#endif // ENABLE_SWITCH_BOT_REMOTE_CONTROLLER
    html += "  <input type='text' name='ntpserver' placeholder='ntpserver' value='";
    html += NtpUtil::getServer();
    html += "''><br>";
    html += "  <input type='text' name='timeoffset' placeholder='timeoffset' value='";
    html += String(NtpUtil::getTimeZoneOffset());
    html += "''><br>";
    html += "  <input type='submit'><br>";
    html += "</form>";
    html += HTML_TAIL;
    mpHttpd->send(200, "text/html", html);
  }
}

void WebConfig::httpd_handleRootPost(void)
{
  DEBUG_PRINTLN("Receive: POST /");
  if( mpHttpd != NULL ){
    String ssid = mpHttpd->arg("ssid");
    String pass = mpHttpd->arg("pass");
#if ENABLE_SWITCH_BOT_REMOTE_CONTROLLER
    String bleaddr = mpHttpd->arg("bleaddr");
    String reverse = mpHttpd->arg("reverse");
    reverse.trim();
#endif // ENABLE_SWITCH_BOT_REMOTE_CONTROLLER
    String ntpserver = mpHttpd->arg("ntpserver");
    String timeoffset = mpHttpd->arg("timeoffset");
  
    // --- SSID & Password are specified.
    if( (ssid=="format") && (pass=="format") ){
      SPIFFS.format();
      String html = HTML_HEAD;
      html += "Format successful on SPIFFS!";
      html += HTML_TAIL;
      mpHttpd->send(200, "text/html", html);
    } else {
      // config
      WiFiUtil::saveWiFiConfig(ssid, pass);
#if ENABLE_SWITCH_BOT_REMOTE_CONTROLLER
      if( bleaddr.length() ){
        SwitchBotUtil::setTargetBleAddr(bleaddr);
      }
      SwitchBotUtil::setModeByString(reverse);
      SwitchBotUtil::saveConfig();
#endif // ENABLE_SWITCH_BOT_REMOTE_CONTROLLER
      NtpUtil::saveConfig(ntpserver, timeoffset);
    }
    
    String html = HTML_HEAD;
    html += "<h1>WiFi Settings</h1>";
    html += ((ssid!="") ? ssid : "ssid isn't changed") + "<br>";
    html += ((pass!="") ? pass : "password isn't changed") + "<br>";
#if ENABLE_SWITCH_BOT_REMOTE_CONTROLLER
    html += "<h1>Switch Bot Settings</h1>";
    html += ((bleaddr!="") ? bleaddr : "bleaddr isn't changed") + "<br>";
    html += ((reverse!="") ? reverse : "mode isn't changed") + "<br>";
#endif // ENABLE_SWITCH_BOT_REMOTE_CONTROLLER
    html += "<h1>Ntp Settings</h1>";
    html += ((ntpserver!="") ? ntpserver : "ntpserver isn't changed") + "<br>";
    html += ((timeoffset!="") ? timeoffset : "timeoffset isn't changed") + "<br>";
    html += HTML_TAIL;

    mpHttpd->send(200, "text/html", html);

    delay(1000*3);

    ESP.restart();
  }
}

void WebConfig::handleWebServer()
{
  if( mpHttpd ){
    mpHttpd->handleClient();
  }
}
