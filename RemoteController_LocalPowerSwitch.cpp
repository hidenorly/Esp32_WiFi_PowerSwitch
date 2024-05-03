/* 
 Copyright (C) 2020, 2024 hidenorly

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
#include "config.h"
#if ENABLE_LOCAL_POWER_CONTROLLER

#include "RemoteController.h"
#include "RemoteController_LocalPowerSwitch.h"


LocalPowerController::LocalPowerController(void):mbPowerOn(false)
{
}

LocalPowerController::~LocalPowerController()
{
}

void LocalPowerController::sendKey(int keyCode)
{
	DEBUG_PRINT("sendKey ");
	DEBUG_PRINTLN(keyCode);

#if LOCAL_POWER_CONTROLLER_MODE==0
	// --- ON/OFF mode
	switch(keyCode){
		case KEY_POWER_ON:
			mbPowerOn = true;
			break;
		case KEY_POWER_OFF:
			mbPowerOn = false;
			break;
		case KEY_POWER:
			mbPowerOn = !mbPowerOn;
			break;
		default:;
	}
	DEBUG_PRINT("Power Status: ");
	DEBUG_PRINTLN( mbPowerOn ? "ON" : "OFF");

	#ifdef LOCAL_POWER_CONTROLLER_GPO
	setOutputAndValue(LOCAL_POWER_CONTROLLER_GPO, mbPowerOn ? 1 : 0);
	#endif // LOCAL_POWER_CONTROLLER_GPO

#elif LOCAL_POWER_CONTROLLER_MODE==1 || LOCAL_POWER_CONTROLLER_MODE==2
	// --- pulse mode
	if(keyCode == KEY_POWER_ON || KEY_POWER){
		setOutputAndValue(LOCAL_POWER_CONTROLLER_GPO, LOCAL_POWER_CONTROLLER_PULSE_INACTIVE);
		delay(LOCAL_POWER_CONTROLLER_PULL_DURATION);
		setOutputAndValue(LOCAL_POWER_CONTROLLER_GPO, LOCAL_POWER_CONTROLLER_PULSE_ACTIVE);
		delay(LOCAL_POWER_CONTROLLER_PULL_DURATION);
		setOutputAndValue(LOCAL_POWER_CONTROLLER_GPO, LOCAL_POWER_CONTROLLER_PULSE_INACTIVE);
	}
#endif
}

bool LocalPowerController::isControlStatusAvailable(void)
{
	return true;
}

bool LocalPowerController::getPowerStatus(void)
{
	return mbPowerOn;
}


#endif // ENABLE_LOCAL_POWER_CONTROLLER