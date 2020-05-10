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
SwitchBotRemoteController::MODE_SWITCH_BOT SwitchBotUtil::mMode[MAX_SWITCH_BOT_DEVICES];

int SwitchBotRemoteController::mCountInitialized = 0;


SwitchBotRemoteController::SwitchBotRemoteController(int id, MODE_SWITCH_BOT mode):mId(id),mMode(mode)
{
	if( !mCountInitialized ){
		BleUtil::initialize();
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
			switch( mMode ){
				case MODE_ONOFF_ON_OFF:
				case MODE_ON_ON:
					actionSwitchBot(ACTION_TURN_ON);
					break;
				case MODE_ONOFF_OFF_ON:
				case MODE_ON_OFF:
					actionSwitchBot(ACTION_TURN_OFF);
					break;
			}
			break;
		case KEY_POWER_OFF:
			switch( mMode ){
				case MODE_ONOFF_ON_OFF:
				case MODE_OFF_OFF:
					actionSwitchBot(ACTION_TURN_OFF);
					break;
				case MODE_ONOFF_OFF_ON:
				case MODE_OFF_ON:
					actionSwitchBot(ACTION_TURN_ON);
					break;
			}
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

void SwitchBotRemoteController::setMode(MODE_SWITCH_BOT mode)
{
	mMode = mode;
}

SwitchBotRemoteController::MODE_SWITCH_BOT SwitchBotRemoteController::getMode(void)
{
	return mMode;
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

String SwitchBotUtil::_getModeString(SwitchBotRemoteController::MODE_SWITCH_BOT mode)
{
	switch( mode ){
		case SwitchBotRemoteController::MODE_ONOFF_ON_OFF:
			return "ONOFF_ON_OFF";
		case SwitchBotRemoteController::MODE_ONOFF_OFF_ON:
			return "ONOFF_OFF_ON";
		case SwitchBotRemoteController::MODE_ON_ON:
			return "ON_ON";
		case SwitchBotRemoteController::MODE_ON_OFF:
			return "ON_OFF";
		case SwitchBotRemoteController::MODE_OFF_ON:
			return "OFF_ON";
		case SwitchBotRemoteController::MODE_OFF_OFF:
			return "OFF_OFF";
	}
	return "ONOFF_ON_OFF";
}

SwitchBotRemoteController::MODE_SWITCH_BOT SwitchBotUtil::_getModeFromString(String mode)
{
	SwitchBotRemoteController::MODE_SWITCH_BOT m = SwitchBotRemoteController::MODE_ONOFF_ON_OFF;

	mode.trim();
	mode.toUpperCase();

	if( mode.equals("ONOFF_ON_OFF")){
		m = SwitchBotRemoteController::MODE_ONOFF_ON_OFF;
	} else if( mode.equals("ONOFF_OFF_ON") ){
		m = SwitchBotRemoteController::MODE_ONOFF_OFF_ON;
	} else if( mode.equals("ON_ON") ){
		m = SwitchBotRemoteController::MODE_ON_ON;
	} else if( mode.equals("ON_OFF") ){
		m = SwitchBotRemoteController::MODE_ON_OFF;
	} else if( mode.equals("OFF_ON") ){
		m = SwitchBotRemoteController::MODE_OFF_ON;
	} else if( mode.equals("OFF_OFF") ){
		m = SwitchBotRemoteController::MODE_OFF_OFF;
	}

	return m;
}

String SwitchBotUtil::getModeString(int id)
{
	if(id<0 || id>=MAX_SWITCH_BOT_DEVICES){
		id = 0;
	}
	return _getModeString( mMode[id] );
}


SwitchBotRemoteController::MODE_SWITCH_BOT SwitchBotUtil::getMode(int id)
{
	if(id<0 || id>=MAX_SWITCH_BOT_DEVICES){
		id = 0;
	}
	return mMode[id];
}

void SwitchBotUtil::setModeByString(String mode, int id)
{
	if(id>=0 && id<MAX_SWITCH_BOT_DEVICES){
		mMode[id] = _getModeFromString(mode);
	}
}

void SwitchBotUtil::setMode(SwitchBotRemoteController::MODE_SWITCH_BOT mode, int id)
{
	if(id>=0 && id<MAX_SWITCH_BOT_DEVICES){
		mMode[id] = _getModeFromString( _getModeString( mode) );
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
			f.println( getModeString(i) );
		}
		f.close();
	}
}

void SwitchBotUtil::loadConfig(void)
{
	for(int i=0; i<MAX_SWITCH_BOT_DEVICES; i++){
		mBleAddr[i] = "";
		mMode[i] = SwitchBotRemoteController::MODE_ONOFF_ON_OFF;
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
			setModeByString( reverse, i );
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