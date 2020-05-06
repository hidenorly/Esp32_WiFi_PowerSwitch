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

#ifndef __BLEUTIL_H__
#define __BLEUTIL_H__

#ifdef __BLESCAN__IMPL__
	#define BLESCAN_INTERVAL_MSEC	1349
	#define BLESCAN_WINDOW_MSEC		449
	#define BLESCAN_DURATION_MSEC	5
#endif // __BLESCAN__IMPL__

#include "BLEDevice.h"

#define MAX_UUID_SUBSCRITIONS 4

class BleUtil : public BLEAdvertisedDeviceCallbacks
{
public:
	static void initialize(void);
	static void uninitialize(void);

	static void startScan(bool is_continue=false);
	static void stopScan(void);

	class BleDevice
	{
	public:
		BleDevice(BLEAdvertisedDevice* pDevice=NULL, String advertiseServiceUUID="");
		virtual ~BleDevice();
		void tryToConnect(void);
		void disconnect(void);
		bool isConnected(void);
		bool writeToCharactertistic(String characteristicUUID, uint8_t* pData, size_t length);

		// primitive functions
		BLEAdvertisedDevice* getDevice(void);
		BLERemoteService* getRemoteService(void);
		BLERemoteCharacteristic* getCharactertistic(String characteristicUUID);

	protected:
		BLEClient* mpBleClient;
		String mAdvertiseServiceUUID;
		BLEAdvertisedDevice* mpDevice;
	};

	static void subscribeAdvertiseService(String uuid, String targetMacAddr = "");
	static void unsubscribeAdvertiseService(String uuid, String targetMacAddr = "");
	static void clearAllSubscribedAdvertiseServices(void);

	static BleDevice* getFoundAdvertiseDevice(String uuid, String targetMacAddr = "");

protected:
	static int _getIndexOfSubscribedAdvertiseService(String uuid, String targetMacAddr = "");
	static void _getSubscribedInfo(int index, String& uuid, String& targetMacAddr);
	static void _registerFoundDevice(int index, BleDevice* pDevice);

	class _BleAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
	{
	public:
		_BleAdvertisedDeviceCallbacks();
		void onResult(BLEAdvertisedDevice advertisedDevice);
	};
	static _BleAdvertisedDeviceCallbacks* mpBleAdvertiseCallback;

protected:
	static String mBleAddr;
	static BLEScan* mpBleScan;
	static BleDevice* mpDevices[MAX_UUID_SUBSCRITIONS];
	static String mTargetBleAddr[MAX_UUID_SUBSCRITIONS];
	static String mSubscribedAdvertiseUUIDs[MAX_UUID_SUBSCRITIONS];
};

#endif // __BLEUTIL_H__
