/* 
 Copyright (C) 2024 hidenorly

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
#include <WebServer.h>
#include "WebPowerController.h"

// --- HTTP server related
WebServer* WebPowerController::mpHttpd = NULL; // http server for WiFi AP Mode
POWERCONTROL_CALLBACK_FUNC WebPowerController::mpPowerControlFunc = NULL;

WebServer* WebPowerController::setup_httpd(WebServer* pWebServer, POWERCONTROL_CALLBACK_FUNC pPowerControlFunc)
{
  mpPowerControlFunc = pPowerControlFunc;
  if( mpHttpd == NULL ){
    mpHttpd = pWebServer;
    if( mpHttpd == NULL ){
      mpHttpd = new WebServer(HTTP_SERVER_PORT);
    }
  }
  mpHttpd->on("/powercontrol", HTTP_GET, httpd_handleRootGet);
  mpHttpd->begin();
  DEBUG_PRINTLN("HTTP server started.");
  return mpHttpd;
}

void WebPowerController::httpd_handleRootGet(void)
{
  DEBUG_PRINTLN("Receive: GET /powercontrol");
  if (mpHttpd != NULL) {
    String html = HTML_HEAD;
    html += "<h1>WiFi Remote Power Control</h1>";
    html += "Power control requested<br>";
    bool bRequestPower = true;
    if(mpHttpd->hasArg("power")){
      html += "Request: ";
      bRequestPower = mpHttpd->arg("power")=="on" ? true : false;
      html += ( bRequestPower ? "ON" : "OFF" );
      html += "<br>";
    }
    html += "</body></html>";
    mpHttpd->send(200, "text/html", html);
    if(mpPowerControlFunc){
      mpPowerControlFunc(bRequestPower);
    }
  }
}

void WebPowerController::handleWebServer()
{
  if( mpHttpd ){
    mpHttpd->handleClient();
  }
}
