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

#include "base.h"
#include "Button.h"

IButton* IButton::mpThis = NULL;
unsigned long IButton::mLastUpdated = 0;

void IButton::update(void)
{
	unsigned long n = millis();
	if( (n - mLastUpdated) < BUTTON_POLLING_PEIROD_MSEC ) return; 
	if(mpThis)
	{
		mpThis->_update();
	}
	mLastUpdated = n;
}
