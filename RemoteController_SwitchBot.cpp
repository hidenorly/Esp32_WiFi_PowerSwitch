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

#include "RemoteController.h"
#define __REMOTE_CONTROLLER_SWITCH_BOT_IMPL_H__
#include "RemoteController_SwitchBot.h"
#include "BleUtil.h"


SwitchBotRemoteController::SwitchBotRemoteController()
{
	BleUtil::initialize();
	BleUtil::loadConfig(); // get target mac Address from SPIFFS config file
	BleUtil::subscribeAdvertiseService(SWITCH_BOT_SERVICE_UUID, BleUtil::getTargetBleAddr());
	BleUtil::startScan();
}

SwitchBotRemoteController::~SwitchBotRemoteController()
{
	BleUtil::uninitialize();
}

void SwitchBotRemoteController::sendKey(int keyCode)
{
	DEBUG_PRINT("sendKey ");
	DEBUG_PRINTLN(keyCode);
	// TODO : Change to table.
	switch(keyCode)
	{
		case KEY_POWER_ON:
			actionSwitchBot(ACTION_TURN_ON);
			break;
		case KEY_POWER_OFF:
			actionSwitchBot(ACTION_TURN_OFF);
			break;
		case KEY_POWER:
			actionSwitchBot(ACTION_PRESS);
			break;
	}	
}

void SwitchBotRemoteController::actionSwitchBot(ACTION_SWITCH_BOT action)
{
	uint8_t cmdPress[ACTION_TURN_OFF-ACTION_PRESS+1][3]={
		{0x57, 0x01, 0x00},	// ACTION_PRESS
		{0x57, 0x01, 0x01}, // ACTION_TURN_ON
		{0x57, 0x01, 0x02}	// ACTION_TURN_OFF
	};
	if( action>=ACTION_PRESS && action<=ACTION_TURN_OFF ){
		BleUtil::BleDevice* pDevice = BleUtil::getFoundAdvertiseDevice(SWITCH_BOT_SERVICE_UUID, BleUtil::getTargetBleAddr());
		if( pDevice ){
			pDevice->tryToConnect();
			pDevice->writeToCharactertistic(SWITCH_BOT_CHARACTERISTIC_UUID, cmdPress[action-ACTION_PRESS], 3);
			pDevice->disconnect();
		}
	}
}




