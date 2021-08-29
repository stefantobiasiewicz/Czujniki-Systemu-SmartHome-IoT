#include <Arduino.h>

/*
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    updates by chegewara
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#include "EEPROM.h"

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "b1a2837c-082e-11ec-9a03-0242ac130003"

#define CHARACTERISTIC_LED "faaa6e3c-082d-11ec-9a03-0242ac130003"
#define CHARACTERISTIC_LOCATION "4aa4ab02-08c7-11ec-9a03-0242ac130003"

#define EEPROM_SIZE  100

#define LedPin 25 

BLECharacteristic *LedCharacteristic;
BLECharacteristic *LocationCharacteristic;

const int eeprom_adres = 0; 

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

  void onRead(BLECharacteristic* pCharacteristic){
    int a = digitalRead(LedPin);
    pCharacteristic->setValue(a);
  }
};

class LocationCallback : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic* pCharacteristic){
    std::string recived = pCharacteristic->getValue();
    int len = recived.length();
    Serial.println(("recived and wirite " + recived).c_str());
    EEPROM.write(eeprom_adres, len);
    for(int i = 0 ; i < len ; i++)
    {
      EEPROM.write(eeprom_adres + 1 + i,recived[i]);
    }
    EEPROM.commit();
  }

  void onRead(BLECharacteristic* pCharacteristic){
    int len = EEPROM.read(eeprom_adres);
    std::string reply;
    for(int i = 0 ; i < len ; i++){
      reply += EEPROM.read(eeprom_adres+1 +i);
    }
    Serial.println(("read from eeprom "+reply).c_str());
    pCharacteristic->setValue(reply);
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  EEPROM.begin(EEPROM_SIZE);
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


  LocationCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_LOCATION,
                                         BLECharacteristic::PROPERTY_READ
                                        | BLECharacteristic::PROPERTY_WRITE
                                       );

  LocationCharacteristic->setCallbacks(new LocationCallback());                                   

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
