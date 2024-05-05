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

#ifndef __WEBPOWERCONTROLLER_H__
#define __WEBPOWERCONTROLLER_H__

#include <WebServer.h>

typedef void (*POWERCONTROL_CALLBACK_FUNC)(bool);

class WebPowerController
{
public:
	static WebServer* setup_httpd(WebServer* pWebServer=NULL, POWERCONTROL_CALLBACK_FUNC pPowerControlFunc=NULL);
	static void handleWebServer(void);

protected:
	static void httpd_handleRootGet(void);

	static WebServer* mpHttpd; // http server for WiFi AP Mode
	static POWERCONTROL_CALLBACK_FUNC mpPowerControlFunc;
};

#endif // __WEBPOWERCONTROLLER_H__