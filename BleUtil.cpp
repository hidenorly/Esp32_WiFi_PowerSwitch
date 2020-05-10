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
#include "config.h"
#define __BLESCAN__IMPL__
#include "BleUtil.h"
#include "BLEDevice.h"

BLEScan* BleUtil::mpBleScan = NULL;
bool BleUtil::mbScanning = false;
unsigned long BleUtil::mLastScan = 0;

BleUtil::BleDevice* BleUtil::mpDevices[MAX_UUID_SUBSCRITIONS]={0};
String BleUtil::mTargetBleAddr[MAX_UUID_SUBSCRITIONS];
String BleUtil::mSubscribedAdvertiseUUIDs[MAX_UUID_SUBSCRITIONS];

BleUtil::_BleAdvertisedDeviceCallbacks* BleUtil::mpBleAdvertiseCallback = NULL;


void BleUtil::initialize(void)
{
  DEBUG_PRINTLN("BleUtil::initialize()");

  for(int i=0; i<MAX_UUID_SUBSCRITIONS; i++){
    mpDevices[i] = NULL;
    mTargetBleAddr[i] = "";
    mSubscribedAdvertiseUUIDs[i] = "";
  }

  if( mpBleAdvertiseCallback ) {
    delete mpBleAdvertiseCallback;
    mpBleAdvertiseCallback = NULL;
  }
  mpBleAdvertiseCallback = new _BleAdvertisedDeviceCallbacks();

  BLEDevice::init("");
  if(!mpBleScan){
    mpBleScan = BLEDevice::getScan();
    if( mpBleScan ){
      mpBleScan->clearResults();
    }
  }
}

void BleUtil::uninitialize(void)
{
  DEBUG_PRINTLN("BleUtil::uninitialize()");
  BleUtil::stopScan();

  clearAllSubscribedAdvertiseServices();

  if( mpBleAdvertiseCallback ) {
    if(mpBleScan && mpBleAdvertiseCallback){
      mpBleScan->setAdvertisedDeviceCallbacks(NULL);
    }
    delete mpBleAdvertiseCallback;
    mpBleAdvertiseCallback = NULL;
  }

  mpBleScan = NULL;
  BLEDevice::deinit(true);
}

void BleUtil::startScan(bool is_continue)
{
  if( mpBleScan && mpBleAdvertiseCallback ){
    mpBleScan->clearResults();
    mpBleScan->setInterval(BLESCAN_INTERVAL_MSEC);
    mpBleScan->setWindow(BLESCAN_WINDOW_MSEC);
    mpBleScan->setActiveScan(true);
    mpBleScan->setAdvertisedDeviceCallbacks(mpBleAdvertiseCallback);
    mpBleScan->start(BLESCAN_DURATION_SEC, is_continue);
    mbScanning = is_continue;
    DEBUG_PRINTLN("Start BLE scan");
  }
}

void BleUtil::stopScan(void)
{
  if( mpBleScan ){
    DEBUG_PRINTLN("Stop BLE scan");
    mpBleScan->stop();
    mbScanning = false;
  }
}

#define NOT_FOUND -1

int BleUtil::_getIndexOfSubscribedAdvertiseService(String uuid, String targetMacAddr)
{
  int result = NOT_FOUND;
  bool bTargetMacAddrExist = targetMacAddr.length();
  for(int i=0; i<MAX_UUID_SUBSCRITIONS; i++){
    if( mSubscribedAdvertiseUUIDs[i].equalsIgnoreCase(uuid) ) {
      if( !bTargetMacAddrExist || (bTargetMacAddrExist && mTargetBleAddr[i].equalsIgnoreCase(targetMacAddr)) ){
        result = i;
        break; 
      }
    }
  }
  return result;
}


void BleUtil::subscribeAdvertiseService(String uuid, String targetMacAddr)
{
  if( NOT_FOUND == _getIndexOfSubscribedAdvertiseService(uuid, targetMacAddr) ){
    for(int i=0; i<MAX_UUID_SUBSCRITIONS; i++){
      if( !mSubscribedAdvertiseUUIDs[i].length() ) {
        mSubscribedAdvertiseUUIDs[i] = uuid;
        mTargetBleAddr[i] = targetMacAddr;
        // just in case
        if( mpDevices[i] ){
          delete mpDevices[i];
          mpDevices[i] = NULL;
        }
        break;
      }
    }
  }
}

void BleUtil::unsubscribeAdvertiseService(String uuid, String targetMacAddr)
{
  int index = _getIndexOfSubscribedAdvertiseService(uuid);
  if( NOT_FOUND != index ){
    // TODO: do unscribe related before termination
    mSubscribedAdvertiseUUIDs[index] = "";
    mTargetBleAddr[index] = "";
    if( mpDevices[index] ){
      delete mpDevices[index];
      mpDevices[index] = NULL;
    }
  }
}

bool BleUtil::_isFoundAllOfSubscribedDevice(void)
{
  bool bResult = true;

  for(int i=0; i<MAX_UUID_SUBSCRITIONS && bResult; i++){
    if( mSubscribedAdvertiseUUIDs[i].length() ) {
      bResult = bResult & (NULL != getFoundAdvertiseDevice(mSubscribedAdvertiseUUIDs[i], mTargetBleAddr[i]) );
    }
  }

  return bResult;
}


void BleUtil::handleInLoop(void)
{
  if( mbScanning ){
    if( !_isFoundAllOfSubscribedDevice() ){
      unsigned long n = millis();
      // not found all of subscribed devices!
      if( (n - mLastScan) < (BLESCAN_DURATION_SEC*2) ) return;
      DEBUG_PRINTLN("Scan again...");
      startScan(mbScanning);
      mLastScan = n;
    } else {
      // found all of subscribed devices!
      stopScan();
    }
  }
}


void BleUtil::clearAllSubscribedAdvertiseServices(void)
{
  for(int i=0; i<MAX_UUID_SUBSCRITIONS; i++){
    if(mSubscribedAdvertiseUUIDs[i]){
      unsubscribeAdvertiseService(mSubscribedAdvertiseUUIDs[i]);
    }
  }
}

BleUtil::BleDevice* BleUtil::getFoundAdvertiseDevice(String uuid, String targetMacAddr)
{
  BleUtil::BleDevice* result = NULL;

  int index = _getIndexOfSubscribedAdvertiseService(uuid, targetMacAddr);
  if( NOT_FOUND != index ){
    result = mpDevices[index];
  }

  return result;
}

void BleUtil::_getSubscribedInfo(int index, String& uuid, String& targetMacAddr)
{
  if( index>=0 && index<MAX_UUID_SUBSCRITIONS ){
    uuid = mSubscribedAdvertiseUUIDs[index];
    targetMacAddr = mTargetBleAddr[index];
  }
}

void BleUtil::_registerFoundDevice(int index, BleUtil::BleDevice* pDevice)
{
  if( index>=0 && index<MAX_UUID_SUBSCRITIONS ){
    if( mpDevices[index] ){
      // just in case
      delete mpDevices[index];
      mpDevices[index] = NULL;
    }
    mpDevices[index] = pDevice;
  }
}


BleUtil::_BleAdvertisedDeviceCallbacks::_BleAdvertisedDeviceCallbacks()
{

}

void BleUtil::_BleAdvertisedDeviceCallbacks::onResult(BLEAdvertisedDevice advertisedDevice)
{
  DEBUG_PRINT("Found BLE device: ");
  DEBUG_PRINTLN( advertisedDevice.toString().c_str() );

  if( advertisedDevice.haveServiceUUID() ){
    for(int i=0; i<MAX_UUID_SUBSCRITIONS; i++){
      String targetUUID = "";
      String targetMacAddr = "";
      BleUtil::_getSubscribedInfo(i, targetUUID, targetMacAddr);

      if( advertisedDevice.isAdvertisingService( BLEUUID::fromString( targetUUID.c_str() ) ) ) {
        bool bFound = true;
        if( targetMacAddr.length() ){
          if( !( String(advertisedDevice.getAddress().toString().c_str()).equalsIgnoreCase(targetMacAddr)) ){
            bFound = false;
          }
        }
        if( bFound ) {
          DEBUG_PRINTLN("!!! Found target service UUID BLE device !!!");
          if( !BleUtil::getFoundAdvertiseDevice(targetUUID, targetMacAddr) ){
            DEBUG_PRINT("!!! Register the device at ");
            DEBUG_PRINTLN(i);
            BLEAdvertisedDevice* pAdvertisedDevice = new BLEAdvertisedDevice(advertisedDevice);
            BleUtil::BleDevice* pBleDevice = new BleUtil::BleDevice(pAdvertisedDevice, targetUUID);
            BleUtil::_registerFoundDevice( i, pBleDevice );
          } else {
            DEBUG_PRINTLN("!!! Already the device was registered.");
          }
          break;
        }
      }
    }
  }
}



BleUtil::BleDevice::BleDevice(BLEAdvertisedDevice* pDevice, String advertiseServiceUUID)
{
  mpDevice = pDevice;
  mAdvertiseServiceUUID = advertiseServiceUUID;
}

BleUtil::BleDevice::~BleDevice()
{
  disconnect();
  DEBUG_PRINTLN("BleDevice::~BleDevice()");
  delete mpDevice; mpDevice = NULL;
}

void BleUtil::BleDevice::tryToConnect(void)
{
  DEBUG_PRINTLN("tryToConnect()");
  if( !isConnected() ){
    if( mpDevice ){
      if( !mpBleClient ){
        mpBleClient = BLEDevice::createClient();
        mpBleClient->setClientCallbacks(this);
      }
      if( mpBleClient ){
        mpBleClient->connect( mpDevice );
        DEBUG_PRINTLN("tryToConnect():done");
      }
    }
  }
}

void BleUtil::BleDevice::disconnect(void)
{
  if(mpBleClient){
    DEBUG_PRINTLN("disconnect()");
    mpBleClient->disconnect();
    mpBleClient->setClientCallbacks(NULL);
    delete mpBleClient;
    mpBleClient = NULL;
  }
}

bool BleUtil::BleDevice::isConnected(void)
{
  return mpBleClient && mpBleClient->isConnected();
}

void BleUtil::BleDevice::onConnect(BLEClient *pClient)
{
  DEBUG_PRINTLN("BleDevice::onConnect");
  if( pClient ){
    DEBUG_PRINTLN(pClient->toString().c_str());
  }
}

void BleUtil::BleDevice::onDisconnect(BLEClient *pClient)
{
  DEBUG_PRINTLN("BleDevice::onDisconnect");
  if( pClient ){
    DEBUG_PRINTLN(pClient->toString().c_str());
  }
}


bool BleUtil::BleDevice::writeToCharactertistic(String characteristicUUID, uint8_t* pData, size_t length)
{
  bool bResult = false;

  DEBUG_PRINTLN("!!! BleDevice::writeToCharactertistic");

  if( isConnected() ){
    BLERemoteCharacteristic* pCharacteristic = getCharactertistic(characteristicUUID.c_str());
    if( pCharacteristic ){
      DEBUG_PRINT("Send data to ");
      DEBUG_PRINT(getRemoteService()->toString().c_str());
      DEBUG_PRINT(" (");
      DEBUG_PRINT(characteristicUUID);
      DEBUG_PRINTLN(")");
      pCharacteristic->writeValue(pData, length, false);
    }
  }

  return bResult;
}

// primitive functions
BLEAdvertisedDevice* BleUtil::BleDevice::getDevice(void)
{
  return mpDevice;
}

BLERemoteService* BleUtil::BleDevice::getRemoteService(void)
{
  BLERemoteService* pRemoteService = NULL;
  DEBUG_PRINTLN("!!! BleDevice::getRemoteService");

  if( mpBleClient && isConnected() ){
    pRemoteService = mpBleClient->getService(mAdvertiseServiceUUID.c_str());
  }

  return pRemoteService;
}

BLERemoteCharacteristic* BleUtil::BleDevice::getCharactertistic(String characteristicUUID)
{
  BLERemoteCharacteristic* pCharacteristic = NULL;
  DEBUG_PRINTLN("!!! BleDevice::getCharacteristic");

  if( isConnected() ){
    BLERemoteService* pRemoteService = getRemoteService();
    if( pRemoteService ){
      pCharacteristic = pRemoteService->getCharacteristic(characteristicUUID.c_str());
      DEBUG_PRINTLN("!!! BleDevice::getCharacteristic:: done");
    }
  }

  return pCharacteristic;
}

