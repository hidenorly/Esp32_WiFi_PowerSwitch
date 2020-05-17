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
#include "base.h"
#include "Button_M5.h"

M5Button::M5Button(BUTTON_ID id):mId(id)
{
	mpThis = this;
}

M5Button::~M5Button()
{

}

bool M5Button::wasPressed(void)
{
	return (mId == BUTTON_A) ? M5.BtnA.wasPressed() : M5.BtnB.wasPressed();
}

bool M5Button::isPressed(void)
{
	return (mId == BUTTON_A) ? M5.BtnA.isPressed() : M5.BtnB.isPressed();
}

void M5Button::_update(void)
{
	M5.update();
}

#endif // TARGET_BOARD_M5STICK_C
