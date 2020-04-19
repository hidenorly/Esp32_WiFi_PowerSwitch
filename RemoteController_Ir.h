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

#ifndef __REMOTE_CONTROLLER_IR_H__
#define __REMOTE_CONTROLLER_IR_H__

#include "base.h"
#include "RemoteController.h"
#include <IRremoteESP8266.h>
#include <IRsend.h>

typedef struct KeyIrCode
{
  int keyCode;
  uint64_t irCode;
  int repeatCount;
  int repeatDelay;
} KEYIRCODE;

class IrRemoteController : public IRemoteController
{
public:
  IrRemoteController(int irPort, KEYIRCODE keys[]=NULL);
  virtual ~IrRemoteController();
  virtual void sendKey(int keyCode);

protected:
  KEYIRCODE* mpKeys;
  IRsend* mIRsend;
};

#endif // __REMOTE_CONTROLLER_IR_H__
