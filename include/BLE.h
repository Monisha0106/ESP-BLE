#ifndef BLE_H
#define BLE_H

#include<Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include<EEPROM.h>



// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
#define MAX_NUMBER_OF_DEVICES 100

class MyServerCallbacks: public BLEServerCallbacks 
{
    void onConnect(BLEServer* pServer);
    void onDisconnect(BLEServer* pServer);
};

struct ID{
  std::string id[10];
};

struct BLE_Device_Properties
{
    int RSSI;
    String Adress;
};


// int count=0;

class MyCallbacks: public BLECharacteristicCallbacks 
{
    void onWrite(BLECharacteristic *pCharacteristic);
};


class BLE: public MyServerCallbacks, public MyCallbacks 
{
    public:
        struct ID employee;
        uint32_t scan_duration;
        int number_of_connected_devices;
        BLEServer *pServer = BLEDevice::createServer();
        BLEService *pService = pServer->createService(SERVICE_UUID);
        BLECharacteristic *pCharacteristic;
        BLEScan *scan = BLEDevice::getScan();
        BLEScanResults results;
        BLEAdvertisedDevice devices[MAX_NUMBER_OF_DEVICES];
        BLE_Device_Properties device_properties[MAX_NUMBER_OF_DEVICES];
        
        BLE();
        void init();
        void Get_Scan_Duration(uint32_t t);
        void Start();
        void Start_Advertising();
        void Scan();

};





#endif