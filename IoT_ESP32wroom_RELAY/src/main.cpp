#include <Arduino.h>

/*
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    updates by chegewara
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#define CHARACTERISTIC_LED "faaa6e3c-082d-11ec-9a03-0242ac130003"

#define LedPin 25 

BLECharacteristic *LedCharacteristic;


class MyCallback : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic* pCharacteristic){
    std::string recived = pCharacteristic->getValue();
    if(recived[0] == 1){
      digitalWrite(LedPin,HIGH);
    }
    else if(recived[0] == 0){
      digitalWrite(LedPin,LOW);
    }
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  pinMode(LedPin,OUTPUT);

  BLEDevice::init("SmartHome Relay");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);


  LedCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_LED,
                                         BLECharacteristic::PROPERTY_READ
                                        | BLECharacteristic::PROPERTY_WRITE
                                        | BLECharacteristic::PROPERTY_NOTIFY
                                       );
                                  
  LedCharacteristic->setCallbacks(new MyCallback());

  pService->start();
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined!");
}

void loop() {
  delay(10);
}
