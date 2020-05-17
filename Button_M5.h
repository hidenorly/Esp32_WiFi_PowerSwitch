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

#include "config.h"
#ifdef TARGET_BOARD_M5STICK_C
#ifndef __BUTTON_M5_H__
#define __BUTTON_M5_H__

#include "base.h"
#include "Button.h"

class M5Button : public IButton
{
public:
	typedef enum
	{
		BUTTON_A,
		BUTTON_B
	} BUTTON_ID;
	M5Button(BUTTON_ID id);
	~M5Button();
	virtual bool wasPressed(void);
	virtual bool isPressed(void);

protected:
	virtual void _update(void);
	BUTTON_ID mId;
};

#endif // __BUTTON_M5_H__
#endif // TARGET_BOARD_M5STICK_C
