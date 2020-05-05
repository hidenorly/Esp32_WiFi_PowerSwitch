/* 
 Copyright (C) 2020 hidenorly

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

#ifndef __REMOTE_CONTROLLER_SWITCH_BOT_H__
#define __REMOTE_CONTROLLER_SWITCH_BOT_H__

#include "base.h"
#include "RemoteController.h"

#ifdef __REMOTE_CONTROLLER_SWITCH_BOT_IMPL_H__
  #define SWITCH_BOT_SERVICE_UUID "CBA20D00-224D-11E6-9FB8-0002A5D5C51B"
  #define SWITCH_BOT_CHARACTERISTIC_UUID "CBA20002-224D-11E6-9FB8-0002A5D5C51B"
#endif // __REMOTE_CONTROLLER_SWITCH_BOT_IMPL_H__

class SwitchBotRemoteController : public IRemoteController
{
public:
  SwitchBotRemoteController();
  virtual ~SwitchBotRemoteController();

  typedef enum
  {
  	ACTION_PRESS,
  	ACTION_TURN_ON,
  	ACTION_TURN_OFF
  } ACTION_SWITCH_BOT;
  void actionSwitchBot(ACTION_SWITCH_BOT action);

  virtual void sendKey(int keyCode);
};


#endif // __REMOTE_CONTROLLER_SWITCH_BOT_H__