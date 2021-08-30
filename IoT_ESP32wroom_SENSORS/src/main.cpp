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
#include <iostream>
#include <string>  

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914e"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#define CHARACTERISTIC_TEMP "bacdf072-0828-11ec-9a03-0242ac130003"
#define CHARACTERISTIC_PRES "e67d52b2-0828-11ec-9a03-0242ac130003"
#define CHARACTERISTIC_HUM "01d6a036-0829-11ec-9a03-0242ac130003"

#define CHARACTERISTIC_LOCATION "4aa4ab02-08c7-11ec-9a03-0242ac130003"


#define EEPROM_SIZE  100

int state = 1;
BLECharacteristic *pCharacteristic;

BLECharacteristic *TempCharacteristic;
BLECharacteristic *PresCharacteristic;
BLECharacteristic *HumCharacteristic;

BLECharacteristic *LocationCharacteristic;

class LocationCallback : public BLECharacteristicCallbacks
{
  const int eeprom_adres = 0; 

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

  BLEDevice::init("Stefan wzywa");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);


  TempCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_TEMP,
                                         BLECharacteristic::PROPERTY_READ
                                        | BLECharacteristic::PROPERTY_NOTIFY
                                       );

  PresCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_PRES,
                                         BLECharacteristic::PROPERTY_READ
                                        | BLECharacteristic::PROPERTY_NOTIFY
                                       );

  HumCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_HUM,
                                         BLECharacteristic::PROPERTY_READ
                                        | BLECharacteristic::PROPERTY_NOTIFY
                                       );                                    

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
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void loop() {

  // raz na sekunde odczytać z czujnika
  delay(1000);
  float temp = (rand() % 200) / 10;
  float hum = (rand() % 100) / 10;
  float pres = (rand() % 2000) / 10;
  TempCharacteristic->setValue(String(temp).c_str());
  HumCharacteristic->setValue(String(hum).c_str());
  PresCharacteristic->setValue(String(pres).c_str());
}