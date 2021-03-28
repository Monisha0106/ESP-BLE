#include "BLE.h"
int address ;
bool deviceConnected;
float txValue;
const int readPin = 32; // Use GPIO number. See ESP32 board pinouts
//std::string rxValue; // Could also make this a global var to access it in loop()

void MyServerCallbacks::onConnect(BLEServer* pServer) 
{
    deviceConnected = true;
};

void MyServerCallbacks::onDisconnect(BLEServer* pServer) 
{
    deviceConnected = false;
}

void MyCallbacks::onWrite(BLECharacteristic *pCharacteristic) 
{
    std::string rxValue = pCharacteristic->getValue();

//   if (rxValue.length() > 0) {
//      employee.id[count]=rxValue;
//      count++;
//     Serial.println("*********");
//     Serial.print("Received Value: ");

//     for (int i = 0; i < rxValue.length(); i++) {
//       EEPROM.put(address,employee);
//       Serial.print(rxValue[i]);
//     }
//     Serial.println();
//     Serial.println();
//     Serial.println("*********");
//   }
}


BLE::BLE()
{

}

void BLE::init()
{
    scan_duration = 1;
    BLEDevice::init("Uttsha's BLE"); // Give it a name
    pServer->setCallbacks(new MyServerCallbacks());
    
    // Create a BLE Characteristic
    pCharacteristic = pService->createCharacteristic(
                CHARACTERISTIC_UUID_TX,
                BLECharacteristic::PROPERTY_NOTIFY
            );
                
    pCharacteristic->addDescriptor(new BLE2902());

    pCharacteristic = pService->createCharacteristic(
                                    CHARACTERISTIC_UUID_RX,
                                    BLECharacteristic::PROPERTY_WRITE
                                );

    pCharacteristic->setCallbacks(new MyCallbacks());
}

void BLE::Start()
{
    pService->start();
}

void BLE::Start_Advertising()
{
    pServer->getAdvertising()->start();
}

void BLE::Scan()
{
    scan->setActiveScan(true);
    results = scan->start(scan_duration);
    number_of_connected_devices = results.getCount();
    for(int i = 0; i < number_of_connected_devices; i++)
    {
        devices[i] = results.getDevice(i);
        device_properties[i].RSSI = devices[i].getRSSI();
        device_properties[i].Adress = devices[i].getAddress().toString().c_str();
    }
}