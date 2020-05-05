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

class BleUtil : public BLEAdvertisedDeviceCallbacks
{
public:
	static void saveConfig(String bleaddr = "");
	static void loadConfig(void);

	static void initialize(void);
	static void uninitialize(void);

	static void startScan(bool is_continue=false);
	static void stopScan(void);
	static void setAdvertisingServiceUUID(String uuid);
	static String getTargetAdvertiseServiceUUID(void);

	class BleDevice
	{
	public:
		BleDevice(BLEAdvertisedDevice* pDevice, String advertiseServiceUUID);
		virtual ~BleDevice();

		void tryToConnect();
		void disconnect(void);
		bool isConnected();
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

	static void tryToConnect();
	static void disconnect(void);
	static bool isConnected();

	static void setTargetBleAddr(String bleAddr);
	static String getTargetBleAddr(void);
	static bool isFoundDevice(void);
	static void setTargetAdvertiseDevice(BLEAdvertisedDevice* pDevice);
	static BLEAdvertisedDevice* getFoundAdvertiseDevice(void);
	static BLERemoteService* getFoundRemoteService(void);
	static BLERemoteCharacteristic* getCharactertistic(String characteristicUUID);

	static bool writeToCharactertistic(String characteristicUUID, uint8_t* pData, size_t length);


protected:
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
	static String mAdvertiseServiceUUID;
	static BLEAdvertisedDevice* mpDevice;
	static BLEClient* mpBleClient;
};

#endif // __BLEUTIL_H__
