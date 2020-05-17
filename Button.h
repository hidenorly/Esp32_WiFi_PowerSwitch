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

#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "base.h"

#ifndef BUTTON_POLLING_PEIROD_MSEC
#define BUTTON_POLLING_PEIROD_MSEC 5
#endif // BUTTON_POLLING_PEIROD_MSEC

class IButton
{
public:
	virtual bool wasPressed(void)=0;
	virtual bool isPressed(void)=0;
	static void update(void);

protected:
	virtual void _update(void)=0;
	static IButton* mpThis;
	static unsigned long mLastUpdated;
};

#endif // __BUTTON_H__
