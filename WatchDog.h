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

#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__

#include "esp_system.h"

class WatchDog
{
public:
  static void enable(int nPeriod = 3000);
  static void disable(void);
  static bool getEnabled(void);
  static void heartBeat(void);

protected:
  static hw_timer_t* mTimer;
};

#endif
