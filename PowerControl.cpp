/* 
 Copyright (C) 2019, 2020 hidenorly

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
#include "PowerControl.h"

PowerControl::PowerControl(IRemoteController* pController):mpController(pController)
{

}

PowerControl::~PowerControl()
{

}

void PowerControl::setPower(bool bPower)
{
	if( mpController ){
		if( bPower != getPowerStatus() ){
			DEBUG_PRINT("Power :");
			DEBUG_PRINTLN(bPower ? "ON" : "OFF");
			mpController->sendKey(bPower ? IRemoteController::KEY_POWER_ON : IRemoteController::KEY_POWER_OFF);
			mbPowerStatus = bPower;
		}
	}
}

bool PowerControl::getPowerStatus(void)
{
	if( mpController && mpController->isControlStatusAvailable() ){
		return mpController->getPowerStatus();
	}
	return mbPowerStatus;
}
