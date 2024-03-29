/*
    Video: https://www.youtube.com/watch?v=oCMOYS71NIU
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
    Ported to Arduino ESP32 by Evandro Copercini

   Create a BLE server that, once we receive a connection, will send periodic notifications.
   The service advertises itself as: 6E400001-B5A3-F393-E0A9-E50E24DCCA9E
   Has a characteristic of: 6E400002-B5A3-F393-E0A9-E50E24DCCA9E - used for receiving data with "WRITE" 
   Has a characteristic of: 6E400003-B5A3-F393-E0A9-E50E24DCCA9E - used to send data with  "NOTIFY"

   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create a BLE Service
   3. Create a BLE Characteristic on the Service
   4. Create a BLE Descriptor on the characteristic
   5. Start the service.
   6. Start advertising.

   In this example rxValue is the data received (only accessible inside that function).
   And txValue is the data to be sent, in this example just a byte incremented every second. 
*/
#include<Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include<EEPROM.h>
#include "ESP32_MQTT.h"

ESP32_MQTT esp_mqtt;
// #include<WiFi.h>

// const char* SSID = "Audhru_2.4";
// const char* Password = "11235813";

int address=0;
BLECharacteristic *pCharacteristic;
bool deviceConnected = false;
float txValue = 0;
const int readPin = 32; // Use GPIO number. See ESP32 board pinouts
const int Gate = 2; // Could be different depending on the dev board. I used the DOIT ESP32 dev board.
String dev[12]={"ac:23:3f:a1:79:3e","ac:23:3f:a1:79:3d","ac:23:3f:a1:7b:07","d8:e0:e1:0a:9e:82","dd:33:0a:01:96:42", "03:e8:0a:05:8a:01" , "40:73:58:6a:f2:a5", "62:09:7a:64:a1:de",
"6e:ce:26:f4:ba:1e",
"6f:ef:c8:15:a6:b0",
"ac:23:3f:a1:79:49",

"ac:23:3f:a1:7a:75"};
// String dev[2] = {"ac:23:3f:a1:79:3d", "ac:23:3f:a1:7b:07"};

bool dev_status[12];
//std::string rxValue; // Could also make this a global var to access it in loop()

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};
struct ID{
  std::string id[10];
};
struct ID employee;
int count=0;
class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
         employee.id[count]=rxValue;
         count++;
        Serial.println("*********");
        Serial.print("Received Value: ");

        for (int i = 0; i < rxValue.length(); i++) {
          EEPROM.put(address,employee);
          Serial.print(rxValue[i]);
        }
        Serial.println();
        Serial.println();
        Serial.println("*********");
      }
    }
};

// void ConnectToWiFi()
// {
 
//   WiFi.mode(WIFI_STA);
//   WiFi.begin(SSID, Password);
//   Serial.print("Connecting to "); Serial.println(SSID);
 
//   while (WiFi.status() != WL_CONNECTED)
//   {
//     Serial.print('.');
//     delay(500);
//   }
 
//   Serial.print(F("Connected. My IP address is: "));
//   Serial.println(WiFi.localIP());
// }

const char *sub_topic = "esp32/dhrubo_sub";
const char *pub_topic = "esp32/dhrubo";

void setup() {
  Serial.begin(9600);

  esp_mqtt.get_WiFi_SSID("Audhru_2.4");
  esp_mqtt.get_WiFi_Password("11235813");
  // esp_mqtt.get_WiFi_Credentials("ESP32", "12345678", "192.168.1.1", "AP");
  delay(2000);
  esp_mqtt.setup_wifi();
  delay(5000);
  esp_mqtt.get_Broker_Address("broker.hivemq.com");
  esp_mqtt.get_Broker_Port(1883);
  esp_mqtt.set_Server_And_Callback();

  esp_mqtt.get_Sub_Topic(sub_topic);
  esp_mqtt.Subscribe(sub_topic);
  pinMode(Gate, OUTPUT);

  // Create the BLE Device
  BLEDevice::init("Uttsha's BLE"); // Give it a name

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_TX,
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
                      
  pCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID_RX,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

void loop() {
  if(!esp_mqtt.is_MQTT_Connected())
  {
    esp_mqtt.MQTT_Reconnect();
  }
  esp_mqtt.loop();
  esp_mqtt.Publish(pub_topic, "Hey Dhrubo");

  BLEScan *scan = BLEDevice::getScan();
  scan->setActiveScan(true);
  BLEScanResults results = scan->start(1);
  int best = -100;
  for (int i = 0; i < results.getCount(); i++) 
  {
    BLEAdvertisedDevice device = results.getDevice(i);
     int rssi = device.getRSSI();
    // Serial.println(rssi);
    // delay(500);
    String nam=device.getAddress().toString().c_str();
    // Serial.println(nam);
    delay(1000); 
    for(int i = 0; i < 3; i ++)
    {
      if(nam == dev[i])
      { 
        // Serial.println(nam);
        dev_status[i] = 1; 
      }
    }

  }
   Serial.println("New Status");
  for(int i = 0; i < 3; i++)
    {
      // Serial.println(dev_status[i]);
      if(dev_status[i])
      {
        Serial.println(dev[i]+" is present");
        dev_status[i] = 0;
      }
      else
      {
        Serial.println(dev[i]+" is absent");
        
      }
    }
  delay(2000);

  if (deviceConnected) {
    // Fabricate some arbitrary junk for now...
    txValue = 10 / 3.456; // This could be an actual sensor reading!

    // Let's convert the value to a char array:
    char txString[8]; // make sure this is big enuffz
    dtostrf(txValue, 1, 2, txString); // float_val, min_width, digits_after_decimal, char_buffer
    
//    pCharacteristic->setValue(&txValue, 1); // To send the integer value
//    pCharacteristic->setValue("Hello!"); // Sending a test message
    pCharacteristic->setValue(txString);
    
    pCharacteristic->notify();} // Send the value to the app!
    // Serial.print("*** Sent Value: ");
    // Serial.print(txString);
    // Serial.println(" ***");

    // You can add the rxValue checks down here instead
    // if you set "rxValue" as a global var at the top!
    // Note you will have to delete "std::string" declaration
    // of "rxValue" in the callback function.
//    if (rxValue.find("A") != -1) { 
//      Serial.println("Turning ON!");
//      digitalWrite(LED, HIGH);
//    }
//    else if (rxValue.find("B") != -1) {
//      Serial.println("Turning OFF!");
//      digitalWrite(LED, LOW);
   
  // delay(1000);
}