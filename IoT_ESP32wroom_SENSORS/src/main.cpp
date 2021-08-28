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

#define CHARACTERISTIC_TEMP "bacdf072-0828-11ec-9a03-0242ac130003"
#define CHARACTERISTIC_PRES "e67d52b2-0828-11ec-9a03-0242ac130003"
#define CHARACTERISTIC_HUM "01d6a036-0829-11ec-9a03-0242ac130003"

int state = 1;
BLECharacteristic *pCharacteristic;

BLECharacteristic *TempCharacteristic;
BLECharacteristic *PresCharacteristic;
BLECharacteristic *HumCharacteristic;

 class callback: public BLECharacteristicCallbacks
 {
   public:
   void onWrite(BLECharacteristic* pCharacteristic){
     Serial.printf("value changed %d\n", *pCharacteristic->getData());
   }
 };


void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

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
  TempCharacteristic->setValue(temp);
  HumCharacteristic->setValue(hum);
  PresCharacteristic->setValue(pres);
}