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
#if ENABLE_SWITCH_BOT_REMOTE_CONTROLLER

#include "RemoteController.h"
#define __REMOTE_CONTROLLER_SWITCH_BOT_IMPL_H__
#include "RemoteController_SwitchBot.h"
#include <SPIFFS.h>
#include "BleUtil.h"

String SwitchBotUtil::mBleAddr[MAX_SWITCH_BOT_DEVICES];
bool SwitchBotUtil::mbReverse[MAX_SWITCH_BOT_DEVICES];

int SwitchBotRemoteController::mCountInitialized = 0;


SwitchBotRemoteController::SwitchBotRemoteController(int id, bool bReverse):mId(id),mbReverse(bReverse)
{
	if( !mCountInitialized ){
		BleUtil::initialize();
		SwitchBotUtil::loadConfig(); // get target mac Address from SPIFFS config file
	}

	BleUtil::subscribeAdvertiseService(SWITCH_BOT_SERVICE_UUID, SwitchBotUtil::getTargetBleAddr(id));

	if( !mCountInitialized ){
		BleUtil::startScan(true);
		mCountInitialized++;
	}
}

SwitchBotRemoteController::~SwitchBotRemoteController()
{
	mCountInitialized--;
	if( !mCountInitialized ){
		BleUtil::uninitialize();
	}
}

void SwitchBotRemoteController::sendKey(int keyCode)
{
	DEBUG_PRINT("sendKey ");
	DEBUG_PRINTLN(keyCode);
	// TODO : Change to table.
	switch(keyCode)
	{
		case KEY_POWER_ON:
			actionSwitchBot(mbReverse ? ACTION_TURN_OFF : ACTION_TURN_ON);
			break;
		case KEY_POWER_OFF:
			actionSwitchBot(mbReverse ? ACTION_TURN_ON : ACTION_TURN_OFF);
			break;
		case KEY_POWER:
			actionSwitchBot(ACTION_PRESS);
			break;
	}	
}

void SwitchBotRemoteController::actionSwitchBot(ACTION_SWITCH_BOT action)
{
	uint8_t cmdPress[ACTION_END-ACTION_BEGIN][3]={
		{0x57, 0x01, 0x00},	// ACTION_PRESS
		{0x57, 0x01, 0x01}, // ACTION_TURN_ON
		{0x57, 0x01, 0x02}	// ACTION_TURN_OFF
	};
	if( action>=ACTION_BEGIN && action<ACTION_END ){
		BleUtil::BleDevice* pDevice = BleUtil::getFoundAdvertiseDevice(SWITCH_BOT_SERVICE_UUID, SwitchBotUtil::getTargetBleAddr());
		if( pDevice ){
			pDevice->tryToConnect();
			pDevice->writeToCharactertistic(SWITCH_BOT_CHARACTERISTIC_UUID, cmdPress[action-ACTION_PRESS], 3);
//			pDevice->disconnect();
		}
	}
}

void SwitchBotUtil::setTargetBleAddr(String bleAddr, int id)
{
	if(id>=0 && id<MAX_SWITCH_BOT_DEVICES){
		mBleAddr[id] = bleAddr;
	}
}

String SwitchBotUtil::getTargetBleAddr(int id)
{
	String result;
	if(id>=0 && id<MAX_SWITCH_BOT_DEVICES){
		result = mBleAddr[id];
	}
	return result;
}

bool SwitchBotUtil::getReverse(int id)
{
	bool bResult = false;

	if(id>=0 && id<MAX_SWITCH_BOT_DEVICES){
		bResult = mbReverse[id];
	}
	return bResult;
}

void SwitchBotUtil::setReverse(bool bReverse, int id)
{
	if(id>=0 && id<MAX_SWITCH_BOT_DEVICES){
		mbReverse[id] = bReverse;
	}
}

void SwitchBotUtil::saveConfig(void)
{
	{
		if ( SPIFFS.exists(SWITCHBOT_CONFIG) ) {
			SPIFFS.remove(SWITCHBOT_CONFIG);
		}
		File f = SPIFFS.open(SWITCHBOT_CONFIG, "w");
		for(int i=0; i<MAX_SWITCH_BOT_DEVICES; i++){
			f.println(mBleAddr[i]);
		}
		f.close();
	}

	{
		if ( SPIFFS.exists(SWITCHBOT_CONFIG_REVERSE) ) {
			SPIFFS.remove(SWITCHBOT_CONFIG_REVERSE);
		}
		File f = SPIFFS.open(SWITCHBOT_CONFIG_REVERSE, "w");
		for(int i=0; i<MAX_SWITCH_BOT_DEVICES; i++){
			f.println(mbReverse[i] ? "1" : "0");
		}
		f.close();
	}
}

void SwitchBotUtil::loadConfig(void)
{
	for(int i=0; i<MAX_SWITCH_BOT_DEVICES; i++){
		mBleAddr[i] = "";
		mbReverse[i] = false;
	}

	if ( SPIFFS.exists(SWITCHBOT_CONFIG) ) {
		// found existing config
		File f = SPIFFS.open(SWITCHBOT_CONFIG, "r");
		int i = 0;
		while( f.available() && i<MAX_SWITCH_BOT_DEVICES){
			String bleAddr = f.readStringUntil('\n');
			bleAddr.trim();
			mBleAddr[i] = bleAddr;
			DEBUG_PRINTLN("Load Config ");
			DEBUG_PRINT(i);
			DEBUG_PRINT(" ");
			DEBUG_PRINTLN(bleAddr);
			i++;
		}
		f.close();

		DEBUG_PRINTLN("Switchbot mac addreess(es) are loaded.");
	}

	if ( SPIFFS.exists(SWITCHBOT_CONFIG_REVERSE) ) {
		// found existing config
		File f = SPIFFS.open(SWITCHBOT_CONFIG_REVERSE, "r");
		int i = 0;
		while( f.available() && i<MAX_SWITCH_BOT_DEVICES){
			String reverse = f.readStringUntil('\n');
			reverse.trim();
			mbReverse[i] = reverse.equals("1") ? true : false;
			DEBUG_PRINTLN("Load Config ");
			DEBUG_PRINT(i);
			DEBUG_PRINT(" ");
			DEBUG_PRINTLN(reverse);
			i++;
		}
		f.close();

		DEBUG_PRINTLN("Switchbot reverse config(s) are loaded.");
	}
}

#endif // ENABLE_SWITCH_BOT_REMOTE_CONTROLLER