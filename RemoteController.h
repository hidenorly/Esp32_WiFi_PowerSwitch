/* 
 Copyright (C) 2018,2019 hidenorly

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

#ifndef __REMOTE_CONTROLLER_H__
#define __REMOTE_CONTROLLER_H__

#include "base.h"

class IRemoteController
{
public:
	virtual void sendKey(int keyCode);

  enum {
    KEY_POWER = 0x1000,
    KEY_POWER_ON = 0x1001,
    KEY_POWER_OFF = 0x1002
   };

   virtual bool isControlStatusAvailable(void){ return false; };
   virtual bool getPowerStatus(void){ return false; };
};

#endif // __REMOTE_CONTROLLER_H__
