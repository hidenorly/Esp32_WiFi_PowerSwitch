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
#include <SPIFFS.h>
#include "BLEDevice.h"


String BleUtil::mBleAddr = "";
BLEScan* BleUtil::mpBleScan = NULL;
String BleUtil::mAdvertiseServiceUUID = "";
BLEAdvertisedDevice* BleUtil::mpDevice = NULL;
BLEClient* BleUtil::mpBleClient = NULL;
BleUtil::_BleAdvertisedDeviceCallbacks* BleUtil::mpBleAdvertiseCallback = NULL;


void BleUtil::setTargetBleAddr(String bleAddr)
{
  mBleAddr = bleAddr;
}

String BleUtil::getTargetBleAddr(void)
{
  return mBleAddr;
}

void BleUtil::saveConfig(String bleaddr)
{
  if( bleaddr=="" ){
    bleaddr = getTargetBleAddr();
  }
  if ( SPIFFS.exists(BLE_CONFIG) ) {
    SPIFFS.remove(BLE_CONFIG);
  }

  File f = SPIFFS.open(BLE_CONFIG, "w");
  f.println(bleaddr);
  f.close();

  mBleAddr = bleaddr;
}

void BleUtil::loadConfig(void)
{
  if ( SPIFFS.exists(BLE_CONFIG) ) {
    // found existing config
    File f = SPIFFS.open(BLE_CONFIG, "r");
    String bleaddr = f.readStringUntil('\n');
    f.close();

    bleaddr.trim();
    mBleAddr = bleaddr;
    DEBUG_PRINT("Loaded. Target BLE Addr :");
    DEBUG_PRINTLN(mBleAddr);
  } else {
    // not found existing config
    mBleAddr = "";
  }
}

void BleUtil::initialize(void)
{
  DEBUG_PRINTLN("BleUtil::initialize()");

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

  if(mpDevice){
    delete mpDevice;
    mpDevice = NULL;
  }

  if( mpBleAdvertiseCallback ) {
    if(mpBleScan && mpBleAdvertiseCallback){
      mpBleScan->setAdvertisedDeviceCallbacks(NULL);
    }
    delete mpBleAdvertiseCallback;
    mpBleAdvertiseCallback = NULL;
  }

  mpBleScan = NULL;
}

void BleUtil::startScan(bool is_continue)
{
  if( mpBleScan && mpBleAdvertiseCallback ){
    mpBleScan->setAdvertisedDeviceCallbacks(mpBleAdvertiseCallback);
    mpBleScan->setInterval(BLESCAN_INTERVAL_MSEC);
    mpBleScan->setWindow(BLESCAN_WINDOW_MSEC);
    mpBleScan->setActiveScan(true);
    mpBleScan->start(BLESCAN_DURATION_MSEC, is_continue);
    DEBUG_PRINTLN("Start BLE scan");
  }
}

void BleUtil::stopScan(void)
{
  if( mpBleScan ){
    DEBUG_PRINTLN("Stop BLE scan");
    mpBleScan->stop();
  }
}


void BleUtil::setAdvertisingServiceUUID(String uuid)
{
  mAdvertiseServiceUUID = uuid;
}


String BleUtil::getTargetAdvertiseServiceUUID(void)
{
  return mAdvertiseServiceUUID;
}


BleUtil::_BleAdvertisedDeviceCallbacks::_BleAdvertisedDeviceCallbacks()
{

}

void BleUtil::_BleAdvertisedDeviceCallbacks::onResult(BLEAdvertisedDevice advertisedDevice)
{
  DEBUG_PRINT("Found BLE device: ");
  DEBUG_PRINTLN( advertisedDevice.toString().c_str() );

  if (!isFoundDevice() && advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService( BLEUUID::fromString( BleUtil::getTargetAdvertiseServiceUUID().c_str() ) ) ) {
    bool bFound = true;
    if( BleUtil::getTargetBleAddr() ){
      if( !( String(advertisedDevice.getAddress().toString().c_str()).equalsIgnoreCase(BleUtil::getTargetBleAddr())) ){
        bFound = false;
      }
    }
    if( bFound ) {
      DEBUG_PRINTLN("!!! Found target service UUID BLE device !!!");
      BleUtil::stopScan();
      BleUtil::setTargetAdvertiseDevice( new BLEAdvertisedDevice(advertisedDevice) );
    }
  }
}

bool BleUtil::isFoundDevice(void)
{
  return mpDevice ? true : false;
}


BLEAdvertisedDevice* BleUtil::getFoundAdvertiseDevice(void)
{
  return mpDevice;
}

void BleUtil::setTargetAdvertiseDevice(BLEAdvertisedDevice* pDevice)
{
  mpDevice = pDevice;
}

void BleUtil::tryToConnect()
{
  if( mpDevice ){
    if( !mpBleClient ){
      mpBleClient = BLEDevice::createClient();
    }
    if( mpBleClient ){
      DEBUG_PRINTLN("tryToConnect()");
      mpBleClient->connect( mpDevice );
    }
  }
}

void BleUtil::disconnect(void)
{
  if(mpBleClient){
    DEBUG_PRINTLN("disconnect()");
    mpBleClient->disconnect();
    mpBleClient = NULL;
  }
}

bool BleUtil::isConnected()
{
  return mpBleClient ? true : false;
}


BLERemoteService* BleUtil::getFoundRemoteService(void)
{
  BLERemoteService* pRemoteService = NULL;

  if(mpBleClient){
    pRemoteService = mpBleClient->getService(mAdvertiseServiceUUID.c_str());
  }

  return pRemoteService;
}

BLERemoteCharacteristic* BleUtil::getCharactertistic(String characteristicUUID)
{
  BLERemoteCharacteristic* pCharacteristic = NULL;

  BLERemoteService* pRemoteService = getFoundRemoteService();
  if( pRemoteService ){
    pCharacteristic = pRemoteService->getCharacteristic(characteristicUUID.c_str());
  }

  return pCharacteristic;
}

bool BleUtil::writeToCharactertistic(String characteristicUUID, uint8_t* pData, size_t length)
{
  bool bResult = false;

  BLERemoteCharacteristic* pCharacteristic = getCharactertistic(characteristicUUID.c_str());
  if( pCharacteristic ){
    DEBUG_PRINT("Send data to ");
    DEBUG_PRINT(getFoundRemoteService()->toString().c_str());
    DEBUG_PRINT(" (");
    DEBUG_PRINT(characteristicUUID);
    DEBUG_PRINTLN(")");
    pCharacteristic->writeValue(pData, length, false);
  }

  return bResult;
}

